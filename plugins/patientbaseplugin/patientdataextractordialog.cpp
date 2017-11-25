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
 *   Main developers: Eric MAEKER, <eric.maeker@gmail.com>                 *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*! \brief Dialog to select 1 or several patients for data export
 * \class Patients::Internal::PatientDataExtractorDialog
 */

#include "patientdataextractordialog.h"
#include "patientmodel.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/idocumentprinter.h>
#include <coreplugin/ipatientdataexporter.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/widgets/datetimedelegate.h>
#include <translationutils/constants.h>
#include <extensionsystem/pluginmanager.h>

#include "ui_patientdataextractordialog.h"

#include <QStringListModel>
#include <QProgressDialog>
#include <QDir>

#include <QDebug>

using namespace Patients;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}
static inline Core::IDocumentPrinter *printer() {return ExtensionSystem::PluginManager::instance()->getObject<Core::IDocumentPrinter>();}

namespace {
const char * const HTML_HEADER = "<!DOCTYPE html>\n"
        "<html>\n"
        "<head>\n"
	"<meta charset=\"UTF-8\">\n"
        "%1\n"
        "<title>%2</title>\n"
        "</head>\n"
        "<body>\n";

const char * const HTML_FOOTER = "\n\n</body></html>\n";
}

namespace Patients {
namespace Internal {

class PatientDataExtractorDialogPrivate
{
public:
    PatientDataExtractorDialogPrivate(PatientDataExtractorDialog */*parent*/) :
        _initialized(false),
        ui(new Ui::PatientDataExtractorDialog),
        _exportButton(0),
        _patientModel(0),
        _selectedModel(0) // ,q(parent)
    {
    }
    
    ~PatientDataExtractorDialogPrivate()
    {
        delete ui;
    }
    
public:
    bool _initialized;
    Ui::PatientDataExtractorDialog *ui;
    QPushButton *_exportButton;
    PatientModel *_patientModel;
    QStringListModel *_selectedModel;
    QString m_LastSearch;

private:
//    PatientDataExtractorDialog *q;
};
} // namespace Internal
} // end namespace Patients


/*! Constructor of the Patients::Internal::PatientDataExtractorDialog class */
PatientDataExtractorDialog::PatientDataExtractorDialog(QWidget *parent) :
    QDialog(parent),
    d(new PatientDataExtractorDialogPrivate(this))
{
    setObjectName("PatientDataExtractorDialog");
//    d->_exporter = new Form::FormExporter(this);
    d->_patientModel = new PatientModel(this);
    d->_selectedModel = new QStringListModel(this);
}

