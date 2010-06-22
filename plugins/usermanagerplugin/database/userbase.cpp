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

/**
  \class UserBase
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

  \ingroup usertoolkit database_usertoolkit
  \ingroup usermanager
*/

#include "userbase.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <printerplugin/textdocumentextra.h>
#include <coreplugin/iuser.h>

#include <utils/log.h>
#include <utils/global.h>

#include <usermanagerplugin/userdata.h>

// include Qt headers
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
    addField(Table_USERS, USER_SURNAME,      "SURNAME",         FieldIsShortText);
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

    // test connection (create DB if not exists)
    // Check settings --> SQLite or MySQL ?
    if (set->value(Core::Constants::S_USE_EXTERNAL_DATABASE, false).toBool()) {
        if (!createConnection(USER_DB_CONNECTION,
                         USER_DB_CONNECTION,
                         QString(QByteArray::fromBase64(set->value(Core::Constants::S_EXTERNAL_DATABASE_HOST, QByteArray("localhost").toBase64()).toByteArray())),
                         Utils::Database::ReadWrite,
                         Utils::Database::MySQL,
                         QString(QByteArray::fromBase64(set->value(Core::Constants::S_EXTERNAL_DATABASE_LOG, QByteArray("root").toBase64()).toByteArray())),
                         QString(QByteArray::fromBase64(set->value(Core::Constants::S_EXTERNAL_DATABASE_PASS, QByteArray("").toBase64()).toByteArray())),
                         QString(QByteArray::fromBase64(set->value(Core::Constants::S_EXTERNAL_DATABASE_PORT, QByteArray("").toBase64()).toByteArray())).toInt(),
                         Utils::Database::CreateDatabase))
            return false;
    } else {
        // Connect SQLite database
        QString pathToDb = set->path(Core::ISettings::ReadWriteDatabasesPath);
        pathToDb = QDir::cleanPath(pathToDb + QDir::separator() + USER_DB_CONNECTION);
        if (!QDir(pathToDb).exists()) {
            Utils::Log::addMessage(this, tkTr(Trans::Constants::CREATE_DIR_1).arg(pathToDb));
            QDir().mkpath(pathToDb);
        }
        if (!createConnection(USER_DB_CONNECTION,
                         QString("%1.db").arg(USER_DB_CONNECTION),
                         pathToDb,
                         Utils::Database::ReadWrite,
                         Utils::Database::SQLite,
                         "log", "pas", 0,
                         Utils::Database::CreateDatabase))
            return false;
    }

    if (!checkDatabaseVersion())
        return false;

    m_initialized = true;
    return true;
}

