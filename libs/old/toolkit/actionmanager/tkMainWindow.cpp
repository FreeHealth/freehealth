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
#include "tkMainWindow.h"

#include <tkActionManager.h>
#include <tkActionContainer.h>

#include <tkUniqueIdentifier.h>
#include <tkConstants.h>
#include <tkConstantTranslations.h>
#include <tkTheme.h>
#include <tkContextManager.h>
#include <tkUpdateChecker.h>
#include <tkTranslators.h>
#include <tkHelpDialog.h>
Q_TK_USING_CONSTANTS

#include <QAction>
#include <QToolBar>
#include <QMenuBar>
#include <QDebug>

tkMainWindow::tkMainWindow(QWidget *parent) :
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
        aAppHelp(0),
        aQtAbout(0),
        aDebugDialog(0),
        aUpdateAvailable(0)
{
    setObjectName("tkDefaultFileMenu");
}

tkMainWindow::~tkMainWindow()
{
}

/** \brief Menu is created in the global context \sa tkConstants::C_GLOBAL_ID. Menu bar is automaticcaly created if necessary. */
void tkMainWindow::createFileMenu()
{
    tkActionManager *am = tkActionManager::instance();

    // creates menu bar
    tkActionContainer *menubar = am->actionContainer(tkConstants::MENUBAR);
    if (!menubar) {
        menubar = am->createMenuBar(tkConstants::MENUBAR);
#ifndef Q_WS_MAC
        setMenuBar(menubar->menuBar());
#endif
    }
    menubar->appendGroup(tkConstants::G_FILE);

    // Create menu
    tkActionContainer *filemenu = am->createMenu(tkConstants::M_FILE);
    menubar->addMenu(filemenu, tkConstants::G_FILE);
    filemenu->setTranslations(tkConstants::M_FILE_TEXT);
    filemenu->appendGroup(tkConstants::G_FILE_NEW);
    filemenu->appendGroup(tkConstants::G_FILE_OPEN);
    filemenu->appendGroup(tkConstants::G_FILE_PROJECT);
    filemenu->appendGroup(tkConstants::G_FILE_SAVE);
    filemenu->appendGroup(tkConstants::G_FILE_PRINT);
    filemenu->appendGroup(tkConstants::G_FILE_RECENTS);
    filemenu->appendGroup(tkConstants::G_FILE_OTHER);
    filemenu->appendGroup(tkConstants::G_FILE_CLOSE);
    filemenu->appendGroup(tkConstants::G_FILE_EXIT);
}

/** \brief Menu is created in the global context \sa tkConstants::C_GLOBAL_ID.*/
void tkMainWindow::createEditMenu()
{
    tkActionManager *am = tkActionManager::instance();

    tkActionContainer *menubar = am->actionContainer(tkConstants::MENUBAR);
    Q_ASSERT(menubar);
    menubar->appendGroup(tkConstants::G_EDIT);

    // Edit Menu groups
    tkActionContainer *editmenu = am->createMenu(tkConstants::M_EDIT);
    menubar->addMenu(editmenu, tkConstants::G_EDIT);
    editmenu->setTranslations(tkConstants::M_EDIT_TEXT);
    editmenu->appendGroup(tkConstants::G_EDIT);
    editmenu->appendGroup(tkConstants::G_EDIT_UNDOREDO);
    editmenu->appendGroup(tkConstants::G_EDIT_COPYPASTE);
    editmenu->appendGroup(tkConstants::G_EDIT_SELECT);
    editmenu->appendGroup(tkConstants::G_EDIT_LIST);
    editmenu->appendGroup(tkConstants::G_EDIT_ADVANCED);
    editmenu->appendGroup(tkConstants::G_EDIT_FIND);
    editmenu->appendGroup(tkConstants::G_EDIT_EDITOR);
    editmenu->appendGroup(tkConstants::G_EDIT_OTHER);
}

