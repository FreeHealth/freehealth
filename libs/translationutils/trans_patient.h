/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developer : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef CONSTANTTRANSLATIONS_PATIENT_H
#define CONSTANTTRANSLATIONS_PATIENT_H

#include <QCoreApplication>

namespace Trans {
namespace Constants {

// Patient related
const char* const PATIENT_NAME                = QT_TRANSLATE_NOOP("tkConstants", "Patient Name");
const char* const PATIENT                     = QT_TRANSLATE_NOOP("tkConstants", "Patient");
const char* const PATIENTS                    = QT_TRANSLATE_NOOP("tkConstants", "Patients");
const char* const PATIENT_INFORMATION         = QT_TRANSLATE_NOOP("tkConstants", "Patient information");
const char* const PATIENT_DATABASE            = QT_TRANSLATE_NOOP("tkConstants", "Patient database");
const char* const CLEAR_PATIENT_INFOS         = QT_TRANSLATE_NOOP("tkConstants", "Clear patient's information");
const char* const TOGGLE_PATIENT_INFORMATIONS = QT_TRANSLATE_NOOP("tkConstants", "Toggle patient's information");

} // end Constants
} // end TranslationUtils

#endif // TKCONSTANTTRANSLATIONS_H
