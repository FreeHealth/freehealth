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
  \class UserPlugin::UserModel
  Users are represented into a table model. Each row represents a user, each
  column a value.
  The current user represents the currently logged in user.
  Before the current user can be changed (disconnection, another user connection)
  IUserListener registered in the PluginManager are asked.
  Set filter with setFilter().

  Unit-tests are available see UserPlugin::Internal::UserManagerPlugin

  \sa UserPlugin::Internal::UserData
  \sa UserPlugin::IUserListener
*/

//TODO: write documentation+++
//TODO: code LOCKER
//TODO: when QDataWidgetMapper (UserViewer) is set, it calls ALL the data of the user, even for the hidden widgets. This causes an important memory usage. Improve this ++++

#include "usermodel.h"

#include <utils/global.h>
#include <utils/log.h>
#include <utils/randomizer.h>
#include <utils/passwordandlogin.h>
#include <utils/databaseconnector.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/translators.h>
#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/icorelistener.h>
#include <coreplugin/ioptionspage.h>
#include <coreplugin/icommandline.h>
#include <coreplugin/constants_tokensandsettings.h>

#include <printerplugin/textdocumentextra.h>

#include <usermanagerplugin/usercore.h>
#include <usermanagerplugin/database/userbase.h>
#include <usermanagerplugin/userdata.h>
#include <usermanagerplugin/iuserlistener.h>

#include <extensionsystem/pluginmanager.h>

#include <QApplication>
#include <QColor>
#include <QByteArray>
#include <QFont>
#include <QHash>
#include <QSqlTableModel>

enum { WarnAllProcesses = false, WarnUserConnection = true };  //                                       *

using namespace UserPlugin;
using namespace UserPlugin::Constants;
using namespace Trans::ConstantTranslations;

static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::ICommandLine *commandLine() {return Core::ICore::instance()->commandLine();}
static inline UserPlugin::UserCore &userCore() {return UserPlugin::UserCore::instance();}
static inline UserPlugin::UserModel *userModel() {return userCore().userModel();}
static inline UserPlugin::Internal::UserBase *userBase() {return userCore().userBase();}

namespace {
const char * const SERVER_ADMINISTRATOR_UUID = "serverAdmin";

//    class UserModelCoreListener : public Core::ICoreListener
//    {
//    public:
//        UserModelCoreListener(QObject *parent) : Core::ICoreListener(parent) {}
//        bool coreAboutToClose()
//        {
//            return true;
//        }
//    };
}

namespace UserPlugin {
namespace Internal {
class UserModelPrivate
{
public:
    UserModelPrivate(UserModel */*parent*/) :
        m_Sql(0)  // , q(parent)
    {}

    ~UserModelPrivate()
    {
        if (m_Uuid_UserList.count() > 0) {
            qDeleteAll(m_Uuid_UserList);
            m_Uuid_UserList.clear();
        }
        if (m_Sql) {
            delete m_Sql;
            m_Sql = 0;
        }
    }

    // Clear the cache
    void clearCache()
    {
        // Keep the current user
        UserData *current = m_Uuid_UserList.take(m_CurrentUserUuid);
        qDeleteAll(m_Uuid_UserList);
        m_Uuid_UserList.clear();
        m_Uuid_UserList.insert(m_CurrentUserUuid, current);
    }

    // Retrieve all users data and store it to the cache of the model.
    // \sa numberOfUsersInMemory(), m_Uuid_UserList
    bool addUserFromDatabase(const QString &uuid)
    {
        if (WarnAllProcesses)
            qWarning() << Q_FUNC_INFO << uuid;
        // make sure it is not already in the hash
        if (m_Uuid_UserList.keys().contains(uuid))
            return true;

        // get user from database
        UserData *un = userBase()->getUserByUuid(uuid);
        m_Uuid_UserList.insert(uuid, un);
        return true;
    }

    /**
     * Retrieve all users data and store it in the cache of the model.
     * Return the created uuid.
     *  \sa numberOfUsersInMemory(), m_Uuid_UserList
     */
    QString addUserFromDatabase(const QString &log64, const QString &pass64)
    {
        // get user from database
        QString uuid = userBase()->getUuid(log64, pass64);
        if (WarnAllProcesses)
            qWarning() << Q_FUNC_INFO << log64 << pass64 << uuid;
        if (uuid.isEmpty())
            return QString();
        // make sure it is not already in the hash
        if (m_Uuid_UserList.keys().contains(uuid)) {
            return uuid;
        }
        m_Uuid_UserList.insert(uuid, userBase()->getUserByUuid(uuid));
        return uuid;
    }

    /**
     * Create and empty user in the model.
     * The uuid of the user is automatically set and returned.
     */
    QString createNewEmptyUser(UserModel *model, const int createdRow)
    {
        Q_UNUSED(model);
        Q_UNUSED(createdRow);
        // 1. create an empty user into the hash
        QString uuid = userBase()->createNewUuid();
        if (WarnAllProcesses)
            qWarning() << Q_FUNC_INFO << uuid;
        m_Uuid_UserList.insert(uuid, new UserData(uuid));
        return uuid;
    }

