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
/**
  \class Core::MainWindowActionHandler
  Core::MainWindowActionHandler is the action keeper for your main window.
  Using the Core::MainWindowActions class, you can define which action you want to create, and then connect
  them to the predefined slots. Slots are virtualized.
  For code sample, please refer to MainWin::MainWindow code.
*/

#include "mainwindowactionhandler.h"

#include "actionmanager.h"
#include "actioncontainer.h"
#include "mainwindowactions.h"

#include <utils/updatechecker.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/uniqueidmanager.h>
#include <coreplugin/contextmanager/contextmanager.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants.h>
#include <coreplugin/translators.h>
#include <coreplugin/dialogs/debugdialog.h>
#include <coreplugin/dialogs/aboutdialog.h>
#include <coreplugin/dialogs/plugindialog.h>
#include <coreplugin/dialogs/helpdialog.h>

#include <QAction>
#include <QToolBar>
#include <QMenuBar>
#include <QWidget>
#include <QDebug>
#include <QKeySequence>
#include <QLocale>
#include <QStatusBar>
#include <QLabel>

using namespace Core;
using namespace Core::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}
static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Utils::UpdateChecker *updateChecker() { return Core::ICore::instance()->updateChecker(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }


MainWindowActionHandler::MainWindowActionHandler(QWidget *parent) :
        QMainWindow(parent),
        aNew(0),
        aOpen(0),
        aSave(0),
        aSaveAs(0),
        aPrint(0),
        aQuit(0),
        aUndo(0),
        aRedo(0),
        aCut(0),
        aCopy(0),
        aPaste(0),
        aSelectAll(0),
        aAppPrefs(0),
        aPlugsPrefs(0),
        aMedinTux(0),
        aLanguageGroup(0),
        aAppAbout(0),
        aPlugsAbout(0),
        aAppHelp(0),
        aQtAbout(0),
        aDebugDialog(0),
        aCheckUpdate(0),
        aUpdateAvailable(0),
        m_UpdateChecker(0)
{
    setObjectName("MainWindowActionHandler");
}

MainWindowActionHandler::~MainWindowActionHandler()
{
}

/** \brief Menu is created in the global context \sa Constants::C_GLOBAL_ID. Menu bar is automaticcaly created if necessary. */
void MainWindowActionHandler::createFileMenu()
{
    // creates menu bar
    ActionContainer *menubar = actionManager()->actionContainer(Constants::MENUBAR);
    if (!menubar) {
        menubar = actionManager()->createMenuBar(Constants::MENUBAR);
#ifndef Q_WS_MAC
        setMenuBar(menubar->menuBar());
#endif
    }
    menubar->appendGroup(Constants::G_FILE);

    // Create menu
    ActionContainer *filemenu = actionManager()->createMenu(Constants::M_FILE);
    menubar->addMenu(filemenu, Constants::G_FILE);
    filemenu->setTranslations(Trans::Constants::M_FILE_TEXT);
    filemenu->appendGroup(Constants::G_FILE_NEW);
    filemenu->appendGroup(Constants::G_FILE_OPEN);
    filemenu->appendGroup(Constants::G_FILE_RECENTS);
    filemenu->appendGroup(Constants::G_FILE_PROJECT);
    filemenu->appendGroup(Constants::G_FILE_SAVE);
    filemenu->appendGroup(Constants::G_FILE_PRINT);
    filemenu->appendGroup(Constants::G_FILE_OTHER);
    filemenu->appendGroup(Constants::G_FILE_CLOSE);
    filemenu->appendGroup(Constants::G_FILE_EXIT);
    ActionContainer *recentmenu = actionManager()->createMenu(Constants::M_FILE_RECENTFILES);
    recentmenu->setTranslations(Trans::Constants::M_FILE_RECENTFILES_TEXT);
    filemenu->addMenu(recentmenu,Constants::G_FILE_RECENTS);
}

void MainWindowActionHandler::createTemplatesMenu()
{
    ActionContainer *menubar = actionManager()->actionContainer(Constants::MENUBAR);
    Q_ASSERT(menubar);
    menubar->appendGroup(Constants::G_TEMPLATES);

    // Create menu
    ActionContainer *menu = actionManager()->createMenu(Constants::M_TEMPLATES);
    menubar->addMenu(menu, Constants::G_TEMPLATES);
    menu->setTranslations(Trans::Constants::TEMPLATES);
    menu->appendGroup(Constants::G_TEMPLATES);
    menu->appendGroup(Constants::G_TEMPLATES_NEW);
    menu->appendGroup(Constants::G_TEMPLATES_MANAGER);
    menu->appendGroup(Constants::G_TEMPLATES_EXTRAS);
    menu->appendGroup(Constants::G_TEMPLATES_OTHERS);
}

/** \brief Menu is created in the global context \sa Constants::C_GLOBAL_ID.*/
void MainWindowActionHandler::createEditMenu()
{
    ActionContainer *menubar = actionManager()->actionContainer(Constants::MENUBAR);
    Q_ASSERT(menubar);
    menubar->appendGroup(Constants::G_EDIT);

    // Edit Menu groups
    ActionContainer *editmenu = actionManager()->createMenu(Constants::M_EDIT);
    menubar->addMenu(editmenu, Constants::G_EDIT);
    editmenu->setTranslations(Trans::Constants::M_EDIT_TEXT);
    editmenu->appendGroup(Constants::G_EDIT);
    editmenu->appendGroup(Constants::G_EDIT_UNDOREDO);
    editmenu->appendGroup(Constants::G_EDIT_COPYPASTE);
    editmenu->appendGroup(Constants::G_EDIT_SELECT);
    editmenu->appendGroup(Constants::G_EDIT_LIST);
    editmenu->appendGroup(Constants::G_EDIT_ADVANCED);
    editmenu->appendGroup(Constants::G_EDIT_FIND);
    editmenu->appendGroup(Constants::G_EDIT_EDITOR);
    editmenu->appendGroup(Constants::G_EDIT_OTHER);
}

/** \brief Menu is created in the global context \sa Constants::C_GLOBAL_ID.*/
void MainWindowActionHandler::createFormatMenu()
{
    ActionContainer *menubar = actionManager()->actionContainer(Constants::MENUBAR);
    Q_ASSERT(menubar);
    menubar->appendGroup(Constants::G_FORMAT);
    ActionContainer *formatmenu = actionManager()->createMenu(Constants::M_FORMAT);
    menubar->addMenu(formatmenu, Constants::G_FORMAT);
    formatmenu->setTranslations(Trans::Constants::M_FORMAT_TEXT);
    formatmenu->appendGroup(Constants::G_FORMAT_FONT);
    formatmenu->appendGroup(Constants::G_FORMAT_PARAGRAPH);
    formatmenu->appendGroup(Constants::G_FORMAT_TABLE);
    formatmenu->appendGroup(Constants::G_FORMAT_IMAGE);
    formatmenu->appendGroup(Constants::G_FORMAT_OTHER);
}

void MainWindowActionHandler::createPluginsMenu()
{
    ActionContainer *menubar = actionManager()->actionContainer(Constants::MENUBAR);
    Q_ASSERT(menubar);
    menubar->appendGroup(Constants::G_PLUGINS);

    ActionContainer *confmenu = actionManager()->createMenu(Constants::M_PLUGINS);
    //    confmenu->setEmptyAction(ActionContainer::EA_Hide);
    menubar->addMenu(confmenu, Constants::G_PLUGINS);
    confmenu->setTranslations(Trans::Constants::M_PLUGINS_TEXT);
    confmenu->appendGroup(Constants::G_PLUGINS_USERMANAGER);
    confmenu->appendGroup(Constants::G_PLUGINS_DRUGS);
    confmenu->appendGroup(Constants::G_PLUGINS_CALENDAR);
    confmenu->appendGroup(Constants::G_PLUGINS_OTHERS);
}

/** \brief Menu is created in the global context \sa Constants::C_GLOBAL_ID.*/
void MainWindowActionHandler::createConfigurationMenu()
{
    ActionContainer *menubar = actionManager()->actionContainer(Constants::MENUBAR);
    Q_ASSERT(menubar);
    menubar->appendGroup(Constants::G_CONFIGURATION);

    ActionContainer *confmenu = actionManager()->createMenu(Constants::M_CONFIGURATION);
    //    confmenu->setEmptyAction(ActionContainer::EA_Hide);
    menubar->addMenu(confmenu, Constants::G_CONFIGURATION);
    confmenu->setTranslations(Trans::Constants::M_CONFIGURATION_TEXT);
    confmenu->appendGroup(Constants::G_APP_CONFIGURATION);
    confmenu->appendGroup(Constants::G_TOOLBARS);
    confmenu->appendGroup(Constants::G_LANGUAGES);
    confmenu->appendGroup(Constants::G_PREFERENCES);
}

/** \brief Menu is created in the global context \sa Constants::C_GLOBAL_ID.*/
void MainWindowActionHandler::createHelpMenu()
{
    ActionContainer *menubar = actionManager()->actionContainer(Constants::MENUBAR);
    Q_ASSERT(menubar);
    menubar->appendGroup(Constants::G_HELP);

    ActionContainer *m = actionManager()->createMenu(Constants::M_HELP);
    menubar->addMenu(m, Constants::G_HELP);
    m->setTranslations(Trans::Constants::M_ABOUT_TEXT);
    m->appendGroup(Constants::G_HELP_HELP);
    m->appendGroup(Constants::G_HELP_ABOUT);
    m->appendGroup(Constants::G_HELP_OTHER);
    m->appendGroup(Constants::G_UPDATE);
    m->appendGroup(Constants::G_HELP_DEBUG);
}

/** \brief Menu is created in the global context \sa Constants::C_GLOBAL_ID.*/
void MainWindowActionHandler::createUpdateMenu()
{
    ActionContainer *menubar = actionManager()->actionContainer(Constants::MENUBAR);
    Q_ASSERT(menubar);
    menubar->appendGroup(Constants::G_UPDATE);

    // Upadate Menu groups
    ActionContainer *upmenu = actionManager()->createMenu(Constants::M_UPDATE);
    menubar->addMenu(upmenu, Constants::G_UPDATE);
    upmenu->setTranslations(Trans::Constants::M_UPDATE_TEXT);
    upmenu->appendGroup(Constants::G_UPDATE_AVAILABLE);
    upmenu->appendGroup(Constants::G_UPDATE_TODATE);
    upmenu->appendGroup(Constants::G_UPDATE_OTHER);
}

/** \brief Actions are created in the global context \sa Constants::C_GLOBAL_ID */
void MainWindowActionHandler::createFileActions(int actions)
{
    QList<int> ctx = QList<int>() << Constants::C_GLOBAL_ID;
    ActionContainer *mfile = actionManager()->actionContainer(Constants::M_FILE);
    mfile->setTranslations(Trans::Constants::M_FILE_TEXT);
    Q_ASSERT(mfile);
    if (!mfile)
        return;

    QAction *a = 0;
    Command *cmd = 0;

    // New File Action
    if (actions & Core::MainWindowActions::A_FileNew) {
        a = aNew = new QAction(this);
        a->setIcon(QIcon(Constants::ICONFILENEW));
        cmd = actionManager()->registerAction(a, Constants::A_FILE_NEW, ctx);
        cmd->setDefaultKeySequence(QKeySequence::New);
        cmd->setTranslations(Trans::Constants::FILENEW_TEXT);
//        cmd->setAttribute(Command::CA_UpdateText);
        mfile->addAction(cmd, Constants::G_FILE_NEW);
    }

    // Open Action
    if (actions & Core::MainWindowActions::A_FileOpen) {
        a = aOpen = new QAction(this);
        a->setIcon(theme()->icon(Constants::ICONOPEN));
        cmd = actionManager()->registerAction(a, Constants::A_FILE_OPEN, ctx);
        cmd->setDefaultKeySequence(QKeySequence::Open);
        cmd->setTranslations(Trans::Constants::FILEOPEN_TEXT );
//        cmd->setAttribute(Command::CA_UpdateText);
        mfile->addAction(cmd, Constants::G_FILE_OPEN);
    }

    // Save Action
    if (actions & Core::MainWindowActions::A_FileSave) {
        a = aSave = new QAction(this);
        a->setIcon(theme()->icon(Constants::ICONSAVE));
        cmd = actionManager()->registerAction(a, Constants::A_FILE_SAVE, ctx);
        cmd->setDefaultKeySequence(QKeySequence::Save);
        cmd->setTranslations(Trans::Constants::FILESAVE_TEXT );
//        cmd->setAttribute(Command::CA_UpdateText);
        mfile->addAction(cmd, Constants::G_FILE_SAVE);
    }

    // SaveAs Action
    if (actions & Core::MainWindowActions::A_FileSaveAs) {
        a = aSaveAs = new QAction(this);
        a->setIcon(theme()->icon(Constants::ICONSAVEAS));
        cmd = actionManager()->registerAction(a, Constants::A_FILE_SAVEAS, ctx);
#if QT_VERSION >= 0x040500
        cmd->setDefaultKeySequence(QKeySequence::SaveAs);
#else
        cmd->setDefaultKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S);
#endif
        cmd->setTranslations(Trans::Constants::FILESAVEAS_TEXT );
//        cmd->setAttribute(Command::CA_UpdateText);
        mfile->addAction(cmd, Constants::G_FILE_SAVE);
    }

    // Print Action
    if (actions & Core::MainWindowActions::A_FilePrint) {
        a = aPrint = new QAction(this);
        a->setIcon(theme()->icon(Constants::ICONPRINT));
        cmd = actionManager()->registerAction(a, Constants::A_FILE_PRINT, ctx);
        cmd->setDefaultKeySequence(QKeySequence::Print);
        cmd->setTranslations(Trans::Constants::FILEPRINT_TEXT );
//        cmd->setAttribute(Command::CA_UpdateText);
        mfile->addAction(cmd, Constants::G_FILE_PRINT);
    }

    // Quit Action
    if (actions & Core::MainWindowActions::A_FileQuit) {
        a = aQuit = new QAction(this);
        a->setIcon(theme()->icon(Constants::ICONQUIT));
        cmd = actionManager()->registerAction(a, Constants::A_FILE_EXIT, ctx);
        cmd->setTranslations(Trans::Constants::FILEEXIT_TEXT );
        cmd->action()->setMenuRole(QAction::QuitRole);
        mfile->addAction(cmd, Constants::G_FILE_EXIT);
        connect(aQuit, SIGNAL(triggered()), this, SLOT(close()));
    }
}

