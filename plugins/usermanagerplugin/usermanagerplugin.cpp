/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
  \namespace UserPlugin
  \brief Namespace reserved for the User manager plugin.
*/

/**
  \class UserPlugin::UserManagerPlugin
  \brief Core::IPlugin class for the user plugin. Owns some global actions (create new user,
  show user manager...).
*/

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
#include <coreplugin/itheme.h>

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
static inline Core::IUser *user() {return Core::ICore::instance()->user();}

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline void messageSplash(const QString &s) {theme()->messageSplashScreen(s); }

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

    messageSplash(tr("Initializing user manager plugin..."));

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

    messageSplash(tr("Initializing user manager plugin..."));

    // add UserPreferences page
    addAutoReleasedObject(new UserPlugin::CurrentUserPreferencesPage(this));

    // add UserManager toogler action to plugin menu
#ifndef FREEMEDFORMS
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
    /** \todo manage user's right to enable/unable these actions */
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

    updateActions();
    // Update context is necessary
//    contextManager()->updateContext();

}

/** \brief Show the user manager session is connected to Core::Constants::A_USERMANAGER command. */
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

/** \brief Create a new user is connected to Core::Constants::A_CREATEUSER. */
void UserManagerPlugin::createUser()
{
    UserWizard wiz;
    wiz.createUser(true);
    wiz.exec();
}

/** \brief Change current user is connected to a private action. */
void UserManagerPlugin::changeCurrentUser()
{
    Internal::UserIdentifier ident;
    if (ident.exec() == QDialog::Rejected)
        return;
    updateActions();
    QString log = ident.login();
    QString pass = ident.cryptedPassword();
    settings()->setValue(Core::Constants::S_LASTLOGIN, log);
    settings()->setValue(Core::Constants::S_LASTPASSWORD, pass);
    Utils::informativeMessageBox(tkTr(Trans::Constants::CONNECTED_AS_1)
                                 .arg(userModel()->currentUserData(Core::IUser::FullName).toString()),"","","");
}

/** \brief Update available global users actions according to the rights of the current user. */
void UserManagerPlugin::updateActions()
{
    if (user()) {
        Core::IUser::UserRights umRights(user()->value(Core::IUser::ManagerRights).toInt());
//        Core::IUser::UserRights adminRights(user()->value(Core::IUser::AdministrativeRights));
        if ((umRights & Core::IUser::AllRights) ||
            (umRights & Core::IUser::ReadAll)) {
            aUserManager->setEnabled(true);
            aCreateUser->setEnabled(true);
        } else {
            aUserManager->setEnabled(false);
            if (umRights & Core::IUser::Create)
                aCreateUser->setEnabled(true);
            else
                aCreateUser->setEnabled(false);
        }
    }
}

Q_EXPORT_PLUGIN(UserManagerPlugin)
