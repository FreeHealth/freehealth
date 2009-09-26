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
#include <utils/updatechecker.h>
#include <utils/log.h>

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
#include <coreplugin/dialogs/plugindialog.h>
#include <coreplugin/dialogs/settingsdialog.h>

#include <extensionsystem/pluginerrorview.h>
#include <extensionsystem/pluginview.h>
#include <extensionsystem/pluginmanager.h>


// include usertoolkit headers
//#include <tkUserManager.h>

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

// GlobalFormModel TESTING
#include <QTreeView>
#include <coreplugin/formglobalmodel.h>
// END

using namespace Core;
using namespace Trans::ConstantTranslations;

inline static Core::ActionManager *actionManager() { return Core::ICore::instance()->actionManager(); }
inline static Core::FileManager *fileManager() { return Core::ICore::instance()->fileManager(); }
inline static ExtensionSystem::PluginManager *pluginManager() { return ExtensionSystem::PluginManager::instance(); }
inline static Core::FormManager *formManager() { return Core::ICore::instance()->formManager(); }
inline static Core::ISettings *settings() { return Core::ICore::instance()->settings(); }

//--------------------------------------------------------------------------------------------------------
//--------------------------------------- Constructor / Destructor ---------------------------------------
//--------------------------------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget * parent)
          : Core::IMainWindow(parent)//, fileToolBar(0), editToolBar(0),
//          m_pRootObject(0), m_MainWidget(0), m_AutomaticSaveInterval(0), m_TimerId(0),
//          m_UserManager(0)
{
    setObjectName("MainWindow");
    setWindowTitle(qApp->applicationName());
}

bool MainWindow::initialize(const QStringList &arguments, QString *errorString)
{
    qWarning() << "mainWin initialize" << arguments << *errorString;
    // Manage window
    createMenusAndActions();
    createToolBars();
    createStatusBar();
    retranslateUi();

    // connect recentfiles about to show
    Core::ActionContainer *fmenu = actionManager()->actionContainer(Core::Constants::M_FILE);
    connect(fmenu->menu(), SIGNAL(aboutToShow()),this, SLOT(aboutToShowRecentFiles()));

    readSettings();
    restoreState(windowState);
////    refreshToolbarMenu();
//
//    if (Utils::isDebugCompilation())
//        setCentralWidget(Core::ICore::settings()->getTreeWidget(this));
//
//    // Some things need to be set just after the form show
//    QTimer::singleShot(0, this, SLOT(afterTheShow()));
//
//    // Make connections with menus/actions
//    QMetaObject::connectSlotsByName(this);
    return true;
}

void MainWindow::extensionsInitialized()
{
    // Start the update checker
    connect(Core::ICore::instance()->updateChecker(), SIGNAL(updateFound()), this, SLOT(updateFound()));
    Core::ICore::instance()->updateChecker()->check(Utils::Constants::FREEMEDFORMS_UPDATE_URL);

    // Open the last form
    if ((m_OpenLastOpenedForm) && (fileManager()->recentFiles().count())) {
        QFile file(fileManager()->recentFiles().at(0));
        if (file.exists())
            loadFile(file.fileName());
    }

//    QTreeView *tree = new QTreeView(this);
//    tree->setModel(new FormGlobalModel(this));
//
//    setCentralWidget(tree);

    show();
}

MainWindow::~MainWindow()
{
}

/** \brief Manages language changes : retranslate Ui and ActionManager. */
void MainWindow::changeEvent(QEvent *event)
{
    if (event->type()==QEvent::LanguageChange) {
        retranslateUi();
        Core::ICore::instance()->actionManager()->retranslateMenusAndActions();
    }
}

void MainWindow::closeEvent(QEvent *event)
{
    writeSettings();
    event->accept();
}

QStatusBar *MainWindow::statusBar()
{
    return QMainWindow::statusBar();
}

bool MainWindow::aboutQt()
{
    aboutPlugins();
    return true;
}

