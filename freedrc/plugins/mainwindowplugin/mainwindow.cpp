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
#include <edrcplugin/constants.h>
#include <edrcplugin/consultresult.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/stylehelper.h>
#include <utils/updatechecker.h>
#include <extensionsystem/pluginerrorview.h>
#include <extensionsystem/pluginview.h>
#include <extensionsystem/pluginmanager.h>
#include <translationutils/constants.h>
#include <translationutils/trans_patient.h>
#include <translationutils/trans_filepathxml.h>
#include <translationutils/trans_msgerror.h>

#include "ui_mainwindow.h"

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

using namespace MainWin;
using namespace MainWin::Internal;
using namespace eDRC;
using namespace eDRC::Internal;
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
const char* const  SETTINGS_COUNTDOWN = "applicationCountDown";

const char* const  XML_PATIENT_NAME = "patient";
const char* const  XML_DATE = "date";
const char* const  XML_USERNAME = "user";

} // namespace Internal
} // namespace Core


//--------------------------------------------------------------------------------------------------------
//--------------------------------------- Constructor / Destructor ---------------------------------------
//--------------------------------------------------------------------------------------------------------
MainWindow::MainWindow( QWidget * parent ) :
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

/**
 * When all dependent plugins are initialized, start the second initialization:
 * - setup ui
 * - start update checker
 * - read settings
 */
void MainWindow::extensionsInitialized()
{
    // Creating MainWindow interface
    ui = new Internal::Ui::MainWindow();
    ui->setupUi(this);
    ui->patientName->setPlaceholderText(tkTr(Trans::Constants::PATIENT_NAME));
    ui->date->setClearIcon(theme()->iconFullPath(Core::Constants::ICONCLEAR));
    ui->date->setDateIcon(theme()->iconFullPath(Core::Constants::ICONDATE));
    ui->date->setDate(QDate::currentDate());

    ui->clearButton->setIcon(theme()->icon(Core::Constants::ICONCLEAR));
    connect(ui->clearButton, SIGNAL(clicked()), this, SLOT(clearUi()));

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
    if (updateChecker()->needsUpdateChecking(settings()->getQSettings())) {
        settings()->setPath(Core::ISettings::UpdateUrl, Utils::Constants::FREEDRC_UPDATE_URL);
        if (checkUpdate())
            settings()->setValue(Utils::Constants::S_LAST_CHECKUPDATE, QDate::currentDate());
    }

    createDockWindows();
    readSettings();

    setWindowTitle(qApp->applicationName() + " - " + qApp->applicationVersion());
    setWindowIcon(theme()->icon(Core::Constants::ICONFREEDRC));

    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreOpened()));
}

MainWindow::~MainWindow()
{
    delete centralWidget();
    setCentralWidget(0);
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "MainWindow::~MainWindow()";
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
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "MainWindow::postCoreOpened()";

    finishSplash(this);

    //setCentralWidget(new eDRC::Internal::RcEditorWidget(this));

    actionManager()->retranslateMenusAndActions();
    contextManager()->updateContext();

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

/** Clear UI editors */
void MainWindow::clearUi()
{
    ui->date->setDate(QDate::currentDate());
    ui->patientName->clear();
    ui->crEditor->clear();
}

void MainWindow::updateCheckerEnd(bool)
{
    delete statusBar();
}

/** Reads main window's settings */
void MainWindow::readSettings()
{
    // Main Application settings
    settings()->restoreState(this);
    fileManager()->getRecentFilesFromSettings();
    fileManager()->getMaximumRecentFilesFromSettings();
//    m_AutomaticSaveInterval = settings()->value(Core::Constants::S_SAVEINTERVAL, 600).toUInt(); // Default = 10 minutes
//    m_OpenLastOpenedForm = settings()->value(Core::Constants::S_OPENLAST, true).toBool();

    // Main Widget settings
//    m_HelpTextShow = settings()->value(Core::Constants::S_SHOWHELPTEXT, true).toBool();

    Utils::StyleHelper::setBaseColor(Utils::StyleHelper::DEFAULT_BASE_COLOR);
}

/** \brief Writes main window's settings */
void MainWindow::writeSettings()
{
    settings()->saveState(this);
    // Recent managers
    fileManager()->saveRecentFiles();
    // Main Application settings
//    settings()->setValue(Core::Constants::S_SAVEINTERVAL, m_AutomaticSaveInterval);
//    settings()->setValue(Core::Constants::S_OPENLAST, m_OpenLastOpenedForm);
//    // Main Widget settings
//    settings()->setValue(Core::Constants::S_SHOWHELPTEXT, m_HelpTextShow);
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
    // get filename
    QString fileName = QFileDialog::getSaveFileName(this, tkTr(Trans::Constants::SAVE_FILE),
                                QDir::homePath(),
                                tkTr(Core::Constants::FREEDRC_FILEFILTER));
    if (fileName.isEmpty())
        return false;

    // Create extra-xml content
    QDomDocument doc;
    QDomElement pe = doc.createElement(::XML_PATIENT_NAME);
    QDomText pet = doc.createTextNode(ui->patientName->text());
    pe.appendChild(pet);
    doc.appendChild(pe);
    QDomElement dt = doc.createElement(::XML_DATE);
    QDomText dtv = doc.createTextNode(ui->date->date().toString(Qt::ISODate));
    dt.appendChild(dtv);
    doc.appendChild(dt);

    // Get full content XML
    ConsultResult cr = ui->crEditor->submit();
    QString xml = cr.toXml(doc.toString());

    // Save to file
    bool ok = Utils::saveStringToFile(xml, fileName, Utils::Overwrite, Utils::DontWarnUser);
    if (ok) {
        fileManager()->addToRecentFiles(fileName);
        fileManager()->setCurrentFile(fileName);
    }
    return ok;
}

bool MainWindow::saveFile()
{
    return saveAsFile();
}

bool MainWindow::openFile()
{
    QString f = QFileDialog::getOpenFileName(this,
                                             tkTr(Trans::Constants::OPEN_FILE),
                                             QDir::homePath(),
                                             tkTr(Core::Constants::FREEDRC_FILEFILTER) );
    if (f.isEmpty())
        return false;
//        QString f = QString("%1/%2/%3")
//                .arg(settings()->path(Core::ISettings::BundleResourcesPath))
//                .arg("textfiles/freedrctest")
//                .arg("cr_2013_09.cr");
    readFile(f);
    fileManager()->addToRecentFiles(f);
    fileManager()->setCurrentFile(f);
    return true;
}

void MainWindow::readFile(const QString &file)
{
    Q_UNUSED(file);
    QString extra;
    ConsultResult cr = ConsultResult::fromXml(Utils::readTextFile(file), &extra);
    extra = extra.simplified();
    if (!extra.isEmpty()) {
        QDomDocument doc;
        QString error;
        int line = 0;
        int col = 0;
        if (!doc.setContent(extra, &error, &line, &col)) {
            LOG_ERROR(tkTr(Trans::Constants::ERROR_1_LINE_2_COLUMN_3).arg(error).arg(line).arg(col));
            LOG_ERROR(QString("Unable to read extra-xml content of file: %1").arg(file));
        } else {
            QDomElement extra = doc.firstChildElement(Constants::XML_EXTRA_TAG);
            QDomElement patient = extra.firstChildElement(::XML_PATIENT_NAME);
            ui->patientName->setText(patient.text());
            QDomElement date = extra.firstChildElement(::XML_DATE);
            ui->date->setDate(QDate::fromString(date.text(), Qt::ISODate));
        }
    }
    ui->crEditor->clear();
    ui->crEditor->setConsultResult(cr);
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

