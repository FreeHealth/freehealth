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
#include "templatebase.h"
#include "constants.h"

#include <utils/global.h>
#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/isettings.h>
#include <coreplugin/icore.h>
#include <coreplugin/constants_tokensandsettings.h>

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QFile>
#include <QDir>

using namespace Templates;
//using namespace Templates::Internal;
using namespace Trans::ConstantTranslations;


static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }


namespace Templates {
namespace Internal {
/**
  \brief Private part of DrugsBase
  \internal
*/
class TemplateBasePrivate
{
public:
    TemplateBasePrivate(TemplateBase *base) : q(base) {}
    ~TemplateBasePrivate()
    {
    }

    void checkDatabaseVersion()
    {
        QString version;
        QSqlQuery query(q->select(Constants::Table_Version, QList<int>() << Constants::VERSION_ACTUAL), QSqlDatabase::database(Constants::DB_TEMPLATES_NAME));
        if (query.isActive()) {
            if (query.next())
                version = query.value(0).toString();
        } else {
            Utils::Log::addQueryError(q, query, __FILE__, __LINE__);
        }
        query.finish();
        bool updateVersionNumber = false;
        if (version=="0.3.0") {
            // Update database schema to 0.4.0
            // MySQL server connection starts here, so no update needed for MySQL database

            // For SQLite :
            // --> USER_UID from int to varchar
            // --> Adding GROUP_UID for templates and categories
            Utils::Log::addMessage(q, "Updating templates database version (0.3.0 to 0.4.0)");

            // 1. Rename old tables
            QStringList reqs;
            reqs
                    << "ALTER TABLE `CATEGORIES` RENAME TO `OLD_CATEGORIES`;"
                    << "ALTER TABLE `TEMPLATES` RENAME TO `OLD_TEMPLATES`;";

            if (!q->executeSQL(reqs, q->database()))
                Utils::Log::addError(q, "Unable to recreate template database during update (0.3.0 to 0.4.0)",
                                     __FILE__, __LINE__);

            // 2. Recreate the db schema
            if (!q->createTables())
                Utils::Log::addError(q, "Unable to recreate template database during update (0.3.0 to 0.4.0)",
                                     __FILE__, __LINE__);

            reqs.clear();
            reqs << QString("INSERT INTO `CATEGORIES` (%1) SELECT %1 FROM `OLD_CATEGORIES`;")
                          .arg("`CATEGORY_ID`,"
                               "`CATEGORY_UUID`,"
                               "`USER_UUID`,"
                               "`PARENT_CATEGORY`,"
                               "`LABEL`,"
                               "`SUMMARY`,"
                               "`MIMETYPES`,"
                               "`DATE_CREATION`,"
                               "`DATE_MODIFICATION`,"
                               "`THEMED_ICON_FILENAME`,"
                               "`TRANSMISSION_DATE`")
                    << "DROP TABLE `OLD_CATEGORIES`;"
                    << QString("INSERT INTO `TEMPLATES` (%1) SELECT %1 FROM `OLD_TEMPLATES`;")
                          .arg("`TEMPLATE_ID`,"
                               "`TEMPLATE_UUID`,"
                               "`USER_UUID`,"
                               "`ID_CATEGORY`,"
                               "`LABEL`,"
                               "`SUMMARY`,"
                               "`CONTENT`,"
                               "`CONTENT_MIMETYPES`,"
                               "`DATE_CREATION`,"
                               "`DATE_MODIFICATION`,"
                               "`THEMED_ICON_FILENAME`,"
                               "`TRANSMISSION_DATE`")
                    << "DROP TABLE `OLD_TEMPLATES`;";


            // Reinsert datas to new tables
            if (!q->executeSQL(reqs, q->database()))
                Utils::Log::addError(q, "Unable to recreate template database during update (0.3.0 to 0.4.0)",
                                     __FILE__, __LINE__);

            // Refresh db version
            version = "0.4.0";
            updateVersionNumber = true;
        }

        if (updateVersionNumber) {
            query.prepare(q->prepareUpdateQuery(Constants::Table_Version, Constants::VERSION_ACTUAL));
            query.bindValue(0, Constants::DB_ACTUAL_VERSION);
            query.exec();
            if (!query.isActive()) {
                Utils::Log::addQueryError(q, query, __FILE__, __LINE__);
            }
            query.finish();
        }
    }

public:
    TemplateBase *q;
    bool m_LogChrono;
};
}  // End Internal
}  // End Templates


