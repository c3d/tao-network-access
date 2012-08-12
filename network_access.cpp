// ****************************************************************************
//  network_access.cpp                                             Tao project
// ****************************************************************************
// 
//   File Description:
// 
//    Functions to get data from the network over a URL
// 
// 
// 
// 
// 
// 
// 
// 
// ****************************************************************************
//  (C) 2011 Christophe de Dinechin <christophe@taodyne.com>
//  (C) 2011 Taodyne SAS
// ****************************************************************************

#include "network_access.h"
#include "tao/tao_utf8.h"
#include "tao/tao_info.h"
#include "tao/module_api.h"
#include <sstream>
#include <QtNetwork>
#include <map>

using namespace Tao;


// Pointer to Tao functions
const Tao::ModuleApi *tao = NULL;

struct NetworkAccessInfo : Tao::Info
// ----------------------------------------------------------------------------
//   Store information about the network access for a given URL
// ----------------------------------------------------------------------------
{
    NetworkAccessInfo():
        network(NULL), pending() {}
    ~NetworkAccessInfo() { }

public:
    struct RequestReply
    {
        RequestReply() : reply(NULL), result() {}
        ~RequestReply() { reply->deleteLater(); }
        QNetworkReply *         reply;
        text                    result;
    };
    typedef std::map<text, RequestReply> transactions;
    typedef transactions::iterator iterator;

public:
    QNetworkAccessManager       network;
    transactions                pending;
};


static text errorText(QNetworkReply::NetworkError error);


static bool hasLicense()
// ----------------------------------------------------------------------------
//   Check if we have a valid licence for this feature
// ----------------------------------------------------------------------------
{
    static bool result = tao->checkImpressOrLicense("NetworkAccess 1.001");
    return result;
}


text getUrlRawData(Tree_p self, Text_p urlText)
// ----------------------------------------------------------------------------
//   State machine to complete a network request repeatedly
// ----------------------------------------------------------------------------
{
    // Find network access info attachment
    NetworkAccessInfo *info = self->GetInfo<NetworkAccessInfo>();
    if (!info)
    {
        info = new NetworkAccessInfo;
        self->SetInfo<NetworkAccessInfo> (info);
    }

    // Identify the request if there is one
    NetworkAccessInfo::RequestReply &rr = info->pending[urlText->value];
    if (!rr.reply)
    {
        QUrl url(+urlText->value);
        QNetworkRequest req(url);
        rr.reply = info->network.get(req);
    }

    // Analyze the reply
    QNetworkReply *reply = rr.reply;
    QNetworkReply::NetworkError error = reply->error();
    if (error == QNetworkReply::NoError)
    {
        // No error, get data
        if (reply->isFinished())
        {
            QString text(reply->readAll());
            rr.result = +text;
            reply->deleteLater();
            rr.reply = NULL;
        }
    }
    else
    {
        // Report error message
        rr.result = "[Error: " + errorText(error) + "]";

        // Delete the reply so that we try again
        reply->deleteLater();
        rr.reply = NULL;
    } // Error cases

    // Licence check
    hasLicense();
    text result = rr.result;

    IFTRACE(netaccess)
        std::cerr << "URL " << urlText->value << ": " << rr.result << "\n";
    return result;
}


Text_p getUrlText(Tree_p self, Text_p urlText)
// ----------------------------------------------------------------------------
//   Interface to XL runtime
// ----------------------------------------------------------------------------
{
    // Build a text with the result
    text result = getUrlRawData(self, urlText);
    return new Text(result, "\"", "\"", self->Position());
}