bool UserBase::isNewlyCreated() const
{
    return m_IsNewlyCreated;
}

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
    if (!DB.isOpen())
        DB.open();

    // get USERS table
    QString req = select(Table_USERS, conditions);
    UserData *toReturn = 0;
    QString uuid = "";
    {
	QSqlQuery q(req , DB);
	if (q.isActive()) {
	    if (q.next()) {
		if (q.record().count() != USER_MaxParam)
		    Utils::Log::addError("UserBase", QCoreApplication::translate("UserBase",
										"ERROR : will retreiving %1. Wrong number of fields")
				     .arg(USER_DB_CONNECTION));
                int i = 0;
		uuid = q.value(USER_UUID).toString();
                toReturn = new UserData(uuid);
		for (i = 0; i < USER_MaxParam; ++i)
		    toReturn->setValue(Table_USERS, i , q.value(i));
            }
        }
        else
	    Utils::Log::addQueryError("UserBase", q);
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
		    Utils::Log::addError("UserBase", QCoreApplication::translate("UserBase",
										"ERROR : will retreiving %1. Wrong number of fields")
				     .arg("users' rights"));
                int i = 0;
		QByteArray id = q.value(RIGHTS_ROLE).toByteArray();
		for (i = 0; i < RIGHTS_MaxParam; ++i)
		    toReturn->addRightsFromDatabase(id , i , q.value(i));
            }
        }
        else
	    Utils::Log::addQueryError("UserBase", q);
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
		    Utils::Log::addError("UserBase", QCoreApplication::translate("UserBase",
										"ERROR : will retreiving %1. Wrong number of fields")
				     .arg("users' rights"));
                int i = 0;
                UserDynamicData *data = new UserDynamicData();
		for (i = 0; i < DATAS_MaxParam; ++i) {
                    data->feedFromSql(i, q.value(i));
                }
                list << data;
            }
        }
        else
	    Utils::Log::addQueryError("UserBase", q);
    }
    if (list.count())
        toReturn->addDynamicDatasFromDatabase(list);

    // get LKID Table
    where.clear();
    where.insert(Constants::LK_USER_UUID, QString("='%1'").arg(uuid));
    req = select(Constants::Table_USER_LK_ID, Constants::LK_LKID, where);
    QList<int> lkids;
    {
        QSqlQuery q(req, DB);
        if (q.isActive()) {
            while (q.next()) {
                lkids << q.value(0).toInt();
            }
        } else {
            Utils::Log::addQueryError("UserBase", q);
        }
        if (lkids.count() < 1) {
            Utils::Log::addError(this, QString("No linker for user %1").arg(toReturn->uuid()));
            return 0;
        }
        toReturn->setLkIds(lkids);
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
UserData *UserBase::getUserByLoginPassword(const QVariant & login, const QVariant & cryptedPassword) const
{
    // retreive corresponding user
    // construct the where clause
    QHash<int, QString> where;
    where.insert(USER_LOGIN, QString("='%1'").arg(login.toString()));
    where.insert(USER_PASSWORD, QString("='%1'").arg(cryptedPassword.toString()));
    // get the asked user
    return getUser(where);
}

/**
  \brief Check the couple login/password passed as params.
*/
bool UserBase::checkLogin(const QString & login, const QString & cryptedPassword) const
{
    m_LastUuid.clear();
    m_LastLogin.clear();
    m_LastPass.clear();
    QSqlDatabase DB = QSqlDatabase::database(USER_DB_CONNECTION);
    if (!DB.isOpen())
        DB.open();

    // create query
    QList<int> list;
    list << USER_UUID << USER_LOGIN << USER_PASSWORD;
    QHash<int, QString> where;
    where.insert(USER_LOGIN, QString("='%1'").arg(login));
    where.insert(USER_PASSWORD, QString("='%1'").arg(cryptedPassword));
    QString req = select(Table_USERS, list, where);

    QSqlQuery q(req , DB);
    if (q.isActive()) {
	if (q.next()) {
	    m_LastUuid = q.value(0).toString();
	    m_LastLogin = q.value(1).toString();
	    m_LastPass = q.value(2).toString();
        }
    }
    else
	Utils::Log::addQueryError("UserBase", q);

    return (! m_LastUuid.isEmpty());
}

/**
  \brief Return the Uuid of the user identified with couple login/password. Returns a null QString if an error occurs.
*/
QString UserBase::getUuid(const QString & log64, const QString &cryptpass64)
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
    }
    else {
	Utils::Log::addError("UserBase", QApplication::translate("UserBase", "Can not create a new user's UUID, database access error"));
	Utils::Log::addQueryError("UserBase", q);
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
        }
        else {
	    Utils::Log::addError("UserBase", QApplication::translate("UserBase", "Can not create a new user's UUID, database access error"));
	    Utils::Log::addQueryError("UserBase", q);
            break;
        }
    }
    return tmp;
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

