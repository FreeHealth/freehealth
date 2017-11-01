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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
#include "mainwindow.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants.h>
#include <coreplugin/translators.h>
#include <coreplugin/itheme.h>
#include <coreplugin/filemanager.h>

#include <coreplugin/actionmanager/mainwindowactions.h>
#include <coreplugin/actionmanager/mainwindowactionhandler.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/dialogs/plugindialog.h>
#include <coreplugin/dialogs/helpdialog.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/idocumentprinter.h>
#include <coreplugin/coreimpl.h>
#include <coreplugin/commandlineparser.h>

#include <mainwindowplugin/medintux.h>

#include <drugsplugin/constants.h>
#include <drugsplugin/drugswidgetmanager.h>

#include <drugsbaseplugin/drugbasecore.h>
#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/constants.h>
#include <drugsbaseplugin/constants_html.h>
#include <drugsbaseplugin/drugsio.h>
#include <drugsbaseplugin/globaldrugsmodel.h>
#include <drugsbaseplugin/drugsmodel.h>
#include <drugsbaseplugin/idrugengine.h>
#include <drugsbaseplugin/prescriptionprinter.h>

//#include <drugsbaseplugin/interactionmanager.h>
//#include <drugsbaseplugin/engines/allergyengine.h>

#include <templatesplugin/templatesview.h>

#include <extensionsystem/pluginerrorview.h>
#include <extensionsystem/pluginview.h>
#include <extensionsystem/pluginmanager.h>

#include <translationutils/constanttranslations.h>
#include <utils/log.h>
#include <utils/global.h>
#include <utils/updatechecker.h>
#include <utils/iconbadgealert.h>
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
#include <QDataWidgetMapper>
#include <QStandardItemModel>
#include <QMenu>

using namespace MainWin;
using namespace MainWin::Internal;
using namespace Trans::ConstantTranslations;