//--------------------------------------------------------------------------------------------------------
//--------------------------------- Initialization of static members -------------------------------------
//--------------------------------------------------------------------------------------------------------
TemplateBase *TemplateBase::m_Instance = 0;
bool TemplateBase::m_initialized = false;

//--------------------------------------------------------------------------------------------------------
//-------------------------------------- Initializing Database -------------------------------------------
//--------------------------------------------------------------------------------------------------------
/** \brief Returns the unique instance of DrugsBase. If it does not exists, it is created */
TemplateBase *TemplateBase::instance()
{
    if (!m_Instance) {
        m_Instance = new TemplateBase(qApp);
        m_Instance->init();
    }
    return m_Instance;
}

/**
   \brief Constructor.
   \private
*/
TemplateBase::TemplateBase(QObject *parent)
    : QObject(parent), Utils::Database(), d(0)
{
    d = new Internal::TemplateBasePrivate(this);
    setObjectName("TemplateBase");

    using namespace Templates::Constants;
    addTable(Table_Templates,  "TEMPLATES");
    addTable(Table_Categories, "CATEGORIES");
    addTable(Table_Version,    "VERSION");

    addField(Table_Templates, TEMPLATE_ID,               "TEMPLATE_ID",          FieldIsUniquePrimaryKey);
    addField(Table_Templates, TEMPLATE_UUID,             "TEMPLATE_UUID",        FieldIsUUID);
    addField(Table_Templates, TEMPLATE_USER_UID,         "USER_UUID",            FieldIsUUID);
    addField(Table_Templates, TEMPLATE_GROUP_UID,        "GROUP_UUID",           FieldIsUUID);
    addField(Table_Templates, TEMPLATE_ID_CATEGORY,      "ID_CATEGORY",          FieldIsInteger);
    addField(Table_Templates, TEMPLATE_LABEL,            "LABEL",                FieldIsShortText);
    addField(Table_Templates, TEMPLATE_SUMMARY,          "SUMMARY",              FieldIsLongText);
    addField(Table_Templates, TEMPLATE_CONTENT,          "CONTENT",              FieldIsBlob);
    addField(Table_Templates, TEMPLATE_CONTENTMIMETYPES, "CONTENT_MIMETYPES",    FieldIsShortText);
    addField(Table_Templates, TEMPLATE_DATECREATION,     "DATE_CREATION",        FieldIsDate);
    addField(Table_Templates, TEMPLATE_DATEMODIF,        "DATE_MODIFICATION",    FieldIsDate);
    addField(Table_Templates, TEMPLATE_THEMEDICON,       "THEMED_ICON_FILENAME", FieldIsShortText);
    addField(Table_Templates, TEMPLATE_TRANSMISSIONDATE, "TRANSMISSION_DATE",    FieldIsDate);

    /** \todo DB schema :: Update Templates::USER_UID from int to varchar */
    /** \todo DB schema :: Add UserGroupUid */

//            "CREATE TABLE IF NOT EXISTS `TEMPLATES` ("
//            "`TEMPLATE_ID`              INTEGER        PRIMARY KEY AUTOINCREMENT,"
//            "`TEMPLATE_UUID`            varchar(40)    NULL,"
//            "`USER_UUID`                int(11)        NULL,"           //  NEEDS UPDATE FROM 0.3.0 TO 0.4.0
//            "`ID_CATEGORY`              int(11)        DEFAULT -1,"
//            "`LABEL`                    varchar(300)   NULL,"
//            "`SUMMARY`                  varchar(500)   NULL,"
//            "`CONTENT`                  blob           NULL,"
//            "`CONTENT_MIMETYPES`        varchar(300)   NULL,"
//            "`DATE_CREATION`            date           NULL,"
//            "`DATE_MODIFICATION`        date           NULL,"
//            "`THEMED_ICON_FILENAME`     varchar(50)    NULL,"
//            "`TRANSMISSION_DATE`        date           NULL"
//            ");"

    addField(Table_Categories,  CATEGORIES_ID,               "CATEGORY_ID",          FieldIsUniquePrimaryKey);
    addField(Table_Categories,  CATEGORIES_UUID,             "CATEGORY_UUID",        FieldIsUUID);
    addField(Table_Categories,  CATEGORIES_USER_UID,         "USER_UUID",            FieldIsUUID);
    addField(Table_Categories,  CATEGORIES_GROUP_UID,        "GROUP_UUID",           FieldIsUUID);
    addField(Table_Categories,  CATEGORIES_PARENT_ID,        "PARENT_CATEGORY",      FieldIsInteger);
    addField(Table_Categories,  CATEGORIES_LABEL,            "LABEL",                FieldIsShortText);
    addField(Table_Categories,  CATEGORIES_SUMMARY,          "SUMMARY",              FieldIsLongText);
    addField(Table_Categories,  CATEGORIES_MIMETYPES,        "MIMETYPES",            FieldIsShortText);
    addField(Table_Categories,  CATEGORIES_DATECREATION,     "DATE_CREATION",        FieldIsDate);
    addField(Table_Categories,  CATEGORIES_DATEMODIF,        "DATE_MODIFICATION",    FieldIsDate);
    addField(Table_Categories,  CATEGORIES_THEMEDICON,       "THEMED_ICON_FILENAME", FieldIsShortText);
    addField(Table_Categories,  CATEGORIES_TRANSMISSIONDATE, "TRANSMISSION_DATE",    FieldIsDate);

    /** \todo DB Schema :: Add USerGroupUid */

//            "CREATE TABLE IF NOT EXISTS `CATEGORIES` ("
//            "`CATEGORY_ID`              INTEGER        PRIMARY KEY AUTOINCREMENT,"
//            "`CATEGORY_UUID`            varchar(40)    NULL,"
//            "`USER_UUID`                int(11)        NULL,"          //  NEEDS UPDATE FROM 0.3.0 TO 0.4.0
//            "`PARENT_CATEGORY`          int(11)        DEFAULT -1,"
//            "`LABEL`                    varchar(300)   NULL,"
//            "`SUMMARY`                  varchar(500)   NULL,"
//            "`MIMETYPES`                varchar(300)   NULL,"
//            "`DATE_CREATION`            date           NULL,"
//            "`DATE_MODIFICATION`        date           NULL,"
//            "`THEMED_ICON_FILENAME`     varchar(50)    NULL,"
//            "`TRANSMISSION_DATE`        date           NULL"
//            ");";

    addField(Table_Version, VERSION_ACTUAL, "ACTUAL", FieldIsShortText);

//            "CREATE TABLE IF NOT EXISTS `VERSION` ("
//            "`ACTUAL`                  varchar(10)"
//            ");";

    connect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));

    init();
}

