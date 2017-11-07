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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/

/**
 * \class Identity::Internal::PasswordDialog
 * \brief Dialog for password changing / setting.
 * With this dialog, users can change their passwords.
 * Current password is typed once, new password is typed twice.
 * When user accepts the dialog a verification is done. \n
 * - canGetNewPassword() returns the verification state. If it's true, all
 * is good: old password was verified, and new password was confirmed correctly.
 * - cryptedPassword() returns the encrypted new password to use.
 * - clearPassword() returns the new password (not encrypted).
*/

#include "passworddialog.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/passwordandlogin.h>
#include <utils/widgets/lineeditechoswitcher.h>
// FIXME: this header should be avoided
#include <translationutils/constanttranslations.h>

#include "ui_passworddialog.h"

#include <QDebug>

using namespace Identity;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}

PasswordDialog::PasswordDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Internal::Ui::PasswordDialog()),
    m_AllIsGood(false)
{
    m_ui->setupUi(this);
    m_ui->newPass->setIcon(theme()->icon(Core::Constants::ICONEYES));
    m_ui->newControl->setIcon(theme()->icon(Core::Constants::ICONEYES));
    m_ui->oldPass->setIcon(theme()->icon(Core::Constants::ICONEYES));
    m_ui->newPass->toogleEchoMode();
    m_ui->newControl->toogleEchoMode();
    m_ui->oldPass->toogleEchoMode();

    // connect buttons
    connect(m_ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    m_ui->oldPass->setFocus();

    // Set title
    m_ui->label->setText(tkTr(Trans::Constants::SET_PASSWORD));
    setWindowTitle(tkTr(Trans::Constants::SET_PASSWORD));

    // Switch to set password only
    m_ui->oldLabel->hide();
    m_ui->oldPass->hide();
    m_ui->newControl->hide();
    m_ui->labelControlPassword->hide();

    connect(m_ui->newControl, SIGNAL(textChanged(QString)), this, SLOT(checkControlPassword(QString)));
    connect(m_ui->newPass, SIGNAL(textChanged(QString)), this, SLOT(checkNewPassword(QString)));
    checkNewPassword("");
}

/** Checks if \e text is equal to UI newPassword lineEdit content */
void PasswordDialog::checkControlPassword(const QString &text)
{
    if (text == m_ui->newPass->text()) {
        m_ui->labelControlPassword->setStyleSheet("color:black");
    } else {
        m_ui->labelControlPassword->setStyleSheet("color:red");
    }
}

/** Checks if \e text content can be used as password. Check password weakness */
void PasswordDialog::checkNewPassword(const QString &text)
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

/** Define the old encrypted password when you want to change the password and not only set the password */
void PasswordDialog::setOldCryptedPassword(const QString &crypted)
{
    m_OldCryptedPass = crypted;
    // Set title
    m_ui->label->setText(tkTr(Trans::Constants::CHANGE_PASSWORD));
    setWindowTitle(tkTr(Trans::Constants::CHANGE_PASSWORD));
    // Manage ui
    m_ui->oldLabel->show();
    m_ui->labelNewPassword->show();
    m_ui->labelControlPassword->show();
    m_ui->oldPass->show();
    m_ui->newPass->show();
    m_ui->newControl->show();
}

/** \brief Return the state of verification. Verification is done when user accepts the dialog. */
bool PasswordDialog::canGetNewPassword() const
{
    return m_AllIsGood;
}

/** Returns the encrypted new password. The dialog must be accepted before. */
QString PasswordDialog::cryptedPassword() const
{
    if (m_AllIsGood)
        return m_CryptedNewPass;
    return QString::null;
}

/** Returns the clear new password. The dialog must be accepted before. */
QString PasswordDialog::uncryptedPassword() const
{
    if (m_AllIsGood)
        return m_ui->newPass->text();
    return QString::null;
}

//bool PasswordDialog::applyChanges(UserModel *model, int userRow) const
//{
//    Q_ASSERT(m_AllIsGood);
//    if (!m_AllIsGood) {
//        LOG_ERROR("Dialog must be validated before");
//        return false;
//    }
//    return model->setData(model->index(userRow, Core::IUser::ClearPassword), clearPassword());
//}

/**
 * If the dialog ends with a QDialog::Accepted, you can get the new password
 * information using the cryptedPassword() or uncryptedPassword().
*/
void PasswordDialog::done(int result)
{
    if (result == Rejected) {
        m_ui->newPass->text().clear();
        QDialog::done(Rejected);
        return;
    }

    if (m_ui->newPass->text().size() < 5)
        return;

    Utils::PasswordCrypter crypter;
    if (m_OldCryptedPass.isEmpty()) {
        // Set password mode
        m_AllIsGood = true;
        m_CryptedNewPass = crypter.cryptPassword(m_ui->newPass->text());
        QDialog::done(result);
    } else {
        // Using PasswordCrypter::checkPassword to check password equivalence
        // instead of a basic string comparison
        // This is du to Qt4 to Qt5 port as we use different hash method with each version
        // See PasswordCrypter::cryptPassword
        bool oldPassCorrect = crypter.checkPassword(m_ui->oldPass->text(), m_OldCryptedPass);
        if (oldPassCorrect &&
                (m_ui->newPass->text() == m_ui->newControl->text())) {
            m_AllIsGood = true;
            m_CryptedNewPass = crypter.cryptPassword(m_ui->newPass->text());
            QDialog::done(result);
        } else {
            m_AllIsGood = false;
            QString info;
            if (oldPassCorrect)
                info = tr("The old password is not correct. Please retry with the correct password.");
            else
                info = tr("Wrong password confirmation.");
            Utils::warningMessageBox(tr("Password cannot be changed."),
                                     info, "", windowTitle());
            QDialog::done(Rejected);
        }
    }
}
