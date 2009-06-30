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
#ifndef TKACTIONMANAGER_H
#define TKACTIONMANAGER_H

#include <tkGlobal.h>
#include <tkExporter.h>
class tkActionManagerPrivate;

#include <QWidget>
#include <QMenu>
#include <QMenuBar>
#include <QAction>

/**
 * \file tkActionManager.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.9
 * \date 30 June 2009
*/

Q_TK_BEGIN_CONSTANTS
const char *const  TK_CONSTANTS_CONTEXT  = "tkConstants";      /*!< \brief Context for translations of constants  \ingroup toolkit_constants */
// Menu's name
const char * const  MENUBAR              = "menuBar";           /*!< \brief Default menubar object name  \ingroup constants_actionmanager */
const char * const  M_FILE               = "menuFile";          /*!< \brief Default menu File object name  \ingroup constants_actionmanager */
const char * const  M_EDIT               = "menuEdit";          /*!< \brief Default menu Edit object name  \ingroup constants_actionmanager */
const char * const  M_EDIT_SELECT        = "menuEdit.Select";   /*!< \brief Default menu Edit/Select object name  \ingroup constants_actionmanager */
const char * const  M_EDIT_LIST          = "menuEdit.List";     /*!< \brief Default menu Edit/List object name  \ingroup constants_actionmanager */
const char * const  M_FORMAT             = "menuFormat";        /*!< \brief Default menu Format object name  \ingroup constants_actionmanager */
const char * const  M_TOOLS              = "menuTools";         /*!< \brief Default menu Tools object name  \ingroup constants_actionmanager */
const char * const  M_PLUGINS            = "menuPlugins";       /*!< \brief Default menu Plugins object name  \ingroup constants_actionmanager */
const char * const  M_CONFIGURATION      = "menuConfiguration"; /*!< \brief Default menu Configuration object name  \ingroup constants_actionmanager */
const char * const  M_TOOLBARS           = "menuToolbars";      /*!< \brief Default menu Toolbars object name  \ingroup constants_actionmanager */
const char * const  M_LANGUAGES          = "menuLanguages";     /*!< \brief Default menu Languages object name  \ingroup constants_actionmanager */
const char * const  M_INTERPRETOR        = "menuInterpretor";   /*!< \brief Default menu Interpertor object name  \ingroup constants_actionmanager */
const char * const  M_ABOUT              = "menuAbout";         /*!< \brief Default menu About object name  \ingroup constants_actionmanager */
const char * const  M_UPDATE             = "menuUpdate";        /*!< \brief Default menu Update object name  \ingroup constants_actionmanager */
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

// Groups' name
// file menu groups
const char * const G_FILE_NEW            = "grFile.New";      /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_FILE_OPEN           = "grFile.Open";     /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_FILE_PROJECT        = "grFile.Project";  /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_FILE_SAVE           = "grFile.Save";     /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_FILE_CLOSE          = "grFile.Close";    /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_FILE_PRINT          = "grFile.Print";    /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_FILE_RECENTS        = "grFile.Recents";  /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_FILE_OTHER          = "grFile.Other";    /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_FILE_EXIT           = "grFile.Exit";     /*!< \brief Default group name  \ingroup constants_actionmanager */

// edit menu groups
const char * const G_EDIT_UNDOREDO       = "grEdit.UndoRedo";  /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_EDIT_COPYPASTE      = "grEdit.CopyPaste"; /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_EDIT_SELECT         = "grEdit.SelectAll"; /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_EDIT_LIST           = "grEdit.List";      /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_EDIT_ADVANCED       = "grEdit.Advanced";  /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_EDIT_FIND           = "grEdit.Find";      /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_EDIT_EDITOR         = "grEdit.Editor";    /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_EDIT_OTHER          = "grEdit.Other";     /*!< \brief Default group name  \ingroup constants_actionmanager */

// format menu groups
const char * const G_FORMAT_FONT         = "grFormat.Font";       /*!< \brief Default group name  \ingroup constants_actionmanager */
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

// Interpretor groups
const char * const G_INTERPRETOR_GENERAL = "grInterpretor.General";  /*!< \brief Default group name  \ingroup constants_actionmanager */

// Configuration groups (global)
const char * const G_CONFIGURATION       = "grConfig.Config";        /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_TOOLBARS            = "grConfig.Toolbars";      /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_LANGUAGES           = "grConfig.Languages";     /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_PREFERENCES         = "grConfig.Preferences";   /*!< \brief Default group name  \ingroup constants_actionmanager */

// Tools groups
const char * const G_TOOLS_GENERAL       = "grTools.General";        /*!< \brief Default group name  \ingroup constants_actionmanager */