// Getting the Core instances
static inline Utils::UpdateChecker *updateChecker() { return Core::ICore::instance()->updateChecker(); }
static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }
static inline Core::ICommandLine *commandLine() { return Core::ICore::instance()->commandLine(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ActionManager *actionManager() { return Core::ICore::instance()->actionManager(); }
static inline Core::IPatient *patient() { return Core::Internal::CoreImpl::instance()->patient(); }
static inline Core::FileManager *fileManager() { return Core::ICore::instance()->fileManager(); }
static inline DrugsDB::DrugsModel *drugModel() { return DrugsWidget::DrugsWidgetManager::instance()->currentDrugsModel(); }
static inline Core::IDocumentPrinter *printer() {return ExtensionSystem::PluginManager::instance()->getObject<Core::IDocumentPrinter>();}
static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}
static inline DrugsDB::DrugsBase &drugsBase() {return DrugsDB::DrugBaseCore::instance().drugsBase();}
static inline DrugsDB::DrugsIO &drugsIo() {return DrugsDB::DrugBaseCore::instance().drugsIo();}
static inline DrugsDB::PrescriptionPrinter &prescriptionPrinter() {return DrugsDB::DrugBaseCore::instance().prescriptionPrinter();}

// SplashScreen Messagers
static inline void messageSplash(const QString &s) {theme()->messageSplashScreen(s); }
static inline void finishSplash(QMainWindow *w) {theme()->finishSplashScreen(w); }


namespace MainWin {
namespace Internal {

class MainWinPrivate {
public:
    MainWinPrivate(MainWindow *parent) :
        m_Mapper(0), m_AllergiesModel(0), m_AllergiesView(0), m_PrecautionView(0),
        m_TemplatesDock(0),
//        m_PrecautionsDock(0),
        m_NameOrder(-1),
        q(parent)
    {}

    ~MainWinPrivate()
    {
        if (m_Mapper)
            delete m_Mapper;
        m_Mapper = 0;
        if (m_AllergiesModel)
            delete m_AllergiesModel;
        m_AllergiesModel = 0;
        if (m_AllergiesView)
            delete m_AllergiesView;
        m_AllergiesView = 0;
    }

    bool readExchangeFile(const QString &msg)
    {
        QString exfile = commandLine()->value(Core::CommandLine::CL_ExchangeOutFile).toString();
        LOG_FOR(q, "Reading exchange out prescription file: " + exfile);
        if (!exfile.isEmpty()) {
            messageSplash(msg);
            if (QFileInfo(exfile).isRelative())
                exfile.prepend(qApp->applicationDirPath() + QDir::separator());
            QString tmp;
            exfile = QDir::cleanPath(exfile);
            if (QFile(exfile).exists())
                tmp = Utils::readTextFile(exfile, Utils::DontWarnUser);
            //            LOG("Content of the exchange file : " + tmp);
            if (tmp.contains(DrugsDB::Constants::ENCODEDHTML_FREEDIAMSTAG)) {
                int begin = tmp.indexOf(DrugsDB::Constants::ENCODEDHTML_FREEDIAMSTAG) + QString(DrugsDB::Constants::ENCODEDHTML_FREEDIAMSTAG).length();
                int end = tmp.indexOf("\"", begin);
                QString encoded = tmp.mid( begin, end - begin );
                drugsIo().prescriptionFromXml(drugModel(), QByteArray::fromBase64(encoded.toAscii()));
            } else if (tmp.contains("DrugsInteractionsEncodedPrescription:")) {
                // TODO: Manage wrong file encoding
                int begin = tmp.indexOf("DrugsInteractionsEncodedPrescription:") + QString("DrugsInteractionsEncodedPrescription:").length();
                int end = tmp.indexOf("\"", begin);
                QString encoded = tmp.mid( begin, end - begin );
                drugsIo().prescriptionFromXml(drugModel(), QByteArray::fromBase64(encoded.toAscii()));
            } else if (tmp.startsWith("<?xml") && tmp.contains("<FreeDiams>", Qt::CaseInsensitive) && tmp.contains("</FreeDiams>", Qt::CaseInsensitive)) {
                // TODO: Read patients datas ?
                drugsIo().prescriptionFromXml(drugModel(), tmp);
            } else {
                return false;
            }
        }
        return true;
    }

    void createMapper()
    {
        if (!m_Mapper)
            m_Mapper = new QDataWidgetMapper(q);
        m_Mapper->setModel(patient());
        m_Mapper->addMapping(q->m_ui->patientName, Core::IPatient::UsualName);
        m_Mapper->addMapping(q->m_ui->patientFirstname, Core::IPatient::Firstname);
        // TODO: manage weight and height
//        m_Mapper->addMapping(q->m_ui->patientSize, Core::IPatient::Height);
//        m_Mapper->addMapping(q->m_ui->sizeUnit, Core::IPatient::HeightUnit);
//        m_Mapper->addMapping(q->m_ui->patientWeight, Core::IPatient::Weight);
//        m_Mapper->addMapping(q->m_ui->weightUnit, Core::IPatient::WeightUnit);
        m_Mapper->addMapping(q->m_ui->sexCombo, Core::IPatient::GenderIndex);
        m_Mapper->addMapping(q->m_ui->patientCreatinin, Core::IPatient::Creatinine);
        m_Mapper->addMapping(q->m_ui->creatinineUnit, Core::IPatient::CreatinineUnit);
        m_Mapper->addMapping(q->m_ui->patientClCr, Core::IPatient::CreatinClearance);
        m_Mapper->addMapping(q->m_ui->crClUnit, Core::IPatient::CreatinClearanceUnit);
        m_Mapper->addMapping(q->m_ui->dobDateEdit, Core::IPatient::DateOfBirth);
        m_Mapper->toFirst();
    }

    void createPrecautionsModelAndView(QTreeView *treeview, QComboBox *combo)
    {
        QList<DrugsDB::IDrugEngine*> engines = pluginManager()->getObjects<DrugsDB::IDrugEngine>();
        if (engines.isEmpty()) {
            LOG_ERROR_FOR("MainWindow", "No allergy engine");
            return;
        }
        DrugsDB::IDrugEngine *engine = 0;
        foreach(DrugsDB::IDrugEngine *e, engines) {
            if (e->isCalculatingPatientDrugAllergiesAndIntolerances()) {
                engine = e;
                break;
            }
        }
        if (!engine) {
            combo->hide();
            return;
        }
        QAbstractItemModel *model = engine->precautionModel();
        if (!model) {
            combo->hide();
            return;
        }
        if (!treeview) {
            treeview = new QTreeView(q);
            combo->setModel(model);
            combo->setView(treeview);
            treeview->header()->hide();
            treeview->expandAll();
            treeview->resizeColumnToContents(0);
            treeview->setIndentation(10);
            treeview->setFrameStyle(QFrame::NoFrame);
            treeview->setAlternatingRowColors(true);
        }
    }

    QString getXmlExtraData()
    {
        QString extraData = patient()->toXml();
        Q_ASSERT(printer());
        if (printer()) {
            const QList<Core::PrintedDocumentTracer> &pdfs = printer()->printedDocs();
            foreach(const Core::PrintedDocumentTracer &t, pdfs) {
                extraData += QString("<Printed file=\"%1\" docName=\"%2\" dateTime=\"%3\" userUid=\"%4\"/>\n")
                        .arg(t.fileName())
                        .arg(t.documentName())
                        .arg(t.dateTime().toString(Qt::ISODate))
                        .arg(t.userUid());
            }
        }
        // Add EMR informations
        if (commandLine()->value(Core::CommandLine::CL_EMR_Name).isValid()) {
            extraData.append(QString("<EMR name=\"%1\"").arg(commandLine()->value(Core::CommandLine::CL_EMR_Name).toString()));
            if (commandLine()->value(Core::CommandLine::CL_EMR_Name).isValid()) {
                extraData.append(QString(" uid=\"%1\"").arg(commandLine()->value(Core::CommandLine::CL_EMR_Uid).toString()));
            }
            extraData.append("/>");
        }
        return extraData;
    }

public:
    QDataWidgetMapper *m_Mapper;
    QStandardItemModel *m_AllergiesModel;
    QTreeView *m_AllergiesView;
    QTreeView *m_PrecautionView;
    QDockWidget *m_TemplatesDock; //, *m_PrecautionsDock;
    int m_NameOrder;

private:
    MainWindow *q;
};

static bool transmitDosage()
{
    if (Utils::isReleaseCompilation()) {
        LOG_FOR("Core", QCoreApplication::translate("MainWindow", "Preparing dosage transmission"));
        // TODO: code here
//        DrugsDB::DrugsIO::instance()->startsDosageTransmission();
    }
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
        aClearPatient(new QAction(this)),
        m_ui(0),
        d(new Internal::MainWinPrivate(this))
{
    setObjectName("MainWindow");
    setWindowIcon(theme()->icon(Core::Constants::ICONFREEDIAMS));
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
    fmenu->setOnAllDisabledBehavior(Core::ActionContainer::Show);
    connect(fmenu->menu(), SIGNAL(aboutToShow()),this, SLOT(aboutToShowRecentFiles()));

    createEditMenu();

    // Create prescription menu
    Core::ActionContainer *pmenu = actionManager()->actionContainer(DrugsWidget::Constants::M_PLUGINS_DRUGS);
    if (!pmenu) {
        using namespace DrugsWidget::Constants;
        pmenu = actionManager()->createMenu(M_PLUGINS_DRUGS);
        pmenu->appendGroup(Core::Id(G_PLUGINS_VIEWS));
        pmenu->appendGroup(Core::Id(G_PLUGINS_MODES));
        pmenu->appendGroup(Core::Id(G_PLUGINS_SEARCH));
        pmenu->appendGroup(Core::Id(G_PLUGINS_DRUGS));
        pmenu->appendGroup(Core::Id(G_PLUGINS_INTERACTIONS));
        pmenu->appendGroup(Core::Id(Core::Constants::G_PLUGINS_PADTOOLS));
        pmenu->setTranslations(DRUGSMENU_TEXT, DRUGCONSTANTS_TR_CONTEXT);
    }
    Q_ASSERT(pmenu);
    actionManager()->actionContainer(Core::Id(Core::Constants::MENUBAR))->addMenu(pmenu, Core::Id(Core::Constants::G_PLUGINS));

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
            Core::MainWindowActions::A_LanguageChange |
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
    aClearPatient->setObjectName("aClearPatient");
    aClearPatient->setIcon(theme()->icon(Core::Constants::ICONCLEAR));
    Core::Command *cmd = actionManager()->registerAction(aClearPatient, Core::Id("aClearPatient"), Core::Context(Core::Constants::C_GLOBAL));
    cmd->setTranslations(tkTr(Trans::Constants::CLEAR_PATIENT_INFOS));
    cmd->setDefaultKeySequence(QKeySequence(Qt::Key_C + Qt::CTRL + Qt::SHIFT));
    Core::ActionContainer *menu = actionManager()->actionContainer(Core::Id(Core::Constants::M_FILE));
    menu->addAction(cmd, Core::Id(Core::Constants::G_FILE_NEW));
    connect(aClearPatient, SIGNAL(triggered()), this, SLOT(clearPatientInfos()));

    return true;
}

/** \brief Before MainWindow is shown.
  * Update the setting app countdown to transmition
  * Disable Non-MedinTux actions
  * Create Ui
  * Initialize DrugsWidgetManager as centralView
  * Change Edition mode
  * Read exchange file
  * Start the update checker
  * Create docks
  * Finish splashscreen
  * Connect to signals
*/
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
    // TODO: this must disappear
    if (commandLine()->value(Core::CommandLine::CL_MedinTux).toBool()) {
        this->aNew->setEnabled(false);
        this->aSave->setEnabled(false);
        this->aMedinTux->setEnabled(false);
    }
    /** end */

    // Creating MainWindow interface
    m_ui = new Ui::MainWindow;
    m_ui->setupUi(this);
    m_ui->centralLayout->setMargin(5);
    m_ui->sexCombo->addItems(genders());

    if (commandLine()->value(Core::CommandLine::CL_EMR_Name).isValid())
        setWindowTitle(QString("%1 - %2 [%3]")
                       .arg(qApp->applicationName())
                       .arg(qApp->applicationVersion())
                       .arg(tr("Connected to %1")
                            .arg(commandLine()->value(Core::CommandLine::CL_EMR_Name).toString())));
    else
        setWindowTitle(QString("%1 - %2")
                       .arg(qApp->applicationName())
                       .arg(qApp->applicationVersion()));

    // Manage patient datas
    m_ui->morePatientInfoButton->setIcon(theme()->icon(Core::Constants::ICONADD));
    m_ui->morePatientInfoButton->setToolTip(tkTr(Trans::Constants::TOGGLE_PATIENT_INFORMATION));
    m_ui->patientInformation->hide();
    m_ui->clearPatient->setIcon(theme()->icon(Core::Constants::ICONCLEAR));
    m_ui->clearPatient->setToolTip(tkTr(Trans::Constants::CLEAR_PATIENT_INFOS));
    connect(m_ui->clearPatient, SIGNAL(clicked()), this, SLOT(clearPatientInfos()));

    messageSplash(tr("Initializing drugs database"));
    m_ui->m_CentralWidget->initialize();

    // SelectionOnly ?
    if (commandLine()->value(Core::CommandLine::CL_SelectionOnly).toBool()) {
        // Log mode
        LOG(tr("Entering selection mode"));
        // Unable some actions in menus
    //    aPrint->setEnabled(false);
    //    aPrintPreview->setEnabled(false);
        // Inform the widgets
        DrugsWidget::DrugsWidgetManager::instance()->setEditMode(DrugsWidget::DrugsWidgetManager::SelectOnly);
    }

    // If needed read exchange file
    if (!d->readExchangeFile(tr("Reading exchange file..."))) {
        LOG_ERROR(tkTr(Trans::Constants::FILE_1_ISNOT_READABLE)
                  .arg(commandLine()->value(Core::CommandLine::CL_ExchangeOutFile).toString()));
    }

    // Start the update checker
    if (updateChecker()->needsUpdateChecking(settings()->getQSettings())) {
        settings()->setPath(Core::ISettings::UpdateUrl, Utils::Constants::FREEDIAMS_UPDATE_URL);
        if (checkUpdate())
            settings()->setValue(Utils::Constants::S_LAST_CHECKUPDATE, QDate::currentDate());
    }

    // Block patient datas
    if (commandLine()->value(Core::CommandLine::CL_BlockPatientDatas).toBool()) {
        m_ui->patientName->setEnabled(false);
        m_ui->patientFirstname->setEnabled(false);
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
        aClearPatient->setEnabled(false);
        m_ui->clearPatient->setEnabled(false);
    }

    // Connect post core initialization
    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreInitialization()));
    connect(&drugsBase(), SIGNAL(drugsBaseHasChanged()), this, SLOT(refreshPatient()));
    connect(drugModel(), SIGNAL(numberOfRowsChanged()), this, SLOT(updateIconBadgeOnMacOs()));
}

MainWindow::~MainWindow()
{
    // delete all dockwigdet by hand
    if (d->m_TemplatesDock) {
        delete d->m_TemplatesDock;
        d->m_TemplatesDock = 0;
    }
//    if (d->m_PrecautionsDock) {
//        delete d->m_PrecautionsDock;
//        d->m_PrecautionsDock = 0;
//    }
    if (d)
        delete d;
    d = 0;
    delete m_ui;
}

void MainWindow::postCoreInitialization()
{
    createDockWindows();
    readSettings();

    show();
    raise();

    switchToCurrentUserLanguage();
    contextManager()->updateContext();
    actionManager()->retranslateMenusAndActions();
    refreshPatient();
    updateIconBadgeOnMacOs();
    finishSplash(this);
}

/**
  \brief Refresh the ui data refering to the patient
  \sa Core::Internal::CoreImpl::instance()->patient(), diPatient
*/
void MainWindow::refreshPatient()
{
    if (d->m_Mapper)
        d->m_Mapper->setCurrentIndex(0);
    else
        d->createMapper();
    d->createPrecautionsModelAndView(d->m_AllergiesView, m_ui->drugsPrecautions);
    if (d->m_PrecautionView)
        d->m_PrecautionView->expandAll();
}

/** \brief Slot specific to MacOs -> update the application icon badge to number of founded interactions. */
void MainWindow::updateIconBadgeOnMacOs()
{
#ifdef Q_OS_MAC
    // TODO: code here : move this in the interactionmanager; result or alert  ?
//    int n = drugModel()->interactionsManager()->getAllInteractionsFound().count();
//    if (n > 0) {
//        Utils::MacOs::setIconBadgeLabel(QString::number(n));
//    }
#endif
}

/** \brief Change the order of the patient's names in the view */
void MainWindow::changePatientNameLabelOrder(const int first)
{
    if (first == -1)
        return;
    if (d->m_NameOrder==-1 && first==PatientSurnameLabel)
        return;
    if (first == d->m_NameOrder)
        return;
    if (first >= PatientNameMax)
        return;
    Q_ASSERT(m_ui);
    if (!m_ui)
        return;
    d->m_NameOrder = first;
    QHBoxLayout *lay = m_ui->patientNamesLayout;
    lay->removeWidget(m_ui->surnamesLabel);
    lay->removeWidget(m_ui->patientName);
    lay->removeWidget(m_ui->firstNamesLabel);
    lay->removeWidget(m_ui->patientFirstname);
    if (first==PatientSurnameLabel) {
        lay->addWidget(m_ui->surnamesLabel);
        lay->addWidget(m_ui->patientName);
        lay->addWidget(m_ui->firstNamesLabel);
        lay->addWidget(m_ui->patientFirstname);
    } else {
        lay->addWidget(m_ui->firstNamesLabel);
        lay->addWidget(m_ui->patientFirstname);
        lay->addWidget(m_ui->surnamesLabel);
        lay->addWidget(m_ui->patientName);
    }
}

/**
  \brief Close the main window and the application
  \todo Add  ICoreListener
*/
void MainWindow::closeEvent(QCloseEvent *event)
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
    QString exfile = commandLine()->value(Core::CommandLine::CL_ExchangeOutFile).toString();
    if (!exfile.isEmpty()) {
        LOG(QString("Exchange File : %1 - %2")
            .arg(exfile)
            .arg(commandLine()->value(Core::CommandLine::CL_EMR_Name).toString()));
        QString format = commandLine()->value(Core::CommandLine::CL_ExchangeOutFileFormat).toString();
        QString tmp;

        QString extraData = d->getXmlExtraData();

        // Manage specific MedinTux output exchange file format
        if (commandLine()->value(Core::CommandLine::CL_MedinTux).toBool() ||
            commandLine()->value(Core::CommandLine::CL_EMR_Name).toString().compare("medintux", Qt::CaseInsensitive) == 0) {
            if (format=="html_xml" || format=="html") {
                DrugsDB::PrescriptionPrinterJob job;
                job.readSettings();
                job.addXmlExtraData(extraData);
                job.setDrugsModel(drugModel());
                tmp = prescriptionPrinter().prescriptionToHtml(job);
                tmp.replace("font-weight:bold;", "font-weight:600;");
                Utils::saveStringToFile(tmp, exfile, Utils::Overwrite, Utils::DontWarnUser);
            } else if (format=="xml") {
                savePrescription(exfile);
            }
        } else {
            if (format=="html_xml" || format=="html") {
                DrugsDB::PrescriptionPrinterJob job;
                job.readSettings();
                job.addXmlExtraData(extraData);
                job.setDrugsModel(drugModel());
                tmp = prescriptionPrinter().prescriptionToHtml(job);
                Utils::saveStringToFile(tmp, exfile, Utils::Overwrite, Utils::DontWarnUser);
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
        if (m_ui) {
            QVariant sex = patient()->data(Core::IPatient::Gender);
            bool state = true;
            m_ui->centralwidget->blockSignals(state);
            m_ui->crClUnit->blockSignals(state);
            m_ui->creatinineUnit->blockSignals(state);
            m_ui->dobDateEdit->blockSignals(state);
            m_ui->m_CentralWidget->blockSignals(state);
            m_ui->patientClCr->blockSignals(state);
            m_ui->patientCreatinin->blockSignals(state);
            m_ui->patientInformation->blockSignals(state);
            m_ui->patientName->blockSignals(state);
            m_ui->patientSize->blockSignals(state);
            m_ui->patientFirstname->blockSignals(state);
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
            m_ui->m_CentralWidget->blockSignals(state);
            m_ui->patientClCr->blockSignals(state);
            m_ui->patientCreatinin->blockSignals(state);
            m_ui->patientInformation->blockSignals(state);
            m_ui->patientName->blockSignals(state);
            m_ui->patientSize->blockSignals(state);
            m_ui->patientFirstname->blockSignals(state);
            m_ui->patientWeight->blockSignals(state);
            m_ui->sexCombo->blockSignals(state);
            m_ui->sizeUnit->blockSignals(state);
            m_ui->weightUnit->blockSignals(state);
            actionManager()->retranslateMenusAndActions();
            patient()->setData(patient()->index(0, Core::IPatient::Gender), sex);
            refreshPatient();
        }
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
    aci->clear();

    foreach (const QString &fileName, fileManager()->recentFiles()) {
        QAction *action = aci->menu()->addAction(fileName);
        action->setData(fileName);
        connect(action, SIGNAL(triggered()), this, SLOT(openRecentFile()));
    }
    aci->menu()->setEnabled(fileManager()->recentFiles().count() > 0);
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

void MainWindow::clearPatientInfos()
{
    if (commandLine()->value(Core::CommandLine::CL_BlockPatientDatas).toBool())
        return;
    patient()->clear();
    drugModel()->clearDrugsList();
    refreshPatient();
}

void MainWindow::togglePrecautions()
{
}

void MainWindow::updateCheckerEnd(bool error)
{
    Q_UNUSED(error);
    // this code avoid deletion of the resizer corner of the mainwindow
    delete statusBar();
    statusBar()->hide();
}

/** \brief Reads main window's settings */
void MainWindow::readSettings()
{
    settings()->restoreState(this, DrugsWidget::Constants::S_STATEPREFIX);
    fileManager()->getRecentFilesFromSettings();
    changePatientNameLabelOrder(settings()->value(DrugsWidget::Constants::S_PATIENTNAMESORDER, -1).toInt());
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
                tr("Save actual prescription?"),
                tr("The actual prescription is not empty. Do you want to save it before creating a new one?"));
        if (yes) {
            saveFile();
        }
    }
    patient()->clear();
    refreshPatient();
    drugModel()->clearDrugsList();
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
    return drugsIo().savePrescription(drugModel(), d->getXmlExtraData(), fileName);
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
                tr("Opening a prescription : merge or replace?"),
                tr("There is a prescription inside editor, do you to replace it or to add the opened prescription?"),
                QString(), QStringList() << tr("Replace prescription") << tr("Add to prescription"),
                tr("Open a prescription") + " - " + qApp->applicationName());
        if (r == 0) {
            drugsIo().loadPrescription(drugModel(), file, datas, DrugsDB::DrugsIO::ReplacePrescription);
        } else if (r==1) {
            drugsIo().loadPrescription(drugModel(), file, datas, DrugsDB::DrugsIO::AppendPrescription);
        }
    } else {
        drugsIo().loadPrescription(drugModel(), file, datas, DrugsDB::DrugsIO::ReplacePrescription);
    }
    // Process patient's data only if not blockDatas and not using exchange-in file
    if ((!commandLine()->value(Core::CommandLine::CL_BlockPatientDatas).toBool()) &&
        (commandLine()->value(Core::CommandLine::CL_ExchangeInFile).toString().isEmpty()))
        patient()->fromXml(datas);
    refreshPatient();
}

