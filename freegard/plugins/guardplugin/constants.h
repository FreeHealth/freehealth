/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef GUARDCORE_CONSTANTS_H
#define GUARDCORE_CONSTANTS_H

#include <coreplugin/constants_menus.h>

/**
 * \file constants.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 12 Oct 2014
*/

namespace Guard {
namespace Constants {

// Modes
const char * const MODE_GUARDIANS = "Guardians";
const char * const MODE_GUARDLINES = "GuardLines";
const char * const MODE_GUARDTABLES = "GuardTables";

const int P_MODE_GUARDIANS   = Core::Constants::FirstModeGroup + Core::Constants::FirstModeSubGroup;
const int P_MODE_GUARDLINES  = Core::Constants::SecondModeGroup + Core::Constants::FirstModeSubGroup;
const int P_MODE_GUARDTABLES = Core::Constants::ThirdModeGroup + Core::Constants::FirstModeSubGroup;

// Contexts
const char * const C_GUARD_PLUGINS = "Guard";

// Options page sortIndex (per categories)
//const int OPTIONINDEX_MAIN         = 0;
//const int OPTIONINDEX_PRINT        = 1000;

} // Constants
} // Guard

#endif // GUARDCORE_CONSTANTS_H
