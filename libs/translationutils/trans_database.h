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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef CONSTANTTRANSLATIONS_DATABASE_H
#define CONSTANTTRANSLATIONS_DATABASE_H

#include <qglobal.h>

namespace Trans {
namespace Constants {

// Database related
const char *const DATABASE_DRIVER_1_NOT_AVAILABLE           = QT_TRANSLATE_NOOP("tkConstants", "The %1 driver is not available.");
const char *const DATABASE_DRIVER_1_NOT_AVAILABLE_DETAIL    = QT_TRANSLATE_NOOP("tkConstants", "The %1 database driver is not installed or can not be found. Please report this error to the developer.");
const char *const DATABASE_1_SCHEMA_ERROR                   = QT_TRANSLATE_NOOP("tkConstants", "Database %1: Schema error.");
const char* const SET_PASSWORD                              = QT_TRANSLATE_NOOP("tkConstants", "Set password");
const char *const CHANGE_PASSWORD                           = QT_TRANSLATE_NOOP("tkConstants", "Change password");
const char *const LOGIN                                     = QT_TRANSLATE_NOOP("tkConstants", "Login");
const char *const PASSWORD                                  = QT_TRANSLATE_NOOP("tkConstants", "Password");

const char* const CONNECTED_TO_DATABASE_1_DRIVER_2          = QT_TRANSLATE_NOOP("tkConstants", "Connected to database '%1' with driver '%2'");
const char* const TRYING_TO_CREATE_1_PLACE_2                = QT_TRANSLATE_NOOP("tkConstants", "Trying to create %1 \nPlace: %2");
const char* const UNABLE_TO_OPEN_DATABASE_1_ERROR_2         = QT_TRANSLATE_NOOP("tkConstants", "Unable to open database %1 - Error: %2");
const char* const UNABLE_TO_SAVE_DATA_IN_DATABASE_1         = QT_TRANSLATE_NOOP("tkConstants", "Unable to save data in database %1.");
const char* const DATABASE_1_CORRECTLY_CREATED              = QT_TRANSLATE_NOOP("tkConstants", "Database %1 successfully created");
const char* const DATABASE_USER_1_CORRECTLY_CREATED         = QT_TRANSLATE_NOOP("tkConstants", "User %1 successfully created");
const char* const DATABASE_1_CANNOT_BE_CREATED_ERROR_2      = QT_TRANSLATE_NOOP("tkConstants", "Database %1 can not be created. Error: %2.");
const char* const SEARCHING_DATABASE_1_IN_PATH_2            = QT_TRANSLATE_NOOP("tkConstants", "Searching database %1 in path %2");

const char* const DRUGS_DATABASE_INFORMATION          = QT_TRANSLATE_NOOP("tkConstants", "Drugs database information");
const char* const DOSAGES_DATABASE_INFORMATION        = QT_TRANSLATE_NOOP("tkConstants", "Protocols database information");
const char* const INTERACTIONS_DATABASE_INFORMATION   = QT_TRANSLATE_NOOP("tkConstants", "Interactions database information");
const char* const PMH_DATABASE_INFORMATION            = QT_TRANSLATE_NOOP("tkConstants", "Past medical history database information");
const char* const FORM_DATABASE_INFORMATION           = QT_TRANSLATE_NOOP("tkConstants", "Forms and episodes database information");
const char* const USER_DATABASE_INFORMATION           = QT_TRANSLATE_NOOP("tkConstants", "Users database information");

} // end Constants
} // end TranslationUtils

#endif // CONSTANTTRANSLATIONS_DATABASE_H
