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

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants.h>
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
#include <coreplugin/ipatient.h>
#include <coreplugin/idocumentprinter.h>

#include <fdcoreplugin/coreimpl.h>
#include <fdcoreplugin/commandlineparser.h>

#include <fdmainwindowplugin/medintux.h>

#include <drugsplugin/constants.h>
#include <drugsplugin/drugswidgetmanager.h>

#include <drugsbaseplugin/drugsio.h>

#include <templatesplugin/templatesview.h>

#include <extensionsystem/pluginerrorview.h>
#include <extensionsystem/pluginview.h>
#include <extensionsystem/pluginmanager.h>

#include "ui_mainwindow.h"

#include <QSettings>
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

using namespace MainWin;
using namespace MainWin::Internal;
using namespace Trans::ConstantTranslations;

// Getting the Core instances
static inline Utils::UpdateChecker *updateChecker() { return Core::ICore::instance()->updateChecker(); }
static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }
static inline Core::CommandLine *commandLine() { return Core::ICore::instance()->commandLine(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ActionManager *actionManager() { return Core::ICore::instance()->actionManager(); }
static inline Core::IPatient *patient() { return Core::Internal::CoreImpl::instance()->patient(); }
static inline Core::FileManager *fileManager() { return Core::ICore::instance()->fileManager(); }
inline static DrugsDB::DrugsModel *drugModel() { return DrugsWidget::DrugsWidgetManager::instance()->currentDrugsModel(); }
static inline Core::IDocumentPrinter *printer() {return ExtensionSystem::PluginManager::instance()->getObject<Core::IDocumentPrinter>();}


// SplashScreen Messagers
static inline void messageSplash(const QString &s) {Core::ICore::instance()->messageSplashScreen(s); }
static inline void finishSplash(QMainWindow *w) {Core::ICore::instance()->finishSplashScreen(w); }


namespace MainWin {
namespace Internal {

class MainWinPrivate {
public:
    MainWinPrivate(MainWindow *parent) : q(parent)  {}
    ~MainWinPrivate() {}

    bool readExchangeFile()
    {
        QString exfile = commandLine()->value(Core::CommandLine::CL_ExchangeFile).toString();
        if (!exfile.isEmpty()) {
            messageSplash(q->tr("Reading exchange file..."));
            if (QFileInfo(exfile).isRelative())
                exfile.prepend(qApp->applicationDirPath() + QDir::separator());
            QString tmp;
            if (QFile(exfile).exists())
                tmp = Utils::readTextFile(exfile, Utils::DontWarnUser);
            //            Utils::Log::addMessage(this, "Content of the exchange file : " + tmp);
            if (tmp.contains(DrugsDB::Constants::ENCODEDHTML_FREEDIAMSTAG)) {
                int begin = tmp.indexOf(DrugsDB::Constants::ENCODEDHTML_FREEDIAMSTAG) + QString(DrugsDB::Constants::ENCODEDHTML_FREEDIAMSTAG).length();
                int end = tmp.indexOf("\"", begin);
                QString encoded = tmp.mid( begin, end - begin );
                DrugsDB::DrugsIO::instance()->prescriptionFromXml(drugModel(), QByteArray::fromBase64(encoded.toAscii()));
            } else if (tmp.contains("DrugsInteractionsEncodedPrescription:")) {
                /** \todo Manage wrong file encoding */
                int begin = tmp.indexOf("DrugsInteractionsEncodedPrescription:") + QString("DrugsInteractionsEncodedPrescription:").length();
                int end = tmp.indexOf("\"", begin);
                QString encoded = tmp.mid( begin, end - begin );
                DrugsDB::DrugsIO::instance()->prescriptionFromXml(drugModel(), QByteArray::fromBase64(encoded.toAscii()));
            } else if (tmp.startsWith("<?xml") && tmp.contains("<FreeDiams>", Qt::CaseInsensitive) && tmp.contains("</FreeDiams>", Qt::CaseInsensitive)) {
                /** \todo Read patients datas ? */
                DrugsDB::DrugsIO::instance()->prescriptionFromXml(drugModel(), tmp);
            } else {
                return false;
            }
        }
        return true;
    }
private:
    MainWindow *q;
};

static bool transmitDosage()
{
    Utils::Log::addMessage("Core", QCoreApplication::translate("MainWindow", "Preparing dosage transmission"));
    DrugsDB::DrugsIO::instance()->startsDosageTransmission();
    return true;
}

static const char* const  SETTINGS_COUNTDOWN = "transmissionCountDown";

} // namespace Internal
} // namespace Core

//--------------------------------------------------------------------------------------------------------
//--------------------------------------- Constructor / Destructor ---------------------------------------
//--------------------------------------------------------------------------------------------------------
MainWindow::MainWindow( QWidget * parent ) :
        Core::IMainWindow(parent),
        aClearPatient(0),
        m_TemplatesDock(0),
        d(new Internal::MainWinPrivate(this))
{
    setObjectName("MainWindow");
    setWindowIcon(theme()->icon(Core::Constants::ICONFREEDIAMS));
    messageSplash(tr("Creating Main Window"));
}

bool MainWindow::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);
    // create menus
    createFileMenu();
    createEditMenu();
    Core::ActionContainer *fmenu = actionManager()->actionContainer(Core::Constants::M_FILE);
    connect(fmenu->menu(), SIGNAL(aboutToShow()),this, SLOT(aboutToShowRecentFiles()));
    Core::ActionContainer *pmenu = actionManager()->actionContainer(Core::Constants::MENUBAR);
    pmenu->appendGroup(DrugsWidget::Constants::G_PLUGINS_MODES);
    pmenu->appendGroup(DrugsWidget::Constants::G_PLUGINS_SEARCH);
    pmenu->appendGroup(DrugsWidget::Constants::G_PLUGINS_DRUGS);
    pmenu->setTranslations(DrugsWidget::Constants::DRUGSMENU_TEXT);
    createTemplatesMenu();
    createConfigurationMenu();
    createHelpMenu();

    Core::MainWindowActions actions;
    actions.setFileActions(
            Core::MainWindowActions::A_FileNew  |
            Core::MainWindowActions::A_FileOpen |
            Core::MainWindowActions::A_FileSave |
            Core::MainWindowActions::A_FileSaveAs |
            Core::MainWindowActions::A_FilePrintPreview |
            Core::MainWindowActions::A_FileQuit);
    actions.setConfigurationActions(
            Core::MainWindowActions::A_AppPreferences |
            Core::MainWindowActions::A_LangageChange |
            Core::MainWindowActions::A_ConfigureMedinTux);
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

    // aClearPatient
    aClearPatient = new QAction(this);
    aClearPatient->setObjectName("aClearPatient");
    aClearPatient->setIcon(theme()->icon(Core::Constants::ICONCLEAR));
    Core::Command *cmd = actionManager()->registerAction(aClearPatient, "aClearPatient", QList<int>() << Core::Constants::C_GLOBAL_ID);
    cmd->setTranslations(QString("%1 %2").arg(Trans::Constants::CLEAR).arg(Trans::Constants::PATIENT));
    cmd->setDefaultKeySequence(QKeySequence(Qt::Key_C + Qt::CTRL + Qt::SHIFT));
    Core::ActionContainer *menu = actionManager()->actionContainer(Core::Constants::M_FILE);
    menu->addAction(cmd, Core::Constants::G_FILE_NEW);
    connect(aClearPatient, SIGNAL(triggered()), this, SLOT(clearPatientInfos()));

    return true;
}