/** \brief Connect created File's menu actions to there standard slots (eg : A_FileNew -> newFile()) */
void MainWindowActionHandler::connectFileActions()
{
    if (aNew)
        connect(aNew, SIGNAL(triggered()), this, SLOT(newFile()));

    if (aOpen)
        connect(aOpen, SIGNAL(triggered()), this, SLOT(openFile()));

    if (aSave)
        connect(aSave, SIGNAL(triggered()), this, SLOT(saveFile()));

    if (aSaveAs)
        connect(aSaveAs, SIGNAL(triggered()), this, SLOT(saveAsFile()));

    if (aPrint)
        connect(aPrint, SIGNAL(triggered()), this, SLOT(print()));

    if (aQuit)
        connect(aQuit, SIGNAL(triggered()), this, SLOT(close()));
}

void MainWindowActionHandler::createEditActions()
{
    QList<int> ctx = QList<int>() << Constants::C_GLOBAL_ID;
    ActionContainer *medit = actionManager()->actionContainer(Constants::M_EDIT);
    Q_ASSERT(medit);
    if (!medit)
        return;

    // Undo Action
    QAction *a = aUndo = new QAction(this);
    a->setObjectName("aUndo");
    a->setIcon(theme()->icon(Constants::ICONUNDO));
    Command *cmd = actionManager()->registerAction(a, Constants::A_EDIT_UNDO, ctx);
    cmd->setDefaultKeySequence(QKeySequence::Undo);
    //    cmd->setAttribute(Command::CA_UpdateText);
    cmd->setTranslations(Trans::Constants::EDITUNDO_TEXT);
    medit->addAction(cmd, Constants::G_EDIT_UNDOREDO);
    a->setEnabled(false);

    // Redo Action
    a = aRedo = new QAction(this);
    a->setObjectName("aRedo");
    a->setIcon(theme()->icon(Constants::ICONREDO));
    cmd = actionManager()->registerAction(a, Constants::A_EDIT_REDO, ctx);
    cmd->setDefaultKeySequence(QKeySequence::Redo);
    //    cmd->setAttribute(Command::CA_UpdateText);
    cmd->setTranslations(Trans::Constants::EDITREDO_TEXT);
    medit->addAction(cmd, Constants::G_EDIT_UNDOREDO);
    a->setEnabled(false);

    // Cut Action
    a = aCut = new QAction(this);
    a->setObjectName("aCut");
    a->setIcon(theme()->icon(Constants::ICONCUT));
    cmd = actionManager()->registerAction(a, Constants::A_EDIT_CUT, ctx);
    cmd->setDefaultKeySequence(QKeySequence::Cut);
    cmd->setTranslations(Trans::Constants::EDITCUT_TEXT );
    medit->addAction(cmd, Constants::G_EDIT_COPYPASTE);
    a->setEnabled(false);

    // Copy Action
    a = aCopy = new QAction(this);
    a->setObjectName("aCopy");
    a->setIcon(theme()->icon(Constants::ICONCOPY));
    cmd = actionManager()->registerAction(a, Constants::A_EDIT_COPY, ctx);
    cmd->setDefaultKeySequence(QKeySequence::Copy);
    cmd->setTranslations(Trans::Constants::EDITCOPY_TEXT );
    medit->addAction(cmd, Constants::G_EDIT_COPYPASTE);
    a->setEnabled(false);

    // Paste Action
    a = aPaste = new QAction(this);
    a->setObjectName("aPaste");
    a->setIcon(theme()->icon(Constants::ICONPASTE));
    cmd = actionManager()->registerAction(a, Constants::A_EDIT_PASTE, ctx);
    cmd->setDefaultKeySequence(QKeySequence::Paste);
    cmd->setTranslations(Trans::Constants::EDITPASTE_TEXT );
    medit->addAction(cmd, Constants::G_EDIT_COPYPASTE);
    a->setEnabled(false);

    // SelectAll Action
    a = aSelectAll = new QAction(this);
    a->setObjectName("aSelectAll");
    a->setIcon(theme()->icon(Constants::ICONSELECTALL));
    cmd = actionManager()->registerAction(a, Constants::A_EDIT_SELECTALL, ctx);
    cmd->setDefaultKeySequence(QKeySequence::SelectAll);
    cmd->setTranslations(Trans::Constants::EDITSELECTALL_TEXT );
    medit->addAction(cmd, Constants::G_EDIT_COPYPASTE);
    a->setEnabled(false);
}

