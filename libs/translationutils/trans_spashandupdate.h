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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef CONSTANTTRANSLATIONS_SPLASHANDUPADTE_H
#define CONSTANTTRANSLATIONS_SPLASHANDUPADTE_H

#include <qglobal.h>

namespace Trans {
namespace Constants {

enum CheckUpdateRepresentation {  // Never change the order of this enum
    CheckUpdate_AtStartup = 0,
    CheckUpdate_EachWeeks,
    CheckUpdate_EachMonth,
    CheckUpdate_EachQuarters,
    CheckUpdate_Never
};

// Splashscreen messages
const char* const USING_LIBRARY_1                   = QT_TRANSLATE_NOOP("tkConstants", "Using library: %1");
const char* const STARTING_APPLICATION_AT_1         = QT_TRANSLATE_NOOP("tkConstants", "Starting application at %1");
const char* const INITIALIZING_TRANSLATIONS         = QT_TRANSLATE_NOOP("tkConstants", "Initializing translations...");
const char* const INITIALIZING_LIBRARIES            = QT_TRANSLATE_NOOP("tkConstants", "Initializing libraries..." );
const char* const NEED_LICENSE_AGREEMENT            = QT_TRANSLATE_NOOP("tkConstants", "Needed license agreement...");
const char* const CHECKING_DIRECTORY_STRUCTURE      = QT_TRANSLATE_NOOP("tkConstants", "Checking directory structure...");
const char* const INITIALIZING_DEFAULTS_PARAMS      = QT_TRANSLATE_NOOP("tkConstants", "Initializing default parameters..." );
const char* const INITIALIZING_PLUGINS              = QT_TRANSLATE_NOOP("tkConstants", "Initializing plugins...");
const char* const FIRST_TIME_RUNNING                = QT_TRANSLATE_NOOP("tkConstants", "First time running");
const char* const FEATURE_NOT_IMPLEMENTED           = QT_TRANSLATE_NOOP("tkConstants", "This feature is not yet implemented.");
const char* const RAISING_APPLICATION               = QT_TRANSLATE_NOOP("tkConstants", "Raising application...");
const char* const INITIALIZATING_DATABASES          = QT_TRANSLATE_NOOP("tkConstants", "Initializing databases...");

// Update
const char* const CHECKUPDATE                       = QT_TRANSLATE_NOOP("tkConstants", "Check for update");
const char* const CHECKING_UPDATES                  = QT_TRANSLATE_NOOP("tkConstants", "Checking for updates...");
const char* const AT_STARTUP                        = QT_TRANSLATE_NOOP("tkConstants", "At startup");
const char* const EACH_DAY                          = QT_TRANSLATE_NOOP("tkConstants", "daily");
const char* const EACH_WEEK                         = QT_TRANSLATE_NOOP("tkConstants", "weekly");
const char* const EACH_MONTH                        = QT_TRANSLATE_NOOP("tkConstants", "monthly");
const char* const EACH_QUARTER                      = QT_TRANSLATE_NOOP("tkConstants", "quarterly");
const char* const NEVER_AUTOMATICALLY               = QT_TRANSLATE_NOOP("tkConstants", "never automatically");

// Bug Reports
const char* const BUG_REPORT                        = QT_TRANSLATE_NOOP("tkConstants", "&Report a problem...");

// Sender and Updater
const char* const START_MESSAGE_SENDING              = QT_TRANSLATE_NOOP("tkConstants", "Starting to send message.");
const char* const MESSAGE_SENT_OK                    = QT_TRANSLATE_NOOP("tkConstants", "Message sent without error.");
const char* const MESSAGE_NOT_SENT                   = QT_TRANSLATE_NOOP("tkConstants", "An error occured, message was not sent.");
const char* const CHECKING_UPDATE_FROM_1             = QT_TRANSLATE_NOOP("tkConstants", "Checking for updates from %1");
const char* const VERSION_UPTODATE                   = QT_TRANSLATE_NOOP("tkConstants", "Version up to date");
const char* const CURRENT_VERSION                    = QT_TRANSLATE_NOOP("tkConstants", "Current version:");
const char* const LATEST_VERSION                     = QT_TRANSLATE_NOOP("tkConstants", "Latest version:");
const char* const UPDATE_AVAILABLE                   = QT_TRANSLATE_NOOP("tkConstants", "Update available");
const char* const UPDATE                             = QT_TRANSLATE_NOOP("tkConstants", "Update");
const char* const LAST_UPDATE_CHECKING               = QT_TRANSLATE_NOOP("tkConstants", "Last update checking");
const char* const INSTALL                            = QT_TRANSLATE_NOOP("tkConstants", "Install");
const char* const RECOMMENDED_UPDATE_FREQUENCY       = QT_TRANSLATE_NOOP("tkConstants", "Recommended update frequency");

} // end Constants
} // end TranslationUtils

#endif // CONSTANTTRANSLATIONS_SPLASHANDUPADTE_H