/** \brief Destructor. */
TemplateBase::~TemplateBase()
{
    if (d) delete d;
    d=0;
}

bool TemplateBase::init()
{
    // only one base can be initialized
    if (m_initialized)
        return true;

    // Check settings --> SQLite or MySQL ?
    if (settings()->value(Core::Constants::S_USE_EXTERNAL_DATABASE, false).toBool()) {
        createConnection(Templates::Constants::DB_TEMPLATES_NAME,
                         Templates::Constants::DB_TEMPLATES_NAME,
                         QString(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_HOST, QByteArray("localhost").toBase64()).toByteArray())),
                         Utils::Database::ReadWrite,
                         Utils::Database::MySQL,
                         QString(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_LOG, QByteArray("root").toBase64()).toByteArray())),
                         QString(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_PASS, QByteArray("").toBase64()).toByteArray())),
                         QString(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_PORT, QByteArray("").toBase64()).toByteArray())).toInt(),
                         Utils::Database::CreateDatabase);
    } else {
        createConnection(Templates::Constants::DB_TEMPLATES_NAME,
                         Templates::Constants::DB_TEMPLATES_FILENAME,
                         settings()->path(Core::ISettings::ReadWriteDatabasesPath) + QDir::separator() + QString(Templates::Constants::DB_TEMPLATES_NAME),
                         Utils::Database::ReadWrite,
                         Utils::Database::SQLite,
                         "log", "pas", 0,
                         Utils::Database::CreateDatabase);
    }
    d->checkDatabaseVersion();

    m_initialized = true;
    return true;
}

