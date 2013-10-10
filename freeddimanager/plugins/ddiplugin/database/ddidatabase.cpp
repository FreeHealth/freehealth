/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "ddidatabase.h"
#include <ddiplugin/constants.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/databaseconnector.h>
#include <translationutils/constants.h>
#include <translationutils/trans_database.h>
#include <translationutils/trans_msgerror.h>
#include <translationutils/trans_filepathxml.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/dialogs/settingsdialog.h>

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDir>
#include <QString>
#include <QFile>
#include <QDate>

using namespace DDI;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline QString databaseFileName() {return settings()->databasePath() + QDir::separator() + QString(Constants::DDIMANAGER_DATABASE_NAME) + QDir::separator() + QString(Constants::DDIMANAGER_DATABASE_FILENAME);}

namespace {
const char * const CURRENTVERSION = "0.8.4";
}

static inline bool connectDatabase(QSqlDatabase &DB, const QString &file, const int line)
{
    if (!DB.isOpen()) {
        if (!DB.open()) {
            Utils::Log::addError("DDIDatabase", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                 .arg(DB.connectionName()).arg(DB.lastError().text()),
                                 file, line);
            return false;
        }
    }
    return true;
}

DDIDatabase::DDIDatabase():
    Utils::Database(),
    m_dbcore_initialized(false),
    m_isDefaultDb(false)
{
    setConnectionName(Constants::DDIMANAGER_DATABASE_NAME);
    using namespace Constants;

    addTable(Table_ATC, "ATC");
    addField(Table_ATC, ATC_ID, "ID", FieldIsUniquePrimaryKey);
    addField(Table_ATC, ATC_ISVALID, "VALID", FieldIsBoolean);
    addField(Table_ATC, ATC_CODE, "CODE", FieldIsShortText);
    addField(Table_ATC, ATC_UID, "UID", FieldIsShortText);
    addField(Table_ATC, ATC_FR, "FR", FieldIsShortText);
    addField(Table_ATC, ATC_EN, "EN", FieldIsShortText);
    addField(Table_ATC, ATC_DE, "DE", FieldIsShortText);
    addField(Table_ATC, ATC_SP, "SP", FieldIsShortText);
    addField(Table_ATC, ATC_DATECREATE, "CREATE", FieldIsDate);
    addField(Table_ATC, ATC_DATEUPDATE, "UPDATE", FieldIsDate);
    addField(Table_ATC, ATC_PREVIOUSCODE, "PREVCODE", FieldIsShortText);
    addField(Table_ATC, ATC_WHOYEARUPDATE, "WHOYEAR", FieldIsDate);
    addField(Table_ATC, ATC_COMMENT, "COMMENT", FieldIsShortText);

    addTable(Table_CURRENTVERSION, "VERSION");
    addField(Table_CURRENTVERSION, CURRENTVERSION_ID, "ID", FieldIsUniquePrimaryKey);
    addField(Table_CURRENTVERSION, CURRENTVERSION_NUMBER, "NUMBER", FieldIsBoolean);
}

/** Force the re-initialization of the database. Call initialize() after this. */
void DDIDatabase::forceFullDatabaseRefreshing()
{
    m_dbcore_initialized = false;
}

/**
 * Initialize the database (create it is required). \n
 * By default, the connection is defined to DrugsDB::Constants::DDIMANAGER_DATABASE_NAME.
 * If you want your own connection name, you can set it with the
 * setConnectionName() and you must set it \b before calling initialize().
 */