    QVariant getUserData(const UserData *user, const int ref)
    {
        QVariant toReturn;
        switch (ref)
        {
        case Core::IUser::Id : toReturn = user->id(); break;
        case Core::IUser::Uuid : toReturn = user->uuid(); break;
        case Core::IUser::Validity : toReturn = user->validity(); break;
        case Core::IUser::IsVirtual : toReturn = user->isVirtual(); break;
        case Core::IUser::Login64 : toReturn = user->login64(); break;
        case Core::IUser::ClearLogin : toReturn = user->clearLogin(); break;
        case Core::IUser::DecryptedLogin : toReturn = user->decryptedLogin(); break;
        case Core::IUser::ClearPassword : toReturn = user->clearPassword(); break;
        case Core::IUser::CryptedPassword : toReturn = user->cryptedPassword(); break;
        case Core::IUser::LastLoggedIn : toReturn = user->lastLoggedIn(); break;
        case Core::IUser::GenderIndex : toReturn = user->genderIndex(); break;
        case Core::IUser::TitleIndex : toReturn = user->titleIndex(); break;
        case Core::IUser::Gender : toReturn = user->gender(); break;
        case Core::IUser::Title : toReturn = user->title(); break;
        case Core::IUser::UsualName : toReturn = user->usualName(); break;
        case Core::IUser::OtherNames : toReturn = user->otherNames(); break;
        case Core::IUser::Firstname : toReturn = user->firstname(); break;
        case Core::IUser::FullName : toReturn = user->fullName(); break;
        case Core::IUser::Mail : toReturn = user->mail(); break;
        case Core::IUser::LanguageISO : toReturn = user->languageIso(); break;
        case Core::IUser::Locker : toReturn = user->locker(); break;
        case Core::IUser::LocaleLanguage : toReturn = user->localeLanguage(); break;
        case Core::IUser::LocaleCodedLanguage: toReturn = user->localeLanguage(); break;

        case Core::IUser::PhotoPixmap : toReturn = user->photo(); break;
        case Core::IUser::DateOfBirth : toReturn = user->dob(); break;

        case Core::IUser::Street : toReturn = user->street(); break;
        case Core::IUser::Zipcode : toReturn = user->zipcode(); break;
        case Core::IUser::StateProvince : toReturn = user->stateProvince(); break;
        case Core::IUser::City : toReturn = user->city(); break;
        case Core::IUser::Country : toReturn = user->country(); break;
        case Core::IUser::IsoCountry : toReturn = user->countryIso(); break;
        case Core::IUser::FullHtmlAddress :
        {
            QString t;
            if (!user->street().isEmpty())
                toReturn = QString("%1<br />%2 %3, %4")
                        .arg(user->street())
                        .arg(user->zipcode())
                        .arg(user->city())
                        .arg(user->country()).simplified();
            break;
        }
        case Core::IUser::FullAddress :
        {
            QString t;
            if (!user->street().isEmpty())
                toReturn = QString("%1\n%2 %3\n%4\n%5")
                        .arg(user->street())
                        .arg(user->zipcode())
                        .arg(user->city())
                        .arg(user->stateProvince())
                        .arg(user->country()).simplified();
            break;
        }

        case Core::IUser::Tel1 : toReturn = user->tels().at(0); break;
        case Core::IUser::Tel2 : toReturn = user->tels().at(1); break;
        case Core::IUser::Tel3 : toReturn = user->tels().at(2); break;
        case Core::IUser::Fax : toReturn = user->fax(); break;
        case Core::IUser::FullHtmlContact :
        {
            QString t;
            QStringList tels = user->tels();
            tels.removeAll("");
            if (!tels.isEmpty())
                t = tkTr(Trans::Constants::TELEPHONE) + " " + tels.join("; ") + "<br />";
            if (!user->fax().isEmpty())
                t += tkTr(Trans::Constants::FAX) + " " + user->fax() + "<br />";
            if (!user->mail().isEmpty())
                t += tkTr(Trans::Constants::MAIL) + " " + user->mail() + "<br />";
            if (t.size()>0)
                t.chop(6);
            toReturn = t;
            break;
        }

        case Core::IUser::ProfessionalIdentifiants : toReturn = user->professionalIdentifiants(); break;
        case Core::IUser::Specialities : toReturn = user->specialties(); break;
        case Core::IUser::Qualifications : toReturn = user->qualifications(); break;

        case Core::IUser::Preferences : toReturn = user->preferences(); break;
        case Core::IUser::DataPackConfig: toReturn = userBase()->getUserDynamicData(user->uuid(), Constants::USER_DATA_DATAPACK_CONFIG); break;

        case Core::IUser::GenericHeader:
        case Core::IUser::GenericFooter:
        case Core::IUser::GenericWatermark:
        case Core::IUser::AdministrativeHeader:
        case Core::IUser::AdministrativeFooter:
        case Core::IUser::AdministrativeWatermark:
        case Core::IUser::PrescriptionHeader:
        case Core::IUser::PrescriptionFooter:
        case Core::IUser::PrescriptionWatermark:
            toReturn = user->extraDocumentHtml(ref);
            break;

        case Core::IUser::GenericHeaderPresence :
        {
            Print::TextDocumentExtra *doc = user->extraDocument(Core::IUser::GenericHeader);
            if (doc)
                return doc->presence();
            return Print::Printer::EachPages;
        }
        case Core::IUser::GenericFooterPresence :
        {
            Print::TextDocumentExtra *doc = user->extraDocument(Core::IUser::GenericFooter);
            if (doc)
                return doc->presence();
            return Print::Printer::EachPages;
        }
        case Core::IUser::GenericWatermarkPresence :
        {
            Print::TextDocumentExtra *doc = user->extraDocument(Core::IUser::GenericWatermark);
            if (doc)
                return doc->presence();
            return Print::Printer::EachPages;
        }
        case Core::IUser::GenericWatermarkAlignement :
        {
            // TODO: return Watermark alignement
            return Qt::AlignCenter;
        }
        case Core::IUser::AdministrativeHeaderPresence :
        {
            Print::TextDocumentExtra *doc = user->extraDocument(Core::IUser::AdministrativeHeader);
            if (doc)
                return doc->presence();
            return Print::Printer::EachPages;
        }
        case Core::IUser::AdministrativeFooterPresence :
        {
            Print::TextDocumentExtra *doc = user->extraDocument(Core::IUser::AdministrativeFooter);
            if (doc)
                return doc->presence();
            return Print::Printer::EachPages;
        }
        case Core::IUser::AdministrativeWatermarkPresence :
        {
            Print::TextDocumentExtra *doc = user->extraDocument(Core::IUser::AdministrativeWatermark);
            if (doc)
                return doc->presence();
            return Print::Printer::EachPages;
        }
        case Core::IUser::AdministrativeWatermarkAlignement :
        {
            // TODO: return Watermark alignement
            return Qt::AlignCenter;
        }

        case Core::IUser::PrescriptionHeaderPresence :
        {
            Print::TextDocumentExtra *doc = user->extraDocument(Core::IUser::PrescriptionHeader);
            if (doc)
                return doc->presence();
            return Print::Printer::EachPages;
        }
        case Core::IUser::PrescriptionFooterPresence :
        {
            Print::TextDocumentExtra *doc = user->extraDocument(Core::IUser::PrescriptionFooter);
            if (doc)
                return doc->presence();
            return Print::Printer::EachPages;
        }
        case Core::IUser::PrescriptionWatermarkPresence :
        {
            Print::TextDocumentExtra *doc = user->extraDocument(Core::IUser::PrescriptionWatermark);
            if (doc)
                return doc->presence();
            return Print::Printer::EachPages;
        }
        case Core::IUser::PrescriptionWatermarkAlignement :
        {
            // TODO: return Watermark alignement
            return Qt::AlignCenter;
        }

        case Core::IUser::IsModified : toReturn = user->isModified(); break;
        case Core::IUser::ManagerRights : toReturn = user->rightsValue(USER_ROLE_USERMANAGER); break;
        case Core::IUser::MedicalRights : toReturn = user->rightsValue(USER_ROLE_MEDICAL); break;
        case Core::IUser::DrugsRights : toReturn = user->rightsValue(USER_ROLE_DOSAGES); break;
        case Core::IUser::ParamedicalRights : toReturn = user->rightsValue(USER_ROLE_PARAMEDICAL); break;
        case Core::IUser::SecretaryRights : toReturn = user->rightsValue(USER_ROLE_SECRETARY); break;
        case Core::IUser::AgendaRights : toReturn = user->rightsValue(USER_ROLE_AGENDA); break;
        case Core::IUser::LoginHistory : toReturn = user->loginHistory(); break;
        case Core::IUser::DebugText : toReturn = user->debugText(); break;
        default : toReturn = QVariant();
        };
        return toReturn;
    }

    void checkNullUser()
    {
        foreach(const Internal::UserData *u, m_Uuid_UserList.values()) {
            if (!u || u->uuid().isEmpty()) {
                LOG_ERROR_FOR("UserModel", "Null user in model");
                qWarning() << m_Uuid_UserList;
                continue;
            }
        }
    }