/** \todo actually does nothing usefull */
void MainWindowActionHandler::connectEditActions()
{
    if (aUndo) {}
    if (aRedo) {}
    if (aCut) {}
    if (aCopy) {}
    if (aPaste) {}
    if (aSelectAll) {}
}

void MainWindowActionHandler::createConfigurationActions(int actions)
{
    QAction *a = 0;
    Command *cmd = 0;
    QList<int> ctx = QList<int>() << Constants::C_GLOBAL_ID;
    ActionContainer *menu = actionManager()->actionContainer(Constants::M_CONFIGURATION);
    Q_ASSERT(menu);
    if (!menu)
        return;

    if (actions & Core::MainWindowActions::A_AppPreferences) {
        a = aAppPrefs = new QAction(this);
        a->setObjectName("aAppPrefs");
        a->setIcon(theme()->icon(Constants::ICONPREFERENCES));
        a->setMenuRole(QAction::PreferencesRole);
        cmd = actionManager()->registerAction(a, Constants::A_PREFERENCES, ctx);
        cmd->setTranslations(Trans::Constants::PREFERENCES_TEXT);
        menu->addAction(cmd, Constants::G_APP_CONFIGURATION);
    }
    if (actions & Core::MainWindowActions::A_PluginsPreferences) {
        a = aPlugsPrefs = new QAction(this);
        a->setObjectName("aPlugsPrefs");
        a->setIcon(theme()->icon(Constants::ICONPREFERENCES));
        cmd = actionManager()->registerAction(a, Constants::A_PLUGINS_PREFERENCES, ctx);
        cmd->setDefaultKeySequence(QKeySequence::SelectAll);
        cmd->setTranslations(Trans::Constants::PLUGINS_CATEGORY);
        menu->addAction(cmd, Constants::G_APP_CONFIGURATION);
    }
    if (actions & Core::MainWindowActions::A_ConfigureMedinTux) {
        a = aMedinTux = new QAction(this);
        a->setObjectName("aMedinTux");
        a->setIcon(theme()->icon(Constants::ICONMEDINTUX));
        cmd = actionManager()->registerAction(a, Constants::A_CONFIGURE_MEDINTUX, ctx);
        cmd->setTranslations(Trans::Constants::CONFIGMEDINTUX_TEXT);
        menu->addAction(cmd, Constants::G_PREFERENCES);
    }
    if (actions & Core::MainWindowActions::A_LangageChange) {
        aLanguageGroup = new QActionGroup(this);
        // create language menu
        ActionContainer *menuconf = actionManager()->actionContainer(Constants::M_CONFIGURATION);
        Q_ASSERT(menuconf);
        ActionContainer *lmenu = actionManager()->createMenu(Constants::M_LANGUAGES);
        menuconf->addMenu(lmenu, Constants::G_LANGUAGES);
        lmenu->appendGroup(Constants::G_LANGUAGES);
        lmenu->setTranslations(Trans::Constants::M_LANGUAGES_TEXT);

        // populate with actions
        QMap<QString, QString> loc_lang = Core::ICore::instance()->translators()->availableLocalesAndLanguages();
        int i = 0;
        foreach( const QString &loc, loc_lang.keys() ) {
            i++;
            QAction *action = new QAction(this);  // QString("&%1 %2").arg(QString::number(i), loc_lang.value(loc)), this);
            action->setText(loc_lang.value(loc));
            action->setObjectName(loc);
            action->setData(loc);
            action->setCheckable(true);
            cmd = actionManager()->registerAction(action, loc, ctx);
            lmenu->addAction(cmd, Constants::G_LANGUAGES);
            aLanguageGroup->addAction(action);
            if (loc == QLocale().name().left(2))
                action->setChecked(true);
        }
        connect(aLanguageGroup, SIGNAL(triggered(QAction*)), this, SLOT(switchLanguage(QAction*)));
    }
}