/** \brief Menu is created in the global context \sa tkConstants::C_GLOBAL_ID.*/
void tkMainWindow::createFormatMenu()
{
    tkActionManager *am = tkActionManager::instance();

    tkActionContainer *menubar = am->actionContainer(tkConstants::MENUBAR);
    Q_ASSERT(menubar);
    menubar->appendGroup(tkConstants::G_FORMAT);
    tkActionContainer *formatmenu = am->createMenu(tkConstants::M_FORMAT);
    menubar->addMenu(formatmenu, tkConstants::G_FORMAT);
    formatmenu->setTranslations(tkConstants::M_FORMAT_TEXT);
    formatmenu->appendGroup(tkConstants::G_FORMAT_FONT);
    formatmenu->appendGroup(tkConstants::G_FORMAT_PARAGRAPH);
    formatmenu->appendGroup(tkConstants::G_FORMAT_TABLE);
    formatmenu->appendGroup(tkConstants::G_FORMAT_IMAGE);
    formatmenu->appendGroup(tkConstants::G_FORMAT_OTHER);
}

void tkMainWindow::createPluginsMenu()
{
    tkActionManager *am = tkActionManager::instance();

    tkActionContainer *menubar = am->actionContainer(tkConstants::MENUBAR);
    Q_ASSERT(menubar);
    menubar->appendGroup(tkConstants::G_PLUGINS);

    tkActionContainer *confmenu = am->createMenu(tkConstants::M_PLUGINS);
    //    confmenu->setEmptyAction(tkActionContainer::EA_Hide);
    menubar->addMenu(confmenu, tkConstants::G_PLUGINS);
    confmenu->setTranslations(tkConstants::M_PLUGINS_TEXT);
    confmenu->appendGroup(tkConstants::G_PLUGINS_USERMANAGER);
    confmenu->appendGroup(tkConstants::G_PLUGINS_DRUGS);
    confmenu->appendGroup(tkConstants::G_PLUGINS_CALENDAR);
    confmenu->appendGroup(tkConstants::G_PLUGINS_OTHERS);
}

/** \brief Menu is created in the global context \sa tkConstants::C_GLOBAL_ID.*/
void tkMainWindow::createConfigurationMenu()
{
    tkActionManager *am = tkActionManager::instance();

    tkActionContainer *menubar = am->actionContainer(tkConstants::MENUBAR);
    Q_ASSERT(menubar);
    menubar->appendGroup(tkConstants::G_CONFIGURATION);

    tkActionContainer *confmenu = am->createMenu(tkConstants::M_CONFIGURATION);
    //    confmenu->setEmptyAction(tkActionContainer::EA_Hide);
    menubar->addMenu(confmenu, tkConstants::G_CONFIGURATION);
    confmenu->setTranslations(tkConstants::M_CONFIGURATION_TEXT);
    confmenu->appendGroup(tkConstants::G_APP_CONFIGURATION);
    confmenu->appendGroup(tkConstants::G_TOOLBARS);
    confmenu->appendGroup(tkConstants::G_LANGUAGES);
    confmenu->appendGroup(tkConstants::G_PREFERENCES);
}

/** \brief Menu is created in the global context \sa tkConstants::C_GLOBAL_ID.*/
void tkMainWindow::createHelpMenu()
{
    tkActionManager *am = tkActionManager::instance();

    tkActionContainer *menubar = am->actionContainer(tkConstants::MENUBAR);
    Q_ASSERT(menubar);
    menubar->appendGroup(tkConstants::G_HELP);

    tkActionContainer *m = am->createMenu(tkConstants::M_HELP);
    menubar->addMenu(m, tkConstants::G_HELP);
    m->setTranslations(tkConstants::M_ABOUT_TEXT);
    m->appendGroup(tkConstants::G_HELP_HELP);
    m->appendGroup(tkConstants::G_HELP_ABOUT);
    m->appendGroup(tkConstants::G_HELP_OTHER);
    m->appendGroup(tkConstants::G_HELP_DEBUG);
}

/** \brief Menu is created in the global context \sa tkConstants::C_GLOBAL_ID.*/
void tkMainWindow::createUpdateMenu()
{
    tkActionManager *am = tkActionManager::instance();

    tkActionContainer *menubar = am->actionContainer(tkConstants::MENUBAR);
    Q_ASSERT(menubar);
    menubar->appendGroup(tkConstants::G_UPDATE);

    // Upadate Menu groups
    tkActionContainer *upmenu = am->createMenu(tkConstants::M_UPDATE);
    menubar->addMenu(upmenu, tkConstants::G_UPDATE);
    upmenu->setTranslations(tkConstants::M_UPDATE_TEXT);
    upmenu->appendGroup(tkConstants::G_UPDATE_AVAILABLE);
    upmenu->appendGroup(tkConstants::G_UPDATE_TODATE);
    upmenu->appendGroup(tkConstants::G_UPDATE_OTHER);
}