/**
  \brief Create the default users database if it does not exists.
  Actually this function only supports SQLite database.
*/
bool UserBase::createDatabase(const QString & connectionName , const QString & dbName,
                    const QString & pathOrHostName,
                    TypeOfAccess access, AvailableDrivers driver,
                    const QString & login, const QString & pass,
                    const int port,
                    CreationOption createOption
                   )
{
    Q_UNUSED(access);
    Q_UNUSED(login);
    Q_UNUSED(pass);
    Q_UNUSED(port);
    Q_UNUSED(createOption);
    Utils::Log::addMessage("UserBase", QCoreApplication::translate("UserBase", "Trying to create empty user database. \nLocation : %1 \nFileName: %2")
		       .arg(pathOrHostName, dbName));

    // create an empty database and connect
    QSqlDatabase DB;
    if (driver == SQLite) {
	DB = QSqlDatabase::addDatabase("QSQLITE" , connectionName);
	DB.setDatabaseName(QDir::cleanPath(pathOrHostName + QDir::separator() + dbName));
        DB.open();
    } else if (driver == MySQL) {
        // TODO : how to create a new mysql database ???
    }

    // create db structure
    // before we need to inform tkDatabase of the connectionName to use
    setConnectionName(connectionName);
    if (!createTables()) {
        Utils::Log::addError("UserBase", tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2).arg(dbName).arg(DB.lastError().text()));
        return false;
    }

    // add general administrator
    UserData* user = new UserData;
    user->setLogin(DEFAULT_USER_LOGIN);
    user->setCryptedPassword(DEFAULT_USER_PASSWORD);
    user->setValidity(true);
    user->setName(DEFAULT_USER_NAME);
    user->setSurname(DEFAULT_USER_SURNAME);
    user->setLanguage(QLocale().name().left(2));
    user->setSpecialty(QStringList() << DEFAULT_USER_SPECIALTY);
    user->setAdress(DEFAULT_USER_ADRESS);
    user->setRights(Constants::USER_ROLE_USERMANAGER, Core::IUser::ReadAll | Core::IUser::WriteAll | Core::IUser::Create | Core::IUser::Delete | Core::IUser::Print);
    user->setRights(Constants::USER_ROLE_MEDICAL, Core::IUser::ReadAll | Core::IUser::WriteAll | Core::IUser::Create | Core::IUser::Delete | Core::IUser::Print);
    user->setRights(Constants::USER_ROLE_ADMINISTRATIVE, Core::IUser::ReadAll | Core::IUser::WriteAll | Core::IUser::Create | Core::IUser::Delete | Core::IUser::Print);
    user->setRights(Constants::USER_ROLE_PARAMEDICAL, Core::IUser::ReadAll | Core::IUser::WriteAll | Core::IUser::Create | Core::IUser::Delete | Core::IUser::Print);

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
    QSqlQuery query(database());
    query.prepare(prepareInsertQuery(Constants::Table_USER_LK_ID));
    query.bindValue(Constants::LK_ID, QVariant());
    query.bindValue(Constants::LK_GROUP_UUID, QVariant());
    query.bindValue(Constants::LK_USER_UUID, user->uuid());
    query.bindValue(Constants::LK_LKID, 1);
    if (!query.exec()) {
        Utils::Log::addQueryError(this, query);
    }

    delete user; // list is deleted here
    if (extraFooter)
        delete extraFooter;
    if (headerDoc)
        delete headerDoc;

    // Table INFORMATIONS
    query.finish();
    query.prepare(prepareInsertQuery(Constants::Table_INFORMATIONS));
    query.bindValue(Constants::INFO_VERSION, Constants::USER_DB_VERSION);
    query.bindValue(Constants::INFO_MAX_LKID, 1);
    if (!query.exec()) {
        Utils::Log::addQueryError(this, query);
    }

    // database is readable/writable
    Utils::Log::addMessage("UserBase", QCoreApplication::translate("UserBase", "User database created : File %1").arg(pathOrHostName + QDir::separator() + dbName));

    m_IsNewlyCreated = true;
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
    DB.open();
    QSqlQuery q(DB);
    q.prepare(prepareUpdateQuery(Table_USERS, USER_LASTLOG, where));
    q.bindValue(0 , now);
    if (! q.exec()) {
	Utils::Log::addQueryError(this, q);
        return QDateTime();
    }
    // TODO add locker
    Utils::Log::addMessage(this, tr("Recorded User Last Login : %1 ").arg(now.toString()));
    return now;
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

    qWarning() << "save user" << user->name();

    // connect user database
    QSqlDatabase DB = database();
    if (!DB.open())
	Utils::Log::addError("UserBase", QCoreApplication::translate("UserBase", "Unable to open database %1").arg(DB.connectionName()));

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
            }
            else
		Utils::Log::addQueryError("UserBase", q);
        }
    }
    // construct query
    QStringList req;
    bool error = false;
    if (toUpdate) {
        // update Table_USERS
        {
	    QSqlQuery q(DB);
	    q.prepare(prepareUpdateQuery(Table_USERS, where));
	    q.bindValue(USER_ID, user->id());
	    q.bindValue(USER_UUID, user->uuid());
	    q.bindValue(USER_VALIDITY, user->validity());
	    q.bindValue(USER_LOGIN, user->login());
	    q.bindValue(USER_PASSWORD, user->cryptedPassword());
	    q.bindValue(USER_LASTLOG, user->lastLogin());
	    q.bindValue(USER_NAME, user->name());
	    q.bindValue(USER_SECONDNAME, user->secondName());
	    q.bindValue(USER_SURNAME, user->surname());
	    q.bindValue(USER_MAIL, user->mail());
	    q.bindValue(USER_LANGUAGE, user->language());
	    q.bindValue(USER_LOCKER, user->locker());
            if (!q.exec()) {
                error = true;
		Utils::Log::addQueryError("UserBase", q);
            }
        }
        // update dynamic datas
	if (user->hasModifiedDynamicDatasToStore()) {
            const QList<UserDynamicData*> &datasToUpdate = user->modifiedDynamicDatas();
	    foreach(UserDynamicData *dyn, datasToUpdate) {
		QSqlQuery q (DB);
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
		    Utils::Log::addQueryError("UserBase", q);
                } else
                    dyn->setDirty(false);
            }
        }
        /** \todo --> update rights */
        if (!error) {
	    Utils::Log::addMessage("UserBase", QCoreApplication::translate("UserBase", "User %1 correctly updated.").arg(user->uuid()));
            user->setModified(false);
        }
    } else {
        // INSERT USER
        // add Table USERS
        {
	    QSqlQuery q (DB);
	    q.prepare(prepareInsertQuery(Table_USERS));
	    q.bindValue(USER_ID,           QVariant());
	    q.bindValue(USER_UUID,         user->uuid());
	    q.bindValue(USER_VALIDITY ,    user->validity());
	    q.bindValue(USER_LOGIN ,       user->login());
	    q.bindValue(USER_PASSWORD ,    user->cryptedPassword());
	    q.bindValue(USER_NAME ,        user->name());
	    q.bindValue(USER_SURNAME ,     user->surname());
	    q.bindValue(USER_SECONDNAME ,  user->secondName());
	    q.bindValue(USER_MAIL ,        user->mail());
	    q.bindValue(USER_LASTLOG ,     user->lastLogin());
	    q.bindValue(USER_LANGUAGE,     user->language());
	    q.bindValue(USER_LOCKER,       user->locker());
            q.exec();
	    if (! q.isActive())
		Utils::Log::addQueryError("UserBase", q);
            user->setId(q.lastInsertId());
        }
        // add dynamic datas
	if (user->hasModifiedDynamicDatasToStore()) {
            const QList<UserDynamicData *> & datasToUpdate = user->modifiedDynamicDatas();
	    foreach(UserDynamicData *dyn, datasToUpdate) {
		QSqlQuery q (DB);
		q.prepare(prepareInsertQuery(Table_DATAS));
		q.bindValue(DATAS_ID, QVariant());
                dyn->prepareQuery(q);
                q.exec();
		if (!q.isActive()) {
		    Utils::Log::addQueryError("UserBase", q);
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
		QSqlQuery q (DB);
		q.prepare(prepareInsertQuery(Table_RIGHTS));
		q.bindValue(RIGHTS_ID,         QVariant());
		q.bindValue(RIGHTS_USER_UUID,  user->uuid());
		q.bindValue(RIGHTS_ROLE ,      s);
		q.bindValue(RIGHTS_RIGHTS ,    user->rightsValue(s, RIGHTS_RIGHTS));
                q.exec();
		if (! q.isActive())
		    Utils::Log::addQueryError("UserBase", q);
            }
        }
    }
    user->setModified(false);
    return true;
}

