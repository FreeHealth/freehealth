/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef CONSTANTTRANSLATIONS_CURRENT_H
#define CONSTANTTRANSLATIONS_CURRENT_H

#include <QCoreApplication>

namespace Trans {
namespace Constants {

// Widely used words
const char* const TYPE                               = QT_TRANSLATE_NOOP("tkConstants", "Type");
const char* const STATUS                             = QT_TRANSLATE_NOOP("tkConstants", "Status");
const char* const ICD10                              = QT_TRANSLATE_NOOP("tkConstants", "ICD10");
const char* const NOW                                = QT_TRANSLATE_NOOP("tkConstants", "now");
const char* const LOW                                = QT_TRANSLATE_NOOP("tkConstants", "low");
const char* const MEDIUM                             = QT_TRANSLATE_NOOP("tkConstants", "medium");
const char* const HIGH                               = QT_TRANSLATE_NOOP("tkConstants", "high");
const char* const MODIFY                             = QT_TRANSLATE_NOOP("tkConstants", "modify");
const char* const SELECTOR                           = QT_TRANSLATE_NOOP("tkConstants", "Selector");
const char* const FORMS                              = QT_TRANSLATE_NOOP("tkConstants", "Forms");
const char* const VIEWER                             = QT_TRANSLATE_NOOP("tkConstants", "Viewer");
const char* const SERVER                             = QT_TRANSLATE_NOOP("tkConstants", "Server");
const char* const PACKAGE                            = QT_TRANSLATE_NOOP("tkConstants", "Package");
const char* const SERVERS                            = QT_TRANSLATE_NOOP("tkConstants", "Servers");
const char* const PACKAGES                           = QT_TRANSLATE_NOOP("tkConstants", "Packages");
const char* const SERVER_S                           = QT_TRANSLATE_NOOP("tkConstants", "Server(s)");
const char* const PACKAGE_S                          = QT_TRANSLATE_NOOP("tkConstants", "Package(s)");
const char* const NAME                               = QT_TRANSLATE_NOOP("tkConstants", "Name");
const char* const TITLE                              = QT_TRANSLATE_NOOP("tkConstants", "Title");
const char* const UNKNOWN_SERVER                     = QT_TRANSLATE_NOOP("tkConstants", "Unknown server");
const char* const INSTALLED                          = QT_TRANSLATE_NOOP("tkConstants", "Installed");
const char* const CURRENTLY_INSTALLED                = QT_TRANSLATE_NOOP("tkConstants", "Currently installed");

// Settings related messages
const char* const SETTINGS_RECOVERED                = QT_TRANSLATE_NOOP("tkConstants", "Settings recovered" );
const char* const LOADING_SETTINGS                  = QT_TRANSLATE_NOOP("tkConstants", "Reading settings file");
const char* const SAVING_SETTINGS                   = QT_TRANSLATE_NOOP("tkConstants", "Settings recovered" );
const char* const CREATING_DEFAULT_SETTINGS_FOR_1   = QT_TRANSLATE_NOOP("tkConstants", "Creating default settings for %1");
const char* const SHOW_DETAILS                      = QT_TRANSLATE_NOOP("tkConstants", "Show details");
const char* const MORE_INFORMATIONS                 = QT_TRANSLATE_NOOP("tkConstants", "More information");
const char* const HIDE_EXTRA_INFORMATIONS           = QT_TRANSLATE_NOOP("tkConstants", "Hide extra information");

// Print related
const char* const HEADER                            = QT_TRANSLATE_NOOP("tkConstants", "Header");
const char* const FOOTER                            = QT_TRANSLATE_NOOP("tkConstants", "Footer");
const char* const WATERMARK                         = QT_TRANSLATE_NOOP("tkConstants", "Watermark");
const char* const PRINTING                          = QT_TRANSLATE_NOOP("tkConstants", "Printing");

} // end Constants
} // end TranslationUtils

#endif // CONSTANTTRANSLATIONS_CURRENT_H
