/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef TKCONSTANTTRANSLATIONS_H
#define TKCONSTANTTRANSLATIONS_H

#include <tkExporter.h>

#include <QString>
#include <QStringList>
#include <QCoreApplication>


Q_TK_BEGIN_CONSTANTS
        const char* const CONSTANTS_TR_CONTEXT = "tkConstants";

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
        NoRelationWiyhMeal = 0,
        DuringMeal,
        BeforeMeal,
        AfterMeal,
        OutsideMeal
    };


    enum DailyScheme
    {
        Undefined    = 0x00000000,
        Morning      = 0x00000001,
        Afternoon    = 0x00000010,
        TeaTime      = 0x00000100,
        Evening      = 0x00001000,
        BedTime      = 0x00010000
    };
    Q_DECLARE_FLAGS( DailySchemes, DailyScheme );
}
Q_DECLARE_OPERATORS_FOR_FLAGS( Time::DailySchemes)

        // Menus translations
        const char * const  M_FILE_TEXT          = QT_TRANSLATE_NOOP("tkConstants", "File");               /*!< \brief Default text for menu File  \ingroup constants_actionmanager */
        const char * const  M_EDIT_TEXT          = QT_TRANSLATE_NOOP("tkConstants", "Edit");               /*!< \brief Default text for menu Edit  \ingroup constants_actionmanager */
        const char * const  M_EDIT_SELECT_TEXT   = QT_TRANSLATE_NOOP("tkConstants", "Selection");          /*!< \brief Default text for menu Selection  \ingroup constants_actionmanager */
        const char * const  M_EDIT_LIST_TEXT     = QT_TRANSLATE_NOOP("tkConstants", "List");               /*!< \brief Default text for menu List  \ingroup constants_actionmanager */
        const char * const  M_FORMAT_TEXT        = QT_TRANSLATE_NOOP("tkConstants", "Format");             /*!< \brief Default text for menu Format  \ingroup constants_actionmanager */
        const char * const  M_TOOLS_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "Tools");              /*!< \brief Default text for menu Tools  \ingroup constants_actionmanager */
        const char * const  M_PLUGINS_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "Plugins");            /*!< \brief Default text for menu Plugins  \ingroup constants_actionmanager */
        const char * const  M_CONFIGURATION_TEXT = QT_TRANSLATE_NOOP("tkConstants", "Configuration");      /*!< \brief Default text for menu Configuration  \ingroup constants_actionmanager */
        const char * const  M_TOOLBARS_TEXT      = QT_TRANSLATE_NOOP("tkConstants", "Toolbars");           /*!< \brief Default text for menu Toolbars  \ingroup constants_actionmanager */
        const char * const  M_LANGUAGES_TEXT     = QT_TRANSLATE_NOOP("tkConstants", "Languages");          /*!< \brief Default text for menu Languages  \ingroup constants_actionmanager */
        const char * const  M_INTERPRETOR_TEXT   = QT_TRANSLATE_NOOP("tkConstants", "Interpretor");        /*!< \brief Default text for menu Interpretor  \ingroup constants_actionmanager */
        const char * const  M_ABOUT_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "About");              /*!< \brief Default text for menu About  \ingroup constants_actionmanager */
        const char * const  M_UPDATE_TEXT        = QT_TRANSLATE_NOOP("tkConstants", "Update available");   /*!< \brief Default text for menu Update  \ingroup constants_actionmanager */

        // Actions translations
        const char * const  FILENEW_TEXT          = QT_TRANSLATE_NOOP("tkConstants", "New");    /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  FILEOPEN_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "Open");    /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  FILESAVE_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "Save");    /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  FILESAVEAS_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "Save as");    /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  FILEPRINT_TEXT        = QT_TRANSLATE_NOOP("tkConstants", "Print...");    /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  FILEEXIT_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "Exit");    /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  CLOSE_TEXT            = QT_TRANSLATE_NOOP("tkConstants", "Close");    /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  EDITCOPY_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "Paste");      /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  EDITCUT_TEXT          = QT_TRANSLATE_NOOP("tkConstants", "Cut");        /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  EDITPASTE_TEXT        = QT_TRANSLATE_NOOP("tkConstants", "Paste");      /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  EDITUNDO_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "Undo");       /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  EDITREDO_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "Redo");       /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  EDITSELECTALL_TEXT    = QT_TRANSLATE_NOOP("tkConstants", "Select All"); /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  EDITDESESLECT_TEXT    = QT_TRANSLATE_NOOP("tkConstants", "Deselect");   /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  NEXT_TEXT             = QT_TRANSLATE_NOOP("tkConstants", "Next");       /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  PREVIOUS_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "Previous");   /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  HOME_TEXT             = QT_TRANSLATE_NOOP("tkConstants", "Home");       /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  LISTADD_TEXT          = QT_TRANSLATE_NOOP("tkConstants", "Add to list");    /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  LISTREMOVE_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "Remove of list"); /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  LISTMOVEUP_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "Move item up");   /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  LISTMOVEDOWN_TEXT     = QT_TRANSLATE_NOOP("tkConstants", "Move item down"); /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  FORMATBOLD_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "Bold");           /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  FORMATITALIC_TEXT     = QT_TRANSLATE_NOOP("tkConstants", "Italic");         /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  FORMATUNDERLINE_TEXT  = QT_TRANSLATE_NOOP("tkConstants", "Underline");      /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  FORMATSTRIKE_TEXT     = QT_TRANSLATE_NOOP("tkConstants", "Strike");         /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  FORMATBIGGER_TEXT     = QT_TRANSLATE_NOOP("tkConstants", "Bigger");         /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  FORMATSMALLER_TEXT    = QT_TRANSLATE_NOOP("tkConstants", "Smaller");        /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  FORMATFONT_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "Font format");    /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  FORMATTABLE_TEXT      = QT_TRANSLATE_NOOP("tkConstants", "Table format");    /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  FORMATIMAGE_TEXT      = QT_TRANSLATE_NOOP("tkConstants", "Image format");    /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  ALIGNCENTER_TEXT      = QT_TRANSLATE_NOOP("tkConstants", "Center");     /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  ALIGNLEFT_TEXT        = QT_TRANSLATE_NOOP("tkConstants", "Left");       /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  ALIGNRIGHT_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "Right");      /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  ALIGNJUSTIFY_TEXT     = QT_TRANSLATE_NOOP("tkConstants", "Justify");    /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  ABOUT_TEXT            = QT_TRANSLATE_NOOP("tkConstants", "About");             /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  ABOUTQT_TEXT          = QT_TRANSLATE_NOOP("tkConstants", "About Qt");          /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  ABOUTFORM_TEXT        = QT_TRANSLATE_NOOP("tkConstants", "About this form");   /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  APPLICATIONHELP_TEXT  = QT_TRANSLATE_NOOP("tkConstants", "Help");              /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  DEBUGHELPER_TEXT      = QT_TRANSLATE_NOOP("tkConstants", "Debugging tools");   /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  HELPTEXTTOGGLER_TEXT  = QT_TRANSLATE_NOOP("tkConstants", "Display/Hide form's help text");    /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  HELP_TEXT             = QT_TRANSLATE_NOOP("tkConstants", "Help");    /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  INTERPRETOR_GENERAL_TEXT  = QT_TRANSLATE_NOOP("tkConstants", "Interpretor");    /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  USERMANAGER_TEXT      = QT_TRANSLATE_NOOP("tkConstants", "User manager");    /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  PREFERENCES_TEXT      = QT_TRANSLATE_NOOP("tkConstants", "Preferences");    /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  VIEWUPDATE_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "View update informations");    /*!< \brief Default action text  \ingroup constants_actionmanager */
        const char * const  PRINT_PREVIEW_TEXT    = QT_TRANSLATE_NOOP("tkConstants", "Print Preview");
        const char * const  TRANSLATORS_TEXT      = QT_TRANSLATE_NOOP("tkConstants", "Translators");
        const char * const  FULLSCREEN_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "Toogle full screen");

        // Drugs
        const char* const INTAKES    = QT_TRANSLATE_NOOP("tkConstants", "intake(s)");

        // Time and periods
        const char* const SECONDS    = QT_TRANSLATE_NOOP("tkConstants", "second(s)");
        const char* const MINUTES    = QT_TRANSLATE_NOOP("tkConstants", "minute(s)");
        const char* const HOURS      = QT_TRANSLATE_NOOP("tkConstants", "hour(s)");
        const char* const DAYS       = QT_TRANSLATE_NOOP("tkConstants", "day(s)");
        const char* const WEEKS      = QT_TRANSLATE_NOOP("tkConstants", "week(s)");
        const char* const MONTHS     = QT_TRANSLATE_NOOP("tkConstants", "month(s)");
        const char* const QUARTERS   = QT_TRANSLATE_NOOP("tkConstants", "quarter(s)");
        const char* const YEARS      = QT_TRANSLATE_NOOP("tkConstants", "years(s)");
        const char* const DECADES    = QT_TRANSLATE_NOOP("tkConstants", "decade(s)");

        // Day
        const char* const WAKEUPTIME           = QT_TRANSLATE_NOOP("tkConstants", "wake up");
        const char* const MORNING              = QT_TRANSLATE_NOOP("tkConstants", "morning");
        const char* const TENOCLOCK            = QT_TRANSLATE_NOOP("tkConstants", "ten o'clock");
        const char* const AFTERNOON            = QT_TRANSLATE_NOOP("tkConstants", "afternoon");
        const char* const TEATIME              = QT_TRANSLATE_NOOP("tkConstants", "tea time");
        const char* const EVENING              = QT_TRANSLATE_NOOP("tkConstants", "evening");
        const char* const BEDTIME              = QT_TRANSLATE_NOOP("tkConstants", "bedtime");

        // Meal time relations
        const char* const NOMEALRELATION      = QT_TRANSLATE_NOOP("tkConstants", "no meal realation");
        const char* const DURINGMEAL          = QT_TRANSLATE_NOOP("tkConstants", "during meal");
        const char* const AFTERMEAL           = QT_TRANSLATE_NOOP("tkConstants", "after meal");
        const char* const BEFOREMEAL          = QT_TRANSLATE_NOOP("tkConstants", "before meal");
        const char* const NOTDURINGMEAL       = QT_TRANSLATE_NOOP("tkConstants", "not during meal");

        // Messages and errors
        const char* const UNKNOWN                            = QT_TRANSLATE_NOOP("tkConstants", "Unknown");
        const char* const READY                              = QT_TRANSLATE_NOOP("tkConstants", "Ready");
        const char* const OPEN_FILE                          = QT_TRANSLATE_NOOP("tkConstants", "Open File");
        const char* const FILE_1_LOADED                      = QT_TRANSLATE_NOOP("tkConstants", "File %1 loaded");
        const char* const MESSAGE_SENDED_OK                  = QT_TRANSLATE_NOOP("tkConstants", "Message sended without error.");
        const char* const MESSAGE_NOT_SENDED                 = QT_TRANSLATE_NOOP("tkConstants", "An error occured, message was not sended.");
        const char* const INFORMATIVE_MESSAGE_1              = QT_TRANSLATE_NOOP("tkConstants", "Informative message : %1");
        const char* const STARTING_TASK_1                    = QT_TRANSLATE_NOOP("tkConstants", "Starting task : %1");
        const char* const POST_TO_1                          = QT_TRANSLATE_NOOP("tkConstants", "posting message to : %1");
        const char* const ERROR_1_OCCURED_WHILE_2            = QT_TRANSLATE_NOOP("tkConstants", "Error %1 occured while %2");
        const char* const CAN_NOT_CREATE_1_ALREADY_EXISTS    = QT_TRANSLATE_NOOP("tkConstants", "Can not create %1 : it already exists");
        const char* const CAN_NOT_RETURN_1_DOESNOT_EXISTS    = QT_TRANSLATE_NOOP("tkConstants", "Can not return %1 : it does not exists");
        const char* const CAN_NOT_CREATE_1_NOPARENT          = QT_TRANSLATE_NOOP("tkConstants", "Can not create %1 : no parent founded");
        const char* const _1_ISNOT_AVAILABLE                 = QT_TRANSLATE_NOOP("tkConstants", "%1 is not avaible");
        const char* const _1_ISNOT_AVAILABLE_CANNOTBE_CREATED = QT_TRANSLATE_NOOP("tkConstants", "%1 is not avaible and can not be created");
        const char* const _1_ARENOT_AVAILABLE                = QT_TRANSLATE_NOOP("tkConstants", "%1 are not avaible");
        const char* const PATH_1_DOESNOT_EXISTS              = QT_TRANSLATE_NOOP("tkConstants", "Path %1 does not exist");
        const char* const FILE_1_DOESNOT_EXISTS              = QT_TRANSLATE_NOOP("tkConstants", "File %1 does not exist");
        const char* const FILE_1_ISNOT_READABLE              = QT_TRANSLATE_NOOP("tkConstants", "File %1 is not readable");
        const char* const SETTING_1_PATH_TO_2                = QT_TRANSLATE_NOOP("tkConstants", "Setting %1 path to %2");
        const char* const CHECKING_UPDATE_FROM_1             = QT_TRANSLATE_NOOP("tkConstants", "Checking for updates from %1");
        const char* const VERSION_UPTODATE                   = QT_TRANSLATE_NOOP("tkConstants", "Version up to date");
        const char* const CURRENT_VERSION                    = QT_TRANSLATE_NOOP("tkConstants", "Current version :");
        const char* const LATEST_VERSION                     = QT_TRANSLATE_NOOP("tkConstants", "Lastest version :");
        const char* const UPDATE_AVAILABLE                   = QT_TRANSLATE_NOOP("tkConstants", "Update Available");
        const char* const ERROR_1_FROM_DATABASE_2            = QT_TRANSLATE_NOOP("tkConstants", "Error %1 from database %2");
        const char* const CREATING_DEFAULT_SETTINGS_FOR_1    = QT_TRANSLATE_NOOP("tkConstants", "Creating default settings for %1");

        // Team functions
        const char* const CREATOR = QT_TRANSLATE_NOOP("tkConstants", "Creator and Main Developper");
        const char* const DEVELOPPERS_CONTRIBUTORS = QT_TRANSLATE_NOOP("tkConstants", "Developpers and Contributors");
        const char* const DRUG_DATABASE_MANAGER = QT_TRANSLATE_NOOP("tkConstants", "Drugs Database Management");
        const char* const MEDICAL_ADVISOR = QT_TRANSLATE_NOOP("tkConstants", "Medical advisor");

        // Patient related
        const char* const PATIENT_NAME = QT_TRANSLATE_NOOP("tkConstants", "Patient Name");


Q_TK_END_CONSTANTS

Q_TK_USING_CONSTANTS

Q_TK_BEGIN_TRANSLATIONS

Q_TK_EXPORT QString tkTr(const char* toTr, const int plurials=1);
Q_TK_EXPORT QStringList periods();
Q_TK_EXPORT QString period( int id );
Q_TK_EXPORT QStringList dailySchemeList();
Q_TK_EXPORT QStringList dailySchemes(const tkConstants::Time::DailySchemes dailySchemeFlag);
Q_TK_EXPORT tkConstants::Time::DailySchemes toDailyScheme( const QStringList & list );
Q_TK_EXPORT QStringList preDeterminedAges();
Q_TK_EXPORT QStringList mealTime();
Q_TK_EXPORT QString mealTime(const int index);

Q_TK_END_TRANSLATIONS


#endif // TKCONSTANTTRANSLATIONS_H
