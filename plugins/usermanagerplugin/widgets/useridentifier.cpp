/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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

/**
 \class UserPlugin::Internal::UserIdentifier
  \brief This class is a dialog that ask user for is login/password, with a limited number of tries.
  You can show some informations on the left using the first parameter of the constructor.
  If there is not informations to show, the informations' groupBox is hidden.

  When the identification is good :
  - the dialog result() is setted to QDialog::Accepted,
  - current user is setted into user's model with the login and password of this dialog,
  - the lastLogin information is saved into database,
  - the login history is completed.

  In the other case, it is setted to QDialog::Rejected.

  You can retreive cryptedLogin and cryptedPassword using : login() and cryptedPassword().
  \ingroup usertoolkit widget_usertoolkit
  \ingroup usermanager
*/

#include "useridentifier.h"

#include <utils/global.h>
#include <utils/log.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <utils/widgets/lineeditechoswitcher.h>

#include <usermanagerplugin/global.h>
#include <usermanagerplugin/usermodel.h>
#include <usermanagerplugin/database/userbase.h>

#include <QApplication>
#include <QSplashScreen>

#include "ui_useridentifier.h"

using namespace UserPlugin;
using namespace UserPlugin::Internal;

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }

UserIdentifier::UserIdentifier(const QStringList &informations, QWidget *parent) :
    QDialog(parent)
{
    // initialization
    setObjectName("UserIdentifier");
    if (theme()->splashScreen())
        theme()->splashScreen()->hide();
    m_ui = new Ui::UserIdentifier();
    m_ui->setupUi(this);
    m_ui->login->setIcon(theme()->icon(Core::Constants::ICONEYES));
    m_ui->password->setIcon(theme()->icon(Core::Constants::ICONEYES));
    QPixmap splash = theme()->splashScreenPixmap(settings()->path(Core::ISettings::Splashscreen));
    if (splash.size().width() > 400 || splash.size().height() >200) {
        splash = splash.scaled(QSize(400,200),Qt::KeepAspectRatio);
    }
    m_ui->lblAppName->setPixmap(splash);
    m_NumberOfTries = 0;
    setWindowTitle(qApp->applicationName());
    if (!parent)
        Utils::centerWidget(this);
    if (informations.count()) {
        foreach(const QString &s, informations)
            new QListWidgetItem(s , m_ui->informationsWidget);
    } else {
        m_ui->groupInformations->hide();
    }
    if (Internal::UserBase::instance()->isNewlyCreated()) {
        m_ui->newlyMessage->show();
    } else {
        m_ui->newlyMessage->hide();
    }

//    m_ui->lblAppName->setText(qApp->applicationName());
    m_ui->password->toogleEchoMode();
    m_ui->login->lineEdit()->setFocus();
}

void UserIdentifier::done(int result)
{
    UserModel *m = UserModel::instance();
    if (result == QDialog::Accepted) {
        // ask database with login/password couple
        if (!m->isCorrectLogin(login(), cryptedPassword())) {
            m_NumberOfTries++;
            if (m_NumberOfTries == MaxNumberOfTries)
		QDialog::done(QDialog::Rejected);
            else {
                Utils::warningMessageBox(tr("Incorrect login/password informations."),
                                         tr("You can try %1 more time(s).")
                                         .arg(MaxNumberOfTries - m_NumberOfTries),"",qApp->applicationName());
            }
        } else {
            Utils::Log::addMessage(this, tr("User is identified."));
	    m->setCurrentUser(login(), cryptedPassword());
            if (theme()->splashScreen())
                theme()->splashScreen()->show();
            QDialog::done(QDialog::Accepted);
        }
    }
    else if (result == QDialog::Rejected) {
        Utils::Log::addMessage(this, tr("User is not identified."));
	QDialog::done(QDialog::Rejected);
    }
}

QString UserIdentifier::cryptedPassword()
{
    return crypt(m_ui->password->lineEdit()->text());
}

QString UserIdentifier::login()
{
    return loginForSQL(m_ui->login->lineEdit()->text());
}