void MainWindow::extensionsInitialized()
{
    // Update countdown to dosage transmission
    int count = settings()->value(Internal::SETTINGS_COUNTDOWN,0).toInt();
    ++count;
    if ((count==30) || (commandLine()->value(Core::CommandLine::CL_TransmitDosage).toBool())) {
        messageSplash(tr("Transmitting posologies..."));
        settings()->setValue(Internal::SETTINGS_COUNTDOWN,0);
        transmitDosage();
    } else {
        settings()->setValue(Internal::SETTINGS_COUNTDOWN,count);
    }

    // Disable some actions when starting as medintux plugin
    /** \todo this must disappear */
    if (commandLine()->value(Core::CommandLine::CL_MedinTux).toBool()) {
        this->aNew->setEnabled(false);
        this->aSave->setEnabled(false);
        this->aMedinTux->setEnabled(false);
    }
    /** end */

    // Creating MainWindow interface
    m_ui = new Internal::Ui::MainWindow();
    m_ui->setupUi(this);
    setWindowTitle(qApp->applicationName() + " - " + qApp->applicationVersion());

    // Manage patient datas
    m_ui->morePatientInfoButton->setIcon(theme()->icon(Core::Constants::ICONADD));
    m_ui->patientInformations->hide();
    m_ui->clearPatient->setIcon(theme()->icon(Core::Constants::ICONCLEAR));
    connect(m_ui->clearPatient, SIGNAL(clicked()), this, SLOT(clearPatientInfos()));

    messageSplash(tr("Initializing drugs database"));
    m_ui->m_CentralWidget->initialize();

    // SelectionOnly ?
    if (commandLine()->value(Core::CommandLine::CL_SelectionOnly).toBool()) {
        // Log mode
        Utils::Log::addMessage(this, tr("Entering selection mode"));
        // Unable some actions in menus
    //    aPrint->setEnabled(false);
    //    aPrintPreview->setEnabled(false);
        // Inform the widgets
        DrugsWidget::DrugsWidgetManager::instance()->setEditMode(DrugsWidget::DrugsWidgetManager::SelectOnly);
    }

    // If needed read exchange file
    if (!d->readExchangeFile()) {
        Utils::Log::addError(this, "Unable to read exchange file");
    }

    // Start the update checker
    if (updateChecker()->needsUpdateChecking(settings()->getQSettings())) {
        messageSplash(tkTr(Trans::Constants::CHECKING_UPDATES));
        Utils::Log::addMessage(this, tkTr(Trans::Constants::CHECKING_UPDATES));
        statusBar()->addWidget(new QLabel(tkTr(Trans::Constants::CHECKING_UPDATES), this));
        statusBar()->addWidget(updateChecker()->progressBar(this),1);
        connect(updateChecker(), SIGNAL(updateFound()), this, SLOT(updateFound()));
        connect(updateChecker(), SIGNAL(done(bool)), this, SLOT(updateCheckerEnd()));
        updateChecker()->check(Utils::Constants::FREEDIAMS_UPDATE_URL);
        settings()->setValue(Utils::Constants::S_LAST_CHECKUPDATE, QDate::currentDate());
    }

    // Block patient datas
    if (commandLine()->value(Core::CommandLine::CL_BlockPatientDatas).toBool()) {
        m_ui->patientName->setEnabled(false);
        m_ui->patientSurname->setEnabled(false);
        m_ui->dobDateEdit->setEnabled(false);
        m_ui->sexCombo->setEnabled(false);
        m_ui->creatinineUnit->setEnabled(false);
        m_ui->crClUnit->setEnabled(false);
        m_ui->patientWeight->setEnabled(false);
        m_ui->weightUnit->setEnabled(false);
        m_ui->sizeUnit->setEnabled(false);
        m_ui->patientSize->setEnabled(false);
        m_ui->patientClCr->setEnabled(false);
        m_ui->patientCreatinin->setEnabled(false);
        m_ui->patientClCr->setEnabled(false);
    }
    m_ui->listOfAllergies->setEnabled(false);

    createDockWindows();
    finishSplash(this);
    readSettings();
    show();
    raise();

    // Connect post core initialization
    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreInitialization()));
}

