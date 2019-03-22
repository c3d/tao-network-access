// *****************************************************************************
// network_access.h                                                Tao3D project
// *****************************************************************************
//
// File description:
//
//     Functions to get data from the network using Qt
//
//
//
//
//
//
//
//
// *****************************************************************************
// This software is licensed under the GNU General Public License v3
// (C) 2012,2019, Christophe de Dinechin <christophe@dinechin.org>
// (C) 2012, Jérôme Forissier <jerome@taodyne.com>
// (C) 2012, Soulisse Baptiste <baptiste.soulisse@taodyne.com>
// *****************************************************************************
// This file is part of Tao3D
//
// Tao3D is free software: you can r redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tao3D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Tao3D, in a file named COPYING.
// If not, see <https://www.gnu.org/licenses/>.
// *****************************************************************************

#include "tree.h"
#include "main.h"
#include "runtime.h"
#include "base.h"


using namespace XL;
Text_p getUrlText(Tree_p self, Text_p url);
Tree_p getUrlData(Context *context, Tree *self,
                  Text_p url, text prefix, text fieldSeps, text recordSeps);
