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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "mainwindow.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/icommandline.h>
#include <coreplugin/constants.h>
#include <coreplugin/itheme.h>
#include <coreplugin/filemanager.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/modemanager/imode.h>
#include <coreplugin/modemanager/modemanager.h>
#include <coreplugin/actionmanager/mainwindowactions.h>
#include <coreplugin/actionmanager/mainwindowactionhandler.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/contextmanager/contextmanager.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/randomizer.h>
#include <utils/stylehelper.h>
#include <utils/updatechecker.h>
#include <utils/widgets/fancyactionbar.h>
#include <utils/widgets/fancytabwidget.h>
#include <extensionsystem/pluginerrorview.h>
#include <extensionsystem/pluginview.h>
#include <extensionsystem/pluginmanager.h>
#include <translationutils/constants.h>
#include <translationutils/trans_patient.h>
#include <translationutils/trans_filepathxml.h>
#include <translationutils/trans_msgerror.h>
#include <translationutils/trans_current.h>
#include <translationutils/trans_database.h>

#include <QString>
#include <QTextEdit>
#include <QTextStream>
#include <QCloseEvent>
#include <QFileDialog>
#include <QStringList>
#include <QCoreApplication>
#include <QDir>
#include <QStatusBar>
#include <QHBoxLayout>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QDockWidget>
#include <QMenu>
#include <QDesktopWidget>
#include <QDomDocument>
#include <QDomElement>
#include <QDomText>
#include <QToolBar>

using namespace MainWin;
using namespace Trans::ConstantTranslations;

enum { WarnLogMessage = false };

// Getting the Core instances
static inline Utils::UpdateChecker *updateChecker() { return Core::ICore::instance()->updateChecker(); }
static inline Core::ICommandLine *commandLine() { return Core::ICore::instance()->commandLine(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ActionManager *actionManager() { return Core::ICore::instance()->actionManager(); }
static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }
static inline Core::FileManager *fileManager() { return Core::ICore::instance()->fileManager(); }
static inline Core::ModeManager *modeManager() { return Core::ICore::instance()->modeManager(); }

// SplashScreen Messagers
static inline void messageSplash(const QString &s) {theme()->messageSplashScreen(s); }
static inline void finishSplash(QMainWindow *w) {theme()->finishSplashScreen(w); }

namespace {
const char* const S_LASTACTIVEMODE = "Mod/LastActive";
}

//--------------------------------------------------------------------------------------------------------
//--------------------------------------- Constructor / Destructor ---------------------------------------
//--------------------------------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
    Core::IMainWindow(parent)
{
    setObjectName("MainWindow");
    messageSplash(tr("Creating Main Window"));
}

/**
 * Initialize the main window:
 * - creates global menus and actions
 * - connect actions to default slots
 * Returns \e true if all goes fine.
 * \sa Core::IMainWindow, Core::MainWindowActions, Core::MainWindowActionHandler
 */
bool MainWindow::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);
    // create menus
    createFileMenu();
    createEditMenu();
    createFormatMenu();
    createConfigurationMenu();
    createHelpMenu();

    Core::MainWindowActions actions;
    actions.setFileActions(
                Core::MainWindowActions::A_FileOpen |
                Core::MainWindowActions::A_FileSave |
                // Core::MainWindowActions::A_FileSaveAs |
                Core::MainWindowActions::A_FilePrint |
                // Core::MainWindowActions::A_FilePrintPreview |
                Core::MainWindowActions::A_FileQuit);
    actions.setConfigurationActions(
                Core::MainWindowActions::A_AppPreferences |
                Core::MainWindowActions::A_LanguageChange //|
                );
    actions.setHelpActions(
                Core::MainWindowActions::A_AppAbout |
                Core::MainWindowActions::A_PluginsAbout |
                Core::MainWindowActions::A_AppHelp |
                Core::MainWindowActions::A_DebugDialog |
                Core::MainWindowActions::A_CheckUpdate
                );
    actions.createEditActions(false);
    createActions(actions);

    connectFileActions();
    connectConfigurationActions();
    connectHelpActions();

    // Create Mode stack
    m_modeStack = new Utils::FancyTabWidget(this);
    modeManager()->init(m_modeStack);

    return true;
}

/**
 * When all dependent plugins are initialized, start the second initialization:
 * - setup ui
 * - start update checker
 * - read settings
 */
