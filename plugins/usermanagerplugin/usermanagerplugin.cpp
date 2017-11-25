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
  \namespace UserPlugin
  \brief Namespace reserved for the User manager plugin.
*/

/**
  \class UserPlugin::UserManagerPlugin
  \brief Core::IPlugin class for the user plugin. Owns some global actions (create new user,
  show user manager...).
*/

#include "usermanagerplugin.h"
#include "usercore.h"
#include "usermodel.h"
#include "database/userbase.h"
#include "widgets/usermanager.h"
#include "widgets/useridentifier.h"
#include "widgets/usercreatorwizard.h"
#include "currentuserpreferencespage.h"
#include "userfirstrunpage.h"
#include "usermanagermode.h"
#include "database/userbase.h"
#include "widgets/usermanager.h"

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants.h>
#include <coreplugin/isettings.h>
#include <coreplugin/translators.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/icommandline.h>
#include <coreplugin/modemanager/modemanager.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/contextmanager/contextmanager.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/databaseconnector.h>
#include <utils/widgets/databaseinformationdialog.h>
#include <translationutils/constants.h>
#include <translationutils/trans_database.h>
#include <translationutils/trans_titles.h>
#include <translationutils/trans_user.h>
#include <translationutils/trans_menu.h>
#include <translationutils/trans_current.h>

#include <extensionsystem/pluginmanager.h>

#include <QtPlugin>
#include <QApplication>
#include <QProgressDialog>
#include <QTreeWidget>
#include <QGridLayout>

#include <QDebug>

enum {
    WithUserMode = false    // When set to true a Core::IMode is created with a
                            // usermanager widget inside
};

using namespace UserPlugin;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::ModeManager *modeManager() { return Core::ICore::instance()->modeManager(); }
static inline Core::IUser *user() {return Core::ICore::instance()->user();}
static inline Core::ICommandLine *commandLine() {return Core::ICore::instance()->commandLine();}

static inline UserPlugin::UserCore &userCore() {return UserPlugin::UserCore::instance();}
static inline UserPlugin::UserModel *userModel() {return userCore().userModel();}
static inline UserPlugin::Internal::UserBase *userBase() {return userCore().userBase();}

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline void messageSplash(const QString &s) {theme()->messageSplashScreen(s); }

UserManagerPlugin::UserManagerPlugin() :
    aCreateUser(0), aChangeUser(0),
    aUserManager(0),
    aAboutDatabase(0),
    m_FirstCreation(new FirstRun_UserCreation(this)),
    m_Mode(0),
    m_UserManagerMainWin(0)
{
    setObjectName("UserManagerPlugin");
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "creating UserManagerPlugin";

    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("plugin_usermanager");

    new UserCore(this);

    addObject(m_FirstCreation);
    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreInitialization()));

#ifdef USERMANAGERTESTER
    // Here we only use the UserManagerMainWindow
    m_UserManagerMainWin = new UserManagerMainWindow;
    Core::ICore::instance()->setMainWindow(m_UserManagerMainWin);
#endif
}

UserManagerPlugin::~UserManagerPlugin()
{
    if (Utils::Log::debugPluginsCreation())
        WARN_FUNC;
}