void MainWindowActionHandler::switchLanguage(QAction * action)
{
    // Inform the Application that language changes, and loads the dictionnaries
    Core::ICore::instance()->translators()->changeLanguage(action->data().toString());
    // All GUIs will automatically refresh via changeEvent() members
}
/** \brief Show standard Help dialog starting at index. \sa Core::HelpDialog */
bool MainWindowActionHandler::applicationHelp()
{
    Core::HelpDialog::showIndex();
    return true;
}

/** \brief Connect created Configuration's menu actions to there standard slots (eg : A_AppPreferences -> applicationPreferences()) */
void MainWindowActionHandler::connectConfigurationActions()
{
    if (aAppPrefs)
        connect(aAppPrefs, SIGNAL(triggered()), this, SLOT(applicationPreferences()));
    if (aMedinTux)
        connect(aMedinTux, SIGNAL(triggered()), this, SLOT(configureMedintux()));
}

void MainWindowActionHandler::createHelpActions(int actions)
{
    QAction *a = 0;
    Command *cmd = 0;
    QList<int> ctx = QList<int>() << Constants::C_GLOBAL_ID;
    ActionContainer *menu = actionManager()->actionContainer(Constants::M_HELP);
    Q_ASSERT(menu);
    if (!menu)
        return;

    if (actions & Core::MainWindowActions::A_AppAbout) {
        a = aAppAbout = new QAction(this);
        a->setIcon(theme()->icon(Constants::ICONABOUT));
        a->setMenuRole(QAction::AboutRole);
        cmd = actionManager()->registerAction(a, Constants::A_ABOUT, ctx);
        cmd->setTranslations(Trans::Constants::ABOUT_TEXT);
        menu->addAction(cmd, Constants::G_HELP_ABOUT);
    }
    if (actions & Core::MainWindowActions::A_PluginsAbout) {
        a = aPlugsAbout = new QAction(this);
        a->setIcon(theme()->icon(Constants::ICONHELP));
        cmd = actionManager()->registerAction(a, Constants::A_ABOUTPLUGINS, ctx);
        cmd->setTranslations(Trans::Constants::ABOUTPLUGINS_TEXT);
        menu->addAction(cmd, Constants::G_HELP_ABOUT);
    }
    if (actions & Core::MainWindowActions::A_AppHelp) {
        a = aAppHelp = new QAction(this);
        a->setIcon(theme()->icon(Constants::ICONHELP));
        cmd = actionManager()->registerAction(a, Constants::A_APPLICATIONHELP, ctx);
        cmd->setDefaultKeySequence(QKeySequence::HelpContents);
        cmd->setTranslations(Trans::Constants::HELP_TEXT);
        menu->addAction(cmd, Constants::G_HELP_HELP);
    }
    if (actions & Core::MainWindowActions::A_QtAbout) {
        a = aQtAbout = new QAction(this);
        a->setIcon(theme()->icon(Constants::ICONABOUT));
        a->setMenuRole(QAction::AboutQtRole);
        cmd = actionManager()->registerAction(a, Constants::A_ABOUTQT, ctx);
        cmd->setTranslations(Trans::Constants::ABOUTQT_TEXT);
        menu->addAction(cmd, Constants::G_HELP_ABOUT);
    }
    if (actions & Core::MainWindowActions::A_DebugDialog) {
        a = aDebugDialog = new QAction(this);
        a->setIcon(theme()->icon(Constants::ICONHELP));
        cmd = actionManager()->registerAction(a, Constants::A_DEBUGHELPER, ctx);
        cmd->setTranslations(Trans::Constants::DEBUGHELPER_TEXT);
        menu->addAction(cmd, Constants::G_HELP_DEBUG);
    }

    if (actions & Core::MainWindowActions::A_CheckUpdate) {
        a = aCheckUpdate = new QAction(this);
        a->setIcon(theme()->icon(Constants::ICONSOFTWAREUPDATEAVAILABLE));
        cmd = actionManager()->registerAction(a, Constants::A_CHECKUPDATE, ctx);
        cmd->setTranslations(Trans::Constants::CHECKUPDATE);
        menu->addAction(cmd, Constants::G_UPDATE);
    }

    //    if (actions & Core::MainWindowActions::A_FormsAbout) {
    //        a = aDebugDialog = new QAction(this);
    //        a->setIcon(theme()->icon(Constants::ICONHELP));
    //        cmd = actionManager()->registerAction(a, Constants::A_DEBUGHELPER, ctx);
    //        cmd->setTranslations( Constants::DEBUGHELPER_TEXT );
    //        menu->addAction(cmd, Constants::G_HELP_DEBUG);
    //    }

}

