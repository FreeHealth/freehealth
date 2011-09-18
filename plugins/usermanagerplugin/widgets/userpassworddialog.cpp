/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/

/**
  \class UserPlugin::UserPasswordDialog
  \brief Dialog for password changing.
  With this dialog, user can change its password. He's asked of the actual password once, and of the new password
  twice. When user accept the dialog a verification is done, no changes are saved into database or users' model. \n
  - canGetNewPassword() return the verification state. If it's true, all is good : old password was verified, and
  new password was correctly confirmed.
  - cryptedPassword() return the crypted new password to use.
  You have to send the new password to the user model by yourself.
*/

#include "userpassworddialog.h"

#include <utils/global.h>
#include <utils/widgets/lineeditechoswitcher.h>

//#include <usermanagerplugin/global.h>

#include "ui_userpassworddialog.h"

#include <QDebug>

using namespace UserPlugin;

UserPasswordDialog::UserPasswordDialog(const QString &actualCryptedPassword, QWidget *parent) :
    QDialog(parent)
{
   Q_ASSERT_X(!actualCryptedPassword.isEmpty(), "UserPasswordDialog", "actualCryptedPassword cannot be empty.");
   if (actualCryptedPassword.isEmpty())
       return;
   m_ui = new Internal::Ui::UserPasswordDialog();
   m_ui->setupUi(this);
   m_ActualPass = actualCryptedPassword;
   m_ui->newPass->toogleEchoMode();
   m_ui->newControl->toogleEchoMode();
   m_ui->oldPass->toogleEchoMode();
   m_AllIsGood = false;
   // connect buttons
   connect(m_ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
   m_ui->oldPass->lineEdit()->setFocus();
}

void UserPasswordDialog::changeTitle(const QString &title)
{
    m_ui->label->setText(title);
}

/** \brief Return the state of verification. Verification is done when user accepts the dialog. */
bool UserPasswordDialog::canGetNewPassword() const
{
    return m_AllIsGood;
}

/** Returns the crypted new password. The dialog must be accepted before. */
QString UserPasswordDialog::cryptedPassword() const
{
    if (m_AllIsGood)
        return m_CryptedNewPass;
    return QString();
}

/** Returns the clear new password. The dialog must be accepted before. */
QString UserPasswordDialog::clearPassword() const
{
    if (m_AllIsGood)
        return m_ui->newPass->lineEdit()->text();
    return QString();
}

void UserPasswordDialog::accept()
{
    const QString &cryptedNewPass = Utils::cryptPassword(m_ui->newPass->lineEdit()->text());
    const QString &oldPass = Utils::cryptPassword(m_ui->oldPass->lineEdit()->text());

    if ((oldPass == m_ActualPass) &&
        (m_ui->newPass->lineEdit()->text() == m_ui->newControl->lineEdit()->text())) {
        m_AllIsGood = true;
        m_CryptedNewPass = cryptedNewPass;
        QDialog::accept();
    } else {
        m_AllIsGood = false;
        QString info;
        if (oldPass != m_ActualPass)
             info = tr("The old password is not correct. Please retry with the correct password.");
        else
            info = tr("Wrong password confirmation.");
        Utils::warningMessageBox(tr("Password can not be change."),
                                 info, "", windowTitle());
        QDialog::reject();
    }
}
