/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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

#include <utils/global.h>
#include <utils/updatechecker.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/contextmanager/contextmanager.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/translators.h>
#include <coreplugin/dialogs/debugdialog.h>
#include <coreplugin/dialogs/aboutdialog.h>
#include <coreplugin/dialogs/plugindialog.h>
#include <coreplugin/dialogs/helpdialog.h>
#include <coreplugin/dialogs/settingsdialog.h>

#include <utils/log.h>

#include <QAction>
#include <QStatusBar>
#include <QLabel>
#include <QDesktopServices>
#include <QGridLayout>
#include <QSpacerItem>

using namespace Core;
using namespace Core::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}
static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Utils::UpdateChecker *updateChecker() { return Core::ICore::instance()->updateChecker(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }


MainWindowActionHandler::MainWindowActionHandler(QWidget *parent) :
        QMainWindow(parent),
        aGeneralNew(0), aGeneralOpen(0), aGeneralSave(0), aGeneralSaveAs(0), aGeneralPrint(0), aGeneralPrintPreview(0), aGeneralQuit(0),
        aGeneralPatientNew(0), aGeneralPatientViewIdentity(0), aGeneralPatientRemove(0),
        aGeneralUndo(0), aGeneralRedo(0), aGeneralCut(0), aGeneralCopy(0), aGeneralPaste(0), aGeneralSelectAll(0),
        aGeneralAppPrefs(0), aGeneralAppConfigurator(0), aGeneralPlugsPrefs(0), aGeneralMedinTux(0),
        aGeneralAppAbout(0), aGeneralPlugsAbout(0), aGeneralAppHelp(0), aGeneralQtAbout(0), aGeneralDebugDialog(0),
        aGeneralCheckUpdate(0), aGeneralUpdateAvailable(0),
        aGeneralLockApplication(0),
        aNew(0),
        aOpen(0),
        aSave(0),
        aSaveAs(0),
        aPrint(0),
        aPrintPreview(0),
        aQuit(0),
        aPatientNew(0),
        aPatientViewIdentity(0),
        aPatientRemove(0),
        aUndo(0),
        aRedo(0),
        aCut(0),
        aCopy(0),
        aPaste(0),
        aSelectAll(0),
        aSearch(0),
        aAppPrefs(0),
        aAppConfigurator(0),
        aPlugsPrefs(0),
        aMedinTux(0),
        aLanguageGroup(0),
        aAppAbout(0),
        aAppGoToWebSite(0),
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
    // Save last used language
    settings()->setValue(Constants::S_PREFERREDLANGUAGE, QLocale().name().left(2));
}

void MainWindowActionHandler::createGeneralMenu()
{
    // creates menu bar
    ActionContainer *menubar = menubarContainer(true);

    // Create menu
    ActionContainer *menu = actionManager()->createMenu(Constants::M_GENERAL);
    menubar->addMenu(menu, Constants::G_GENERAL);
    menu->setTranslations(Trans::Constants::GENERAL);
    menu->appendGroup(Id(Constants::G_GENERAL_FILE));
    menu->appendGroup(Id(Constants::G_GENERAL_RECENTS));
    menu->appendGroup(Id(Constants::G_GENERAL_EDIT));
    menu->appendGroup(Id(Constants::G_GENERAL_PATIENTS));
    menu->appendGroup(Id(Constants::G_GENERAL_USERS));
    menu->appendGroup(Id(Constants::G_GENERAL_PRINT));
    menu->appendGroup(Id(Constants::G_GENERAL_CONFIG));
    menu->appendGroup(Id(Constants::G_GENERAL_HELP));
    menu->appendGroup(Id(Constants::G_GENERAL_OTHERS));
    menu->appendGroup(Id(Constants::G_GENERAL_EXIT));
    ActionContainer *newmenu = actionManager()->createMenu(Constants::M_GENERAL_NEW);
    newmenu->setTranslations(Trans::Constants::FILENEW_TEXT);
    menu->addMenu(newmenu, Constants::G_GENERAL_FILE);
    newmenu->appendGroup(Id(Constants::G_GENERAL_NEW));
}

void MainWindowActionHandler::createGeneralActions(const int actions)
{
    if (!actions)
        return;
    Core::Context ctx(Constants::C_GLOBAL);
    ActionContainer *menu = actionManager()->actionContainer(Constants::M_GENERAL);
    Q_ASSERT(menu);
    if (!menu)
        return;
    ActionContainer *newmenu = actionManager()->createMenu(Constants::M_GENERAL_NEW);
    Q_ASSERT(newmenu);
    if (!newmenu)
        return;

    QAction *a = 0;
    Command *cmd = 0;
//    QString group = Constants::G_GENERAL_FILE;
    Id group(Constants::G_GENERAL_FILE);

    // New File Action
    if (actions & Core::MainWindowActions::A_FileNew) {
        a = aGeneralNew = new QAction(this);
        a->setIcon(QIcon(Constants::ICONFILENEW));
        cmd = actionManager()->registerAction(a, Constants::A_FILE_NEW, ctx);
        cmd->setDefaultKeySequence(QKeySequence::New);
        cmd->setTranslations(Trans::Constants::FILENEW_TEXT);
        newmenu->addAction(cmd, Constants::G_GENERAL_NEW);
    }

    // Open Action
    if (actions & Core::MainWindowActions::A_FileOpen) {
        // Create action
        a = aGeneralOpen = new QAction(this);
        a->setIcon(theme()->icon(Constants::ICONOPEN));
        cmd = actionManager()->registerAction(a, Constants::A_FILE_OPEN, ctx);
        cmd->setDefaultKeySequence(QKeySequence::Open);
        cmd->setTranslations(Trans::Constants::FILEOPEN_TEXT );
        menu->addAction(cmd, group);
    }

    // Save Action
    if (actions & Core::MainWindowActions::A_FileSave) {
        a = aGeneralSave = new QAction(this);
        a->setIcon(theme()->icon(Constants::ICONSAVE));
        cmd = actionManager()->registerAction(a, Constants::A_FILE_SAVE, ctx);
        cmd->setDefaultKeySequence(QKeySequence::Save);
        cmd->setTranslations(Trans::Constants::FILESAVE_TEXT );
//        cmd->setAttribute(Command::CA_UpdateText);
        menu->addAction(cmd, group);
    }

    // SaveAs Action
    if (actions & Core::MainWindowActions::A_FileSaveAs) {
        a = aGeneralSaveAs = new QAction(this);
        a->setIcon(theme()->icon(Constants::ICONSAVEAS));
        cmd = actionManager()->registerAction(a, Constants::A_FILE_SAVEAS, ctx);
#if QT_VERSION >= 0x040500
        cmd->setDefaultKeySequence(QKeySequence::SaveAs);
#else
        cmd->setDefaultKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S);
#endif
        cmd->setTranslations(Trans::Constants::FILESAVEAS_TEXT );
        menu->addAction(cmd, group);
    }

    group = Id(Constants::G_GENERAL_PRINT);

    // Print Action
    if (actions & Core::MainWindowActions::A_FilePrint) {
        a = aGeneralPrint = new QAction(this);
        a->setIcon(theme()->icon(Constants::ICONPRINT));
        cmd = actionManager()->registerAction(a, Constants::A_FILE_PRINT, ctx);
        cmd->setDefaultKeySequence(QKeySequence::Print);
        cmd->setTranslations(Trans::Constants::FILEPRINT_TEXT );
        menu->addAction(cmd, group);
    }

    // Print Preview
    if (actions & Core::MainWindowActions::A_FilePrintPreview) {
        a = aGeneralPrintPreview = new QAction(this);
        a->setIcon(theme()->icon(Constants::ICONPRINTPREVIEW));
        cmd = actionManager()->registerAction(a, Constants::A_FILE_PRINTPREVIEW, ctx);
//        cmd->setDefaultKeySequence(QKeySequence::Print);
        cmd->setTranslations(Trans::Constants::PRINTPREVIEW_TEXT );
        menu->addAction(cmd, group);
    }

    group = Id(Constants::G_GENERAL_EXIT);
    // Lock application
