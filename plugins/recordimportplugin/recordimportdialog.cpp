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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main developers : Jerome Pinguet <jerome@jerome.cc>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 * \class RecordImport::RecordImportDialog
 * Import record(s) from external softwares.
 */

#include <memory>
#include "recordimportdialog_p.h"

#include <coreplugin/ipatient.h>
#include <formmanagerplugin/constants_settings.h>
#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/iformitemspec.h>
#include <formmanagerplugin/formcore.h>
#include "formmanagerplugin/formeditordialog.h"
#include "formmanagerplugin/formfilesselectorwidget.h"
#include <formmanagerplugin/formmanager.h>
#include "formmanagerplugin/formmanager_exporter.h"
#include "formmanagerplugin/iformio.h"
#include <formmanagerplugin/episodemanager.h>
#include <formmanagerplugin/episodemodel.h>
#include <patientbaseplugin/patientbase.h>
#include "utils/genericdescription.h"
#include <utils/global.h>
#include "utils/widgets/genericdescriptioneditor.h"

#include <QFormLayout>
#include <QCheckBox>
#include <QDateTimeEdit>
#include <QLineEdit>
#include <QComboBox>

using namespace RecordImport;
using namespace Internal;

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Form::FormManager &formManager() {return Form::FormCore::instance().formManager();}
static inline Form::EpisodeManager &episodeManager() {return Form::FormCore::instance().episodeManager();}
static inline Patients::Internal::PatientBase *patientBase() { return Patients::Internal::PatientBase::instance(); }
static inline Core::IPatient *patient()  { return Core::ICore::instance()->patient(); }

/*!
 * \brief RecordImportDialog::RecordImportDialog
 * \param parent
 * Date of birth format should be ISO 8601 (YYYY-MM-DD). If it is not, or you
 * need to modify the function RecordImportDialo::import() to convert your date
 * string to YYYY-MM-DD
 */
RecordImportDialog::RecordImportDialog(QWidget *parent) :
    QDialog(parent),
    d(new RecordImportDialogPrivate(this))
{
    d->createUi();
    setSoftwareComboBoxItems(RecordImport::Internal::RecordimportPlugin::softwareList());
    d->ui->softwareLabel->setText(tr("Software"));
    d->ui->startPushButton->setText(tr("Start import"));
    d->ui->startPushButton->setEnabled(false);
    d->connectUi();
    //d->getReportInformation();
    //d->_sendButton->setText(tr("&Send report"));
    setWindowTitle(tr("Record import dialog"));
    Utils::centerWidget(this, parent);
    QObject::connect(d->ui->formPushButton, &QPushButton::clicked,
                     this, &RecordImportDialog::selectForm);
}

RecordImportDialog::~RecordImportDialog()
{
    //    if (d)
    //        delete d;
    //    d = 0;
}

void RecordImportDialog::changeEvent(QEvent *e)
{
    Q_UNUSED(e);
    return;
}

void RecordImportDialog::setSoftwareComboBoxItems(const QStringList &softlist)
{
    d->ui->softwareComboBox->clear();
    d->ui->softwareComboBox->addItems(softlist);
    d->ui->softwareComboBox->setCurrentIndex(-1);
}

void RecordImportDialog::parse()
{
    if (d->ui->fieldSeparatorLineEdit->text().isEmpty()) {
        d->ui->fieldSeparatorLineEdit->setFocus();
        d->ui->fieldSeparatorLineEdit->setPlaceholderText(tr("Mandatory field."));
    } else if (d->ui->recordSeparatorLineEdit->text().isEmpty()) {
        d->ui->recordSeparatorLineEdit->setFocus();
        d->ui->recordSeparatorLineEdit->setPlaceholderText(tr("Mandatory field."));
    }
    QChar field = d->ui->fieldSeparatorLineEdit->text().at(0);
    QChar record = d->ui->recordSeparatorLineEdit->text().at(0);
    auto data_uptr = ProcessRecordImport::parse(d->fileName(), field, record);
    m_data = data_uptr.release();
    for (int i = 0; i < m_data->size(); ++i) {
        qWarning()  << "##### StringList " << i << " #####";
        for (int j = 0; j < m_data->at(i).size(); ++j){
            if (!(m_data->at(i).size()==m_data->at(0).size())) {
                qWarning() << "Number of QStringList items ("
                           << m_data->at(i).size()
                           << ") is different from number of header items ("
                           << m_data->at(0).size();
            }
            qWarning() << j << m_data->at(i).at(j);
        }
    }

    m_field.clear();
    for (int j = 0; j < m_data->at(0).size(); ++j) {
        m_field << m_data->at(0).at(j);
    };
    m_formItemList.clear();
    Form::FormMain *form = formManager().form(m_uuid);
    foreach(Form::FormItem *item, form->flattenedFormItemChildren()) {
        m_formItemList << item;
    }

    matchPatientWidget();
    matchEpisodeWidget();
    matchFormWidget();
}

