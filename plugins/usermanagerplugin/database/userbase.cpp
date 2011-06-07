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
  \class UserPlugin::Internal::UserBase
  \brief This class owns the user database mechanism. It should never be directly accessed. Use tkUserModel to access to the database.

  1. Initialization\n
  This class owns a singleton. To instanciate it, use instance(). When instanciate for the first time,
  the initialize() member is called.

  2. Users retreiver\n
  You can retreive users using getUserByUuid() or using getUserByLoginPassword().

  3. Users saver\n
  You can regardless save or update users to database using the unique member : saveUser().

  4. Users datas checkers\n
  You can check the identifiants of users with the checkLogin() member.

  5. Users trace keeper\n
  The recaordLastLogin() update user's database with the actual login's date and time. It also update the login
  history data of the user. This login history need to be saved by hand (saveUser()).
*/

#include "userbase.h"
#include "constants.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <printerplugin/textdocumentextra.h>
#include <coreplugin/iuser.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/databaseconnector.h>

#include <usermanagerplugin/userdata.h>

#include <QCoreApplication>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QFile>
#include <QDir>
#include <QLocale>
#include <QDateTime>
#include <QUuid>

using namespace UserPlugin::Internal;
using namespace UserPlugin::Constants;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }


// Initializing static datas
bool UserBase::m_initialized = false;
QString  UserBase::m_LastUuid = "";
QString  UserBase::m_LastLogin = "";
QString  UserBase::m_LastPass = "";
UserBase* UserBase::m_Instance = 0;

/**
  \brief Returns the unique instance of UserBase. If the instance does not exist it is created.
  You should never construct a instance of this object using the constructor.
*/
UserBase *UserBase::instance()
{
    if (!m_Instance)
        m_Instance = new UserBase(qApp);
    return m_Instance;
}

/**
  \brief Constructor, inform Utils::Database of the database scheme.
*/
UserBase::UserBase(QObject *parent)
        : QObject(parent), Utils::Database()
{
    setObjectName("UserBase");
    m_initialized = false;
    m_IsNewlyCreated = false;

    // populate tables and fields of database
    addTable(Table_USERS,  "USERS");
    addTable(Table_DATAS,  "DATAS");
    addTable(Table_RIGHTS, "RIGHTS");

    addField(Table_USERS, USER_ID,           "USER_ID",         FieldIsUniquePrimaryKey);
    addField(Table_USERS, USER_UUID,         "USER_UUID",       FieldIsUUID);
    addField(Table_USERS, USER_VALIDITY,     "VALIDITY",        FieldIsBoolean);
    addField(Table_USERS, USER_LOGIN,        "LOGIN",           FieldIsShortText);
    addField(Table_USERS, USER_PASSWORD,     "PASSWORD",        FieldIsShortText);
    addField(Table_USERS, USER_LASTLOG,      "LASTLOGIN",       FieldIsDate);
    addField(Table_USERS, USER_NAME,         "NAME",            FieldIsShortText);
    addField(Table_USERS, USER_SECONDNAME,   "SECONDNAME",      FieldIsShortText);
    addField(Table_USERS, USER_FIRSTNAME,    "SURNAME",         FieldIsShortText);
    addField(Table_USERS, USER_MAIL,         "MAIL",            FieldIsShortText);
    addField(Table_USERS, USER_LANGUAGE,     "LANGUAGE",        FieldIsLanguageText);
    addField(Table_USERS, USER_LOCKER,       "LOCKER",          FieldIsBoolean);

    addField(Table_DATAS, DATAS_ID,          "DATAS_ID",        FieldIsUniquePrimaryKey);
    addField(Table_DATAS, DATAS_USER_UUID,   "USER_UUID",       FieldIsUUID);
    addField(Table_DATAS, DATAS_DATANAME,    "DATANAME",        FieldIsShortText);
    addField(Table_DATAS, DATAS_STRING,      "DATA_STRING",     FieldIsShortText);
    addField(Table_DATAS, DATAS_LONGSTRING,  "DATA_LONGSTRING", FieldIsLongText);
    addField(Table_DATAS, DATAS_FILE,        "DATA_FILE",       FieldIsBlob);
    addField(Table_DATAS, DATAS_NUMERIC,     "DATA_NUMERIC",    FieldIsInteger);
    addField(Table_DATAS, DATAS_DATE,        "DATA_DATE",       FieldIsDate);
    addField(Table_DATAS, DATAS_LANGUAGE,    "DATA_LANGUAGE",   FieldIsShortText);
    addField(Table_DATAS, DATAS_LASTCHANGE,  "LASTCHANGE",      FieldIsDate);
    addField(Table_DATAS, DATAS_TRACE_ID,    "TRACE_ID",        FieldIsInteger);

    addField(Table_RIGHTS, RIGHTS_ID,        "RIGHTS_ID",       FieldIsUniquePrimaryKey);
    addField(Table_RIGHTS, RIGHTS_USER_UUID, "USER_UUID",       FieldIsUUID);
    addField(Table_RIGHTS, RIGHTS_ROLE,      "RIGHTS_ROLE",     FieldIsShortText);
    addField(Table_RIGHTS, RIGHTS_RIGHTS,    "RIGHTS_RIGHTS",   FieldIsInteger);

    addField(Table_GROUPS, GROUPS_ID,               "GROUP_ID",   FieldIsUniquePrimaryKey);
    addField(Table_GROUPS, GROUPS_UID,              "GROUP_UID",  FieldIsUUID);
    addField(Table_GROUPS, GROUPS_USER_UID,         "USER_UID",   FieldIsUUID);
    addField(Table_GROUPS, GROUPS_PARENT_GROUP_UID, "PARENT_GROUP_UID", FieldIsUUID);

    // links
    addTable(Table_USER_LK_ID, "LK_USER");
    addField(Table_USER_LK_ID, LK_ID,         "ID", FieldIsUniquePrimaryKey);
    addField(Table_USER_LK_ID, LK_LKID,       "LK_ID", FieldIsUUID);
    addField(Table_USER_LK_ID, LK_USER_UUID,  "USER_UID", FieldIsUUID);
    addField(Table_USER_LK_ID, LK_GROUP_UUID, "GROUP_UID", FieldIsUUID);

    // informations
    addTable(Table_INFORMATIONS, "INFORMATIONS");
    addField(Table_INFORMATIONS, INFO_VERSION,  "VERSION", FieldIsShortText);
    addField(Table_INFORMATIONS, INFO_MAX_LKID, "MAX_LK_ID", FieldIsInteger);

    initialize(Core::ICore::instance()->settings());
}