/** \brief Connect created Help's menu actions to there standard slots (eg : A_AppAbout -> aboutApplication()) */
void MainWindowActionHandler::connectHelpActions()
{
    if (aAppAbout)
        connect(aAppAbout, SIGNAL(triggered()), this, SLOT(aboutApplication()));

    if (aPlugsAbout)
        connect(aPlugsAbout, SIGNAL(triggered()), this, SLOT(aboutPlugins()));

    if (aAppHelp)
        connect(aAppHelp, SIGNAL(triggered()), this, SLOT(applicationHelp()));

    if (aQtAbout)
        connect(aQtAbout, SIGNAL(triggered()), this, SLOT(aboutQt()));

    if (aDebugDialog)
        connect(aDebugDialog, SIGNAL(triggered()), this, SLOT(debugDialog()));

    if (aCheckUpdate)
        connect(aCheckUpdate, SIGNAL(triggered()), this, SLOT(checkUpdate()));
}

void MainWindowActionHandler::createTemplatesActions(int actions)
{
    QAction *a = 0;
    Command *cmd = 0;
    QList<int> ctx = QList<int>() << Constants::C_GLOBAL_ID;
    ActionContainer *menu = actionManager()->actionContainer(Constants::M_TEMPLATES);
    Q_ASSERT(menu);
    if (!menu)
        return;

    if (actions & Core::MainWindowActions::A_Templates_New) {
        a = new QAction(this);
        a->setIcon(theme()->icon(Core::Constants::ICONTEMPLATES));
        cmd = actionManager()->registerAction(a, Constants::A_TEMPLATE_CREATE, ctx);
        cmd->setTranslations(Trans::Constants::CREATETEMPLATE_TEXT);
        menu->addAction(cmd, Constants::G_TEMPLATES_NEW);
    }
    if (actions & Core::MainWindowActions::A_Templates_Manager) {
//        a = new QAction(this);
//        a->setIcon(theme()->icon(Constants::ICONABOUT));
//        a->setMenuRole(QAction::AboutRole);
//        cmd = actionManager()->registerAction(a, Constants::A_ABOUT, ctx);
//        cmd->setTranslations(Trans::Constants::ABOUT_TEXT);
//        menu->addAction(cmd, Constants::G_HELP_ABOUT);
    }
    if (actions & Core::MainWindowActions::A_Templates_ToogleViewer) {
        a = new QAction(this);
        a->setIcon(theme()->icon(Constants::ICONTEMPLATES));
        cmd = actionManager()->registerAction(a, Constants::A_TEMPLATE_TOOGLEVIEW, ctx);
        cmd->setTranslations(Trans::Constants::TEMPLATES_TOOGLEVIEW_TEXT);
        menu->addAction(cmd, Constants::G_TEMPLATES_EXTRAS);
    }
}