void MainWindow::createDockWindows()
{
    // Create template dock
    QDockWidget *dock = d->m_TemplatesDock = new QDockWidget(tkTr(Trans::Constants::TEMPLATES), this);
    dock->setObjectName("templatesDock");
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dock->setWidget(new Templates::TemplatesView(dock));
    addDockWidget(Qt::RightDockWidgetArea, dock);
    QMenu *menu = actionManager()->actionContainer(Core::Constants::M_TEMPLATES)->menu();
    menu->addAction(dock->toggleViewAction());

    // Create patient's precautions dock
//    dock = d->m_PrecautionsDock = new QDockWidget(tkTr(Trans::Constants::PATIENT_INFORMATION), this);
//    dock->setObjectName("precautionsDock");
//    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
//    addDockWidget(Qt::RightDockWidgetArea, dock);

//    DrugsDB::Internal::DrugAllergyEngine *engine = pluginManager()->getObject<DrugsDB::Internal::DrugAllergyEngine>();
//    if (!engine) {
//        LOG_ERROR_FOR("MainWindow", "No allergy engine");
//        return;
//    }
//    QAbstractItemModel *model = engine->drugPrecautionModel();

//    d->m_PrecautionView = new QTreeView(dock);
//    d->m_PrecautionView->setModel(model);
//    d->m_PrecautionView->header()->hide();
//    d->m_PrecautionView->expandAll();
//    d->m_PrecautionView->setEditTriggers(QAbstractItemView::NoEditTriggers);
//    dock->setWidget(d->m_PrecautionView);
//    // aShowPrecautionsDock
//    QAction *a = dock->toggleViewAction();
//    a->setObjectName("aShowPrecautionsDock");
//    Core::Command *cmd = actionManager()->registerAction(a, "aShowPrecautionsDock", QList<int>() << Core::Constants::C_GLOBAL_ID);
//    cmd->setTranslations(tr("Toggle patient's precautions"));
//    Core::ActionContainer *plugmenu = actionManager()->actionContainer(DrugsWidget::Constants::M_PLUGINS_DRUGS);
//    plugmenu->addAction(cmd, DrugsWidget::Constants::G_PLUGINS_VIEWS);
}