/**
  \brief Delete an user identified by its \e uuid from the database.
  \todo delete the user's LKID
*/
bool UserBase::deleteUser(const QString & uuid)
{
    // connect user database
    QSqlDatabase DB = database();
    if (! DB.open())
	Utils::Log::addError("UserBase", QCoreApplication::translate("UserBase", "Unable to open database %1").arg(DB.connectionName()));

    QSqlQuery query(DB);

    // delete table USERS
    QHash<int,QString> where;
    where.insert(Constants::USER_UUID, QString("='%1'").arg(uuid));
    if (!query.exec(prepareDeleteQuery(Table_USERS, where))) {
        Utils::Log::addQueryError(this, query);
        return false;
    }
    query.finish();
    where.clear();
    where.insert(Constants::DATAS_USER_UUID, QString("='%1'").arg(uuid));
    if (!query.exec(prepareDeleteQuery(Table_RIGHTS, where))) {
        Utils::Log::addQueryError(this, query);
        return false;
    }
    query.finish();
    where.clear();
    where.insert(Constants::RIGHTS_USER_UUID, QString("='%1'").arg(uuid));
    if (!query.exec(prepareDeleteQuery(Table_DATAS, where))) {
        Utils::Log::addQueryError(this, query);
        return false;
    }
    query.finish();
    return true;
}

int UserBase::getMaxLinkId()
{
    QSqlQuery query(database());
    if (!query.exec(select(Constants::Table_INFORMATIONS, Constants::INFO_MAX_LKID))) {
        Utils::Log::addQueryError(this, query);
        return 0;
    } else {
        if (query.next())
            return query.value(0).toInt();
    }
    return 0;
}

void UserBase::updateMaxLinkId(const int max)
{
    QSqlQuery query(database());
    query.prepare(prepareUpdateQuery(Constants::Table_INFORMATIONS, Constants::INFO_MAX_LKID));
    query.bindValue(0, max);
    if (!query.exec())
        Utils::Log::addQueryError(this, query);
}
