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
#include "constants.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/translators.h>
#include <coreplugin/itheme.h>
#include <coreplugin/filemanager.h>

#include <coreplugin/actionmanager/mainwindowactions.h>
#include <coreplugin/actionmanager/mainwindowactionhandler.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/dialogs/plugindialog.h>
#include <coreplugin/dialogs/settingsdialog.h>
#include <coreplugin/dialogs/helpdialog.h>
//#include <coreplugin/idocumentprinter.h>

#include <icdplugin/icddownloader.h>
#include <icdplugin/constants.h>

// TEST
#include <icdplugin/icdcodeselector.h>
#include <icdplugin/simpleicdmodel.h>
#include <icdplugin/icddialog.h>
// TEST

#include <extensionsystem/pluginerrorview.h>
#include <extensionsystem/pluginview.h>
#include <extensionsystem/pluginmanager.h>

#include <translationutils/constanttranslations.h>
#include <utils/log.h>
#include <utils/global.h>
#include <utils/updatechecker.h>
//#include <utils/iconbadgealert.h>

#include "ui_mainwindow.h"

#include <QSettings>
#include <QCoreApplication>
#include <QDir>
#include <QStatusBar>
#include <QFileDialog>

using namespace MainWin;
using namespace MainWin::Internal;
using namespace Trans::ConstantTranslations;

// Getting the Core instances
static inline Utils::UpdateChecker *updateChecker() { return Core::ICore::instance()->updateChecker(); }
static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }
//static inline Core::ICommandLine *commandLine() { return Core::ICore::instance()->commandLine(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ActionManager *actionManager() { return Core::ICore::instance()->actionManager(); }
static inline Core::FileManager *fileManager() { return Core::ICore::instance()->fileManager(); }
//static inline Core::IDocumentPrinter *printer() {return ExtensionSystem::PluginManager::instance()->getObject<Core::IDocumentPrinter>();}

// SplashScreen Messagers
static inline void messageSplash(const QString &s) {theme()->messageSplashScreen(s); }
static inline void finishSplash(QMainWindow *w) {theme()->finishSplashScreen(w); }


namespace MainWin {
namespace Internal {

class MainWinPrivate {
public:
    MainWinPrivate(MainWindow *parent) :
            q(parent)
    {}

    ~MainWinPrivate()
    {
    }

//    bool readExchangeFile(const QString &msg)
//    {
//        QString exfile = commandLine()->value(Core::CommandLine::CL_ExchangeOutFile).toString();
//        if (!exfile.isEmpty()) {
//            messageSplash(msg);
//            if (QFileInfo(exfile).isRelative())
//                exfile.prepend(qApp->applicationDirPath() + QDir::separator());
//            QString tmp;
//            if (QFile(exfile).exists())
//                tmp = Utils::readTextFile(exfile, Utils::DontWarnUser);
//            //            Utils::Log::addMessage(this, "Content of the exchange file : " + tmp);
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
//            } else if (tmp.startsWith("<?xml") && tmp.contains("<FreeDiams>", Qt::CaseInsensitive) && tmp.contains("</FreeDiams>", Qt::CaseInsensitive)) {
//                // TODO: Read patients datas ? */
//                DrugsDB::DrugsIO::instance()->prescriptionFromXml(drugModel(), tmp);
//            } else {
//                return false;
//            }
//        }
//        return true;
//    }

public:

private:
    MainWindow *q;
};


} // namespace Internal
} // namespace Core

//--------------------------------------------------------------------------------------------------------
//--------------------------------------- Constructor / Destructor ---------------------------------------
//--------------------------------------------------------------------------------------------------------
MainWindow::MainWindow( QWidget * parent ) :
        Core::IMainWindow(parent),
        d(new Internal::MainWinPrivate(this))
{
    setObjectName("MainWindow");
    messageSplash(tr("Creating Main Window"));

    // Install the event filter
    qApp->installEventFilter(this);
}

/** \brief Create the menubar and the MainWindow actions. */
bool MainWindow::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);
    // create menus
    createFileMenu();
    Core::ActionContainer *fmenu = actionManager()->actionContainer(Core::Constants::M_FILE);
    connect(fmenu->menu(), SIGNAL(aboutToShow()),this, SLOT(aboutToShowRecentFiles()));

    createEditMenu();

    // Create prescription menu
