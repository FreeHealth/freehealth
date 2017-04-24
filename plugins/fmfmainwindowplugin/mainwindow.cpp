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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "mainwindow.h"

#include <translationutils/constanttranslations.h>
#include <utils/log.h>
#include <utils/global.h>
#include <utils/stylehelper.h>
#include <utils/updatechecker.h>
#include <utils/widgets/fancyactionbar.h>
#include <utils/widgets/fancytabwidget.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/translators.h>
#include <coreplugin/filemanager.h>
#include <coreplugin/modemanager/modemanager.h>
#include <coreplugin/actionmanager/mainwindowactions.h>
#include <coreplugin/actionmanager/mainwindowactionhandler.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/dialogs/settingsdialog.h>
#include <coreplugin/icommandline.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/iuser.h>
#include <coreplugin/theme.h>

#include <formmanagerplugin/formcore.h>
#include <formmanagerplugin/iformio.h>
#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/iformwidgetfactory.h>
#include <formmanagerplugin/formplaceholder.h>
#include <formmanagerplugin/episodemodel.h>

#include <fmfcoreplugin/coreimpl.h>

#include <patientbaseplugin/patientbar.h>
#include <patientbaseplugin/patientsearchmode.h>
#include <patientbaseplugin/patientwidgetmanager.h>
#include <patientbaseplugin/patientcore.h>
#include <patientbaseplugin/patientcreatorwizard.h>

#include <extensionsystem/pluginerrorview.h>
#include <extensionsystem/pluginview.h>
#include <extensionsystem/pluginmanager.h>

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
#include <QMenu>

using namespace MainWin;
using namespace MainWin::Internal;
using namespace Trans::ConstantTranslations;

// Getting the Core instances
static inline Utils::UpdateChecker *updateChecker() { return Core::ICore::instance()->updateChecker(); }

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ActionManager *actionManager() { return Core::ICore::instance()->actionManager(); }
static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }
static inline Core::FileManager *fileManager() { return Core::ICore::instance()->fileManager(); }
static inline Core::ModeManager *modeManager() { return Core::ICore::instance()->modeManager(); }
static inline Core::IUser *user() {return Core::ICore::instance()->user();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline ExtensionSystem::PluginManager *pluginManager() { return ExtensionSystem::PluginManager::instance(); }
static inline Form::FormCore &formCore() {return Form::FormCore::instance();}
static inline Patients::PatientCore *patientCore() {return Patients::PatientCore::instance();}
static inline Core::ICommandLine *commandLine()  { return Core::ICore::instance()->commandLine(); }

// SplashScreen Messagers
static inline void messageSplash(const QString &s) {theme()->messageSplashScreen(s); }

MainWindowUserListener::MainWindowUserListener(MainWindow *parent) :
    UserPlugin::IUserListener(parent), m_MainWindow(parent)
{
}

MainWindowUserListener::~MainWindowUserListener()
{
}

bool MainWindowUserListener::userAboutToChange()
{
    m_MainWindow->writeSettings();
    return true;
}

bool MainWindowUserListener::currentUserAboutToDisconnect()
{
    m_MainWindow->writeSettings();
    return true;
}

MainWindow::MainWindow(QWidget *parent) :
    Core::IMainWindow(parent),
    m_modeStack(0),
    m_UserListener(0)
{
    setObjectName("MainWindow");
    messageSplash(tr("Creating Main Window"));
    setAttribute(Qt::WA_QuitOnClose);
    // Connect post core initialization
    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreInitialization()));
}

/**
 * Initialize the main window.\n
 * Creates menus, actions and the modeWidget
 * \sa Core::IMainWindow, Core::MainWindowActions, Core::MainWindowActionHandler
 */