/** \brief Actions are created in the global context \sa tkConstants::C_GLOBAL_ID */
void tkMainWindow::createFileActions(FileActions actions)
{
    tkActionManager *am = tkActionManager::instance();
    QList<int> ctx = QList<int>() << tkConstants::C_GLOBAL_ID;
    tkActionContainer *mfile = am->actionContainer(tkConstants::M_FILE);
    Q_ASSERT(mfile);
    if (!mfile)
        return;

    QAction *a = 0;
    tkCommand *cmd = 0;

    // New File Action
    if (actions & A_FileNew) {
        a = aNew = new QAction(this);
        a->setIcon(QIcon(tkConstants::ICONFILENEW));
        cmd = am->registerAction(a, tkConstants::A_FILE_NEW, ctx);
        cmd->setDefaultKeySequence(QKeySequence::New);
        cmd->setTranslations(tkConstants::FILENEW_TEXT);
        cmd->setAttribute(tkCommand::CA_UpdateText);
        mfile->addAction(cmd, tkConstants::G_FILE_NEW);
    }

    // Open Action
    if (actions & A_FileOpen) {
        a = aOpen = new QAction(this);
        a->setIcon(tkTheme::icon(tkConstants::ICONOPEN));
        cmd = am->registerAction(a, tkConstants::A_FILE_OPEN, ctx);
        cmd->setDefaultKeySequence(QKeySequence::Open);
        cmd->setTranslations( tkConstants::FILEOPEN_TEXT );
        cmd->setAttribute(tkCommand::CA_UpdateText);
        mfile->addAction(cmd, tkConstants::G_FILE_OPEN);
    }

    // Save Action
    if (actions & A_FileSave) {
        a = aSave = new QAction(this);
        a->setIcon(tkTheme::icon(tkConstants::ICONSAVE));
        cmd = am->registerAction(a, tkConstants::A_FILE_SAVE, ctx);
        cmd->setDefaultKeySequence(QKeySequence::Save);
        cmd->setTranslations( tkConstants::FILESAVE_TEXT );
        cmd->setAttribute(tkCommand::CA_UpdateText);
        mfile->addAction(cmd, tkConstants::G_FILE_SAVE);
    }

    // SaveAs Action
    if (actions & A_FileSaveAs) {
        a = aSaveAs = new QAction(this);
        a->setIcon(tkTheme::icon(tkConstants::ICONSAVEAS));
        cmd = am->registerAction(a, tkConstants::A_FILE_SAVEAS, ctx);
        cmd->setDefaultKeySequence(QKeySequence::SaveAs);
        cmd->setTranslations( tkConstants::FILESAVEAS_TEXT );
        cmd->setAttribute(tkCommand::CA_UpdateText);
        mfile->addAction(cmd, tkConstants::G_FILE_SAVE);
    }

    // Print Action
    if (actions & A_FilePrint) {
        a = aPrint = new QAction(this);
        a->setIcon(tkTheme::icon(tkConstants::ICONPRINT));
        cmd = am->registerAction(a, tkConstants::A_FILE_PRINT, ctx);
        cmd->setDefaultKeySequence(QKeySequence::Print);
        cmd->setTranslations( tkConstants::FILEPRINT_TEXT );
        cmd->setAttribute(tkCommand::CA_UpdateText);
        mfile->addAction(cmd, tkConstants::G_FILE_PRINT);
    }

    // Quit Action
    if (actions & A_FileQuit) {
        a = aQuit = new QAction(this);
        a->setIcon(tkTheme::icon(tkConstants::ICONQUIT));
        cmd = am->registerAction(a, tkConstants::A_FILE_EXIT, ctx);
        cmd->setTranslations( tkConstants::FILEEXIT_TEXT );
        cmd->action()->setMenuRole(QAction::QuitRole);
        mfile->addAction(cmd, tkConstants::G_FILE_EXIT);
        connect(aQuit, SIGNAL(triggered()), this, SLOT(close()));
    }
}

void tkMainWindow::connectFileActions()
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