#ifdef WITH_UI_AUTO_LOCKING
    if (actions & Core::MainWindowActions::A_FileLockApplication) {
        a = aGeneralLockApplication = new QAction(this);
        a->setIcon(theme()->icon(Constants::ICONLOCK));
        cmd = actionManager()->registerAction(a, Constants::A_FILE_LOCKAPPLICATION, ctx);
        cmd->setTranslations(Trans::Constants::FILELOCKAPPLICATION_TEXT);
        menu->addAction(cmd, group);
    }
#endif

    // Quit Action
    if (actions & Core::MainWindowActions::A_FileQuit) {
        a = aGeneralQuit = new QAction(this);
        a->setIcon(theme()->icon(Constants::ICONQUIT));
        cmd = actionManager()->registerAction(a, Constants::A_FILE_EXIT, ctx);
        cmd->setTranslations(Trans::Constants::FILEEXIT_TEXT );
        cmd->setDefaultKeySequence(QKeySequence::Quit);
#ifdef Q_OS_MAC
        cmd->action()->setMenuRole(QAction::QuitRole);
#endif
        menu->addAction(cmd, group);
    }

    group = Id(Constants::G_GENERAL_PATIENTS);
    // Patient's new
    if (actions & Core::MainWindowActions::A_Patients_New) {
        a = aGeneralPatientNew = new QAction(this);
        a->setObjectName("aGeneralPatientNew");
        QIcon icon;
        icon.addFile(theme()->iconFullPath(Constants::ICONPATIENTNEW, Core::ITheme::SmallIcon), QSize(16, 16));
        icon.addFile(theme()->iconFullPath(Constants::ICONPATIENTNEW, Core::ITheme::MediumIcon), QSize(32, 32));
        a->setIcon(icon);
        cmd = actionManager()->registerAction(a, Constants::A_PATIENT_NEW, ctx);
        cmd->setDefaultKeySequence(Qt::CTRL + Qt::Key_N);
        cmd->setTranslations(Trans::Constants::PATIENTNEW_TEXT);
        newmenu->addAction(cmd, Constants::G_GENERAL_NEW);
    }
    // Patient's identity
    if (actions & Core::MainWindowActions::A_Patients_ViewIdentity) {
        a = aGeneralPatientViewIdentity = new QAction(this);
        a->setObjectName("aGeneralPatientViewIdentity");
        a->setIcon(theme()->icon(Constants::ICONPATIENT));
        cmd = actionManager()->registerAction(a, Constants::A_PATIENT_VIEWIDENTITY, ctx);
        cmd->setTranslations(Trans::Constants::PATIENTVIEWIDENTITY_TEXT);
        menu->addAction(cmd, group);
    }
    // Patient's remove
    if (actions & Core::MainWindowActions::A_Patients_Remove) {
        a = aGeneralPatientRemove = new QAction(this);
        a->setObjectName("aGeneralPatientRemove");
        a->setIcon(theme()->icon(Constants::ICONPATIENT));
        cmd = actionManager()->registerAction(a, Constants::A_PATIENT_REMOVE, ctx);
        cmd->setTranslations(Trans::Constants::PATIENTREMOVE_TEXT);
        menu->addAction(cmd, group);
    }

    group = Id(Constants::G_GENERAL_CONFIG);

    // Configuration
    if (actions & Core::MainWindowActions::A_AppConfigurator) {
        a = aGeneralAppConfigurator = new QAction(this);
        a->setObjectName("aGeneralAppConfigurator");
        a->setIcon(theme()->icon(Constants::ICONPREFERENCES));
        cmd = actionManager()->registerAction(a, Constants::A_APPCONFIGURATOR, ctx);
        cmd->setTranslations(Trans::Constants::APPCONFIGURATOR_TEXT);
        menu->addAction(cmd, group);
    }
    if (actions & Core::MainWindowActions::A_AppPreferences) {
        a = aGeneralAppPrefs = new QAction(this);
        a->setObjectName("aGeneralAppPrefs");
        a->setIcon(theme()->icon(Constants::ICONPREFERENCES));
        cmd = actionManager()->registerAction(a, Constants::A_PREFERENCES, ctx);
        cmd->setTranslations(Trans::Constants::PREFERENCES_TEXT);
#ifdef Q_OS_MAC
        cmd->setDefaultKeySequence(QKeySequence(tr("Ctrl+,")));
        cmd->action()->setMenuRole(QAction::PreferencesRole);
#endif
        menu->addAction(cmd, group);
    }
    if (actions & Core::MainWindowActions::A_PluginsPreferences) {
        a = aGeneralPlugsPrefs = new QAction(this);
        a->setObjectName("aGeneralPlugsPrefs");
        a->setIcon(theme()->icon(Constants::ICONPLUGIN));
        cmd = actionManager()->registerAction(a, Constants::A_PLUGINS_PREFERENCES, ctx);
        // cmd->setDefaultKeySequence(QKeySequence::SelectAll);
        cmd->setTranslations(Trans::Constants::PLUGINS_CATEGORY);
        menu->addAction(cmd, group);
    }
    if (actions & Core::MainWindowActions::A_ConfigureMedinTux) {
        a = aGeneralMedinTux = new QAction(this);
        a->setObjectName("aGeneralMedinTux");
        a->setIcon(theme()->icon(Constants::ICONMEDINTUX));
        cmd = actionManager()->registerAction(a, Constants::A_CONFIGURE_MEDINTUX, ctx);
        cmd->setTranslations(Trans::Constants::CONFIGMEDINTUX_TEXT);
        menu->addAction(cmd, group);
    }

    group = Id(Constants::G_GENERAL_HELP);

    // Help menu
    if (actions & Core::MainWindowActions::A_AppAbout) {
        a = aGeneralAppAbout = new QAction(this);
        a->setIcon(theme()->icon(Constants::ICONABOUT));
        a->setMenuRole(QAction::AboutRole);
        cmd = actionManager()->registerAction(a, Constants::A_ABOUT, ctx);
        if (Utils::isRunningOnMac())
            cmd->setTranslations(Trans::Constants::MAC_ABOUT_TEXT);
        else
            cmd->setTranslations(Trans::Constants::ABOUT_TEXT);
        menu->addAction(cmd, group);
    }
    if (actions & Core::MainWindowActions::A_PluginsAbout) {
        a = aGeneralPlugsAbout = new QAction(this);
        a->setIcon(theme()->icon(Constants::ICONPLUGIN));
//        a->setMenuRole(QAction::AboutRole);
        cmd = actionManager()->registerAction(a, Constants::A_ABOUTPLUGINS, ctx);
        if (Utils::isRunningOnMac())
            cmd->setTranslations(Trans::Constants::MAC_ABOUTPLUGINS_TEXT);
        else
            cmd->setTranslations(Trans::Constants::ABOUTPLUGINS_TEXT);
        menu->addAction(cmd, group);
    }
    if (actions & Core::MainWindowActions::A_AppHelp) {
        a = aGeneralAppHelp = new QAction(this);
        a->setIcon(theme()->icon(Constants::ICONHELP));
        cmd = actionManager()->registerAction(a, Constants::A_APPLICATIONHELP, ctx);
        cmd->setDefaultKeySequence(QKeySequence::HelpContents);
        cmd->setTranslations(Trans::Constants::HELP_TEXT);
        menu->addAction(cmd, group);
    }
    if (actions & Core::MainWindowActions::A_QtAbout) {
        a = aGeneralQtAbout = new QAction(this);
        a->setIcon(theme()->icon(Constants::ICONABOUT));
        a->setMenuRole(QAction::AboutQtRole);
        cmd = actionManager()->registerAction(a, Constants::A_ABOUTQT, ctx);
        cmd->setTranslations(Trans::Constants::ABOUTQT_TEXT);
        menu->addAction(cmd, group);
    }
    if (actions & Core::MainWindowActions::A_DebugDialog) {
        a = aGeneralDebugDialog = new QAction(this);
        a->setIcon(theme()->icon(Constants::ICONHELP));
        cmd = actionManager()->registerAction(a, Constants::A_DEBUGHELPER, ctx);
        cmd->setTranslations(Trans::Constants::DEBUGHELPER_TEXT);
        menu->addAction(cmd, group);
    }
    if (actions & Core::MainWindowActions::A_CheckUpdate) {
        a = aGeneralCheckUpdate = new QAction(this);
        a->setIcon(theme()->icon(Constants::ICONSOFTWAREUPDATEAVAILABLE));
        cmd = actionManager()->registerAction(a, Constants::A_CHECKUPDATE, ctx);
        cmd->setTranslations(Trans::Constants::CHECKUPDATE);
        menu->addAction(cmd, group);
    }

}

