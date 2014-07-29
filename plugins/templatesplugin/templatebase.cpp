/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "templatebase.h"
#include "constants.h"

#include <utils/global.h>
#include <utils/log.h>
#include <utils/databaseconnector.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>
#include <translationutils/trans_database.h>
#include <translationutils/trans_msgerror.h>

#include <coreplugin/isettings.h>
#include <coreplugin/icore.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/icommandline.h>

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QFile>
#include <QDir>

using namespace Templates;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ICommandLine *commandLine()  { return Core::ICore::instance()->commandLine(); }

static inline bool connectDatabase(QSqlDatabase &DB, const int line)
{
    if (!DB.isOpen()) {
        if (!DB.open()) {
            Utils::Log::addError("TemplateBase", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                 .arg(DB.connectionName()).arg(DB.lastError().text()),
                                 __FILE__, line);
            return false;
        }
    }
    return true;
}

namespace Templates {
namespace Internal {
/**
  \brief Private part of DrugsBase
  \internal
*/
class TemplateBasePrivate
{
public:
    TemplateBasePrivate(TemplateBase *base) :
        m_initialized(false),
        m_LogChrono(false),
        q(base) {}
    ~TemplateBasePrivate()
    {
    }

    void checkDatabaseVersion()
    {
        QString version = q->getVersion(Utils::Field(Constants::Table_Version, Constants::VERSION_ACTUAL));
        bool updateVersionNumber = false;
        if (version=="0.3.0") {
            QSqlDatabase DB = QSqlDatabase::database(Constants::DB_TEMPLATES_NAME);
            if (!connectDatabase(DB, __LINE__)) {
                return;
            }
            // Update database schema to 0.4.0
            // MySQL server connection starts here, so no update needed for MySQL database

            // For SQLite :
            // --> USER_UID from int to varchar
            // --> Adding GROUP_UID for templates and categories
            LOG_FOR(q, "Updating templates database version (0.3.0 to 0.4.0)");

            // 1. Rename old tables
            QStringList reqs;
            reqs
                    << "ALTER TABLE `CATEGORIES` RENAME TO `OLD_CATEGORIES`;"
                    << "ALTER TABLE `TEMPLATES` RENAME TO `OLD_TEMPLATES`;";

            if (!q->executeSQL(reqs, DB))
                LOG_ERROR_FOR(q, "Unable to recreate template database during update (0.3.0 to 0.4.0)");

            // 2. Recreate the db schema
            if (!q->createTables())
                LOG_ERROR_FOR(q, "Unable to recreate template database during update (0.3.0 to 0.4.0)");

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


            // Reinsert data to new tables
            if (!q->executeSQL(reqs, DB))
                LOG_ERROR_FOR(q, "Unable to recreate template database during update (0.3.0 to 0.4.0)");

            // Refresh db version
            version = "0.4.0";
            updateVersionNumber = true;
        }

        if (updateVersionNumber) {
            q->setVersion(Utils::Field(Constants::Table_Version, Constants::VERSION_ACTUAL), Constants::DB_ACTUAL_VERSION);
        }
    }

public:
    bool m_initialized;
    bool m_LogChrono;

private:
    TemplateBase *q;
};
}  // End Internal
}  // End Templates


//--------------------------------------------------------------------------------------------------------
//-------------------------------------- Initializing Database -------------------------------------------
//--------------------------------------------------------------------------------------------------------
/**
   \brief Constructor.
   \private
*/
TemplateBase::TemplateBase(QObject *parent) :
    QObject(parent), Utils::Database(),
    d(0)
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

    // TODO: code here : use the category plugin
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

    // TODO: DB Schema :: Add USerGroupUid

    addField(Table_Version, VERSION_ACTUAL, "ACTUAL", FieldIsShortText);

    // Connect first run database creation requested
    connect(Core::ICore::instance(), SIGNAL(firstRunDatabaseCreation()), this, SLOT(onCoreFirstRunCreationRequested()));
}

/** \brief Destructor. */
TemplateBase::~TemplateBase()
{
    if (d) delete d;
    d=0;
}

bool TemplateBase::initialize()
{
    // only one base can be initialized
    if (d->m_initialized)
        return true;

    // connect
    if (commandLine()->value(Core::ICommandLine::ClearUserDatabases).toBool()) {
        createConnection(Templates::Constants::DB_TEMPLATES_NAME, Templates::Constants::DB_TEMPLATES_NAME,
                         settings()->databaseConnector(),
                         Utils::Database::DeleteAndRecreateDatabase);
    } else {
        createConnection(Templates::Constants::DB_TEMPLATES_NAME, Templates::Constants::DB_TEMPLATES_NAME,
                         settings()->databaseConnector(),
                         Utils::Database::CreateDatabase);
    }

    if (!database().isOpen()) {
        if (!database().open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Templates::Constants::DB_TEMPLATES_NAME).arg(database().lastError().text()));
        } else {
            LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().databaseName()).arg(database().driverName()));
        }
    } else {
        LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().databaseName()).arg(database().driverName()));
    }

    if (!checkDatabaseScheme()) {
        LOG_ERROR(tkTr(Trans::Constants::DATABASE_1_SCHEMA_ERROR).arg(Templates::Constants::DB_TEMPLATES_NAME));
        return false;
    }

    d->checkDatabaseVersion();

    connect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
    d->m_initialized = true;
    return true;
}

