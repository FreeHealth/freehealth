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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developer: Jerome Pinguet <jerome@jerome.cc>                     *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef RECORDIMPORTCONSTANTS_H
#define RECORDIMPORTCONSTANTS_H

#include <qglobal.h>

namespace RecordImport {
namespace Constants {

// Actions
const char * const ACTION_ID = "grPlugins.recordimport";
const char * const MENU_ID = "grGeneral.Plugins";

// Icon
const char * const RECORDIMPORT_ICON = "merge-arrow.png";

// Softwares
const char * const CIELCOMPTA  = "Ciel Compta";
const char * const GESTCAB     = "Gestcab";
const char * const OTHER       = "Other";

// Patient identity
const char * const PATIENT_IDENTITY_USUALNAME   = "Usual name";
const char * const PATIENT_IDENTITY_FIRSTNAME   = "First name";
const char * const PATIENT_IDENTITY_DOB         = "Date of birth";

// Translations
const char* const RECORD_IMPORT = QT_TRANSLATE_NOOP("tkConstants", "&Import records");

} // namespace RecordImport
} // namespace Constants

#endif // RECORDIMPORTCONSTANTS_H
