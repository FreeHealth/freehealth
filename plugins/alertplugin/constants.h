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
 *   Main Developpers:                                                     *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *       Pierre-Marie Desombre <pm.desombre@gmail.com>                     *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef CONSTANTS_H
#define CONSTANTS_H

/**
 * \file constants.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 28 May 2012
*/

namespace Alert {
namespace Constants {

const char * const  DB_NAME            = "alerts";
const char * const  DB_FILENAME        = "alerts.db";
const char * const  DB_ACTUALVERSION   = "0.1";

// Tables
enum Tables {
    Table_ALERT = 0,
    Table_ALERT_LABELS,
    Table_ALERT_SCRIPTS,
    Table_ALERT_TIMING,
    Table_ALERT_VERSION
};

// Fields
enum AlertFields {
    ALERT_ID = 0,

    // Uids
    ALERT_UID,
    ALERT_USER_UID,
    ALERT_GROUP_UID,  // for future use (user groups)
    ALERT_PATIENT_UID,
    ALERT_FAMILY_UID, // for future use (patient family)
    ALERT_APP_NAME,
    ALERT_CATEGORY_UID,
    ALERT_SCRIPTS_ID,
    ALERT_ISVALID,
    // TODO : Add a trace for alert modification

    // Types
    ALERT_VIEW_TYPE,
    ALERT_CONTENT_TYPE,
    ALERT_TIMING_ID,
    ALERT_CONDITION_TYPE,
    ALERT_PRIORITY,

    // Translatable description items
    ALERT_LABEL_LABELID,
    ALERT_DESCRIPTION_LABELID,
    ALERT_COMMENT_LABELID,

    // Non translable items
    ALERT_CREATION_DATE,
    ALERT_LAST_UPDATE_DATE,
    ALERT_THEMED_ICON,
    ALERT_THEME_CSS,
    ALERT_CRYPTED_PASSWORD,

    // Other
    ALERT_EXTRA_XML
};

enum AlertLabelFields {
    ALERT_LABELS_ID = 0,
    ALERT_LABELS_LABELID,
    ALERT_LABELS_LANG,
    ALERT_LABELS_VALUE,
    ALERT_LABELS_ISVALID
};

enum AlertScriptsFields {
    ALERT_SCRIPTS_SID = 0,
    ALERT_SCRIPT_UID,
    ALERT_SCRIPT_ISVALID,
    ALERT_SCRIPT_TYPE,
    ALERT_SCRIPT_CONTENT
};

enum AlertTimingFields {
    ALERT_TIMING_TIMINGID = 0,
    ALERT_TIMING_ISVALID,
    ALERT_TIMING_STARTDATETIME,
    ALERT_TIMING_ENDDATETIME,
    ALERT_TIMING_CYCLES,        // number of cycle or infinite cycling
    ALERT_TIMING_CYCLINGDELAY,  // cycle every X days
    ALERT_TIMING_NEXTCYCLE
};

enum VersionFields {
    VERSION_TEXT = 0
};

}  // namespace Constants
}  // namespace Alert

#endif // CONSTANTS_H
