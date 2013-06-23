/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include <coreplugin/translators.h>
#include <coreplugin/itheme.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/iuser.h>
#include <coreplugin/filemanager.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/actionmanager/mainwindowactions.h>
#include <coreplugin/actionmanager/mainwindowactionhandler.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/dialogs/plugindialog.h>
#include <coreplugin/dialogs/settingsdialog.h>
#include <coreplugin/dialogs/helpdialog.h>

#include <edrcplugin/widgets/rceditorwidget.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/updatechecker.h>
#include <extensionsystem/pluginerrorview.h>
#include <extensionsystem/pluginview.h>
#include <extensionsystem/pluginmanager.h>
#include <translationutils/constants.h>

#include "ui_mainwindow.h"

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

using namespace MainWin;
using namespace MainWin::Internal;
using namespace Trans::ConstantTranslations;

enum { WarnLogMessage = false };

// Getting the Core instances
static inline Utils::UpdateChecker *updateChecker() { return Core::ICore::instance()->updateChecker(); }
static inline Core::ICommandLine *commandLine() { return Core::ICore::instance()->commandLine(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ActionManager *actionManager() { return Core::ICore::instance()->actionManager(); }
static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }
static inline Core::IPatient *patient() { return Core::ICore::instance()->patient(); }
static inline Core::IUser *user() { return Core::ICore::instance()->user(); }
static inline Core::FileManager *fileManager() { return Core::ICore::instance()->fileManager(); }

// SplashScreen Messagers
static inline void messageSplash(const QString &s) {theme()->messageSplashScreen(s); }
static inline void finishSplash(QMainWindow *w) {theme()->finishSplashScreen(w); }

namespace MainWin {
namespace Internal {
    static const char* const  SETTINGS_COUNTDOWN = "applicationCountDown";
} // namespace Internal
} // namespace Core


//--------------------------------------------------------------------------------------------------------
//--------------------------------------- Constructor / Destructor ---------------------------------------
//--------------------------------------------------------------------------------------------------------
MainWindow::MainWindow( QWidget * parent ) :
        Core::IMainWindow(parent)
{
    setObjectName("MainWindow");
//    setWindowIcon(theme()->icon(Core::Constants::ICONFREEDRC));
    messageSplash(tr("Creating Main Window"));
}

bool MainWindow::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);
    // create menus
    createFileMenu();
    createFileNewSubMenu();
    Core::ActionContainer *fmenu = actionManager()->createMenu(Core::Constants::M_FILE_RECENTFILES);
    connect(fmenu->menu(), SIGNAL(aboutToShow()),this, SLOT(aboutToShowRecentFiles()));
    createEditMenu();
    createFormatMenu();
    createConfigurationMenu();
    createHelpMenu();

    Core::MainWindowActions actions;
    actions.setFileActions(
            Core::MainWindowActions::A_FileOpen |
            Core::MainWindowActions::A_FileSave |
            Core::MainWindowActions::A_FileSaveAs |
            Core::MainWindowActions::A_FilePrintPreview |
            Core::MainWindowActions::A_FileQuit);
    actions.setConfigurationActions(
            Core::MainWindowActions::A_AppPreferences |
            Core::MainWindowActions::A_LanguageChange //|
//            Core::MainWindowActions::A_ConfigureMedinTux
            );
    actions.setHelpActions(
            Core::MainWindowActions::A_AppAbout |
            Core::MainWindowActions::A_PluginsAbout |
            Core::MainWindowActions::A_AppHelp |
            Core::MainWindowActions::A_DebugDialog |
            Core::MainWindowActions::A_CheckUpdate //|
//            Core::MainWindowActions::A_QtAbout
            );
//    actions.setTemplatesActions( Core::MainWindowActions::A_Templates_New );
    actions.createEditActions(false);
    createActions(actions);

    connectFileActions();
    connectConfigurationActions();
    connectHelpActions();

    return true;
}