//    Core::ActionContainer *pmenu = actionManager()->actionContainer(DrugsWidget::Constants::M_PLUGINS_DRUGS);
//    if (!pmenu) {
//        pmenu = actionManager()->createMenu(DrugsWidget::Constants::M_PLUGINS_DRUGS);
//        pmenu->appendGroup(DrugsWidget::Constants::G_PLUGINS_VIEWS);
//        pmenu->appendGroup(DrugsWidget::Constants::G_PLUGINS_MODES);
//        pmenu->appendGroup(DrugsWidget::Constants::G_PLUGINS_SEARCH);
//        pmenu->appendGroup(DrugsWidget::Constants::G_PLUGINS_DRUGS);
//        pmenu->appendGroup(DrugsWidget::Constants::G_PLUGINS_INTERACTIONS);
//        pmenu->setTranslations(DrugsWidget::Constants::DRUGSMENU_TEXT);
//    }
//    Q_ASSERT(pmenu);
//    actionManager()->actionContainer(Core::Constants::MENUBAR)->addMenu(pmenu, DrugsWidget::Constants::G_PLUGINS_DRUGS);

    createConfigurationMenu();
    createHelpMenu();

    Core::MainWindowActions actions;
    actions.setFileActions(
            Core::MainWindowActions::A_FileNew  |
            Core::MainWindowActions::A_FileOpen |
            Core::MainWindowActions::A_FileSave |
            Core::MainWindowActions::A_FileSaveAs |
            Core::MainWindowActions::A_FilePrint |
            Core::MainWindowActions::A_FilePrintPreview |
            Core::MainWindowActions::A_FileQuit
            );
    actions.setConfigurationActions(
            Core::MainWindowActions::A_AppPreferences |
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
    actions.createEditActions(true);
    createActions(actions);

    connectFileActions();
    connectConfigurationActions();
    connectHelpActions();

    // Creating MainWindow interface
    m_ui = new Internal::Ui::MainWindow();
    m_ui->setupUi(this);

    return true;
}

/** \brief Before MainWindow is shown.
*/
void MainWindow::extensionsInitialized()
{
    messageSplash(tr("Initializing ICD10 database"));
//    m_ui->m_CentralWidget->initialize();

//    // If needed read exchange file
//    if (!d->readExchangeFile(tr("Reading exchange file..."))) {
//        Utils::Log::addError(this, tkTr(Trans::Constants::FILE_1_ISNOT_READABLE).arg(commandLine()->value(Core::CommandLine::CL_ExchangeOutFile).toString()),
//                             __FILE__, __LINE__);
//    }

    // Start the update checker
    if (updateChecker()->needsUpdateChecking(settings()->getQSettings())) {
        settings()->setPath(Core::ISettings::UpdateUrl, Utils::Constants::FREEICD_UPDATE_URL);
        if (checkUpdate())
            settings()->setValue(Utils::Constants::S_LAST_CHECKUPDATE, QDate::currentDate());
    }

//    createDockWindows();
    readSettings();
    show();
    raise();

    // Connect post core initialization
    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreInitialization()));
}

MainWindow::~MainWindow()
{
    delete m_ui->widget; m_ui->widget=0;
    delete m_ui;
}

void MainWindow::postCoreInitialization()
{
    finishSplash(this);

    contextManager()->updateContext();
    actionManager()->retranslateMenusAndActions();

    setWindowTitle(qApp->applicationName() + " - " + qApp->applicationVersion());
    setWindowIcon(theme()->icon(Core::Constants::ICONFREEICD));
//    refreshPatient();
//    updateIconBadgeOnMacOs();

}

/** \brief Slot specific to MacOs -> update the application icon badge to number of founded interactions. */
void MainWindow::updateIconBadgeOnMacOs()
{
#ifdef Q_OS_MAC
//    int n = drugModel()->interactionsManager()->getAllInteractionsFound().count();
//    if (n > 0) {
//        Utils::MacOs::setIconBadgeLabel(QString::number(n));
//    }
#endif
}

/**
  \brief Close the main window and the application
  \todo Add  ICoreListener
*/
void MainWindow::closeEvent(QCloseEvent *event)
{
    Utils::Log::addMessage(this, "Closing MainWindow");
    Core::ICore::instance()->requestSaveSettings();

    //    const QList<ICoreListener *> listeners =
    //        ExtensionSystem::PluginManager::instance()->getObjects<Core::ICoreListener>();
    //    foreach (Core::ICoreListener *listener, listeners) {
    //        if (!listener->coreAboutToClose()) {
    //            event->ignore();
    //            return;
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
        if (m_ui)
            m_ui->retranslateUi(this);
        actionManager()->retranslateMenusAndActions();
//        refreshPatient();
    }
}

