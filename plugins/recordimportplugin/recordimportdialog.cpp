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
#include "utils/genericdescription.h"
#include "utils/widgets/genericdescriptioneditor.h"

#include <QFormLayout>
#include <QCheckBox>

using namespace RecordImport;
using namespace Internal;

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Form::FormManager &formManager() {return Form::FormCore::instance().formManager();}
static inline Form::EpisodeManager &episodeManager() {return Form::FormCore::instance().episodeManager();}

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
    QGridLayout *matchPatientGridLayout = new QGridLayout;
    //QWidget *matchPatientWidget = new QWidget;
    QStringList patientLabels = QStringList();
    patientLabels << RecordImport::Constants::PATIENT_IDENTITY_USUALNAME
                  << RecordImport::Constants::PATIENT_IDENTITY_FIRSTNAME
                  << RecordImport::Constants::PATIENT_IDENTITY_DOB;
    for (int i = 0; i < patientLabels.size(); ++i) {
        QString label = patientLabels.at(i);
        matchPatientGridLayout->addWidget(new QLabel(label), i, 0);
        QComboBox *combo = new QComboBox;
        combo->setObjectName(label);
        combo->addItems(m_field);
        combo->setCurrentIndex(-1);
        matchPatientGridLayout->addWidget(combo, i, 1);

    }
    d->ui->patientGroupBox->setLayout(matchPatientGridLayout);
}

void RecordImportDialog::matchEpisodeWidget() {
        QGridLayout *matchEpisodeGridLayout = new QGridLayout;
        QWidget *matchEpisodeWidget = new QWidget;
        QStringList episodeItems;
        episodeItems << "UserDateTime"
                     << "Label"
                     << "CreationDateTime"
                     << "Priority"
                     << "UserCreatorName";
        for (int j = 0; j < episodeItems.size(); ++j) {
            QString label = m_data->at(0).at(j);
            matchEpisodeGridLayout->addWidget(new QLabel (episodeItems.at(j)), j, 0);
            QComboBox *combo = new QComboBox;
            combo->setObjectName(label);
            combo->addItems(m_field);
            combo->setCurrentIndex(-1);
            matchEpisodeGridLayout->addWidget(combo, j, 1);
        }
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
        for (int j = 0; j < m_formItemList.size(); ++j) {
            QString label = m_formItemList.at(j)->spec()->label();
            QString labelPlugin = m_formItemList.at(j)->spec()->pluginName();
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
    d->ui->startPushButton->setEnabled(false);
    //auto data_uptr = ProcessRecordImport::parse(d->fileName(), d->fieldSeparator(), d->recordSeparator());
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
        }
        return 1;
    }
    return 0;
}