void MainWindow::init()
{
    Q_ASSERT(actionManager());
    Q_ASSERT(contextManager());

    // create menus
    createGeneralMenu();
    createPatientMenu();
    createEditMenu();
    createFormatMenu();
    createPluginsMenu();
    createConfigurationMenu();
    createTemplatesMenu();
    createHelpMenu();

    // Connect menus for recent managers
    //    Core::ActionContainer *fmenu = actionManager()->actionContainer(Core::Constants::M_FILE);
    //    Q_ASSERT(fmenu);
    //    connect(fmenu->menu(), SIGNAL(aboutToShow()),this, SLOT(aboutToShowRecentFiles()));

    Core::MainWindowActions actions;

    actions.setGeneralActions(
                //            Core::MainWindowActions::A_FileNew  |
                Core::MainWindowActions::A_FileSave |
                Core::MainWindowActions::A_FilePrint |
                Core::MainWindowActions::A_FilePrintPreview |
                Core::MainWindowActions::A_FileQuit |
                Core::MainWindowActions::A_Patients_New
                );

    actions.setPatientsActions(
                Core::MainWindowActions::A_Patients_ViewIdentity |
                Core::MainWindowActions::A_Patients_Remove
                );

    actions.setConfigurationActions(
                Core::MainWindowActions::A_AppPreferences |
                //            Core::MainWindowActions::A_AppConfigurator |
                Core::MainWindowActions::A_PluginsPreferences |
                Core::MainWindowActions::A_LanguageChange
                );

    actions.setHelpActions(
                Core::MainWindowActions::A_AppAbout |
                Core::MainWindowActions::A_PluginsAbout |
                Core::MainWindowActions::A_AppHelp |
                Core::MainWindowActions::A_DebugDialog |
                Core::MainWindowActions::A_CheckUpdate |
                Core::MainWindowActions::A_AppGoToWebSite
                );
    actions.setTemplatesActions(Core::MainWindowActions::A_Templates_New);

    actions.createEditActions(true);
    createActions(actions);

    // Create Mode stack
    m_modeStack = new Utils::FancyTabWidget(this);
    modeManager()->init(m_modeStack);
}

/**
 * Second initialization step after all dependencies are initialized.
 * Set windowIcon, connect menus and actions, start the updatechecker,
 */
void MainWindow::extensionsInitialized()
{
    //    qWarning() << Q_FUNC_INFO << "user ok" << user()->hasCurrentUser();
    // First check if there is a logged user
    if (!user()->hasCurrentUser()) {
        return;
    }
    setWindowIcon(theme()->icon(Core::Constants::ICONEHR));

    // Connect actions
    connectFileActions();
    connectGeneralActions();
    connectPatientActions();
    connectConfigurationActions();
    connectHelpActions();

    // Start the update checker
    if (!commandLine()->value(Core::ICommandLine::NoCheckUpdate).toBool()) {
        if (updateChecker()->needsUpdateChecking(settings()->getQSettings())) {
            settings()->setPath(Core::ISettings::UpdateUrl, Utils::Constants::EHR_UPDATE_URL);
            if (checkUpdate())
                settings()->setValue(Utils::Constants::S_LAST_CHECKUPDATE, QDate::currentDate());
        }
    }

    m_modeStack->insertTopWidget(patientCore()->patientBar());
    m_modeStack->statusBar()->hide();

    setCentralWidget(m_modeStack);

    //    if (settings()->firstTimeRunning()) {
    //        if (!applicationConfiguratorWizard()) {
    //            theme()->finishSplashScreen(this);
    //            qApp->exit(1234);
    //            return;
    //        }
    //        settings()->noMoreFirstTimeRunning();
    //    }
}

MainWindow::~MainWindow()
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "MainWindow::~MainWindow()";
    if (m_UserListener) {
        pluginManager()->removeObject(m_UserListener);
    }
    // delete ui components
    delete m_modeStack;
    m_modeStack = 0;
}

/**
 * Third step of initialization after the core gets fuly opened.
 * Manages user, menus and actions translations, finish splashscreen
 * show UI.
 * \sa Core::ICore::coreOpened()
 */
void MainWindow::postCoreInitialization()
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << Q_FUNC_INFO;

    // Manage current user and patient
    onCurrentUserChanged();
    pluginManager()->addObject(m_UserListener = new MainWindowUserListener(this));
    connect(user(), SIGNAL(userChanged()), this, SLOT(onCurrentUserChanged()), Qt::UniqueConnection);
    connect(user(), SIGNAL(userDataChanged(int)), this, SLOT(onUserDataChanged(int)), Qt::UniqueConnection);
    connect(user(), SIGNAL(reset()), this, SLOT(onCurrentUserChanged()), Qt::UniqueConnection);
    connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(onCurrentPatientChanged()), Qt::UniqueConnection);

    switchToCurrentUserLanguage();

    contextManager()->updateContext();
    actionManager()->retranslateMenusAndActions();

    theme()->finishSplashScreen(this);

    manageIModeEnabledState();
    connect(modeManager(), SIGNAL(currentModeChanged(Core::IMode*)), this, SLOT(onCurrentModeChanged(Core::IMode*)), Qt::UniqueConnection);
    modeManager()->activateMode(Core::Constants::MODE_PATIENT_SEARCH);

    raise();
    show();

    // clear the focus of the mainwin so that the lineeditbuton show the tooltip
    setFocus();
}