/**
  \brief Initialize users base using the \e settings values.
  \sa Core::ISettings, Core::ISettings::ReadWriteDatabasesPath
*/
bool UserBase::initialize(Core::ISettings *s)
{
    Core::ISettings *set = s;
    if (!set)
        set = settings();

    if (m_initialized)
        return true;

    // connect
    createConnection(USER_DB_CONNECTION, USER_DB_CONNECTION,
                     settings()->databaseConnector(),
                     Utils::Database::CreateDatabase);

    if (!database().isOpen()) {
        if (!database().open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(USER_DB_CONNECTION).arg(database().lastError().text()));
        } else {
            LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().connectionName()).arg(database().driverName()));
        }
    } else {
        LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().connectionName()).arg(database().driverName()));
    }

    if (!checkDatabaseScheme()) {
        LOG_ERROR(tkTr(Trans::Constants::DATABASE_1_SCHEMA_ERROR).arg(USER_DB_CONNECTION));
        return false;
    }

    if (!checkDatabaseVersion())
        return false;

    m_initialized = true;
    return true;
}

/** Return true if the userbase is initialized and usable */
bool UserBase::isInitialized() const
{
    return m_initialized;
}

/** Return true if the userbase has just been created */
bool UserBase::isNewlyCreated() const
{
    return m_IsNewlyCreated;
}

/** Return true if the userbase is the last version (database is updated by this member if needed) */
bool UserBase::checkDatabaseVersion()
{
    /** \todo Code : UserBase::checkDatabaseVersion() */
    return true;
}