/** \brief Shows the debug dialog. \sa Core::DebugDialog */
bool MainWindowActionHandler::debugDialog()
{
    DebugDialog dlg(this);
    dlg.showDialog();
    return true;
}
/** \brief Shows the standard About dialog. \sa Core::AboutDialog */
bool MainWindowActionHandler::aboutApplication()
{
    AboutDialog dlg(this);
    dlg.showDialog();
    return true;
}
/** \shows the standard About Plugins dialog. \sa Core::PluginDialog */
bool MainWindowActionHandler::aboutPlugins()
{
    Core::PluginDialog dlg(this);
    dlg.exec();
    return true;
}

/** \brief Manage action creation. Actions are not connected. \sa Core::MainWindowActions */
void MainWindowActionHandler::createActions(const Core::MainWindowActions &actions)
{
    createFileActions(actions.fileActions());
    createConfigurationActions(actions.configurationActions());
    createHelpActions(actions.helpActions());
    createTemplatesActions(actions.templatesActions());
    if (actions.editActionsToCreate())
        createEditActions();
}

/** \brief Check over the web if an update of the software is available */
bool MainWindowActionHandler::checkUpdate()
{
    if (!updateChecker()->isChecking()) {
        statusBar()->addWidget(new QLabel(tkTr(Trans::Constants::CHECKING_UPDATES), this));
        statusBar()->addWidget(updateChecker()->progressBar(this),1);
        connect(updateChecker(), SIGNAL(updateFound()), this, SLOT(updateFound()));
        connect(updateChecker(), SIGNAL(done(bool)), this, SLOT(updateCheckerEnd()));
        updateChecker()->check(settings()->path(Core::ISettings::UpdateUrl));
    }
    return true;
}

