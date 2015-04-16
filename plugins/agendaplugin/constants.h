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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef AGENDA_CONSTANTS_H
#define AGENDA_CONSTANTS_H

#include <calendar/modelanditem/calendar_item.h>

namespace Agenda {
namespace Constants {

    // User Database connection
    const char * const DB_NAME           = "agenda";
    const char * const DB_VERSION        = "0.1";

    // For database management
    /** \brief Enumerates all the tables of the database */
    enum Tables
    {
        Table_CALENDAR = 0, Table_USERCALENDARS, Table_AVAILABILITIES,
        Table_TIMERANGE, Table_AVAIL_TO_TIMERANGE, Table_COMMON,
        Table_EVENTS, Table_CYCLINGEVENTS,
        Table_PEOPLE,
        // Table_USER_WORKINGTIME,
        Table_VERSION,
        Table_MaxParam
    };

    enum fieldsUSERCALENDARS {
        USERCAL_ID = 0,
        USERCAL_USER_UUID,
        USERCAL_CAL_ID
    };

    enum FieldsCALENDAR {
            CAL_ID = 0,
            CAL_UID,
            CAL_CATEGORYID,
            CAL_SORTID,
            CAL_ISVALID,
            CAL_ISVIRTUAL,
            CAL_STATUS,
            CAL_TYPE,
            CAL_SITEUID,
            CAL_DEFAULT,
            CAL_ISPRIVATE,
            CAL_PASSWORD,
            CAL_DEFAULTDURATION,
            CAL_LABEL,
            CAL_FULLCONTENT,
            CAL_THEMEDICON,
            CAL_XMLOPTIONS,
            CAL_MaxParam
    };

    enum FieldsAVAILABILITIES {
        AVAIL_ID = 0,
        AVAIL_CAL_ID,
        AVAIL_WEEKDAY
    };

    enum FieldsAVTOTR {
        AVTOTR_AVID = 0,
        AVTOTR_TRID
    };

    enum FieldsTIMERANGE {
        TIMERANGE_ID = 0,
        TIMERANGE_FROM,
        TIMERANGE_TO
    };
    enum fieldsCOMMON_FOR_EVENTS {
        COMMON_ID = 0,
        COMMON_CAT_ID,
        COMMON_TYPE_ID,
        COMMON_STATUS_ID, // (like draft, validated, rejected, need more info, need to send letter…)
        COMMON_SITE_UID, // (for a later use)
        COMMON_ISVIRTUAL,
        COMMON_ISPRIVATE,
        COMMON_PASSWORD, // (private events can be protected by a password)
        COMMON_ISBUSY,
        COMMON_ISAGROUPEVENT, // (event can be defined by and for a group of users)
        COMMON_LABEL,
        COMMON_FULLCONTENT,
        COMMON_TEXTUAL_SITE,
        COMMON_THEMEDICON,
        COMMON_XMLVIEWOPTIONS, // (color, margins, spacing…)
        COMMON_XMLCALOPTIONS // (can be changed…)
    };

    enum fieldsEVENTS {
        EVENT_ID = 0,
        EVENT_CAL_ID,
        EVENT_COMMON_ID,
        EVENT_ISVALID,
        EVENT_DATESTART,
        EVENT_DATEEND
    };

    enum fieldsPEOPLE {
        PEOPLE_ID = 0,
        PEOPLE_CAL_ID,
        PEOPLE_EVENT_ID,
        PEOPLE_UID,
        PEOPLE_TYPE
    };

    enum fieldsCYCLINGEVENTS {
        CYCLING_ID = 0,
        CYCLING_CALENDAR_ID,
        CYCLING_COMMON_ID,
        CYCLING_PATIENT_UID,
        CYCLING_ISNOTRELATEDTOPATIENT,
        CYCLING_ISVALID,
        CYCLING_DATESTART,
        CYCLING_DATEEND,
        CYCLING_REPEATINTERVAL,
        CYCLING_REPEATSCHEMA,
        CYCLING_REPEATTIMES // (eg repeat only seven times)
    };

    enum fieldsUSER_WORKINGTIME {
//        Define here the user vacancies, working time, time for each types
    };

    enum VERSION_Fields {
        VERSION_ACTUAL = 0
    };

    // Private data for Calendar items
    enum PrivateDataCalendarItem {
        Db_CalId = Calendar::CalendarItem::UserData + 1,
        Db_UserCalId,
        Db_CatId,
        Db_EvId,
        Db_CyclingEvId,
        Db_ComId,
        Db_IsValid,
        Db_IsVirtual,
        Db_SortId,
        Db_XmlViewOptions,
        Db_XmlOptions
    };

    // WIDGETS
    const char * const C_AGENDA_PLUGIN = "contextAgenga";
    const char * const M_PLUGINS_AGENDA = "menuAgenda";
    const char * const G_AGENDA_VIEW = "gAgendaView";
    const char * const G_AGENDA_EDIT = "gAgendaEdit";
    const char * const G_AGENDA_OPTIONS = "gAgendaOptions";

    const char * const A_NEW_AGENDAEVENT = "agendaNewEvent";
    const char * const A_PRINT_SELECTION = "agendaPrintSelection";
    const char * const A_PRINT_PREVIEW_SELECTION = "agendaPreviewSelection";
    const char * const A_AGENDADATABASE_INFORMATION = "agendaDbInfo";

    // Icons
    const char * const I_NEW_AGENDAEVENT = "appointment-new.png";
    const char * const I_VIEWMODE = "agenda_viewmode.png";
    const char * const I_DAYVIEWMODE = "agenda_viewmode_day.png";
    const char * const I_MONTHVIEWMODE = "agenda_viewmode_month.png";
    const char * const I_WEEKVIEWMODE = "agenda_viewmode_week.png";
    const char * const I_RESET_TO_DEFAULT = "reset-to-default.png";

    // Translations
    const char * const AGENDA_TR_CONTEXT = "Agenda";
    const char * const PRINTSELECTION_TEXT = QT_TRANSLATE_NOOP("Agenda", "Print current selection");

}  // End Constants
}  // End Agenda

#endif // AGENDA_CONSTANTS_H