bool TemplateBase::isInitialized()
{
    return d->m_initialized;
}

void TemplateBase::logChronos(bool state)
{
    Q_UNUSED(state);
}

bool TemplateBase::createDatabase(const QString &connectionName , const QString &dbName,
                    const QString &pathOrHostName,
                    TypeOfAccess access, AvailableDrivers driver,
                    const QString & login, const QString & pass,
                    const int port,
                    CreationOption /*createOption*/
                   )
{
    Q_UNUSED(access);
    if (connectionName != Templates::Constants::DB_TEMPLATES_NAME)
        return false;

    LOG(tkTr(Trans::Constants::TRYING_TO_CREATE_1_PLACE_2)
        .arg(dbName).arg(pathOrHostName));

    setConnectionName(connectionName);
    setDriver(driver);

    // create an empty database and connect
    QSqlDatabase DB;
    if (driver == SQLite) {
        DB = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        if (!QDir(pathOrHostName).exists())
            if (!QDir().mkpath(pathOrHostName))
                LOG(tkTr(Trans::Constants::_1_ISNOT_AVAILABLE_CANNOTBE_CREATED).arg(pathOrHostName));
        DB.setDatabaseName(QDir::cleanPath(pathOrHostName + QDir::separator() + dbName));
        if (!DB.open())
            LOG(tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2).arg(dbName).arg(DB.lastError().text()));
    }
    else if (driver == MySQL) {
        DB = QSqlDatabase::database(connectionName);
        if (!DB.open()) {
            QSqlDatabase d = QSqlDatabase::addDatabase("QMYSQL", "__TEMPLATES_CREATOR");
            d.setHostName(pathOrHostName);
            d.setUserName(login);
            d.setPassword(pass);
            d.setPort(port);
            if (!d.open()) {
                Utils::warningMessageBox(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                         .arg(DB.connectionName()).arg(DB.lastError().text()),
                                         tkTr(Trans::Constants::CONTACT_DEV_TEAM));
                return false;
            }
            QSqlQuery q(QString("CREATE DATABASE `%1`").arg(dbName), d);
            if (!q.isActive()) {
                LOG_QUERY_ERROR(q);
                Utils::warningMessageBox(tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2)
                                         .arg(DB.connectionName()).arg(DB.lastError().text()),
                                         tkTr(Trans::Constants::CONTACT_DEV_TEAM));
                return false;
            }
            if (!DB.open()) {
                Utils::warningMessageBox(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                         .arg(DB.connectionName()).arg(DB.lastError().text()),
                                         tkTr(Trans::Constants::CONTACT_DEV_TEAM));
                return false;
            }
            DB.setDatabaseName(dbName);
        }
        if (QSqlDatabase::connectionNames().contains("__TEMPLATES_CREATOR"))
            QSqlDatabase::removeDatabase("__TEMPLATES_CREATOR");
        if (!DB.open()) {
            Utils::warningMessageBox(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                     .arg(DB.connectionName()).arg(DB.lastError().text()),
                                     tkTr(Trans::Constants::CONTACT_DEV_TEAM));
            return false;
        }
    }


    if (createTables()) {
        LOG(tkTr(Trans::Constants::DATABASE_1_CORRECTLY_CREATED).arg(dbName));
    } else {
        LOG_ERROR(tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2)
                  .arg(dbName, DB.lastError().text()));
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

    // Add version number
    if (!setVersion(Utils::Field(Constants::Table_Version, Constants::VERSION_ACTUAL), Constants::DB_ACTUAL_VERSION)) {
        LOG_ERROR_FOR("TemplateBase", "Unable to set version");
    }
    return true;
}

void TemplateBase::onCoreDatabaseServerChanged()
{
    d->m_initialized = false;
    if (QSqlDatabase::connectionNames().contains(Templates::Constants::DB_TEMPLATES_NAME)) {
        QSqlDatabase::removeDatabase(Templates::Constants::DB_TEMPLATES_NAME);
    }
    disconnect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
    disconnect(Core::ICore::instance(), SIGNAL(firstRunDatabaseCreation()), this, SLOT(onCoreFirstRunCreationRequested()));
    initialize();
}

void TemplateBase::onCoreFirstRunCreationRequested()
{
    disconnect(Core::ICore::instance(), SIGNAL(firstRunDatabaseCreation()), this, SLOT(onCoreFirstRunCreationRequested()));
    initialize();
}
