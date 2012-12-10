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

#include "bugreportdialog.h"
#include "ui_bugreportdialog.h"

#include <QPushButton>

#include <coreplugin/iuser.h>
#include <coreplugin/icore.h>
#include <utils/emailvalidator.h>

static inline Core::IUser *user() { return Core::ICore::instance()->user(); }

BugReportDialog::BugReportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BugReportDialog)
{
    ui->setupUi(this);
    ui->emailEdit->setValidator(new EmailValidator(this));

    ui->gmailAccountCheck->setVisible(false);

    m_sendReportButton = new QPushButton(tr("&Send report"), this);
    m_sendReportButton->setDisabled(true);
    ui->buttonBox->addButton(m_sendReportButton, QDialogButtonBox::ActionRole);
    connect(m_sendReportButton, SIGNAL(clicked()), this, SLOT(sendBugReport()));

    // if current user has a valid email address, use it
    QString email = user()->value(Core::IUser::Mail).toString();
    int pos = 0;
    if (ui->emailEdit->validator()->validate(email, pos) == QValidator::Acceptable)
        ui->emailEdit->setText(email);
}

BugReportDialog::~BugReportDialog()
{
    delete ui;
}

void BugReportDialog::on_emailEdit_textChanged(const QString &text)
{
    // use GMail if it appears to be a valid gmail address
    const bool isValidGMailAddress = text.endsWith("gmail.com");
    const bool isValidEmailAddress = ui->emailEdit->hasAcceptableInput();

    ui->gmailAccountCheck->setVisible(isValidEmailAddress && isValidGMailAddress);
    ui->bugReportEdit->setEnabled(isValidEmailAddress);
    m_sendReportButton->setEnabled(isValidEmailAddress);
}

void BugReportDialog::sendBugReport()
{
}

void BugReportDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
