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
#ifndef TKCONSTANTTRANSLATIONS_H
#define TKCONSTANTTRANSLATIONS_H

#include <translationutils/translationutils_exporter.h>

#include <QString>
#include <QStringList>
#include <QCoreApplication>

namespace Trans {
namespace Constants {

const char* const CONSTANTS_TR_CONTEXT = "tkConstants";
const char* const CONSTANTS_TRANSLATOR_NAME = "translationutils";

namespace Physiology {

    enum PregnancyLimit  // QFlag
    {
        NeverDuringPregnacy = 0x00000000,
        FirstQuaOk          = 0x00000010,
        SecondQuaOk         = 0x00000100,
        ThirdQuaOk          = 0x00001000,
        UsableWithWarn      = 0x00010000,
        PregnancyOk         = 0x00100000
    };
    Q_DECLARE_FLAGS( PregnancyLimits, PregnancyLimit );

    enum PhysiologicalLimit // QFlag
    {
        EveryOne     = 0x00000000,
        Infant       = 0x00000001,  // Nourrisson
        Child        = 0x00000010,
        AdultOnly    = 0x00000100,
        ChronicRenalFailureAvoid   = 0x00001000,
        ChronicHepaticFailureAvoid   = 0x00010000,
        ManOnly      = 0x01000000,
        WomanOnly    = 0x10000000
    };
    Q_DECLARE_FLAGS( PhysiologicalLimits, PhysiologicalLimit );

}
Q_DECLARE_OPERATORS_FOR_FLAGS( Physiology::PregnancyLimits)
Q_DECLARE_OPERATORS_FOR_FLAGS( Physiology::PhysiologicalLimits)


namespace Time {

    enum Period
    {
        Seconds = 0,
        Minutes,
        Hours,
        Days,
        Weeks,
        Months,
        Quarter,
        Year,
        Decade
    };

    enum MealTime
    {
        Undefined = 0,
        NoRelationWithMeal,
        DuringMeal,
        BeforeMeal,
        AfterMeal,
        OutsideMeal,
        OnlyIfMeal,
        SeparatedFromFood,
        SuggestedWithFood,
        WithOrWithoutFood
    };


    enum DailyScheme
    {
        DS_WakeUpTime = 0,
        DS_Breakfast,
        DS_Morning,
        DS_10OClockBrunch,
        DS_MidDay,
        DS_Lunch,
        DS_TeaTime,
        DS_Evening,
        DS_Diner,
        DS_BedTime,
        DS_MaxParam
    };
//    Q_DECLARE_FLAGS( DailySchemes, DailyScheme );
}
//Q_DECLARE_OPERATORS_FOR_FLAGS(Time::DailySchemes)


enum TitleInterpret {
    NoTitle = 0,
    Mister,
    Miss,
    Madam,
    Doctor,
    Professor,
    Captain
};




	// Used for the language menu
        const char * const  ENGLISH             = QT_TRANSLATE_NOOP("tkConstants", "English");

