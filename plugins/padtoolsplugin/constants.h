/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developers:  Eric Maeker <eric.maeker@gmail.com>,                *
 *                    Guillaume Denry <guillaume.denry@gmail.com>          *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#ifndef PADTOOLS_CONSTANTS_H
#define PADTOOLS_CONSTANTS_H

#include <QTextCharFormat>
#include <QTextFrameFormat>

/**
 * \file constants.h
 * \author Eric Maeker
 * \version 0.8.0
 * \date 25 Apr 2012
*/

namespace PadTools {
namespace Constants {

// Menus && Actions
const char * const M_PLUGIN_PADTOOLS = "mPadTools";
const char * const C_PADTOOLS_PLUGINS = "cPadTools";
const char * const A_PADTOOLS_VIEWOUTPUT = "aPTViewOutPut";
const char * const A_PADTOOLS_SHOWSOURCE = "aPTShowSource";

const char * const TOKENVALUE_MIME = "freepad/token/value";
const char * const TOKENRAWSOURCE_MIME = "freepad/token/rawsource";
const char * const TOKENUID_MIME = "freepad/token/uid";

const char * const TOKEN_CORE_DELIMITER  = "~";
const char * const TOKEN_OPEN_DELIMITER  = "{{";
const char * const TOKEN_CLOSE_DELIMITER = "}}";

const char * const TOKEN_AND_POSITION_TAG = "\n<!--%2-->%1<!--%2-->\n";
const char * const ANCHOR_ITEM = "item";

// Translations
const char * const PADWRITER_TRANS_CONTEXT = "PadWriter";

const char * const PADTOOLS_TEXT = QT_TRANSLATE_NOOP("PadWriter", "PadTools");
const char * const FOLLOW_CURSOR_IN_RESULT_OUTPUT = QT_TRANSLATE_NOOP("PadWriter", "Follow cursor in result output");
const char * const FIND_CURSOR_IN_RESULT = QT_TRANSLATE_NOOP("PadWriter", "Find cursor");
const char * const AUTO_UPDATE_RESULT = QT_TRANSLATE_NOOP("PadWriter", "Automatic update of results");
const char * const VIEW_OUTPUT = QT_TRANSLATE_NOOP("PadWriter", "View output");
const char * const SHOW_SOURCE = QT_TRANSLATE_NOOP("PadWriter", "Show source");

// Translations used for the TokenOutputDocument drag'n drop
const char * const INSIDE_CONDITIONNAL_BEFORE_TEXT = QT_TRANSLATE_NOOP("PadWriter", "Inside conditional before text");
const char * const INSIDE_CONDITIONNAL_AFTER_TEXT = QT_TRANSLATE_NOOP("PadWriter", "Inside conditional after text");
const char * const BEFORE_TOKEN = QT_TRANSLATE_NOOP("PadWriter", "Before the token");
const char * const AFTER_TOKEN = QT_TRANSLATE_NOOP("PadWriter", "After the token");
const char * const NESTED_TOKEN = QT_TRANSLATE_NOOP("PadWriter", "Nested token");

const char * const ICON_PADSOURCE = "pad_source.png";
const char * const ICON_PADTOKENS = "pad_tokens.png";

/** Column representation for the token model */
enum TokenModelDataRepresentation {
    TokenModel_HtmlLabelColumn = 0,
    TokenModel_UuidColumn,
    TokenModel_ColumnCount
};

}  // namespace Constants
}  // namespace PadTools

#endif // PADTOOLS_CONSTANTS_H