/*! Destructor of the Patients::Internal::PatientDataExtractorDialog class */
PatientDataExtractorDialog::~PatientDataExtractorDialog()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool PatientDataExtractorDialog::initialize()
{
    if (d->_initialized)
        return true;

    d->ui->setupUi(this);
    setWindowTitle(tr("Patient data extractor"));
    setWindowIcon(theme()->icon(Core::Constants::ICONEHR));
    d->ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    d->_exportButton = d->ui->buttonBox->addButton(tr("Export..."), QDialogButtonBox::ActionRole);
    connect(d->_exportButton, SIGNAL(clicked()), this, SLOT(onExportRequested()));

    d->ui->pathChooser->setPath(settings()->path(Core::ISettings::UserDocumentsPath));
    // d->ui->pathChooser->setPromptDialogTitle("");

    d->ui->search->setDelayedSignals(true);
    d->ui->search->setLeftIcon(theme()->icon(Core::Constants::ICONSEARCH));
    connect(d->ui->search, SIGNAL(textChanged(QString)), this, SLOT(refreshPatientModelFilter()));

    // TODO: use a common PatientSelector widget
    d->_patientModel->setFilter("%", "%");
    d->ui->availablePatients->setModel(d->_patientModel);
    d->ui->availablePatients->setItemDelegateForColumn(Core::IPatient::DateOfBirth, new Utils::DateTimeDelegate(this, true));
    for(int i=0; i < Core::IPatient::NumberOfColumns; ++i)
        d->ui->availablePatients->hideColumn(i);
    d->ui->availablePatients->showColumn(Core::IPatient::UsualName);
    d->ui->availablePatients->showColumn(Core::IPatient::OtherNames);
    d->ui->availablePatients->showColumn(Core::IPatient::Firstname);
    d->ui->availablePatients->showColumn(Core::IPatient::Title);
    d->ui->availablePatients->showColumn(Core::IPatient::DateOfBirth);
    d->ui->availablePatients->showColumn(Core::IPatient::FullAddress);
    d->ui->availablePatients->showColumn(Core::IPatient::PractitionerUuid);
    d->ui->availablePatients->horizontalHeader()->setStretchLastSection(false);

    d->ui->availablePatients->horizontalHeader()->setSectionResizeMode(Core::IPatient::UsualName, QHeaderView::Stretch);
    d->ui->availablePatients->horizontalHeader()->setSectionResizeMode(Core::IPatient::OtherNames, QHeaderView::Stretch);
    d->ui->availablePatients->horizontalHeader()->setSectionResizeMode(Core::IPatient::Firstname, QHeaderView::Stretch);
    d->ui->availablePatients->horizontalHeader()->setSectionResizeMode(Core::IPatient::FullName, QHeaderView::ResizeToContents);
    d->ui->availablePatients->horizontalHeader()->setSectionResizeMode(Core::IPatient::IconizedGender, QHeaderView::ResizeToContents);
    d->ui->availablePatients->horizontalHeader()->setSectionResizeMode(Core::IPatient::Title, QHeaderView::ResizeToContents);
    d->ui->availablePatients->horizontalHeader()->setSectionResizeMode(Core::IPatient::DateOfBirth, QHeaderView::ResizeToContents);
    d->ui->availablePatients->horizontalHeader()->setSectionResizeMode(Core::IPatient::FullAddress, QHeaderView::Stretch);
    d->ui->availablePatients->horizontalHeader()->setSectionResizeMode(Core::IPatient::PractitionerUuid, QHeaderView::ResizeToContents);

    connect(d->ui->availablePatients, SIGNAL(activated(QModelIndex)),
            this, SLOT(onPatientActivated(QModelIndex)));

    connect(d->ui->addCurrent, SIGNAL(clicked()),
            this, SLOT(onAddCurrentClicked()), Qt::UniqueConnection);

    connect(d->ui->addAll, SIGNAL(clicked()),
            this, SLOT(onAddAllClicked()));

    d->ui->selectedPatients->setModel(d->_selectedModel);

    connect(d->ui->selectedPatients, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(onPatientRemoved(QModelIndex)));

    connect(d->ui->removeAll, SIGNAL(clicked(bool)),
            this, SLOT(onRemoveAllClicked()));

    return true;
}

void PatientDataExtractorDialog::refreshPatientModelFilter()
{
    if (!d->_patientModel)
        return;
    QString text = d->ui->search->text();
    if (text == d->m_LastSearch)
        return;
    d->m_LastSearch = text;
    QString name;
    d->_patientModel->setFilter(text, "%");
}

void PatientDataExtractorDialog::onPatientActivated(const QModelIndex &index)
{
    QModelIndex full = d->_patientModel->index(index.row(), Core::IPatient::FullName);
    QModelIndex uid = d->_patientModel->index(index.row(), Core::IPatient::Uid);
    QModelIndex dob = d->_patientModel->index(index.row(), Core::IPatient::DateOfBirth);
    QString toAdd = QString("%1 %2 {%3}")
            .arg(d->_patientModel->data(full).toString())
            .arg(d->_patientModel->data(dob).toString())
            .arg(d->_patientModel->data(uid).toString());
    QStringList list = d->_selectedModel->stringList();
    if (list.contains(toAdd))                                                   
        return;
    list << toAdd;
    d->_selectedModel->setStringList(list);
}

void PatientDataExtractorDialog::onPatientRemoved(const QModelIndex &index)
{
    QStringList list = d->_selectedModel->stringList();
    QString patient = index.data().toString();
    if (list.contains(patient)) {
        list.removeAll(patient);
        d->_selectedModel->setStringList(list);
    }
    return;
}

void PatientDataExtractorDialog::onAddCurrentClicked()
{
    if (patient()->uuid().isEmpty())
        return;
    QString toAdd = QString("%1 %2 {%3}")
            .arg(patient()->data(Core::IPatient::FullName).toString())
            .arg(patient()->data(Core::IPatient::DateOfBirth).toString())
            .arg(patient()->data(Core::IPatient::Uid).toString());
    QStringList list = d->_selectedModel->stringList();
    if (list.contains(toAdd))
        return;
    list << toAdd;
    d->_selectedModel->setStringList(list);
}

