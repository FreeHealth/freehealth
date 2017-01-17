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
 ***************************************************************************/
#ifndef CONSTANTTRANSLATIONS_MSGERROR_H
#define CONSTANTTRANSLATIONS_MSGERROR_H

#include <qglobal.h>

namespace Trans {
namespace Constants {

// Messages and errors
const char* const INFORMATION                        = QT_TRANSLATE_NOOP("tkConstants", "Information");
const char* const CANCEL                             = QT_TRANSLATE_NOOP("tkConstants", "Cancel");
const char* const CLEAR                              = QT_TRANSLATE_NOOP("tkConstants", "Clear");
const char* const VERSION                            = QT_TRANSLATE_NOOP("tkConstants", "Version");
const char* const DESCRIPTION                        = QT_TRANSLATE_NOOP("tkConstants", "Description");
const char* const AUTHOR                             = QT_TRANSLATE_NOOP("tkConstants", "Author");
const char* const VENDOR                             = QT_TRANSLATE_NOOP("tkConstants", "Vendor");
const char* const PROVIDER                           = QT_TRANSLATE_NOOP("tkConstants", "Provider");
const char* const NOT_DEFINED                        = QT_TRANSLATE_NOOP("tkConstants", "Not defined");
const char* const UNKNOWN                            = QT_TRANSLATE_NOOP("tkConstants", "Unknown");
const char* const READY                              = QT_TRANSLATE_NOOP("tkConstants", "Ready");
const char* const AVAILABLE                          = QT_TRANSLATE_NOOP("tkConstants", "Available");
const char* const UNAVAILABLE                        = QT_TRANSLATE_NOOP("tkConstants", "Unavailable");
const char* const MESSAGES                           = QT_TRANSLATE_NOOP("tkConstants", "Messages");
const char* const LOGGED_MESSAGES                    = QT_TRANSLATE_NOOP("tkConstants", "Logged Messages");
const char* const ERRORS                             = QT_TRANSLATE_NOOP("tkConstants", "Errors");
const char* const ERROR                              = QT_TRANSLATE_NOOP("tkConstants", "Error");
const char* const LOGGED_ERRORS                      = QT_TRANSLATE_NOOP("tkConstants", "Logged Errors");
const char* const INFORMATIVE_MESSAGE_1              = QT_TRANSLATE_NOOP("tkConstants", "Informative message: %1");
const char* const STARTING_TASK_1                    = QT_TRANSLATE_NOOP("tkConstants", "Starting task: %1");
const char* const POST_TO_1                          = QT_TRANSLATE_NOOP("tkConstants", "posting message to: %1");
const char* const ERROR_1_OCCURED_WHILE_2            = QT_TRANSLATE_NOOP("tkConstants", "Error %1 occured while %2");
const char* const ERROR_1_LINE_2_COLUMN_3            = QT_TRANSLATE_NOOP("tkConstants", "Error %1: line %2, column %3");
const char* const CAN_NOT_CREATE_1_ALREADY_EXISTS    = QT_TRANSLATE_NOOP("tkConstants", "Can not create %1: it already exists");
const char* const CAN_NOT_RETURN_1_DOESNOT_EXISTS    = QT_TRANSLATE_NOOP("tkConstants", "Can not return %1: it does not exist");
const char* const CAN_NOT_CREATE_1_NOPARENT          = QT_TRANSLATE_NOOP("tkConstants", "Can not create %1: no parent found");
const char* const _1_ISNOT_AVAILABLE                 = QT_TRANSLATE_NOOP("tkConstants", "%1 is not avaible");
const char* const _1_ISNOT_AVAILABLE_CANNOTBE_CREATED = QT_TRANSLATE_NOOP("tkConstants", "%1 is not avaible and can not be created");
const char* const _1_ARENOT_AVAILABLE                = QT_TRANSLATE_NOOP("tkConstants", "%1 are not avaible");
const char* const SETTING_1_PATH_TO_2                = QT_TRANSLATE_NOOP("tkConstants", "Setting %1 path to %2");
const char* const ERROR_1_FROM_DATABASE_2            = QT_TRANSLATE_NOOP("tkConstants", "Error %1 from database %2");
const char* const NO_OBJECT_TO_TRANSLATE             = QT_TRANSLATE_NOOP("tkConstants", "No object to translate");
const char* const NO_1_FOUND                         = QT_TRANSLATE_NOOP("tkConstants", "No %1 found.");
const char* const PLEASE_DESCRIBE_PROBLEM            = QT_TRANSLATE_NOOP("tkConstants", "Please describe your problem.");
const char* const YOUR_MAIL_ADDRESS                  = QT_TRANSLATE_NOOP("tkConstants", "Your email.");
const char* const YES                                = QT_TRANSLATE_NOOP("tkConstants", "Yes");
const char* const NO                                 = QT_TRANSLATE_NOOP("tkConstants", "No");
const char* const GENERAL                            = QT_TRANSLATE_NOOP("tkConstants", "&General");
const char* const GENERAL_PREFERENCES                = QT_TRANSLATE_NOOP("tkConstants", "General preferences");
const char* const USER_PREFERENCES                   = QT_TRANSLATE_NOOP("tkConstants", "User preferences");
const char* const APPLICATION_FAILURE                = QT_TRANSLATE_NOOP("tkConstants", "Fatal error: Application failure. Please restart application.");
const char* const INTERNET_CONNECTION_PREFERENCES    = QT_TRANSLATE_NOOP("tkConstants", "Internet connection preferences");
const char* const PROXY                              = QT_TRANSLATE_NOOP("tkConstants", "Proxy");

const char* const LAST_CHECK                         = QT_TRANSLATE_NOOP("tkConstants", "Last check");
const char* const CONNECTED                          = QT_TRANSLATE_NOOP("tkConstants", "Connected");
const char* const NOT_CONNECTED                      = QT_TRANSLATE_NOOP("tkConstants", "Not connected");

const char* const NO_ADDRESS_RECORDED                = QT_TRANSLATE_NOOP("tkConstants", "No address recorded");

} // end Constants
} // end TranslationUtils

#endif // CONSTANTTRANSLATIONS_MSGERROR_H
