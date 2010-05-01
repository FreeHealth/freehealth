/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
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
#include "mainwindow.h"

#include <translationutils/constanttranslations.h>
#include <utils/log.h>
#include <utils/global.h>
#include <utils/updatechecker.h>
#include <utils/widgets/fancyactionbar.h>
#include <utils/widgets/fancytabwidget.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants.h>
#include <coreplugin/translators.h>
#include <coreplugin/itheme.h>
#include <coreplugin/filemanager.h>
#include <coreplugin/modemanager/modemanager.h>

#include <formmanagerplugin/iformio.h>
#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/iformwidgetfactory.h>
#include <formmanagerplugin/formmanager.h>
#include <formmanagerplugin/formplaceholder.h>

#include <coreplugin/actionmanager/mainwindowactions.h>
#include <coreplugin/actionmanager/mainwindowactionhandler.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/dialogs/settingsdialog.h>

#include <fmfcoreplugin/coreimpl.h>
#include <fmfcoreplugin/commandlineparser.h>

#include <usermanagerplugin/usermodel.h>

#include <patientbaseplugin/patientbar.h>
#include <patientbaseplugin/patientsearchmode.h>
#include <patientbaseplugin/patientwidgetmanager.h>

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

using namespace MainWin;
using namespace MainWin::Internal;
using namespace Trans::ConstantTranslations;

// Getting the Core instances
static inline Utils::UpdateChecker *updateChecker() { return Core::ICore::instance()->updateChecker(); }
static inline Core::CommandLine *commandLine() { return Core::ICore::instance()->commandLine(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ActionManager *actionManager() { return Core::ICore::instance()->actionManager(); }
static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }
static inline Core::FileManager *fileManager() { return Core::ICore::instance()->fileManager(); }
static inline ExtensionSystem::PluginManager *pluginManager() { return ExtensionSystem::PluginManager::instance(); }
static inline Form::FormManager *formManager() { return Form::FormManager::instance(); }
static inline Core::ModeManager *modeManager() { return Core::ICore::instance()->modeManager(); }

// SplashScreen Messagers
static inline void messageSplash(const QString &s) {Core::ICore::instance()->messageSplashScreen(s); }
static inline void finishSplash(QMainWindow *w) {Core::ICore::instance()->finishSplashScreen(w); }

//--------------------------------------------------------------------------------------------------------
//--------------------------------------- Constructor / Destructor ---------------------------------------
//--------------------------------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
        Core::IMainWindow(parent),
        m_modeStack(0),
        m_PatientBar(0)
{
    setObjectName("MainWindow");
    setWindowTitle(qApp->applicationName() + " - " + qApp->applicationVersion());
    messageSplash(tr("Creating Main Window"));
    setAttribute(Qt::WA_QuitOnClose);
}

void MainWindow::init()
{
    Q_ASSERT(actionManager());
    Q_ASSERT(contextManager());

    // create menus
    createFileMenu();
    createPatientMenu();
    createEditMenu();
    createFormatMenu();
    createPluginsMenu();
    createConfigurationMenu();
    createTemplatesMenu();
    createHelpMenu();

    Core::ActionContainer *fmenu = actionManager()->actionContainer(Core::Constants::M_FILE);
    Q_ASSERT(fmenu);
    connect(fmenu->menu(), SIGNAL(aboutToShow()),this, SLOT(aboutToShowRecentFiles()));

    Core::MainWindowActions actions;

    actions.setFileActions(
            Core::MainWindowActions::A_FileNew  |
            Core::MainWindowActions::A_FileOpen |
            Core::MainWindowActions::A_FileSave |
            Core::MainWindowActions::A_FileSaveAs |
            Core::MainWindowActions::A_FilePrint |
            Core::MainWindowActions::A_FileQuit
            );

    actions.setConfigurationActions(
            Core::MainWindowActions::A_AppPreferences |
            Core::MainWindowActions::A_PluginsPreferences |
            Core::MainWindowActions::A_LangageChange
            );

    actions.setHelpActions(
            Core::MainWindowActions::A_AppAbout |
            Core::MainWindowActions::A_PluginsAbout |
            Core::MainWindowActions::A_AppHelp |
            Core::MainWindowActions::A_DebugDialog |
            Core::MainWindowActions::A_CheckUpdate //|
//            Core::MainWindowActions::A_QtAbout
            );
    actions.setTemplatesActions(Core::MainWindowActions::A_Templates_New);

    actions.createEditActions(true);
    createActions(actions);

    connectFileActions();
    connectConfigurationActions();
    connectHelpActions();

    readSettings();

    // Create Mode stack
    m_modeStack = new Utils::FancyTabWidget;
    modeManager()->init(m_modeStack);
}

bool MainWindow::initialize(const QStringList &arguments, QString *errorString)
{
    Utils::Log::addMessage(this , tkTr(Trans::Constants::RAISING_APPLICATION));
    raise();
    show();
    return true;
}

void MainWindow::extensionsInitialized()
{
    // First check if there is a logged user
    if (!UserPlugin::UserModel::instance()->hasCurrentUser()) {
        return;
    }
    // Start the update checker
    if (updateChecker()->needsUpdateChecking(settings()->getQSettings())) {
        messageSplash(tkTr(Trans::Constants::CHECKING_UPDATES));
        Utils::Log::addMessage(this, tkTr(Trans::Constants::CHECKING_UPDATES));
        statusBar()->addWidget(new QLabel(tkTr(Trans::Constants::CHECKING_UPDATES), this));
        statusBar()->addWidget(updateChecker()->progressBar(this),1);
        connect(updateChecker(), SIGNAL(updateFound()), this, SLOT(updateFound()));
        connect(updateChecker(), SIGNAL(done(bool)), this, SLOT(updateCheckerEnd()));
        updateChecker()->check(Utils::Constants::FREEMEDFORMS_UPDATE_URL);
        settings()->setValue(Utils::Constants::S_LAST_CHECKUPDATE, QDate::currentDate());
    }

    finishSplash(this);

    setCentralWidget(m_modeStack);

    // Connect post core initialization
    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreInitialization()));
}