/**
  \brief Slot to connect with the Utils::UpdateChecke::updateFound() signal.
  Add a menu to the menu bar and connect the only action to the update dialog. If the sender is
  known as a Utls::UpdateChecker, the action is automatically connected to the view update dialog from it.
*/
bool MainWindowActionHandler::updateFound()
{
    Utils::UpdateChecker *up = qobject_cast<Utils::UpdateChecker *>(sender());
    Q_ASSERT(up);
    ActionContainer *menu = actionManager()->actionContainer(Constants::M_UPDATE);
    if (!menu) {
        createUpdateMenu();
        menu = actionManager()->actionContainer(Constants::M_UPDATE);
        menu->retranslate();
    }
    QList<int> ctx = QList<int>() << Constants::C_GLOBAL_ID;
    Q_ASSERT(menu);
    if (!menu)
        return false;

    QAction *a = aUpdateAvailable = new QAction(this);
    a->setIcon(theme()->icon(Constants::ICONSOFTWAREUPDATEAVAILABLE));
    Command *cmd = actionManager()->registerAction(a, Constants::A_VIEWUPDATE, ctx);
    cmd->setTranslations(Trans::Constants::VIEWUPDATE_TEXT);
    menu->addAction(cmd, Constants::G_UPDATE_AVAILABLE);
    cmd->retranslate();
    Core::ICore::instance()->contextManager()->updateContext();
    if (up)
        connect(a, SIGNAL(triggered()), up, SLOT(showUpdateInformations()));
    return true;
}

void MainWindowActionHandler::updateCheckerEnd(bool error)
{
    Q_UNUSED(error);
    /** \todo improve this for FreeMedForms (delete only label and progressbar) */
    delete statusBar();
}
