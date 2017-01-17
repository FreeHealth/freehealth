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
#ifndef CORE_CONSTANTS_MENUS_H
#define CORE_CONSTANTS_MENUS_H

/**
 * \file constants_menus.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.4.0
 * \date 13 June 2010
 * \brief Regroups Menus, Actions, Groups and Modes identifiers
*/

namespace Core {

namespace Constants {

// Menu's name
const char * const  MENUBAR              = "menuBar";           /*!< \brief Default menubar object name  \ingroup constants_actionmanager */
const char * const  M_GENERAL            = "menuGeneral";          /*!< \brief Default menu File object name  \ingroup constants_actionmanager */
const char * const  M_GENERAL_NEW        = "menuGeneral.New";
const char * const  M_FILE               = "menuFile";          /*!< \brief Default menu File object name  \ingroup constants_actionmanager */
const char * const  M_FILE_NEW           = "menuFile.New";          /*!< \brief Default menu File object name  \ingroup constants_actionmanager */
const char * const  M_FILE_RECENTFILES   = "menuFile.recents";  /*!< \brief Default menu File object name  \ingroup constants_actionmanager */
const char * const  M_TEMPLATES          = "menuTemplates";          /*!< \brief Default menu Edit object name  \ingroup constants_actionmanager */
const char * const  M_EDIT               = "mEdit";          /*!< \brief Default menu Edit object name  \ingroup constants_actionmanager */
const char * const  M_PATIENTS           = "mPatients";          /*!< \brief Default menu Patients object name  \ingroup constants_actionmanager */
const char * const  M_PATIENTS_NAVIGATION= "mPatients.Navigation";
const char * const  M_EDITOR_EDIT        = "m.editor.edition";
const char * const  M_EDITOR_FILE        = "m.editor.file";
const char * const  M_EDIT_SELECT        = "mEdit.Select";   /*!< \brief Default menu Edit/Select object name  \ingroup constants_actionmanager */
const char * const  M_EDIT_LIST          = "mEdit.List";     /*!< \brief Default menu Edit/List object name  \ingroup constants_actionmanager */
const char * const  M_EDIT_TEMPLATES     = "mEdit.Templates";     /*!< \brief Default menu Edit/List object name  \ingroup constants_actionmanager */
const char * const  M_FORMAT             = "menuFormat";        /*!< \brief Default menu Format object name  \ingroup constants_actionmanager */
const char * const  M_FORMAT_FONT        = "menuFormat.Font";  /*!< \brief Default menu Format object name  \ingroup constants_actionmanager */
const char * const  M_FORMAT_PARAGRAPH   = "menuFormat.Paragraph";  /*!< \brief Default menu Format object name  \ingroup constants_actionmanager */
const char * const  M_FORMAT_TABLE       = "menuFormat.Table";  /*!< \brief Default menu Format object name  \ingroup constants_actionmanager */
const char * const  M_FORMAT_IMAGE       = "menuFormat.Image";  /*!< \brief Default menu Format object name  \ingroup constants_actionmanager */
const char * const  M_TOOLS              = "menuTools";         /*!< \brief Default menu Tools object name  \ingroup constants_actionmanager */
const char * const  M_PLUGINS            = "menuPlugins";       /*!< \brief Default menu Plugins object name  \ingroup constants_actionmanager */
const char * const  M_CONFIGURATION      = "menuConfiguration"; /*!< \brief Default menu Configuration object name  \ingroup constants_actionmanager */
const char * const  M_TOOLBARS           = "menuToolbars";      /*!< \brief Default menu Toolbars object name  \ingroup constants_actionmanager */
const char * const  M_LANGUAGES          = "menuLanguages";     /*!< \brief Default menu Languages object name  \ingroup constants_actionmanager */
const char * const  M_INTERPRETOR        = "menuInterpretor";   /*!< \brief Default menu Interpretor object name  \ingroup constants_actionmanager */
const char * const  M_HELP               = "menuHelp";          /*!< \brief Default menu About object name  \ingroup constants_actionmanager */
const char * const  M_HELP_DATABASES     = "menuHelp.DB";       /*!< \brief Default menu About object name  \ingroup constants_actionmanager */
const char * const  M_UPDATE             = "menuUpdate";        /*!< \brief Default menu Update object name  \ingroup constants_actionmanager */

// Groups' name
// file menu groups
const char * const G_GENERAL          = "grGeneral";          /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_GENERAL_FILE     = "grGeneral.File";      /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_GENERAL_NEW      = "grGeneral.New";      /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_GENERAL_RECENTS  = "grGeneral.Recents";  /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_GENERAL_PATIENTS = "grGeneral.Patients";     /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_GENERAL_USERS    = "grGeneral.Users";  /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_GENERAL_EDIT     = "grGeneral.Edit";     /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_GENERAL_PRINT    = "grGeneral.Print";     /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_GENERAL_PLUGINS  = "grGeneral.Plugins";    /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_GENERAL_CONFIG   = "grGeneral.Config";    /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_GENERAL_HELP     = "grGeneral.Help";    /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_GENERAL_OTHERS   = "grGeneral.Others";    /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_GENERAL_EXIT     = "grGeneral.Exit";     /*!< \brief Default group name  \ingroup constants_actionmanager */

// file menu groups
const char * const G_FILE                = "grFile";          /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_FILE_NEW            = "grFile.New";      /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_FILE_OPEN           = "grFile.Open";     /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_FILE_PROJECT        = "grFile.Project";  /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_FILE_SAVE           = "grFile.Save";     /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_FILE_PRINT          = "grFile.Print";    /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_FILE_RECENTS        = "grFile.Recents";  /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_FILE_OTHER          = "grFile.Other";    /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_FILE_CLOSE          = "grFile.Close";    /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_FILE_EXIT           = "grFile.Exit";     /*!< \brief Default group name  \ingroup constants_actionmanager */

// Templates' menu groups
const char * const G_TEMPLATES           = "grTemplates";           /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_TEMPLATES_NEW       = "grTemplates.New";           /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_TEMPLATES_MANAGER   = "grTemplates.Manager";           /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_TEMPLATES_EXTRAS    = "grTemplates.Extras";           /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_TEMPLATES_OTHERS    = "grTemplates.Others";           /*!< \brief Default group name  \ingroup constants_actionmanager */


// edit menu groups
const char * const G_EDIT                = "grEdit";           /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_EDIT_UNDOREDO       = "grEdit.UndoRedo";  /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_EDIT_COPYPASTE      = "grEdit.CopyPaste"; /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_EDIT_SELECT         = "grEdit.SelectAll"; /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_EDIT_LIST           = "grEdit.List";      /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_EDIT_TEMPLATES      = "grEdit.Templates"; /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_EDIT_CATEGORIES     = "grEdit.Categories"; /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_EDIT_ADVANCED       = "grEdit.Advanced";  /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_EDIT_FIND           = "grEdit.Find";      /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_EDIT_EDITOR         = "grEdit.Editor";    /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_EDIT_OTHER          = "grEdit.Other";     /*!< \brief Default group name  \ingroup constants_actionmanager */

// patients menu groups
const char * const G_PATIENTS             = "grPatients";     /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_PATIENT_NAVIGATION   = "grPatientsNavigation";
const char * const G_PATIENT_INFORMATION  = "grPatientsInfo";

// format menu groups
const char * const G_FORMAT              = "grFormat";            /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_FORMAT_FONT         = "grFormat.Font";       /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_FORMAT_FONT_BASE    = "grFormat.FontBase";       /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_FORMAT_FONT_SIZE    = "grFormat.FontSize";       /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_FORMAT_FONT_EXTRAS  = "grFormat.FontExtras";       /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_FORMAT_PARAGRAPH    = "grFormat.Paragraph";  /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_FORMAT_TABLE        = "grFormat.Table";      /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_FORMAT_IMAGE        = "grFormat.Image";      /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_FORMAT_OTHER        = "grFormat.Other";      /*!< \brief Default group name  \ingroup constants_actionmanager */

// window menu groups
//    const char * const G_WINDOW_SIZE         = "grwindow.Size";
//    const char * const G_WINDOW_PANES        = "grwindow.Panes";
//    const char * const G_WINDOW_SPLIT        = "grwindow.Split";
//    const char * const G_WINDOW_CLOSE        = "grwindow.Close";
//    const char * const G_WINDOW_NAVIGATE     = "grwindow.Navigate";
//    const char * const G_WINDOW_NAVIGATE_GROUPS = "grwindow.Navigate.Groups";
//    const char * const G_WINDOW_OTHER        = "grwindow.Other";
//    const char * const G_WINDOW_LIST         = "grwindow.List";
//    const char * const G_WINDOW_FULLSCREEN   = "grwindow.Fullscreen";

// Plugins groups
const char * const G_PLUGINS                =   "grPlugins";
const char * const G_PLUGINS_USERMANAGER    =   "grPlugins.usermanager";
const char * const G_PLUGINS_FORM           =   "grPlugins.forms";
const char * const G_PLUGINS_DRUGS          =   "grPlugins.drugs";
const char * const G_PLUGINS_ACCOUNT        =   "grPlugins.account";
const char * const G_PLUGINS_CALENDAR       =   "grPlugins.calendar";
const char * const G_PLUGINS_PADTOOLS       =   "grPlugins.padTools";
const char * const G_PLUGINS_OTHERS         =   "grPlugins.others";
const char * const G_PLUGINS_PATIENTIMPORT  =   "grPlugins.patientimport";
const char * const G_PLUGINS_RECORDIMPORT   =   "grPlugins.recordimport";

// Interpretor groups
const char * const G_INTERPRETOR_GENERAL = "grInterpretor.General";  /*!< \brief Default group name  \ingroup constants_actionmanager */

// Configuration groups (global)
const char * const G_CONFIGURATION       = "grConfig";               /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_APP_CONFIGURATION   = "grConfig.AppConfig";     /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_TOOLBARS            = "grConfig.Toolbars";      /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_LANGUAGES           = "grConfig.Languages";     /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_PREFERENCES         = "grConfig.Preferences";   /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_DATAPACK            = "grConfig.DataPack";   /*!< \brief Default group name  \ingroup constants_actionmanager */

// Tools groups
const char * const G_TOOLS_GENERAL       = "grTools.General";        /*!< \brief Default group name  \ingroup constants_actionmanager */

// help groups (global)
const char * const G_HELP                = "grHelp";            /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_HELP_HELP           = "grHelp.Help";       /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_HELP_ABOUT          = "grHelp.About";      /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_HELP_DATABASES      = "grHelp.About.DB";      /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_HELP_OTHER          = "grHelp.Other";      /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_HELP_DEBUG          = "grHelp.Debug";      /*!< \brief Default group name  \ingroup constants_actionmanager */

// Update groups
const char * const G_UPDATE              = "grUpdate";            /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_UPDATE_AVAILABLE    = "grUpdate.Available";  /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_UPDATE_TODATE       = "grUpdate.toDate";     /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_UPDATE_OTHER        = "grUpdate.Other";     /*!< \brief Default group name  \ingroup constants_actionmanager */

// Default groups
const char * const G_DEFAULT_ONE         = "gr.One";     /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_DEFAULT_TWO         = "gr.Two";     /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_DEFAULT_THREE       = "gr.Three";     /*!< \brief Default group name  \ingroup constants_actionmanager */


// File Menu Action's name (find icon default into tkTheme.h)
const char * const  A_FILE_NEW               = "aFileNew";    /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FILE_OPEN              = "aFileOpen";   /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FILE_OPENFORM          = "aFileOpenForm";   /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FILE_OPENPRESCRIPTION  = "aFileOpenPrescription";   /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FILE_SAVE              = "aFileSave";   /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FILE_SAVEFORM          = "aFileSaveForm";   /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FILE_SAVEPRESCRIPTION  = "aFileSavePrescription";   /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FILE_SAVEAS            = "aFileSaveAs"; /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FILE_PRINT             = "aFilePrint";  /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FILE_PRINTFORM         = "aFilePrintForm";  /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FILE_PRINTPRESCRIPTION = "aFilePrintPrescription";  /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FILE_PRINTPREVIEW      = "aFilePrintPreview";  /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FILE_LOCKAPPLICATION   = "aFileLockApplication";   /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FILE_EXIT              = "aFileExit";   /*!< \brief Default action object name  \ingroup constants_actionmanager */

// Patient actions
const char * const  A_PATIENT_NEW           = "aPatientNew";   /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_PATIENT_REMOVE        = "aPatientRemove";   /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_PATIENT_VIEWIDENTITY  = "aPatientViewIdentity";   /*!< \brief Default action object name  \ingroup constants_actionmanager */


// Edit Menu Action's name (find icon default into tkTheme.h)
// A_EDIT_COPY << A_EDIT_CUT << A_EDIT_PASTE << A_EDIT_UNDO << A_EDIT_REDO << A_EDIT_SELECTALL << A_EDIT_DESELECT
const char * const  A_EDIT_COPY           = "aEditCopy";      /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_EDIT_CUT            = "aEditCut";       /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_EDIT_PASTE          = "aEditPaste";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_EDIT_UNDO           = "aEditUndo";      /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_EDIT_REDO           = "aEditRedo";      /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_EDIT_SELECTALL      = "aEditSelectAll"; /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_EDIT_DESELECT       = "aEditDeselect";  /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_EDIT_SEARCH         = "aEditSearch";  /*!< \brief Default action object name  \ingroup constants_actionmanager */

// List actions's
const char * const  A_LIST_ADD            = "aListAdd";        /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_LIST_REMOVE         = "aListRemove";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_LIST_MOVEUP         = "aListMoveUp";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_LIST_MOVEDOWN       = "aListMoveDown";   /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_LIST_CLEAR          = "aListClear";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_LIST_SORT           = "aListSort";     /*!< \brief Default action object name  \ingroup constants_actionmanager */

// Templates actions
const char * const  A_TEMPLATE_CREATE     = "aTemplateCreate";        /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_TEMPLATE_TOGGLEVIEW = "aTemplateToggleView";        /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_TEMPLATE_ADD        = "aTemplateAdd";        /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_TEMPLATE_SAVE       = "aTemplateSave";        /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_TEMPLATE_PRINT      = "aTemplatePrint";        /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_CATEGORY_ADD        = "aCategoryAdd";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_TEMPLATE_REMOVE     = "aTemplateRemove";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_CATEGORY_REMOVE     = "aCategoryRemove";   /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_TEMPLATE_EDIT       = "aTemplateEdit";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_CATEGORY_EDIT       = "aCategoryEdit";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_TEMPLATE_LOCK       = "aTemplateLock";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_TEMPLATE_DATABASEINFORMATION = "aTemplateDBInfo";     /*!< \brief Default action object name  \ingroup constants_actionmanager */

// Format Menu Action's name  (find icon default into tkTheme.h)
const char * const  A_FORMAT_BOLD         = "aFormatBold";       /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FORMAT_ITALIC       = "aFormatItalic";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FORMAT_UNDERLINE    = "aFormatUnderline";  /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FORMAT_STRIKE       = "aFormatStrike";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FORMAT_BIGGER       = "aFormatBigger";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FORMAT_SMALLER      = "aFormatSmaller";    /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FORMAT_TYPEWRITERFONT = "aFormatTypeWriterFont";    /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FORMAT_FONT         = "aFormatFont";       /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FORMAT_FONTCOLOR    = "aFormatFontColor";  /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FORMAT_TABLE        = "aFormatTable";      /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FORMAT_IMAGE        = "aFormatImage";      /*!< \brief Default action object name  \ingroup constants_actionmanager */

// Paragraph Format Action's name
const char * const  A_ALIGN_CENTER        = "aAlignCenter";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_ALIGN_LEFT          = "aAlignLeft";       /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_ALIGN_RIGHT         = "aAlignRight";      /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_ALIGN_JUSTIFY       = "aAlignJustify";    /*!< \brief Default action object name  \ingroup constants_actionmanager */

// Tables' actions
const char * const  A_TABLE_ADDTABLE      = "aAddTable";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_TABLE_TABLEPROP     = "aTableProperties";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_TABLE_REMOVETABLE   = "aRemoveTable";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_TABLE_ADDROW        = "aTableAddRow";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_TABLE_REMOVEROW     = "aTableRemoveRow";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_TABLE_ADDCOLUMN     = "aTableAddCol";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_TABLE_REMOVECOLUMN  = "aTableRemoveCol";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_TABLE_MERGECELLS    = "aTableMergeCells";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_TABLE_SPLITCELLS    = "aTableSplitCells";     /*!< \brief Default action object name  \ingroup constants_actionmanager */


// Interpretor Actions
const char * const  A_INTERPRETOR_GENERAL     = "aInterpretor";      /*!< \brief Default action object name  \ingroup constants_actionmanager */

// FreeMedForms Tools Actions
const char * const  A_USERMANAGER     = "aUserManager";      /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_CREATEUSER      = "aCreateUser";      /*!< \brief Default action object name  \ingroup constants_actionmanager */

// About Menu Action's name
const char * const  A_ABOUT              = "aAbout";             /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_APPWEBSITE         = "aWebSite";             /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_ABOUTPLUGINS       = "aPluginsAbout";      /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_ABOUTQT            = "aAboutQt";           /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_ABOUTFORM          = "aAboutThisForm";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_APPLICATIONHELP    = "aApplicationHelp";   /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_DEBUGHELPER        = "aDebugHelper";       /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_BUGREPORT          = "aBugReport";         /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_HELPTEXT_TOGGLER   = "aHelpTextToggler";   /*!< \brief Default action object name  \ingroup constants_actionmanager */

// Configuration Menu Action's name
const char * const  A_PREFERENCES        = "aConfigure";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_APPCONFIGURATOR    = "aAppConfigurator";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_PLUGINS_PREFERENCES= "aPlugPrefs";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_CONFIGURE_MEDINTUX = "aConfMedintux";  /*!< \brief Default action object name  \ingroup constants_actionmanager */

// Update Menu Actions' name
const char * const  A_CHECKUPDATE      = "aCheckUpdate";      /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_VIEWUPDATE       = "aViewUpdate";      /*!< \brief Default action object name  \ingroup constants_actionmanager */

// Modes
// TODO: mode uids should not be stored in the Core plugin
const char * const MODE_PATIENT_SEARCH    = "PatientSearch";
const char * const MODE_PATIENT_FILE      = "central";
const char * const MODE_PATIENT_HISTORY   = "PatientHistory";
const char * const MODE_PATIENT_DRUGS     = "drugs";
const char * const MODE_ACCOUNT           = "account";
const char * const MODE_AGENDA            = "agenda";
const char * const MODE_USERMANAGER       = "usermanager";

// Mode priorities: use Group+SubGroup+{YourGroupOrder}
enum ModeGroup {
    LastModeGroup   = 1000,
    FifthModeGroup  = LastModeGroup + 1000,
    FourthModeGroup = FifthModeGroup + 1000,
    ThirdModeGroup  = FourthModeGroup + 1000,
    SecondModeGroup = ThirdModeGroup + 1000,
    FirstModeGroup  = SecondModeGroup + 1000
};

enum ModeSubGroup {
    LastModeSubGroup   = 0,
    FifthModeSubGroup  = LastModeSubGroup + 50,
    FourthModeSubGroup = FifthModeSubGroup + 50,
    ThirdModeSubGroup  = FourthModeSubGroup + 50,
    SecondModeSubGroup = ThirdModeSubGroup + 50,
    FirstModeSubGroup  = SecondModeSubGroup + 50
};

// TODO: Mode Priorities should not be stored in the Core plugin
const int P_MODE_PATIENT_SEARCH  = FirstModeGroup + FirstModeSubGroup;
const int P_MODE_AGENDA          = SecondModeGroup + FirstModeSubGroup;
const int P_MODE_PATIENT_HISTORY = ThirdModeGroup + FirstModeSubGroup;
const int P_MODE_PATIENT_DRUGS   = ThirdModeGroup + SecondModeSubGroup;
const int P_MODE_PATIENT_FILE    = ThirdModeGroup + ThirdModeSubGroup;
const int P_MODE_USERMANAGER     = FourthModeGroup + FirstModeSubGroup;
const int P_MODE_TOOLS           = FourthModeGroup + SecondModeSubGroup;
const int P_MODE_ACCOUNT         = FourthModeGroup + ThirdModeSubGroup;

// Options page sortIndex (per categories)
const int          OPTIONINDEX_MAIN         = 0;
const int          OPTIONINDEX_PRINT        = 1000;
const int          OPTIONINDEX_AGENDA       = 2000;
const int          OPTIONINDEX_PATIENTS     = 3000;
const int          OPTIONINDEX_ALERTS       = 4000;
const int          OPTIONINDEX_PMHX         = 5000;
const int          OPTIONINDEX_FORMS        = 6000;
const int          OPTIONINDEX_DRUGS        = 7000;
const int          OPTIONINDEX_TEMPLATES    = 8000;
const int          OPTIONINDEX_WEBCAM       = 9000;
const int          OPTIONINDEX_ACCOUNT      = 10000;
const int          OPTIONINDEX_TOOLS        = 11000;

// Contexts
const char* const   C_GLOBAL             = "context.global";
const char* const   C_EDITOR_BASIC       = "context.Editor";
const char* const   C_EDITOR_CHAR_FORMAT = "context.Editor.char";
const char* const   C_EDITOR_PARAGRAPH   = "context.Editor.paragraph";
const char* const   C_EDITOR_CLIPBOARD   = "context.Editor.clipboard";
const char* const   C_EDITOR_TABLE       = "context.Editor.Table";
const char* const   C_EDITOR_IO          = "context.Editor.IO";
const char* const   C_EDITOR_PRINT       = "context.Editor.Print";
const char* const   C_EDITOR_ADDTEXT     = "context.Editor.AddText";
const char* const   C_USERMANAGER        = "context.UserManager";

// EDITOR
const char * const A_EDITOR_TOOGLETOOLBAR= "aEditorToggleToolbar";
const char * const M_EDITOR              = "menu.Editor";
const char * const M_EDITOR_CONTEXT      = "menu.Popup.Editor";
const char * const M_EDITOR_ADDTEXT      = "menu.Editor.AddText";
const char * const G_EDITOR_FILE         = "group.Editor.File";
const char * const G_EDITOR_CONTEXT      = "group.Editor.Context";
const char * const G_EDITOR_ADDTEXT      = "group.Editor.AddText";
const char * const A_EDITOR_FILEOPEN     = "a.Editor.Open";
const char * const A_EDITOR_FILESAVE     = "a.Editor.Save";
const char * const A_EDITOR_FILEPRINT    = "a.Editor.Print";
const char * const A_EDITOR_ADDDATE_LONG   = "a.Editor.AddDate.LongFormat";
const char * const A_EDITOR_ADDDATE_SHORT  = "a.Editor.AddDate.ShortFormat";
const char * const A_EDITOR_ADDUSERNAME    = "a.Editor.AddUserName";
const char * const A_EDITOR_ADDPATIENTNAME = "a.Editor.AddPatientName";

}  // end Constants
} // end Core

#endif // CORE_CONSTANTS_MENUS_H
