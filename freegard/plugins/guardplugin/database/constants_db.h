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
 *  Main Developers:   Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#ifndef GUARD_DATABASE_CONSTANTS_H
#define GUARD_DATABASE_CONSTANTS_H

#include <QFlags>
#include <QHash>
#include <QStringList>

/**
 * \file constants_db.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 10 Oct 2014
*/

namespace Guard {
namespace Constants {

    const char * const DB_GUARD           = "guard";
    const char * const DB_DEFAULT_USER    = "GuardDefaultUser";
    const char * const DB_ACTUALVERSION   = "0.0.1";

    //DatapackModel
//    const char * const DATAPACK_GUARD = "guard";
//    const char * const DATAPACK_DB = "guard_datapack";
//    const char * const DATAPACK_GUARD_FILENAME = "guard_datapack.db";

    // Database abstraction
    enum Tables {
        Table_Guardian = 0,
        Table_GuardLine,
        Table_GuardLineTimeSlot,
        Table_GuardLineInclusion,
        Table_GuardianPref,
        Table_GuardTable,
        Table_Location,
        Table_LocationLink,
        Table_VERSION
    };

    enum TableGuardian_Fields {
        GUARDIAN_ID = 0,
        GUARDIAN_UID,
        GUARDIAN_ISVALID,
        GUARDIAN_FULLNAME,
        GUARDIAN_EMAIL,
        GUARDIAN_ACCEPT_MAILS,
        GUARDIAN_MOBILEPHONE,
        GUARDIAN_ACCEPT_SMS,
        GUARDIAN_GENERALPREF_UID,
        GUARDIAN_LOCATION_FK,
        GUARDIAN_MaxParam
    };

    enum Table_GuardianPref_Fields {
        GUARDIAN_PREFS_ID = 0,
        GUARDIAN_PREFS_GUARDIAN_UID,
        GUARDIAN_PREFS_GUARDLINE_UID,
        GUARDIAN_PREFS_CONTENT,
        GUARDIAN_PREFS_MaxParam
    };

    enum TableGuardLine_Fields {
        GUARDLINE_ID = 0,
        GUARDLINE_UID,
        GUARDLINE_ISVALID,
        GUARDLINE_LABEL,
        GUARDLINE_LOCATION,
        GUARDLINE_INFO,
        GUARDLINE_MAILINGLIST,
        GUARDLINE_LINESLOT_FK,
        GUARDLINE_PRIORITY,
        GUARDLINE_LOCATION_FK,
        GUARDLINE_MaxParam
    };

    enum Table_GuardLineTimeSlot_Fields {
        LINESLOT_ID = 0,
        LINESLOT_ISVALID,
        LINESLOT_FK,
        LINESLOT_MONDAY,
        LINESLOT_TUESDAY,
        LINESLOT_WEDNESDAY,
        LINESLOT_THURSDAY,
        LINESLOT_FRIDAY,
        LINESLOT_SATURDAY,
        LINESLOT_SUNDAY,
        LINESLOT_SPECIFIEDDATES,
        LINESLOT_BEGINHOUR,
        LINESLOT_ENDHOUR,
        LINESLOT_PERIOD_TYPE,
        LINESLOT_PERIOD_INFORMATION,
        LINESLOT_MaxParam
    };

    enum TableGuardTable_Fields {
        GUARDTABLE_ID = 0,
        GUARDTABLE_UID,
        GUARDTABLE_ISVALID,
        GUARDTABLE_STATE,           // Preview (rw), updated preview (rw), definitive (ro)
        GUARDTABLE_MaxParam
    };

    enum TableGuardLineInclusion_Fields {
        LINEINCL_ID = 0,
        LINEINCL_LINE_UID,
        LINEINCL_GARDIAN_UID,
        LINEINCL_DATESTART,
        LINEINCL_DATEEND,
        LINEINCL_MONTH,
        LINEINCL_YEARS,
        LINEINCL_CYCLING_START,
        LINEINCL_CYCLING_END,
        LINEINCL_MaxParam
    };

    enum Table_Location_Fields {
        LOCATION_ID = 0,
        LOCATION_UID,
        LOCATION_LABEL,
        LOCATION_MaxParam
    };

    enum Table_LocationLink_Fields {
        LOCATIONLINK_FK = 0,
        LOCATIONLINK_LOCATION_ID,
        LOCATIONLINK_MaxParam
    };

    enum Table_Version_Fields {
        VERSION_TEXT = 0
    };

}  // End namespace Constants
}  // End namespace Guard

#endif // GUARD_DATABASE_CONSTANTS_H