bool UserManagerPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "UserManagerPlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

    messageSplash(tr("Initializing user manager plugin..."));

    // WorkFlow:
    // 1. Create the userBase without initialization
    // 2. Identify user
    //   2.1. Three tries -> no users -> close app
    //   2.2. User logs are correct -> continue
    // 3. Initialize database, create usermodel, connect user using the model

    // Ask for User login
    if (!identifyUser()) {
        if (!errorString)
            errorString = new QString();
        errorString->append(tr("User is not identified."));
        Core::ICore::instance()->setUser(0);
        return false;
    }

    if (!userCore().initialize()
            || !userBase()->isInitialized()) {
        LOG_ERROR("Unable to initialize user core/base. Application closes.");
        return false;
    }

    // Set current user into UserModel
    if (!userModel()->setCurrentUser(settings()->databaseConnector().clearLog(), settings()->databaseConnector().clearPass())) {
        LOG("Unable to set UserModel current user. Quit application.");
        return false;
    }

    // manage virtual user creation
    // TODO: warning: creating virtual user can lead to severe security issue (because log and pass are known)
    if (commandLine()->value(Core::ICommandLine::CreateVirtuals).toBool()) {
        QProgressDialog dlg(tr("Creating virtual users"), tr("Please wait"), 0, 0);
        dlg.setWindowModality(Qt::WindowModal);
        dlg.setMinimumDuration(1000);
        dlg.show();
        dlg.setFocus();
        dlg.setValue(0);

        // physician
        bool created = userBase()->createVirtualUser("d1f29ad4a4ea4dabbe40ec888d153228",
                                                     "McCoy",
                                                     "Leonard",
                                                     Trans::Constants::Doctor,
                                                     genders().indexOf(tkTr(Trans::Constants::MALE)),
                                                     QStringList() << "Medical Doctor",
                                                     QStringList() << "Chief medical officer USS Enterprise",
                                                     Core::IUser::AllRights,
                                                     Core::IUser::AllRights,
                                                     Core::IUser::ReadOwn | Core::IUser::WriteOwn | Core::IUser::Delete,
                                                     Core::IUser::AllRights,
                                                     Core::IUser::AllRights);
        if (created) {
            // "physician"
            userBase()->createVirtualUser("b5caead635a246a2a87ce676e9d2ef4d",
                                          "Phlox",
                                          "",
                                          Trans::Constants::Doctor,
                                          genders().indexOf(tkTr(Trans::Constants::MALE)),
                                          QStringList() << "Intergalactic medicine",
                                          QStringList() << "Chief medical officer Enterprise NX-01",
                                          Core::IUser::AllRights,
                                          Core::IUser::AllRights,
                                          Core::IUser::ReadOwn | Core::IUser::WriteOwn | Core::IUser::Delete,
                                          Core::IUser::AllRights,
                                          Core::IUser::AllRights);
            // "secretary"
            userBase()->createVirtualUser("0f148ea3de6e47b8bbf9c2cedea47511",
                                          "Uhura",
                                          "",
                                          Trans::Constants::Madam,
                                          genders().indexOf(tkTr(Trans::Constants::FEMALE)),
                                          QStringList() << "Communications officer",
                                          QStringList() << "Enterprise NX-01",
                                          0,
                                          0,
                                          0,
                                          Core::IUser::AllRights,
                                          0);
            // "nurse"
            userBase()->createVirtualUser("b94ad4ee401a4fada0bf29fc8f8f3597",
                                          "Chapel",
                                          "Christine",
                                          Trans::Constants::Madam,
                                          genders().indexOf(tkTr(Trans::Constants::FEMALE)),
                                          QStringList() << "Space nurse",
                                          QStringList() << "Nurse, Enterprise NX-01",
                                          0,
                                          0,
                                          0,
                                          Core::IUser::AllRights,
                                          Core::IUser::AllRights);
            // refresh model
            userModel()->refresh();
            // reconnect user
            Utils::DatabaseConnector c = settings()->databaseConnector();
            // clear cache, don't check preferences validity
            userModel()->setCurrentUser(c.clearLog(), c.clearPass(), true, false);
        }
    }

    if (m_UserManagerMainWin) {
        if (!m_UserManagerMainWin->initialize()) {
            LOG_ERROR("Main window not initialized");
            return false;
        }
    }

    return true;
}

void UserManagerPlugin::extensionsInitialized()
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "UserManagerPlugin::extensionsInitialized";

    messageSplash(tr("Initializing user manager plugin..."));

    // add UserPreferences page
//    addAutoReleasedObject(new UserPlugin::CurrentUserPreferencesPage(this));

#ifdef USERMANAGERTESTER
    // Nothing to do with the tester as we only need a usermanager mainwindow
    if (m_UserManagerMainWin)
        m_UserManagerMainWin->extensionsInitialized();
#endif

    // add UserManager toogler action to plugin menu