    /**
      * Return true if the current user has enough rights to read data from the
      * user \e userUidToRead
      */
    bool userCanReadData(const QString &userUidToRead)
    {
        // TODO: manage user delegates
        bool canReadAll = false;
        bool canReadOwn = false;
        if (!m_CurrentUserUuid.isEmpty()) {
            // Use internal data
            canReadAll = m_CurrentUserRights & Core::IUser::ReadAll;
            canReadOwn = (m_CurrentUserUuid==userUidToRead && m_CurrentUserRights & Core::IUser::ReadOwn);
        } else {
            // Use Core::IUser
            Core::IUser::UserRights rights = Core::IUser::UserRights(userModel()->currentUserData(Core::IUser::ManagerRights).toInt());
            const QString &userUuid = userModel()->currentUserData(Core::IUser::Uuid).toString();
            canReadAll = rights & Core::IUser::ReadAll;
            canReadOwn = (userUuid==userUidToRead && rights & Core::IUser::ReadOwn);
        }
        return (canReadAll || canReadOwn);
    }

    /**
     * Return true if the current user has enough rights to write data to the
     * user \e userUidToRead
     */
    bool userCanWriteData(const QString &userUidToRead)
    {
        // TODO: manage user delegates
        bool canWriteAll = false;
        bool canWriteOwn = false;
        if (!m_CurrentUserUuid.isEmpty()) {
            // Use internal data
            canWriteAll = m_CurrentUserRights & Core::IUser::WriteAll;
            canWriteOwn = (m_CurrentUserUuid==userUidToRead && m_CurrentUserRights & Core::IUser::WriteOwn);
        } else {
            // Use Core::IUser
            Core::IUser::UserRights rights = Core::IUser::UserRights(userModel()->currentUserData(Core::IUser::ManagerRights).toInt());
            const QString &userUuid = userModel()->currentUserData(Core::IUser::Uuid).toString();
            canWriteAll = rights & Core::IUser::WriteAll;
            canWriteOwn = (userUuid==userUidToRead && rights & Core::IUser::WriteOwn);
        }
        return (canWriteAll || canWriteOwn);
    }

public:
    QSqlTableModel *m_Sql;
    QHash<QString, UserData *> m_Uuid_UserList;
    QString m_CurrentUserUuid;
    Core::IUser::UserRights m_CurrentUserRights;

private:
    //    UserModel *q;
};

}  // End Internal
}  // End UserPlugin

/** Constructor */
UserModel::UserModel(QObject *parent) :
    QAbstractTableModel(parent),
    d(new Internal::UserModelPrivate(this))
{
    setObjectName("UserModel");
}

/** Initialize the model */
bool UserModel::initialize()
{
    onCoreDatabaseServerChanged();
    d->checkNullUser();
    //    connect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
    return true;
}

/** Destructor */
UserModel::~UserModel()
{
    //    if (!d->m_CurrentUserUuid.isEmpty() && d->m_CurrentUserUuid != ::SERVER_ADMINISTRATOR_UUID) {
    //        // save user preferences
    //            Internal::UserData *user = d->m_Uuid_UserList.value(d->m_CurrentUserUuid,0);
    //            if (user) {
    //                user->setPreferences(settings()->userSettings());
    //                userBase()->saveUser(user);
    //            }
    //    }
    if (d) {
        delete d;
        d=0;
    }
}

void UserModel::onCoreDatabaseServerChanged()
{
    if (WarnAllProcesses)
        qWarning() << Q_FUNC_INFO << d->m_Sql;
    if (d->m_Sql)
        delete d->m_Sql;
    d->m_Sql = new QSqlTableModel(this, userBase()->database());
    d->m_Sql->setTable(userBase()->table(Table_USERS));
    d->m_Sql->setEditStrategy(QSqlTableModel::OnManualSubmit);
    d->m_Sql->select();
    d->checkNullUser();
}

/**
  Defines the current user using its login and password. There can be only one current user.
  The date and time of loggin are trace into database.
*/
bool UserModel::setCurrentUser(const QString &clearLog, const QString &clearPassword, bool refreshCache, bool checkPrefValidity)
{
    // FIXME: move this in the private part of the Core::IUser==CoreUserModelWrapper?
    if (WarnAllProcesses || WarnUserConnection)
        qWarning() << Q_FUNC_INFO << clearLog;
    d->checkNullUser();

    Utils::PasswordCrypter crypter;
    QString log64 = Utils::loginForSQL(clearLog);
    // Get crypted password from database
    QString cryptpass64 = userBase()->getCryptedPassword(clearLog);
    // Check password
    if (!crypter.checkPassword(clearPassword, cryptpass64)) {
        LOG_ERROR("Wrong password");
        return false;
    }

    QList<IUserListener *> listeners = pluginManager()->getObjects<IUserListener>();

    // 1. Ask all listeners to prepare the current user disconnection
    foreach(IUserListener *l, listeners) {
        if (!l->userAboutToChange())
            return false;
    }

    // 2. Check "in memory" users
    QString uuid;
    foreach(Internal::UserData *u, d->m_Uuid_UserList.values()) {
        if (!u || u->uuid().isEmpty()) {
            // paranoid code -> if one user is corrupt -> clear all
            LOG_ERROR("Null user in model");
            qDeleteAll(d->m_Uuid_UserList);
            d->m_Uuid_UserList.clear();
            break;
        }
        if (u->login64()==log64 && u->cryptedPassword()==cryptpass64) {
            if (!refreshCache) {
                uuid = u->uuid();
                break;
            } else {
                d->m_Uuid_UserList.remove(u->uuid());
                delete u;
                u = 0;
                break;
            }
        }
    }

    // 3. Get user from Database
    if (uuid.isEmpty()) {
        uuid = d->addUserFromDatabase(log64, cryptpass64);
    }
    if (uuid.isEmpty()) {
        LOG_ERROR(tr("Unable to retrieve user into the model using login and password."));
        return false;
    }

    // 3. Ask all listeners for the current user disconnection
    if (!d->m_CurrentUserUuid.isEmpty()) {
        Q_EMIT userAboutToDisconnect(d->m_CurrentUserUuid);
        foreach(IUserListener *l, listeners) {
            if (!l->currentUserAboutToDisconnect())
                return false;
        }
    }

    // 4. Connect new user
    if (!d->m_CurrentUserUuid.isEmpty()) {
        // save user preferences
        Internal::UserData *user = d->m_Uuid_UserList.value(d->m_CurrentUserUuid, 0);
        if (user) {
            if (WarnUserConnection)
                qWarning() << "Saving user peferences" << d->m_CurrentUserUuid;
            user->setPreferences(settings()->userSettings());
            userBase()->saveUserPreferences(user->uuid(), user->preferences());
        }
    }
    Q_EMIT userDisconnected(d->m_CurrentUserUuid);

    Q_EMIT(userAboutToConnect(uuid));
    LOG(tr("Setting current user uuid to %1").arg(uuid));
    d->m_CurrentUserRights = Core::IUser::NoRights;
    d->m_CurrentUserUuid = uuid;
    foreach(Internal::UserData *u, d->m_Uuid_UserList.values())
        u->setCurrent(false);

    // 5. If precedent currentUser was SERVER_ADMINISTRATOR_UUID --> remove it from the cache
    // TODO: code here

    // 6. Feed Core::ISettings with the user's settings
    Internal::UserData *user = d->m_Uuid_UserList.value(d->m_CurrentUserUuid, 0);
    if (user) {
        settings()->setUserSettings(user->preferences());

        // 7. Trace log
        user->setCurrent(true);
        user->setLastLoggedIn(QDateTime::currentDateTime());
        // function addLoginToHistory() is buggy with MySQL 5.7
        // resulting data is not used by the software
        // diable this until complete rewrite of login history see issue #61
        // user->addLoginToHistory();
        if (!userBase()->saveUser(user))
            return false;
    }

    // TODO: this is not the usermanger role if asked uuid == currentuser
    d->m_CurrentUserRights = Core::IUser::UserRights(user->rightsValue(USER_ROLE_USERMANAGER).toInt());

    // If we are running with a server, we need to reconnect all databases
    if (settings()->databaseConnector().driver()==Utils::Database::MySQL) {
        Utils::DatabaseConnector connector = settings()->databaseConnector();
        connector.setClearLog(clearLog);
        connector.setClearPass(clearPassword);
        settings()->setDatabaseConnector(connector);
        // refresh userbase and model
        delete d->m_Sql;
        d->m_Sql = 0;
        userBase()->onCoreDatabaseServerChanged();
        onCoreDatabaseServerChanged();
        // emit core signal for server login changed
        Core::ICore::instance()->databaseServerLoginChanged(); // with this signal all databases should reconnect
    }
    if (WarnAllProcesses || WarnUserConnection)
        LOG(tkTr(Trans::Constants::CONNECTED_AS_1).arg(user->fullName()));

    // Refresh the newly connected user's preferences
    if (checkPrefValidity)
        checkUserPreferencesValidity();

    // Inform the listeners
    foreach(IUserListener *l, listeners) {
        l->newUserConnected(d->m_CurrentUserUuid);
    }

    // Update UI Language to the user's language
    Core::ICore::instance()->translators()->changeLanguage(settings()->value(Core::Constants::S_PREFERREDLANGUAGE, user->languageIso()).toString());
    Q_EMIT userConnected(uuid);
    d->checkNullUser();
    return true;
}