void PatientDataExtractorDialog::onAddAllClicked()
{
    for (int row = 0; row < d->_patientModel->rowCount(); ++row) {
        QModelIndex patient_idx = d->_patientModel->index(row, 0);
        onPatientActivated(patient_idx);
    }
}

void PatientDataExtractorDialog::onRemoveAllClicked()
{
    d->_selectedModel->setStringList(QStringList());
}

static QString getUid(const QString &selected)
{
    int b = selected.indexOf("{") + 1;
    int e = selected.indexOf("}", b);
    return selected.mid(b, e-b);
}

static QList<Core::PatientDataExtraction*> extract(const QList<Core::IPatientDataExporter *> &extractors, const Core::PatientDataExporterJob &job, Core::IPatientDataExporter::ExporterType type)
{
    QList<Core::PatientDataExtraction *> result;
    foreach(Core::IPatientDataExporter *e, extractors) {
        if (e->exporterTypes().testFlag(type)) {
            Core::PatientDataExtraction *r = e->startExportationJob(job);
            if (r)
                result << r;
        }
    }
    return result;
}

static QList<Core::PatientDataExtraction*> extractAll(const QList<Core::IPatientDataExporter *> &extractors, const Core::PatientDataExporterJob &job)
{
    // Extract in the correct order
    QList<Core::PatientDataExtraction *> result;
    result << extract(extractors, job, Core::IPatientDataExporter::IdentityExporter);
    result << extract(extractors, job, Core::IPatientDataExporter::PmhxExporter);
    result << extract(extractors, job, Core::IPatientDataExporter::FormsExporter);
    result << extract(extractors, job, Core::IPatientDataExporter::VaccineExporter);
    result << extract(extractors, job, Core::IPatientDataExporter::DocumentExporter);
    return result;
}