#ifdef FREEHEALTH
    const char * const menuId = Core::Constants::M_GENERAL;
    const char * const menuNewId = Core::Constants::M_GENERAL_NEW;
    const char * const groupUsers = Core::Constants::G_GENERAL_USERS;
    const char * const groupNew = Core::Constants::G_GENERAL_NEW;
#else
    const char * const menuId = Core::Constants::M_FILE;
    const char * const menuNewId = Core::Constants::M_FILE_NEW;
    const char * const groupUsers = Core::Constants::G_FILE_OTHER;
    const char * const groupNew =  Core::Constants::G_FILE_NEW;
#endif

    Core::ActionContainer *menu = actionManager()->actionContainer(menuId);
    Q_ASSERT(menu);
    if (!menu)
        return;
    Core::ActionContainer *newmenu = actionManager()->actionContainer(menuNewId);
    if (!newmenu)
        newmenu = menu;

    Core::Context ctx(Core::Constants::C_GLOBAL);
    QAction *a = 0;
    Core::Command *cmd = 0;

    // Create user
    // TODO: manage user's right to enable/disable these actions
    a = aCreateUser = new QAction(this);
    a->setObjectName("aCreateUser");
    a->setIcon(QIcon(Core::Constants::ICONNEWUSER));
    cmd = actionManager()->registerAction(aCreateUser, Core::Id(Core::Constants::A_CREATEUSER), ctx);
    Q_ASSERT(cmd);
    cmd->setDefaultKeySequence(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_U));
    cmd->setTranslations(Trans::Constants::USER);
    newmenu->addAction(cmd, Core::Id(groupNew));
    cmd->retranslate();
    connect(aCreateUser, SIGNAL(triggered()), this, SLOT(createUser()));

    // Change current user
    a = aChangeUser = new QAction(this);
    a->setObjectName("aChangeUser");
    a->setIcon(QIcon(Core::Constants::ICONUSER));
    cmd = actionManager()->registerAction(aChangeUser, Core::Id("aChangeCurrentUser"), ctx);
    Q_ASSERT(cmd);
    cmd->setTranslations(Trans::Constants::CHANGE_USER);
    menu->addAction(cmd, Core::Id(groupUsers));
    cmd->retranslate();
    connect(aChangeUser, SIGNAL(triggered()), this, SLOT(changeCurrentUser()));

    a = aUserManager = new QAction(this);
    a->setObjectName("aUserManager");
    a->setIcon(QIcon(Core::Constants::ICONUSERMANAGER));
    cmd = actionManager()->registerAction(aUserManager, Core::Id("aUserManager"), ctx);
    Q_ASSERT(cmd);
    cmd->setTranslations(Trans::Constants::USERMANAGER_TEXT);
    menu->addAction(cmd, Core::Id(groupUsers));
    cmd->retranslate();
    connect(aUserManager, SIGNAL(triggered()), this, SLOT(showUserManager()));

    Core::ActionContainer *hmenu = actionManager()->actionContainer(Core::Id(Core::Constants::M_HELP_DATABASES));
    if (hmenu) {
        a = aAboutDatabase = new QAction(this);
        a->setObjectName("aAboutDatabase");
        a->setIcon(QIcon(Core::Constants::ICONHELP));
        cmd = actionManager()->registerAction(aAboutDatabase, Core::Id("aAboutDatabase"), ctx);
        Q_ASSERT(cmd);
        cmd->setTranslations(Trans::Constants::USER_DATABASE_INFORMATION);
        hmenu->addAction(cmd, Core::Id(Core::Constants::G_HELP_DATABASES));
        cmd->retranslate();
        connect(aAboutDatabase, SIGNAL(triggered()), this, SLOT(showDatabaseInformation()));
    }

    updateActions();

    // create the mode
    if (WithUserMode)
        m_Mode = new Internal::UserManagerMode(this);
}

/**
 * Try to identify the current user using:
 * - settings
 * - commandline
 * - or at least a dialog
 */