void TemplateBase::logChronos(bool state)
{}

bool TemplateBase::createDatabase(const QString &connectionName , const QString &dbName,
                    const QString &pathOrHostName,
                    TypeOfAccess access, AvailableDrivers driver,
                    const QString & login, const QString & pass,
                    const int port,
                    CreationOption /*createOption*/
                   )
{
    if (connectionName != Templates::Constants::DB_TEMPLATES_NAME)
        return false;

    Utils::Log::addMessage(this, tkTr(Trans::Constants::TRYING_TO_CREATE_1_PLACE_2)
                           .arg(dbName).arg(pathOrHostName));

    // create an empty database and connect
    QSqlDatabase DB;
    if (driver == SQLite) {
        DB = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        if (!QDir(pathOrHostName).exists()) {
            if (!QDir().mkpath(pathOrHostName)) {
                Utils::Log::addError(this, tkTr(Trans::Constants::_1_ISNOT_AVAILABLE_CANNOTBE_CREATED).arg(pathOrHostName),
                                     __FILE__, __LINE__);
            } else {
                Utils::Log::addMessage(this, tkTr(Trans::Constants::CREATE_DIR_1).arg(pathOrHostName));
            }
        }
        DB.setDatabaseName(QDir::cleanPath(pathOrHostName + QDir::separator() + dbName));
        if (!DB.open())
            Utils::Log::addError(this, tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2).arg(dbName).arg(DB.lastError().text()),
                                 __FILE__, __LINE__);
        setDriver(Utils::Database::SQLite);
    }
    else if (driver == MySQL) {
        DB = QSqlDatabase::database(connectionName);
        if (!DB.open()) {
            QSqlDatabase d = QSqlDatabase::addDatabase("QMYSQL", "CREATOR");
            d.setHostName(pathOrHostName);
            d.setUserName(login);
            d.setPassword(pass);
            d.setPort(port);
            if (!d.open()) {
                Utils::warningMessageBox(tr("Unable to connect the Templates host."),tr("Please contact dev team."));
                return false;
            }
            QSqlQuery q(QString("CREATE DATABASE `%1`").arg(dbName), d);
            if (!q.isActive()) {
                Utils::Log::addQueryError("TemplateBase", q, __FILE__, __LINE__);
                Utils::warningMessageBox(tr("Unable to create the Templates database."),tr("Please contact dev team."));
                return false;
            }
            if (!DB.open()) {
                Utils::warningMessageBox(tr("Unable to connect the Templates database."),tr("Please contact dev team."));
                return false;
            }
            DB.setDatabaseName(dbName);
        }
        if (QSqlDatabase::connectionNames().contains("CREATOR"))
            QSqlDatabase::removeDatabase("CREATOR");
        DB.open();
        setDriver(Utils::Database::MySQL);
    }

    // create db structure
    // before we need to inform Utils::Database of the connectionName to use
    setConnectionName(connectionName);

    if (createTables()) {
        Utils::Log::addMessage(this, tkTr(Trans::Constants::DATABASE_1_CORRECTLY_CREATED).arg(dbName));
    } else {
        Utils::Log::addError(this, tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2)
                         .arg(dbName, DB.lastError().text()),
                         __FILE__, __LINE__);
        return false;
    }

    QString req = "CREATE TRIGGER delete_all_category_children AFTER "
                  "DELETE ON `CATEGORIES` "
                  "FOR EACH ROW "
                  "  BEGIN"
                  "    DELETE FROM `CATEGORIES` WHERE `CATEGORIES`.`PARENT_CATEGORY`=old.`CATEGORY_ID`;"
                  "    DELETE FROM `TEMPLATES` WHERE `TEMPLATES`.`ID_CATEGORY`=old.`CATEGORY_ID`;"
                  "  END;";
    if (!executeSQL(req, DB)) {
        return false;
    }

    return true;
}

void TemplateBase::onCoreDatabaseServerChanged()
{
    m_initialized = false;
    if (QSqlDatabase::connectionNames().contains(Templates::Constants::DB_TEMPLATES_NAME)) {
        QSqlDatabase::removeDatabase(Templates::Constants::DB_TEMPLATES_NAME);
    }
    init();
}