MainWindow::~MainWindow()
{
    // avoid a bug with contextManager updateContext
    delete m_TemplatesDock;
    m_TemplatesDock = 0;
}

void MainWindow::postCoreInitialization()
{
    contextManager()->updateContext();
    actionManager()->retranslateMenusAndActions();
    refreshPatient();
}


/**
  \brief Refresh the ui data refering to the patient
  \sa Core::Internal::CoreImpl::instance()->patient(), diPatient
*/
void MainWindow::refreshPatient()
{
    bool state = true;
    m_ui->centralwidget->blockSignals(state);
    m_ui->crClUnit->blockSignals(state);
    m_ui->creatinineUnit->blockSignals(state);
    m_ui->dobDateEdit->blockSignals(state);
    m_ui->listOfAllergies->blockSignals(state);
    m_ui->m_CentralWidget->blockSignals(state);
    m_ui->patientClCr->blockSignals(state);
    m_ui->patientCreatinin->blockSignals(state);
    m_ui->patientInformations->blockSignals(state);
    m_ui->patientName->blockSignals(state);
    m_ui->patientSize->blockSignals(state);
    m_ui->patientSurname->blockSignals(state);
    m_ui->patientWeight->blockSignals(state);
    m_ui->sexCombo->blockSignals(state);
    m_ui->sizeUnit->blockSignals(state);
    m_ui->weightUnit->blockSignals(state);

    m_ui->patientName->setText(patient()->value(Core::IPatient::BirthName).toString());
    m_ui->patientName->setToolTip( QString("Nom : %1 Prénom : %2<br />Date de naissance : %3<br />Poids : %4<br />"
                                     "Taille : %5<br />Clearance : %6")
                             .arg( patient()->value(Core::IPatient::BirthName).toString(),
                                   patient()->value(Core::IPatient::Surname).toString(),
                                   patient()->value(Core::IPatient::DateOfBirth).toString(),
                                   patient()->value(Core::IPatient::Weight).toString() )
                             .arg( patient()->value(Core::IPatient::Height).toString(),
                                   patient()->value(Core::IPatient::CreatinClearance).toString() ));
    m_ui->patientSurname->setText(patient()->value(Core::IPatient::Surname).toString());
    m_ui->dobDateEdit->setDate(patient()->value(Core::IPatient::DateOfBirth).toDate());
    m_ui->sexCombo->setCurrentIndex(m_ui->sexCombo->findText(patient()->value(Core::IPatient::Gender).toString(), Qt::MatchFixedString));

    m_ui->weightUnit->setCurrentIndex(m_ui->weightUnit->findText(patient()->value(Core::IPatient::WeightUnit).toString(), Qt::MatchFixedString));
    m_ui->sizeUnit->setCurrentIndex(m_ui->sizeUnit->findText(patient()->value(Core::IPatient::HeightUnit).toString(), Qt::MatchFixedString));
    m_ui->creatinineUnit->setCurrentIndex(m_ui->creatinineUnit->findText(patient()->value(Core::IPatient::CreatinineUnit).toString(), Qt::MatchFixedString));
    m_ui->crClUnit->setCurrentIndex(m_ui->crClUnit->findText(patient()->value(Core::IPatient::CreatinClearanceUnit).toString(), Qt::MatchFixedString));

    m_ui->patientWeight->setValue(patient()->value(Core::IPatient::Weight).toInt());
    m_ui->patientSize->setValue(patient()->value(Core::IPatient::Height).toInt());
    m_ui->patientClCr->setValue(patient()->value(Core::IPatient::CreatinClearance).toDouble());
    m_ui->patientCreatinin->setValue(patient()->value(Core::IPatient::Creatinine).toDouble());
    m_ui->patientClCr->setValue(patient()->value(Core::IPatient::CreatinClearance).toDouble());
    /** \todo manage allergies */
    QString a;
    const QStringList &drug = patient()->value(Core::IPatient::DrugsUidAllergies).toStringList();
    if (!drug.isEmpty())
        a += tr("Drugs(%1) // ").arg(drug.join(";"));
    const QStringList &inns = patient()->value(Core::IPatient::DrugsInnAllergies).toStringList();
    if (!inns.isEmpty())
        a += tr("INN(%1) // ").arg(inns.join(";"));
    const QStringList &atc = patient()->value(Core::IPatient::DrugsAtcAllergies).toStringList();
    if (!atc.isEmpty())
        a += tr("ATC(%1) // ").arg(atc.join(";"));
    a.chop(4);
    m_ui->listOfAllergies->setText(a);

    state = false;
    m_ui->centralwidget->blockSignals(state);
    m_ui->crClUnit->blockSignals(state);
    m_ui->creatinineUnit->blockSignals(state);
    m_ui->dobDateEdit->blockSignals(state);
    m_ui->listOfAllergies->blockSignals(state);
    m_ui->m_CentralWidget->blockSignals(state);
    m_ui->patientClCr->blockSignals(state);
    m_ui->patientCreatinin->blockSignals(state);
    m_ui->patientInformations->blockSignals(state);
    m_ui->patientName->blockSignals(state);
    m_ui->patientSize->blockSignals(state);
    m_ui->patientSurname->blockSignals(state);
    m_ui->patientWeight->blockSignals(state);
    m_ui->sexCombo->blockSignals(state);
    m_ui->sizeUnit->blockSignals(state);
    m_ui->weightUnit->blockSignals(state);
}