/** When the current Core::IMode is beeing updated, check the patient visibility */
void MainWindow::onCurrentModeChanged(Core::IMode *newMode)
{
    Q_ASSERT(newMode);
    if (!newMode)
        return;
    if (newMode->patientBarVisibility())
        patient()->showPatientBar();
    else
        patient()->hidePatientBar();
}

/**
 * \internal
 * Slot connected to Core::IUser::userChanged().
 */
void MainWindow::onCurrentUserChanged()
{
    // Change window title
    setWindowTitle(qApp->applicationName() + " - " + qApp->applicationVersion() + " / " +
                   user()->value(Core::IUser::FullName).toString());
    setWindowIcon(theme()->icon(Core::Constants::ICONEHR));
    readSettings();
}

/**
 * \internal
 * Slot connected to Core::IUser::userDataChanged(int).
 * Reset the window title.
 */
void MainWindow::onUserDataChanged(int id)
{
    if (id == Core::IUser::FullName) {
        // Change window title
        setWindowTitle(qApp->applicationName() + " - " + qApp->applicationVersion() + " / " +
                       user()->value(Core::IUser::FullName).toString());
    }
}

/**
 * \internal
 * Define the current patient to different structure such as
 * Form::FormManager, Form::EpisodeModel.
 */
void MainWindow::onCurrentPatientChanged()
{
    // Manage mode enabled state
    manageIModeEnabledState();

    // Activate Patient files mode
    formCore().activatePatientFileCentralMode();

    endProcessingSpinner();
}

/** Enable/Disable modes according to the Core::IMode::isEnabledOnlyWithCurrentPatient() state */
void MainWindow::manageIModeEnabledState()
{
    QList<Core::IMode*> modes = pluginManager()->getObjects<Core::IMode>();
    foreach(Core::IMode *mode, modes) {
        if (mode->isEnabledOnlyWithCurrentPatient())
            mode->setEnabled(patient()->currentPatientIndex().isValid());
    }
}

/** \brief Close the main window and the application */
void MainWindow::closeEvent(QCloseEvent *event)
{
    LOG("Closing MainWindow");
    // Set focus to ensure that QDataWidgetMapper will send their last changes to their models
    setFocus();
    writeSettings();
    QList<Core::ICoreListener *> listeners = pluginManager()->getObjects<Core::ICoreListener>();
    for(int i = 0; i < listeners.count(); ++i) {
        if (!listeners.at(i)->coreAboutToClose()) {
            const QString &msg = listeners.at(i)->errorMessage();
            if (!msg.isEmpty()) {
                Utils::warningMessageBox(tr("Unable to close window"),
                                         tr("Unable to close the application, error message: <br/><b>%1</b>").arg(msg),
                                         "", tr("Unable to close window"));
            }
            event->ignore();
            return;
        }
    }
    event->accept();
}

/** \brief Manages language changes : retranslate Ui and ActionManager. */
void MainWindow::changeEvent(QEvent *event)
{
    if (event->type()==QEvent::LanguageChange) {
        //	m_ui->retranslateUi(this);
        if (actionManager())
            actionManager()->retranslateMenusAndActions();
        // Save last used language
        settings()->setValue(Core::Constants::S_PREFERREDLANGUAGE, QLocale().name().left(2));
        settings()->sync();
    }
}

QStatusBar *MainWindow::statusBar()
{
    return QMainWindow::statusBar();
}

void MainWindow::updateCheckerEnd(bool error)
{
    Q_UNUSED(error)
    delete statusBar();
}

/** \brief Load a patient XML file into the FormManager. */
void MainWindow::openPatientFormsFile()
{
    // TODO: Save patient forms file to database
    //    loadFile(settings()->value(Core::Constants::S_PATIENTFORMS_FILENAME).toString());
}