void MainWindowActionHandler::connectGeneralActions()
{
    if (aGeneralNew)
        connect(aGeneralNew, SIGNAL(triggered()), this, SLOT(newFile()));

    if (aGeneralOpen)
        connect(aGeneralOpen, SIGNAL(triggered()), this, SLOT(openFile()));

    if (aGeneralSave)
        connect(aGeneralSave, SIGNAL(triggered()), this, SLOT(saveFile()));

    if (aGeneralSaveAs)
        connect(aGeneralSaveAs, SIGNAL(triggered()), this, SLOT(saveAsFile()));

    if (aGeneralPrint)
        connect(aGeneralPrint, SIGNAL(triggered()), this, SLOT(print()));

    if (aGeneralPrintPreview)
        connect(aGeneralPrintPreview, SIGNAL(triggered()), this, SLOT(printPreview()));

    if (aGeneralLockApplication)
        connect(aGeneralLockApplication, SIGNAL(triggered()), this, SLOT(lockApplication()));
    if (aGeneralQuit)
        connect(aGeneralQuit, SIGNAL(triggered()), this, SLOT(close()));

    if (aGeneralPatientNew)
        connect(aGeneralPatientNew, SIGNAL(triggered()), this, SLOT(createNewPatient()));
    if (aGeneralPatientViewIdentity)
        connect(aGeneralPatientViewIdentity, SIGNAL(triggered()), this, SLOT(viewPatientIdentity()));
    if (aGeneralPatientRemove)
        connect(aGeneralPatientRemove, SIGNAL(triggered()), this, SLOT(removePatient()));
    if (aGeneralAppPrefs)
        connect(aGeneralAppPrefs, SIGNAL(triggered()), this, SLOT(applicationPreferences()));
    if (aGeneralAppConfigurator)
        connect(aGeneralAppConfigurator, SIGNAL(triggered()), this, SLOT(applicationConfiguratorWizard()));
    if (aGeneralMedinTux)
        connect(aGeneralMedinTux, SIGNAL(triggered()), this, SLOT(configureMedintux()));
    if (aGeneralAppAbout)
        connect(aGeneralAppAbout, SIGNAL(triggered()), this, SLOT(aboutApplication()));

    if (aGeneralPlugsAbout)
        connect(aGeneralPlugsAbout, SIGNAL(triggered()), this, SLOT(aboutPlugins()));

    if (aGeneralAppHelp)
        connect(aGeneralAppHelp, SIGNAL(triggered()), this, SLOT(applicationHelp()));

    if (aGeneralQtAbout)
        connect(aGeneralQtAbout, SIGNAL(triggered()), this, SLOT(aboutQt()));

    if (aGeneralDebugDialog)
        connect(aGeneralDebugDialog, SIGNAL(triggered()), this, SLOT(debugDialog()));

    if (aGeneralCheckUpdate)
        connect(aGeneralCheckUpdate, SIGNAL(triggered()), this, SLOT(checkUpdate()));
}

Core::ActionContainer *MainWindowActionHandler::menubarContainer(bool createIfNotExist)
{
    // creates menu bar
    ActionContainer *mb = actionManager()->actionContainer(Constants::MENUBAR);
    if (createIfNotExist && !mb) {
        mb = actionManager()->createMenuBar(Constants::MENUBAR);
        mb->appendGroup(Core::Constants::G_GENERAL);
        mb->appendGroup(Core::Constants::G_FILE);
        mb->appendGroup(Core::Constants::G_EDIT);
        mb->appendGroup(Core::Constants::G_FORMAT);
        mb->appendGroup(Core::Constants::G_PLUGINS);
        mb->appendGroup(Core::Constants::G_TEMPLATES);
        mb->appendGroup(Core::Constants::G_PATIENTS);
        mb->appendGroup(Core::Constants::G_CONFIGURATION);
        mb->appendGroup(Core::Constants::G_HELP);
        mb->appendGroup(Core::Constants::G_UPDATE);
#ifndef Q_WS_MAC
        setMenuBar(mb->menuBar());
#endif
    }
    return mb;
}