//--------------------------------------------------------------------------------------------------------
//------------------------------------------- Datas retreivers -------------------------------------------
//--------------------------------------------------------------------------------------------------------
/** \brief Retreive all users datas from the users' database. If an error occurs, it returns 0. */
UserData *UserBase::getUser(const QHash<int, QString> & conditions) const
{
    QSqlDatabase DB = QSqlDatabase::database(USER_DB_CONNECTION);
    if (!DB.isOpen()) {
        if (!DB.open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(database().connectionName()).arg(database().lastError().text()));
            return 0;
        }
    }

    // get USERS table
    QString req = select(Table_USERS, conditions);
    UserData *toReturn = 0;
    QString uuid = "";
    {
        QSqlQuery q(req , DB);
        if (q.isActive()) {
            if (q.next()) {
                if (q.record().count() != USER_MaxParam)
                    LOG_ERROR(QCoreApplication::translate("UserBase", "ERROR : will retreiving %1. Wrong number of fields")
                                     .arg(USER_DB_CONNECTION));
                int i = 0;
                uuid = q.value(USER_UUID).toString();
                toReturn = new UserData(uuid);
                for (i = 0; i < USER_MaxParam; ++i)
                    toReturn->setValue(Table_USERS, i , q.value(i));
            }
        } else {
            LOG_QUERY_ERROR(q);
        }
    }

    // get RIGHTS table  ***************************************** -1
    QHash<int, QString> where;
    where.insert(RIGHTS_USER_UUID, QString("='%1'").arg(uuid));
    req = select(Table_RIGHTS, where);
    {
        QSqlQuery q(req , DB);
        if (q.isActive()) {
            while (q.next()) {
                if (q.record().count() != RIGHTS_MaxParam)
                    LOG_ERROR(QCoreApplication::translate("UserBase",
                                                                                 "ERROR : will retreiving %1. Wrong number of fields")
                                         .arg("users' rights"));
                int i = 0;
                QByteArray id = q.value(RIGHTS_ROLE).toByteArray();
                for (i = 0; i < RIGHTS_MaxParam; ++i)
                    toReturn->addRightsFromDatabase(id , i , q.value(i));
            }
        } else {
            LOG_QUERY_ERROR(q);
        }
    }

    // get DATAS table  ***************************************** -1
    where.clear();
    where.insert(DATAS_USER_UUID, QString("='%1'").arg(uuid));
    req = select(Table_DATAS, where);
    QList<UserDynamicData*> list;
    {
        QSqlQuery q(req , DB);
        if (q.isActive()) {
            while (q.next()) {
                if (q.record().count() != DATAS_MaxParam)
                    LOG_ERROR(QCoreApplication::translate("UserBase",
                                                          "ERROR : will retreiving %1. Wrong number of fields")
                              .arg("users' rights"));
                int i = 0;
                UserDynamicData *data = new UserDynamicData();
                for (i = 0; i < DATAS_MaxParam; ++i) {
                    data->feedFromSql(i, q.value(i));
                }
                list << data;
            }
        } else {
            LOG_QUERY_ERROR(q);
        }
    }
    if (list.count())
        toReturn->addDynamicDatasFromDatabase(list);

    // get personal LINK_ID
    /** \todo this must be updated: manage groups too */
    where.clear();
    where.insert(Constants::LK_USER_UUID, QString("='%1'").arg(uuid));
    req = select(Constants::Table_USER_LK_ID, Constants::LK_LKID, where);
    int lkid = -1;
    {
        QSqlQuery q(req, DB);
        if (q.isActive()) {
            if (q.next()) {
                lkid = q.value(0).toInt();
            }
        } else {
            LOG_QUERY_ERROR(q);
        }
        if (lkid == -1) {
            /** \todo WARNING this causes segfault */
            LOG_ERROR(QString("No linker for user %1").arg(toReturn->uuid()));
            return 0;
        }
        toReturn->setPersonalLkId(lkid);
    }

    if (toReturn)
        toReturn->setModified(false);

    return toReturn;
}

/**
  \brief Retreive all users datas from the users' database. If an error occurs, it returns 0.
  \sa getUser()
*/
UserData *UserBase::getUserById(const QVariant & _id) const
{
    // retreive corresponding user
    // construct the where clause
    QString req = QString("=%1").arg(_id.toInt()) ;
    QHash<int, QString> where;
    where.insert(USER_ID, req);
    // get the asked user
    return getUser(where);
}

/**
  \brief Retreive all users datas from the users' database. If an error occurs, it returns 0.
  \sa getUser()
*/
UserData* UserBase::getUserByUuid(const QString & uuid) const
{
    // retreive corresponding user
    // construct the where clause
    QString req = QString("='%1'").arg(uuid) ;
    QHash<int, QString> where;
    where.insert(USER_UUID, req);
    // get the asked user
    return getUser(where);
}

/**
  \brief Retreive all users datas from the users' database. If an error occurs, it returns 0.
  \sa getUser()
*/
UserData *UserBase::getUserByLoginPassword(const QVariant &login, const QVariant &cryptedPassword) const
{
    // retreive corresponding user
    // construct the where clause
    QHash<int, QString> where;
    where.insert(USER_LOGIN, QString("='%1'").arg(login.toString()));
    where.insert(USER_PASSWORD, QString("='%1'").arg(cryptedPassword.toString()));
    // get the asked user
    return getUser(where);
}