        // Menus translations
        const char * const  M_FILE_TEXT              = QT_TRANSLATE_NOOP("tkConstants", "File");
        const char * const  M_FILE_RECENTFILES_TEXT  = QT_TRANSLATE_NOOP("tkConstants", "Recent files");
        const char * const  M_PATIENT_NAVIGATION_TEXT= QT_TRANSLATE_NOOP("tkConstants", "Patient navigation");
        const char * const  M_EDIT_TEXT              = QT_TRANSLATE_NOOP("tkConstants", "Edit");
        const char * const  M_EDIT_SELECT_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "Selection");
        const char * const  M_EDIT_LIST_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "List");
        const char * const  M_EDIT_TEMPLATES_TEXT    = QT_TRANSLATE_NOOP("tkConstants", "Templates");
        const char * const  M_FORMAT_TEXT            = QT_TRANSLATE_NOOP("tkConstants", "Format");
        const char * const  M_FORMAT_FONT_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "Font");
        const char * const  M_FORMAT_PARAGRAPH_TEXT  = QT_TRANSLATE_NOOP("tkConstants", "Paragraph");
        const char * const  M_FORMAT_TABLE_TEXT      = QT_TRANSLATE_NOOP("tkConstants", "Table");
        const char * const  M_FORMAT_IMAGE_TEXT      = QT_TRANSLATE_NOOP("tkConstants", "Image");
        const char * const  M_TOOLS_TEXT             = QT_TRANSLATE_NOOP("tkConstants", "Tools");
        const char * const  M_PLUGINS_TEXT           = QT_TRANSLATE_NOOP("tkConstants", "Plugins");
        const char * const  M_CONFIGURATION_TEXT     = QT_TRANSLATE_NOOP("tkConstants", "Configuration");
        const char * const  M_TOOLBARS_TEXT          = QT_TRANSLATE_NOOP("tkConstants", "Toolbars");
        const char * const  M_LANGUAGES_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "Languages");
        const char * const  M_INTERPRETOR_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "Interpretor");
        const char * const  M_ABOUT_TEXT             = QT_TRANSLATE_NOOP("tkConstants", "About");
        const char * const  ABOUTDATABASES_TEXT      = QT_TRANSLATE_NOOP("tkConstants", "About databases");
        const char * const  M_UPDATE_TEXT            = QT_TRANSLATE_NOOP("tkConstants", "Update available");

        // Actions translations
        const char * const  FILENEW_TEXT          = QT_TRANSLATE_NOOP("tkConstants", "New");
        const char * const  FILEOPEN_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "Open");
        const char * const  FILESAVE_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "Save");
        const char * const  FILESAVEAS_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "Save as");
        const char * const  FILEPRINT_TEXT        = QT_TRANSLATE_NOOP("tkConstants", "Print...");
        const char * const  PRINTPREVIEW_TEXT     = QT_TRANSLATE_NOOP("tkConstants", "Print preview");
        const char * const  FILEEXIT_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "Exit");
        const char * const  SEARCHMENU_TEXT           = QT_TRANSLATE_NOOP("tkConstants", "Search...");

        const char * const  PATIENTNEW_TEXT           = QT_TRANSLATE_NOOP("tkConstants", "New Patient");
        const char * const  PATIENTREMOVE_TEXT        = QT_TRANSLATE_NOOP("tkConstants", "Remove Patient");
        const char * const  PATIENTVIEWIDENTITY_TEXT  = QT_TRANSLATE_NOOP("tkConstants", "View Patient Identity");
        const char * const  IDENTITY_TEXT             = QT_TRANSLATE_NOOP("tkConstants", "Identity");

        const char * const  OPENPRESCRIPTION_TEXT = QT_TRANSLATE_NOOP("tkConstants", "Open a prescription");
        const char * const  SAVEPRESCRIPTION_TEXT = QT_TRANSLATE_NOOP("tkConstants", "Save a prescription");
        const char * const  PRINTPRESCRIPTION_TEXT = QT_TRANSLATE_NOOP("tkConstants", "Print a prescription");

        const char * const  CLOSE_TEXT            = QT_TRANSLATE_NOOP("tkConstants", "Close");
        const char * const  EDITCOPY_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "Copy");
        const char * const  EDITCUT_TEXT          = QT_TRANSLATE_NOOP("tkConstants", "Cut");
        const char * const  EDITPASTE_TEXT        = QT_TRANSLATE_NOOP("tkConstants", "Paste");
        const char * const  EDITUNDO_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "Undo");
        const char * const  EDITREDO_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "Redo");
        const char * const  EDITSELECTALL_TEXT    = QT_TRANSLATE_NOOP("tkConstants", "Select All");
        const char * const  EDITDESESLECT_TEXT    = QT_TRANSLATE_NOOP("tkConstants", "Deselect");
        const char * const  NEXT_TEXT             = QT_TRANSLATE_NOOP("tkConstants", "Next");
        const char * const  PREVIOUS_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "Previous");
        const char * const  HOME_TEXT             = QT_TRANSLATE_NOOP("tkConstants", "Home");
        const char * const  LISTADD_TEXT          = QT_TRANSLATE_NOOP("tkConstants", "Add to list");
        const char * const  LISTREMOVE_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "Remove from list");
        const char * const  LISTMOVEUP_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "Move item up");
        const char * const  LISTMOVEDOWN_TEXT     = QT_TRANSLATE_NOOP("tkConstants", "Move item down");
        const char * const  LISTCLEAR_TEXT        = QT_TRANSLATE_NOOP("tkConstants", "Clear");
        const char * const  LISTSORT_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "Sort");

        const char * const  TEMPLATES             = QT_TRANSLATE_NOOP("tkConstants", "Templates");
        const char * const  CREATETEMPLATE_TEXT   = QT_TRANSLATE_NOOP("tkConstants", "Create a new template");
        const char * const  TEMPLATES_TOGGLEVIEW_TEXT   = QT_TRANSLATE_NOOP("tkConstants", "Toggle template view");
        const char * const  ADDCATEGORY_TEXT      = QT_TRANSLATE_NOOP("tkConstants", "Add a category");
        const char * const  ADD_TEXT              = QT_TRANSLATE_NOOP("tkConstants", "Add");
        const char * const  REMOVE_TEXT           = QT_TRANSLATE_NOOP("tkConstants", "Remove");
        const char * const  LOCK_TEXT             = QT_TRANSLATE_NOOP("tkConstants", "Lock");
        const char * const  UNLOCK_TEXT           = QT_TRANSLATE_NOOP("tkConstants", "Unlock");
        const char * const  LOCKED_TEXT           = QT_TRANSLATE_NOOP("tkConstants", "Locked");
        const char * const  UNLOCKED_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "Unlocked");
        const char * const  TEMPLATES_DATABASE_INFORMATIONS_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "Templates database informations");

        const char * const  PMHX                  = QT_TRANSLATE_NOOP("tkConstants", "PMHx");
        const char * const  PASTMEDICALHISTORY    = QT_TRANSLATE_NOOP("tkConstants", "Past medical history");
        const char * const  PMHX_PREFERENCES      = QT_TRANSLATE_NOOP("tkConstants", "PMHx preferences");

        const char * const  FORMATBOLD_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "Bold");
        const char * const  FORMATITALIC_TEXT     = QT_TRANSLATE_NOOP("tkConstants", "Italic");
        const char * const  FORMATUNDERLINE_TEXT  = QT_TRANSLATE_NOOP("tkConstants", "Underline");
        const char * const  FORMATSTRIKE_TEXT     = QT_TRANSLATE_NOOP("tkConstants", "Strike");
        const char * const  FORMATBIGGER_TEXT     = QT_TRANSLATE_NOOP("tkConstants", "Bigger");
        const char * const  FORMATSMALLER_TEXT    = QT_TRANSLATE_NOOP("tkConstants", "Smaller");
        const char * const  FORMATFONT_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "Font format");
        const char * const  FORMATFONTCOLOR_TEXT  = QT_TRANSLATE_NOOP("tkConstants", "Color");
        const char * const  FORMATTABLE_TEXT      = QT_TRANSLATE_NOOP("tkConstants", "Table format");
        const char * const  FORMATIMAGE_TEXT      = QT_TRANSLATE_NOOP("tkConstants", "Image format");
        const char * const  ALIGNCENTER_TEXT      = QT_TRANSLATE_NOOP("tkConstants", "Center");
        const char * const  ALIGNLEFT_TEXT        = QT_TRANSLATE_NOOP("tkConstants", "Left");
        const char * const  ALIGNRIGHT_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "Right");
        const char * const  ALIGNJUSTIFY_TEXT     = QT_TRANSLATE_NOOP("tkConstants", "Justify");

        const char * const  TABLE_ADDTABLE_TEXT   = QT_TRANSLATE_NOOP("tkConstants", "Add table");
        const char * const  TABLE_PROPERTIES_TEXT = QT_TRANSLATE_NOOP("tkConstants", "Table properties");
        const char * const  TABLE_ADDROW_TEXT   = QT_TRANSLATE_NOOP("tkConstants", "Add row");
        const char * const  TABLE_REMOVEROW_TEXT   = QT_TRANSLATE_NOOP("tkConstants", "Remove row");
        const char * const  TABLE_ADDCOLUMN_TEXT   = QT_TRANSLATE_NOOP("tkConstants", "Add colunm");
        const char * const  TABLE_REMOVECOLUMN_TEXT   = QT_TRANSLATE_NOOP("tkConstants", "Remove column");
        const char * const  TABLE_MERGECELLS_TEXT   = QT_TRANSLATE_NOOP("tkConstants", "Merge cells");
        const char * const  TABLE_SPLITCELLS_TEXT   = QT_TRANSLATE_NOOP("tkConstants", "Split cells");

        const char * const  ABOUT_TEXT            = QT_TRANSLATE_NOOP("tkConstants", "About");
        const char * const  ABOUTPLUGINS_TEXT     = QT_TRANSLATE_NOOP("tkConstants", "About Plugins");
        const char * const  ABOUTQT_TEXT          = QT_TRANSLATE_NOOP("tkConstants", "About Qt");
        const char * const  ABOUTFORM_TEXT        = QT_TRANSLATE_NOOP("tkConstants", "About this form");
        const char * const  APPLICATIONHELP_TEXT  = QT_TRANSLATE_NOOP("tkConstants", "Help");
        const char * const  DEBUGHELPER_TEXT      = QT_TRANSLATE_NOOP("tkConstants", "Debugging tools");
        const char * const  HELPTEXTTOGGLER_TEXT  = QT_TRANSLATE_NOOP("tkConstants", "Display/Hide form's help text");
        const char * const  HELP_TEXT             = QT_TRANSLATE_NOOP("tkConstants", "Help");
        const char * const  INTERPRETOR_GENERAL_TEXT  = QT_TRANSLATE_NOOP("tkConstants", "Interpretor");
        const char * const  USERMANAGER_TEXT      = QT_TRANSLATE_NOOP("tkConstants", "User manager");
        const char * const  PREFERENCES_TEXT      = QT_TRANSLATE_NOOP("tkConstants", "Preferences");
        const char * const  APPCONFIGURATOR_TEXT  = QT_TRANSLATE_NOOP("tkConstants", "Application global configurator");
        const char* const   CONFIGMEDINTUX_TEXT   = QT_TRANSLATE_NOOP("tkConstants", "Configure MedinTux");
        const char * const  VIEWUPDATE_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "View update informations");
        const char * const  PRINT_PREVIEW_TEXT    = QT_TRANSLATE_NOOP("tkConstants", "Print Preview");
        const char * const  TRANSLATORS_TEXT      = QT_TRANSLATE_NOOP("tkConstants", "Translators");

        const char * const  FULLSCREEN_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "Toggle full screen");
        const char * const  ZOOMIN_TEXT           = QT_TRANSLATE_NOOP("tkConstants", "Zoom in");
        const char * const  ZOOMOUT_TEXT          = QT_TRANSLATE_NOOP("tkConstants", "Zoom out");

        const char * const  NAVIGATION            = QT_TRANSLATE_NOOP("tkConstants", "Navigation");
        const char * const  DEFAULTS              = QT_TRANSLATE_NOOP("tkConstants", "Defaults");

        // Drugs
        const char* const DRUGS_DATABASE                = QT_TRANSLATE_NOOP("tkConstants", "Drugs database");
        const char* const DRUGS_DATABASE_INFORMATIONS   = QT_TRANSLATE_NOOP("tkConstants", "Drugs database informations");
        const char* const DOSAGES_DATABASE_INFORMATIONS   = QT_TRANSLATE_NOOP("tkConstants", "Protocols database informations");
        const char* const INTERACTIONS_DATABASE_INFORMATIONS   = QT_TRANSLATE_NOOP("tkConstants", "Interactions database informations");
        const char* const PMH_DATABASE_INFORMATIONS     = QT_TRANSLATE_NOOP("tkConstants", "Past medical history database informations");
        const char* const INTERACTIONS_DATABASE         = QT_TRANSLATE_NOOP("tkConstants", "Interactions database");
        const char* const DRUGS                         = QT_TRANSLATE_NOOP("tkConstants", "Drugs");
        const char* const PROTOCOLS                     = QT_TRANSLATE_NOOP("tkConstants", "Protocols");
        const char* const INN                           = QT_TRANSLATE_NOOP("tkConstants", "INN");
        const char* const ATC                           = QT_TRANSLATE_NOOP("tkConstants", "ATC");
        const char* const INTAKES                       = QT_TRANSLATE_NOOP("tkConstants", "intake(s)");
        const char* const VIEWINTERACTIONS_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "View synthetic interactions");
        const char* const FROM                          = QT_TRANSLATE_NOOP("tkConstants", "from");
        const char* const TO                            = QT_TRANSLATE_NOOP("tkConstants", "to");
        const char* const FROM_1_TO_2                   = QT_TRANSLATE_NOOP("tkConstants", "from %1 to %2");
        const char* const DURING                        = QT_TRANSLATE_NOOP("tkConstants", "during");
        const char* const EACH                          = QT_TRANSLATE_NOOP("tkConstants", "each");
        const char* const AND                           = QT_TRANSLATE_NOOP("tkConstants", "and");
        const char* const ALLERGY_TO_1                  = QT_TRANSLATE_NOOP("tkConstants", "Allergy to %1");
        const char* const INTOLERANCE_TO_1              = QT_TRANSLATE_NOOP("tkConstants", "Intolerance to %1");
        const char* const NO_ALLERGIES_INTOLERANCES     = QT_TRANSLATE_NOOP("tkConstants", "No known allergies / intolerances");
        const char* const KNOWN_ALLERGIES               = QT_TRANSLATE_NOOP("tkConstants", "Known allergies");
        const char* const KNOWN_INTOLERANCES            = QT_TRANSLATE_NOOP("tkConstants", "Known intolerances");

        // Drugs Interactions
        const char* const INTERACTION                   = QT_TRANSLATE_NOOP("tkConstants", "Interaction");
        const char* const TAKE_INTO_ACCOUNT             = QT_TRANSLATE_NOOP("tkConstants", "Take into account");
        const char* const DISCOURAGED                   = QT_TRANSLATE_NOOP("tkConstants", "Discouraged");
        const char* const CONTRAINDICATION              = QT_TRANSLATE_NOOP("tkConstants", "Contraindication");
        const char* const P450_IAM                      = QT_TRANSLATE_NOOP("tkConstants", "Cytochrome P450 interaction");
        const char* const GPG_IAM                       = QT_TRANSLATE_NOOP("tkConstants", "Glycoprotein-P interaction");
        const char* const PRECAUTION_FOR_USE            = QT_TRANSLATE_NOOP("tkConstants", "Precaution for use");
        const char* const INFORMATION                   = QT_TRANSLATE_NOOP("tkConstants", "Information");
        const char* const INN_DUPLICATION               = QT_TRANSLATE_NOOP("tkConstants", "Molecule is duplicated");
        const char* const CLASS_DUPLICATION             = QT_TRANSLATE_NOOP("tkConstants", "Class is duplicated");

        // PMHx
        const char* const PMHMENU_TEXT                  = QT_TRANSLATE_NOOP("tkConstants", "Past medical history");

        // Account
        const char* const ACCOUNTANCY  = QT_TRANSLATE_NOOP("tkConstants", "Accountancy");
        const char* const BANK_DETAILS = QT_TRANSLATE_NOOP("tkConstants", "Bank details");
        const char* const AVAILABLE_MOVEMENTS = QT_TRANSLATE_NOOP("tkConstants", "Available movements");
        const char* const MEDICAL_PROCEDURES = QT_TRANSLATE_NOOP("tkConstants", "Medical procedures");
        const char*const  SITES = QT_TRANSLATE_NOOP("tkConstants", "Sites");
        const char*const  INSURANCE = QT_TRANSLATE_NOOP("tkConstants", "Insurance");
        const char*const  PERCENT = QT_TRANSLATE_NOOP("tkConstants", "Percentages");
        const char*const  DISTRULES = QT_TRANSLATE_NOOP("tkConstants", "Distance rules");
        const char*const  ASSETSRATES = QT_TRANSLATE_NOOP("tkConstants", "Assets rates");

        // Database related
        const char *const DATABASE_DRIVER_1_NOT_AVAILABLE = QT_TRANSLATE_NOOP("tkConstants", "The %1 driver is not available.");
        const char *const DATABASE_DRIVER_1_NOT_AVAILABLE_DETAIL = QT_TRANSLATE_NOOP("tkConstants", "The %1 database driver is not installed or can not be found. Please report this error to the developper.");
        const char *const DATABASE_1_SCHEMA_ERROR = QT_TRANSLATE_NOOP("tkConstants", "Database %1: Schema error.");
        const char *const CHANGE_PASSWORD = QT_TRANSLATE_NOOP("tkConstants", "Change password");

        // Time and periods
        const char* const DURATION   = QT_TRANSLATE_NOOP("tkConstants", "Duration");
        const char* const SECOND_S    = QT_TRANSLATE_NOOP("tkConstants", "second(s)");
        const char* const MINUTE_S    = QT_TRANSLATE_NOOP("tkConstants", "minute(s)");
        const char* const HOUR_S      = QT_TRANSLATE_NOOP("tkConstants", "hour(s)");
        const char* const DAY_S       = QT_TRANSLATE_NOOP("tkConstants", "day(s)");
        const char* const WEEK_S      = QT_TRANSLATE_NOOP("tkConstants", "week(s)");
        const char* const MONTH_S     = QT_TRANSLATE_NOOP("tkConstants", "month(s)");
        const char* const QUARTER_S   = QT_TRANSLATE_NOOP("tkConstants", "quarter(s)");
        const char* const YEAR_S      = QT_TRANSLATE_NOOP("tkConstants", "year(s)");
        const char* const DECADE_S    = QT_TRANSLATE_NOOP("tkConstants", "decade(s)");

        const char* const SECOND    = QT_TRANSLATE_NOOP("tkConstants", "second");
        const char* const MINUTE    = QT_TRANSLATE_NOOP("tkConstants", "minute");
        const char* const HOUR      = QT_TRANSLATE_NOOP("tkConstants", "hour");
        const char* const DAY       = QT_TRANSLATE_NOOP("tkConstants", "day");
        const char* const WEEK      = QT_TRANSLATE_NOOP("tkConstants", "week");
        const char* const MONTH     = QT_TRANSLATE_NOOP("tkConstants", "month");
        const char* const QUARTER   = QT_TRANSLATE_NOOP("tkConstants", "quarter");
        const char* const YEAR      = QT_TRANSLATE_NOOP("tkConstants", "year");
        const char* const DECADE    = QT_TRANSLATE_NOOP("tkConstants", "decade");

        const char* const SECONDS    = QT_TRANSLATE_NOOP("tkConstants", "seconds");
        const char* const MINUTES    = QT_TRANSLATE_NOOP("tkConstants", "minutes");
        const char* const HOURS      = QT_TRANSLATE_NOOP("tkConstants", "hours");
        const char* const DAYS       = QT_TRANSLATE_NOOP("tkConstants", "days");
        const char* const WEEKS      = QT_TRANSLATE_NOOP("tkConstants", "weeks");
        const char* const MONTHS     = QT_TRANSLATE_NOOP("tkConstants", "months");
        const char* const QUARTERS   = QT_TRANSLATE_NOOP("tkConstants", "quarters");
        const char* const YEARS      = QT_TRANSLATE_NOOP("tkConstants", "years");
        const char* const DECADES    = QT_TRANSLATE_NOOP("tkConstants", "decades");

        // Day
        const char* const WAKEUPTIME           = QT_TRANSLATE_NOOP("tkConstants", "wake up");
        const char* const BREAKFAST            = QT_TRANSLATE_NOOP("tkConstants", "breakfast");
        const char* const TENOCLOCKBRUNCH      = QT_TRANSLATE_NOOP("tkConstants", "mid-morning");
        const char* const MORNING              = QT_TRANSLATE_NOOP("tkConstants", "morning");
        const char* const MIDDAY               = QT_TRANSLATE_NOOP("tkConstants", "midday");
        const char* const LUNCH                = QT_TRANSLATE_NOOP("tkConstants", "lunch");
        const char* const AFTERNOON            = QT_TRANSLATE_NOOP("tkConstants", "afternoon");
        const char* const TEATIME              = QT_TRANSLATE_NOOP("tkConstants", "mid-afternoon");
        const char* const DINER                = QT_TRANSLATE_NOOP("tkConstants", "dinner");
        const char* const EVENING              = QT_TRANSLATE_NOOP("tkConstants", "evening");
        const char* const BEDTIME              = QT_TRANSLATE_NOOP("tkConstants", "bedtime");

        // Meal time relations
        const char* const NOMEALRELATION      = QT_TRANSLATE_NOOP("tkConstants", "no meal relation");
        const char* const DURINGMEAL          = QT_TRANSLATE_NOOP("tkConstants", "during meal");
        const char* const AFTERMEAL           = QT_TRANSLATE_NOOP("tkConstants", "after meal");
        const char* const BEFOREMEAL          = QT_TRANSLATE_NOOP("tkConstants", "before meal");
        const char* const NOTDURINGMEAL       = QT_TRANSLATE_NOOP("tkConstants", "not during meal");
        const char* const ONLYIFMEAL          = QT_TRANSLATE_NOOP("tkConstants", "only if meal");
        const char* const SEPARATEDFROMFOOD   = QT_TRANSLATE_NOOP("tkConstants", "separated from food (1-2 hours)");
        const char* const SUGGESTEDWITHFOOD   = QT_TRANSLATE_NOOP("tkConstants", "suggested with food");
        const char* const WITHORWITHOUTFOOD   = QT_TRANSLATE_NOOP("tkConstants", "with or without food");

        // Splashscreen messages
        const char* const USING_LIBRARY_1                   = QT_TRANSLATE_NOOP("tkConstants", "Using library : %1");
        const char* const STARTING_APPLICATION_AT_1         = QT_TRANSLATE_NOOP("tkConstants", "Starting application at %1");
        const char* const INITIALIZING_TRANSLATIONS         = QT_TRANSLATE_NOOP("tkConstants", "Initializing Translations...");
        const char* const INITIALIZING_LIBRARIES            = QT_TRANSLATE_NOOP("tkConstants", "Initializing libraries..." );
        const char* const NEED_LICENSE_AGREEMENT            = QT_TRANSLATE_NOOP("tkConstants", "Needed Licence Agreement...");
        const char* const CHECKING_DIRECTORY_STRUCTURE      = QT_TRANSLATE_NOOP("tkConstants", "Checking directory structure...");
        const char* const INITIALIZING_DEFAULTS_PARAMS      = QT_TRANSLATE_NOOP("tkConstants", "Initializing Default Parameters..." );
        const char* const INITIALIZING_PLUGINS              = QT_TRANSLATE_NOOP("tkConstants", "Initializing Plugins...");
        const char* const FIRST_TIME_RUNNING                = QT_TRANSLATE_NOOP("tkConstants", "First time running");
        const char* const FEATURE_NOT_IMPLEMENTED           = QT_TRANSLATE_NOOP("tkConstants", "This feature is not yet implemented.");
        const char* const RAISING_APPLICATION               = QT_TRANSLATE_NOOP("tkConstants", "Raising Application...");
        const char* const CHECKUPDATE                       = QT_TRANSLATE_NOOP("tkConstants", "Check for update");
        const char* const CHECKING_UPDATES                  = QT_TRANSLATE_NOOP("tkConstants", "Checking for updates...");
        const char* const INITIALIZATING_DATABASES          = QT_TRANSLATE_NOOP("tkConstants", "Initializing databases...");

        // Widely used words
        const char* const TYPE                               = QT_TRANSLATE_NOOP("tkConstants", "Type");
        const char* const STATUS                             = QT_TRANSLATE_NOOP("tkConstants", "Status");
        const char* const ICD10                              = QT_TRANSLATE_NOOP("tkConstants", "ICD10");
        const char* const NOW                                = QT_TRANSLATE_NOOP("tkConstants", "now");
        const char* const LOW                                = QT_TRANSLATE_NOOP("tkConstants", "low");
        const char* const MEDIUM                             = QT_TRANSLATE_NOOP("tkConstants", "medium");
        const char* const HIGH                               = QT_TRANSLATE_NOOP("tkConstants", "high");
        const char* const MODIFY                             = QT_TRANSLATE_NOOP("tkConstants", "modify");

        // Settings related messages
        const char* const SETTINGS_RECOVERED                = QT_TRANSLATE_NOOP("tkConstants", "Settings recovered" );
        const char* const LOADING_SETTINGS                  = QT_TRANSLATE_NOOP("tkConstants", "Reading settings file");
        const char* const SAVING_SETTINGS                   = QT_TRANSLATE_NOOP("tkConstants", "Settings recovered" );
        const char* const CREATING_DEFAULT_SETTINGS_FOR_1   = QT_TRANSLATE_NOOP("tkConstants", "Creating default settings for %1");
        const char* const SHOW_DETAILS                      = QT_TRANSLATE_NOOP("tkConstants", "Show details");
        const char* const MORE_INFORMATIONS                 = QT_TRANSLATE_NOOP("tkConstants", "More informations");
        const char* const HIDE_EXTRA_INFORMATIONS           = QT_TRANSLATE_NOOP("tkConstants", "Hide extra informations");

        // Users related message
        const char* const WELCOME_USER                      = QT_TRANSLATE_NOOP("tkConstants", "Welcome to FreeMedForms, you are now connected.");
        const char* const CONNECTED_AS_1                    = QT_TRANSLATE_NOOP("tkConstants", "Connected as\n  %1");
        const char* const NO_USER_CONNECTED                 = QT_TRANSLATE_NOOP("tkConstants", "No user connected.");
        const char* const USER                              = QT_TRANSLATE_NOOP("tkConstants", "User");
        const char* const USERS                             = QT_TRANSLATE_NOOP("tkConstants", "Users");
        const char* const USER_DATAS                        = QT_TRANSLATE_NOOP("tkConstants", "User datas");
        const char* const RIGHTS                            = QT_TRANSLATE_NOOP("tkConstants", "Rights");
        const char* const USER_RIGHTS                       = QT_TRANSLATE_NOOP("tkConstants", "User rights");
        const char* const RIGHTS_MEDICAL                    = QT_TRANSLATE_NOOP("tkConstants", "Medical rights");
        const char* const RIGHTS_ADMINISTRATIVE             = QT_TRANSLATE_NOOP("tkConstants", "Administrative rights");
        const char* const RIGHTS_USERMANAGER                = QT_TRANSLATE_NOOP("tkConstants", "User manager rights");
        const char* const RIGHTS_DRUGS                      = QT_TRANSLATE_NOOP("tkConstants", "Drug rights");

        const char* const SAVE_AND_RESTORE                  = QT_TRANSLATE_NOOP("tkConstants", "Save and restore");

        // Files and Paths
        const char* const PATH                               = QT_TRANSLATE_NOOP("tkConstants", "Path");
        const char* const PATH_1_DOESNOT_EXISTS              = QT_TRANSLATE_NOOP("tkConstants", "Path %1 does not exist");
        const char* const PATH_1_ISNOT_EMPTY                 = QT_TRANSLATE_NOOP("tkConstants", "Path %1 is not empty");
        const char* const PATH_1_CONTAINS_DIRS               = QT_TRANSLATE_NOOP("tkConstants", "Path %1 contains dirs");
        const char* const PATH_1_CANNOT_BE_REMOVED           = QT_TRANSLATE_NOOP("tkConstants", "Path %1 can not be removed");
        const char* const PATH_1_CANNOT_BE_CREATED           = QT_TRANSLATE_NOOP("tkConstants", "Path %1 can not be created");
        const char* const CREATE_DIR_1                       = QT_TRANSLATE_NOOP("tkConstants", "Creating dir %1");
        const char* const FILE_1_DOESNOT_EXISTS              = QT_TRANSLATE_NOOP("tkConstants", "File %1 does not exist");
        const char* const FILE_1_ISNOT_READABLE              = QT_TRANSLATE_NOOP("tkConstants", "File %1 is not readable");
        const char* const FILE_1_CANNOT_BE_REMOVED           = QT_TRANSLATE_NOOP("tkConstants", "File %1 can not be removed");
        const char* const FILE_1_LOADED                      = QT_TRANSLATE_NOOP("tkConstants", "File %1 loaded");
        const char* const FILE_1_ISEMPTY                     = QT_TRANSLATE_NOOP("tkConstants", "File %1 is empty");

        const char* const OPEN_FILE                          = QT_TRANSLATE_NOOP("tkConstants", "Open file");
        const char* const OPENING_FILE_1                     = QT_TRANSLATE_NOOP("tkConstants", "Opening file %1");
        const char* const SAVING_FILE_1                      = QT_TRANSLATE_NOOP("tkConstants", "Saving file %1");


        const char* const XML_WRONG_NUMBER_OF_TAG_1          = QT_TRANSLATE_NOOP("tkConstants", "Wrong number of tags (%1)");
        const char* const XML_WRONG_ROOT_TAG_1_2             = QT_TRANSLATE_NOOP("tkConstants", "Wrong root tag %1 %2.");
        const char* const XML_DOCUMENT_TYPE_MISMATCH         = QT_TRANSLATE_NOOP("tkConstants", "Document type name mismatch.");

        // Messages and errors
        const char* const INFORMATIONS                       = QT_TRANSLATE_NOOP("tkConstants", "Informations");
        const char* const CANCEL                             = QT_TRANSLATE_NOOP("tkConstants", "Cancel");
        const char* const CLEAR                              = QT_TRANSLATE_NOOP("tkConstants", "Clear");
        const char* const VERSION                            = QT_TRANSLATE_NOOP("tkConstants", "Version");
        const char* const DESCRIPTION                        = QT_TRANSLATE_NOOP("tkConstants", "Description");
        const char* const AUTHOR                             = QT_TRANSLATE_NOOP("tkConstants", "Author");
        const char* const PROVIDER                           = QT_TRANSLATE_NOOP("tkConstants", "Provider");
        const char* const UNKNOWN                            = QT_TRANSLATE_NOOP("tkConstants", "Unknown");
        const char* const READY                              = QT_TRANSLATE_NOOP("tkConstants", "Ready");
        const char* const AVAILABLE                          = QT_TRANSLATE_NOOP("tkConstants", "Available");
        const char* const UNAVAILABLE                        = QT_TRANSLATE_NOOP("tkConstants", "Unavailable");
        const char* const MESSAGES                           = QT_TRANSLATE_NOOP("tkConstants", "Messages");
        const char* const LOGGED_MESSAGES                    = QT_TRANSLATE_NOOP("tkConstants", "Logged Messages");
        const char* const ERRORS                             = QT_TRANSLATE_NOOP("tkConstants", "Errors");
        const char* const LOGGED_ERRORS                      = QT_TRANSLATE_NOOP("tkConstants", "Logged Errors");
        const char* const INFORMATIVE_MESSAGE_1              = QT_TRANSLATE_NOOP("tkConstants", "Informative message : %1");
        const char* const STARTING_TASK_1                    = QT_TRANSLATE_NOOP("tkConstants", "Starting task : %1");
        const char* const POST_TO_1                          = QT_TRANSLATE_NOOP("tkConstants", "posting message to : %1");
        const char* const ERROR_1_OCCURED_WHILE_2            = QT_TRANSLATE_NOOP("tkConstants", "Error %1 occured while %2");
        const char* const ERROR_1_LINE_2_COLUMN_3            = QT_TRANSLATE_NOOP("tkConstants", "Error %1 : line %2, column %3");
        const char* const CAN_NOT_CREATE_1_ALREADY_EXISTS    = QT_TRANSLATE_NOOP("tkConstants", "Can not create %1 : it already exists");
        const char* const CAN_NOT_RETURN_1_DOESNOT_EXISTS    = QT_TRANSLATE_NOOP("tkConstants", "Can not return %1 : it does not exists");
        const char* const CAN_NOT_CREATE_1_NOPARENT          = QT_TRANSLATE_NOOP("tkConstants", "Can not create %1 : no parent founded");
        const char* const _1_ISNOT_AVAILABLE                 = QT_TRANSLATE_NOOP("tkConstants", "%1 is not avaible");
        const char* const _1_ISNOT_AVAILABLE_CANNOTBE_CREATED = QT_TRANSLATE_NOOP("tkConstants", "%1 is not avaible and can not be created");
        const char* const _1_ARENOT_AVAILABLE                = QT_TRANSLATE_NOOP("tkConstants", "%1 are not avaible");
        const char* const SETTING_1_PATH_TO_2                = QT_TRANSLATE_NOOP("tkConstants", "Setting %1 path to %2");
        const char* const ERROR_1_FROM_DATABASE_2            = QT_TRANSLATE_NOOP("tkConstants", "Error %1 from database %2");
        const char* const NO_OBJECT_TO_TRANSLATE             = QT_TRANSLATE_NOOP("tkConstants", "No object to translate");
        const char* const NO_1_FOUND                         = QT_TRANSLATE_NOOP("tkConstants", "No %1 found.");
        const char* const PLEASE_DESCRIBE_PROBLEM            = QT_TRANSLATE_NOOP("tkConstants", "Please describe your problem.");
        const char* const YES                                = QT_TRANSLATE_NOOP("tkConstants", "Yes");
        const char* const NO                                 = QT_TRANSLATE_NOOP("tkConstants", "No");
        const char* const GENERAL                            = QT_TRANSLATE_NOOP("tkConstants", "General");
        const char* const GENERAL_PREFERENCES                = QT_TRANSLATE_NOOP("tkConstants", "General preferences");
        const char* const APPLICATION_FAILURE                = QT_TRANSLATE_NOOP("tkConstants", "Fatal error : Application failure. Please restart application.");

        const char* const CONNECTED_TO_DATABASE_1_DRIVER_2   = QT_TRANSLATE_NOOP("tkConstants", "Connected to database %1 with driver %2");
        const char* const TRYING_TO_CREATE_1_PLACE_2         = QT_TRANSLATE_NOOP("tkConstants", "Trying to create %1 \nPlace : %2");
        const char* const UNABLE_TO_OPEN_DATABASE_1_ERROR_2  = QT_TRANSLATE_NOOP("tkConstants", "Unable to open database %1 : Error : %2");
        const char* const UNABLE_TO_SAVE_DATA_IN_DATABASE_1  = QT_TRANSLATE_NOOP("tkConstants", "Unable to save data in database %1.");
        const char* const DATABASE_1_CORRECTLY_CREATED       = QT_TRANSLATE_NOOP("tkConstants", "Database %1 correctly created");
        const char* const DATABASE_USER_1_CORRECTLY_CREATED  = QT_TRANSLATE_NOOP("tkConstants", "User %1 correctly created");
        const char* const DATABASE_1_CANNOT_BE_CREATED_ERROR_2 = QT_TRANSLATE_NOOP("tkConstants", "Database %1 can not be created. Error: %2.");

        // Sender and Updater
        const char* const START_MESSAGE_SENDING              = QT_TRANSLATE_NOOP("tkConstants", "Starting to send message.");
        const char* const MESSAGE_SENDED_OK                  = QT_TRANSLATE_NOOP("tkConstants", "Message sended without error.");
        const char* const MESSAGE_NOT_SENDED                 = QT_TRANSLATE_NOOP("tkConstants", "An error occured, message was not sended.");
        const char* const CHECKING_UPDATE_FROM_1             = QT_TRANSLATE_NOOP("tkConstants", "Checking for updates from %1");
        const char* const VERSION_UPTODATE                   = QT_TRANSLATE_NOOP("tkConstants", "Version up to date");
        const char* const CURRENT_VERSION                    = QT_TRANSLATE_NOOP("tkConstants", "Current version :");
        const char* const LATEST_VERSION                     = QT_TRANSLATE_NOOP("tkConstants", "Lastest version :");
        const char* const UPDATE_AVAILABLE                   = QT_TRANSLATE_NOOP("tkConstants", "Update Available");

        // Team functions
        const char* const TEAM_CREATOR = QT_TRANSLATE_NOOP("tkConstants", "Creator and Main Developper");
        const char* const TEAM_DEVELOPPERS_CONTRIBUTORS = QT_TRANSLATE_NOOP("tkConstants", "Developpers and Contributors");
        const char* const TEAM_DEBUGGER_ADVISOR = QT_TRANSLATE_NOOP("tkConstants", "Debugger advisor");
        const char* const TEAM_DEVELOPPER_AND_DEBUGGER = QT_TRANSLATE_NOOP("tkConstants", "Developper and debugger advisor");
        const char* const TEAM_PACKAGER = QT_TRANSLATE_NOOP("tkConstants", "Packager");
        const char* const TEAM_PACKAGER_FOR_1 = QT_TRANSLATE_NOOP("tkConstants", "Packager for %1");
        const char* const TEAM_DRUG_DATABASE_MANAGER = QT_TRANSLATE_NOOP("tkConstants", "Drugs Database Management");
        const char* const TEAM_MEDICAL_ADVISORS = QT_TRANSLATE_NOOP("tkConstants", "Medical advisors");
        const char* const TEAM_MEDICAL_ADVISOR = QT_TRANSLATE_NOOP("tkConstants", "Medical advisor");
	const char* const TEAM_WEBMASTER = QT_TRANSLATE_NOOP("tkConstants", "Webmaster");
	const char* const TEAM_TRANSLATORS = QT_TRANSLATE_NOOP("tkConstants", "Translators");

        // Build related
        const char* const BUILD_INFORMATIONS = QT_TRANSLATE_NOOP("tkConstants", "Build informations");
        const char* const BUILD_VERSION_INFORMATIONS = QT_TRANSLATE_NOOP("tkConstants", "Version informations");
        const char* const BUILD_DATE_TIME_1_2 = QT_TRANSLATE_NOOP("tkConstants", "Build on %1 %2");
        const char* const BUILD_VERSION_1 = QT_TRANSLATE_NOOP("tkConstants", "Build version : %1");
        const char* const BUILD_FULLAPP = QT_TRANSLATE_NOOP("tkConstants", "Full application");
        const char* const BUILD_SVNAPP = QT_TRANSLATE_NOOP("tkConstants", "SVN application");
        const char* const BUILD_DEBUG = QT_TRANSLATE_NOOP("tkConstants", "Build in debug mode");
        const char* const BUILD_RELEASE = QT_TRANSLATE_NOOP("tkConstants", "Build in release mode");

        // Print related
        const char* const HEADER = QT_TRANSLATE_NOOP("tkConstants", "Header");
        const char* const FOOTER = QT_TRANSLATE_NOOP("tkConstants", "Footer");
        const char* const WATERMARK = QT_TRANSLATE_NOOP("tkConstants", "Watermark");
        const char* const PRINTING = QT_TRANSLATE_NOOP("tkConstants", "Printing");

        // Agenda related
        const char * const AGENDA = QT_TRANSLATE_NOOP("tkConstants", "Agenda");
        const char * const AGENDA_EVENT = QT_TRANSLATE_NOOP("tkConstants", "Agenda event");
        const char * const AGENDA_EVENTS = QT_TRANSLATE_NOOP("tkConstants", "Agenda events");
        const char * const AGENDA_DATABASE_INFORMATIONS = QT_TRANSLATE_NOOP("tkConstants", "Agenda database informations");
        const char * const NO_AVAILABILITY = QT_TRANSLATE_NOOP("tkConstants", "No availability");
        const char * const ATTENDEE = QT_TRANSLATE_NOOP("tkConstants", "Attendee");
        const char * const ATTENDEES = QT_TRANSLATE_NOOP("tkConstants", "Attendees");
        const char * const USER_DELEGATES = QT_TRANSLATE_NOOP("tkConstants", "User delegate");
        const char * const OWNER = QT_TRANSLATE_NOOP("tkConstants", "Owner");

        // Status
        const char * const ARRIVED = QT_TRANSLATE_NOOP("tkConstants", "Arrived");
        const char * const CANCELLED = QT_TRANSLATE_NOOP("tkConstants", "Cancelled");
        const char * const MISSED = QT_TRANSLATE_NOOP("tkConstants", "Missed");
        const char * const WAITING = QT_TRANSLATE_NOOP("tkConstants", "Waiting");
        const char * const CHANGED = QT_TRANSLATE_NOOP("tkConstants", "Changed");
        const char * const APPROVED = QT_TRANSLATE_NOOP("tkConstants", "Approved");

        // File filters
        const char * const FILE_FILTER_ZIP = QT_TRANSLATE_NOOP("tkConstants", "Zip archive (*zip)");

        // Plugins related
        const char* const PLUGINS_CATEGORY = QT_TRANSLATE_NOOP("tkConstants", "Plugins");

        // Countries
        const char* const COUNTRY_FRANCE      = QT_TRANSLATE_NOOP( "tkConstants", "France" );
        const char* const COUNTRY_GERMANY     = QT_TRANSLATE_NOOP( "tkConstants", "Germany" );
        const char* const COUNTRY_CANADA      = QT_TRANSLATE_NOOP( "tkConstants", "Canada" );

        // Patient related
        const char* const PATIENT_NAME = QT_TRANSLATE_NOOP("tkConstants", "Patient Name");
        const char* const PATIENT      = QT_TRANSLATE_NOOP("tkConstants", "Patient");
        const char* const PATIENTS     = QT_TRANSLATE_NOOP("tkConstants", "Patients");
        const char* const PATIENT_INFORMATION  = QT_TRANSLATE_NOOP("tkConstants", "Patient informations");
        const char* const PATIENT_DATABASE  = QT_TRANSLATE_NOOP("tkConstants", "Patient database");
        const char* const CLEAR_PATIENT_INFOS = QT_TRANSLATE_NOOP("tkConstants", "Clear patient's information");
        const char* const TOGGLE_PATIENT_INFORMATIONS = QT_TRANSLATE_NOOP("tkConstants", "Toggle patient's information");

        // Genders
        const char* const MALE          = QT_TRANSLATE_NOOP("tkConstants", "Male");
        const char* const FEMALE        = QT_TRANSLATE_NOOP("tkConstants", "Female");
        const char* const HERMAPHRODISM = QT_TRANSLATE_NOOP("tkConstants", "Hermaphrodism");

        // Titles && professions
        const char* const MISTER      = QT_TRANSLATE_NOOP("tkConstants", "Mister");
        const char* const MISS        = QT_TRANSLATE_NOOP("tkConstants", "Miss");
        const char* const MADAM       = QT_TRANSLATE_NOOP("tkConstants", "Madam");
        const char* const DOCTOR      = QT_TRANSLATE_NOOP("tkConstants", "Doctor");
        const char* const PROFESSOR   = QT_TRANSLATE_NOOP("tkConstants", "Professor");
        const char* const CAPTAIN     = QT_TRANSLATE_NOOP("tkConstants", "Captain");
        const char* const NURSE       = QT_TRANSLATE_NOOP("tkConstants", "Nurse");
        const char* const SECRETARY   = QT_TRANSLATE_NOOP("tkConstants", "Secretary");
        const char* const CAREGIVER   = QT_TRANSLATE_NOOP("tkConstants", "Caregiver");
        const char* const MEDICAL_STUDENT = QT_TRANSLATE_NOOP("tkConstants", "Medical student");
        const char* const SOFT_ADMIN  = QT_TRANSLATE_NOOP("tkConstants", "Software administrator");

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
const char * const EDITOR_ADDDATE_TEXT  = QT_TRANSLATE_NOOP("tkConstants", "Add current date");
const char * const EDITOR_ADDUSERNAME_TEXT  = QT_TRANSLATE_NOOP("tkConstants", "Add user name");
const char * const EDITOR_ADDPATIENTNAME_TEXT  = QT_TRANSLATE_NOOP("tkConstants", "Add patient name");
const char * const EDITOR_ADDTEXTMENU_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "Add text");