/** \brief Menu is created in the global context \sa Constants::C_GLOBAL. Menu bar is automaticcaly created if necessary. */
void MainWindowActionHandler::createFileMenu()
{
    // creates menu bar
    ActionContainer *menubar = menubarContainer(true);

    // Create menu
    ActionContainer *filemenu = actionManager()->createMenu(Constants::M_FILE);
    menubar->addMenu(filemenu, Constants::G_FILE);
    filemenu->setTranslations(Trans::Constants::M_FILE_TEXT);
    filemenu->appendGroup(Id(Constants::G_FILE_NEW));
    filemenu->appendGroup(Id(Constants::G_FILE_OPEN));
    filemenu->appendGroup(Id(Constants::G_FILE_RECENTS));
    filemenu->appendGroup(Id(Constants::G_FILE_PROJECT));
    filemenu->appendGroup(Id(Constants::G_FILE_SAVE));
    filemenu->appendGroup(Id(Constants::G_FILE_PRINT));
    filemenu->appendGroup(Id(Constants::G_FILE_OTHER));
    filemenu->appendGroup(Id(Constants::G_FILE_CLOSE));
    filemenu->appendGroup(Id(Constants::G_FILE_EXIT));
    ActionContainer *recentmenu = actionManager()->createMenu(Constants::M_FILE_RECENTFILES);
    recentmenu->setTranslations(Trans::Constants::M_FILE_RECENTFILES_TEXT);
    filemenu->addMenu(recentmenu,Constants::G_FILE_RECENTS);
}

/** \brief Create the File > New sub menu. Must firstly create the file menu. */
void MainWindowActionHandler::createFileNewSubMenu()
{
    ActionContainer *filemenu = actionManager()->actionContainer(Constants::M_FILE);
    if (!filemenu) {
        LOG_ERROR("You must create the file menu first");
        return;
    }
    ActionContainer *newmenu = actionManager()->createMenu(Constants::M_FILE_NEW);
    newmenu->setTranslations(Trans::Constants::FILENEW_TEXT);
    filemenu->addMenu(newmenu, Constants::G_FILE_NEW);
    newmenu->appendGroup(Id(Constants::G_FILE_NEW));
}

void MainWindowActionHandler::createTemplatesMenu()
{
    ActionContainer *menubar = menubarContainer();
    Q_ASSERT(menubar);

    // Create menu
    ActionContainer *menu = actionManager()->createMenu(Constants::M_TEMPLATES);
    menubar->addMenu(menu, Constants::G_TEMPLATES);
    menu->setTranslations(Trans::Constants::TEMPLATES);
    menu->appendGroup(Id(Constants::G_TEMPLATES));
    menu->appendGroup(Id(Constants::G_TEMPLATES_NEW));
    menu->appendGroup(Id(Constants::G_TEMPLATES_MANAGER));
    menu->appendGroup(Id(Constants::G_TEMPLATES_EXTRAS));
    menu->appendGroup(Id(Constants::G_TEMPLATES_OTHERS));
}

/** \brief Menu is created in the global context \sa Constants::C_GLOBAL.*/
void MainWindowActionHandler::createEditMenu()
{
    ActionContainer *menubar = menubarContainer();
    Q_ASSERT(menubar);

    // Edit Menu groups
    ActionContainer *editmenu = actionManager()->createMenu(Constants::M_EDIT);
    menubar->addMenu(editmenu, Constants::G_EDIT);
    editmenu->setTranslations(Trans::Constants::M_EDIT_TEXT);
    editmenu->appendGroup(Id(Constants::G_EDIT));
    editmenu->appendGroup(Id(Constants::G_EDIT_UNDOREDO));
    editmenu->appendGroup(Id(Constants::G_EDIT_COPYPASTE));
    editmenu->appendGroup(Id(Constants::G_EDIT_SELECT));
    editmenu->appendGroup(Id(Constants::G_EDIT_LIST));
    editmenu->appendGroup(Id(Constants::G_EDIT_ADVANCED));
    editmenu->appendGroup(Id(Constants::G_EDIT_FIND));
    editmenu->appendGroup(Id(Constants::G_EDIT_EDITOR));
    editmenu->appendGroup(Id(Constants::G_EDIT_OTHER));
}

/** \brief Menu is created in the global context \sa Constants::C_GLOBAL.*/
void MainWindowActionHandler::createPatientMenu()
{
    ActionContainer *menubar = menubarContainer();
    Q_ASSERT(menubar);

    ActionContainer *menu = actionManager()->createMenu(Constants::M_PATIENTS);
    menu->appendGroup(Core::Constants::G_PATIENTS);
    menu->appendGroup(Core::Constants::G_PATIENT_NAVIGATION);
    menu->appendGroup(Core::Constants::G_PATIENT_INFORMATION);

    menubar->addMenu(menu, Constants::G_PATIENTS);
    menu->setTranslations(Trans::Constants::PATIENTS);

    ActionContainer *historyMenu = actionManager()->createMenu(Constants::M_PATIENTS_NAVIGATION);
    historyMenu->setTranslations(Trans::Constants::M_PATIENT_NAVIGATION_TEXT);
    menu->addMenu(historyMenu, Constants::G_PATIENT_NAVIGATION);
}

/** \brief Menu is created in the global context \sa Constants::C_GLOBAL.*/
void MainWindowActionHandler::createFormatMenu()
{
    ActionContainer *menubar = menubarContainer();
    Q_ASSERT(menubar);

    ActionContainer *formatmenu = actionManager()->createMenu(Constants::M_FORMAT);
    menubar->addMenu(formatmenu, Constants::G_FORMAT);
    formatmenu->setTranslations(Trans::Constants::M_FORMAT_TEXT);
    formatmenu->appendGroup(Id(Constants::G_FORMAT_FONT));
    formatmenu->appendGroup(Id(Constants::G_FORMAT_PARAGRAPH));
    formatmenu->appendGroup(Id(Constants::G_FORMAT_TABLE));
    formatmenu->appendGroup(Id(Constants::G_FORMAT_IMAGE));
    formatmenu->appendGroup(Id(Constants::G_FORMAT_OTHER));
}

void MainWindowActionHandler::createPluginsMenu()
{
    ActionContainer *menubar = menubarContainer(true);
    Q_ASSERT(menubar);

    ActionContainer *confmenu = actionManager()->createMenu(Constants::M_PLUGINS);
    //    confmenu->setEmptyAction(ActionContainer::EA_Hide);
    menubar->addMenu(confmenu, Constants::G_PLUGINS);
    confmenu->setTranslations(Trans::Constants::M_PLUGINS_TEXT);
    confmenu->appendGroup(Id(Constants::G_PLUGINS_USERMANAGER));
    confmenu->appendGroup(Id(Constants::G_PLUGINS_FORM));
    confmenu->appendGroup(Id(Constants::G_PLUGINS_DRUGS));
    confmenu->appendGroup(Id(Constants::G_PLUGINS_CALENDAR));
    confmenu->appendGroup(Id(Constants::G_PLUGINS_PADTOOLS));
    confmenu->appendGroup(Id(Constants::G_PLUGINS_ACCOUNT));
    confmenu->appendGroup(Id(Constants::G_PLUGINS_OTHERS));
}

