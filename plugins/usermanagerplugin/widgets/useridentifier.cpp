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
 * \class UserPlugin::Internal::UserIdentifier
 * This class is a dialog that asks the user for her/his login/password, with a limited number of tries.
 * You can show some information on the left using the first parameter of the constructor.
 * If there is no information to show, the information's groupBox is hidden.
 *
 * The dialog does not need the UserPlugin::Internal::UserBase to be initialized
 * (UserPlugin::Internal::UserBase::initialize()) to check the login informations.
 *
 * When the user is successfully identified on the server and/or the database:
 *   - the UserPlugin::Internal::UserBase is initialized,
 *   - the dialog result() is set to QDialog::Accepted,
 * You still need to set the current user into user's model.
 *
 * In the other case, the dialog returns \e QDialog::Rejected.
 *
 * You can retrieve login(), login64crypt(), password() and cryptedPassword().
*/

#include "useridentifier.h"

#include <utils/global.h>
#include <utils/log.h>
#include <utils/databaseconnector.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <utils/passwordandlogin.h>
#include <utils/widgets/loginwidget.h>

#include <usermanagerplugin/usercore.h>
#include <usermanagerplugin/database/userbase.h>

#include <QApplication>
#include <QSplashScreen>
#include <QClipboard>
#include <QDesktopWidget>

#include "ui_useridentifier.h"

using namespace UserPlugin;
using namespace UserPlugin::Internal;

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline UserPlugin::UserCore &userCore() {return UserPlugin::UserCore::instance();}
static inline UserPlugin::Internal::UserBase *userBase() {return userCore().userBase();}

UserIdentifier::UserIdentifier(QWidget *parent) :
        QDialog(parent)
{
    // initialization
    setObjectName("UserIdentifier");
    if (theme()->splashScreen())
        theme()->splashScreen()->hide();
    m_ui = new Ui::UserIdentifier();
    m_ui->setupUi(this);
    m_ui->loginWidget->setToggleViewIcon(theme()->iconFullPath(Core::Constants::ICONEYES));
    m_ui->loginWidget->togglePasswordEcho(false);

    // Get splash & resize
    QPixmap splash = theme()->splashScreenPixmap(settings()->path(Core::ISettings::SplashScreen));
    if (splash.size().width() > 300)
        splash = splash.scaledToWidth(300);
    m_ui->lblAppName->setMinimumSize(splash.size() + QSize(10,10));
    m_ui->lblAppName->setPixmap(splash);
    m_NumberOfTries = 0;
    setWindowTitle(qApp->applicationName());
    if (userBase()->isNewlyCreated()) {
        m_ui->newlyMessage->show();
    } else {
        m_ui->newlyMessage->hide();
    }
    m_ui->loginWidget->focusLogin();

    // Server or local
    if (settings()->databaseConnector().driver()==Utils::Database::MySQL) {
        m_ui->groupServer->show();
        m_ui->host->setText(settings()->databaseConnector().host());
        m_ui->port->setValue(settings()->databaseConnector().port());
    } else {
        m_ui->groupServer->hide();
    }

    // Clear the clipboard (security improvement)
    qApp->clipboard()->clear(QClipboard::Clipboard);
    qApp->clipboard()->clear(QClipboard::FindBuffer);
    qApp->clipboard()->clear(QClipboard::Selection);

    adjustSize();
    layout()->setSizeConstraint(QLayout::SetFixedSize);
    Utils::centerWidget(this, qApp->desktop());
}

void UserIdentifier::done(int result)
{
    if (result == QDialog::Accepted) {
        // Query database with login/password couple
        if (!userBase()->checkLogin(login(), password())) {
            m_NumberOfTries++;
            if (m_NumberOfTries == MaxNumberOfTries) {
                QDialog::done(QDialog::Rejected);
            } else {
                Utils::warningMessageBox(tr("Incorrect login/password information."),
                                         tr("You can try %1 more time(s).")
                                         .arg(MaxNumberOfTries - m_NumberOfTries - 1),
                                         "",qApp->applicationName());
            }
        } else {
            LOG(tr("User can be identified."));
            if (theme()->splashScreen())
                theme()->splashScreen()->show();
            QDialog::done(QDialog::Accepted);
        }
    } else if (result == QDialog::Rejected) {
        LOG(tr("User is not identified."));
        QDialog::done(QDialog::Rejected);
    }
}

/** Plain text login. */
QString UserIdentifier::login() const
{
    return m_ui->loginWidget->login();
}

/** FreeMedForms encrypted login. */
QString UserIdentifier::login64crypt() const
{
    return Utils::loginForSQL(m_ui->loginWidget->login());
}

/** Plain text password. */
QString UserIdentifier::password() const
{
    return m_ui->loginWidget->password();
}

/** FreeMedForms encrypted password (the encryption algorithm is destructive). */
QString UserIdentifier::cryptedPassword() const
{
    Utils::PasswordCrypter crypter;
    return crypter.cryptPassword(m_ui->loginWidget->password());
}