/** Check the couple login/password passed as params. */
bool UserBase::checkLogin(const QString &clearLogin, const QString &clearPassword) const
{
    qWarning() << Q_FUNC_INFO << clearLogin << clearPassword;
    QSqlDatabase DB = QSqlDatabase::database(USER_DB_CONNECTION);
    if (!DB.isOpen()) {
        if (!DB.open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(DB.connectionName()).arg(DB.lastError().text()));
            return false;
        }
    }
    m_LastUuid.clear();
    m_LastLogin.clear();
    m_LastPass.clear();
    // Connect to drivers
    if (QSqlDatabase::connectionNames().contains("__ConnectionTest__")) {
        QSqlDatabase::removeDatabase("__ConnectionTest__");
    }
    switch (driver()) {
    case Utils::Database::MySQL:
        {
            // Try to connect with the new identifiers to MySQL server
            QSqlDatabase connectionTest = DB.cloneDatabase(DB, "__ConnectionTest__");
            connectionTest.setUserName(clearLogin);
            connectionTest.setPassword(clearPassword);
            if (!connectionTest.open()) {
                qWarning() << connectionTest;
                LOG_ERROR(QString("Unable to connect to the MySQL server, with user %1 : %2").arg(clearLogin).arg(clearPassword));
                return false;
            }
            LOG("Database server identifiers are correct");
            // Reconnect with these identifiers all databases
            break;
        }
    case Utils::Database::SQLite: break;
    case Utils::Database::PostSQL:
        {
            return false;
            break;
        }
    }

    // create query
    QList<int> list;
    list << USER_UUID << USER_LOGIN << USER_PASSWORD;
    QHash<int, QString> where;
    where.insert(USER_LOGIN, QString("='%1'").arg(Utils::loginForSQL(clearLogin)));
    where.insert(USER_PASSWORD, QString("='%1'").arg(Utils::cryptPassword(clearPassword)));
    QString req = select(Table_USERS, list, where);

    {
        QSqlQuery q(req , QSqlDatabase::database(USER_DB_CONNECTION));
        if (q.isActive()) {
            if (q.next()) {
                m_LastUuid = q.value(0).toString();
                m_LastLogin = q.value(1).toString();
                m_LastPass = q.value(2).toString();
            }
        } else {
            LOG_QUERY_ERROR(q);
        }
    }

    if (QSqlDatabase::connectionNames().contains("__ConnectionTest__")) {
        QSqlDatabase::removeDatabase("__ConnectionTest__");
    }

    return (!m_LastUuid.isEmpty());
}

/**
  \brief Return the Uuid of the user identified with couple login/password. Returns a null QString if an error occurs.
*/
QString UserBase::getUuid(const QString &log64, const QString &cryptpass64)
{
    if ((log64 == m_LastLogin) && (cryptpass64 == m_LastPass))
        return m_LastUuid;
    m_LastUuid.clear();
    // create query
    QHash<int, QString> where;
    where.insert(USER_LOGIN, QString("='%1'").arg(log64));
    where.insert(USER_PASSWORD, QString("='%1'").arg(cryptpass64));
    QString req = select(Table_USERS, USER_UUID, where);
    QSqlQuery q(req , database());
    if (q.isActive()) {
        if (q.next()) {
            m_LastUuid = q.value(0).toString();
        }
    } else {
        LOG_ERROR(QApplication::translate("UserBase", "Can not create a new user's UUID, database access error"));
        LOG_QUERY_ERROR(q);
    }
    return m_LastUuid;
}

/**
  \brief Return a new Uuid assuming that it is not already used in base.
*/
QString UserBase::createNewUuid()
{
    QString tmp;
    while (tmp.isEmpty()) {
        tmp = QUuid::createUuid().toString();
        // create query
        QHash<int, QString> where;
        where.insert(USER_UUID, QString("='%1'").arg(tmp));
        QString req = select(Table_USERS, USER_UUID, where);
        QSqlQuery q(req , database());
        if (q.isActive()) {
            if (q.next()) {
                tmp.clear();
            }
        } else {
            LOG_ERROR(QApplication::translate("UserBase", "Can not create a new user's UUID, database access error"));
            LOG_QUERY_ERROR(q);
            break;
        }
    }
    return tmp;
}

QString UserBase::getLogin64(const QString &uuid)
{
    if (uuid == m_LastUuid)
        return m_LastLogin;
    // create query
    QHash<int, QString> where;
    where.insert(USER_UUID, QString("='%1'").arg(uuid));
    QString req = select(Table_USERS, USER_LOGIN, where);
    QSqlQuery q(req , database());
    if (q.isActive()) {
        if (q.next()) {
            return q.value(0).toString();
        }
    } else {
        LOG_ERROR(QApplication::translate("UserBase", "Can not retreive login from the uuid"));
        LOG_QUERY_ERROR(q);
    }
    return QString();
}

//--------------------------------------------------------------------------------------------------------
//--------------------------------------------- Datas savers ---------------------------------------------
//--------------------------------------------------------------------------------------------------------
static inline QString defaultHeader()
{
    return Utils::readTextFile(settings()->path(Core::ISettings::BundleResourcesPath) + "/textfiles/default_user_header.htm");
}