MainWindow::~MainWindow()
{
    qWarning() << "MainWindow::~MainWindow()";
}

void MainWindow::postCoreInitialization()
{
    // Create and insert the patient tab in the formplaceholder
    m_PatientBar = new Patients::PatientBar(this);
    formManager()->formPlaceHolder()->addTopWidget(m_PatientBar);

    // Connect this tab with the patientsearchmode
    connect(Patients::PatientWidgetManager::instance()->selector(), SIGNAL(patientSelected(QModelIndex)),
            this, SLOT(setCurrentPatient(QModelIndex)));

    contextManager()->updateContext();
    actionManager()->retranslateMenusAndActions();

    // Open Last Opened Forms is necessary
    openLastOpenedForm();
}

void MainWindow::setCurrentPatient(const QModelIndex &index)
{
    m_PatientBar->setCurrentIndex(index);
    formManager()->activateMode();
}

/** \brief Close the main window and the application */
void MainWindow::closeEvent(QCloseEvent *event)
{
    Utils::Log::addMessage(this, "Closing MainWindow");
    writeSettings();
    event->accept();
}

/** \brief Manages language changes : retranslate Ui and ActionManager. */
void MainWindow::changeEvent(QEvent *event)
{
    if (event->type()==QEvent::LanguageChange) {
//	m_ui->retranslateUi(this);
        if (actionManager())
            actionManager()->retranslateMenusAndActions();
    }
}

QStatusBar *MainWindow::statusBar()
{
    return QMainWindow::statusBar();
}

void MainWindow::updateCheckerEnd()
{
//    delete statusBar();
}

void MainWindow::openLastOpenedForm()
{
    // Open the last form
    if ((m_OpenLastOpenedForm) && (fileManager()->recentFiles().count())) {
        QFile file(fileManager()->recentFiles().at(0));
        if (file.exists())
            loadFile(file.fileName());
    }
}

bool MainWindow::openFile()
{
    // Get all IFormIO from pluginsmanager
    QList<Form::IFormIO *> list = pluginManager()->getObjects<Form::IFormIO>();
    // Ask list for the file filters
    QStringList filters;
    foreach(const Form::IFormIO *io, list) {
        filters << io->fileFilters();
    }
    // Ask user for a file
    QString file;
    file = QFileDialog::getOpenFileName(this, tr("Choose a file"),
                                        Core::ICore::instance()->settings()->path(Core::ISettings::SampleFormsPath),
                                        filters.join(";"));
    // If one file is selected ask canReadForm()
    if (file.isEmpty())
        return false;
    if (formManager()->loadFile(file, list)) {
        fileManager()->setCurrentFile(file);
        fileManager()->addToRecentFiles(file);
    } else {
        return false;
    }
    return true;
}

