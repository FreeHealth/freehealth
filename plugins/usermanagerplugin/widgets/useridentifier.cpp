/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
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
 \class UserIdentifier
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

#include <utils/widgets/lineeditechoswitcher.h>

#include <usermanagerplugin/global.h>
#include <usermanagerplugin/usermodel.h>
#include <usermanagerplugin/database/userbase.h>

#include <QApplication>
#include <QSplashScreen>

#include "ui_useridentifier.h"

using namespace UserPlugin;
using namespace UserPlugin::Internal;

UserIdentifier::UserIdentifier(const QStringList &informations, QWidget *parent) :
    QDialog(parent)
{
    // initialization
    setObjectName("UserIdentifier");
    Core::ICore::instance()->splashScreen()->hide();
    m_ui = new Ui::UserIdentifier();
    m_ui->setupUi(this);
    m_ui->lblAppName->setPixmap(Core::ICore::instance()->theme()->splashScreen(Core::Constants::FREEMEDFORMS_SPLASHSCREEN).scaled(QSize(400,200),Qt::KeepAspectRatio));
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
            Core::ICore::instance()->splashScreen()->show();
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