// help groups (global)
const char * const G_HELP_HELP           = "grHelp.Help";       /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_HELP_ABOUT          = "grHelp.About";      /*!< \brief Default group name  \ingroup constants_actionmanager */
const char * const G_HELP_DEBUG          = "grHelp.Debug";      /*!< \brief Default group name  \ingroup constants_actionmanager */

// Update groups
const char * const G_UPDATE              = "grHelp.Update";     /*!< \brief Default group name  \ingroup constants_actionmanager */

// File Menu Action's name (find icon default into tkTheme.h)
const char * const  A_FILE_NEW            = "actionFileNew";    /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FILE_OPEN           = "actionFileOpen";   /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FILE_SAVE           = "actionFileSave";   /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FILE_SAVEAS         = "actionFileSaveAs"; /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FILE_PRINT          = "actionFilePrint";  /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FILE_EXIT           = "actionFileExit";   /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  FILENEW_TEXT          = QT_TRANSLATE_NOOP("tkConstants", "New");    /*!< \brief Default action text  \ingroup constants_actionmanager */
const char * const  FILEOPEN_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "Open");    /*!< \brief Default action text  \ingroup constants_actionmanager */
const char * const  FILESAVE_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "Save");    /*!< \brief Default action text  \ingroup constants_actionmanager */
const char * const  FILESAVEAS_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "Save as");    /*!< \brief Default action text  \ingroup constants_actionmanager */
const char * const  FILEPRINT_TEXT        = QT_TRANSLATE_NOOP("tkConstants", "Print...");    /*!< \brief Default action text  \ingroup constants_actionmanager */
const char * const  FILEEXIT_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "Exit");    /*!< \brief Default action text  \ingroup constants_actionmanager */

// Edit Menu Action's name (find icon default into tkTheme.h)
// A_EDIT_COPY << A_EDIT_CUT << A_EDIT_PASTE << A_EDIT_UNDO << A_EDIT_REDO << A_EDIT_SELECTALL << A_EDIT_DESELECT
const char * const  A_EDIT_COPY           = "actionEditCopy";      /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_EDIT_CUT            = "actionEditCut";       /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_EDIT_PASTE          = "actionEditPaste";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_EDIT_UNDO           = "actionEditUndo";      /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_EDIT_REDO           = "actionEditRedo";      /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_EDIT_SELECTALL      = "actionEditSelectAll"; /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_EDIT_DESELECT       = "actionEditDeselect";  /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  EDITCOPY_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "Paste");      /*!< \brief Default action text  \ingroup constants_actionmanager */
const char * const  EDITCUT_TEXT          = QT_TRANSLATE_NOOP("tkConstants", "Cut");        /*!< \brief Default action text  \ingroup constants_actionmanager */
const char * const  EDITPASTE_TEXT        = QT_TRANSLATE_NOOP("tkConstants", "Paste");      /*!< \brief Default action text  \ingroup constants_actionmanager */
const char * const  EDITUNDO_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "Undo");       /*!< \brief Default action text  \ingroup constants_actionmanager */
const char * const  EDITREDO_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "Redo");       /*!< \brief Default action text  \ingroup constants_actionmanager */
const char * const  EDITSELECTALL_TEXT    = QT_TRANSLATE_NOOP("tkConstants", "Select All"); /*!< \brief Default action text  \ingroup constants_actionmanager */
const char * const  EDITDESESLECT_TEXT    = QT_TRANSLATE_NOOP("tkConstants", "Deselect");   /*!< \brief Default action text  \ingroup constants_actionmanager */

// List actions's
const char * const  A_LIST_ADD            = "actionListAdd";        /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_LIST_REMOVE         = "actionListRemove";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_LIST_MOVEUP         = "actionListMoveUp";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_LIST_MOVEDOWN       = "actionListMoveDown";   /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  LISTADD_TEXT          = QT_TRANSLATE_NOOP("tkConstants", "Add to list");    /*!< \brief Default action text  \ingroup constants_actionmanager */
const char * const  LISTREMOVE_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "Remove of list"); /*!< \brief Default action text  \ingroup constants_actionmanager */
const char * const  LISTMOVEUP_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "Move item up");   /*!< \brief Default action text  \ingroup constants_actionmanager */
const char * const  LISTMOVEDOWN_TEXT     = QT_TRANSLATE_NOOP("tkConstants", "Move item down"); /*!< \brief Default action text  \ingroup constants_actionmanager */