void RecordImportDialog::matchPatientWidget() {
    d->ui->usualNameLabel->setText(RecordImport::Constants::PATIENT_IDENTITY_USUALNAME);
    d->ui->firstNameLabel->setText(RecordImport::Constants::PATIENT_IDENTITY_FIRSTNAME);
    d->ui->dobLabel->setText(RecordImport::Constants::PATIENT_IDENTITY_DOB);
    d->ui->usualNameComboBox->addItems(m_field);
    d->ui->usualNameComboBox->setCurrentIndex(-1);
    d->ui->firstNameComboBox->addItems(m_field);
    d->ui->firstNameComboBox->setCurrentIndex(-1);
    d->ui->dobComboBox->addItems(m_field);
    d->ui->dobComboBox->setCurrentIndex(-1);
}

void RecordImportDialog::matchEpisodeWidget() {
    QWidget *matchEpisodeWidget = new QWidget;
    QGridLayout *matchEpisodeGridLayout = new QGridLayout;
    QStringList episodeItems;
    episodeItems << "UserDateTime"
                 << "Label"
                 << "Priority"
                 << "UserCreatorName (uuid)";
    for (int j = 0; j < episodeItems.size(); ++j) {
        QString label = m_data->at(0).at(j);
        matchEpisodeGridLayout->addWidget(new QLabel (episodeItems.at(j)), j, 0);
    }
    // UserDateTime
    m_episodeUserDateTimeComboBox = new QComboBox;
    m_episodeUserDateTimeComboBox->addItems(m_field);
    m_episodeUserDateTimeComboBox->setCurrentIndex(-1);
    matchEpisodeGridLayout->addWidget(m_episodeUserDateTimeComboBox, 0, 1);

    // Label
    m_episodeLabelComboBox = new QComboBox;
    m_episodeLabelComboBox->addItems(m_field);
    m_episodeLabelComboBox->setCurrentIndex(-1);
    matchEpisodeGridLayout->addWidget(m_episodeLabelComboBox, 1, 1);

    // Priority: let's not implement this right now

    // UserCreatorName (uuid)
    m_episodeUserCreatorNameComboBox = new QComboBox;
    m_episodeUserCreatorNameComboBox->addItems(m_field);
    m_episodeUserCreatorNameComboBox->setCurrentIndex(-1);
    matchEpisodeGridLayout->addWidget(m_episodeUserCreatorNameComboBox, 3, 1);

    // user chosen values applied to all episodes
    // UserDateTime
    m_episodeUserDateTimeDateTimeEdit = new QDateTimeEdit;
    matchEpisodeGridLayout->addWidget(m_episodeUserDateTimeDateTimeEdit, 0, 2);
    // Label
    m_episodeLabelLineEdit = new QLineEdit;
    matchEpisodeGridLayout->addWidget(m_episodeLabelLineEdit, 1, 2);
    // Priority
    QComboBox *priority = new QComboBox;
    QStringList priorityList = QStringList();
    priorityList << "High" << "Medium" << "Low";
    priority->addItems(priorityList);
    matchEpisodeGridLayout->addWidget(priority, 2, 2);
    // UserCreatorName (uuid)
    QLineEdit *userCreatorName = new QLineEdit;
    matchEpisodeGridLayout->addWidget(userCreatorName, 3, 2);

    // validate episode widgets
    matchEpisodeWidget->setLayout(matchEpisodeGridLayout);
    d->ui->matchEpisodeScrollArea->setWidget(matchEpisodeWidget);

    Form::FormMain *form = formManager().form(m_uuid);
    foreach(Form::FormItem *item, form->flattenedFormItemChildren()) {
        qDebug() << item->spec()->label();
    }
}


