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
#ifndef CONSTANTTRANSLATIONS_TEAM_H
#define CONSTANTTRANSLATIONS_TEAM_H

#include <qglobal.h>

namespace Trans {
namespace Constants {

// Team functions
const char* const TEAM_CREATOR = QT_TRANSLATE_NOOP("tkConstants", "Creator and Developer");
const char* const TEAM_DEVELOPERS_CONTRIBUTORS = QT_TRANSLATE_NOOP("tkConstants", "Developers and Contributors");
const char* const TEAM_DEBUGGER_ADVISOR = QT_TRANSLATE_NOOP("tkConstants", "Debugger advisor");
const char* const TEAM_DEVELOPER_AND_DEBUGGER = QT_TRANSLATE_NOOP("tkConstants", "Developer and debugger advisor");
const char* const TEAM_PACKAGER = QT_TRANSLATE_NOOP("tkConstants", "Packager");
const char* const TEAM_PACKAGER_FOR_1 = QT_TRANSLATE_NOOP("tkConstants", "Packager for %1");
const char* const TEAM_DRUG_DATABASE_MANAGER = QT_TRANSLATE_NOOP("tkConstants", "Drugs Database Management");
const char* const TEAM_MEDICAL_ADVISORS = QT_TRANSLATE_NOOP("tkConstants", "Medical advisors");
const char* const TEAM_MEDICAL_ADVISOR = QT_TRANSLATE_NOOP("tkConstants", "Medical advisor");
const char* const TEAM_WEBMASTER = QT_TRANSLATE_NOOP("tkConstants", "Webmaster");
const char* const TEAM_TRANSLATORS = QT_TRANSLATE_NOOP("tkConstants", "Translators");

// Build related
const char* const BUILD_INFORMATION = QT_TRANSLATE_NOOP("tkConstants", "Build information");
const char* const BUILD_VERSION_INFORMATION = QT_TRANSLATE_NOOP("tkConstants", "Version information");
const char* const BUILD_DATE_TIME_1_2 = QT_TRANSLATE_NOOP("tkConstants", "Built on %1 at %2");
const char* const BUILD_VERSION_1 = QT_TRANSLATE_NOOP("tkConstants", "Build version: %1");
const char* const BUILD_FULLAPP = QT_TRANSLATE_NOOP("tkConstants", "Full application");
const char* const BUILD_GITAPP = QT_TRANSLATE_NOOP("tkConstants", "GIT application");
const char* const BUILD_DEBUG = QT_TRANSLATE_NOOP("tkConstants", "Built in debug mode");
const char* const BUILD_RELEASE = QT_TRANSLATE_NOOP("tkConstants", "Built in release mode");

} // end Constants
} // end TranslationUtils

#endif // CONSTANTTRANSLATIONS_TEAM_H