// Format Menu Action's name  (find icon default into tkTheme.h)
// << A_FORMAT_BOLD << A_FORMAT_ITALIC << A_FORMAT_UNDERLINE << A_FORMAT_STRIKE << A_FORMAT_TABLE << A_FORMAT_IMAGE
const char * const  A_FORMAT_BOLD         = "actionFormatBold";       /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FORMAT_ITALIC       = "actionFormatItalic";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FORMAT_UNDERLINE    = "actionFormatUnderline";  /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FORMAT_STRIKE       = "actionFormatStrike";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FORMAT_BIGGER       = "actionFormatBigger";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FORMAT_SMALLER      = "actionFormatSmaller";    /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FORMAT_FONT         = "actionFormatFont";       /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FORMAT_TABLE        = "actionFormatTable";      /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_FORMAT_IMAGE        = "actionFormatImage";      /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  FORMATBOLD_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "Bold");           /*!< \brief Default action text  \ingroup constants_actionmanager */
const char * const  FORMATITALIC_TEXT     = QT_TRANSLATE_NOOP("tkConstants", "Italic");         /*!< \brief Default action text  \ingroup constants_actionmanager */
const char * const  FORMATUNDERLINE_TEXT  = QT_TRANSLATE_NOOP("tkConstants", "Underline");      /*!< \brief Default action text  \ingroup constants_actionmanager */
const char * const  FORMATSTRIKE_TEXT     = QT_TRANSLATE_NOOP("tkConstants", "Strike");         /*!< \brief Default action text  \ingroup constants_actionmanager */
const char * const  FORMATBIGGER_TEXT     = QT_TRANSLATE_NOOP("tkConstants", "Bigger");         /*!< \brief Default action text  \ingroup constants_actionmanager */
const char * const  FORMATSMALLER_TEXT    = QT_TRANSLATE_NOOP("tkConstants", "Smaller");        /*!< \brief Default action text  \ingroup constants_actionmanager */
const char * const  FORMATFONT_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "Font format");    /*!< \brief Default action text  \ingroup constants_actionmanager */
const char * const  FORMATTABLE_TEXT      = QT_TRANSLATE_NOOP("tkConstants", "Table format");    /*!< \brief Default action text  \ingroup constants_actionmanager */
const char * const  FORMATIMAGE_TEXT      = QT_TRANSLATE_NOOP("tkConstants", "Image format");    /*!< \brief Default action text  \ingroup constants_actionmanager */

// Paragraph Format Action's name
const char * const  A_ALIGN_CENTER        = "actionAlignCenter";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_ALIGN_LEFT          = "actionAlignLeft";       /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_ALIGN_RIGHT         = "actionAlignRight";      /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_ALIGN_JUSTIFY       = "actionAlignJustify";    /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  ALIGNCENTER_TEXT      = QT_TRANSLATE_NOOP("tkConstants", "Center");     /*!< \brief Default action text  \ingroup constants_actionmanager */
const char * const  ALIGNLEFT_TEXT        = QT_TRANSLATE_NOOP("tkConstants", "Left");       /*!< \brief Default action text  \ingroup constants_actionmanager */
const char * const  ALIGNRIGHT_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "Right");      /*!< \brief Default action text  \ingroup constants_actionmanager */
const char * const  ALIGNJUSTIFY_TEXT     = QT_TRANSLATE_NOOP("tkConstants", "Justify");    /*!< \brief Default action text  \ingroup constants_actionmanager */

// Interpretor Actions
const char * const  A_INTERPRETOR_GENERAL     = "actionInterpretor";      /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  INTERPRETOR_GENERAL_TEXT  = QT_TRANSLATE_NOOP("tkConstants", "Interpretor");    /*!< \brief Default action text  \ingroup constants_actionmanager */

// FreeMedForms Tools Actions
const char * const  A_USERMANAGER     = "actionUserManager";      /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  USERMANAGER_TEXT  = QT_TRANSLATE_NOOP("tkConstants", "User manager");    /*!< \brief Default action text  \ingroup constants_actionmanager */

// About Menu Action's name (find icon default into tkTheme.h)
const char * const  A_ABOUT              = "actionAbout";             /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_ABOUTQT            = "actionAboutQt";           /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_ABOUTFORM          = "actionAboutThisForm";     /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_APPLICATIONHELP    = "actionApplicationHelp";   /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_DEBUGHELPER        = "actionDebugHelper";       /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  A_HELPTEXT_TOGGLER   = "actionHelpTextToggler";   /*!< \brief Default action object name  \ingroup constants_actionmanager */