void tkMainWindow::createEditActions()
{
    tkActionManager *am = tkActionManager::instance();
    QList<int> ctx = QList<int>() << tkConstants::C_GLOBAL_ID;
    tkActionContainer *medit = am->actionContainer(tkConstants::M_EDIT);
    Q_ASSERT(medit);
    if (!medit)
        return;

    // Undo Action
    QAction *a = aUndo = new QAction(this);
    a->setIcon(tkTheme::icon(tkConstants::ICONUNDO));
    tkCommand *cmd = am->registerAction(a, tkConstants::A_EDIT_UNDO, ctx);
    cmd->setDefaultKeySequence(QKeySequence::Undo);
    //    cmd->setAttribute(tkCommand::CA_UpdateText);
    cmd->setTranslations(tkConstants::EDITUNDO_TEXT);
    medit->addAction(cmd, tkConstants::G_EDIT_UNDOREDO);
    a->setEnabled(false);

    // Redo Action
    a = aRedo = new QAction(this);
    a->setIcon(tkTheme::icon(tkConstants::ICONREDO));
    cmd = am->registerAction(a, tkConstants::A_EDIT_REDO, ctx);
    cmd->setDefaultKeySequence(QKeySequence::Redo);
    //    cmd->setAttribute(tkCommand::CA_UpdateText);
    cmd->setTranslations( tkConstants::EDITREDO_TEXT );
    medit->addAction(cmd, tkConstants::G_EDIT_UNDOREDO);
    a->setEnabled(false);

    // Cut Action
    a = aCut = new QAction(this);
    a->setIcon(tkTheme::icon(tkConstants::ICONCUT));
    cmd = am->registerAction(a, tkConstants::A_EDIT_CUT, ctx);
    cmd->setDefaultKeySequence(QKeySequence::Cut);
    cmd->setTranslations( tkConstants::EDITCUT_TEXT );
    medit->addAction(cmd, tkConstants::G_EDIT_COPYPASTE);
    a->setEnabled(false);

    // Copy Action
    a = aCopy = new QAction(this);
    a->setIcon(tkTheme::icon(tkConstants::ICONCOPY));
    cmd = am->registerAction(a, tkConstants::A_EDIT_COPY, ctx);
    cmd->setDefaultKeySequence(QKeySequence::Copy);
    cmd->setTranslations( tkConstants::EDITCOPY_TEXT );
    medit->addAction(cmd, tkConstants::G_EDIT_COPYPASTE);
    a->setEnabled(false);

    // Paste Action
    a = aPaste = new QAction(this);
    a->setIcon(tkTheme::icon(tkConstants::ICONPASTE));
    cmd = am->registerAction(a, tkConstants::A_EDIT_PASTE, ctx);
    cmd->setDefaultKeySequence(QKeySequence::Paste);
    cmd->setTranslations( tkConstants::EDITPASTE_TEXT );
    medit->addAction(cmd, tkConstants::G_EDIT_COPYPASTE);
    a->setEnabled(false);

    // SelectAll Action
    a = aSelectAll = new QAction(this);
    a->setIcon(tkTheme::icon(tkConstants::ICONSELECTALL));
    cmd = am->registerAction(a, tkConstants::A_EDIT_SELECTALL, ctx);
    cmd->setDefaultKeySequence(QKeySequence::SelectAll);
    cmd->setTranslations( tkConstants::EDITSELECTALL_TEXT );
    medit->addAction(cmd, tkConstants::G_EDIT_COPYPASTE);
    a->setEnabled(false);
}

/** \todo actually does nothing usefull */
void tkMainWindow::connectEditActions()
{
    if (aUndo) {}
    if (aRedo) {}
    if (aCut) {}
    if (aCopy) {}
    if (aPaste) {}
    if (aSelectAll) {}
}