static inline QString defaultFooter()
{
    return Utils::readTextFile(settings()->path(Core::ISettings::BundleResourcesPath) + "/textfiles/default_user_footer.htm");
}

/** Create the default users database if it does not exists. */
bool UserBase::createDatabase(const QString &connectionName , const QString &dbName,
                    const QString &pathOrHostName,
                    TypeOfAccess access, AvailableDrivers driver,
                    const QString &login, const QString &pass,
                    const int port,
                    CreationOption createOption
                   )
{
    Q_UNUSED(access);
    Q_UNUSED(createOption);

    if (connectionName != Constants::USER_DB_CONNECTION)
        return false;

    LOG(QCoreApplication::translate("UserBase",
                                    "Trying to create empty user database. \n"
                                    "       Location : %1 \n"
                                    "       FileName: %2\n"
                                    "       Driver: %3")
        .arg(pathOrHostName, dbName).arg(driver));

    setConnectionName(connectionName);
    setDriver(driver);

    // create an empty database and connect
    QSqlDatabase DB;
    if (driver == SQLite) {
        DB = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        if (!QDir(pathOrHostName).exists())
            if (!QDir().mkpath(pathOrHostName))
                tkTr(Trans::Constants::_1_ISNOT_AVAILABLE_CANNOTBE_CREATED).arg(pathOrHostName);
        DB.setDatabaseName(QDir::cleanPath(pathOrHostName + QDir::separator() + dbName));
        if (!DB.open())
            LOG_ERROR(tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2).arg(dbName).arg(DB.lastError().text()));
        setDriver(Utils::Database::SQLite);
    } else if (driver == MySQL) {
        /** \todo test grants here or before ? */
        if (QSqlDatabase::connectionNames().contains(connectionName)) {
            DB = QSqlDatabase::database(connectionName);
        } else {
            DB = QSqlDatabase::addDatabase("QMYSQL" , connectionName);
            DB.setHostName(pathOrHostName);
            DB.setUserName(login);
            DB.setPassword(pass);
            DB.setPort(port);
        }
        DB.setDatabaseName("mysql");
        if (!DB.open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(DB.connectionName()).arg(DB.lastError().text()));
            return false;
        }
        //        qWarning() << "createMySQLDatabase(dbName);";
        createMySQLDatabase(dbName);
        // change database connection
        DB.setDatabaseName(dbName);
        if (!DB.open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(DB.connectionName()).arg(DB.lastError().text()));
            return false;
        }
    }

    // create db structure
    if (createTables()) {
        LOG(tkTr(Trans::Constants::DATABASE_1_CORRECTLY_CREATED).arg(dbName));
    } else {
        LOG_ERROR(tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2)
                  .arg(dbName, DB.lastError().text()));
        return false;
    }

    // add general administrator
    createDefaultUser();

    // Table INFORMATIONS
    QSqlQuery query(DB);
    query.prepare(prepareInsertQuery(Constants::Table_INFORMATIONS));
    query.bindValue(Constants::INFO_VERSION, Constants::USER_DB_VERSION);
    query.bindValue(Constants::INFO_MAX_LKID, 1);
    if (!query.exec()) {
        LOG_QUERY_ERROR(query);
    }

    // database is readable/writable
    LOG(tkTr(Trans::Constants::DATABASE_1_CORRECTLY_CREATED).arg(pathOrHostName + QDir::separator() + dbName));

    m_IsNewlyCreated = true;
    return true;
}