/**
  \brief Close the main window and the application
  \todo Add  ICoreListener
*/
void MainWindow::closeEvent( QCloseEvent *event )
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

    // Save exchange file
    QString exfile = commandLine()->value(Core::CommandLine::CL_ExchangeFile).toString();
    if (!exfile.isEmpty()) {
        Utils::Log::addMessage(this, QString("Exchange File : %1 - %2")
                               .arg(exfile)
                               .arg(commandLine()->value(Core::CommandLine::CL_EMR_Name).toString()));
        QString format = commandLine()->value(Core::CommandLine::CL_ExchangeFileFormat).toString();
        QString tmp;

        // create PDF cache printed docs XML lines
        const QList<Core::PrintedDocumentTracer> &pdfs = printer()->printedDocs();
        QString extraDatas = patient()->toXml();
        foreach(const Core::PrintedDocumentTracer &t, pdfs) {
            extraDatas += QString("  <Printed file=\"%1\" docName=\"%2\" dateTime=\"%3\" userUid=\"%4\"/>\n")
                           .arg(t.fileName())
                           .arg(t.documentName())
                           .arg(t.dateTime().toString(Qt::ISODate))
                           .arg(t.userUid());
        }

        // Manage specific MedinTux output exchange file format
        if (commandLine()->value(Core::CommandLine::CL_MedinTux).toBool() ||
            commandLine()->value(Core::CommandLine::CL_EMR_Name).toString().compare("medintux",Qt::CaseInsensitive) == 0) {
            if (format=="html_xml" || format=="html") {
                tmp = DrugsDB::DrugsIO::instance()->prescriptionToHtml(drugModel(), extraDatas, DrugsDB::DrugsIO::MedinTuxVersion);
                tmp.replace("font-weight:bold;", "font-weight:600;");
                Utils::saveStringToFile(Utils::toHtmlAccent(tmp), exfile, Utils::Overwrite, Utils::DontWarnUser);
            } else if (format=="xml") {
                savePrescription(exfile);
            }
        } else {
            if (format=="html_xml" || format=="html") {
                tmp = DrugsDB::DrugsIO::instance()->prescriptionToHtml(drugModel(), extraDatas, DrugsDB::DrugsIO::MedinTuxVersion);
                Utils::saveStringToFile(Utils::toHtmlAccent(tmp), exfile, Utils::Overwrite, Utils::DontWarnUser);
            } else if (format=="xml") {
                savePrescription(exfile);
            }
        }
    }

    Core::ICore::instance()->coreIsAboutToClose();
    writeSettings();
    event->accept();
}