void tkMainWindow::createConfigurationActions(ConfigurationActions actions)
{
    QAction *a = 0;
    tkCommand *cmd = 0;
    tkActionManager *am = tkActionManager::instance();
    QList<int> ctx = QList<int>() << tkConstants::C_GLOBAL_ID;
    tkActionContainer *menu = am->actionContainer(tkConstants::M_CONFIGURATION);
    Q_ASSERT(menu);
    if (!menu)
        return;

    if (actions & A_AppPreferences) {
        a = aAppPrefs = new QAction(this);
        a->setIcon(tkTheme::icon(tkConstants::ICONPREFERENCES));
        a->setMenuRole(QAction::PreferencesRole);
        cmd = am->registerAction(a, tkConstants::A_PREFERENCES, ctx);
        cmd->setTranslations(tkConstants::PREFERENCES_TEXT);
        menu->addAction(cmd, tkConstants::G_APP_CONFIGURATION);
    }
    if (actions & A_PluginsPreferences) {
        a = aPlugsPrefs = new QAction(this);
        a->setIcon(tkTheme::icon(tkConstants::ICONPREFERENCES));
        cmd = am->registerAction(a, tkConstants::A_PREFERENCES, ctx);
        cmd->setDefaultKeySequence(QKeySequence::SelectAll);
        cmd->setTranslations( tkConstants::PREFERENCES_TEXT );
        menu->addAction(cmd, tkConstants::G_APP_CONFIGURATION);
    }
    if (actions & A_ConfigureMedinTux) {
        a = aMedinTux = new QAction(this);
        a->setIcon(tkTheme::icon(tkConstants::ICONMEDINTUX));
        cmd = am->registerAction(a, tkConstants::A_CONFIGURE_MEDINTUX, ctx);
        cmd->setTranslations( tkConstants::CONFIGMEDINTUX_TEXT );
        menu->addAction(cmd, tkConstants::G_PREFERENCES);
    }
    if (actions & A_LangageChange) {
        aLanguageGroup = new QActionGroup(this);
        // create language menu
        tkActionManager *am = tkActionManager::instance();
        tkActionContainer *menuconf = am->actionContainer(tkConstants::M_CONFIGURATION);
        Q_ASSERT(menuconf);
        tkActionContainer *lmenu = am->createMenu(tkConstants::M_LANGUAGES);
        menuconf->addMenu(lmenu, tkConstants::G_LANGUAGES);
        lmenu->setTranslations(tkConstants::M_LANGUAGES_TEXT);

        // populate with actions
        QMap<QString, QString> loc_lang = tkTranslators::availableLocalesAndLanguages();
        int i = 0;
        foreach( const QString &loc, loc_lang.keys() ) {
            i++;
            QAction *action = new QAction(this);  // QString("&%1 %2").arg(QString::number(i), loc_lang.value(loc)), this);
            action->setText(loc_lang.value(loc));
            action->setObjectName(loc);
            action->setData(loc);
            action->setCheckable(true);
            cmd = am->registerAction(action, loc, ctx);
            lmenu->addAction(cmd);
            aLanguageGroup->addAction(action);
            if (loc == QLocale().name().left(2))
                action->setChecked(true);
        }
        connect(aLanguageGroup, SIGNAL(triggered(QAction*)), this, SLOT(switchLanguage(QAction*)));
    }
}

void tkMainWindow::switchLanguage(QAction * action)
{
    // Inform the Application that language changes, and loads the dictionnaries
    tkTranslators::instance(qApp)->changeLanguage(action->data().toString());
    // All GUIs will automatically refresh via changeEvent() members
}

bool tkMainWindow::applicationHelp()
{
    tkHelpDialog::showIndex();
    return true;
}

void tkMainWindow::connectConfigurationActions()
{
    if (aAppPrefs)
        connect(aAppPrefs, SIGNAL(triggered()), this, SLOT(applicationPreferences()));
    if (aMedinTux)
        connect(aMedinTux, SIGNAL(triggered()), this, SLOT(configureMedintux()));
}