/**
  Define the current user as the database server manager. The unique user that
  can create users at startup time.
  \sa UserPlugin::UserCreationPage
*/
bool UserModel::setCurrentUserIsServerManager()
{
    if (WarnAllProcesses)
        qWarning() << Q_FUNC_INFO;
    d->checkNullUser();

    //    if (userBase()->database().driverName()=="QSQLITE") {
    //        return false;
    //    }
    if (!d->m_Sql->database().isOpen()) {
        if (!d->m_Sql->database().open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(d->m_Sql->database().connectionName().arg(d->m_Sql->database().lastError().text())));
            return false;
        }
    }
    QList<IUserListener *> listeners = pluginManager()->getObjects<IUserListener>();

    // 1. Ask all listeners to prepare the current user disconnection
    foreach(IUserListener *l, listeners) {
        if (!l->userAboutToChange())
            return false;
    }

    // 2. Check "in memory" users
    QString uuid = ::SERVER_ADMINISTRATOR_UUID;
    Internal::UserData *u = d->m_Uuid_UserList.value(uuid, 0);
    if (!u) {
        u = new Internal::UserData(uuid);
        u->setUsualName(tr("Database server administrator"));
        u->setRights(Constants::USER_ROLE_USERMANAGER, Core::IUser::AllRights);
        u->setModified(false);
        d->m_Uuid_UserList.insert(uuid, u);
    }

    // 3. Ask all listeners for the current user disconnection
    if (!d->m_CurrentUserUuid.isEmpty()) {
        Q_EMIT userAboutToDisconnect(d->m_CurrentUserUuid);
        foreach(IUserListener *l, listeners) {
            if (!l->currentUserAboutToDisconnect())
                return false;
        }
    }
    Q_EMIT userDisconnected(d->m_CurrentUserUuid);

    // 4. Connect new user
    Q_EMIT(userAboutToConnect(uuid));
    LOG(tr("Setting current user uuid to %1 (su)").arg(uuid));
    d->m_CurrentUserRights = Core::IUser::AllRights;
    d->m_CurrentUserUuid = uuid;
    foreach(Internal::UserData *user, d->m_Uuid_UserList.values()) {
        if (!user || user->uuid().isEmpty()) {
            // paranoid code -> if one user is corrupt -> clear all
            LOG_ERROR("Null user in model");
            qDeleteAll(d->m_Uuid_UserList);
            d->m_Uuid_UserList.clear();
            u = new Internal::UserData(uuid);
            u->setUsualName(tr("Database server administrator"));
            u->setRights(Constants::USER_ROLE_USERMANAGER, Core::IUser::AllRights);
            u->setCurrent(false);
            d->m_Uuid_UserList.insert(uuid, u);
            break;
        }
        user->setCurrent(false);
    }
    u->setCurrent(true);
    u->setModified(false);

    LOG(tkTr(Trans::Constants::CONNECTED_AS_1).arg(u->fullName()));
    foreach(IUserListener *l, listeners)
        l->newUserConnected(d->m_CurrentUserUuid);
    Q_EMIT userConnected(uuid);
    d->checkNullUser();
    return true;
}

/** Return true if a current user has been defined. */
bool UserModel::hasCurrentUser() const
{
    if (WarnAllProcesses)
        qWarning() << Q_FUNC_INFO << (!d->m_CurrentUserUuid.isEmpty());
    d->checkNullUser();
    return (!d->m_CurrentUserUuid.isEmpty());
}

/** Return the index of the current user. */
QModelIndex UserModel::currentUserIndex() const
{
    if (d->m_CurrentUserUuid.isEmpty())
        return QModelIndex();
    d->checkNullUser();
    QModelIndexList list = match(this->index(0, Core::IUser::Uuid), Qt::DisplayRole, d->m_CurrentUserUuid, 1);
    if (list.count() == 1) {
        return list.at(0);
    }
    return QModelIndex();
}

/**
 * Force a reset of the model:
 * - clear any internal cache
 * - reset the model
 */
void UserModel::forceReset()
{
    beginResetModel();
    d->clearCache();
    d->m_Sql->select();
    endResetModel();
}

/** Clears the content of the model. Silently save users if needed. */
void UserModel::clear()
{
    if (WarnAllProcesses)
        qWarning() << Q_FUNC_INFO;
    d->checkNullUser();
    // d->m_Sql ?
    submitAll();
    d->m_CurrentUserRights = 0;
    d->m_CurrentUserUuid.clear();
    qDeleteAll(d->m_Uuid_UserList);
    d->m_Uuid_UserList.clear();
}

void UserModel::refresh()
{
    beginResetModel();
    if (WarnAllProcesses)
        qWarning() << Q_FUNC_INFO;
    d->checkNullUser();
    clear();
    d->m_Sql->select();
    endResetModel();
}

/** Check login/password validity. \sa UserBase::checkLogin(). */
bool UserModel::isCorrectLogin(const QString &clearLog, const QString &clearPassword)
{
    d->checkNullUser();
    return userBase()->checkLogin(clearLog, clearPassword);
}

