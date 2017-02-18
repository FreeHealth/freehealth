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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef PATIENTS_TRANS_CONSTANTS_H
#define PATIENTS_TRANS_CONSTANTS_H

/**
 * \file constants_trans.h
 * \author Eric MAEKER
 * \version 0.7.6
 * \date 02 Mar 2011
*/

namespace Patients {
namespace Constants {

    const char* const   TRANS_CONTEXT             = "Patients";

    const char* const   SEARCHBYNAME_TEXT         = QT_TRANSLATE_NOOP("Patients", "Search by Name");
    const char* const   SEARCHBYFIRSTNAME_TEXT      = QT_TRANSLATE_NOOP("Patients", "Search by First name");
    const char* const   SEARCHBYNAMEFIRSTNAME_TEXT  = QT_TRANSLATE_NOOP("Patients", "Search by Name and First name");
    const char* const   SEARCHBYDOB_TEXT          = QT_TRANSLATE_NOOP("Patients", "Search by Date Of Birth");

    const char* const   SEARCHBYNAME_TOOLTIP         = QT_TRANSLATE_NOOP("Patients", "Type the name or second name of the patient");
    const char* const   SEARCHBYFIRSTNAME_TOOLTIP      = QT_TRANSLATE_NOOP("Patients", "Type the firstname of the patient");
    const char* const   SEARCHBYNAMEFIRSTNAME_TOOLTIP  = QT_TRANSLATE_NOOP("Patients", "Type the name or second name followed by ; and the firstname");
    const char* const   SEARCHBYDOB_TOOLTIP          = QT_TRANSLATE_NOOP("Patients", "Type the date of birth (yyyy/MM/dd)");

}  // End namespace Patients::Constants
}  // End namespace Patients

#endif // PATIENTS_TRANS_CONSTANTS_H
