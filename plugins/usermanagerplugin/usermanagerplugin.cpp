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
#include "database/userbase.h"
#include "widgets/usermanager.h"
#include "widgets/useridentifier.h"
#include "widgets/userwizard.h"
#include "currentuserpreferencespage.h"
#include "userfistrunpage.h"
#include "usermanagermode.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants.h>
#include <coreplugin/translators.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/iuser.h>
#include <coreplugin/itheme.h>
#include <coreplugin/icommandline.h>

#include <translationutils/constanttranslations.h>
#include <utils/log.h>
#include <utils/global.h>
#include <utils/databaseconnector.h>

#include <extensionsystem/pluginmanager.h>

#include <QtCore/QtPlugin>
#include <QApplication>
#include <QDebug>

using namespace UserPlugin;
using namespace Trans::ConstantTranslations;

static inline Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline UserPlugin::UserModel *userModel() {return UserPlugin::UserModel::instance();}
static inline UserPlugin::Internal::UserBase *userBase() {return UserPlugin::Internal::UserBase::instance();}
static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }
static inline Core::IUser *user() {return Core::ICore::instance()->user();}
static inline Core::ICommandLine *commandLine() {return Core::ICore::instance()->commandLine();}

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline void messageSplash(const QString &s) {theme()->messageSplashScreen(s); }

UserManagerPlugin::UserManagerPlugin() :
        aCreateUser(0), aChangeUser(0),
        m_FirstCreation(new FirstRun_UserCreation(this)),
        m_Mode(0)
{
    setObjectName("UserManagerPlugin");
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating UserManagerPlugin";
    addObject(m_FirstCreation);
}

UserManagerPlugin::~UserManagerPlugin()
{
    qWarning() << "UserManagerPlugin::~UserManagerPlugin()";
    if (m_FirstCreation) {
        removeObject(m_FirstCreation);
        delete m_FirstCreation;
        m_FirstCreation = 0;
    }
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

    // is UserBase reachable ?
    userBase();
    if (!userBase()->isInitialized()) {
        Utils::warningMessageBox(tr("Unable to connect to the user database."),
                                 tr("The user database is not reachable. Please check your configuration. \n"
                                    "Application will stop."));
        return false;
    }

    // Ask for User login
    if (!identifyUser()) {
        *errorString = tr("User is not identified.");
        return false;
    }

    // manage virtual user creation
    if (commandLine()->value(Core::ICommandLine::CreateVirtuals).toBool()) {
        bool created = true;
        // Doctors
        created = userBase()->createVirtualUser("d1f29ad4a4ea4dabbe40ec888d153228", "McCoy", "Leonard", Trans::Constants::Doctor, genders().indexOf(tkTr(Trans::Constants::MALE)),
                                      QStringList() << "Medical Doctor",
                                      QStringList() << "Chief medical officer USS Enterprise",
                                      Core::IUser::AllRights, Core::IUser::AllRights, 0, Core::IUser::AllRights, Core::IUser::AllRights);
        if (!created) {
            return true;
        }

        userBase()->createVirtualUser("b5caead635a246a2a87ce676e9d2ef4d", "Phlox", "", Trans::Constants::Doctor, genders().indexOf(tkTr(Trans::Constants::MALE)),
                                      QStringList() << "Intergalactic medicine",
                                      QStringList() << "Chief medical officer Enterprise NX-01",
                                      Core::IUser::AllRights, Core::IUser::AllRights, 0, Core::IUser::AllRights, Core::IUser::AllRights);
        // Secretaries or so :  Uhura  U.S.S. Enterprise
        userBase()->createVirtualUser("0f148ea3de6e47b8bbf9c2cedea47511", "Uhura", "", Trans::Constants::Madam, genders().indexOf(tkTr(Trans::Constants::FEMALE)),
                                      QStringList() << "Communications officer",
                                      QStringList() << "Enterprise NX-01",
                                      0, 0, 0, Core::IUser::AllRights, 0);
        // Nurses : Christine Chapel U.S.S. Enterprise
        userBase()->createVirtualUser("b94ad4ee401a4fada0bf29fc8f8f3597", "Chapel", "Christine", Trans::Constants::Madam, genders().indexOf(tkTr(Trans::Constants::FEMALE)),
                                      QStringList() << "Space nurse",
                                      QStringList() << "Nurse, Enterprise NX-01",
                                      0, 0, 0, Core::IUser::AllRights, Core::IUser::AllRights);
        // Admins

        // refresh model
        UserModel::instance()->refresh();
        // reconnect user
        Utils::DatabaseConnector c = settings()->databaseConnector();
        UserModel::instance()->setCurrentUser(c.clearLog(), c.clearPass(), true);
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

    // Create user
    /** \todo manage user's right to enable/unable these actions */
    a = aCreateUser = new QAction(this);
    a->setObjectName("aCreateUser");
    a->setIcon(QIcon(Core::Constants::ICONNEWUSER));
    cmd = actionManager()->registerAction(aCreateUser, Core::Constants::A_CREATEUSER, ctx);
    Q_ASSERT(cmd);
    cmd->setDefaultKeySequence(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_U));
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
    menu->addAction(cmd, groupUsers);
    cmd->retranslate();
    connect(aChangeUser, SIGNAL(triggered()), this, SLOT(changeCurrentUser()));

    updateActions();

    // create the mode
    m_Mode = new Internal::UserManagerMode(this);

    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreInitialization()));
}

bool UserManagerPlugin::identifyUser()
{
    // instanciate user model
    userModel();
    QString log;
    QString pass;
    bool sqliteVersion = (settings()->databaseConnector().driver()==Utils::Database::SQLite);
    if (sqliteVersion) {
        log = settings()->databaseConnector().clearLog();
        pass = settings()->databaseConnector().clearPass();
    }
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
            pass = ident.password();

            if (sqliteVersion) {
                Utils::DatabaseConnector c = settings()->databaseConnector();
                c.setClearLog(log);
                c.setClearPass(pass);
                settings()->setDatabaseConnector(c);
            }

            ask = false;
            break;
        }
    }
    return true;
}

void UserManagerPlugin::postCoreInitialization()
{
    // be sure everyone is informed of the currently connected user
    userModel()->emitUserConnected();
}

/** \brief Create a new user is connected to Core::Constants::A_CREATEUSER. */
void UserManagerPlugin::createUser()
{
    UserWizard wiz;
//    wiz.createUser(true);
    wiz.exec();
}

/** \brief Change current user is connected to a private action. */
void UserManagerPlugin::changeCurrentUser()
{
    Internal::UserIdentifier ident;
    if (ident.exec() == QDialog::Rejected)
        return;
    updateActions();
    const QString &log = ident.login();
    const QString &pass = ident.password();
    bool sqliteVersion = (settings()->databaseConnector().driver()==Utils::Database::SQLite);
    if (sqliteVersion) {
        Utils::DatabaseConnector c = settings()->databaseConnector();
        c.setClearLog(log);
        c.setClearPass(pass);
        settings()->setDatabaseConnector(c);
    }
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
            aCreateUser->setEnabled(true);
        } else {
            if (umRights & Core::IUser::Create)
                aCreateUser->setEnabled(true);
            else
                aCreateUser->setEnabled(false);
        }
    }
}

Q_EXPORT_PLUGIN(UserManagerPlugin)
