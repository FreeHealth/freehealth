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
#ifndef CATEGORY_PLUGIN_CONSTANTS_H
#define CATEGORY_PLUGIN_CONSTANTS_H

#include <QtGlobal>
#include <QString>

namespace Category {
namespace Constants {

    // Translate context
    const char * const  CATEGORY_CONSTANTS_TR_CONTEXT = "Category";

    // Database
    const char * const DB_NAME            = "category";
    const char * const DB_FILENAME        = "category.db";
    //const char * const DB_CURRENTVERSION   = "0.1";
    const char * const DB_INITIAL_VERSION = "0.1";
    const char * const OLD_VERSION_TABLE  = "VERSION";
    const char * const OLD_VERSION_FIELD  = "VERSION";
    const int          DB_CURRENT_CODE_VERSION  = 1;

    enum Tables {
      Table_CATEGORIES = 0,
      Table_CATEGORY_LABEL,
      Table_PROTECTION,
      //Table_VERSION
      Table_SCHEMA
    };

    enum CategoryFields {
        CATEGORY_ID = 0,
        CATEGORY_UUID,
        CATEGORY_PARENT,
        CATEGORY_LABEL_ID,
        CATEGORY_MIME,
        CATEGORY_PROTECTION_ID,
        CATEGORY_SORT_ID,
        CATEGORY_PASSWORD,
        CATEGORY_ISVALID,
        CATEGORY_THEMEDICON,
        CATEGORY_EXTRAXML,
        CATEGORY_MaxParam
    };

    enum CategoryLabelFields {
        CATEGORYLABEL_ID = 0,
        CATEGORYLABEL_LABEL_ID,
        CATEGORYLABEL_LANG,
        CATEGORYLABEL_VALUE,
        CATEGORYLABEL_ISVALID,
        CATEGORYLABEL_MaxParam
    };

    enum ProtectionFields {
        PROTECTION_ID = 0,
        PROTECTION_PID,
        PROTECTION_RESTRICTEDTOUSER,
        PROTECTION_RESTRICTEDTOGROUP,
        PROTECTION_MaxParam
    };

    /*enum VersionFields {
        VERSION_TEXT = 0
    };*/
    enum SchemaFields {
        SCHEMA_ID =0 ,
        SCHEMA_VERSION,
        SCHEMA_SCRIPT,
        SCHEMA_TIMESTAMP
    };

}  // End namespace Constants
}  // End namespace Category


#endif // CATEGORY_PLUGIN_CONSTANTS_H