bool DDIDatabase::initialize(const QString &pathToDb, bool createIfNotExists)
{
    if (m_dbcore_initialized)
        return true;
    setDriver(Utils::Database::SQLite);

    // test driver
    if (!QSqlDatabase::isDriverAvailable("QSQLITE")) {
        LOG_ERROR_FOR("DDIDatabase", tkTr(Trans::Constants::DATABASE_DRIVER_1_NOT_AVAILABLE).arg("SQLite"));
        Utils::warningMessageBox(tkTr(Trans::Constants::APPLICATION_FAILURE),
                                 tkTr(Trans::Constants::DATABASE_DRIVER_1_NOT_AVAILABLE_DETAIL).arg("SQLite"),
                                 "", qApp->applicationName());
        return false;
    }

    // Connect Drugs Database
    Utils::DatabaseConnector drugConnector;
    QString path = pathToDb;
    if (!QFileInfo(pathToDb).isDir())
        path = QFileInfo(pathToDb).absolutePath();
//    if (!path.endsWith(Constants::DDIMANAGER_DATABASE_NAME)) {
//        path.append(QDir::separator() + QString(Constants::DDIMANAGER_DATABASE_NAME));
//    }
    drugConnector.setAbsPathToReadOnlySqliteDatabase(path);
    drugConnector.setHost(QFileInfo(databaseFileName()).fileName());
    drugConnector.setAccessMode(Utils::DatabaseConnector::ReadOnly);
    drugConnector.setDriver(Utils::Database::SQLite);

    LOG_FOR("DDIDatabase", tkTr(Trans::Constants::SEARCHING_DATABASE_1_IN_PATH_2).arg(connectionName()).arg(pathToDb));

    if (createIfNotExists) {
        createConnection(connectionName(), Constants::DDIMANAGER_DATABASE_FILENAME,
                         drugConnector,
                         Utils::Database::CreateDatabase);
    } else {
        createConnection(connectionName(), Constants::DDIMANAGER_DATABASE_FILENAME,
                         drugConnector,
                         Utils::Database::WarnOnly);
    }

    if (!database().isOpen()) {
        if (!database().open()) {
            LOG_ERROR_FOR("DDIDatabase",tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(connectionName()).arg(database().lastError().text()));
        } else {
            LOG_FOR("DDIDatabase",tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().connectionName()).arg(database().driverName()));
        }
    } else {
        LOG_FOR("DDIDatabase", tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().connectionName()).arg(database().driverName()));
    }

    if (!checkDatabaseScheme()) {
        LOG_ERROR_FOR("DDIDatabase",tkTr(Trans::Constants::DATABASE_1_SCHEMA_ERROR).arg(connectionName()));
        return false;
    }

    if (!checkDatabaseVersion()) {
        LOG_ERROR_FOR("DDIDatabase", QString("Wrong database version. Db: %1; Current: %2").arg(version()).arg(::CURRENTVERSION));
        return false;
    } else {
        LOG_FOR("DDIDatabase", QString("Using drug database version " + version()));
    }

    m_dbcore_initialized = true;
    return true;
}

/** Set the version of the database to \e version */
void DDIDatabase::setVersion(const QString &version)
{
    QSqlDatabase DB = QSqlDatabase::database(connectionName());
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return;
    executeSQL(prepareDeleteQuery(Constants::Table_CURRENTVERSION, QHash<int,QString>()), DB);
    QSqlQuery query(DB);
    query.prepare(prepareInsertQuery(Constants::Table_CURRENTVERSION));
    query.bindValue(Constants::CURRENTVERSION_ID, QVariant());
    query.bindValue(Constants::CURRENTVERSION_NUMBER, version);
    if (!query.exec()) {
        LOG_QUERY_ERROR_FOR("DDIDatabase", query);
    }
}

/** Returns the version of the database */
QString DDIDatabase::version() const
{
    QSqlDatabase DB = QSqlDatabase::database(connectionName());
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return QString();
    QSqlQuery query(DB);
    query.prepare(select(Constants::Table_CURRENTVERSION));
    if (!query.exec()) {
        LOG_QUERY_ERROR_FOR("DDIDatabase", query);
    } else {
        if (query.next()) {
            return query.value(Constants::CURRENTVERSION_NUMBER).toString();
        }
    }
    return QString();
}

/** Check the database version. Returns true if the database version is the latest */
bool DDIDatabase::checkDatabaseVersion() const
{
    return (version()==::CURRENTVERSION);
}

QString DDIDatabase::atcLabelForCode(const QString &code, const QString &lang) const
{
    QSqlDatabase DB = QSqlDatabase::database(connectionName());
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return QString();

    QHash<int, QString> where;
    QString c = code;
    where.insert(Constants::ATC_CODE, QString("='%1'").arg(c.remove("'")));
    int langField = Constants::ATC_FR;
    if (lang.toLower() == "en")
        langField = Constants::ATC_EN;
    else if (lang.toLower() == "de")
        langField = Constants::ATC_DE;
    else if (lang.toLower() == "sp")
        langField = Constants::ATC_SP;

    QString req = select(Constants::Table_ATC, langField, where);
    QSqlQuery query(DB);
    if (query.exec(req)) {
        if (query.next()) {
            return query.value(0).toString();
        }
    } else {
        LOG_QUERY_ERROR_FOR("DDIDatabase", query);
    }
    return QString();
}