bool UserBase::createDefaultUser()
{
    UserData* user = new UserData;
    user->setLogin64(Utils::loginForSQL(DEFAULT_USER_CLEARLOGIN));
    user->setClearPassword(DEFAULT_USER_CLEARPASSWORD);
    user->setValidity(true);
    user->setName(DEFAULT_USER_NAME);
    user->setFirstname(DEFAULT_USER_FIRSTNAME);
    user->setLanguage(QLocale().language());
    user->setSpecialty(QStringList() << DEFAULT_USER_SPECIALTY);
    user->setAddress(DEFAULT_USER_ADRESS);
    user->setRights(Constants::USER_ROLE_USERMANAGER, Core::IUser::ReadOwn | Core::IUser::ReadDelegates | Core::IUser::ReadAll | Core::IUser::WriteAll | Core::IUser::Create | Core::IUser::Delete | Core::IUser::Print);
    user->setRights(Constants::USER_ROLE_MEDICAL, Core::IUser::ReadAll | Core::IUser::WriteAll | Core::IUser::Create | Core::IUser::Delete | Core::IUser::Print);
    user->setRights(Constants::USER_ROLE_ADMINISTRATIVE, Core::IUser::ReadAll | Core::IUser::WriteAll | Core::IUser::Create | Core::IUser::Delete | Core::IUser::Print);
    user->setRights(Constants::USER_ROLE_PARAMEDICAL, Core::IUser::ReadAll | Core::IUser::WriteAll | Core::IUser::Create | Core::IUser::Delete | Core::IUser::Print);
    user->setPersonalLkId(1);

    QList<UserDynamicData*> list;

    // Create default header
    Print::TextDocumentExtra *headerDoc = new Print::TextDocumentExtra();
    headerDoc->setHtml(defaultHeader());
    UserDynamicData *header = new UserDynamicData();
    header->setName(USER_DATAS_GENERICHEADER);
    header->setValue(headerDoc);
    header->setUserUuid(user->uuid());
    list << header;

    // Create default footer
    UserDynamicData *footer = new UserDynamicData();
    Print::TextDocumentExtra *extraFooter = new Print::TextDocumentExtra();
    extraFooter->setHtml(defaultFooter());
    footer->setName(USER_DATAS_GENERICFOOTER);
    footer->setValue(extraFooter);
    footer->setUserUuid(user->uuid());
    list << footer;

    user->addDynamicDatasFromDatabase(list);

    /** \todo add a transaction */
    saveUser(user);

    // create the linker
    QSqlDatabase DB = QSqlDatabase::database(Constants::USER_DB_CONNECTION);
    if (!DB.isOpen()) {
        if (!DB.open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(DB.connectionName()).arg(DB.lastError().text()));
            delete user; // list is deleted here
            if (extraFooter)
                delete extraFooter;
            if (headerDoc)
                delete headerDoc;
            return false;
        }
    }
    QSqlQuery query(DB);
    query.prepare(prepareInsertQuery(Constants::Table_USER_LK_ID));
    query.bindValue(Constants::LK_ID, QVariant());
    query.bindValue(Constants::LK_GROUP_UUID, QVariant());
    query.bindValue(Constants::LK_USER_UUID, user->uuid());
    query.bindValue(Constants::LK_LKID, user->personalLinkId());
    if (!query.exec()) {
        LOG_QUERY_ERROR(query);
        delete user; // list is deleted here
        if (extraFooter)
            delete extraFooter;
        if (headerDoc)
            delete headerDoc;
        return false;
    }

    delete user; // list is deleted here
    if (extraFooter)
        delete extraFooter;
    if (headerDoc)
        delete headerDoc;
    return true;
}

/**
  \brief Record the last login date for the user identified by couple login/password. The date is returned.
  \todo add a locker for users
*/
QDateTime UserBase::recordLastLogin(const QString & log, const QString & pass)
{
    // change last login value
    QDateTime now = QDateTime::currentDateTime();
    QHash< int, QString >  where;
    where.insert(USER_LOGIN, QString("='%1'").arg(log));
    where.insert(USER_PASSWORD, QString("='%1'").arg(pass));
    QSqlDatabase DB = database();
    if (!DB.isOpen()) {
        if (!DB.open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(DB.connectionName()).arg(DB.lastError().text()));
            return now;
        }
    }
    QSqlQuery q(DB);
    q.prepare(prepareUpdateQuery(Table_USERS, USER_LASTLOG, where));
    q.bindValue(0 , now);
    if (!q.exec()) {
        Utils::Log::addQueryError(this, q);
        return QDateTime();
    }
    // TODO add locker
    Utils::Log::addMessage(this, tr("Recorded User Last Login : %1 ").arg(now.toString()));
    return now;
}

/** Creates a new user in the database according to the actual specified driver. */
bool UserBase::createUser(UserData *user)
{
    qWarning() << Q_FUNC_INFO << user->name();
    // check current user grants
    /** \todo code here */

    switch (driver()) {
    case Utils::Database::MySQL:
        {
            // create user grants
            Utils::Database::Grants grants = Grant_Select | Grant_Update | Grant_Insert | Grant_Delete | Grant_Create | Grant_Drop | Grant_Alter;
            if (user->hasRight(Constants::USER_ROLE_USERMANAGER, Core::IUser::Create)) {
                grants |= Grant_CreateUser;
            }

            // create a MySQL user
            if (!createMySQLUser(user->clearLogin(), user->clearPassword(), grants))
                return false;
            break;
        }
    case Utils::Database::SQLite:
        {
            break;
        }
    case Utils::Database::PostSQL:
        {
            return false;
            break;
        }
    }

    // Create the FreeMedForms user
    return saveUser(user);
}