void tkMainWindow::createHelpActions(HelpActions actions)
{
    QAction *a = 0;
    tkCommand *cmd = 0;
    tkActionManager *am = tkActionManager::instance();
    QList<int> ctx = QList<int>() << tkConstants::C_GLOBAL_ID;
    tkActionContainer *menu = am->actionContainer(tkConstants::M_HELP);
    Q_ASSERT(menu);
    if (!menu)
        return;

    if (actions & A_AppAbout) {
        a = aAppAbout = new QAction(this);
        a->setIcon(tkTheme::icon(tkConstants::ICONABOUT));
        a->setMenuRole(QAction::AboutRole);
        cmd = am->registerAction(a, tkConstants::A_ABOUT, ctx);
        cmd->setTranslations(tkConstants::ABOUT_TEXT);
        menu->addAction(cmd, tkConstants::G_HELP_ABOUT);
    }
    if (actions & A_AppHelp) {
        a = aAppHelp = new QAction(this);
        a->setIcon(tkTheme::icon(tkConstants::ICONHELP));
        cmd = am->registerAction(a, tkConstants::A_APPLICATIONHELP, ctx);
        cmd->setDefaultKeySequence(QKeySequence::HelpContents);
        cmd->setTranslations( tkConstants::HELP_TEXT );
        menu->addAction(cmd, tkConstants::G_HELP_HELP);
    }
    if (actions & A_QtAbout) {
        a = aQtAbout = new QAction(this);
        a->setIcon(tkTheme::icon(tkConstants::ICONABOUT));
        a->setMenuRole(QAction::AboutQtRole);
        cmd = am->registerAction(a, tkConstants::A_ABOUTQT, ctx);
        cmd->setTranslations( tkConstants::ABOUTQT_TEXT );
        menu->addAction(cmd, tkConstants::G_HELP_ABOUT);
    }

    if (actions & A_DebugDialog) {
        a = aDebugDialog = new QAction(this);
        a->setIcon(tkTheme::icon(tkConstants::ICONHELP));
        cmd = am->registerAction(a, tkConstants::A_DEBUGHELPER, ctx);
        cmd->setTranslations( tkConstants::DEBUGHELPER_TEXT );
        menu->addAction(cmd, tkConstants::G_HELP_DEBUG);
    }

    //    if (actions & A_PluginsAbout) {
    //        a = aDebugDialog = new QAction(this);
    //        a->setIcon(tkTheme::icon(tkConstants::ICONHELP));
    //        cmd = am->registerAction(a, tkConstants::A_DEBUGHELPER, ctx);
    //        cmd->setTranslations( tkConstants::DEBUGHELPER_TEXT );
    //        menu->addAction(cmd, tkConstants::G_HELP_DEBUG);
    //    }
    //
    //    if (actions & A_FormsAbout) {
    //        a = aDebugDialog = new QAction(this);
    //        a->setIcon(tkTheme::icon(tkConstants::ICONHELP));
    //        cmd = am->registerAction(a, tkConstants::A_DEBUGHELPER, ctx);
    //        cmd->setTranslations( tkConstants::DEBUGHELPER_TEXT );
    //        menu->addAction(cmd, tkConstants::G_HELP_DEBUG);
    //    }

}

void tkMainWindow::connectHelpActions()
{
    if (aAppAbout)
        connect(aAppAbout, SIGNAL(triggered()), this, SLOT(aboutApplication()));

    if (aAppHelp)
        connect(aAppHelp, SIGNAL(triggered()), this, SLOT(applicationHelp()));

    if (aQtAbout)
        connect(aQtAbout, SIGNAL(triggered()), this, SLOT(aboutQt()));

    if (aDebugDialog)
        connect(aDebugDialog, SIGNAL(triggered()), this, SLOT(debugDialog()));
}

/**
  \brief Slot to connect with the tkUpdateChecker::updateFounded()
  Add a menu to the menu bar and connect the only action to the update dialog. If the sender is
  known as a tkUpdateChecker, the action is automatically connected to the view update dialog from it.
*/
bool tkMainWindow::updateFound()
{
    tkUpdateChecker *up = qobject_cast<tkUpdateChecker *>(sender());
    tkActionManager *am = tkActionManager::instance();
    tkActionContainer *menu = am->actionContainer(tkConstants::M_UPDATE);
    if (!menu) {
        createUpdateMenu();
        menu = am->actionContainer(tkConstants::M_UPDATE);
        menu->retranslate();
    }
    QList<int> ctx = QList<int>() << tkConstants::C_GLOBAL_ID;
    Q_ASSERT(menu);
    if (!menu)
        return false;

    QAction *a = aUpdateAvailable = new QAction(this);
    a->setIcon(tkTheme::icon(tkConstants::ICONSOFTWAREUPDATEAVAILABLE));
    tkCommand *cmd = am->registerAction(a, tkConstants::A_VIEWUPDATE, ctx);
    cmd->setTranslations( tkConstants::VIEWUPDATE_TEXT );
    menu->addAction(cmd, tkConstants::G_UPDATE_AVAILABLE);
    cmd->retranslate();
    tkContextManager::instance()->updateContext();
    if (up)
        connect(a, SIGNAL(triggered()), up, SLOT(showUpdateInformations()));
    return true;
}