void MainWindow::extensionsInitialized()
{
#ifndef WITH_TESTS
    // Start the update checker
    if (updateChecker()->needsUpdateChecking(settings()->getQSettings())) {
        settings()->setPath(Core::ISettings::UpdateUrl, Utils::Constants::FREEGUARD_UPDATE_URL);
        if (checkUpdate())
            settings()->setValue(Utils::Constants::S_LAST_CHECKUPDATE, QDate::currentDate());
    }
#endif
    m_modeStack->statusBar()->hide();
    setCentralWidget(m_modeStack);
    // createDockWindows();

    setWindowTitle(QString("%1 %2 - (c) %3").arg(qApp->applicationName()).arg(qApp->applicationVersion()).arg(tkTr(Trans::Constants::THE_FREEMEDFORMS_COMMUNITY)));
    setWindowIcon(theme()->icon(Core::Constants::ICONFREEGUARD));

    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreOpened()));
}

MainWindow::~MainWindow()
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "MainWindow::~MainWindow()";
    // We need to delete UI components before the mainwindow is destroy
    // because all mainwindow Core components (action manager, mode manager...)
    // are deleted with the main window object but some widgets
    // can need a last access to these Core components
    delete m_modeStack;
    m_modeStack = 0;
}

/**
 * When all plugins are correctly initialized and the Core plugin is opened,
 * start a third initialization.
 * - set the central widget of the main window
 * - translate UI, actions and menus
 * - resize window to default
 * - show and raise main window
 * \sa Core::ICore::coreOpened()
 */
void MainWindow::postCoreOpened()
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "MainWindow::postCoreOpened()";

    finishSplash(this);
    actionManager()->retranslateMenusAndActions();
    contextManager()->updateContext();
    raise();
    show();
    readSettings();
}

/**
  \brief Refresh the ui data refering to the patient
  \sa Core::Internal::CoreImpl::instance()->patient()
*/
void MainWindow::refreshPatient()
{
}

/**
  \brief Close the main window and the application
  \todo Add  ICoreListener
*/
void MainWindow::closeEvent(QCloseEvent *event)
{
    LOG("Closing MainWindow");
    Core::ICore::instance()->requestSaveSettings();
    Core::ICore::instance()->coreIsAboutToClose();
    writeSettings();
    event->accept();
}

/** \brief Manages language changes : retranslate Ui and ActionManager. */
void MainWindow::changeEvent(QEvent *event)
{
    if (event->type()==QEvent::LanguageChange) {
        actionManager()->retranslateMenusAndActions();
        refreshPatient();
    }
}

/** \brief Populate recent files menu */
void MainWindow::aboutToShowRecentFiles()
{
    Core::ActionContainer *aci = actionManager()->actionContainer(Core::Constants::M_FILE_RECENTFILES);
    aci->clear();

    foreach (const QString &fileName, fileManager()->recentFiles()) {
        QAction *action = aci->menu()->addAction(fileName);
        action->setData(fileName);
        connect(action, SIGNAL(triggered()), this, SLOT(openRecentFile()));
    }
    aci->setOnAllDisabledBehavior(Core::ActionContainer::Show);
}

/** \brief Opens a recent file. This solt must be called by a recent files' menu's action. */
void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (!action)
        return;
    QString fileName = action->data().toString();
    if (!fileName.isEmpty()) {
        readFile(fileName);
    }
}

/** Reads main window's settings */
void MainWindow::readSettings()
{
    settings()->restoreState(this);
    fileManager()->getRecentFilesFromSettings();
    fileManager()->getMaximumRecentFilesFromSettings();
    fileManager()->setCurrentFile(QString::null);
    switchToCurrentUserLanguage();
    Utils::StyleHelper::setBaseColor(Utils::StyleHelper::DEFAULT_BASE_COLOR);
    modeManager()->activateMode(settings()->value(::S_LASTACTIVEMODE).toString());
}

/** \brief Writes main window's settings */
void MainWindow::writeSettings()
{
    settings()->saveState(this);
    fileManager()->saveRecentFiles();
    settings()->setValue(::S_LASTACTIVEMODE, modeManager()->currentMode()->id());
    settings()->sync();
}

bool MainWindow::saveAsFile()
{
    return true;
}

bool MainWindow::saveFile()
{
    return true;
}

bool MainWindow::openFile()
{
    return true;
}

/**
 * Write XML the content of the CR to the file \e fileName.
 * File name must be an absolute file path.
 */
bool MainWindow::saveFileContent(const QString &fileName)
{
    Q_UNUSED(fileName);
    return true;
}

/**
 * Read the XML content of the CR \e fileName.
 * File name must be an absolute file path.
 */
void MainWindow::readFile(const QString &fileName)
{
    Q_UNUSED(fileName);
}