bool UserManagerPlugin::identifyUser()
{
    // instanciate user model
    userModel();

    // Try to catch user log and pass from settings and/or commandline
    QString log;
    QString pass;
    Utils::DatabaseConnector connector = settings()->databaseConnector();

#ifdef WITH_USER_AUTOLOGIN
    if (connector.driver()==Utils::Database::SQLite) {
        log = connector.clearLog();
        pass = connector.clearPass();
    }
#endif
    if (commandLine()->value(Core::ICommandLine::UserClearLogin).isValid()) {
        log = commandLine()->value(Core::ICommandLine::UserClearLogin).toString();
        pass = commandLine()->value(Core::ICommandLine::UserClearPassword).toString();
        LOG(tr("Using command line user identifiants: %1").arg(log));
    }

    // Try to connect to user database
    while (true) {
        if ((!log.isEmpty() && !pass.isEmpty())
            && userBase()->checkLogin(log, pass)) {
            // We currently have the correct log and pass, store them in the connector
            connector.setClearLog(log);
            connector.setClearPass(pass);
            break;
        } else {
            Internal::UserIdentifier ident;
            if (ident.exec() == QDialog::Rejected)
                return false;
            connector.setClearLog(ident.login());
            connector.setClearPass(ident.password());
            break;
        }
    }

    // Here the connector should be complete
    settings()->setDatabaseConnector(connector);
    return true;
}

void UserManagerPlugin::postCoreInitialization()
{
    if (Utils::Log::debugPluginsCreation())
        WARN_FUNC;
    userCore().postCoreInitialization();
}

/** \brief Create a new user is connected to Core::Constants::A_CREATEUSER. */
void UserManagerPlugin::createUser()
{
    UserCreatorWizard wiz(Core::ICore::instance()->mainWindow());
    wiz.show();
    Utils::resizeAndCenter(&wiz, Core::ICore::instance()->mainWindow());
    wiz.exec();
}

/** \brief Change current user is connected to a private action. */
void UserManagerPlugin::changeCurrentUser()
{
    // FIXME: user does not change at all
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
    // Set current user into UserModel
    if (!userModel()->setCurrentUser(log, pass)) {
        LOG("Unable to set UserModel current user.");
        Utils::warningMessageBox(tr("Unable to change current user"),
                                 tr("An error occured when trying to change "
                                    "the current user. %1")
                                 .arg(tkTr(Trans::Constants::CONTACT_DEV_TEAM)));
        return;
    }
    modeManager()->activateMode(Core::Constants::MODE_PATIENT_SEARCH);
    Utils::informativeMessageBox(tkTr(Trans::Constants::CONNECTED_AS_1)
                                 .arg(userModel()->currentUserData(Core::IUser::FullName).toString()),"","","");
}

/** \brief Update available global users actions according to the rights of the current user. */
void UserManagerPlugin::updateActions()
{
    if (user()) {
        Core::IUser::UserRights umRights(user()->value(Core::IUser::ManagerRights).toInt());
        aUserManager->setEnabled(umRights & Core::IUser::AllRights);
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

void UserManagerPlugin::showUserManager()
{
    UserManagerDialog dlg(Core::ICore::instance()->mainWindow());
//    dlg.setModal(true);
    dlg.initialize();
    Utils::resizeAndCenter(&dlg, Core::ICore::instance()->mainWindow());
    dlg.show();
    dlg.initializeAfterShowing();
    dlg.exec();
}

void UserManagerPlugin::showDatabaseInformation()
{
    Utils::DatabaseInformationDialog dlg(Core::ICore::instance()->mainWindow());
    dlg.setTitle(tkTr(Trans::Constants::TEMPLATE_DATABASE_INFORMATION));
    dlg.setDatabase(*userBase());
    Utils::resizeAndCenter(&dlg);
    dlg.exec();
}

ExtensionSystem::IPlugin::ShutdownFlag UserManagerPlugin::aboutToShutdown()
{
    if (Utils::Log::debugPluginsCreation())
        WARN_FUNC;
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    // Remove preferences pages to plugins manager object pool
    if (m_FirstCreation) {
        removeObject(m_FirstCreation);
        delete m_FirstCreation;
        m_FirstCreation = 0;
    }
    return SynchronousShutdown;
}

Q_EXPORT_PLUGIN(UserManagerPlugin)