/** \brief Menu is created in the global context \sa Constants::C_GLOBAL.*/
void MainWindowActionHandler::createConfigurationMenu()
{
    ActionContainer *menubar = menubarContainer(true);
    Q_ASSERT(menubar);

    ActionContainer *confmenu = actionManager()->createMenu(Constants::M_CONFIGURATION);
    //    confmenu->setEmptyAction(ActionContainer::EA_Hide);
    menubar->addMenu(confmenu, Constants::G_CONFIGURATION);
    confmenu->setTranslations(Trans::Constants::M_CONFIGURATION_TEXT);
    confmenu->appendGroup(Id(Constants::G_APP_CONFIGURATION));
    confmenu->appendGroup(Id(Constants::G_TOOLBARS));
    confmenu->appendGroup(Id(Constants::G_LANGUAGES));
    confmenu->appendGroup(Id(Constants::G_PREFERENCES));
}

/** \brief Menu is created in the global context \sa Constants::C_GLOBAL.*/
void MainWindowActionHandler::createHelpMenu()
{
    ActionContainer *menubar = menubarContainer(true);
    Q_ASSERT(menubar);

    ActionContainer *m = actionManager()->createMenu(Constants::M_HELP);
    menubar->addMenu(m, Constants::G_HELP);
    m->setTranslations(Trans::Constants::M_ABOUT_TEXT);
    m->appendGroup(Id(Constants::G_HELP_HELP));
    m->appendGroup(Id(Constants::G_UPDATE));
    m->appendGroup(Id(Constants::G_HELP_DEBUG));
    m->appendGroup(Id(Constants::G_HELP_OTHER));
    m->appendGroup(Id(Constants::G_HELP_DATABASES));
    m->appendGroup(Id(Constants::G_HELP_ABOUT));

    ActionContainer *mdb = actionManager()->createMenu(Constants::M_HELP_DATABASES);
    m->addMenu(mdb, Constants::G_HELP_DATABASES);
    mdb->appendGroup(Id(Constants::G_HELP_DATABASES));
    mdb->setTranslations(Trans::Constants::ABOUTDATABASES_TEXT);
}

/** \brief Menu is created in the global context \sa Constants::C_GLOBAL.*/
void MainWindowActionHandler::createUpdateMenu()
{
    ActionContainer *menubar = menubarContainer(true);
    Q_ASSERT(menubar);

    // Upadate Menu groups
    ActionContainer *upmenu = actionManager()->createMenu(Constants::M_UPDATE);
    menubar->addMenu(upmenu, Constants::G_UPDATE);
    upmenu->setTranslations(Trans::Constants::M_UPDATE_TEXT);
    upmenu->appendGroup(Id(Constants::G_UPDATE_AVAILABLE));
    upmenu->appendGroup(Id(Constants::G_UPDATE_TODATE));
    upmenu->appendGroup(Id(Constants::G_UPDATE_OTHER));
}

/** \brief Actions are created in the global context \sa Constants::C_GLOBAL */
void MainWindowActionHandler::createFileActions(int actions)
{
    if (!actions)
        return;
    Core::Context ctx(Constants::C_GLOBAL);
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
        mfile->addAction(cmd, Id(Constants::G_FILE_NEW));
    }

    // Open Action
    if (actions & Core::MainWindowActions::A_FileOpen) {
        // Create action
        a = aOpen = new QAction(this);
        a->setIcon(theme()->icon(Constants::ICONOPEN));
        cmd = actionManager()->registerAction(a, Constants::A_FILE_OPEN, ctx);
        cmd->setDefaultKeySequence(QKeySequence::Open);
        cmd->setTranslations(Trans::Constants::FILEOPEN_TEXT );
//        cmd->setAttribute(Command::CA_UpdateText);
        mfile->addAction(cmd, Id(Constants::G_FILE_OPEN));
    }

    // Save Action
    if (actions & Core::MainWindowActions::A_FileSave) {
        a = aSave = new QAction(this);
        a->setIcon(theme()->icon(Constants::ICONSAVE));
        cmd = actionManager()->registerAction(a, Constants::A_FILE_SAVE, ctx);
        cmd->setDefaultKeySequence(QKeySequence::Save);
        cmd->setTranslations(Trans::Constants::FILESAVE_TEXT );
//        cmd->setAttribute(Command::CA_UpdateText);
        mfile->addAction(cmd, Id(Constants::G_FILE_SAVE));
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
        mfile->addAction(cmd, Id(Constants::G_FILE_SAVE));
    }

    // Print Action
    if (actions & Core::MainWindowActions::A_FilePrint) {
        a = aPrint = new QAction(this);
        a->setIcon(theme()->icon(Constants::ICONPRINT));
        cmd = actionManager()->registerAction(a, Constants::A_FILE_PRINT, ctx);
        cmd->setDefaultKeySequence(QKeySequence::Print);
        cmd->setTranslations(Trans::Constants::FILEPRINT_TEXT );
        mfile->addAction(cmd, Id(Constants::G_FILE_PRINT));
    }

    // Print Preview
    if (actions & Core::MainWindowActions::A_FilePrintPreview) {
        a = aPrintPreview = new QAction(this);
        a->setIcon(theme()->icon(Constants::ICONPRINTPREVIEW));
        cmd = actionManager()->registerAction(a, Constants::A_FILE_PRINTPREVIEW, ctx);
//        cmd->setDefaultKeySequence(QKeySequence::Print);
        cmd->setTranslations(Trans::Constants::PRINTPREVIEW_TEXT );
        mfile->addAction(cmd, Id(Constants::G_FILE_PRINT));
    }

    // Quit Action
    if (actions & Core::MainWindowActions::A_FileQuit) {
        a = aQuit = new QAction(this);
        a->setIcon(theme()->icon(Constants::ICONQUIT));
        cmd = actionManager()->registerAction(a, Constants::A_FILE_EXIT, ctx);
        cmd->setTranslations(Trans::Constants::FILEEXIT_TEXT );
        cmd->action()->setMenuRole(QAction::QuitRole);
        cmd->setDefaultKeySequence(QKeySequence::Quit);
        mfile->addAction(cmd, Id(Constants::G_FILE_EXIT));
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

    if (aPrintPreview)
        connect(aPrintPreview, SIGNAL(triggered()), this, SLOT(printPreview()));

    if (aQuit)
        connect(aQuit, SIGNAL(triggered()), this, SLOT(close()));
}

