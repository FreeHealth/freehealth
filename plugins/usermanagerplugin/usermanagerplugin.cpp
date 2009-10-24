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
#include "usermanagerplugin.h"
#include "usermodel.h"
#include "widgets/usermanager.h"
#include "widgets/useridentifier.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/contextmanager/contextmanager.h>

#include <translationutils/constanttranslations.h>
#include <utils/log.h>
#include <utils/global.h>

#include <QtCore/QtPlugin>
#include <QDebug>

using namespace UserPlugin;
using namespace Trans::ConstantTranslations;

static inline Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline UserPlugin::UserModel *userModel() {return UserPlugin::UserModel::instance();}
static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }

static inline bool identifyUser()
{
    // instanciate user model
    userModel();
    QString log = settings()->value(Core::Constants::S_LASTLOGIN).toString();
    QString pass = settings()->value(Core::Constants::S_LASTPASSWORD).toString();
    while (true) {
        if (userModel()->isCorrectLogin(log, pass)) {
            userModel()->setCurrentUser(log, pass);
            break;
        } else {
            log.clear();
            pass.clear();
            Internal::UserIdentifier ident;
            if (ident.exec() == QDialog::Rejected)
                return false;
            log = ident.login();
            pass = ident.cryptedPassword();
            settings()->setValue(Core::Constants::S_LASTLOGIN, log);
            settings()->setValue(Core::Constants::S_LASTPASSWORD, pass);
        }
    }
    // messageBox : Welcome User
    Utils::informativeMessageBox(tkTr(Trans::Constants::CONNECTED_AS_1)
                                 .arg(userModel()->currentUserData(User::Name).toString()),
                                 tkTr(Trans::Constants::WELCOME_USER));

    return true;
}


UserManagerPlugin::UserManagerPlugin() : m_UserManager(0)
{
	qWarning() << "creating UserManagerPlugin";
}

UserManagerPlugin::~UserManagerPlugin()
{
}

bool UserManagerPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "UserManagerPlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

    // Ask for User login
    if (!identifyUser())
        return false;

    return true;
}

void UserManagerPlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "UserManagerPlugin::extensionsInitialized";

    // add UserManager toogler action to plugin menu
    Core::ActionContainer *menu = actionManager()->actionContainer(Core::Constants::M_PLUGINS);
    Q_ASSERT(menu);
    QList<int> ctx = QList<int>() << Core::Constants::C_GLOBAL_ID;
    aUserManager = new QAction(this);
    aUserManager->setIcon(QIcon(Core::Constants::ICONUSERMANAGER));
    Core::Command *cmd = actionManager()->registerAction(aUserManager, Core::Constants::A_USERMANAGER, ctx);
    Q_ASSERT(cmd);
//    cmd->setDefaultKeySequence(QKeySequence::New);
    cmd->setTranslations(Trans::Constants::USERMANAGER_TEXT);
    menu->addAction(cmd,Core::Constants::G_PLUGINS_USERMANAGER);
    cmd->retranslate();
    connect(aUserManager, SIGNAL(triggered()), this, SLOT(showUserManager()));
    // Update context is necessary
    contextManager()->updateContext();

}

void UserManagerPlugin::showUserManager()
{
    if (m_UserManager) {
        m_UserManager->show();
    } else {
        m_UserManager = new UserManager();
        m_UserManager->initialize();
        m_UserManager->show();
    }
}


Q_EXPORT_PLUGIN(UserManagerPlugin)
