/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "patientcreatorwizard.h"
#include "identitywidget.h"
#include "patientmodel.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/ipatient.h>

#include <utils/global.h>
#include <utils/log.h>

#include <QLabel>
#include <QLineEdit>
#include <QDateEdit>
#include <QComboBox>
#include <QGridLayout>

using namespace Patients;

static inline Core::ITheme *theme() { return Core::ICore::instance()->theme(); }


PatientCreatorWizard::PatientCreatorWizard(QWidget *parent) :
        QWizard(parent)
{
    addPage(new IdentityPage(this));
    setWindowTitle(tr("New Patient"));
    QList<QWizard::WizardButton> layout;
    layout << QWizard::CancelButton << QWizard::Stretch << QWizard::BackButton
            << QWizard::NextButton << QWizard::FinishButton;
    setButtonLayout(layout);
    setPixmap(QWizard::WatermarkPixmap, theme()->icon(Core::Constants::ICONPATIENTS).pixmap(QSize(64,64)));
    setPixmap(QWizard::BackgroundPixmap, theme()->icon(Core::Constants::ICONPATIENTS).pixmap(QSize(64,64)));
    setPixmap(QWizard::BannerPixmap, theme()->icon(Core::Constants::ICONPATIENTS).pixmap(QSize(64,64)));
    setPixmap(QWizard::LogoPixmap, theme()->icon(Core::Constants::ICONPATIENTS).pixmap(QSize(64,64)));
}

void PatientCreatorWizard::done(int r)
{
    if (r == QDialog::Rejected) {
        m_Saved = false;
        bool yes = Utils::yesNoMessageBox(tr("WARNING ! You don't saved this patient."),
                               tr("If you continue changes will be lost.\n"
                                  "Do you really want to close this dialog ?"),
                               "", tr("Patient not saved"));
        if (yes) {
            QDialog::done(r);
            if (Patients::PatientModel::activeModel())
                Patients::PatientModel::activeModel()->refreshModel();
        }
    } else if (r==QDialog::Accepted) {
        if (!validateCurrentPage())
            return;

        QDialog::done(r);
        if (Patients::PatientModel::activeModel())
            Patients::PatientModel::activeModel()->refreshModel();
    }
}

IdentityPage::IdentityPage(QWidget *parent)
    : QWizardPage(parent)
{
    setObjectName("IdentityPage");
    setTitle(tr("Please enter the patient's identity."));
    m_Identity = new IdentityWidget(this, IdentityWidget::ReadWriteMode);
    m_Model = new PatientModel(this);
    m_Model->setFilter("", "", "", PatientModel::FilterOnUuid);
    m_Model->insertRow(0);
    QString uuid = m_Model->index(0, Core::IPatient::Uid).data().toString();

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
    if (m_Identity->submit()) {
        Utils::Log::addMessage(this, "Patient correctly created");
    } else {
        Utils::Log::addMessage(this, "Unable to create patient. IdentityPage::validatePage()");
        ok = false;
    }
    return ok;
}