//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
bool MainWindow::openFile()
{
    // Get all IFormIO from pluginsmanager
    QList<IFormIO *> list = pluginManager()->getObjects<IFormIO>();
    // Ask list for the file filters
    QStringList filters;
    foreach(const IFormIO *io, list) {
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

bool MainWindow::loadFile(const QString &filename, const QList<IFormIO *> &iolist)
{
    if (filename.isEmpty())
        return false;
    IFormIO *reader = 0;
    QList<IFormIO *> list;
    if (iolist.isEmpty())
         list = pluginManager()->getObjects<IFormIO>();
    else
        list = iolist;

    foreach(IFormIO *io, list) {
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



//void MainWindow::showWidget()
//{
//    if (m_pRootObject == 0)
//        return;
//    m_MainWidget = new mfMainWidget(m_pRootObject, this);
//    setCentralWidget(m_MainWidget);
//    // each time a widget is added to this main window restoreState should be called
//    restoreState(windowState);
//}
//
//void MainWindow::on_actionInterpretor_triggered()
//{
//#ifdef DEBUG
//    if (!m_pRootObject)
//    {
//        QMessageBox::warning(this, qApp->applicationName(),
//                              tr("No existing form to interpret."));
//        return;
//    }
//    mfDialogInterpret * interpret = new mfDialogInterpret(m_pRootObject , this);
//    interpret->exec();
//#else
//    QMessageBox::information(this, qApp->applicationName(), tkTr(FEATURE_NOT_IMPLEMENTED));
//#endif
//}
//
//void MainWindow::userManagerRequested()
//{
//    if (!m_UserManager) {
//        m_UserManager = new tkUserManager(this);
//        m_UserManager->initialize();
//        m_UserManager->show();
//    } else {
//        qApp->setActiveWindow(m_UserManager);
//        m_UserManager->activateWindow();
//    }
//}
//
////--------------------------------------------------------------------------------------------------------
////------------------------------------------ MANAGING EVENTS ---------------------------------------------
////--------------------------------------------------------------------------------------------------------
//void MainWindow::closeEvent(QCloseEvent *event)
//{
//    if (maybeSave()){
//        thread.quit();
//        this->killTimer(m_TimerId);
//        writeSettings();
//        thread.wait();
//        event->accept();
//    }
//    else
//        event->ignore();
//}
//
//
//void MainWindow::timerEvent(QTimerEvent *event)
//{
//    if (event->timerId() == m_TimerId)
//        thread.saveRecoveringFile(m_pRootObject);
//}
//
//
////--------------------------------------------------------------------------------------------------------
////------------------------------------------- File Menu Slots --------------------------------------------
////--------------------------------------------------------------------------------------------------------
//void MainWindow::documentWasModified()
//{
//    // Function connected with m_pRootObject valueChanged signal
//    if (isWindowModified())return;
//    // Start a timer for automatic save
//    statusBar()->showMessage(tr("Starting crash recoverer every %1 seconds.")
//                              .arg(m_AutomaticSaveInterval), 1000);
//    m_TimerId = this->startTimer(m_AutomaticSaveInterval * 1000);
//    thread.saveRecoveringFile(m_pRootObject);
//    setWindowModified(true);
//}
//

//bool MainWindow::print()
//{
//    if (!m_pRootObject)return false;
//    mfPrinter printer(m_pRootObject);
//    printer.print();
//    return true;
//}
//
//bool MainWindow::maybeSave()
//{
//    // Test if form was modified --> ask user what to do
//    if (m_pRootObject)
//        if (m_pRootObject->isModified()){
//        int ret = QMessageBox::warning(this, qApp->applicationName(),
//                                        tr("The document has been modified.\n"
//                                            "Do you want to save your changes?"),
//                                        QMessageBox::Yes | QMessageBox::Default,
//                                        QMessageBox::No,
//                                        QMessageBox::Cancel | QMessageBox::Escape);
//        if (ret == QMessageBox::Yes)
//            return saveFile();
//        else
//            if (ret == QMessageBox::Cancel)
//                return false;
//    }
//    return true;
//}
//
//void MainWindow::refreshWholeUi()
//{
//    // Delete all object that need to be deleted
//    if (m_MainWidget)
//        delete m_MainWidget;
//    m_MainWidget = 0;
//    showWidget();
//    connect(m_pRootObject, SIGNAL(formUiMustBeUpdated()),
//             this, SLOT(refreshWholeUi()));
//}
//
//void MainWindow::on_actionHelpTextToggler_triggered()
//{
//    m_HelpTextShow = !m_HelpTextShow;
//
//    foreach (mfAbstractWidget *widget, m_MainWidget->findChildren<mfAbstractWidget*>())
//    {
//        if (widget->getObject()->type() == mfObject::Type_HelpText)
//            widget->setVisible(m_HelpTextShow);
//    }
//}
//
//
////--------------------------------------------------------------------------------------------------------
////--------------------------------------- Creating Actions and Menus -------------------------------------
////--------------------------------------------------------------------------------------------------------
//void MainWindow::refreshToolbarMenu()
//{
//    // clear toolbar menu then re-populate it
////    QMenu *m = mfCore::actionManager()->menu(M_TOOLBARS);
////    m->clear();
////    QList<QToolBar*> toolbars = this->findChildren<QToolBar*>();
////    foreach(QToolBar* tb, toolbars){
////        if (tb->windowTitle().isEmpty())
////            continue;
////        QAction * a = m->addAction(tb->windowTitle());
////        a->setObjectName(tb->objectName());
////        a->setCheckable(true);
////        a->setChecked(tb->isVisible());
////        connect(a, SIGNAL(toggled(bool)), this, SLOT(toolbarToggled(bool)));
////    }
//}
//
//void MainWindow::formEditor()
//{
//    //      FormEditorWindow *window = new FormEditorWindow(this);
//    //      window->show();
//}
//
void MainWindow::createMenusAndActions()
{
    createFileMenu();
    createEditMenu();
    createFormatMenu();
    createPluginsMenu();
    createConfigurationMenu();
    createHelpMenu();

    Core::MainWindowActions actions;
    actions.setFileActions(
            Core::MainWindowActions::A_FileNew  |
            Core::MainWindowActions::A_FileOpen |
            Core::MainWindowActions::A_FileSave |
            Core::MainWindowActions::A_FileSaveAs |
            Core::MainWindowActions::A_FilePrint |
            Core::MainWindowActions::A_FileQuit);
    actions.setConfigurationActions(
            Core::MainWindowActions::A_AppPreferences |
            Core::MainWindowActions::A_PluginsPreferences |
            Core::MainWindowActions::A_LangageChange);
    actions.setHelpActions(
            Core::MainWindowActions::A_AppAbout |
            Core::MainWindowActions::A_AppHelp |
            Core::MainWindowActions::A_DebugDialog |
            Core::MainWindowActions::A_FormsAbout |
            Core::MainWindowActions::A_QtAbout);
    actions.createEditActions(true);
    createActions(actions);

    connectFileActions();
    connectConfigurationActions();
    connectHelpActions();

    QAction *a = 0;
    Core::Command *cmd = 0;
    Core::ActionManager *am = Core::ICore::instance()->actionManager();
    Core::ITheme *theme = Core::ICore::instance()->theme();
    QList<int> ctx = QList<int>() << Core::Constants::C_GLOBAL_ID;
    Core::ActionContainer *menu = am->actionContainer(Core::Constants::M_PLUGINS);
    Q_ASSERT(menu);
    if (!menu)
        return;

//    a = aUserManager = new QAction(this);
//    a->setIcon(theme->icon(Core::Constants::ICONUSERMANAGER));
//    cmd = am->registerAction(a, Core::Constants::A_USERMANAGER, ctx);
//    cmd->setTranslations(Trans::Constants::USERMANAGER_TEXT);
//    menu->addAction(cmd, Core::Constants::G_PLUGINS_USERMANAGER);
//    connect(aUserManager, SIGNAL(triggered()), this, SLOT(userManagerRequested()));

//    for (int i = 0; i < MaxRecentFiles; ++i)
//    {
//        recentFilesAct[i] = new QAction(this);
//        recentFilesAct[i]->setVisible(false);
//        connect(recentFilesAct[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
//        am->addAction(recentFilesAct[i], QString("RECENT%1").arg(i), G_FILE_RECENTS);
//    }

//    // Menu interpretor
//    am->createMenu(M_INTERPRETOR, MENUBAR, M_INTERPRETOR_TEXT , "", this); //interpretMenu = menuBar()->addMenu(" ");
//    am->appendGroup(G_INTERPRETOR_GENERAL, M_INTERPRETOR);
//    am->createAction(A_INTERPRETOR_GENERAL, G_INTERPRETOR_GENERAL, this);
//    am->setActionDatas(A_INTERPRETOR_GENERAL, INTERPRETOR_GENERAL_TEXT, INTERPRETOR_GENERAL_TEXT, ""); // interpretMenu->addAction(interpretationAct);
//
    Core::ICore::instance()->contextManager()->updateContext();
    Core::ICore::instance()->actionManager()->retranslateMenusAndActions();
}


//--------------------------------------------------------------------------------------------------------
//-------------------------------------- ToolBars and StatusBar ------------------------------------------
//--------------------------------------------------------------------------------------------------------
void MainWindow::createToolBars()
{
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
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::aboutPlugins()
{
    Core::PluginDialog dialog(this);
    dialog.exec();
}

bool MainWindow::applicationPreferences()
{
    qWarning() << "appPrefs";
    Core::SettingsDialog dlg(this);
    dlg.exec();
    return true;
}

////--------------------------------------------------------------------------------------------------------
////-------------------------------------- Settings --------------------------------------------------------
////--------------------------------------------------------------------------------------------------------
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
}


void MainWindow::retranslateUi()
{
//    fileToolBar->setWindowTitle(tr("&File"));
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
