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
#include <ddiplugin/interactors/druginteractor.h>

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

    addTable(Table_INTERACTORS, "INTERACTORS");
    addField(Table_INTERACTORS, INTERACTOR_ID, "ID", FieldIsUniquePrimaryKey);
    addField(Table_INTERACTORS, INTERACTOR_UID, "UID", FieldIsShortText);
    addField(Table_INTERACTORS, INTERACTOR_ISVALID, "ISVALID", FieldIsBoolean);
    addField(Table_INTERACTORS, INTERACTOR_ISREVIEWED, "ISREVIEW", FieldIsBoolean);
    addField(Table_INTERACTORS, INTERACTOR_ISAUTOFOUND, "ISAUTOFOUND", FieldIsBoolean);
    addField(Table_INTERACTORS, INTERACTOR_ISCLASS, "ISCLASS", FieldIsBoolean);
    addField(Table_INTERACTORS, INTERACTOR_WARNDUPLICATES, "WARNDUPLICATES", FieldIsBoolean);
    addField(Table_INTERACTORS, INTERACTOR_ATC, "ATC", FieldIsLongText);
    addField(Table_INTERACTORS, INTERACTOR_CHILDREN, "CHILDREN", FieldIsLongText);
    addField(Table_INTERACTORS, INTERACTOR_FR, "FR", FieldIsShortText);
    addField(Table_INTERACTORS, INTERACTOR_EN, "EN", FieldIsShortText);
    addField(Table_INTERACTORS, INTERACTOR_DE, "DE", FieldIsShortText);
    addField(Table_INTERACTORS, INTERACTOR_INFO_FR, "INFOFR", FieldIsLongText);
    addField(Table_INTERACTORS, INTERACTOR_INFO_EN, "INFOEN", FieldIsLongText);
    addField(Table_INTERACTORS, INTERACTOR_INFO_DE, "INFODE", FieldIsLongText);
    addField(Table_INTERACTORS, INTERACTOR_DATECREATE, "DATECREATE", FieldIsDate);
    addField(Table_INTERACTORS, INTERACTOR_DATEUPDATE, "DATEUPDATE", FieldIsDate);
    addField(Table_INTERACTORS, INTERACTOR_DATEREVIEW, "DATEREVIEW", FieldIsDate);
    addField(Table_INTERACTORS, INTERACTOR_REF, "REF", FieldIsShortText);
    addField(Table_INTERACTORS, INTERACTOR_PMIDS, "PMIDS", FieldIsLongText);
    addField(Table_INTERACTORS, INTERACTOR_COMMENT, "COMMENT", FieldIsShortText);

    addTable(Table_DDI, "DDI");
    addField(Table_DDI, DDI_ID, "ID", FieldIsUniquePrimaryKey);
    addField(Table_DDI, DDI_UID, "UID", FieldIsUUID);
    addField(Table_DDI, DDI_FIRSTINTERACTORUID, "FIRST", FieldIsShortText);
    addField(Table_DDI, DDI_SECONDINTERACTORUID, "SECOND", FieldIsShortText);
    addField(Table_DDI, DDI_ISREVIEWED, "ISREVIEWED", FieldIsBoolean);
    addField(Table_DDI, DDI_ISVALID, "ISVALID", FieldIsBoolean);
    addField(Table_DDI, DDI_LEVELCODE, "LEVELCODE", FieldIsShortText);
    addField(Table_DDI, DDI_DATECREATION, "DATECREATION", FieldIsDate);
    addField(Table_DDI, DDI_DATELASTUPDATE, "DATELASTUPDATE", FieldIsDate);
    addField(Table_DDI, DDI_RISKFR, "RISKFR", FieldIsLongText);
    addField(Table_DDI, DDI_RISKEN, "RISKEN", FieldIsLongText);
    addField(Table_DDI, DDI_MANAGEMENTFR, "MANAGEMENTFR", FieldIsLongText);
    addField(Table_DDI, DDI_MANAGEMENTEN, "MANAGEMENTEN", FieldIsLongText);
    addField(Table_DDI, DDI_REVIEWERSSTRINGLIST, "REVIEWERS", FieldIsLongText);
    addField(Table_DDI, DDI_SOURCE, "SOURCE", FieldIsShortText);
    addField(Table_DDI, DDI_COMMENT, "COMMENT", FieldIsLongText);
    addField(Table_DDI, DDI_FIRSTINTERACTORROUTEOFADMINISTRATIONIDS, "FIRSTROUTES", FieldIsLongText);
    addField(Table_DDI, DDI_SECONDINTERACTORROUTEOFADMINISTRATIONIDS, "SECONDROUTES", FieldIsLongText);
    addField(Table_DDI, DDI_FIRSTDOSEUSEFROM, "FIRSTDOSEUSEFROM", FieldIsBoolean);
    addField(Table_DDI, DDI_FIRSTDOSEUSESTO, "FIRSTDOSEUSESTO", FieldIsBoolean);
    addField(Table_DDI, DDI_FIRSTDOSEFROMVALUE, "FIRSTDOSEFROMVALUE", FieldIsReal);
    addField(Table_DDI, DDI_FIRSTDOSEFROMUNITS, "FIRSTDOSEFROMUNITS", FieldIsShortText);
    addField(Table_DDI, DDI_FIRSTDOSEFROMREPARTITION, "FIRSTDOSEFROMREPARTITION", FieldIsShortText);
    addField(Table_DDI, DDI_FIRSTDOSETOVALUE, "FIRSTDOSETOVALUE", FieldIsReal);
    addField(Table_DDI, DDI_FIRSTDOSETOUNITS, "FIRSTDOSETOUNITS", FieldIsShortText);
    addField(Table_DDI, DDI_FIRSTDOSETOREPARTITION, "FIRSTDOSETOREPARTITION", FieldIsShortText);
    addField(Table_DDI, DDI_SECONDDOSEUSEFROM, "SECONDDOSEUSEFROM", FieldIsBoolean);
    addField(Table_DDI, DDI_SECONDDOSEUSESTO, "SECONDDOSEUSESTO", FieldIsBoolean);
    addField(Table_DDI, DDI_SECONDDOSEFROMVALUE, "SECONDDOSEFROMVALUE", FieldIsReal);
    addField(Table_DDI, DDI_SECONDDOSEFROMUNITS, "SECONDDOSEFROMUNITS", FieldIsShortText);
    addField(Table_DDI, DDI_SECONDDOSEFROMREPARTITION, "SECONDDOSEFROMREPARTITION", FieldIsShortText);
    addField(Table_DDI, DDI_SECONDDOSETOVALUE, "SECONDDOSETOVALUE", FieldIsReal);
    addField(Table_DDI, DDI_SECONDDOSETOUNITS, "SECONDDOSETOUNITS", FieldIsShortText);
    addField(Table_DDI, DDI_SECONDDOSETOREPARTITION, "SECONDDOSETOREPARTITION", FieldIsShortText);
    addField(Table_DDI, DDI_PMIDSTRINGLIST, "PMIDSTRINGLIST", FieldIsLongText);

    addTable(Table_CURRENTVERSION, "VERSION");
    addField(Table_CURRENTVERSION, CURRENTVERSION_ID, "ID", FieldIsUniquePrimaryKey);
    addField(Table_CURRENTVERSION, CURRENTVERSION_NUMBER, "NUMBER", FieldIsBoolean);
}