void MainWindowActionHandler::createEditActions()
{
//    QList<int> ctx = QList<int>() << Constants::C_GLOBAL;
    Core::Context ctx(Constants::C_GLOBAL);
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
    medit->addAction(cmd, Id(Constants::G_EDIT_UNDOREDO));
    a->setEnabled(false);

    // Redo Action
    a = aRedo = new QAction(this);
    a->setObjectName("aRedo");
    a->setIcon(theme()->icon(Constants::ICONREDO));
    cmd = actionManager()->registerAction(a, Constants::A_EDIT_REDO, ctx);
    cmd->setDefaultKeySequence(QKeySequence::Redo);
    //    cmd->setAttribute(Command::CA_UpdateText);
    cmd->setTranslations(Trans::Constants::EDITREDO_TEXT);
    medit->addAction(cmd, Id(Constants::G_EDIT_UNDOREDO));
    a->setEnabled(false);

    // Cut Action
    a = aCut = new QAction(this);
    a->setObjectName("aCut");
    a->setIcon(theme()->icon(Constants::ICONCUT));
    cmd = actionManager()->registerAction(a, Constants::A_EDIT_CUT, ctx);
    cmd->setDefaultKeySequence(QKeySequence::Cut);
    cmd->setTranslations(Trans::Constants::EDITCUT_TEXT );
    medit->addAction(cmd, Id(Constants::G_EDIT_COPYPASTE));
    a->setEnabled(false);

    // Copy Action
    a = aCopy = new QAction(this);
    a->setObjectName("aCopy");
    a->setIcon(theme()->icon(Constants::ICONCOPY));
    cmd = actionManager()->registerAction(a, Constants::A_EDIT_COPY, ctx);
    cmd->setDefaultKeySequence(QKeySequence::Copy);
    cmd->setTranslations(Trans::Constants::EDITCOPY_TEXT );
    medit->addAction(cmd, Id(Constants::G_EDIT_COPYPASTE));
    a->setEnabled(false);

    // Paste Action
    a = aPaste = new QAction(this);
    a->setObjectName("aPaste");
    a->setIcon(theme()->icon(Constants::ICONPASTE));
    cmd = actionManager()->registerAction(a, Constants::A_EDIT_PASTE, ctx);
    cmd->setDefaultKeySequence(QKeySequence::Paste);
    cmd->setTranslations(Trans::Constants::EDITPASTE_TEXT );
    medit->addAction(cmd, Id(Constants::G_EDIT_COPYPASTE));
    a->setEnabled(false);

    // SelectAll Action
    a = aSelectAll = new QAction(this);
    a->setObjectName("aSelectAll");
    a->setIcon(theme()->icon(Constants::ICONSELECTALL));
    cmd = actionManager()->registerAction(a, Constants::A_EDIT_SELECTALL, ctx);
    cmd->setDefaultKeySequence(QKeySequence::SelectAll);
    cmd->setTranslations(Trans::Constants::EDITSELECTALL_TEXT );
    medit->addAction(cmd, Id(Constants::G_EDIT_COPYPASTE));
    a->setEnabled(false);

    // Search Action
    a = aSearch = new QAction(this);
    a->setObjectName("aSearch");
    a->setIcon(theme()->icon(Constants::ICONSEARCH));
    cmd = actionManager()->registerAction(a, Constants::A_EDIT_SEARCH, ctx);
    cmd->setDefaultKeySequence(QKeySequence::Find);
    cmd->setTranslations(Trans::Constants::SEARCHMENU_TEXT);
    medit->addAction(cmd, Id(Constants::G_EDIT_FIND));
    a->setEnabled(false);
}

// TODO: actually does nothing useful
void MainWindowActionHandler::connectEditActions()
{
//    if (aUndo) {}
//    if (aRedo) {}
//    if (aCut) {}
//    if (aCopy) {}
//    if (aPaste) {}
//    if (aSelectAll) {}
//    if (aSearch) {}
}

/** \brief Actions are created in the global context \sa Constants::C_GLOBAL */
void MainWindowActionHandler::createPatientsActions(int actions)
{
    // TODO: Patients actions should be exploded into different menus or in a single menu ?
    if (!actions)
        return;

    QAction *a = 0;
    Command *cmd = 0;
    Core::Context ctx(Constants::C_GLOBAL);
    ActionContainer *menu = actionManager()->actionContainer(Constants::M_PATIENTS);
    Q_ASSERT(menu);
    if (!menu)
        return;

    if (actions & Core::MainWindowActions::A_Patients_New) {
        a = aPatientNew = new QAction(this);
        a->setObjectName("aPatientNew");
        a->setIcon(theme()->icon(Constants::ICONPATIENT));
        cmd = actionManager()->registerAction(a, Constants::A_PATIENT_NEW, ctx);
        cmd->setDefaultKeySequence(Qt::CTRL + Qt::Key_N);
        cmd->setTranslations(Trans::Constants::PATIENTNEW_TEXT);
        menu->addAction(cmd, Id(Constants::G_PATIENTS));
    }
    if (actions & Core::MainWindowActions::A_Patients_ViewIdentity) {
        a = aPatientViewIdentity = new QAction(this);
        a->setObjectName("aPatientViewIdentity");
        a->setIcon(theme()->icon(Constants::ICONPATIENT));
        cmd = actionManager()->registerAction(a, Constants::A_PATIENT_VIEWIDENTITY, ctx);
        cmd->setTranslations(Trans::Constants::PATIENTVIEWIDENTITY_TEXT);
        menu->addAction(cmd, Id(Constants::G_PATIENTS));
    }
    if (actions & Core::MainWindowActions::A_Patients_Remove) {
        a = aPatientRemove = new QAction(this);
        a->setObjectName("aPatientRemove");
        a->setIcon(theme()->icon(Constants::ICONPATIENT));
        cmd = actionManager()->registerAction(a, Constants::A_PATIENT_REMOVE, ctx);
        cmd->setTranslations(Trans::Constants::PATIENTREMOVE_TEXT);
        menu->addAction(cmd, Id(Constants::G_PATIENTS));
    }
}

/** \brief Connect created Patients' menu actions to there standard slots. */
void MainWindowActionHandler::connectPatientActions()
{
    if (aPatientNew)
        connect(aPatientNew, SIGNAL(triggered()), this, SLOT(createNewPatient()));
    if (aPatientViewIdentity)
        connect(aPatientViewIdentity, SIGNAL(triggered()), this, SLOT(viewPatientIdentity()));
    if (aPatientRemove)
        connect(aPatientRemove, SIGNAL(triggered()), this, SLOT(removePatient()));
}

