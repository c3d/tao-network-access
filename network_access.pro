# ******************************************************************************
#  network_access.pro                                               Tao project
# ******************************************************************************
# File Description:
# Qt build file for the Hello World module
# ******************************************************************************
# This software is property of Taodyne SAS - Confidential
# Ce logiciel est la propriété de Taodyne SAS - Confidentiel
# (C) 2010 Jerome Forissier <jerome@taodyne.com>
# (C) 2010 Taodyne SAS
# ******************************************************************************

MODINSTDIR = network_access

include(../modules.pri)


INCLUDEPATH += $${TAOTOPSRC}/tao/include/tao/
HEADERS = network_access.h

SOURCES = network_access.cpp

TBL_SOURCES  = network_access.tbl

OTHER_FILES = network_access.xl network_access.tbl traces.tbl
QT          += core \
               gui \
               opengl \
               network

INSTALLS    += thismod_icon