/** \brief Load a patient XML file into the FormManager. */
bool MainWindow::openFile()
{
    // Ask user for a patient form dir
    QString dir;
    dir = QFileDialog::getExistingDirectory(this, tr("Choose a patient file directory"),
                                            settings()->path(Core::ISettings::CompleteFormsPath));
    // If one file is selected ask canReadForm()
    if (dir.isEmpty())
        return false;
    return loadFile(dir);
}

/** \brief Send a signal: load a patient form file. \sa Core::ICore::loadPatientForms() */
bool MainWindow::loadFile(const QString &absDirPath)
{
    if (absDirPath.isEmpty())
        return false;

    // Get the PatientFile FormMain empty root from FormManager
    //    Form::FormMain *root = 0;
    //    if (root = formManager().loadFile(filename, list)) {
    //        fileManager()->setCurrentFile(filename);
    //    } else {
    //        return false;
    //    }

    //    Q_EMIT loadPatientForms(absDirPath);

    return true;
}

/** \brief Create a new patient. \sa Patients::PatientCreatorWizard */
bool MainWindow::createNewPatient()
{
    // TODO: move this in the patient plugin
    Patients::PatientCreatorWizard wiz(this);
    wiz.exec();
    return true;
}

bool MainWindow::viewPatientIdentity()
{
    return true;
}

bool MainWindow::removePatient()
{
    // TODO: write MainWindow::removePatient()
    return true;
}

/** \brief Populate recent files menu */
void MainWindow::aboutToShowRecentFiles()
{
    // update recent forms files
    Core::ActionContainer *recentsMenu = actionManager()->actionContainer(Core::Constants::M_FILE_RECENTFILES);
    if (!recentsMenu)
        return;
    if (!recentsMenu->menu())
        return;
    recentsMenu->menu()->clear();

    bool hasRecentFiles = false;
    foreach (const QString &fileName, fileManager()->recentFiles()) {
        hasRecentFiles = true;
        QAction *action = recentsMenu->menu()->addAction(QFileInfo(fileName).baseName());
        action->setData(fileName);
        action->setToolTip(fileName);
        connect(action, SIGNAL(triggered()), this, SLOT(openRecentFile()));
    }
    recentsMenu->menu()->setEnabled(hasRecentFiles);
}

/** \brief Opens a recent file. This slot is called by a recent files' menu's action. */
void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (!action)
        return;
    const QString &fileName = action->data().toString();
    if (!fileName.isEmpty()) {
        loadFile(fileName);
    }
}

/** \brief Reads main window's settings */
void MainWindow::readSettings()
{
    //    statusBar()->showMessage(tkTr(Trans::Constants::LOADING_SETTINGS));

    // Main Application settings
    settings()->restoreState(this);
    fileManager()->getRecentFilesFromSettings();
    fileManager()->getMaximumRecentFilesFromSettings();
    m_AutomaticSaveInterval = settings()->value(Core::Constants::S_SAVEINTERVAL, 600).toUInt(); // Default = 10 minutes
    m_OpenLastOpenedForm = settings()->value(Core::Constants::S_OPENLAST, true).toBool();

    // Main Widget settings
    m_HelpTextShow = settings()->value(Core::Constants::S_SHOWHELPTEXT, true).toBool();

    Utils::StyleHelper::setBaseColor(Utils::StyleHelper::DEFAULT_BASE_COLOR);
    // Notify
    //    statusBar()->showMessage(tkTr(Trans::Constants::SETTINGS_RECOVERED), 2000);
}

/** \brief Write main window's settings */
void MainWindow::writeSettings()
{
    settings()->saveState(this);
    // Recent managers
    fileManager()->saveRecentFiles();
    // Main Application settings
    settings()->setValue(Core::Constants::S_SAVEINTERVAL, m_AutomaticSaveInterval);
    settings()->setValue(Core::Constants::S_OPENLAST, m_OpenLastOpenedForm);
    // Main Widget settings
    settings()->setValue(Core::Constants::S_SHOWHELPTEXT, m_HelpTextShow);
    settings()->sync();
}

/** \brief Open the preferences dialog */
bool MainWindow::applicationPreferences()
{
    Core::SettingsDialog dlg(this);
    dlg.exec();
    return true;
}

/** \brief Open the application global configurator dialog. \sa MainWindow::AppConfigWizard */
bool MainWindow::applicationConfiguratorWizard()
{
    // TODO: code here?
    //    AppConfigWizard wiz(this);
    //    int r = wiz.exec();
    //    return r==QDialog::Accepted;
    return true;
}