bool MainWindow::loadFile(const QString &filename, const QList<Form::IFormIO *> &iolist)
{
    if (filename.isEmpty())
        return false;

    // Get all IFormIO from pluginsmanager
    QList<Form::IFormIO *> list = iolist;

    if (iolist.isEmpty())
        QList<Form::IFormIO *> list = pluginManager()->getObjects<Form::IFormIO>();

    if (formManager()->loadFile(filename, list)) {
        fileManager()->setCurrentFile(filename);
    } else {
        return false;
    }
    return true;
}

/** \brief Populate recent files menu */
void MainWindow::aboutToShowRecentFiles()
{
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

/** \brief Opens a recent file. This solt must be called by a recent files' menu's action. */
void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (!action)
        return;
    QString fileName = action->data().toString();
    if (!fileName.isEmpty()) {
        loadFile(fileName);
    }
}


/** \brief Reads main window's settings */
void MainWindow::readSettings()
{
    statusBar()->showMessage(tkTr(Trans::Constants::LOADING_SETTINGS));

    // Main Application settings
    settings()->restoreState(this);
    fileManager()->getRecentFilesFromSettings();
    m_AutomaticSaveInterval = settings()->value(Core::Constants::S_SAVEINTERVAL, 600).toUInt(); // Default = 10 minutes
    m_OpenLastOpenedForm = settings()->value(Core::Constants::S_OPENLAST, true).toBool();

    // Main Widget settings
    m_HelpTextShow = settings()->value(Core::Constants::S_SHOWHELPTEXT, true).toBool();

    // Notify
    statusBar()->showMessage(tkTr(Trans::Constants::SETTINGS_RECOVERED), 2000);
}

void MainWindow::writeSettings()
{
    settings()->saveState(this);
    fileManager()->saveRecentFiles();
    // Main Application settings
    settings()->setValue(Core::Constants::S_SAVEINTERVAL, m_AutomaticSaveInterval);
    settings()->setValue(Core::Constants::S_OPENLAST, m_OpenLastOpenedForm);
    // Main Widget settings
    settings()->setValue(Core::Constants::S_SHOWHELPTEXT, m_HelpTextShow);
    settings()->sync();
}

/** \obsolete */
void MainWindow::createStatusBar()
{
    statusBar()->showMessage( tkTr(Trans::Constants::READY), 2000 );
}

/**
  \brief Open the preferences dialog
*/
bool MainWindow::applicationPreferences()
{
    Core::SettingsDialog dlg(this);
    dlg.exec();
    return true;
}

//void MainWindow::toolbarToggled(bool state)
//{
//    QAction * a = qobject_cast<QAction*>(sender());
//    // find toolbar and hide it
//    QToolBar* tb = findChild<QToolBar*>(a->objectName());
//    if (tb)tb->setVisible(state);
//}
//
//void MainWindow::saveFormData()
//{
//#ifdef DEBUG
//    mfIOInterface *ioPlugin = mfCore::pluginsManager()->currentIO();
//    if (!ioPlugin)
//        return;
//
//    if (ioPlugin->saveFormData(mfIOPlace::fromLocalFile(m_CurrentFile + ".dat"), *m_pRootObject))
//        tkLog::addMessage(this,  tr("Form data saved"));
//    else
//        tkLog::addError(this, tr("Failure in form data saving operation."));
//#else
//    QMessageBox::information(this, qApp->applicationName(), tkTr(FEATURE_NOT_IMPLEMENTED));
//#endif
//}
//
//void MainWindow::loadFormData()
//{
//#ifdef DEBUG
//    mfIOInterface *ioPlugin = mfCore::pluginsManager()->currentIO();
//    if (!ioPlugin)
//        return;
//
//    QString fileName = QFileDialog::getOpenFileName(this, tr("choose a file data to load"), mfCore::settings()->formPath());
//    if (fileName == "")
//        return;
//
//    if (ioPlugin->loadFormData(mfIOPlace::fromLocalFile(fileName), *m_pRootObject))
//    {
//        m_pRootObject->setModified(false);
//        setWindowModified(false);
//        tkLog::addMessage(this, tr("Form data loaded"));
//    }
//    else
//        tkLog::addError(this, tr("Failure in form data loading operation."));
//#else
//    QMessageBox::information(this, qApp->applicationName(), tkTr(FEATURE_NOT_IMPLEMENTED));
//#endif
//}
