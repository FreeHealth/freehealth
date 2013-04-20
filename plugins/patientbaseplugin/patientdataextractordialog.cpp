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
 * \class Patients::Internal::FormExporterDialog
 */

#include "formexporterdialog.h"
#include "patientmodel.h"

#include <formmanagerplugin/formexporter.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/constants_icons.h>

#include <utils/widgets/datetimedelegate.h>
#include <translationutils/constants.h>

#include "ui_formexporterdialog.h"

#include <QStringListModel>

#include <QDebug>

using namespace Patients;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}

namespace Patients {
namespace Internal {

class FormExporterDialogPrivate
{
public:
    FormExporterDialogPrivate(FormExporterDialog *parent) :
        _initialized(false),
        ui(new Ui::FormExporterDialog),
        _exportButton(0),
        _exporter(0),
        _patientModel(0),
        _selectedModel(0),
        q(parent)
    {
    }
    
    ~FormExporterDialogPrivate()
    {
        delete ui;
    }
    
public:
    bool _initialized;
    Ui::FormExporterDialog *ui;
    QPushButton *_exportButton;
    Form::FormExporter *_exporter;
    PatientModel *_patientModel;
    QStringListModel *_selectedModel;
    QString m_LastSearch;

private:
    FormExporterDialog *q;
};
} // namespace Internal
} // end namespace Patients


/*! Constructor of the Patients::Internal::FormExporterDialog class */
FormExporterDialog::FormExporterDialog(QWidget *parent) :
    QDialog(parent),
    d(new FormExporterDialogPrivate(this))
{
    d->_exporter = new Form::FormExporter(this);
    d->_patientModel = new PatientModel(this);
    d->_selectedModel = new QStringListModel(this);
}

/*! Destructor of the Patients::Internal::FormExporterDialog class */
FormExporterDialog::~FormExporterDialog()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool FormExporterDialog::initialize()
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

void FormExporterDialog::refreshPatientModelFilter()
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

void FormExporterDialog::onPatientActivated(const QModelIndex &index)
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

void FormExporterDialog::onExportRequested()
{
    d->_exportButton->setEnabled(false);
    Form::FormExporterJob job;
    job.setExportGroupmentType(Form::FormExporterJob::FormOrderedExportation);
    if (d->ui->exportAll->isChecked()) {
        job.setExportAllPatients(true);
    } else {
        const QStringList &list = d->_selectedModel->stringList();
        QStringList uids;
        foreach(const QString &patient, list) {
            uids << getUid(patient);
        }
        job.setPatientUids(uids);
    }
    d->_exporter->startExportation(job);
    d->_exportButton->setEnabled(true);
}