/** Force the re-initialization of the database. Call initialize() after this. */
void DDIDatabase::forceFullDatabaseRefreshing()
{
    QSqlDatabase::removeDatabase(connectionName());
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
        LOG_FOR("DDIDatabase", QString("Using DDI database version " + version()));
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

/**
 * Read the raw CSV ATC file and populate the database with its data.
 * Returns the number of ATC codes inserted. \n
 * \note The raw ATC file is not included in the git repository, you should ask the mailing list freemedforms-dev@googlegroups.com for it.
 */
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
        // All language not defined?
        for(int i=0; i < vals.count(); ++i)
            vals[i] = vals[i].remove("\"");
        if (vals.at(1).isEmpty() ||
                vals.at(2).isEmpty() ||
                vals.at(3).isEmpty()) {
            // find the defined one (fr or en)
            QString defined;
            if (!vals.at(1).isEmpty()) // EN
                defined = vals.at(1);
            else if (!vals.at(2).isEmpty()) // FR
                defined = vals.at(2);
            else if (!vals.at(3).isEmpty())
                defined = vals.at(3);
            // Then define the non defined
            if (vals.at(1).isEmpty())
                vals[1] = defined;
            if (vals.at(2).isEmpty())
                vals[2] = defined;
            if (vals.at(3).isEmpty())
                vals[3] = defined;
        }
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

/**
 * Read the raw DrugInteractor file and populate the database with its data.
 * Returns the number of interactor inserted. \n
 * \note The raw file is not included in the git repository, you should ask the mailing list freemedforms-dev@googlegroups.com for it.
 */
int DDIDatabase::insertDrugInteractorsDataFromXml(const QString &fileName)
{
    // Check database
    QSqlDatabase DB = QSqlDatabase::database(connectionName());
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return false;

    // Check file
    if (!QFile(fileName).exists()) {
        LOG_ERROR_FOR("DDIDatabase", tkTr(Trans::Constants::FILE_1_DOESNOT_EXISTS).arg(fileName));
        return false;
    }

    // Clean DrugInteractor table from old values
    QString req = prepareDeleteQuery(Constants::Table_INTERACTORS);
    if (!executeSQL(req, DB))
        LOG_ERROR_FOR("DDIDatabase", "Unable to clear old DrugInteractor data");

    // Read the XML file
    DB.transaction();
    QSqlQuery query(DB);
    QString content = Utils::readTextFile(fileName, Utils::DontWarnUser);
    QList<DrugInteractor> list = DrugInteractor::listFromXml(content);
    int n = 0;
    foreach(const DrugInteractor &di, list) {
        if (!di.isValid())
            continue;
        query.prepare(prepareInsertQuery(Constants::Table_INTERACTORS));
        query.bindValue(Constants::INTERACTOR_ID, QVariant());
        query.bindValue(Constants::INTERACTOR_UID, di.data(DrugInteractor::Uid));
        query.bindValue(Constants::INTERACTOR_ISVALID, di.isValid()?"1":"0");
        query.bindValue(Constants::INTERACTOR_ISREVIEWED, di.isReviewed()?"1":"0");
        query.bindValue(Constants::INTERACTOR_ISAUTOFOUND, di.isAutoFound()?"1":"0");
        query.bindValue(Constants::INTERACTOR_ISCLASS, di.isClass()?"1":"0");
        query.bindValue(Constants::INTERACTOR_WARNDUPLICATES, di.data(DrugInteractor::DoNotWarnDuplicated).toBool()?"1":"0");
        query.bindValue(Constants::INTERACTOR_ATC, di.data(DrugInteractor::ATCCodeStringList).toStringList().join(";"));
        query.bindValue(Constants::INTERACTOR_CHILDREN, di.childrenIds().join(";"));
        query.bindValue(Constants::INTERACTOR_FR, di.data(DrugInteractor::FrLabel));
        query.bindValue(Constants::INTERACTOR_EN, di.data(DrugInteractor::EnLabel));
        query.bindValue(Constants::INTERACTOR_DE, di.data(DrugInteractor::DeLabel));
        query.bindValue(Constants::INTERACTOR_INFO_FR, di.data(DrugInteractor::ClassInformationFr));
        query.bindValue(Constants::INTERACTOR_INFO_EN, di.data(DrugInteractor::ClassInformationEn));
        query.bindValue(Constants::INTERACTOR_INFO_DE, di.data(DrugInteractor::ClassInformationDe));
        query.bindValue(Constants::INTERACTOR_DATECREATE, di.data(DrugInteractor::DateOfCreation));
        query.bindValue(Constants::INTERACTOR_DATEUPDATE, di.data(DrugInteractor::DateLastUpdate));
        query.bindValue(Constants::INTERACTOR_DATEREVIEW, di.data(DrugInteractor::DateOfReview));
        query.bindValue(Constants::INTERACTOR_REF, di.data(DrugInteractor::Reference));
        query.bindValue(Constants::INTERACTOR_PMIDS, QVariant());
        query.bindValue(Constants::INTERACTOR_COMMENT, di.data(DrugInteractor::Comment));
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
    return n;
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

    // Insert raw data & version
    setVersion(::CURRENTVERSION);
    insertAtcDataFromCsv(settings()->path(Core::ISettings::BundleResourcesPath) + Constants::ATC_CSV_FILENAME);
    insertDrugInteractorsDataFromXml(settings()->path(Core::ISettings::BundleResourcesPath) + Constants::INTERACTORS_XML_FILENAME);

    // database is readable/writable
    LOG_FOR("DDIDatabase", tkTr(Trans::Constants::DATABASE_1_CORRECTLY_CREATED).arg(pathOrHostName + QDir::separator() + prefixedDbName));
    return true;
}
