/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#ifndef CONSTANTTRANSLATIONS_EDITOR_H
#define CONSTANTTRANSLATIONS_EDITOR_H

#include <qglobal.h>

namespace Trans {
namespace Constants {

// ShortCut keys
/*: Text Align Left shortcut key */
const char* const K_ALIGNLEFT                = QT_TRANSLATE_NOOP("tkConstants", "Ctrl+L");

/*: Text Align Right shortcut key */
const char* const K_ALIGNRIGHT               = QT_TRANSLATE_NOOP("tkConstants", "Ctrl+R");

/*: Text Align Center shortcut key */
const char* const K_ALIGNCENTER              = QT_TRANSLATE_NOOP("tkConstants", "Ctrl+E");

/*: Text Align Justify shortcut key */
const char* const K_ALIGNJUSTIFY             = QT_TRANSLATE_NOOP("tkConstants", "Ctrl+J");

/*: Text Align Left shortcut key : Mac specific */
const char* const K_ALIGNLEFT_MAC            = QT_TRANSLATE_NOOP("tkConstants", "Ctrl+L");

/*: Text Align Right shortcut key : Mac specific */
const char* const K_ALIGNRIGHT_MAC           = QT_TRANSLATE_NOOP("tkConstants", "Ctrl+R");

/*: Text Align Center shortcut key : Mac specific */
const char* const K_ALIGNCENTER_MAC          = QT_TRANSLATE_NOOP("tkConstants", "Ctrl+E");

/*: Text Align Justify shortcut key : Mac specific */
const char* const K_ALIGNJUSTIFY_MAC         = QT_TRANSLATE_NOOP("tkConstants", "Ctrl+J");

/*: Add table Editor shortcut key */
const char* const K_EDITOR_ADDTABLE         = QT_TRANSLATE_NOOP("tkConstants", "Ctrl+Shift+T");

/*: Add table Editor shortcut key : Mac specific */
const char* const K_EDITOR_ADDTABLE_MAC     = QT_TRANSLATE_NOOP("tkConstants", "");

const char * const EDITOR_TOGGLETOOLBAR_TEXT  = QT_TRANSLATE_NOOP("tkConstants", "Toggle toolbar");
const char * const EDITOR_FILEOPEN_TEXT  = QT_TRANSLATE_NOOP("tkConstants", "Open in editor");
const char * const EDITOR_FILESAVE_TEXT  = QT_TRANSLATE_NOOP("tkConstants", "Save editor");
const char * const EDITORMENU_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "Rich Text Editor");
const char * const EDITOR_ADDDATE_LONG_TEXT  = QT_TRANSLATE_NOOP("tkConstants", "Add current date (long format)");
const char * const EDITOR_ADDDATE_SHORT_TEXT  = QT_TRANSLATE_NOOP("tkConstants", "Add current date (short format)");
const char * const EDITOR_ADDUSERNAME_TEXT  = QT_TRANSLATE_NOOP("tkConstants", "Add user name");
const char * const EDITOR_ADDPATIENTNAME_TEXT  = QT_TRANSLATE_NOOP("tkConstants", "Add patient name");
const char * const EDITOR_ADDTEXTMENU_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "Add text");

// ShortCuts
//: ShortCut for "Print prescription"
const char* const   K_PRINT_PRESCRIPTION = QT_TRANSLATE_NOOP("tkConstants", "Ctrl+Shift+P");

} // end Constants
} // end translationUtils

#endif // CONSTANTTRANSLATIONS_EDITOR_H