/**
  \brief Save users datas to the database.
  You can use this function to save a newly created user or to update an already existing user. This function
  manages both cases.
*/
bool UserBase::saveUser(UserData *user)
{
    if (!user->isModified())
        return true;

//    qWarning() << "save user" << user->name();

    // connect user database
    QSqlDatabase DB = database();
    if (!DB.isOpen()) {
        if (!DB.open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(DB.connectionName()).arg(DB.lastError().text()));
            return false;
        }
    }

    // if user already exists ==> update   else ==> insert
    bool toUpdate = false;
    QHash<int, QString> where;
    if (user->id() != -1) {
        where.insert(USER_UUID, QString("='%1'").arg(user->uuid()));
        QString querychecker = select(Table_USERS, USER_UUID, where);
        {
            QSqlQuery q(querychecker, DB);
            if (q.isActive()) {
                if (q.next())
                    toUpdate = true;
            } else {
                LOG_QUERY_ERROR(q);
            }
        }
    }
    // construct query
    QStringList req;
    bool error = false;
    if (toUpdate) {
        // update Table_USERS
        /** \todo update identifiers according to the driver. */
        {
            QSqlQuery q(DB);
            q.prepare(prepareUpdateQuery(Table_USERS, where));
            q.bindValue(USER_ID, user->id());
            q.bindValue(USER_UUID, user->uuid());
            q.bindValue(USER_VALIDITY, user->validity());
            q.bindValue(USER_LOGIN, user->login64());
            q.bindValue(USER_PASSWORD, user->cryptedPassword());
            q.bindValue(USER_LASTLOG, user->lastLogin());
            q.bindValue(USER_NAME, user->name());
            q.bindValue(USER_SECONDNAME, user->secondName());
            q.bindValue(USER_FIRSTNAME, user->firstname());
            q.bindValue(USER_MAIL, user->mail());
            q.bindValue(USER_LANGUAGE, user->language());
            q.bindValue(USER_LOCKER, user->locker());
            if (!q.exec()) {
                error = true;
                LOG_QUERY_ERROR(q);
            }
        }
        // update dynamic datas
        if (user->hasModifiedDynamicDatasToStore()) {
            const QList<UserDynamicData*> &datasToUpdate = user->modifiedDynamicDatas();
            foreach(UserDynamicData *dyn, datasToUpdate) {
                QSqlQuery q(DB);
                //                qWarning() << "SAVE UDD TO BASE" ;
                //                dyn->warn();
                if (dyn->id() == -1) {
                    // create the dynamic data
                    q.prepare(prepareInsertQuery(Table_DATAS));
                    q.bindValue(DATAS_ID, QVariant()); // auto-id
                } else {
                    // update the dynamic data
                    QHash<int , QString> w;
                    w.insert(DATAS_USER_UUID, QString("='%1'").arg(user->uuid()));
                    w.insert(DATAS_ID, QString ("=%1").arg(dyn->id()));
                    w.insert(DATAS_DATANAME, QString("='%1'").arg(dyn->name()));
                    q.prepare(prepareUpdateQuery(Table_DATAS, w));
                    q.bindValue(DATAS_ID, dyn->id());
                }
                dyn->prepareQuery(q);
                if (!q.exec()) {
                    error = true;
                    LOG_QUERY_ERROR(q);
                } else {
                    dyn->setDirty(false);
                }
                if (dyn->id() == -1) {
                    dyn->setId(q.lastInsertId().toInt());
                }
            }
        }

        /** \todo --> update rights */
        /** \todo --> update UserLkId */

        if (!error) {
            LOG(QCoreApplication::translate("UserBase", "User %1 correctly updated.").arg(user->uuid()));
            user->setModified(false);
        }
    } else {
        // INSERT USER
        // add Table USERS
        {
            QSqlQuery q(DB);
            q.prepare(prepareInsertQuery(Table_USERS));
            q.bindValue(USER_ID,           QVariant());
            q.bindValue(USER_UUID,         user->uuid());
            q.bindValue(USER_VALIDITY ,    user->validity());
            q.bindValue(USER_LOGIN ,       user->login64());
            q.bindValue(USER_PASSWORD ,    user->cryptedPassword());
            q.bindValue(USER_NAME ,        user->name());
            q.bindValue(USER_FIRSTNAME ,   user->firstname());
            q.bindValue(USER_SECONDNAME ,  user->secondName());
            q.bindValue(USER_MAIL ,        user->mail());
            q.bindValue(USER_LASTLOG ,     user->lastLogin());
            q.bindValue(USER_LANGUAGE,     user->language());
            q.bindValue(USER_LOCKER,       user->locker());
            q.exec();
            if (! q.isActive())
                LOG_QUERY_ERROR(q);
            user->setId(q.lastInsertId());
        }
        // add dynamic datas
        if (user->hasModifiedDynamicDatasToStore()) {
            const QList<UserDynamicData *> &datasToUpdate = user->modifiedDynamicDatas();
            foreach(UserDynamicData *dyn, datasToUpdate) {
                QSqlQuery q (DB);
                q.prepare(prepareInsertQuery(Table_DATAS));
                q.bindValue(DATAS_ID, QVariant());
                dyn->prepareQuery(q);
                q.exec();
                if (!q.isActive()) {
                    LOG_QUERY_ERROR(q);
                } else {
                    dyn->setId(q.lastInsertId().toInt());
                    dyn->setDirty(false);
                }
            }
        }

        // add Table RIGHTS
        if (user->hasModifiedRightsToStore()) {
            const QStringList & rolesToUpdate = user->modifiedRoles();
            foreach(const QString & s, rolesToUpdate) {
                QSqlQuery q(DB);
                q.prepare(prepareInsertQuery(Table_RIGHTS));
                q.bindValue(RIGHTS_ID,         QVariant());
                q.bindValue(RIGHTS_USER_UUID,  user->uuid());
                q.bindValue(RIGHTS_ROLE ,      s);
                q.bindValue(RIGHTS_RIGHTS ,    user->rightsValue(s, RIGHTS_RIGHTS));
                q.exec();
                if (! q.isActive())
                    LOG_QUERY_ERROR(q);
            }
        }

        // create the USER_LK
        if (user->personalLinkId() == -1) {
            user->setPersonalLkId(getMaxLinkId() + 1);
            QSqlQuery q(DB);
            q.prepare(prepareInsertQuery(Table_USER_LK_ID));
            q.bindValue(LK_ID,         QVariant());
            q.bindValue(LK_LKID,       user->personalLinkId());
            q.bindValue(LK_USER_UUID,  user->uuid());
            q.bindValue(LK_GROUP_UUID, QVariant());
            if (!q.exec())
                LOG_QUERY_ERROR(q);
        }
    }

    user->setModified(false);
    return true;
}