text errorText(QNetworkReply::NetworkError error)
// ----------------------------------------------------------------------------
//   Return the error name for a given error code
// ----------------------------------------------------------------------------
{
    switch (error)
    {
    case QNetworkReply::ConnectionRefusedError:
        return "the remote server refused the connection "
            "(the server is not accepting requests)";
    case QNetworkReply::RemoteHostClosedError:
        return "the remote server closed the connection prematurely, "
            "before the entire reply was received and processed";
    case QNetworkReply::HostNotFoundError:
        return "the remote host name was not found (invalid hostname)";
    case QNetworkReply::TimeoutError:
        return "the connection to the remote server timed out";
    case QNetworkReply::OperationCanceledError:
        return "the operation was canceled via calls to abort() "
            "or close() before it was finished.";
    case QNetworkReply::SslHandshakeFailedError:
        return "the SSL/TLS handshake failed and the encrypted channel"
            " could not be established. The sslErrors() signal should"
            " have been emitted.";
    case QNetworkReply::TemporaryNetworkFailureError:
        return "the connection was broken due to disconnection from"
            " the network, however the system has initiated roaming to"
            " another access point. The request should be resubmitted"
            " and will be processed as soon as the connection is"
            " re-established.";
    case QNetworkReply::ProxyConnectionRefusedError:
        return "the connection to the proxy server was refused"
            " (the proxy server is not accepting requests)";
    case QNetworkReply::ProxyConnectionClosedError:
        return "the proxy server closed the connection prematurely,"
            " before the entire reply was received and processed";
    case QNetworkReply::ProxyNotFoundError:
        return "the proxy host name was not found"
            " (invalid proxy hostname)";
    case QNetworkReply::ProxyTimeoutError:
        return "the connection to the proxy timed out or"
            " the proxy did not reply in time to the request sent";
    case QNetworkReply::ProxyAuthenticationRequiredError:
        return "the proxy requires authentication in order to honour"
            " the request but did not accept any credentials offered"
            " (if any)";
    case QNetworkReply::ContentAccessDenied:
        return "the access to the remote content was denied"
            " (similar to HTTP error 401)";
    case QNetworkReply::ContentOperationNotPermittedError:
        return "the operation requested on the remote content"
            " is not permitted";
    case QNetworkReply::ContentNotFoundError:
        return "the remote content was not found at the server"
            " (similar to HTTP error 404)";
    case QNetworkReply::AuthenticationRequiredError:
        return "the remote server requires authentication to serve"
            " the content but the credentials provided were not accepted"
            " (if any)";
    case QNetworkReply::ContentReSendError:
        return "the request needed to be sent again, but this failed "
            " for example because the upload data could not be read"
            " a second time.";
    case QNetworkReply::ProtocolUnknownError:
        return "the Network Access API cannot honor the request"
            " because the protocol is not known";
    case QNetworkReply::ProtocolInvalidOperationError:
        return "the requested operation is invalid for this protocol";
    case QNetworkReply::UnknownNetworkError:
        return "an unknown network-related error was detected";
    case QNetworkReply::UnknownProxyError:
        return "an unknown proxy-related error was detected";
    case QNetworkReply::UnknownContentError:
        return "an unknown error related to the remote content"
            " was detected";
    case QNetworkReply::ProtocolFailure:
        return "a breakdown in protocol was detected"
            " (parsing error, invalid or unexpected responses, etc.)";
    default:
        return "unknown error";
    }
}


struct GetUrlDataInfo : XL::Info
// ----------------------------------------------------------------------------
//   Information about the data that was loaded
// ----------------------------------------------------------------------------
{
    GetUrlDataInfo() {}
    text        source;
};


Tree_p getUrlData(Context *context, Tree *self,
                  Text_p url, text prefix, text fieldSeps, text recordSeps)
// ----------------------------------------------------------------------------
//    Load a comma-separated or tab-separated file from disk
// ----------------------------------------------------------------------------
{
    text rawData = getUrlRawData(self, url);

    // Check if the file has already been loaded somehwere.
    // If so, return the loaded data
    GetUrlDataInfo *info = self->GetInfo<GetUrlDataInfo>();
    if (!info)
    {
        // Create cache
        info = new GetUrlDataInfo();
        self->SetInfo<GetUrlDataInfo> (info);
    }

    // Record what state we were in
    bool cached = info->source == rawData;
    info->source = rawData;

    std::istringstream input(rawData);
    return xl_load_data(context, self, input, cached,
                        prefix, fieldSeps, recordSeps);
}



// ============================================================================
// 
//    Module glue
// 
// ============================================================================

int module_init(const Tao::ModuleApi *api, const Tao::ModuleInfo *)
// ----------------------------------------------------------------------------
//   Initialize the Tao module
// ----------------------------------------------------------------------------
{
    XL_INIT_TRACES();
    tao = api;
    return 0;
}


int module_exit()
// ----------------------------------------------------------------------------
//   Uninitialize the Tao module
// ----------------------------------------------------------------------------
{
    return 0;
}



// Define traces for this modules
XL_DEFINE_TRACES

