/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developpers:                                                     *
 *       Eric Maeker <e>                             *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef HPRIM_INTEGRATOR_CONSTANTS_H
#define HPRIM_INTEGRATOR_CONSTANTS_H

namespace Tools {
namespace Constants {

const char *const S_DEFAULT_FILE_ENCODING           = "Tools/HprimIntegrator/DefaultFileEncoding";
const char *const S_PATH_TO_SCAN                    = "Tools/HprimIntegrator/PathToScan";
const char *const S_FORMITEM_UUIDS                  = "Tools/HprimIntegrator/FormItemUuids";
const char *const S_FILE_MANAGEMENT                 = "Tools/HprimIntegrator/FileManagement";
const char *const S_FILE_MANAGEMENT_STORING_PATH    = "Tools/HprimIntegrator/FileStoringPath";
const char *const S_ACTIVATION                      = "Tools/HprimIntegrator/Activation";

enum FileManagement {
    RemoveFileDefinitively = 0,
    RemoveFileOneMonthAfterIntegration,
    StoreFileInPath
};

enum ServiceActivation {
    OnlyForFrance = 0,
    Enabled,
    Disabled
};

enum FileEncoding {
    AutoDetect = 0,
    ForceUtf8,
    ForceMacRoman,
    ForceIso8859_1
};

} // namespace Constants
} // namespace Tools

#endif // HPRIM_INTEGRATOR_CONSTANTS_H
