/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
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
#ifndef CORE_CONSTANTS_MENUS_H
#define CORE_CONSTANTS_MENUS_H


/**
  Regroups Menus, Actions, Groups and Modes identifiers
*/

namespace Core {

namespace Constants {

// Menu's name
const char * const  MENUBAR              = "menuBar";           /*!< \brief Default menubar object name  \ingroup constants_actionmanager */
const char * const  M_FILE               = "menuFile";          /*!< \brief Default menu File object name  \ingroup constants_actionmanager */
const char * const  M_FILE_RECENTFILES   = "menuFile.recents";  /*!< \brief Default menu File object name  \ingroup constants_actionmanager */
const char * const  M_TEMPLATES          = "menuTemplates";          /*!< \brief Default menu Edit object name  \ingroup constants_actionmanager */
const char * const  M_EDIT               = "menuEdit";          /*!< \brief Default menu Edit object name  \ingroup constants_actionmanager */
const char * const  M_PATIENTS           = "menuPatients";          /*!< \brief Default menu Patients object name  \ingroup constants_actionmanager */
const char * const  M_PATIENTS_HISTORY   = "menuPatients.history";
const char * const  M_EDITOR_EDIT        = "menu.editor.edition";
const char * const  M_EDITOR_FILE        = "menu.editor.file";
const char * const  M_EDIT_SELECT        = "menuEdit.Select";   /*!< \brief Default menu Edit/Select object name  \ingroup constants_actionmanager */
const char * const  M_EDIT_LIST          = "menuEdit.List";     /*!< \brief Default menu Edit/List object name  \ingroup constants_actionmanager */
const char * const  M_EDIT_TEMPLATES     = "menuEdit.Templates";     /*!< \brief Default menu Edit/List object name  \ingroup constants_actionmanager */
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
const char * const  M_INTERPRETOR        = "menuInterpretor";   /*!< \brief Default menu Interpertor object name  \ingroup constants_actionmanager */
const char * const  M_HELP               = "menuHelp";          /*!< \brief Default menu About object name  \ingroup constants_actionmanager */
const char * const  M_HELP_DATABASES     = "menuHelp.DB";       /*!< \brief Default menu About object name  \ingroup constants_actionmanager */
const char * const  M_UPDATE             = "menuUpdate";        /*!< \brief Default menu Update object name  \ingroup constants_actionmanager */

// Groups' name
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
const char * const G_PATIENT_HISTORY      = "grPatientsHist";
const char * const G_PATIENT_INFORMATIONS = "grPatientsInfos";

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
const char * const G_PLUGINS              =   "grPlugins";
const char * const G_PLUGINS_USERMANAGER  =   "grPlugins.usermanager";
const char * const G_PLUGINS_DRUGS        =   "grPlugins.drugs";
const char * const G_PLUGINS_CALENDAR     =   "grPlugins.calendar";
const char * const G_PLUGINS_OTHERS       =   "grPlugins.others";

// Interpretor groups
const char * const G_INTERPRETOR_GENERAL = "grInterpretor.General";  /*!< \brief Default group name  \ingroup constants_actionmanager */

// Configuration groups (global)
const char * const G_CONFIGURATION       = "grConfig";               /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_APP_CONFIGURATION   = "grConfig.AppConfig";     /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_TOOLBARS            = "grConfig.Toolbars";      /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_LANGUAGES           = "grConfig.Languages";     /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_PREFERENCES         = "grConfig.Preferences";   /*!< \brief Default group name  \ingroup constants_actionmanager */

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
const char * const  A_FILE_NEW               = "actionFileNew";    /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FILE_OPEN              = "actionFileOpen";   /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FILE_OPENFORM          = "actionFileOpenForm";   /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FILE_OPENPRESCRIPTION  = "actionFileOpenPrescription";   /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FILE_SAVE              = "actionFileSave";   /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FILE_SAVEFORM          = "actionFileSaveForm";   /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FILE_SAVEPRESCRIPTION  = "actionFileSavePrescription";   /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FILE_SAVEAS            = "actionFileSaveAs"; /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FILE_PRINT             = "actionFilePrint";  /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FILE_PRINTFORM         = "actionFilePrintForm";  /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FILE_PRINTPRESCRIPTION = "actionFilePrintPrescription";  /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FILE_PRINTPREVIEW      = "actionFilePrintPreview";  /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FILE_EXIT              = "actionFileExit";   /*!< \brief Default action object name  \ingroup constants_actionmanager */

// Patient actions
const char * const  A_PATIENT_NEW           = "actionPatientNew";   /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_PATIENT_REMOVE        = "actionPatientRemove";   /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_PATIENT_VIEWIDENTITY  = "actionPatientViewIdentity";   /*!< \brief Default action object name  \ingroup constants_actionmanager */


// Edit Menu Action's name (find icon default into tkTheme.h)
// A_EDIT_COPY << A_EDIT_CUT << A_EDIT_PASTE << A_EDIT_UNDO << A_EDIT_REDO << A_EDIT_SELECTALL << A_EDIT_DESELECT
const char * const  A_EDIT_COPY           = "actionEditCopy";      /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_EDIT_CUT            = "actionEditCut";       /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_EDIT_PASTE          = "actionEditPaste";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_EDIT_UNDO           = "actionEditUndo";      /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_EDIT_REDO           = "actionEditRedo";      /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_EDIT_SELECTALL      = "actionEditSelectAll"; /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_EDIT_DESELECT       = "actionEditDeselect";  /*!< \brief Default action object name  \ingroup constants_actionmanager */

// List actions's
const char * const  A_LIST_ADD            = "actionListAdd";        /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_LIST_REMOVE         = "actionListRemove";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_LIST_MOVEUP         = "actionListMoveUp";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_LIST_MOVEDOWN       = "actionListMoveDown";   /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_LIST_CLEAR          = "actionListClear";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_LIST_SORT           = "actionListSort";     /*!< \brief Default action object name  \ingroup constants_actionmanager */

// Templates actions
const char * const  A_TEMPLATE_CREATE     = "actionTemplateCreate";        /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_TEMPLATE_TOGGLEVIEW = "actionTemplateToggleView";        /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_TEMPLATE_ADD        = "actionTemplateAdd";        /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_TEMPLATE_SAVE       = "actionTemplateSave";        /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_TEMPLATE_PRINT      = "actionTemplatePrint";        /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_CATEGORY_ADD        = "actionCategoryAdd";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_TEMPLATE_REMOVE     = "actionTemplateRemove";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_CATEGORY_REMOVE     = "actionCategoryRemove";   /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_TEMPLATE_EDIT       = "actionTemplateEdit";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_CATEGORY_EDIT       = "actionCategoryEdit";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_TEMPLATE_LOCK       = "actionTemplateLock";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_TEMPLATE_DATABASEINFORMATIONS       = "actionTemplateDBInfos";     /*!< \brief Default action object name  \ingroup constants_actionmanager */

// Format Menu Action's name  (find icon default into tkTheme.h)
const char * const  A_FORMAT_BOLD         = "actionFormatBold";       /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FORMAT_ITALIC       = "actionFormatItalic";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FORMAT_UNDERLINE    = "actionFormatUnderline";  /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FORMAT_STRIKE       = "actionFormatStrike";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FORMAT_BIGGER       = "actionFormatBigger";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FORMAT_SMALLER      = "actionFormatSmaller";    /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FORMAT_FONT         = "actionFormatFont";       /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FORMAT_FONTCOLOR    = "actionFormatFontColor";  /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FORMAT_TABLE        = "actionFormatTable";      /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FORMAT_IMAGE        = "actionFormatImage";      /*!< \brief Default action object name  \ingroup constants_actionmanager */

// Paragraph Format Action's name
const char * const  A_ALIGN_CENTER        = "actionAlignCenter";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_ALIGN_LEFT          = "actionAlignLeft";       /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_ALIGN_RIGHT         = "actionAlignRight";      /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_ALIGN_JUSTIFY       = "actionAlignJustify";    /*!< \brief Default action object name  \ingroup constants_actionmanager */

// Tables' actions
const char * const  A_TABLE_ADDTABLE      = "actionAddTable";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_TABLE_TABLEPROP     = "actionTableProperties";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_TABLE_REMOVETABLE   = "actionRemoveTable";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_TABLE_ADDROW        = "actionTableAddRow";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_TABLE_REMOVEROW     = "actionTableRemoveRow";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_TABLE_ADDCOLUMN     = "actionTableAddCol";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_TABLE_REMOVECOLUMN  = "actionTableRemoveCol";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_TABLE_MERGECELLS    = "actionTableMergeCells";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_TABLE_SPLITCELLS    = "actionTableSplitCells";     /*!< \brief Default action object name  \ingroup constants_actionmanager */


// Interpretor Actions
const char * const  A_INTERPRETOR_GENERAL     = "actionInterpretor";      /*!< \brief Default action object name  \ingroup constants_actionmanager */

// FreeMedForms Tools Actions
const char * const  A_USERMANAGER     = "actionUserManager";      /*!< \brief Default action object name  \ingroup constants_actionmanager */

// About Menu Action's name
const char * const  A_ABOUT              = "actionAbout";             /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_ABOUTPLUGINS       = "actionPluginsAbout";      /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_ABOUTQT            = "actionAboutQt";           /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_ABOUTFORM          = "actionAboutThisForm";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_APPLICATIONHELP    = "actionApplicationHelp";   /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_DEBUGHELPER        = "actionDebugHelper";       /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_HELPTEXT_TOGGLER   = "actionHelpTextToggler";   /*!< \brief Default action object name  \ingroup constants_actionmanager */

// Configuration Menu Action's name
const char * const  A_PREFERENCES        = "actionConfigure";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_PLUGINS_PREFERENCES= "actionPlugPrefs";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_CONFIGURE_MEDINTUX = "actionConfMedintux";  /*!< \brief Default action object name  \ingroup constants_actionmanager */

// Update Menu Actions' name
const char * const  A_CHECKUPDATE      = "actionCheckUpdate";      /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_VIEWUPDATE       = "actionViewUpdate";      /*!< \brief Default action object name  \ingroup constants_actionmanager */


//modes
const char * const MODE_PATIENT_SEARCH   = "PatientSearch";
const char * const MODE_PATIENT_FILE     = "PatientFile";
const int          P_MODE_PATIENT_SEARCH = 100;
const int          P_MODE_PATIENT_FILE   = 90;

// Contexts
const char* const   C_GLOBAL             = "context.global";
const int           C_GLOBAL_ID          = 0;
const char* const   C_EDITOR_BASIC       = "context.Editor";
const char* const   C_EDITOR_TABLE       = "context.Editor.Table";
const char* const   C_EDITOR_IO          = "context.Editor.IO";
const char* const   C_USERMANAGER        = "context.UserManager";

// EDITOR
const char * const A_EDITOR_TOOGLETOOLBAR= "actionEditorToggleToolbar";
const char * const M_EDITOR              = "menu.Editor";
const char * const M_EDITOR_CONTEXT      = "menu.Popup.Editor";
const char * const G_EDITOR_FILE         = "group.Editor.File";
const char * const G_EDITOR_CONTEXT      = "group.Editor.Context";
const char * const A_EDITOR_FILEOPEN     = "action.Editor.Open";
const char * const A_EDITOR_FILESAVE     = "action.Editor.Save";
const char * const A_EDITOR_FILEPRINT    = "action.Editor.Print";

}  // end Constants
} // end Core

#endif // CORE_CONSTANTS_MENUS_H