void RecordImportDialog::matchFormWidget() {
        QGridLayout *matchFormGridLayout = new QGridLayout;
        QWidget *matchFormWidget = new QWidget;
        m_formItemHash = new QHash<QString, QString>;
        for (int j = 0; j < m_formItemList.size(); ++j) {
            QString label = m_formItemList.at(j)->spec()->label();
            QString labelPlugin = m_formItemList.at(j)->spec()->pluginName();
            if (!m_formItemList.at(j)->uuid().isEmpty()) {
                m_formItemHash->insert(m_formItemList.at(j)->uuid(), QString());
                qDebug() << "key: "
                     << m_formItemList.at(j)->uuid()
                     << "value: "
                     << m_formItemHash->value(m_formItemList.at(j)->uuid());
            }
            matchFormGridLayout->addWidget(new QLabel(label), j, 0);
            matchFormGridLayout->addWidget(new QLabel(labelPlugin), j, 1);
            QComboBox *combo = new QComboBox;
            combo->addItems(m_field);
            combo->setCurrentIndex(-1);
            m_comboFormList.append(combo);
            matchFormGridLayout->addWidget(m_comboFormList.at(j), j, 2);
        }
        matchFormWidget->setLayout(matchFormGridLayout);
        d->ui->matchFormScrollArea->setWidget(matchFormWidget);
}

void RecordImportDialog::import()
{
    //Form::FormMain *form = formManager().form(m_uuid);

    d->ui->startPushButton->setEnabled(false);
    //QString form_uuid = "Chave::Past::Medical::History";
    Form::EpisodeModel *model = episodeManager().episodeModel(m_formUuid);
    QString usualName;
    QString firstname;
    QString dateOfBirth;
    QDate dob;
    QString othernames;
    QString gender;
    QModelIndex idx;
    for (int i=1; i < m_data->size(); ++i) {
        usualName = m_data->at(i).at(d->ui->usualNameComboBox->currentIndex());
        firstname = m_data->at(i).at(d->ui->firstNameComboBox->currentIndex());
        dateOfBirth = m_data->at(i).at(d->ui->dobComboBox->currentIndex());
        if (d->ui->softwareComboBox->currentText() == RecordImport::Constants::GESTCAB) {
            dateOfBirth = dateOfBirth/*.remove(QChar('"'))*/.remove(QChar('/'));
            dob = QDate::fromString(dateOfBirth, "ddMMyyyy");
        } else {
            dob = QDate::fromString(dateOfBirth, "yyyy-MM-dd");
        }
        QString patientUuid = patientBase()->patientUuid(usualName, othernames, firstname, gender, dob);
        qDebug() << usualName << firstname << dateOfBirth << dob << patientUuid;
        //model->refreshFilter();
        patient()->setCurrentPatientUid(patientUuid);

        //Form::FormMain *form = formManager().form(m_uuid);
        //qDebug() << form;

        //model = episodeManager().episodeModel(form);
        qDebug() << model;
        //model->setCurrentPatient(uuid);
        model->setReadOnly(false);
        qDebug() << "rowCount" << model->rowCount();
        int row;
        for (int k=0; k < model->rowCount(); ++k) {
            idx = model->index(k, Form::EpisodeModel::DataRepresentation::IsValid);
            qDebug() << "model->data(idx).toInt()" << model->data(idx).toInt();
        }
        qDebug() << "isUniqueEpisode = " << formManager().form(m_formUuid)->isUniqueEpisode();
        qDebug() << "del uniq checkbox" << d->ui->deleteUniqueEpisodeCheckBox->isChecked();
        qDebug() << "m_uuid" << m_uuid;
        qDebug() << "m_uuid" << m_formUuid;

        if (d->ui->deleteUniqueEpisodeCheckBox->isChecked()
                && formManager().form(m_formUuid)->isUniqueEpisode()) {
            qDebug() << "inside delete episodes";
            //model->removeAllEpisodes();
            for (int k=0; k < model->rowCount(); ++k) {
                //idx = model->index(k, Form::EpisodeModel::DataRepresentation::IsValid);
                //model->setData(idx, 0);
                idx = model->index(k);
                model->removeEpisode(idx);
                model->submit();
            }
            qDebug() << "rowCount" << model->rowCount();
            model->submit();
            //model->refreshFilter();
            for (int k=0; k < model->rowCount(); ++k) {
                idx = model->index(k, Form::EpisodeModel::DataRepresentation::IsValid);
                qDebug() << k << "model->data(idx).toInt()" << model->data(idx).toInt();
            }
            qDebug() << "rowCount" << model->rowCount();
            // insert a row in the model with defautl values
            model->insertRow(model->rowCount());
            row = model->rowCount()-1;
        } else {
            for (int j=0; j < model->rowCount(); ++j) {
                idx = model->index(j, Form::EpisodeModel::DataRepresentation::IsValid);
                qDebug() << "model->data(idx).toInt()" << model->data(idx).toInt();
                if (model->data(idx).toInt() == 1) {
                        row = j;
                        break;
                }
            }
        }
        if (!formManager().form(m_formUuid)->isUniqueEpisode()) {
            qDebug() << "inside NOT unique episode";
            model->insertRow(model->rowCount());
            row = model->rowCount()-1;
        }
        qDebug() << "row = " << row;
            // set the values related to the episode (if the user set them)
            // UserDateTime
            idx = model->index(row, Form::EpisodeModel::DataRepresentation::UserDateTime);
        QString isoUtcDateTimeString;
        if (!(m_episodeUserDateTimeComboBox->currentIndex() == -1)) {
            qDebug() << "index: " << m_episodeUserDateTimeComboBox->currentIndex() ;
            QString isoDate = m_data->at(i).at(m_episodeUserDateTimeComboBox->currentIndex());
            qDebug() << "isoDate: " << isoDate;
            QDateTime isoDateTime = QDateTime::fromString(isoDate, "yyyy-MM-dd");
            qDebug() << "isoDateTime: " << isoDateTime;
            isoUtcDateTimeString = isoDateTime.toUTC().toString(Qt::ISODate);
        } else if (m_episodeUserDateTimeDateTimeEdit->dateTime().isValid()) {
            isoUtcDateTimeString = m_episodeUserDateTimeDateTimeEdit->dateTime().toUTC().toString(Qt::ISODate);
        }
        qDebug() << "isoUtcDateTimeString: " << isoUtcDateTimeString;
        model->setData(idx, isoUtcDateTimeString);

        // Label
        idx = model->index(row, Form::EpisodeModel::DataRepresentation::Label);
        QString label = m_episodeLabelLineEdit->text();
        model->setData(idx, label);

        // xml
        QString xml = xmlEpisode(i);
        qDebug() << "xml: " << xml;
        idx = model->index(row, Form::EpisodeModel::DataRepresentation::XmlContent);
        model->setData(idx, xml);
        qDebug() << "rowCount" << model->rowCount();
        model->submit();
        qDebug() << "rowCount" << model->rowCount();
        model->setReadOnly(true);
    }
}