void MainWindow::extensionsInitialized()
{
    // Creating MainWindow interface
    ui = new Internal::Ui::MainWindow();
    ui->setupUi(this);
    setWindowTitle(qApp->applicationName() + " - " + qApp->applicationVersion());

    // Disable some actions when starting as medintux plugin
//    if (commandLine()->value(Core::Constants::CL_MedinTux).toBool()) {
//        this->aNew->setEnabled(false);
//        this->aSave->setEnabled(false);
//        this->aMedinTux->setEnabled(false);
//    }

    // If needed read exchange out file
//    const QString &exfile = commandLine()->value(Core::Constants::CL_ExchangeOutFile).toString();
//    if (!exfile.isEmpty()) {
//        messageSplash(tr("Reading exchange file..."));
//        if (commandLine()->value(Core::Internal::CommandLine::CL_MedinTux).toBool()) {
//            Utils::Log::addMessage(this, tr("Reading a MedinTux exchange file."));
//            QString tmp = Utils::readTextFile(exfile, Utils::DontWarnUser);
//            Utils::Log::addMessage(this, "Content of the exchange file : " + tmp);
//            if (tmp.contains(DrugsDB::Constants::ENCODEDHTML_FREEDIAMSTAG)) {
//                int begin = tmp.indexOf(DrugsDB::Constants::ENCODEDHTML_FREEDIAMSTAG) + QString(DrugsDB::Constants::ENCODEDHTML_FREEDIAMSTAG).length();
//                int end = tmp.indexOf("\"", begin);
//                QString encoded = tmp.mid( begin, end - begin );
//                DrugsDB::DrugsIO::instance()->prescriptionFromXml(drugModel(), QByteArray::fromBase64(encoded.toAscii()));
//            } else if (tmp.contains("DrugsInteractionsEncodedPrescription:")) {
//                // TODO: Manage wrong file encoding */
//                int begin = tmp.indexOf("DrugsInteractionsEncodedPrescription:") + QString("DrugsInteractionsEncodedPrescription:").length();
//                int end = tmp.indexOf("\"", begin);
//                QString encoded = tmp.mid( begin, end - begin );
//                DrugsDB::DrugsIO::instance()->prescriptionFromXml(drugModel(), QByteArray::fromBase64(encoded.toAscii()));
//            }
//        } else {
//            QString extras;
//            DrugsDB::DrugsIO::loadPrescription(drugModel(), exfile, extras);
//            patient()->fromXml(extras);
//        }
//    }

    // Start the update checker
//    if (updateChecker()->needsUpdateChecking(settings()->getQSettings())) {
//        settings()->setPath(Core::ISettings::UpdateUrl, Utils::Constants::FREEDRC_UPDATE_URL);
//        if (checkUpdate())
//            settings()->setValue(Utils::Constants::S_LAST_CHECKUPDATE, QDate::currentDate());
//    }

    createDockWindows();
    readSettings();
    userChanged();

//    setWindowIcon(theme()->icon(Core::Constants::ICONFREEDRC));

    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreOpened()));
    connect(user(), SIGNAL(userChanged()), this, SLOT(userChanged()));
}

MainWindow::~MainWindow()
{
    delete centralWidget();
    setCentralWidget(0);
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "MainWindow::~MainWindow()";
}

void MainWindow::postCoreOpened()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "MainWindow::postCoreOpened()";

    finishSplash(this);

    setCentralWidget(new eDRC::Internal::RcEditorWidget(this));

    actionManager()->retranslateMenusAndActions();
    contextManager()->updateContext();

    resize(800, 600);
    Utils::centerWidget(this, QDesktopWidget().screen());

    raise();
    show();
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
void MainWindow::closeEvent( QCloseEvent *event )
{
    LOG("Closing MainWindow");
    Core::ICore::instance()->requestSaveSettings();

    //    const QList<ICoreListener *> listeners =
    //        ExtensionSystem::PluginManager::instance()->getObjects<Core::ICoreListener>();
    //    foreach (Core::ICoreListener *listener, listeners) {
    //        if (!listener->coreAboutToClose()) {
    //            event->ignore();
    //            return;
    //        }
    //    }

    // Save exchange file
//    QString exfile = commandLine()->value(Core::Internal::CommandLine::CL_ExchangeFileOut).toString();
//    if ((!exfile.isEmpty()) && (!QFile(exfile).exists())) {
//        Utils::Log::addError(this,tkTr(Trans::Constants::FILE_1_DOESNOT_EXISTS).arg(exfile));
//    } else if ((!exfile.isEmpty()) && (QFile(exfile).exists())) {
//        Utils::Log::addMessage(this, QString("Exchange File : %1 ").arg(exfile));
//        Utils::Log::addMessage(this, QString("Running as MedinTux plug : %1 ").arg(commandLine()->value(Core::Internal::CommandLine::CL_MedinTux).toString()));
//        // if is a medintux plugins --> save prescription to exchange file
//        if (commandLine()->value(Core::Internal::CommandLine::CL_MedinTux).toBool()) {
//            QString tmp = DrugsDB::DrugsIO::instance()->prescriptionToHtml(drugModel());
//            tmp.replace("font-weight:bold;", "font-weight:600;");
//            Utils::saveStringToFile(Utils::htmlReplaceAccents(tmp) , exfile, Utils::DontWarnUser);
//        } else {
//            savePrescription(exfile);
//        }
//    }

    Core::ICore::instance()->coreIsAboutToClose();
    writeSettings();
    event->accept();
}

/** \brief Manages language changes : retranslate Ui and ActionManager. */
void MainWindow::changeEvent(QEvent *event)
{
    if (event->type()==QEvent::LanguageChange) {
//        ui->retranslateUi(this);
        actionManager()->retranslateMenusAndActions();
        refreshPatient();
    }
}


