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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef CONSTANTTRANSLATIONS_PATIENT_H
#define CONSTANTTRANSLATIONS_PATIENT_H

#include <qglobal.h>

namespace Trans {
namespace Constants {

// Patient related
const char* const PATIENT_NAME                = QT_TRANSLATE_NOOP("tkConstants", "Patient name");
const char* const PATIENT_BIRTHNAME           = QT_TRANSLATE_NOOP("tkConstants", "Patient birthname");
const char* const PATIENT_USUALNAME           = QT_TRANSLATE_NOOP("tkConstants", "Patient usual name");
const char* const PATIENT_OTHERNAMES          = QT_TRANSLATE_NOOP("tkConstants", "Patient other names");
const char* const PATIENT_SECONDNAME          = QT_TRANSLATE_NOOP("tkConstants", "Patient secondname");
const char* const PATIENT_FIRSTNAME           = QT_TRANSLATE_NOOP("tkConstants", "Patient firstname");
const char* const PATIENT_FULLNAME            = QT_TRANSLATE_NOOP("tkConstants", "Patient full name");

const char* const BIRTHNAME                   = QT_TRANSLATE_NOOP("tkConstants", "Birthname");
const char* const USUALNAME                   = QT_TRANSLATE_NOOP("tkConstants", "Usual name");
const char* const OTHERNAMES                  = QT_TRANSLATE_NOOP("tkConstants", "Other names");
const char* const SECONDNAME                  = QT_TRANSLATE_NOOP("tkConstants", "Secondname");
const char* const FIRSTNAME                   = QT_TRANSLATE_NOOP("tkConstants", "Firstname");
const char* const FULLNAME                    = QT_TRANSLATE_NOOP("tkConstants", "Full name");
const char* const NAME                        = QT_TRANSLATE_NOOP("tkConstants", "Name");
const char* const TITLE                       = QT_TRANSLATE_NOOP("tkConstants", "Title");

const char* const CONTACT           = QT_TRANSLATE_NOOP("tkConstants", "Contact");
const char* const FULLCONTACT_HTML  = QT_TRANSLATE_NOOP("tkConstants", "Full contact (html)");
const char* const ADDRESS           = QT_TRANSLATE_NOOP("tkConstants", "Address");
const char* const FULLADDRESS       = QT_TRANSLATE_NOOP("tkConstants", "Full address");
const char* const FULLADDRESS_HTML  = QT_TRANSLATE_NOOP("tkConstants", "Full address (html)");
const char* const STREET            = QT_TRANSLATE_NOOP("tkConstants", "Street");
const char* const CITY              = QT_TRANSLATE_NOOP("tkConstants", "City");
const char* const ZIPCODE           = QT_TRANSLATE_NOOP("tkConstants", "Zipcode");
const char* const COUNTRY           = QT_TRANSLATE_NOOP("tkConstants", "Country");
const char* const STATE             = QT_TRANSLATE_NOOP("tkConstants", "State/Province");
const char* const EMAILS            = QT_TRANSLATE_NOOP("tkConstants", "E-Mails");
const char* const EMAIL             = QT_TRANSLATE_NOOP("tkConstants", "E-Mail");
const char* const TELS              = QT_TRANSLATE_NOOP("tkConstants", "Tels");
const char* const MOBILEPHONE       = QT_TRANSLATE_NOOP("tkConstants", "Mobile phone");

const char* const DATE_OF_BIRTH               = QT_TRANSLATE_NOOP("tkConstants", "Date of birth");
const char* const DATE_OF_BIRTH_1             = QT_TRANSLATE_NOOP("tkConstants", "Date of birth: %1");
const char* const DATE_OF_DEATH               = QT_TRANSLATE_NOOP("tkConstants", "Date of death");
const char* const AGE                         = QT_TRANSLATE_NOOP("tkConstants", "Age");
const char* const AGE_1                       = QT_TRANSLATE_NOOP("tkConstants", "Age: %1");
const char* const AGE_IN_YEARS                = QT_TRANSLATE_NOOP("tkConstants", "Age in years");
const char* const AGE_1_DOB_2                 = QT_TRANSLATE_NOOP("tkConstants", "Age: %1; DOB: %2");

const char* const WEIGHT                      = QT_TRANSLATE_NOOP("tkConstants", "Weight");
const char* const HEIGHT                      = QT_TRANSLATE_NOOP("tkConstants", "Height");
const char* const CREATININ_CLEARANCE         = QT_TRANSLATE_NOOP("tkConstants", "Creatinin clerance");

const char* const UNITS                       = QT_TRANSLATE_NOOP("tkConstants", "Units");
const char* const METRICS                     = QT_TRANSLATE_NOOP("tkConstants", "Metrics");
const char* const BIOLOGY                     = QT_TRANSLATE_NOOP("tkConstants", "Biology");

const char* const PATIENT                     = QT_TRANSLATE_NOOP("tkConstants", "Patient");
const char* const PATIENTS                    = QT_TRANSLATE_NOOP("tkConstants", "Patients");
const char* const PATIENT_INFORMATION         = QT_TRANSLATE_NOOP("tkConstants", "Patient information");
const char* const PATIENT_DATABASE            = QT_TRANSLATE_NOOP("tkConstants", "Patient database");
const char* const CLEAR_PATIENT_INFOS         = QT_TRANSLATE_NOOP("tkConstants", "Clear patient's information");
const char* const TOGGLE_PATIENT_INFORMATION  = QT_TRANSLATE_NOOP("tkConstants", "Toggle patient's information");

const char* const SOCIAL_NUMBER               = QT_TRANSLATE_NOOP("tkConstants", "Social number");
const char* const SOCIAL_NUMBERS               = QT_TRANSLATE_NOOP("tkConstants", "Social number (all)");
const char* const SOCIAL_NUMBER_OWNER_NAME    = QT_TRANSLATE_NOOP("tkConstants", "Social number (owner name)");
const char* const SOCIAL_NUMBER_OWNER_ADDRESS = QT_TRANSLATE_NOOP("tkConstants", "Social number (owner address)");

const char* const PATIENT_OVERVIEW            = QT_TRANSLATE_NOOP("tkConstants", "Patient overview");

const char* const LONG_TERM_THERAPY  = QT_TRANSLATE_NOOP("tkConstants", "Long term therapy");

} // end Constants
} // end TranslationUtils

#endif // TKCONSTANTTRANSLATIONS_H
