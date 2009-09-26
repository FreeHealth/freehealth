/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
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

/**
  \class UserPasswordDialog
  \brief Dialog for password changing.
  With this dialog, user can change its password. He's asked of the actual password once, and of the new password
  twice. When user accept the dialog a verification is done, no changes are saved into database or users' model. \n
  \li canGetNewPassword() return the verification state. If it's true, all is good : old password was verified, and
  new password was correctly confirmed.
  \li cryptedPassword() return the crypted new password to use.
  \ingroup usertoolkit widget_usertoolkit usermanager
*/

#include "userpassworddialog.h"

#include <utils/global.h>
#include <utils/widgets/lineeditechoswitcher.h>

#include <usermanagerplugin/global.h>

#include "ui_userpassworddialog.h"

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
}

/** \brief Return the state of verification. Verification is done when user accepts the dialog. */
bool UserPasswordDialog::canGetNewPassword()
{
    return m_AllIsGood;
}

/** \brief Returns the crypted password */
QString UserPasswordDialog::cryptedPassword()
{
    if (m_AllIsGood)
        return m_CryptedNewPass;
    return QString::null;
}

void UserPasswordDialog::accept()
{
    const QString &cryptedNewPass = UserPlugin::crypt(m_ui->newPass->lineEdit()->text());
    const QString &oldPass = UserPlugin::crypt(m_ui->oldPass->lineEdit()->text());
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