/**
  \brief Delete an user identified by its \e uuid from the database.
  \todo delete the user's LKID
*/
bool UserBase::deleteUser(const QString &uuid)
{
    // connect user database
    QSqlDatabase DB = database();
    if (!DB.isOpen()) {
        if (!DB.open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(DB.connectionName()).arg(DB.lastError().text()));
            return false;
        }
    }

    QSqlQuery query(DB);

    switch (driver()) {
    case Utils::Database::MySQL:
        {
            // drop a MySQL user
            QString clearLog = QString(QByteArray::fromBase64(getLogin64(uuid).toAscii()));
            if (!dropMySQLUser(clearLog))
                return false;
            break;
        }
    case Utils::Database::SQLite:
        {
            break;
        }
    case Utils::Database::PostSQL:
        {
            return false;
            break;
        }
    }

    // delete table USERS
    QHash<int,QString> where;
    where.insert(Constants::USER_UUID, QString("='%1'").arg(uuid));
    if (!query.exec(prepareDeleteQuery(Table_USERS, where))) {
        LOG_QUERY_ERROR(query);
        return false;
    }
    query.finish();
    where.clear();
    where.insert(Constants::DATAS_USER_UUID, QString("='%1'").arg(uuid));
    if (!query.exec(prepareDeleteQuery(Table_RIGHTS, where))) {
        LOG_QUERY_ERROR(query);
        return false;
    }
    query.finish();
    where.clear();
    where.insert(Constants::RIGHTS_USER_UUID, QString("='%1'").arg(uuid));
    if (!query.exec(prepareDeleteQuery(Table_DATAS, where))) {
        LOG_QUERY_ERROR(query);
        return false;
    }
    query.finish();
    where.clear();
    where.insert(Constants::LK_USER_UUID, QString("='%1'").arg(uuid));
    if (!query.exec(prepareDeleteQuery(Table_USER_LK_ID, where))) {
        LOG_QUERY_ERROR(query);
        return false;
    }
    query.finish();
    return true;
}

int UserBase::getMaxLinkId()
{
    QSqlQuery query(database());
    if (!query.exec(select(Constants::Table_INFORMATIONS, Constants::INFO_MAX_LKID))) {
        LOG_QUERY_ERROR(query);
        return -1;
    } else {
        if (query.next())
            return query.value(0).toInt();
    }
    return -1;
}

void UserBase::updateMaxLinkId(const int max)
{
    QSqlQuery query(database());
    query.prepare(prepareUpdateQuery(Constants::Table_INFORMATIONS, Constants::INFO_MAX_LKID));
    query.bindValue(0, max);
    if (!query.exec())
        LOG_QUERY_ERROR(query);
}