/** \brief Manages language changes : retranslate Ui and ActionManager. */
void MainWindow::changeEvent(QEvent *event)
{
    if (event->type()==QEvent::LanguageChange) {
        QVariant sex = patient()->value(Core::IPatient::Gender);
        bool state = true;
        m_ui->centralwidget->blockSignals(state);
        m_ui->crClUnit->blockSignals(state);
        m_ui->creatinineUnit->blockSignals(state);
        m_ui->dobDateEdit->blockSignals(state);
        m_ui->listOfAllergies->blockSignals(state);
        m_ui->m_CentralWidget->blockSignals(state);
        m_ui->patientClCr->blockSignals(state);
        m_ui->patientCreatinin->blockSignals(state);
        m_ui->patientInformations->blockSignals(state);
        m_ui->patientName->blockSignals(state);
        m_ui->patientSize->blockSignals(state);
        m_ui->patientSurname->blockSignals(state);
        m_ui->patientWeight->blockSignals(state);
        m_ui->sexCombo->blockSignals(state);
        m_ui->sizeUnit->blockSignals(state);
        m_ui->weightUnit->blockSignals(state);

	m_ui->retranslateUi(this);

        state = false;
        m_ui->centralwidget->blockSignals(state);
        m_ui->crClUnit->blockSignals(state);
        m_ui->creatinineUnit->blockSignals(state);
        m_ui->dobDateEdit->blockSignals(state);
        m_ui->listOfAllergies->blockSignals(state);
        m_ui->m_CentralWidget->blockSignals(state);
        m_ui->patientClCr->blockSignals(state);
        m_ui->patientCreatinin->blockSignals(state);
        m_ui->patientInformations->blockSignals(state);
        m_ui->patientName->blockSignals(state);
        m_ui->patientSize->blockSignals(state);
        m_ui->patientSurname->blockSignals(state);
        m_ui->patientWeight->blockSignals(state);
        m_ui->sexCombo->blockSignals(state);
        m_ui->sizeUnit->blockSignals(state);
        m_ui->weightUnit->blockSignals(state);

        actionManager()->retranslateMenusAndActions();
        patient()->setValue(Core::IPatient::Gender, sex);
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

void MainWindow::clearPatientInfos()
{
    if (commandLine()->value(Core::CommandLine::CL_BlockPatientDatas).toBool())
        return;
    patient()->clear();
    refreshPatient();
}

void MainWindow::updateCheckerEnd()
{
    // this code avoid deletion of the resizer corner of the mainwindow
    delete statusBar();
    statusBar()->hide();
}

/** \brief Reads main window's settings */
void MainWindow::readSettings()
{
    settings()->restoreState(this, DrugsWidget::Constants::S_STATEPREFIX);
    fileManager()->getRecentFilesFromSettings();
}

/** \brief Writes main window's settings */
void MainWindow::writeSettings()
{
    settings()->saveState(this, DrugsWidget::Constants::S_STATEPREFIX);
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
    if (drugModel()->drugsList().count()) {
        bool yes = Utils::yesNoMessageBox(
                tr("Save actual prescription ?"),
                tr("The actual prescription is not empty. Do you want to save it before creating a new one ?"));
        if (yes) {
            saveFile();
        }
    }
    patient()->clear();
    refreshPatient();
    drugModel()->clearDrugsList();
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
    m_ui->m_CentralWidget->changeFontTo(font);
    m_ui->patientName->setFont(font);
}


/**
  \brief Prints the prescription using the header, footer and watermark.
  \sa tkPrinter
*/
bool MainWindow::print()
{
    return m_ui->m_CentralWidget->printPrescription();
}

bool MainWindow::printPreview()
{
    m_ui->m_CentralWidget->printPreview();
    return true;
}

/** \brief Runs the MedinTux configurator */
bool MainWindow::configureMedintux()
{
    Internal::configureMedinTux();
    return true;
}

bool MainWindow::saveAsFile()
{
    return savePrescription();
}

bool MainWindow::saveFile()
{
    return savePrescription();
}

/**
  \brief Saves a prescription. If fileName is empty, user is ask about a file name.
  \sa openPrescription()
  \sa DrugsIO
*/
bool MainWindow::savePrescription(const QString &fileName)
{
    const QList<Core::PrintedDocumentTracer> &pdfs = printer()->printedDocs();
    QString extraDatas = patient()->toXml();
    foreach(const Core::PrintedDocumentTracer &t, pdfs) {
        extraDatas += QString("<Printed file=\"%1\" docName=\"%2\" dateTime=\"%3\" userUid=\"%4\"/>\n")
                       .arg(t.fileName())
                       .arg(t.documentName())
                       .arg(t.dateTime().toString(Qt::ISODate))
                       .arg(t.userUid());
    }
    if (commandLine()->value(Core::CommandLine::CL_EMR_Name).isValid()) {
        extraDatas.append(QString("<EMR name=\"%1\"").arg(commandLine()->value(Core::CommandLine::CL_EMR_Name).toString()));
        if (commandLine()->value(Core::CommandLine::CL_EMR_Name).isValid()) {
            extraDatas.append(QString(" uid=\"%1\"").arg(commandLine()->value(Core::CommandLine::CL_EMR_Uid).toString()));
        }
        extraDatas.append("/>");
    }
//    qWarning() << extraDatas;
    return DrugsDB::DrugsIO::savePrescription(drugModel(), extraDatas, fileName);
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
                                             tkTr(Core::Constants::FREEDIAMS_FILEFILTER) );
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
    QString datas;
    if (drugModel()->rowCount() > 0) {
        int r = Utils::withButtonsMessageBox(
                tr("Opening a prescription : merge or replace ?"),
                tr("There is a prescription inside editor, do you to replace it or to add the opened prescription ?"),
                QString(), QStringList() << tr("Replace prescription") << tr("Add to prescription"),
                tr("Open a prescription") + " - " + qApp->applicationName());
        if (r == 0) {
            DrugsDB::DrugsIO::loadPrescription(drugModel(), file, datas, DrugsDB::DrugsIO::ReplacePrescription);
        } else if (r==1) {
            DrugsDB::DrugsIO::loadPrescription(drugModel(), file, datas, DrugsDB::DrugsIO::AppendPrescription);
        }
    } else {
        DrugsDB::DrugsIO::loadPrescription(drugModel(), file, datas, DrugsDB::DrugsIO::ReplacePrescription);
    }
//    qWarning() << datas;
    patient()->fromXml(datas);
    refreshPatient();
}

void MainWindow::createDockWindows()
{
    QDockWidget *dock = m_TemplatesDock = new QDockWidget(tkTr(Trans::Constants::TEMPLATES), this);
    dock->setObjectName("templatesDock");
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dock->setWidget(new Templates::TemplatesView(dock));
    addDockWidget(Qt::RightDockWidgetArea, dock);
    QMenu *menu = actionManager()->actionContainer(Core::Constants::M_TEMPLATES)->menu();
    menu->addAction(dock->toggleViewAction());
}


/** \brief Always keep uptodate patient's datas */
void MainWindow::on_patientName_textChanged(const QString &text)
{
    patient()->setValue(Core::IPatient::BirthName, text);
}

/** \brief Always keep uptodate patient's datas */
void MainWindow::on_patientSurname_textChanged(const QString &text)
{
    patient()->setValue(Core::IPatient::Surname, text);
}

/** \brief Always keep uptodate patient's datas */
void MainWindow::on_sexCombo_currentIndexChanged(const QString &text)
{
    patient()->setValue(Core::IPatient::Gender, text);
    refreshPatient();
}

/** \brief Always keep uptodate patient's datas */
void MainWindow::on_patientWeight_valueChanged(const QString &text)
{
    patient()->setValue(Core::IPatient::Weight, text);
    refreshPatient();
}

/** \brief Always keep uptodate patient's datas */
void MainWindow::on_patientSize_valueChanged(const QString & text)
{
    patient()->setValue(Core::IPatient::Height, text);
    refreshPatient();
}

/** \brief Always keep uptodate patient's datas */
void MainWindow::on_patientClCr_valueChanged(const QString & text)
{
    patient()->setValue(Core::IPatient::CreatinClearance, text);
    refreshPatient();
}

/** \brief Always keep uptodate patient's datas */
void MainWindow::on_patientCreatinin_valueChanged(const QString & text)
{
    patient()->setValue(Core::IPatient::Creatinine, text);
    refreshPatient();
}

/** \brief Always keep uptodate patient's datas */
void MainWindow::on_listOfAllergies_textChanged(const QString &text)
{
    /** \todo manage allergies */
    patient()->setValue(Core::IPatient::DrugsAtcAllergies, text);
    refreshPatient();
}