/**
 \todo alert when user to delete is modified ?
 \todo pb when userviewer is showing the index to delete
*/
bool UserModel::removeRows(int row, int count, const QModelIndex &)
{
    if (WarnAllProcesses)
        qWarning() << Q_FUNC_INFO;
    d->checkNullUser();
    Internal::UserData *user = d->m_Uuid_UserList.value(d->m_CurrentUserUuid,0);
    if (user) {
        Core::IUser::UserRights umRights = Core::IUser::UserRights(user->rightsValue(USER_ROLE_USERMANAGER).toInt());
        if (!umRights.testFlag(Core::IUser::Delete))
            return false;
    } else {
        LOG_ERROR("No current user");
        return false;
    }

    bool noError = true;
    beginRemoveRows(QModelIndex(), row, row+count);
    // TODO: alert when user to delete is modified?
    // TODO: pb when userviewer is showing the index to delete
    int i = 0;
    for (i=0; i < count ; i++) {
        QString uuid = d->m_Sql->index(row+i , USER_UUID).data().toString();

        if (uuid == d->m_CurrentUserUuid) {
            Utils::okCancelMessageBox(tr("User can not be deleted."),
                                      tr("You can not delete your own user."),
                                      "",
                                      qApp->applicationName());
            // TODO: add a button (change su password)
            continue;
        }

        // Delete from QHash
        if (d->m_Uuid_UserList.keys().contains(uuid)) {
            if (d->m_Uuid_UserList.value(uuid)->isModified())  {
                LOG_ERROR(tr("You can not delete a modified user, save it before."));
                noError = false;
            } else {
                Internal::UserData *deleteme = d->m_Uuid_UserList.value(uuid,0);
                delete deleteme;
                deleteme = 0;
                d->m_Uuid_UserList.remove(uuid);
            }
        }

        // Delete from real database
        if (!userBase()->purgeUser(uuid)) {
            LOG_ERROR(tr("User can not be deleted from database."));
            noError = false;
        }
    }
    endRemoveRows();

    beginResetModel();
    d->m_Sql->select();
    endResetModel();
    d->checkNullUser();
    return noError;
}

/** Create a new row (new user) into the model. */
bool UserModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (WarnAllProcesses)
        qWarning() << Q_FUNC_INFO;
    d->checkNullUser();
    if (!d->m_CurrentUserRights.testFlag(Core::IUser::Create))
        return false;
    int i=0;
    for (i=0; i<count; i++) {
        // create a new empty tkUser
        if (!d->m_Sql->insertRows(row + i, 1, parent)) {
            LOG_ERROR(QString("Can not create a new user into SQL Table."));
            return i;
        }
        // feed the QSqlTableModel with uuid and crypted empty password
        QString uuid = d->createNewEmptyUser(this, row+i);
        //Internal::UserData *user = d->m_Uuid_UserList.value(uuid, 0);
        QModelIndex newIndex = index(row+i, Core::IUser::Uuid);
        if (!d->m_Sql->setData(newIndex, uuid, Qt::EditRole)) {
            LOG_ERROR(QString("Can not add user's uuid into the new user into SQL Table. Row = %1 , UUID = %2 ")
                      .arg(row+i).arg(uuid));
            return i;
        }
        newIndex = index(row+i, Core::IUser::CryptedPassword);
        Utils::PasswordCrypter crypter;
        if (!d->m_Sql->setData(newIndex, crypter.cryptPassword(""), Qt::EditRole)) {
            LOG_ERROR(QString("Can not add user's login into the new user into SQL Table. Row = %1 , UUID = %2 ")
                      .arg(row+i).arg(uuid));
            return i;
        }
    }
    d->checkNullUser();
    return i;
}

int UserModel::rowCount(const QModelIndex &parent) const
{
    // prevent trees
    if (parent.isValid()) return 0;

    return d->m_Sql->rowCount();
}

int UserModel::columnCount(const QModelIndex &) const
{
    return Core::IUser::NumberOfColumns;
}

Qt::ItemFlags UserModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