/** \brief Event filter created to catch the FileOpenEvent from MacOS X */
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::FileOpen) {
        QFileOpenEvent *fe = static_cast<QFileOpenEvent *>(event);
        if (fe) {
            readFile(fe->file());
        }
        return true;
    }
    return QObject::eventFilter(obj, event);
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

void MainWindow::updateCheckerEnd(bool error)
{
    // this code avoid deletion of the resizer corner of the mainwindow
    delete statusBar();
    statusBar()->hide();
}

/** \brief Reads main window's settings */
void MainWindow::readSettings()
{
    settings()->restoreState(this, Constants::S_STATEPREFIX);
    fileManager()->getRecentFilesFromSettings();
}

/** \brief Writes main window's settings */
void MainWindow::writeSettings()
{
    settings()->saveState(this, Constants::S_STATEPREFIX);
    fileManager()->saveRecentFiles();
    settings()->sync();
}

/** \obsolete */
void MainWindow::createStatusBar()
{
    statusBar()->showMessage( tkTr(Trans::Constants::READY), 2000 );
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

/**
  \brief Open the preferences dialog
  \sa mfDrugsPreferences
*/
bool MainWindow::applicationPreferences()
{
    Core::SettingsDialog dlg(this);
    dlg.exec();
    return true;
}

/** \brief Change the font of the viewing widget */
void MainWindow::changeFontTo( const QFont &font )
{
}

bool MainWindow::saveAsFile()
{
    // TODO: save filename */
    return saveIcdCollection();
}

bool MainWindow::saveFile()
{
    return saveIcdCollection();
}

/**
  \brief Saves a prescription. If fileName is empty, user is ask about a file name.
  \sa openPrescription()
  \sa DrugsIO
*/
bool MainWindow::saveIcdCollection(const QString &fileName)
{
//    const QList<Core::PrintedDocumentTracer> &pdfs = printer()->printedDocs();
//    QString extraDatas = patient()->toXml();
//    foreach(const Core::PrintedDocumentTracer &t, pdfs) {
//        extraDatas += QString("<Printed file=\"%1\" docName=\"%2\" dateTime=\"%3\" userUid=\"%4\"/>\n")
//                       .arg(t.fileName())
//                       .arg(t.documentName())
//                       .arg(t.dateTime().toString(Qt::ISODate))
//                       .arg(t.userUid());
//    }
//    if (commandLine()->value(Core::CommandLine::CL_EMR_Name).isValid()) {
//        extraDatas.append(QString("<EMR name=\"%1\"").arg(commandLine()->value(Core::CommandLine::CL_EMR_Name).toString()));
//        if (commandLine()->value(Core::CommandLine::CL_EMR_Name).isValid()) {
//            extraDatas.append(QString(" uid=\"%1\"").arg(commandLine()->value(Core::CommandLine::CL_EMR_Uid).toString()));
//        }
//        extraDatas.append("/>");
//    }
    QString xml = m_ui->widget->collectionToXml();

    return Utils::saveStringToFile(xml, QDir::homePath(), QCoreApplication::translate(ICD::Constants::ICDCONSTANTS_TR_CONTEXT, ICD::Constants::FREEICD_FILEFILTER));
}

/**
  \brief Opens a prescription saved using savePrescription().
  \sa savePrescription()
  \sa DrugsIO
*/
bool MainWindow::openFile()
{
    QString f = QFileDialog::getOpenFileName(this,
                                             tkTr(Trans::Constants::OPEN_FILE),
                                             QDir::homePath(),
                                             QCoreApplication::translate(ICD::Constants::ICDCONSTANTS_TR_CONTEXT, ICD::Constants::FREEICD_FILEFILTER) );
    if (f.isEmpty())
        return false;
    //    QString f = "/Users/eric/prescription.di";
    readFile(f);
    fileManager()->setCurrentFile(f);
    fileManager()->addToRecentFiles(f);
    return true;
}

void MainWindow::readFile(const QString &file)
{
    m_ui->widget->openFile(file);
//    patient()->fromXml(datas);
//    refreshPatient();
}

void MainWindow::createDockWindows()
{
//    // Create template dock
//    QDockWidget *tdock = new QDockWidget(tkTr(Trans::Constants::TEMPLATES), this);
//    tdock->setObjectName("templatesDock");
//    tdock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
//    tdock->setWidget(new Templates::TemplatesView(tdock));
//    addDockWidget(Qt::RightDockWidgetArea, tdock);
//    QMenu *menu = actionManager()->actionContainer(Core::Constants::M_TEMPLATES)->menu();
//    menu->addAction(tdock->toggleViewAction());
}