/** \brief Populate recent files menu */
void MainWindow::aboutToShowRecentFiles()
{
    Core::ActionContainer *aci = actionManager()->actionContainer(Core::Constants::M_FILE_RECENTFILES);
    aci->menu()->clear();

    bool hasRecentFiles = false;
    foreach (const QString &fileName, fileManager()->recentFiles()) {
        hasRecentFiles = true;
        QAction *action = aci->menu()->addAction(fileName);
        action->setData(fileName);
        connect(action, SIGNAL(triggered()), this, SLOT(openRecentFile()));
    }
    aci->menu()->setEnabled(hasRecentFiles);
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

void MainWindow::userChanged()
{
    setWindowTitle(qApp->applicationName() + " - " + qApp->applicationVersion());
}

void MainWindow::updateCheckerEnd(bool)
{
    delete statusBar();
}

/** \brief Reads main window's settings */
void MainWindow::readSettings()
{
//    settings()->restoreState(this, Account::Constants::S_STATEPREFIX);
    fileManager()->getRecentFilesFromSettings();
}

/** \brief Writes main window's settings */
void MainWindow::writeSettings()
{
//    settings()->saveState(this, Account::Constants::S_STATEPREFIX);
    fileManager()->saveRecentFiles();
    settings()->sync();
}

/** \obsolete */
void MainWindow::createStatusBar()
{
//    statusBar()->showMessage( tkTr(Trans::Constants::READY), 2000 );
}

bool MainWindow::newFile()
{
//    if (drugModel()->drugsList().count()) {
//        bool yes = Utils::yesNoMessageBox(
//                tr("Save actual prescription ?"),
//                tr("The actual prescription is not empty. Do you want to save it before creating a new one ?"));
//        if (yes) {
//            saveFile();
//        }
//    }
//    patient()->clear();
//    refreshPatient();
//    drugModel()->clearDrugsList();
    return true;
}

/** Open the preferences dialog */
bool MainWindow::applicationPreferences()
{
    Core::SettingsDialog dlg(this);
    dlg.exec();
    return true;
}

void MainWindow::changeFontTo(const QFont &font)
{
    Q_UNUSED(font);
}


bool MainWindow::print()
{
//    return ui->m_CentralWidget->printPrescription();
    return true;
}

bool MainWindow::printPreview()
{
//    ui->m_CentralWidget->printPreview();
    return true;
}

/** \brief Runs the MedinTux configurator */
bool MainWindow::configureMedintux()
{
//    Internal::configureMedinTux();
    return true;
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
//    QString f = QFileDialog::getOpenFileName(this,
//                                             tkTr(Trans::Constants::OPEN_FILE),
//                                             QDir::homePath(),
//                                             tkTr(Core::Constants::FREEDIAMS_FILEFILTER) );
//    if (f.isEmpty())
//        return false;
//    //    QString f = "/Users/eric/prescription.di";
//    readFile(f);
//    fileManager()->setCurrentFile(f);
//    fileManager()->addToRecentFiles(f);
    return true;
}

void MainWindow::readFile(const QString &file)
{
    Q_UNUSED(file);
    // TODO: code here */
//    QString datas;
//    if (drugModel()->rowCount() > 0) {
//        int r = Utils::withButtonsMessageBox(
//                tr("Opening a prescription : merge or replace ?"),
//                tr("There is a prescription inside editor, do you to replace it or to add the opened prescription ?"),
//                QString(), QStringList() << tr("Replace prescription") << tr("Add to prescription"),
//                tr("Open a prescription") + " - " + qApp->applicationName());
//        if (r == 0) {
//            DrugsDB::DrugsIO::loadPrescription(drugModel(), file, datas, DrugsDB::DrugsIO::ReplacePrescription);
//        } else if (r==1) {
//            DrugsDB::DrugsIO::loadPrescription(drugModel(), file, datas, DrugsDB::DrugsIO::AppendPrescription);
//        }
//    } else {
//        DrugsDB::DrugsIO::loadPrescription(drugModel(), file, datas, DrugsDB::DrugsIO::ReplacePrescription);
//    }
////    qWarning() << datas;
//    patient()->fromXml(datas);
//    refreshPatient();
}

void MainWindow::createDockWindows()
{
//    QDockWidget *dock = m_TemplatesDock = new QDockWidget(tkTr(Trans::Constants::TEMPLATES), this);
//    dock->setObjectName("templatesDock");
//    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
//    dock->setWidget(new Templates::TemplatesView(dock));
//    addDockWidget(Qt::RightDockWidgetArea, dock);
//    QMenu *menu = actionManager()->actionContainer(Core::Constants::M_TEMPLATES)->menu();
//    menu->addAction(dock->toggleViewAction());
}