int RecordImportDialog::selectForm()
{
    qDebug() << Q_FUNC_INFO;
    QDialog *dialog = new QDialog(this);
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                       | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted,
            dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected,
            dialog, &QDialog::reject);
    m_selector = new Form::FormFilesSelectorWidget(this);
    m_selector->setFormType(Form::FormFilesSelectorWidget::SubForms);
    m_selector->expandAllItems();
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_selector);
    layout->addWidget(buttonBox);
    dialog->setLayout(layout);
    if (dialog->exec()) {
        QList<Form::FormIODescription *> sel = m_selector->selectedForms();
        if (!sel.isEmpty()) {
            Form::FormIODescription *selectedForm = sel.at(0);
            m_selectedForm = selectedForm;
            m_uuid = selectedForm->data(Form::FormIODescription::UuidOrAbsPath).toString();
            d->ui->formLabel->setText(m_uuid);
            m_formMain = formManager().form(m_uuid);
            QList<Form::FormMain*> formList = m_formMain->flattenedFormMainChildren();
            QString uuid;
            if (!formList.isEmpty()) {
                uuid = formList.at(0)->uuid();
                m_formUuid = uuid;
            } else {
                uuid = QString("No uuid found. Check your form code.");
            }
            d->ui->uuidLabel->setText(uuid);
        }
        return 1;
    }
    return 0;
}

QString RecordImportDialog::xmlEpisode(int &i)
{
    for (int j = 0; j < m_formItemList.size(); ++j) {
        qDebug() << m_formItemList.at(j);
        int idx = m_comboFormList.at(j)->currentIndex();
        if (!(idx == -1)) {
            qDebug() << "combo idx = " << idx;
            qDebug() << "value for combo idx: " << m_data->at(i).at(idx);
            m_formItemHash->insert(m_formItemList.at(j)->uuid(),
                               m_data->at(i).at(idx));
        }
    }
    m_formMain = formManager().form(m_uuid);
    QHash<QString, QString> xmlData;
    foreach(Form::FormItem *it, m_formMain->flattenedFormItemChildren()) {
        if (it->itemData()) {
            xmlData.insert(it->uuid(), m_formItemHash->value(it->uuid()));
        }
    }
    return Utils::createXml(Form::Constants::XML_FORM_GENERAL_TAG, xmlData, 2, false);
}
