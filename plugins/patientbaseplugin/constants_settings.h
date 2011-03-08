/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef PATIENTS_SETTINGS_CONSTANTS_H
#define PATIENTS_SETTINGS_CONSTANTS_H

namespace Patients {
namespace Constants {

    const char * const  S_GROUP                  = "Patients";                              /*!< \brief Key for settings. */
    const char * const  S_SELECTOR_FIELDSTOSHOW  = "Patients/Selector/Fields";              /*!< \brief Key for settings. */
    const char * const  S_SELECTOR_SHOWVIRTUALPATIENTS  = "Patients/Selector/ShowVirtual";              /*!< \brief Key for settings. */
    const char * const  S_SELECTOR_USEGENDERCOLORS  = "Patients/Selector/UseGenderColors";
    const char * const  S_SEARCHMETHOD = "Patients/Selector/SearchMethod";
    const char * const  S_PATIENTBARCOLOR = "Patients/Bar/Color";

}  // End namespace Patients::Constants
}  // End namespace Patients

#endif // PATIENTS_SETTINGS_CONSTANTS_H
