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

/**
 * \class UserPlugin::Internal::UserPasswordDialog
 * \brief Dialog for password changing.
 * With this dialog, user can change its password. He's asked of the actual
 * password once, and of the new password twice. When user accept the dialog
 * a verification is done, no changes are saved into database or users' model. \n
 * - canGetNewPassword() return the verification state. If it's true, all
 * is good: old password was verified, and new password was confirmed correctly.
 * - cryptedPassword() return the crypted new password to use.
 * - clearPassword() return the new password (non crypted).
 * - applyChanges() call it after dialog validation to submit the new password
 * to the database and server.
 * You have to send the new password to the user model by yourself.
*/

#include "userpassworddialog.h"

#include <usermanagerplugin/usercore.h>
#include <usermanagerplugin/usermodel.h>
#include <usermanagerplugin/database/userbase.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/widgets/lineeditechoswitcher.h>
#include <translationutils/constanttranslations.h>

#include "ui_userpassworddialog.h"

#include <QDebug>

using namespace UserPlugin;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline UserPlugin::UserCore &userCore() {return UserPlugin::UserCore::instance();}
static inline UserPlugin::Internal::UserBase *userBase() {return userCore().userBase();}

UserPasswordDialog::UserPasswordDialog(const QString &actualCryptedPassword, QWidget *parent) :
    QDialog(parent)
{
   Q_ASSERT_X(!actualCryptedPassword.isEmpty(), "UserPasswordDialog", "actualCryptedPassword cannot be empty.");
   if (actualCryptedPassword.isEmpty())
       return;
   m_ui = new Internal::Ui::UserPasswordDialog();
   m_ui->setupUi(this);

   changeTitle(Trans::Constants::CHANGE_PASSWORD);

   m_ActualPass = actualCryptedPassword;
   m_ui->newPass->toogleEchoMode();
   m_ui->newControl->toogleEchoMode();
   m_ui->oldPass->toogleEchoMode();
   m_AllIsGood = false;

   // connect buttons
   connect(m_ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
   m_ui->oldPass->setFocus();

   connect(m_ui->newControl, SIGNAL(textChanged(QString)), this, SLOT(checkControlPassword(QString)));
   connect(m_ui->newPass, SIGNAL(textChanged(QString)), this, SLOT(checkNewPassword(QString)));
   checkNewPassword("");
}

void UserPasswordDialog::changeTitle(const QString &title)
{
    m_ui->label->setText(title);
}

void UserPasswordDialog::checkControlPassword(const QString &text)
{
    if (text==m_ui->newPass->text()) {
        m_ui->labelControlPassword->setStyleSheet("color:black");
    } else {
        m_ui->labelControlPassword->setStyleSheet("color:red");
    }
}

void UserPasswordDialog::checkNewPassword(const QString &text)
{
    if (text.size() >= 5) {
        m_ui->labelNewPassword->setStyleSheet("color:black");
        m_ui->labelNewPassword->setToolTip("");
        m_ui->newPass->setToolTip("");
    } else {
        m_ui->labelNewPassword->setStyleSheet("color:red");
        m_ui->labelNewPassword->setToolTip(tr("Password must have at least 5 chars."));
        m_ui->newPass->setToolTip(tr("Password must have at least 5 chars."));
    }
    checkControlPassword(m_ui->newControl->text());
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
        return m_ui->newPass->text();
    return QString();
}

bool UserPasswordDialog::applyChanges(UserModel *model, int userRow) const
{
    Q_ASSERT(m_AllIsGood);
    if (!m_AllIsGood) {
        LOG_ERROR("Dialog must be validated before");
        return false;
    }
    return model->setData(model->index(userRow, Core::IUser::ClearPassword), clearPassword());
}

void UserPasswordDialog::accept()
{
    if (m_ui->newPass->text().size()<5) {
        return;
    }
    const QString &cryptedNewPass = Utils::cryptPassword(m_ui->newPass->text());
    const QString &oldPass = Utils::cryptPassword(m_ui->oldPass->text());

    if ((oldPass == m_ActualPass) &&
        (m_ui->newPass->text() == m_ui->newControl->text())) {
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