/**
 * Define the data of users. \n
 * For index creation, you must use the Core::IUser enumerator for the column.\n
 * Using the Core::IUser::ClearPassword has column, defines the users'
 * clear password: user's password is changed from the old to the new data
 * (see also: UserPlugin::Internal::UserBase::changeUserPassword()).
*/
bool UserModel::setData(const QModelIndex &item, const QVariant &value, int role)
{
    if (!item.isValid())
        return false;

    // work only for EditRole
    if (role != Qt::EditRole)
        return false;

    // get uuid from real database
    QString uuid = d->m_Sql->data(d->m_Sql->index(item.row(), USER_UUID), Qt::DisplayRole).toString();
    if (uuid.isEmpty()) {
        LOG_ERROR(QString("Wrong uuid, Index(%1,%2)").arg(item.row()).arg(item.column()));
        return false;
    }

    // Check user rights
    if (!d->userCanWriteData(uuid))
        return false;

    // Get userdata pointer from cache/database
    if (!d->m_Uuid_UserList.keys().contains(uuid)) {
        d->addUserFromDatabase(uuid);
    }
    Internal::UserData *user = d->m_Uuid_UserList.value(uuid, 0);
    if (!user) {
        LOG_ERROR("No user for uuid " + uuid);
        return false;
    }

    // set data directly into database using QSqlTableModel if possible
    if (item.column() < USER_MaxParam) {
        // prepare SQL update
        QModelIndex sqlIndex = d->m_Sql->index(item.row(), item.column());
        if (!d->m_Sql->setData(sqlIndex, value, role)) {
            LOG_ERROR(QString("enable to setData to SqlModel. Row %1, col %2, data %3")
                      .arg(sqlIndex.row()).arg(sqlIndex.column()).arg(value.toString()));
            return false;
        }
        // continue to feed UserData (need to know if it is modified)
    }

    QList<int> colsToEmit;
    colsToEmit << item.column();

    switch (item.column())
    {
    case Core::IUser::Id :  user->setId(value); break;
    case Core::IUser::Uuid :  user->setUuid(value.toString()); break;
    case Core::IUser::Validity :  user->setValidity(value.toBool()); break;
    case Core::IUser::Login64 :  user->setLogin64(value); break;
    case Core::IUser::DecryptedLogin : user->setLogin64(value.toString().toUtf8().toBase64()); break;
    case Core::IUser::ClearPassword :
    {
        // When a clear password is defined through the model
        // the model reacts **in real time**. The user password
        // is updated on the database server and inside the
        // app user database. You do not need to call submit()
        QString oldPass = user->clearPassword();
        user->setClearPassword(value.toString());
        if (!userBase()->changeUserPassword(user, value.toString()))
            user->setClearPassword(oldPass);
        user->setPasswordModified(false); // as we just saved it or revert the change
        break;
    }
    case Core::IUser::CryptedPassword :  user->setCryptedPassword(value); break;
    case Core::IUser::LastLoggedIn :  user->setLastLoggedIn(value); break;
    case Core::IUser::GenderIndex :
        colsToEmit << Core::IUser::Gender << Core::IUser::FullName<< Core::IUser::FullHtmlContact;
        user->setGenderIndex(value.toInt());
        break;
    case Core::IUser::TitleIndex :
        colsToEmit << Core::IUser::Title << Core::IUser::TitleIndex << Core::IUser::FullName<< Core::IUser::FullHtmlContact;
        user->setTitleIndex(value.toInt());
        break;
    case Core::IUser::UsualName:
        colsToEmit << Core::IUser::FullName<< Core::IUser::FullHtmlContact;
        user->setUsualName(value);
        break;
    case Core::IUser::OtherNames:
        colsToEmit << Core::IUser::FullName<< Core::IUser::FullHtmlContact;
        user->setOtherNames(value);
        break;
    case Core::IUser::Firstname :
        colsToEmit << Core::IUser::FullName<< Core::IUser::FullHtmlContact;
        user->setFirstname(value);
        break;
    case Core::IUser::Mail :  user->setMail(value); break;
    case Core::IUser::LanguageISO :  user->setLanguageIso(value); break;
    case Core::IUser::LocaleCodedLanguage: user->setLocaleLanguage(QLocale::Language(value.toInt())); break;
    case Core::IUser::PhotoPixmap: user->setPhoto(value.value<QPixmap>()); break;
    case Core::IUser::DateOfBirth : user->setDob(value); break;
    case Core::IUser::Street:
        colsToEmit << Core::IUser::FullHtmlAddress << Core::IUser::FullHtmlContact;
        user->setStreet(value);
        break;
    case Core::IUser::Zipcode:
        colsToEmit << Core::IUser::FullHtmlAddress << Core::IUser::FullHtmlContact;
        user->setZipcode(value);
        break;
    case Core::IUser::StateProvince:
        colsToEmit << Core::IUser::FullHtmlAddress << Core::IUser::FullHtmlContact;
        user->setStateProvince(value);
        break;
    case Core::IUser::City :
        colsToEmit << Core::IUser::FullHtmlAddress << Core::IUser::FullHtmlContact;
        user->setCity(value);
        break;
    case Core::IUser::Country :
        colsToEmit << Core::IUser::FullHtmlAddress << Core::IUser::FullHtmlContact;
        user->setCountry(value);
        break;
    case Core::IUser::IsoCountry :
        colsToEmit << Core::IUser::FullHtmlAddress << Core::IUser::FullHtmlContact;
        user->setCountryIso(value);
        break;
    case Core::IUser::Tel1 :
        colsToEmit << Core::IUser::FullHtmlContact;
        user->setTel1(value);
        break;
    case Core::IUser::Tel2 :
        colsToEmit << Core::IUser::FullHtmlContact;
        user->setTel2(value);
        break;
    case Core::IUser::Tel3 :
        colsToEmit << Core::IUser::FullHtmlContact;
        user->setTel3(value);
        break;
    case Core::IUser::Fax :
        colsToEmit << Core::IUser::FullHtmlContact;
        user->setFax(value);
        break;
    case Core::IUser::ProfessionalIdentifiants :  user->setPractitionerIdentifiant(value.toStringList()); break;
    case Core::IUser::Specialities :  user->setSpecialty(value.toStringList()); break;
    case Core::IUser::Qualifications :  user->setQualification(value.toStringList()); break;
    case Core::IUser::Preferences :  user->setPreferences(value); break;
    case Core::IUser::DataPackConfig: return userBase()->saveUserDynamicData(user->uuid(), Constants::USER_DATA_DATAPACK_CONFIG, value);

        // TODO: Add Xml extra document to model
    case Core::IUser::GenericHeader : user->setExtraDocumentHtml(value, Core::IUser::GenericHeader); Q_EMIT(userDocumentsChanged()); break;
    case Core::IUser::GenericFooter :  user->setExtraDocumentHtml(value, Core::IUser::GenericFooter); Q_EMIT(userDocumentsChanged()); break;
    case Core::IUser::GenericWatermark :  user->setExtraDocumentHtml(value, Core::IUser::GenericWatermark); Q_EMIT(userDocumentsChanged()); break;
    case Core::IUser::GenericHeaderPresence : user->setExtraDocumentPresence(value.toInt(), Core::IUser::GenericHeader); Q_EMIT(userDocumentsChanged()); break;
    case Core::IUser::GenericFooterPresence : user->setExtraDocumentPresence(value.toInt(), Core::IUser::GenericFooter); Q_EMIT(userDocumentsChanged()); break;
    case Core::IUser::GenericWatermarkPresence : user->setExtraDocumentPresence(value.toInt(), Core::IUser::GenericWatermark); Q_EMIT(userDocumentsChanged()); break;

    case Core::IUser::AdministrativeHeader : user->setExtraDocumentHtml(value, Core::IUser::AdministrativeHeader); Q_EMIT(userDocumentsChanged()); break;
    case Core::IUser::AdministrativeFooter : user->setExtraDocumentHtml(value, Core::IUser::AdministrativeFooter); Q_EMIT(userDocumentsChanged()); break;
    case Core::IUser::AdministrativeWatermark : user->setExtraDocumentHtml(value, Core::IUser::AdministrativeWatermark); Q_EMIT(userDocumentsChanged()); break;
    case Core::IUser::AdministrativeHeaderPresence : user->setExtraDocumentPresence(value.toInt(), Core::IUser::AdministrativeHeader); Q_EMIT(userDocumentsChanged()); break;
    case Core::IUser::AdministrativeFooterPresence : user->setExtraDocumentPresence(value.toInt(), Core::IUser::AdministrativeFooter); Q_EMIT(userDocumentsChanged()); break;
    case Core::IUser::AdministrativeWatermarkPresence : user->setExtraDocumentPresence(value.toInt(), Core::IUser::AdministrativeWatermark); Q_EMIT(userDocumentsChanged()); break;

    case Core::IUser::PrescriptionHeader : user->setExtraDocumentHtml(value, Core::IUser::PrescriptionHeader); Q_EMIT(userDocumentsChanged()); break;
    case Core::IUser::PrescriptionFooter : user->setExtraDocumentHtml(value, Core::IUser::PrescriptionFooter); Q_EMIT(userDocumentsChanged()); break;
    case Core::IUser::PrescriptionWatermark : user->setExtraDocumentHtml(value, Core::IUser::PrescriptionWatermark); Q_EMIT(userDocumentsChanged()); break;
    case Core::IUser::PrescriptionHeaderPresence : user->setExtraDocumentPresence(value.toInt(), Core::IUser::PrescriptionHeader); Q_EMIT(userDocumentsChanged()); break;
    case Core::IUser::PrescriptionFooterPresence : user->setExtraDocumentPresence(value.toInt(), Core::IUser::PrescriptionFooter); Q_EMIT(userDocumentsChanged()); break;
    case Core::IUser::PrescriptionWatermarkPresence : user->setExtraDocumentPresence(value.toInt(), Core::IUser::PrescriptionWatermark); Q_EMIT(userDocumentsChanged()); break;

    case Core::IUser::ManagerRights : user->setRights(USER_ROLE_USERMANAGER, Core::IUser::UserRights(value.toInt())); break;
    case Core::IUser::MedicalRights : user->setRights(USER_ROLE_MEDICAL, Core::IUser::UserRights(value.toInt())); break;
    case Core::IUser::DrugsRights : user->setRights(USER_ROLE_DOSAGES, Core::IUser::UserRights(value.toInt())); break;
    case Core::IUser::ParamedicalRights : user->setRights(USER_ROLE_PARAMEDICAL, Core::IUser::UserRights(value.toInt())); break;
    case Core::IUser::SecretaryRights : user->setRights(USER_ROLE_SECRETARY, Core::IUser::UserRights(value.toInt())); break;
    case Core::IUser::AgendaRights : user->setRights(USER_ROLE_AGENDA, Core::IUser::UserRights(value.toInt())); break;

    default : return false;
    };

    for(int i=0; i < colsToEmit.count(); ++i)
        Q_EMIT dataChanged(index(item.row(), i), index(item.row(), i));

    return true;
}