void MainWindowActionHandler::createConfigurationActions(int actions)
{
    if (!actions)
        return;

    QAction *a = 0;
    Command *cmd = 0;
//    QList<int> ctx = QList<int>() << Constants::C_GLOBAL;
    Core::Context ctx(Constants::C_GLOBAL);
    ActionContainer *menu = actionManager()->actionContainer(Constants::M_CONFIGURATION);
    Q_ASSERT(menu);
    if (!menu)
        return;

    if (actions & Core::MainWindowActions::A_AppConfigurator) {
        a = aAppConfigurator = new QAction(this);
        a->setObjectName("aAppConfigurator");
        a->setIcon(theme()->icon(Constants::ICONPREFERENCES));
        cmd = actionManager()->registerAction(a, Constants::A_APPCONFIGURATOR, ctx);
        cmd->setTranslations(Trans::Constants::APPCONFIGURATOR_TEXT);
        menu->addAction(cmd, Id(Constants::G_APP_CONFIGURATION));
    }
    if (actions & Core::MainWindowActions::A_AppPreferences) {
        a = aAppPrefs = new QAction(this);
        a->setObjectName("aAppPrefs");
        a->setIcon(theme()->icon(Constants::ICONPREFERENCES));
        cmd = actionManager()->registerAction(a, Constants::A_PREFERENCES, ctx);
        cmd->setTranslations(Trans::Constants::PREFERENCES_TEXT);
#ifdef Q_OS_MAC
        cmd->setDefaultKeySequence(QKeySequence(tr("Ctrl+,")));
        cmd->action()->setMenuRole(QAction::PreferencesRole);
#endif
        menu->addAction(cmd, Id(Constants::G_APP_CONFIGURATION));
    }
    if (actions & Core::MainWindowActions::A_ConfigureMedinTux) {
        a = aMedinTux = new QAction(this);
        a->setObjectName("aMedinTux");
        a->setIcon(theme()->icon(Constants::ICONMEDINTUX));
        cmd = actionManager()->registerAction(a, Constants::A_CONFIGURE_MEDINTUX, ctx);
        cmd->setTranslations(Trans::Constants::CONFIGMEDINTUX_TEXT);
        menu->addAction(cmd, Id(Constants::G_PREFERENCES));
    }
    if (actions & Core::MainWindowActions::A_LanguageChange) {
        aLanguageGroup = new QActionGroup(this);
        // create language menu
        ActionContainer *menuconf = actionManager()->actionContainer(Constants::M_CONFIGURATION);
        Q_ASSERT(menuconf);
        ActionContainer *lmenu = actionManager()->createMenu(Constants::M_LANGUAGES);
        menuconf->addMenu(lmenu, Constants::G_LANGUAGES);
        lmenu->appendGroup(Id(Constants::G_LANGUAGES));
        lmenu->setTranslations(Trans::Constants::M_LANGUAGES_TEXT);

        // populate with actions
        const QMap<QString, QString> &loc_lang = Core::ICore::instance()->translators()->availableLocalesAndLanguages();
        int i = 0;

        foreach(const QString &loc, loc_lang.keys()) {
            ++i;
            QAction *action = new QAction(this);  // QString("&%1 %2").arg(QString::number(i), loc_lang.value(loc)), this);
            action->setText(loc_lang.value(loc));
            action->setObjectName(loc);
            action->setData(loc);
            action->setCheckable(true);
            cmd = actionManager()->registerAction(action, Id(loc), ctx);
            lmenu->addAction(cmd, Id(Constants::G_LANGUAGES));
            aLanguageGroup->addAction(action);
        }
        connect(aLanguageGroup, SIGNAL(triggered(QAction*)), this, SLOT(switchLanguage(QAction*)));
    }
}

void MainWindowActionHandler::switchToCurrentUserLanguage()
{
    QString lang = settings()->value(Constants::S_PREFERREDLANGUAGE, QLocale().name().left(2)).toString().toLower();
    foreach(QAction *a, aLanguageGroup->actions()) {
        if (a->data().toString().toLower() == lang) {
            a->setChecked(true);
            switchLanguage(a);
            break;
        }
    }
}

void MainWindowActionHandler::switchLanguage(QAction * action)
{
    // Inform the Application that language changes, and loads the dictionnaries
    settings()->setValue(Constants::S_PREFERREDLANGUAGE, action->data().toString());
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
    if (aAppConfigurator)
        connect(aAppConfigurator, SIGNAL(triggered()), this, SLOT(applicationConfiguratorWizard()));
    if (aMedinTux)
        connect(aMedinTux, SIGNAL(triggered()), this, SLOT(configureMedintux()));
}

void MainWindowActionHandler::createHelpActions(int actions)
{
    if (!actions)
        return;

    QAction *a = 0;
    Command *cmd = 0;
//    QList<int> ctx = QList<int>() << Constants::C_GLOBAL;
    Core::Context ctx(Constants::C_GLOBAL);
    ActionContainer *menu = actionManager()->actionContainer(Constants::M_HELP);
    Q_ASSERT(menu);
    if (!menu)
        return;

    // Application help
    if (actions & Core::MainWindowActions::A_AppHelp) {
        a = aAppHelp = new QAction(this);
        a->setIcon(theme()->icon(Constants::ICONHELP));
        cmd = actionManager()->registerAction(a, Id(Constants::A_APPLICATIONHELP), ctx);
        cmd->setDefaultKeySequence(QKeySequence::HelpContents);
        cmd->setTranslations(Trans::Constants::HELP_TEXT);
        menu->addAction(cmd, Id(Constants::G_HELP_HELP));
    }

    // Web site
    if (actions & Core::MainWindowActions::A_AppGoToWebSite) {
        a = aAppGoToWebSite = new QAction(this);
        a->setIcon(theme()->icon(Constants::ICONINTERNET));
        cmd = actionManager()->registerAction(a, Id(Constants::A_APPWEBSITE), ctx);
        cmd->setTranslations(Trans::Constants::WEBSITE_TEXT);
        menu->addAction(cmd, Id(Constants::G_HELP_HELP));
    }

    menu->addSeparator(ctx, Id(Constants::G_HELP_HELP));

    // Check for update
    if (actions & Core::MainWindowActions::A_CheckUpdate) {
        a = aCheckUpdate = new QAction(this);
        a->setIcon(theme()->icon(Constants::ICONSOFTWAREUPDATEAVAILABLE));
        cmd = actionManager()->registerAction(a, Id(Constants::A_CHECKUPDATE), ctx);
        cmd->setTranslations(Trans::Constants::CHECKUPDATE);
        menu->addAction(cmd, Id(Constants::G_UPDATE));
    }

    // Debugging tools
    if (actions & Core::MainWindowActions::A_DebugDialog) {
        a = aDebugDialog = new QAction(this);
        a->setIcon(theme()->icon(Constants::ICONHELP));
        cmd = actionManager()->registerAction(a, Id(Constants::A_DEBUGHELPER), ctx);
        cmd->setTranslations(Trans::Constants::DEBUGHELPER_TEXT);
        menu->addAction(cmd, Id(Constants::G_HELP_DEBUG));
    }

    menu->addSeparator(ctx, Id(Constants::G_HELP_DEBUG));

    // About plugins
    if (actions & Core::MainWindowActions::A_PluginsAbout) {
        a = aPlugsAbout = new QAction(this);
        a->setIcon(theme()->icon(Constants::ICONPLUGIN));
        cmd = actionManager()->registerAction(a, Id(Constants::A_ABOUTPLUGINS), ctx);
        cmd->setTranslations(Trans::Constants::ABOUTPLUGINS_TEXT);
        menu->addAction(cmd, Id(Constants::G_HELP_ABOUT));
    }

    // About Qt
    if (actions & Core::MainWindowActions::A_QtAbout) {
        a = aQtAbout = new QAction(this);
        a->setIcon(theme()->icon(Constants::ICONABOUT));
        cmd = actionManager()->registerAction(a, Id(Constants::A_ABOUTQT), ctx);
        cmd->setTranslations(Trans::Constants::ABOUTQT_TEXT);
#ifdef Q_OS_MAC
        cmd->action()->setMenuRole(QAction::AboutQtRole);
#endif
        menu->addAction(cmd, Id(Constants::G_HELP_ABOUT));
    }

    // About App (FreeMedForms, FreeDiams,...)
    if (actions & Core::MainWindowActions::A_AppAbout) {
        a = aAppAbout = new QAction(this);
        a->setIcon(theme()->icon(Constants::ICONABOUT));
        cmd = actionManager()->registerAction(a, Id(Constants::A_ABOUT), ctx);
        cmd->setTranslations(Trans::Constants::ABOUT_TEXT);
#ifdef Q_OS_MAC
        cmd->action()->setMenuRole(QAction::AboutRole);
#endif
        menu->addAction(cmd, Id(Constants::G_HELP_ABOUT));
    }

    //    if (actions & Core::MainWindowActions::A_FormsAbout) {
    //        a = aDebugDialog = new QAction(this);
    //        a->setIcon(theme()->icon(Constants::ICONHELP));
    //        cmd = actionManager()->registerAction(a, Constants::A_DEBUGHELPER, ctx);
    //        cmd->setTranslations( Constants::DEBUGHELPER_TEXT );
    //        menu->addAction(cmd, Id(Constants::G_HELP_DEBUG));
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

    if (aAppGoToWebSite)
        connect(aAppGoToWebSite, SIGNAL(triggered()), this, SLOT(goToAppWebSite()));
}

