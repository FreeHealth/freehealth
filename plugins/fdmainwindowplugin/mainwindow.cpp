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
#include <drugsbaseplugin/constants.h>
#include <drugsplugin/drugswidgetmanager.h>

#include <drugsbaseplugin/drugsio.h>
#include <drugsbaseplugin/drugsbase.h>

#include <templatesplugin/templatesview.h>

#include <extensionsystem/pluginerrorview.h>
#include <extensionsystem/pluginview.h>
#include <extensionsystem/pluginmanager.h>

#include "ui_mainwindow.h"

#include <translationutils/constanttranslations.h>
#include <utils/log.h>
#include <utils/global.h>
#include <utils/updatechecker.h>

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
static inline DrugsDB::Internal::DrugsBase *drugsBase() {return DrugsDB::Internal::DrugsBase::instance();}

// SplashScreen Messagers
static inline void messageSplash(const QString &s) {Core::ICore::instance()->messageSplashScreen(s); }
static inline void finishSplash(QMainWindow *w) {Core::ICore::instance()->finishSplashScreen(w); }


namespace MainWin {
namespace Internal {

class MainWinPrivate {
public:
    MainWinPrivate(MainWindow *parent) : m_Mapper(0), m_AllergiesModel(0), m_AllergiesView(0), q(parent)  {}
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
        QString exfile = commandLine()->value(Core::CommandLine::CL_ExchangeFile).toString();
        if (!exfile.isEmpty()) {
            messageSplash(msg);
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

    void createMapper()
    {
        if (!m_Mapper)
            m_Mapper = new QDataWidgetMapper(q);
        m_Mapper->setModel(patient());
        m_Mapper->addMapping(q->m_ui->patientName, Core::IPatient::BirthName);
        m_Mapper->addMapping(q->m_ui->patientSurname, Core::IPatient::Surname);
        m_Mapper->addMapping(q->m_ui->patientSize, Core::IPatient::Height);
        m_Mapper->addMapping(q->m_ui->sizeUnit, Core::IPatient::HeightUnit);
        m_Mapper->addMapping(q->m_ui->patientWeight, Core::IPatient::Weight);
        m_Mapper->addMapping(q->m_ui->weightUnit, Core::IPatient::WeightUnit);
        m_Mapper->addMapping(q->m_ui->sexCombo, Core::IPatient::GenderIndex);
        m_Mapper->addMapping(q->m_ui->patientCreatinin, Core::IPatient::Creatinine);
        m_Mapper->addMapping(q->m_ui->creatinineUnit, Core::IPatient::CreatinineUnit);
        m_Mapper->addMapping(q->m_ui->patientClCr, Core::IPatient::CreatinClearance);
        m_Mapper->addMapping(q->m_ui->crClUnit, Core::IPatient::CreatinClearanceUnit);
        m_Mapper->addMapping(q->m_ui->dobDateEdit, Core::IPatient::DateOfBirth);
        m_Mapper->toFirst();
    }

    static void addBranch(QStandardItem *rootAllergies, QStandardItem *rootIntolerances, const QString &name,
                          const QStringList &allergies, const QStringList &intolerances, bool atc, bool uids,
                          const QBrush &allergiesBrush, const QBrush &intolerancesBrush)
    {
        QStandardItem *allergiesItem = new QStandardItem(name);
        QStandardItem *intolerancesItem = new QStandardItem(name);
        allergiesItem->setForeground(allergiesBrush);
        intolerancesItem->setForeground(intolerancesBrush);
        rootAllergies->appendRow(allergiesItem);
        rootIntolerances->appendRow(intolerancesItem);
        //    allergies
        QStringList sorted = allergies;
        qSort(sorted);
        foreach(const QString &item, sorted) {
            QString lbl;
            if (atc)
                 lbl = drugsBase()->getAtcLabel(item);
            else if (uids)
                 lbl = drugsBase()->getDrugName(item);
            else lbl = item;
            if (!lbl.isEmpty()) {
                QStandardItem *i = new QStandardItem(lbl);
                i->setForeground(allergiesBrush);
                i->setToolTip(tkTr(Trans::Constants::ALLERGY_TO_1).arg(lbl));
                allergiesItem->appendRow(i);
            }
        }
        //    intol
        sorted.clear();
        sorted = intolerances;
        qSort(sorted);
        foreach(const QString &item, sorted) {
            QString lbl;
            if (atc)
                 lbl = drugsBase()->getAtcLabel(item);
            else if (uids)
                 lbl = drugsBase()->getDrugName(item);
            else lbl = item;
            if (!lbl.isEmpty()) {
                QStandardItem *i = new QStandardItem(lbl);
                i->setToolTip(tkTr(Trans::Constants::INTOLERANCE_TO_1).arg(lbl));
                i->setForeground(intolerancesBrush);
                intolerancesItem->appendRow(i);
            }
        }
    }

    void createPrecautionsModelAndView(const QStringList &atcAllergies, const QStringList &uidsAllergies, const QStringList &innsAllergies,
                                       const QStringList &atcIntolerances, const QStringList &uidsIntolerances, const QStringList &innsIntolerances,
                                       QStandardItemModel *model, QTreeView *treeview, QComboBox *combo)
    {
        // Create the model
        if (!model)
            model = new QStandardItemModel(q);
        QFont bold;
        bold.setBold(true);
        QStandardItem *rootItem = model->invisibleRootItem();
        if (atcAllergies.isEmpty() &&
            uidsAllergies.isEmpty() &&
            innsAllergies.isEmpty() &&
            atcIntolerances.isEmpty() &&
            uidsIntolerances.isEmpty() &&
            innsIntolerances.isEmpty()) {
            QStandardItem *uniqueItem = new QStandardItem(tkTr(Trans::Constants::NO_ALLERGIES_INTOLERANCES));
            uniqueItem->setFont(bold);
            rootItem->appendRow(uniqueItem);
        } else {
            QStandardItem *allergiesItem = new QStandardItem(tkTr(Trans::Constants::KNOWN_ALLERGIES));
            QStandardItem *intolerancesItem = new QStandardItem(tkTr(Trans::Constants::KNOWN_INTOLERANCES));
            allergiesItem->setFont(bold);
            intolerancesItem->setFont(bold);
            QBrush allergiesBrush = QBrush(QColor(settings()->value(DrugsDB::Constants::S_ALLERGYBACKGROUNDCOLOR).toString()).darker(300));
            QBrush intolerancesBrush = QBrush(QColor(settings()->value(DrugsDB::Constants::S_INTOLERANCEBACKGROUNDCOLOR).toString()).darker(300));
            allergiesItem->setForeground(allergiesBrush);
            intolerancesItem->setForeground(intolerancesBrush);

            rootItem->appendRow(allergiesItem);
            rootItem->appendRow(intolerancesItem);

            addBranch(allergiesItem, intolerancesItem, tkTr(Trans::Constants::ATC), atcAllergies, atcIntolerances, true, false, allergiesBrush, intolerancesBrush);
            addBranch(allergiesItem, intolerancesItem, tkTr(Trans::Constants::DRUGS), uidsAllergies, uidsIntolerances, false, true, allergiesBrush, intolerancesBrush);
            addBranch(allergiesItem, intolerancesItem, tkTr(Trans::Constants::INN), innsAllergies, innsIntolerances, false, false, allergiesBrush, intolerancesBrush);
        }

        // Create the view
        if (!treeview) {
            treeview = new QTreeView(q);
            q->m_ui->drugsPrecautions->setModel(model);
            q->m_ui->drugsPrecautions->setView(treeview);
            treeview->header()->hide();
            treeview->expandAll();
            treeview->resizeColumnToContents(0);
            treeview->setIndentation(10);
            treeview->setFrameStyle(QFrame::NoFrame);
            treeview->setAlternatingRowColors(true);
        }
    }

public:
    QDataWidgetMapper *m_Mapper;
    QStandardItemModel *m_AllergiesModel;
    QTreeView *m_AllergiesView;

private:
    MainWindow *q;
};

static bool transmitDosage()
{
    if (!Utils::isDebugCompilation()) {
        Utils::Log::addMessage("Core", QCoreApplication::translate("MainWindow", "Preparing dosage transmission"));
        DrugsDB::DrugsIO::instance()->startsDosageTransmission();
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
    m_ui->sexCombo->addItems(genders());

    if (commandLine()->value(Core::CommandLine::CL_EMR_Name).isValid())
        setWindowTitle(QString("%1 - %2 [%3 %4]")
                       .arg(qApp->applicationName())
                       .arg(qApp->applicationVersion())
                       .arg(tr("Connected to"))
                       .arg(commandLine()->value(Core::CommandLine::CL_EMR_Name).toString()));
    else
        setWindowTitle(QString("%1 - %2")
                       .arg(qApp->applicationName())
                       .arg(qApp->applicationVersion()));

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
    if (!d->readExchangeFile(tr("Reading exchange file..."))) {
        Utils::Log::addError(this, tkTr(Trans::Constants::FILE_1_ISNOT_READABLE).arg(commandLine()->value(Core::CommandLine::CL_ExchangeFile).toString()));
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
    if (d->m_Mapper)
        d->m_Mapper->setCurrentIndex(0);
    else
        d->createMapper();
    d->createPrecautionsModelAndView(patient()->data(Core::IPatient::DrugsAtcAllergies).toStringList(),
                                     patient()->data(Core::IPatient::DrugsUidAllergies).toStringList(),
                                     patient()->data(Core::IPatient::DrugsInnAllergies).toStringList(),
                                     patient()->data(Core::IPatient::DrugsAtcIntolerances).toStringList(),
                                     patient()->data(Core::IPatient::DrugsUidIntolerances).toStringList(),
                                     patient()->data(Core::IPatient::DrugsInnIntolerances).toStringList(),
                                     d->m_AllergiesModel,
                                     d->m_AllergiesView,
                                     m_ui->drugsPrecautions);
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

        // Add date of creation
        extraDatas.prepend(QString("  <DateOfCreation>%1</DateOfCreation>").arg(QDateTime::currentDateTime().toString(Qt::ISODate)));

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
        QVariant sex = patient()->data(Core::IPatient::Gender);
        bool state = true;
        m_ui->centralwidget->blockSignals(state);
        m_ui->crClUnit->blockSignals(state);
        m_ui->creatinineUnit->blockSignals(state);
        m_ui->dobDateEdit->blockSignals(state);
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
        patient()->setData(patient()->index(0, Core::IPatient::Gender), sex);
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