/** Returns the data refering to the \e column (see Core::IUser::DataRepresentation) */
QVariant UserModel::currentUserData(const int column) const
{
    if (WarnAllProcesses)
        qWarning() << Q_FUNC_INFO << column;
    d->checkNullUser();

    // No current user?
    if (d->m_CurrentUserUuid.isEmpty())
        return QVariant();

    // Check user rights
    if (!d->userCanReadData(d->m_CurrentUserUuid))
        return QVariant();

    // Get data
    const Internal::UserData *user = d->m_Uuid_UserList.value(d->m_CurrentUserUuid, 0);
    if (!user)
        return QVariant();
    return d->getUserData(user, column);
}

/** Returns the data of users. \sa Core::IUser::DataRepresentation */
QVariant UserModel::data(const QModelIndex &item, int role) const
{
    if (!item.isValid())
        return QVariant();

    // get user
    QString uuid = d->m_Sql->data(d->m_Sql->index(item.row(), USER_UUID), Qt::DisplayRole).toString();
    if (uuid.isEmpty()) {
        LOG_ERROR(QString("Wrong uuid, Index(%1,%2)").arg(item.row()).arg(item.column()));
        return QVariant();
    }

    // Check user rights
    if (!d->userCanReadData(uuid))
        return QVariant();

    if (uuid==d->m_CurrentUserUuid && (role==Qt::DisplayRole || role==Qt::EditRole)) {
        return currentUserData(item.column());
    }
    QVariant toReturn;

    // First manage decoration WITHOUT retrieving any user from database
    if (role == Qt::FontRole) {
        QFont font;
        if (d->m_Uuid_UserList.keys().contains(uuid)) {
            Internal::UserData *user = d->m_Uuid_UserList.value(uuid,0);
            if (!user)
                return QVariant();
            if (user->isModified())
                font.setBold(true);
            else
                font.setBold(false);
        } else
            font.setBold(false);
        return font;
    }
    else if (role == Qt::BackgroundRole) {
        QColor c;
        if (d->m_Uuid_UserList.keys().contains(uuid)) {
            if (d->m_Uuid_UserList.value(uuid)->isModified())
                c = QColor(Qt::red);
            else
                c = QColor(Qt::white);
        } else
            c = QColor(Qt::white);
        return c;
    }
    else if ((role == Qt::DisplayRole) || (role == Qt::EditRole)) {
        // From internal SQL?
        if ((item.column() < Core::IUser::LocaleLanguage)) {
            QModelIndex sqlIndex = d->m_Sql->index(item.row(), item.column());
            return d->m_Sql->data(sqlIndex, role);
        }

        // From complete UserData ?
        if (!d->m_Uuid_UserList.keys().contains(uuid)) {
            d->addUserFromDatabase(uuid);
        }
        const Internal::UserData *user = d->m_Uuid_UserList.value(uuid,0);
        Q_ASSERT(user);
        return d->getUserData(user, item.column());
    }
    return toReturn;
}

/** Not implemented */
void UserModel::setSort(int /*column*/, Qt::SortOrder /*order*/)
{
}
/** Not implemented */
void UserModel::sort(int /*column*/, Qt::SortOrder /*order*/)
{
}
/** Not implemented */
void UserModel::setTable(const QString &/*tableName*/)
{
}
/** Not implemented */
void UserModel::setFilter(const QString &/*filter*/)
{
}

/**
 * \e uuid
 */

/**
 * Set the paper for the user.
 * @param uuid: user uuid
 * @param ref: refers to Core::IUser::{Generic,Administrative,Prescription}{Header,Footer,Watermark}
 * @param extra: the Print::TextDocumentExtra
 * @return \e true if all goods fine
 */
bool UserModel::setPaper(const QString &uuid, const int ref, Print::TextDocumentExtra *extra)
{
    if (WarnAllProcesses)
        qWarning() << Q_FUNC_INFO << uuid << ref << extra;
    d->checkNullUser();
    Internal::UserData *user = d->m_Uuid_UserList.value(uuid,0);
    if (!user)
        return false;
    user->setExtraDocument(extra, ref);
    user->setModified(true);
    return true;
}

Print::TextDocumentExtra *UserModel::paper(const int row, const int ref)
{
    d->checkNullUser();
    QString uuid = d->m_Sql->data(d->m_Sql->index(row, USER_UUID), Qt::DisplayRole).toString();
    if (WarnAllProcesses)
        qWarning() << Q_FUNC_INFO << uuid << ref << uuid;
    Internal::UserData *user = d->m_Uuid_UserList.value(uuid,0);
    if (!user)
        return 0;
    return user->extraDocument(ref);
}

/** Returns true if model has dirty rows that need to be saved into database. */
bool UserModel::isDirty() const
{
    if (WarnAllProcesses)
        qWarning() << Q_FUNC_INFO;
    d->checkNullUser();
    foreach(const Internal::UserData *u, d->m_Uuid_UserList.values()) {
        if (!u || u->uuid().isEmpty()) {
            // paranoid code
            LOG_ERROR("Null user in model");
            qWarning() << d->m_Uuid_UserList;
            qDeleteAll(d->m_Uuid_UserList.values(""));
            d->m_Uuid_UserList.remove(0);
            continue;
        }
        if (u->isModified()) {
            if (WarnAllProcesses)
                qWarning() << u->uuid() << "isModified";
            return true;
        }
    }
    return false;
}

/** Submit all changes of the model into database */
bool UserModel::submitAll()
{
    if (WarnAllProcesses)
        qWarning() << Q_FUNC_INFO;
    d->checkNullUser();
    bool toReturn = true;
    foreach(const QString &s, d->m_Uuid_UserList.keys()) {
        if (!submitUser(s))
            toReturn = false;
    }
    return toReturn;
}

/** Submit only one user changes of the model into database according to the current user rights. */
bool UserModel::submitUser(const QString &uuid)
{
    if (WarnAllProcesses)
        qWarning() << Q_FUNC_INFO << uuid;
    d->checkNullUser();

    if (uuid==::SERVER_ADMINISTRATOR_UUID)
        return true;

    bool toReturn = true;

    Internal::UserData *user = d->m_Uuid_UserList.value(uuid, 0);
    if (WarnAllProcesses)
        qWarning() << user;

    if (!user)
        return false;

    if (WarnAllProcesses)
        qWarning() << "modified" << user->isModified();

    // act only on modified users
    if (user->isModified()) {
        if (!d->userCanWriteData(uuid)) {
            LOG_ERROR("Not enought rights to save data");
            return false;
        }
        if (!userBase()->saveUser(user))
            toReturn = false;
    }

    if (WarnAllProcesses)
        qWarning() << "saved" << toReturn;

    d->checkNullUser();
    return toReturn;
}