void MainWindowActionHandler::createTemplatesActions(int actions)
{
    if (!actions)
        return;

    QAction *a = 0;
    Command *cmd = 0;
//    QList<int> ctx = QList<int>() << Constants::C_GLOBAL;
    Core::Context ctx(Constants::C_GLOBAL);
    ActionContainer *menu = actionManager()->actionContainer(Constants::M_TEMPLATES);
    Q_ASSERT(menu);
    if (!menu)
        return;

    if (actions & Core::MainWindowActions::A_Templates_New) {
        a = new QAction(this);
        a->setIcon(theme()->icon(Core::Constants::ICONTEMPLATES));
        cmd = actionManager()->registerAction(a, Id(Constants::A_TEMPLATE_CREATE), ctx);
        cmd->setTranslations(Trans::Constants::CREATETEMPLATE_TEXT);
        menu->addAction(cmd, Id(Constants::G_TEMPLATES_NEW));
    }
    if (actions & Core::MainWindowActions::A_Templates_Manager) {
//        a = new QAction(this);
//        a->setIcon(theme()->icon(Constants::ICONABOUT));
//        a->setMenuRole(QAction::AboutRole);
//        cmd = actionManager()->registerAction(a, Id(Constants::A_ABOUT), ctx);
//        cmd->setTranslations(Trans::Constants::ABOUT_TEXT);
//        menu->addAction(cmd, Constants::G_HELP_ABOUT);
    }
    if (actions & Core::MainWindowActions::A_Templates_ToogleViewer) {
        a = new QAction(this);
        a->setIcon(theme()->icon(Constants::ICONTEMPLATES));
        cmd = actionManager()->registerAction(a, Id(Constants::A_TEMPLATE_TOGGLEVIEW), ctx);
        cmd->setTranslations(Trans::Constants::TEMPLATES_TOGGLEVIEW_TEXT);
        menu->addAction(cmd, Id(Constants::G_TEMPLATES_EXTRAS));
    }
}

/** \brief Shows the debug dialog. \sa Core::DebugDialog */
bool MainWindowActionHandler::debugDialog()
{
    DebugDialog dlg(this);
    dlg.exec();
    return true;
}

/** \brief Shows the standard Preferences dialog. \sa Core::SettingsDialog */
bool MainWindowActionHandler::applicationPreferences()
{
    Core::SettingsDialog dlg (this);
    dlg.exec();
    return true;
}

/** \brief Shows the standard About dialog. \sa Core::AboutDialog */
bool MainWindowActionHandler::aboutApplication()
{
    AboutDialog dlg(this);
    dlg.exec();
    return true;
}
/** \brief Shows the standard About Plugins dialog. \sa Core::PluginDialog */
bool MainWindowActionHandler::aboutPlugins()
{
    Core::PluginDialog dlg(this);
    dlg.exec();
    return true;
}

/** \brief Manage action creation. Actions are not connected. \sa Core::MainWindowActions */
void MainWindowActionHandler::createActions(const Core::MainWindowActions &actions)
{
    createGeneralActions(actions.generalActions());
    createFileActions(actions.fileActions());
    createPatientsActions(actions.patientsActions());
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
        connect(updateChecker(), SIGNAL(done(bool)), this, SLOT(updateCheckerEnd(bool)));
        updateChecker()->check(settings()->path(Core::ISettings::UpdateUrl));
    }
    return true;
}


/**
  \brief Slot to connect with the Utils::UpdateChecker::updateFound() signal.
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
    //    QList<int> ctx = QList<int>() << Constants::C_GLOBAL;
        Core::Context ctx(Constants::C_GLOBAL);
    Q_ASSERT(menu);
    if (!menu)
        return false;

    QAction *a = aUpdateAvailable = new QAction(this);
    a->setIcon(theme()->icon(Constants::ICONSOFTWAREUPDATEAVAILABLE));
    Command *cmd = actionManager()->registerAction(a, Id(Constants::A_VIEWUPDATE), ctx);
    cmd->setTranslations(Trans::Constants::VIEWUPDATE_TEXT);
    menu->addAction(cmd, Id(Constants::G_UPDATE_AVAILABLE));
    cmd->retranslate();
    Core::ICore::instance()->contextManager()->updateContext();
    if (up)
        connect(a, SIGNAL(triggered()), up, SLOT(showUpdateInformation()));
    return true;
}

void MainWindowActionHandler::updateCheckerEnd(bool error)
{
    Q_UNUSED(error);
    // TODO: improve this for FreeMedForms (delete only label and progressbar)
    delete statusBar();
}

/** \brief If the Core::ISettings::WebSiteUrl is informed in the Core::ISettings::path(), opens the URL. */
void MainWindowActionHandler::goToAppWebSite()
{
    if (!settings()->path(Core::ISettings::WebSiteUrl).isEmpty())
        QDesktopServices::openUrl(QUrl(settings()->path(Core::ISettings::WebSiteUrl)));
}

/**
 * Lock the application:
 * - change the mainwindow central widget
 * - require user authentication to unlock the window
 */
void MainWindowActionHandler::lockApplication()
{
#ifdef WITH_UI_AUTO_LOCKING
    // TODO: code this, one action is already available in the General menu and connected to this slot
//    QWidget *currentCentral = this->centralWidget();
//    currentCentral->setEnabled(false);
//    QWidget *w = new QWidget(this);
//    QGridLayout *grid = new QGridLayout(w);
    // +----------+-----------+----------+
    // |          |   SPACER  |          |
    // +----------+-----------+----------+
    // | SPACER   |   LOGIN   |  SPACER  |
    // +----------+-----------+----------+
    // |          |   BUTTONS |          |
    // +----------+-----------+----------+
    // |          |   SPACER  |          |
    // +----------+-----------+----------+
#endif
}