const char * const  ABOUT_TEXT           = QT_TRANSLATE_NOOP("tkConstants", "About");             /*!< \brief Default action text  \ingroup constants_actionmanager */
const char * const  ABOUTQT_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "About Qt");          /*!< \brief Default action text  \ingroup constants_actionmanager */
const char * const  ABOUTFORM_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "About this form");   /*!< \brief Default action text  \ingroup constants_actionmanager */
const char * const  APPLICATIONHELP_TEXT = QT_TRANSLATE_NOOP("tkConstants", "Help");              /*!< \brief Default action text  \ingroup constants_actionmanager */
const char * const  DEBUGHELPER_TEXT     = QT_TRANSLATE_NOOP("tkConstants", "Debugging tools");   /*!< \brief Default action text  \ingroup constants_actionmanager */
const char * const  HELPTEXTTOGGLER_TEXT = QT_TRANSLATE_NOOP("tkConstants", "Display/Hide form's help text");    /*!< \brief Default action text  \ingroup constants_actionmanager */

// Configuration Menu Action's name (find icon default into tkTheme.h)
const char * const  A_PREFERENCES       = "actionConfigure";      /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  PREFERENCES_TEXT    = QT_TRANSLATE_NOOP("tkConstants", "Preferences");    /*!< \brief Default action text  \ingroup constants_actionmanager */

// Update Menu Actions' name
const char * const  A_VIEWUPDATE       = "actionViewUpdate";      /*!< \brief Default action object name  \ingroup constants_actionmanager */
const char * const  VIEWUPDATE_TEXT    = QT_TRANSLATE_NOOP("tkConstants", "View update informations");    /*!< \brief Default action text  \ingroup constants_actionmanager */


Q_TK_END_CONSTANTS


class Q_TK_EXPORT tkActionManager : public QObject
{
    Q_OBJECT
    friend const void tkGlobal::initLib();

public:
    static tkActionManager *instance();
    ~tkActionManager();

    void createDefaultMenusAndGroups( QWidget * parent = 0 );
    void createDefaultFileMenu( QWidget * parent = 0 );
    void createDefaultEditMenu( QWidget * parent = 0, const QString &toMenuOrMenuBar = QString::null );
    void createDefaultFormatMenu( QWidget *parent = 0, const QString &toMenuOrMenuBar = QString::null );
    void createDefaultConfigurationMenu( QWidget *parent = 0 );
    void createDefaultAboutMenu( QWidget * parent = 0 );

    void createDefaultActions( QObject *parent = 0 );
    void createDefaultFileMenuActions( QObject *parent = 0 );
    void createDefaultEditMenuActions( QObject *parent = 0 );
    void createDefaultFormatMenuActions( QObject *parent = 0 );
    void createDefaultConfigurationMenuActions( QObject *parent = 0 );
    void createDefaultAboutMenuActions( QObject *parent = 0 );

    void setTrContext( const QString &context );

    // Managing menus and groups
    QMenuBar * createMenuBar( const QString &menuId, QWidget *parent );
    QMenu * createMenu( const QString &menuId, const QString &menuParent_OR_groupParent,
                        const QString &untranslatedName,
                        const QString &iconPath = QString::null, QWidget *parentWidget = 0 );
    QMenu * createPopMenu( const QString &menuId,
                           const QString &untranslatedName,
                           const QString &iconPath = QString::null, QWidget *parentWidget = 0 );
    bool appendGroup( const QString &groupId, const QString &menuId );
    void refreshMenuBar( const QString &menuId );
    void refreshMenu( const QString &menuId );
    void refreshAll();

    // Adding and create actions
    bool addAction( QAction *action, const QString &actionId, const QString &toGroup );
    QAction *createAction( const QString &actionId, const QString &toGroup, QObject *parent = 0 );

    void setActionDatas( const QString &actionId, const QString &unTrText,
                         const QString &unTrToolTip=QString::null, const QString &themedIconPath=QString::null );
    void setActionText( const QString &actionId, const QString &unTrText );         // allows automatic translation
    void setActionToolTip( const QString &actionId, const QString &unTrToolTip );   // allows automatic translation
    void setActionThemedIcon( const QString &actionId, const QString &iconPath );   // allows automatic translation
    void setActionUnthemedIcon( const QString &actionId, const QString &iconPath );
    void setActionEnabled( const QString &actionId, bool state );

    // Removing menus/menubars/actions/groups : delete pointer

    // Getters
    QMenu *menu( const QString &id );
    QMenuBar *menubar( const QString &id );
    QAction *action( const QString &id );
    bool actionExists( const QString &id );
    bool actionsExists( const QStringList &list );
    bool menuExists( const QString &id );

protected:
    tkActionManager( QObject *parent = 0 );

private Q_SLOTS:
    void objectToBeDestroyed(QObject*);
    void retranslate();

private:
    tkActionManagerPrivate *d;
    static tkActionManager *m_Instance;
};

#endif // TKACTIONMANAGER_H
