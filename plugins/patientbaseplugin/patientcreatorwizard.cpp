/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
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
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
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


PatientCreatorWizard::PatientCreatorWizard(QWidget *parent)
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
            Patients::PatientModel::activeModel()->refreshModel();
        }
    } else if (r==QDialog::Accepted) {
        if (!validateCurrentPage())
            return;

        QDialog::done(r);
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
