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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "patientcreatorwizard.h"
#include "identitywidget.h"
#include "patientmodel.h"
#include "constants_settings.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/isettings.h>

#include <utils/global.h>
#include <utils/log.h>

#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QGridLayout>

using namespace Patients;

static inline Core::ITheme *theme() { return Core::ICore::instance()->theme(); }
static inline Core::IPatient *patient() { return Core::ICore::instance()->patient(); }
static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }


PatientCreatorWizard::PatientCreatorWizard(QWidget *parent) :
        QWizard(parent)
{
    m_Page = new IdentityPage(this);
    addPage(m_Page);
    setWindowTitle(tr("New Patient"));
    QList<QWizard::WizardButton> layout;
    layout << QWizard::CancelButton << QWizard::Stretch << QWizard::BackButton
            << QWizard::NextButton << QWizard::FinishButton;
    setButtonLayout(layout);
    QPixmap pix = theme()->splashScreenPixmap("newpatient-wizard.png");
    setPixmap(QWizard::BackgroundPixmap, pix);
    setPixmap(QWizard::WatermarkPixmap, pix);
}

void PatientCreatorWizard::done(int r)
{
    if (r == QDialog::Rejected) {
        m_Saved = false;
        bool yes = Utils::yesNoMessageBox(tr("WARNING! You did not save this patient. "
                                             "If you continue without saving, all changes will be lost."),
                               tr("Do you really want to close this dialog?"),
                               "", tr("Patient not saved"));
        if (yes) {
            QDialog::done(r);
            if (Patients::PatientModel::activeModel())
                Patients::PatientModel::activeModel()->refreshModel();
        }
    } else if (r==QDialog::Accepted) {
        if (!validateCurrentPage())
            return;
        if (settings()->value(Constants::S_PATIENTCHANGEONCREATION).toBool()) {
            Patients::PatientModel *m = Patients::PatientModel::activeModel();
            if (m) {
                QString uid = m_Page->lastInsertedUuid();
                m->setFilter("", "", uid, Patients::PatientModel::FilterOnUuid);
                m->setCurrentPatient(m->index(0,0));
            }
        }
        QDialog::done(r);
    }
}

IdentityPage::IdentityPage(QWidget *parent) :
    QWizardPage(parent)
{
    setObjectName("IdentityPage");
    setTitle(tr("Please enter the patient's identity."));
    m_Identity = new IdentityWidget(this, IdentityWidget::ReadWriteMode);
    m_Model = new PatientModel(this);
    m_Model->setFilter("", "", "__", PatientModel::FilterOnUuid);
    m_Model->emitPatientCreationOnSubmit(true);
    m_Model->insertRow(0);
    m_uuid = m_Model->index(0, Core::IPatient::Uid).data().toString();

    m_Identity->setCurrentPatientModel(m_Model);
    m_Identity->setCurrentIndex(m_Model->index(0,0));

    QGridLayout *layout = new QGridLayout;
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(m_Identity, 0, 0);
    setLayout(layout);
}

bool IdentityPage::validatePage()
{
    if (!m_Identity->isIdentityValid())
        return false;
    bool ok = true;
    connect(m_Model, SIGNAL(patientCreated(QString)), Patients::PatientModel::activeModel(), SIGNAL(patientCreated(QString)));
    if (m_Identity->submit()) {
        Patients::PatientModel::activeModel()->refreshModel();
        LOG("Patient successfully created");
    } else {
        LOG_ERROR("Unable to create patient. IdentityPage::validatePage()");
        ok = false;
    }
    return ok;
}
