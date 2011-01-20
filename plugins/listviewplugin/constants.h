/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef VIEWS_PLUGIN_CONSTANTS_H
#define VIEWS_PLUGIN_CONSTANTS_H

#include <QtGlobal>
#include <QString>

namespace Views {
namespace Constants {

    // Translate context
    const char * const  VIEWS_CONSTANTS_TR_CONTEXT = "Views";

    // Database
    const char * const  DB_NAME            = "views";
    const char * const  DB_FILENAME        = "views.db";
    const char * const  DB_ACTUALVERSION   = "0.1";

    // Tables
    enum Tables {
      Table_CATEGORIES = 0,
      Table_CATEGORY_LABEL,
      Table_VERSION
    };

    enum PmhCatFields {
        CATEGORY_ID = 0, CATEGORY_MIME, CATEGORY_PARENT, CATEGORY_LABEL_ID, CATEGORY_THEMEDICON,
        CATEGORY_MaxParam
    };

    enum PmhCatLabelFields {
        CATEGORYLABEL_ID = 0, CATEGORYLABEL_LANG, CATEGORYLABEL_VALUE,
        CATEGORYLABEL_MaxParam
    };

}  // End namespace Constants
}  // End namespace Views


#endif // VIEWS_PLUGIN_CONSTANTS_H
