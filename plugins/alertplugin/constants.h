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
 *  Main Developers:                                                       *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef ALERT_PLUGIN_CONSTANTS_H
#define ALERT_PLUGIN_CONSTANTS_H

/**
 * \file constants.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 28 May 2012
*/

namespace Alert {
namespace Constants {

const char * const DB_NAME                 = "alerts";
const char * const DB_FILENAME             = "alerts.db";
const char * const DB_INITIAL_VERSION      = "0.1";
const char * const OLD_VERSION_TABLE       = "VER";
const char * const OLD_VERSION_FIELD       = "TXT";
const int          DB_CURRENT_CODE_VERSION = 1;

// Tables
enum Tables {
    Table_ALERT = 0,
    Table_ALERT_RELATED,
    Table_ALERT_LABELS,
    Table_ALERT_SCRIPTS,
    Table_ALERT_TIMING,
    Table_ALERT_VALIDATION,
    Table_ALERT_PACKS,
    //Table_ALERT_VERSION
    Table_SCHEMA
};

// Fields
enum AlertFields {
    ALERT_ID = 0,

    // Uids
    ALERT_UID,
    ALERT_PACKUID,
    ALERT_CATEGORY_UID,  // for later use with category plugin
    ALERT_REL_ID,
    ALERT_SID,
    ALERT_VAL_ID,
    ALERT_TIM_ID,
    ALERT_ISVALID,
    ALERT_ISREMINDABLE,
    // TODO : Add a trace for alert modification

    // Types
    ALERT_VIEW_TYPE,
    ALERT_CONTENT_TYPE,
    ALERT_CONDITION_TYPE,
    ALERT_PRIORITY,
    ALERT_OVERRIDEREQUIREUSERCOMMENT,
    ALERT_MUSTBEREAD,

    // Translatable description items
    ALERT_LABEL_LID,
    ALERT_CATEGORY_LID,  // while waiting integration of category plugin
    ALERT_DESCRIPTION_LID,
    ALERT_COMMENT_LID,

    // Non translable items
    ALERT_CREATION_DATE,
    ALERT_LAST_UPDATE_DATE,
    ALERT_THEMED_ICON,
    ALERT_THEME_CSS,
    ALERT_CRYPTED_PASSWORD,

    // Other
    ALERT_EXTRA_XML
};

enum AlertRelatedFields {
    ALERT_RELATED_ID = 0,
    ALERT_RELATED_REL_ID,
    ALERT_RELATED_RELATED_TO,
    ALERT_RELATED_RELATED_UID
};

enum AlertLabelFields {
    ALERT_LABELS_ID = 0,
    ALERT_LABELS_LABELID,
    ALERT_LABELS_LANG,
    ALERT_LABELS_VALUE,
    ALERT_LABELS_ISVALID
};

enum AlertScriptsFields {
    ALERT_SCRIPTS_ID = 0,
    ALERT_SCRIPTS_SID,
    ALERT_SCRIPT_UID,
    ALERT_SCRIPT_ISVALID,
    ALERT_SCRIPT_TYPE,
    ALERT_SCRIPT_CONTENT
};

enum AlertTimingFields {
    ALERT_TIMING_TIMINGID = 0,
    ALERT_TIMING_TIM_ID,
    ALERT_TIMING_ISVALID,
    ALERT_TIMING_STARTDATETIME,
    ALERT_TIMING_ENDDATETIME,
    ALERT_TIMING_CYCLES,        // number of cycle or infinite cycling
    ALERT_TIMING_CYCLINGDELAY,  // cycle every X **minutes**
    ALERT_TIMING_NEXTCYCLE
};

enum AlertValidationFields {
    ALERT_VALIDATION_VID = 0,
    ALERT_VALIDATION_VAL_ID,
    ALERT_VALIDATION_DATEOFVALIDATION,
    ALERT_VALIDATION_VALIDATOR_UUID,
    ALERT_VALIDATION_OVERRIDDEN,
    ALERT_VALIDATION_USER_COMMENT,
    ALERT_VALIDATION_VALIDATED_UUID
};

enum AlertPacksFields {
    ALERT_PACKS_ID = 0,
    ALERT_PACKS_UID,
    ALERT_PACKS_ISVALID,
    ALERT_PACKS_IN_USE,
    ALERT_PACKS_LABEL_LID,
    ALERT_PACKS_CATEGORY_LID,  // while waiting integration of category plugin
    ALERT_PACKS_DESCRIPTION_LID,
    ALERT_PACKS_AUTHORS,
    ALERT_PACKS_VENDOR,
    ALERT_PACKS_URL,
    ALERT_PACKS_THEMEDICON,
    ALERT_PACKS_VERSION,
    ALERT_PACKS_FMFVERSION,
    ALERT_PACKS_CREATEDATE,
    ALERT_PACKS_LASTUPDATE,
    ALERT_PACKS_XTRAXML
};

enum SchemaFields {
    SCHEMA_ID =0 ,
    SCHEMA_VERSION,
    SCHEMA_SCRIPT,
    SCHEMA_TIMESTAMP
};

// Pack description
const char * const  PACK_DESCRIPTION_FILENAME   = "packdescription.xml";

}  // namespace Constants
}  // namespace Alert

#endif // ALERT_PLUGIN_CONSTANTS_H