/** Read the raw CSV ATC file and populate the database with. Returns the number of ATC codes inserted. */
int DDIDatabase::insertAtcDataFromCsv(const QString &fileName)
{
    // Check database
    QSqlDatabase DB = QSqlDatabase::database(connectionName());
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return false;

    // Check CSV file
    if (!QFile(fileName).exists()) {
        LOG_ERROR_FOR("DDIDatabase", tkTr(Trans::Constants::FILE_1_DOESNOT_EXISTS).arg(fileName));
        return false;
    }

    // Clean ATC table from old values
    QString req = prepareDeleteQuery(Constants::Table_ATC);
    if (!executeSQL(req, DB))
        LOG_ERROR_FOR("DDIDatabase", "Unable to clear old ATC codes");

    // Read the CSV file
    DB.transaction();
    QSqlQuery query(DB);
    QString content = Utils::readTextFile(fileName, Utils::DontWarnUser);
    int n = 0;
    foreach(const QString &line, content.split("\n", QString::SkipEmptyParts)) {
        if (line.startsWith("--"))
            continue;
        // "A01";"STOMATOLOGICAL PREPARATIONS";"PREPARATIONS STOMATOLOGIQUES";"STOMATOLOGIKA"
        QStringList vals = line.split(";");
        while (vals.count() < 5)
            vals << QString();
        if (vals.at(2).isEmpty())
            vals[2] = vals.at(1);
        if (vals.at(3).isEmpty())
            vals[3] = vals.at(1);
        if (vals.at(4).isEmpty())
            vals[4] = vals.at(1);
        query.prepare(prepareInsertQuery(Constants::Table_ATC));
        query.bindValue(Constants::ATC_ISVALID, 1);
        query.bindValue(Constants::ATC_CODE, vals.at(0).toUpper().remove("\""));
        query.bindValue(Constants::ATC_UID, vals.at(2).toUpper().remove("\""));
        query.bindValue(Constants::ATC_FR, vals.at(2).toUpper().remove("\""));
        query.bindValue(Constants::ATC_EN, vals.at(1).toUpper().remove("\""));
        query.bindValue(Constants::ATC_DE, vals.at(3).toUpper().remove("\""));
        query.bindValue(Constants::ATC_SP, QVariant());
        query.bindValue(Constants::ATC_DATECREATE, QDate::currentDate().toString(Qt::ISODate));
        query.bindValue(Constants::ATC_DATEUPDATE, QDate::currentDate().toString(Qt::ISODate));
        query.bindValue(Constants::ATC_PREVIOUSCODE, QVariant());
        query.bindValue(Constants::ATC_WHOYEARUPDATE, QVariant());
        query.bindValue(Constants::ATC_COMMENT, QVariant());
        if (!query.exec()) {
            LOG_QUERY_ERROR_FOR("DDIDatabase", query);
            query.finish();
            DB.rollback();
            return false;
        }
        ++n;
        if (n % 250) {
            DB.commit();
            DB.transaction();
        }
        query.finish();
    }
    DB.commit();
    return true;
}

bool DDIDatabase::createDatabase(const QString &connection, const QString &prefixedDbName,
                                  const Utils::DatabaseConnector &connector,
                                  CreationOption createOption
                                  )
{
//    WARN_FUNC << connectionName << prefixedDbName;
//    qWarning() << connector;

    if (connection != connectionName())
        return false;
    if (connector.driver() != SQLite) {
        return false;
    }
    if (createOption!=Utils::Database::CreateDatabase)
        return false;
    QString pathOrHostName = connector.absPathToSqliteReadOnlyDatabase() + QDir::separator() + QString(connectionName());
    LOG_FOR("DDIDatabase", tkTr(Trans::Constants::TRYING_TO_CREATE_1_PLACE_2).arg(prefixedDbName).arg(pathOrHostName));

    setConnectionName(connectionName());
    setDriver(connector.driver());

    // create an empty database and connect
    if (QSqlDatabase::connectionNames().contains(connectionName())) {
        QSqlDatabase::removeDatabase(connectionName());
    }

    if (!Utils::checkDir(pathOrHostName, true, "DDIDatabase")) {
        LOG_ERROR_FOR("DDIDatabase", tkTr(Trans::Constants::_1_ISNOT_AVAILABLE_CANNOTBE_CREATED).arg(pathOrHostName));
        return false;
    }

    QSqlDatabase DB;
    DB = QSqlDatabase::addDatabase("QSQLITE", connectionName());
    DB.setDatabaseName(QDir::cleanPath(pathOrHostName + QDir::separator() + prefixedDbName));
    if (!DB.open())
        LOG_FOR("DDIDatabase", tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2).arg(prefixedDbName).arg(DB.lastError().text()));
    setDriver(Utils::Database::SQLite);

    // create db structure
    if (createTables()) {
        LOG_FOR("DDIDatabase", tkTr(Trans::Constants::DATABASE_1_CORRECTLY_CREATED).arg(prefixedDbName));
    } else {
        LOG_ERROR_FOR("DDIDatabase", tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2)
                      .arg(prefixedDbName, DB.lastError().text()));
        return false;
    }

    setVersion(::CURRENTVERSION);

    // database is readable/writable
    LOG_FOR("DDIDatabase", tkTr(Trans::Constants::DATABASE_1_CORRECTLY_CREATED).arg(pathOrHostName + QDir::separator() + prefixedDbName));
    return true;
}
