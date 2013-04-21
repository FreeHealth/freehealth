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
 *   Main developers: Eric MAEKER, <eric.maeker@gmail.com>                 *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class Patients::Internal::PatientDataExtractorDialog
 */

#include "patientdataextractordialog.h"
#include "patientmodel.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/ipatientdataexporter.h>

#include <utils/log.h>
#include <utils/widgets/datetimedelegate.h>
#include <translationutils/constants.h>
#include <extensionsystem/pluginmanager.h>

#include "ui_patientdataextractordialog.h"

#include <QStringListModel>

#include <QDebug>

using namespace Patients;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}

namespace Patients {
namespace Internal {

class PatientDataExtractorDialogPrivate
{
public:
    PatientDataExtractorDialogPrivate(PatientDataExtractorDialog *parent) :
        _initialized(false),
        ui(new Ui::PatientDataExtractorDialog),
        _exportButton(0),
        _patientModel(0),
        _selectedModel(0),
        q(parent)
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
    PatientDataExtractorDialog *q;
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

    setWindowTitle(tr("Patient data extractor"));
    setWindowIcon(theme()->icon(Core::Constants::ICONFREEMEDFORMS));
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
    d->ui->availablePatients->showColumn(Core::IPatient::IconizedGender);
    d->ui->availablePatients->showColumn(Core::IPatient::Title);
    d->ui->availablePatients->showColumn(Core::IPatient::DateOfBirth);
    d->ui->availablePatients->showColumn(Core::IPatient::FullAddress);
    d->ui->availablePatients->horizontalHeader()->setStretchLastSection(false);
#if QT_VERSION < 0x050000
    d->ui->availablePatients->horizontalHeader()->setResizeMode(Core::IPatient::UsualName, QHeaderView::Stretch);
    d->ui->availablePatients->horizontalHeader()->setResizeMode(Core::IPatient::OtherNames, QHeaderView::Stretch);
    d->ui->availablePatients->horizontalHeader()->setResizeMode(Core::IPatient::Firstname, QHeaderView::Stretch);
    d->ui->availablePatients->horizontalHeader()->setResizeMode(Core::IPatient::FullName, QHeaderView::ResizeToContents);
    d->ui->availablePatients->horizontalHeader()->setResizeMode(Core::IPatient::IconizedGender, QHeaderView::ResizeToContents);
    d->ui->availablePatients->horizontalHeader()->setResizeMode(Core::IPatient::Title, QHeaderView::ResizeToContents);
    d->ui->availablePatients->horizontalHeader()->setResizeMode(Core::IPatient::DateOfBirth, QHeaderView::ResizeToContents);
    d->ui->availablePatients->horizontalHeader()->setResizeMode(Core::IPatient::FullAddress, QHeaderView::Stretch);
    d->ui->availablePatients->horizontalHeader()->setResizeMode(Core::IPatient::PractitionnerLkID, QHeaderView::ResizeToContents);
#else
    // Qt5
    d->ui->availablePatients->horizontalHeader()->setSectionResizeMode(Core::IPatient::UsualName, QHeaderView::Stretch);
    d->ui->availablePatients->horizontalHeader()->setSectionResizeMode(Core::IPatient::OtherNames, QHeaderView::Stretch);
    d->ui->availablePatients->horizontalHeader()->setSectionResizeMode(Core::IPatient::Firstname, QHeaderView::Stretch);
    d->ui->availablePatients->horizontalHeader()->setSectionResizeMode(Core::IPatient::FullName, QHeaderView::ResizeToContents);
    d->ui->availablePatients->horizontalHeader()->setSectionResizeMode(Core::IPatient::IconizedGender, QHeaderView::ResizeToContents);
    d->ui->availablePatients->horizontalHeader()->setSectionResizeMode(Core::IPatient::Title, QHeaderView::ResizeToContents);
    d->ui->availablePatients->horizontalHeader()->setSectionResizeMode(Core::IPatient::DateOfBirth, QHeaderView::ResizeToContents);
    d->ui->availablePatients->horizontalHeader()->setSectionResizeMode(Core::IPatient::FullAddress, QHeaderView::Stretch);
    d->ui->availablePatients->horizontalHeader()->setSectionResizeMode(Core::IPatient::PractitionnerLkID, QHeaderView::ResizeToContents);
#endif
    connect(d->ui->availablePatients, SIGNAL(activated(QModelIndex)), this, SLOT(onPatientActivated(QModelIndex)));

    d->ui->selectedPatients->setModel(d->_selectedModel);
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
    QString toAdd = QString("%1 {%2}")
            .arg(d->_patientModel->data(full).toString())
            .arg(d->_patientModel->data(uid).toString());
    QStringList list = d->_selectedModel->stringList();
    list << toAdd;
    d->_selectedModel->setStringList(list);
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
    job.setExportGroupmentType(Core::PatientDataExporterJob::DateOrderedExportation);

    // Get patient uids to extract
    QStringList uids;
    if (d->ui->exportAll->isChecked()) {
//        uids << patient()->allValidPatientUids();
    } else {
        const QStringList &list = d->_selectedModel->stringList();
        foreach(const QString &patient, list) {
            uids << getUid(patient);
        }
    }
    Utils::Log::logTimeElapsed(chr, objectName(), "Getting patient uids to extract");

    // Store current patient uid
    QString current = patient()->uuid();

    // Start extraction patient by patient
    foreach(const QString &uid, uids) {
        job.setPatientUid(uid);
        patient()->setCurrentPatientUid(uid);
        result << extractAll(extractors, job);
        Utils::Log::logTimeElapsed(chr, objectName(), QString("Extracted patient uid: %1").arg(uid));
    }

    // Reset current patient
    patient()->setCurrentPatientUid(current);

    qWarning() << result.count();
    qDeleteAll(result);

    Utils::Log::logTimeElapsed(chrGlobal, objectName(), QString("Patient data extraction done"));
    LOG("---------- Patient data extraction done");

    d->_exportButton->setEnabled(true);
}