//#include <QTextBrowser>
void PatientDataExtractorDialog::onExportRequested()
{
    // Avoid multiclicks on button
    d->_exportButton->setEnabled(false);

    // Get all extractors
    QList<Core::IPatientDataExporter *> extractors = pluginManager()->getObjects<Core::IPatientDataExporter>();
    if (extractors.isEmpty()) {
        LOG_ERROR("No Core::IPatientDataExporter object found");
        return;
    }

    // Log & timer
    LOG("---------- Starting patient data extraction");
    QTime chr;
    chr.start();
    QTime chrGlobal;
    chrGlobal.start();

    // Prepare patient extraction job
    QList<Core::PatientDataExtraction *> result;
    Core::PatientDataExporterJob job;
    if (d->ui->group->currentIndex() == 0)
        job.setExportGroupmentType(Core::PatientDataExporterJob::FormOrderedExportation);
    else
        job.setExportGroupmentType(Core::PatientDataExporterJob::DateOrderedExportation);

    // Get patient uids to extract
    QStringList uids;
    const QStringList &list = d->_selectedModel->stringList();
    foreach(const QString &patient, list) {
        uids << getUid(patient);
    }
    Utils::Log::logTimeElapsed(chr, objectName(), "Getting patient uids to extract");

    // Store current patient uid
    QString current = patient()->uuid();

    // Show a progress dialog
    QProgressDialog dlg(this);
    dlg.setWindowTitle(windowTitle());
    dlg.setWindowIcon(windowIcon());
    dlg.setLabelText(tr("Start patient file extraction"));
    dlg.setAutoClose(false);
    dlg.setRange(0, 0);
    Utils::centerWidget(&dlg, this);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    dlg.show();

    // Connect all extractor signals to the progress dialog
    foreach(Core::IPatientDataExporter *e, extractors) {
        connect(e, SIGNAL(extractionProgressRangeChanged(int,int)), &dlg, SLOT(setRange(int,int)));
        connect(e, SIGNAL(extractionProgressValueChanged(int)), &dlg, SLOT(setValue(int)));
        connect(e, SIGNAL(extractionProgressMessageChanged(QString)), &dlg, SLOT(setLabelText(QString)));
    }

    // Start extraction patient by patient
    QStringList finalMessage;
    foreach(const QString &uid, uids) {
        // Progress cosmetics
        dlg.setLabelText(tr("Setting patient uuid %1 as current patient").arg(uid));
        Utils::centerWidget(&dlg, this);
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

        // Define current patient for extraction
        job.setPatientUid(uid);
        patient()->setCurrentPatientUid(uid);

        // Define output path
        QString fullname = patient()->data(Core::IPatient::FullName).toString();
        QString dob = patient()->data(Core::IPatient::DateOfBirth).toString();
        QString path = QString("%1/%2 %3")
               .arg(d->ui->pathChooser->path())
               .arg(fullname)
               .arg(dob);
        if (!QDir().mkpath(path))
            LOG_ERROR("Unable to create path: " + path);
        job.setOutputAbsolutePath(path);

        // Run extractors
        result << extractAll(extractors, job);
        Utils::Log::logTimeElapsed(chr, objectName(), QString("Extracted patient uid: %1").arg(uid));

        // According to params -> recreate one big HTML file
        QString out, outCss;
        foreach(Core::PatientDataExtraction *exp, result) {
            // Append all secondary files into the master
            QString master;
            master = Utils::readTextFile(exp->masterAbsoluteFilePath(), Utils::DontWarnUser);
            QFile(exp->masterAbsoluteFilePath()).remove();
            if (master.isEmpty())
                continue;
            QString masterCss;
            masterCss = Utils::htmlTakeAllCssContent(master);
            master = Utils::htmlBodyContent(master);
            foreach(const QString &sec, exp->secondaryFiles()) {
                QString second = Utils::readTextFile(sec, Utils::DontWarnUser);
                QFile(sec).remove();
                if (second.isEmpty())
                    continue;
                QString secCss;
                secCss = Utils::htmlTakeAllCssContent(second);
                master += Utils::htmlBodyContent(second);
                masterCss += secCss;
            }
            out += master;
            outCss += masterCss;
        }

        // According to params -> transform to one big HTML file
        finalMessage << tr("Exporting %1 file")
                        .arg(QString("<b>%1</b>")
                             .arg(patient()->data(Core::IPatient::FullName).toString())
                             );

        // Re-create a full HTML document with exported datas
        QString header = QString(HTML_HEADER).arg(outCss).arg(tr("Patient file exportation - %1").arg(patient()->data(Core::IPatient::FullName).toString()));
        QString html = QString("%1\n%2\n%3").arg(header).arg(out).arg(HTML_FOOTER);
        // file name
        QString filename = QString("%2 %3").arg(fullname).arg(dob);
        // Save HTML file
        if (d->ui->outputFormat->currentText().contains("HTML", Qt::CaseInsensitive)) {
            if (!Utils::saveStringToFile(html, job.outputAbsolutePath() + "/" + filename + ".html")) {
                LOG_ERROR("Unable to save file");
                finalMessage << QString("&nbsp;&nbsp;%1")
                                .arg(tr("An error occured when saving the HTML patient file"));
            } else {
                finalMessage << QString("&nbsp;&nbsp;%1")
                                .arg(QString("<a href='file://%1'>%2</a>")
                                     .arg(tr("Patient HTML file correctly created"))
                                     .arg(job.outputAbsolutePath() + "/" + filename + ".html"));
            }
        }

        // Save PDF file
        if (d->ui->outputFormat->currentText().contains("PDF", Qt::CaseInsensitive)) {
            if (!printer()->toPdf(html, job.outputAbsolutePath() + "/" + filename + ".pdf")) {
                LOG_ERROR("Unable to save file");
                finalMessage << QString("&nbsp;&nbsp;%1")
                                .arg(tr("An error occured when saving the PDF patient file"));
            } else {
                finalMessage << QString("&nbsp;&nbsp;%1")
                                .arg(QString("<a href='file://%1'>%2</a>")
                                     .arg(tr("Patient PDF file correctly created"))
                                     .arg(job.outputAbsolutePath() + "/" + filename + ".pdf"));
            }
        }
    }

    // Reset current patient
    dlg.setLabelText(tr("Setting patient uuid %1 as current patient").arg(current));
    Utils::centerWidget(&dlg, this);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    patient()->setCurrentPatientUid(current);

    qDeleteAll(result);

    Utils::Log::logTimeElapsed(chrGlobal, objectName(), QString("Patient data extraction done"));
    LOG("---------- Patient data extraction done");

    // Show a small dialog with the finalMessage content
//    QTextBrowser *b = new QTextBrowser;
//    b->setHtml(finalMessage.join("<br/><br/>"));
//    b->show();
    d->_exportButton->setEnabled(true);
    d->ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}