// ShortCuts
//: ShortCut for "Print prescription"
const char* const   K_PRINT_PRESCRIPTION = QT_TRANSLATE_NOOP("tkConstants", "Ctrl+Shift+P");


// For multilingual class template
const char* const ALL_LANGUAGE = "xx";
const char* const ALL_LANGUAGE_TEXT = QT_TRANSLATE_NOOP("tkConstants", "All languages");
const char* const ALL_OTHER_LANGUAGES_TEXT = QT_TRANSLATE_NOOP("tkConstants", "All other languages");


} // end Constants


namespace ConstantTranslations {

TRUTILS_EXPORT QString tkTr(const char* toTr, const int plurials=1);
TRUTILS_EXPORT QStringList periods();
TRUTILS_EXPORT QString period(int id);
TRUTILS_EXPORT QString periodPlurialForm(int id, int nb = 1, const QString &defaultValue = QString::null);
TRUTILS_EXPORT QStringList dailySchemeList();
TRUTILS_EXPORT QStringList dailySchemeXmlTagList();
TRUTILS_EXPORT QString dailyScheme(const int dailySchemeFlag);
TRUTILS_EXPORT QStringList preDeterminedAges();
TRUTILS_EXPORT QStringList mealTime();
TRUTILS_EXPORT QString mealTime(const int index);

TRUTILS_EXPORT QStringList genders();
TRUTILS_EXPORT QStringList titles();

} // end  ConstantTranslations

} // end TranslationUtils

#endif // TKCONSTANTTRANSLATIONS_H
