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
#include "mainwindow.h"

#include <translationutils/constanttranslations.h>
#include <utils/log.h>
#include <utils/global.h>
#include <utils/updatechecker.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants.h>
#include <coreplugin/translators.h>
#include <coreplugin/itheme.h>
#include <coreplugin/iformio.h>
#include <coreplugin/iformitem.h>
#include <coreplugin/iformwidgetfactory.h>
#include <coreplugin/formmanager.h>
#include <coreplugin/filemanager.h>

#include <coreplugin/actionmanager/mainwindowactions.h>
#include <coreplugin/actionmanager/mainwindowactionhandler.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/dialogs/settingsdialog.h>

#include <fmfcoreplugin/coreimpl.h>
#include <fmfcoreplugin/commandlineparser.h>

#include <usermanagerplugin/usermodel.h>

#include <extensionsystem/pluginerrorview.h>
#include <extensionsystem/pluginview.h>
#include <extensionsystem/pluginmanager.h>

//#include "ui_mainwindow.h"

// include Qt headers
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
inline static ExtensionSystem::PluginManager *pluginManager() { return ExtensionSystem::PluginManager::instance(); }
inline static Core::FormManager *formManager() { return Core::ICore::instance()->formManager(); }

// SplashScreen Messagers
static inline void messageSplash(const QString &s) {Core::ICore::instance()->messageSplashScreen(s); }
static inline void finishSplash(QMainWindow *w) {Core::ICore::instance()->finishSplashScreen(w); }

//--------------------------------------------------------------------------------------------------------
//--------------------------------------- Constructor / Destructor ---------------------------------------
//--------------------------------------------------------------------------------------------------------
MainWindow::MainWindow( QWidget * parent )
          : Core::IMainWindow(parent)
{
    setObjectName("MainWindow");
    messageSplash(tr("Creating Main Window"));

    setAttribute(Qt::WA_QuitOnClose);
}

bool MainWindow::initialize(const QStringList &arguments, QString *errorString)
{
    Q_ASSERT(actionManager());
    Q_ASSERT(contextManager());
    // create menus
    createFileMenu();
    createEditMenu();
    createFormatMenu();
    createPluginsMenu();
    createConfigurationMenu();
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
            Core::MainWindowActions::A_AppHelp |
            Core::MainWindowActions::A_DebugDialog |
            Core::MainWindowActions::A_FormsAbout |
            Core::MainWindowActions::A_PluginsAbout
            );

    actions.createEditActions(true);
    createActions(actions);

    connectFileActions();
    connectConfigurationActions();
    connectHelpActions();

    contextManager()->updateContext();
    actionManager()->retranslateMenusAndActions();
    readSettings();

    return true;
}

void MainWindow::extensionsInitialized()
{
    // First check if there is a logged user
    if (!UserPlugin::UserModel::instance()->hasCurrentUser()) {
        return;
    }

    // Creating MainWindow interface
//    m_ui = new Internal::Ui::MainWindow();
//    m_ui->setupUi(this);
    setWindowTitle(qApp->applicationName() + " - " + qApp->applicationVersion());

    Utils::Log::addMessage(this , tkTr(Trans::Constants::RAISING_APPLICATION) );
    raise();

    // Start the update checker
    int chk = settings()->value(Core::Constants::S_CHECKUPDATE,Core::Constants::S_CheckUpdate_AtStartup).toInt();
    QDate last = settings()->value(Core::Constants::S_LAST_CHECKUPDATE,QDate::currentDate()).toDate();
    if ((chk == Core::Constants::S_CheckUpdate_AtStartup)
       || ((chk == Core::Constants::S_CheckUpdate_EachWeeks) && (last.addDays(7) < QDate::currentDate()))
       || ((chk == Core::Constants::S_CheckUpdate_EachMonth) && (last.addMonths(1) < QDate::currentDate()))
       || ((chk == Core::Constants::S_CheckUpdate_EachQuarters) && (last.addMonths(3) < QDate::currentDate())) ) {
        messageSplash(tkTr(Trans::Constants::CHECKING_UPDATES));
        statusBar()->addWidget(new QLabel(tkTr(Trans::Constants::CHECKING_UPDATES), this));
        statusBar()->addWidget(updateChecker()->progressBar(this),1);
        connect(updateChecker(), SIGNAL(updateFound()), this, SLOT(updateFound()));
        connect(updateChecker(), SIGNAL(done(bool)), this, SLOT(updateCheckerEnd()));
        updateChecker()->check(Utils::Constants::FREEMEDFORMS_UPDATE_URL);
    }

    // Open Last Opened Forms is necessary
    if (fileManager()->recentFiles().count() > 0) {
        connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(openLastOpenedForm()));
    }

    finishSplash(this);
    show();
}

MainWindow::~MainWindow()
{
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
    QList<Core::IFormIO *> list = pluginManager()->getObjects<Core::IFormIO>();
    // Ask list for the file filters
    QStringList filters;
    foreach(const Core::IFormIO *io, list) {
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
    return loadFile(file, list);
}

bool MainWindow::loadFile(const QString &filename, const QList<Core::IFormIO *> &iolist)
{
    if (filename.isEmpty())
        return false;
    Core::IFormIO *reader = 0;
    QList<Core::IFormIO *> list;
    if (iolist.isEmpty())
         list = pluginManager()->getObjects<Core::IFormIO>();
    else
        list = iolist;

    foreach(Core::IFormIO *io, list) {
        if (io->setFileName(filename) && io->canReadFile()) {
            if (io->loadForm())
                reader = io;
        }
    }
    if (!reader)
        return false;
    fileManager()->setCurrentFile(filename);
    fileManager()->addToRecentFiles(filename);

    QWidget *w = new QWidget(this);
    QHBoxLayout *vb = new QHBoxLayout(w);
    w->setLayout(vb);
    QTreeWidget *t = formManager()->formsTreeWidget(w);
    QWidget *w2 = new QWidget(this);
    QStackedLayout *slayout = formManager()->formsStackedLayout(w2);
    vb->addWidget(t);
    vb->addWidget(w2);
//    if (formManager()->forms().count()>0) {
//        if (formManager()->forms().at(0)->formWidget())
//            vb->addWidget(formManager()->forms().at(0)->formWidget());
//    }
    setCentralWidget(w);
//    if (root) {
//        // inform form manager or create ui
//        formManager()->setFormObjects(root);
//    }
    return true;
}

/** \brief Populate recent files menu */
void MainWindow::aboutToShowRecentFiles()
{
    Core::ActionContainer *recentsMenu = actionManager()->actionContainer(Core::Constants::M_FILE_RECENTFILES);
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

//void MainWindow::createToolBars()
//{
//    tkActionManager *am = mfCore::actionManager();
//    fileToolBar = addToolBar("");
//    fileToolBar->setObjectName("fileToolBar"); // objectName() is used for state save/restoration
//    fileToolBar->addAction(am->action(A_FILE_NEW));   //newAct);
//    fileToolBar->addAction(am->action(A_FILE_OPEN));  //openAct);
//    fileToolBar->addAction(am->action(A_FILE_SAVE));  //saveAct);
//    fileToolBar->addAction(am->action(A_FILE_PRINT));  //printAct);
//    fileToolBar->addAction(am->action(A_INTERPRETOR_GENERAL));  //interpretationAct);
//    fileToolBar->addAction(am->action(A_HELPTEXT_TOGGLER));  //helpTextAct);
//    fileToolBar->addAction(am->action(A_ABOUTFORM));  //aboutThisFormAct);
//}

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