bool UserModel::submitRow(const int row)
{
    return submitUser(index(row, Core::IUser::Uuid).data().toString());
}

/** Reverts the model. */
bool UserModel::revertAll()
{
    if (WarnAllProcesses)
        qWarning() << Q_FUNC_INFO;
    d->checkNullUser();
    // FIXME: ASSERT failure in QSqlTableModelPrivate::revertCachedRow(): "Invalid entry in cache map", file models\qsqltablemodel.cpp, line 151
    int i = 0;
    for(i=0; i < rowCount() ; i++)
        revertRow(i);

    beginResetModel();
    d->m_Sql->select();
    endResetModel();
    d->checkNullUser();
    return true;
}

/** Revert a row */
void UserModel::revertRow(int row)
{
    beginResetModel();
    d->checkNullUser();
    QString uuid = d->m_Sql->index(row, USER_UUID).data().toString();
    if (WarnAllProcesses)
        qWarning() << Q_FUNC_INFO << row << uuid;
    d->m_Sql->revertRow(row);
    if (d->m_Uuid_UserList.keys().contains(uuid)) {
        Internal::UserData *deleteme = d->m_Uuid_UserList.value(uuid,0);
        delete deleteme;
        deleteme = 0;
        d->m_Uuid_UserList.remove(uuid);
    }
    endResetModel();
    d->checkNullUser();
}

/**
 * Define the filter of the model. The QHash keys are Core::IUser datarepresentation.
 * \sa Utils::Database::getWhereClause()
*/
void UserModel::setFilter(const QHash<int,QString> &conditions)
{
    d->checkNullUser();
    // TODO: filter by name AND Firstname at the same time
    QString filter = "";
    const Internal::UserBase *b = userBase();
    foreach(const int r, conditions.keys()) {
        QString baseField = "";
        switch (r)
        {
        case Core::IUser::Uuid : baseField = b->fieldName(Table_USERS, USER_UUID); break;
        case Core::IUser::UsualName : baseField = b->fieldName(Table_USERS, USER_USUALNAME); break;
        case Core::IUser::Firstname : baseField = b->fieldName(Table_USERS, USER_FIRSTNAME); break;
        default: break;
        }
        if (baseField.isEmpty())
            continue;
        filter += QString("(`%1` %2) AND\n").arg(baseField, conditions.value(r));
    }
    filter.chop(5);
    beginResetModel();
    d->m_Sql->setFilter(filter);
    d->m_Sql->select();
    endResetModel();
    //    qWarning() << filter;
    d->checkNullUser();
}

/** Get all user's name from their \e uids.
 * The returned QHash contains as key the uid of users, and as value their name. */
QHash<QString, QString> UserModel::getUserNames(const QStringList &uids)  // static
{
    QHash<QString, QString> toReturn;
    QHash<int, QString> where;
    userBase()->database().transaction();
    QSqlQuery query(userBase()->database());
    for(int i = 0; i < uids.count(); ++i) {
        where.clear();
        where.insert(Constants::USER_UUID, QString("='%1'").arg(uids.at(i)));
        QString req = userBase()->select(Constants::Table_USERS,
                                         QList<int>()
                                         << Constants::USER_TITLE
                                         << Constants::USER_USUALNAME
                                         << Constants::USER_FIRSTNAME
                                         << Constants::USER_OTHERNAMES
                                         , where);
        if (query.exec(req)) {
            if (query.next()) {
                QString name = QString("%1 %2 %3 %4")
                        .arg(titles().at(query.value(0).toInt()))
                        .arg(query.value(1).toString())
                        .arg(query.value(2).toString())
                        .arg(query.value(3).toString());
                name = name.simplified();
                toReturn.insert(uids.at(i), name);
            }
        } else {
            LOG_QUERY_ERROR_FOR("UserModel", query);
        }
        query.finish();
    }
    userBase()->database().commit();
    return toReturn;
}

bool UserModel::createVirtualUsers(const int count)
{
    bool ok = true;
    Utils::Randomizer r;
    r.setPathToFiles(settings()->path(Core::ISettings::BundleResourcesPath) + "/textfiles/");

    for(int i = 0; i < count ; ++i) {
        Internal::UserData *u = new Internal::UserData;

        int genderIndex = r.randomInt(1);
        QString name = r.randomName();
        QString firstName = r.randomFirstName(genderIndex==1);

        u->setUsualName(name);
        u->setFirstname(firstName);
        u->setTitleIndex(r.randomInt(0, 4));
        u->setGenderIndex(genderIndex);
        u->setValidity(true);

        // Create log and pass
        QString pass = name + "." + firstName;
        pass = pass.toLower();
        pass = Utils::removeAccents(pass);
        u->setLogin64(Utils::loginForSQL(pass));
        u->setClearPassword(pass);

        if (!userBase()->createUser(u))
            return false;
    }
    return ok;
}

/** Returns the number of user stored into the memory. */
int UserModel::numberOfUsersInMemory() const
{
    return d->m_Uuid_UserList.count();
}

/** Used by UserManagerPlugin to inform the currently connected user after Core is opened. */
void UserModel::emitUserConnected()
{
    Q_EMIT userConnected(d->m_CurrentUserUuid);
}

/**
 * Get and save user preferences to the user database.
 * \sa Core::ISettings::userSettingsSynchronized()
 */
void UserModel::updateUserPreferences()
{
    WARN_FUNC;
    if (WarnAllProcesses)
        qWarning() << Q_FUNC_INFO;
    d->checkNullUser();
    if (!d->m_CurrentUserUuid.isEmpty() && d->m_CurrentUserUuid!=::SERVER_ADMINISTRATOR_UUID) {
        // save user preferences
        Internal::UserData *user = d->m_Uuid_UserList.value(d->m_CurrentUserUuid, 0);
        if (user) {
            user->setPreferences(settings()->userSettings());
            userBase()->saveUserPreferences(user->uuid(), settings()->userSettings());
            if (user->hasModifiedDynamicDataToStore()) {
                // save papers ?
                userBase()->savePapers(user);
            }
        } else {
            LOG_ERROR("No user uuid");
        }
    }
}

void UserModel::checkUserPreferencesValidity()
{
    WARN_FUNC;
    disconnect(settings(), SIGNAL(userSettingsSynchronized()), this, SLOT(updateUserPreferences()));
    QList<Core::IOptionsPage *> prefs = pluginManager()->getObjects<Core::IOptionsPage>();
    if (commandLine()->value(Core::ICommandLine::ResetUserPreferences).toBool()) {
        for(int i=0; i < prefs.count(); ++i) {
            prefs.at(i)->resetToDefaults();
        }
    } else {
        for(int i=0; i < prefs.count(); ++i) {
            prefs.at(i)->checkSettingsValidity();
        }
    }
    updateUserPreferences();
    connect(settings(), SIGNAL(userSettingsSynchronized()), this, SLOT(updateUserPreferences()));
}
