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
#include "usermanagerplugin.h"
#include "usermodel.h"
#include "widgets/usermanager.h"
#include "widgets/useridentifier.h"
#include "widgets/userwizard.h"
#include "currentuserpreferencespage.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants.h>
#include <coreplugin/translators.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/iuser.h>

#include <translationutils/constanttranslations.h>
#include <utils/log.h>
#include <utils/global.h>

#include <extensionsystem/pluginmanager.h>

#include <QtCore/QtPlugin>
#include <QApplication>
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
    bool ask = true;
    while (true) {
        if (userModel()->isCorrectLogin(log, pass)) {
            userModel()->setCurrentUser(log, pass);
            if (ask) {
                int r = Utils::withButtonsMessageBox(tkTr(Trans::Constants::CONNECTED_AS_1)
                                                     .arg(userModel()->currentUserData(Core::IUser::FullName).toString()),
                                                     QApplication::translate("UserManagerPlugin", "You can pursue with this user or connect with another one."),
                                                     "", QStringList()
                                                     << QApplication::translate("UserManagerPlugin", "Stay connected")
                                                     << QApplication::translate("UserManagerPlugin", "Change the current user"));
                if (r==1) {
                    log.clear();
                    pass.clear();
                    userModel()->clear();
                    ask = false;
                    continue;
                }
                break;
            }
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
    return true;
}


UserManagerPlugin::UserManagerPlugin() :
        aUserManager(0), aCreateUser(0), aChangeUser(0), m_UserManager(0)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating UserManagerPlugin";
}

UserManagerPlugin::~UserManagerPlugin()
{
    qWarning() << "UserManagerPlugin::~UserManagerPlugin()";
}

bool UserManagerPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "UserManagerPlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("usermanagerplugin");

    // Ask for User login
    if (!identifyUser()) {
        *errorString = tr("User is not identified.");
        return false;
    }

    return true;
}

void UserManagerPlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "UserManagerPlugin::extensionsInitialized";

    // add UserPreferences page
    addAutoReleasedObject(new UserPlugin::CurrentUserPreferencesPage(this));

    // add UserManager toogler action to plugin menu
#ifdef FREEACCOUNT
    const char * const menuId = Core::Constants::M_FILE;
    const char * const menuNewId = Core::Constants::M_FILE;
    const char * const groupUsers = Core::Constants::G_FILE_OTHER;
    const char * const groupNew =  Core::Constants::G_FILE_NEW;
#else
    const char * const menuId = Core::Constants::M_GENERAL;
    const char * const menuNewId = Core::Constants::M_GENERAL_NEW;
    const char * const groupUsers = Core::Constants::G_GENERAL_USERS;
    const char * const groupNew = Core::Constants::G_GENERAL_NEW;
#endif

    Core::ActionContainer *menu = actionManager()->actionContainer(menuId);
    Q_ASSERT(menu);
    if (!menu)
        return;
    Core::ActionContainer *newmenu = actionManager()->actionContainer(menuNewId);
    Q_ASSERT(newmenu);
    if (!newmenu)
        return;

    QList<int> ctx = QList<int>() << Core::Constants::C_GLOBAL_ID;
    QAction *a = 0;
    Core::Command *cmd = 0;

    // User Manager action
    a = aUserManager = new QAction(this);
    a->setObjectName("aUserManager");
    a->setIcon(QIcon(Core::Constants::ICONUSERMANAGER));
    cmd = actionManager()->registerAction(aUserManager, Core::Constants::A_USERMANAGER, ctx);
    Q_ASSERT(cmd);
    cmd->setTranslations(Trans::Constants::USERMANAGER_TEXT);
    menu->addAction(cmd, Core::Constants::G_GENERAL_USERS);
    cmd->retranslate();
    connect(aUserManager, SIGNAL(triggered()), this, SLOT(showUserManager()));

    // Create user
    a = aCreateUser = new QAction(this);
    a->setObjectName("aCreateUser");
    a->setIcon(QIcon(Core::Constants::ICONNEWUSER));
    cmd = actionManager()->registerAction(aCreateUser, Core::Constants::A_CREATEUSER, ctx);
    Q_ASSERT(cmd);
    cmd->setDefaultKeySequence(QKeySequence(Qt::CTRL + Qt::Key_N, Qt::CTRL + Qt::Key_U));
    cmd->setTranslations(Trans::Constants::USER);
    newmenu->addAction(cmd, groupNew);
    cmd->retranslate();
    connect(aCreateUser, SIGNAL(triggered()), this, SLOT(createUser()));

    // Change current user
    a = aChangeUser = new QAction(this);
    a->setObjectName("aChangeUser");
    a->setIcon(QIcon(Core::Constants::ICONUSER));
    cmd = actionManager()->registerAction(aChangeUser, "aChangeCurrentUser", ctx);
    Q_ASSERT(cmd);
    cmd->setTranslations(tr("Change current user"));
    menu->addAction(cmd, groupNew);
    cmd->retranslate();
    connect(aChangeUser, SIGNAL(triggered()), this, SLOT(changeCurrentUser()));


    // Update context is necessary
//    contextManager()->updateContext();

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

void UserManagerPlugin::createUser()
{
    UserWizard wiz;
    wiz.createUser(true);
    wiz.exec();
}

void UserManagerPlugin::changeCurrentUser()
{
    Internal::UserIdentifier ident;
    if (ident.exec() == QDialog::Rejected)
        return;
    QString log = ident.login();
    QString pass = ident.cryptedPassword();
    settings()->setValue(Core::Constants::S_LASTLOGIN, log);
    settings()->setValue(Core::Constants::S_LASTPASSWORD, pass);
    Utils::informativeMessageBox(tkTr(Trans::Constants::CONNECTED_AS_1)
                                 .arg(userModel()->currentUserData(Core::IUser::FullName).toString()),"","","");
}

Q_EXPORT_PLUGIN(UserManagerPlugin)
