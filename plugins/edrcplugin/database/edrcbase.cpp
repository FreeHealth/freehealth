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
 *  This program is distributed in the hope that it will be useful, *
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
/**
 * \class eDRC::Internal::DrcDatabase
 * Contains the eDRC data imported from the SFMG XLS file ported to CSV.\n
 * This database does not contains any other informations.
 *
 * Datapack usage:\n
 * The database can be downloaded from datapack servers or pre-installed in the
 * Datapack Application Installed path (\sa Core::ISettings::DatapackAppInstalled).
 */

#include "edrcbase.h"
#include "constants_db.h"
#include <edrcplugin/consultresult.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/databaseconnector.h>
#include <utils/databasecsvimportator.h>
#include <translationutils/constants.h>
#include <translationutils/trans_database.h>
#include <translationutils/trans_msgerror.h>

#include <QFileInfo>
#include <QDir>

using namespace eDRC;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace {
// Try to connect the database, returns true if correctly connected
static inline bool connectDatabase(QSqlDatabase &DB, const QString &file, const int line)
{
    if (!DB.isOpen()) {
        if (!DB.open()) {
            Utils::Log::addError("DrcDatabase", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                 .arg(DB.connectionName()).arg(DB.lastError().text()),
                                 file, line);
            return false;
        }
    }
    return true;
}
} // namespace anonymous

DrcDatabase::DrcDatabase(const QString &absPathToDb) :
    Utils::Database(),
    _initialized(false),
    _databasePath(absPathToDb)
{
    // Create the eDRC read-only database
    using namespace eDRC::Constants;
    setConnectionName(DB_NAME);

    addTable(Table_REF_RC, "REF_RC");
    addField(Table_REF_RC, REF_RC_SEQ, "REF_RC_SEQ", FieldIsUniquePrimaryKey);
    addField(Table_REF_RC, LIB_RC_FR, "LIB_RC_FR", FieldIsShortText);
    addField(Table_REF_RC, VALIDITE, "VALIDITE", FieldIsBoolean);
    addField(Table_REF_RC, PA, "PA", FieldIsBoolean);
    addField(Table_REF_RC, PB, "PB", FieldIsBoolean);
    addField(Table_REF_RC, PC, "PC", FieldIsBoolean);
    addField(Table_REF_RC, PD, "PD", FieldIsBoolean);
    addField(Table_REF_RC, PZ, "PZ", FieldIsBoolean);
    addField(Table_REF_RC, P1, "P1", FieldIsBoolean);
    addField(Table_REF_RC, P2, "P2", FieldIsBoolean);
    addField(Table_REF_RC, P3, "P3", FieldIsBoolean);
    addField(Table_REF_RC, P4, "P4", FieldIsBoolean);
    addField(Table_REF_RC, P5, "P5", FieldIsBoolean);
    addField(Table_REF_RC, REG_RC_LIB, "REG_RC_LIB", FieldIsShortText);
    addField(Table_REF_RC, REG_RC_ID, "REG_RC_ID", FieldIsInteger);
    addField(Table_REF_RC, SUB_RC_ID, "SUB_RC_ID", FieldIsInteger);
    addField(Table_REF_RC, VALID_DEBUT, "VALID_DEBUT", FieldIsDate);
    addField(Table_REF_RC, VALID_FIN, "VALID_FIN", FieldIsDate);
    addField(Table_REF_RC, ARGUMENTAIRE, "ARGUMENTAIRE", FieldIsLongText);
    addField(Table_REF_RC, NATURE, "NATURE", FieldIsInteger);

    addTable(Table_Ref_RCItem, "REF_RCITEM");
    addField(Table_Ref_RCItem, REF_RCITEM_SEQ, "REF_RCITEM_SEQ", FieldIsUniquePrimaryKey);
    addField(Table_Ref_RCItem, LIB_RCITEM_FR, "LIB_RCITEM_FR", FieldIsShortText);

    addTable(Table_Ref_RCE, "REF_RCE");
    addField(Table_Ref_RCE, REF_RCE_SEQ, "REF_RCE_SEQ", FieldIsUniquePrimaryKey);
    addField(Table_Ref_RCE, RCE_LIB, "RCE_LIB", FieldIsShortText);

    addTable(Table_Ref_ClassRC, "REF_CLASSRC");
    addField(Table_Ref_ClassRC, REF_CLASSRC_SEQ, "REF_CLASSRC_SEQ", FieldIsUniquePrimaryKey);
    addField(Table_Ref_ClassRC, REF_CLASSRC_LIB, "REF_CLASSRC_LIB", FieldIsShortText);

    addTable(Table_Ref_CIM10, "REF_CIM10");
    addField(Table_Ref_CIM10, REF_CIM10_SEQ, "REF_CIM10_SEQ", FieldIsUniquePrimaryKey);
    addField(Table_Ref_CIM10, COD_CIM10, "COD_CIM10", FieldIsShortText);
    addField(Table_Ref_CIM10, CIM10_LIBELLE, "LIBELLE", FieldIsShortText);

    addTable(Table_RC_Link_RC_Criteres, "REF_L_RC_CRITERES");
    addField(Table_RC_Link_RC_Criteres, RC_LCRITERES_REF_LRCCRITERES_SEQ, "REF_LRCCRITERES_SEQ", FieldIsUniquePrimaryKey);
    addField(Table_RC_Link_RC_Criteres, RC_LCRITERES_REF_RC_ID, "REF_RC_ID", FieldIsInteger);
    addField(Table_RC_Link_RC_Criteres, RC_LCRITERES_AFFICH_ORDRE, "AFFICH_ORDRE", FieldIsInteger);
    addField(Table_RC_Link_RC_Criteres, RC_LCRITERES_REF_RCITEM_ID, "REF_RCITEM_ID", FieldIsInteger);
    addField(Table_RC_Link_RC_Criteres, RC_LCRITERES_REF_RETRAIT_ID, "REF_RETRAIT_ID", FieldIsInteger);
    addField(Table_RC_Link_RC_Criteres, RC_LCRITERES_REF_PONDER_ID, "REF_PONDER_ID", FieldIsInteger);
    addField(Table_RC_Link_RC_Criteres, RC_LCRITERES_LIB_CRITERES_FR, "LIB_CRITERES_FR", FieldIsShortText);

    addTable(Table_RC_Link_RC_RCE, "REF_L_RC_RCE");
    addField(Table_RC_Link_RC_RCE, RC_LRCE_REF_RC_ID, "REF_RC_ID", FieldIsInteger);
    addField(Table_RC_Link_RC_RCE, RC_LRCE_REF_RCE_ID, "REF_RCE_ID", FieldIsInteger);

    addTable(Table_RC_Link_RC_CIM10, "REF_L_RC_CIM10");
    addField(Table_RC_Link_RC_CIM10, RC_LCIM_REF_CIM10_ID, "REF_CIM10_ID", FieldIsInteger);
    addField(Table_RC_Link_RC_CIM10, RC_LCIM_REF_RC_ID, "REF_RC_ID", FieldIsInteger);
    addField(Table_RC_Link_RC_CIM10, RC_LCIM_CONTEXTE, "CONTEXTE", FieldIsShortText);

    addTable(Table_RC_Link_RC_Class, "REF_L_RC_CLASS");
    addField(Table_RC_Link_RC_Class, RC_LCLASS_REF_RC_ID, "REF_RC_ID", FieldIsInteger);
    addField(Table_RC_Link_RC_Class, RC_LCLASS_REF_CLASSRC_ID, "REF_CLASSRC_ID", FieldIsInteger);

    addTable(Table_RC_Link_VA, "REF_L_RC_VOIR_AUSSI");
    addField(Table_RC_Link_VA, VOIR_AUSSI_REF_RC_ID, "REF_RC_ID", FieldIsInteger);
    addField(Table_RC_Link_VA, VOIR_AUSSI_ID, "VOIR_AUSSI_ID", FieldIsInteger);

    addTable(Table_Version, "VERSION");
    addField(Table_Version, VERSION_TEXT, "CURRENT", FieldIsShortText);
    addField(Table_Version, VERSION_DATE, "DATE", FieldIsShortText);
}

DrcDatabase::~DrcDatabase()
{}

/**
 * Initialize the database. \n
 * By default, the connection is defined to eDRC::Constants::DB_NAME.
 * If you want your own connection name, you can set it with the
 * setConnectionName() and you must set it \b before calling initialize(). \n
 * If you want to create the database from the rawsource CSV, you must set the
 * \e createIfNotExists to true and set the correct absolute path to the csv files
 * \e absPathToCsvRawSourceFiles.
 */
bool DrcDatabase::initialize(bool createIfNotExists, const QString &absPathToCsvRawSourceFiles)
{
    if (_initialized)
        return true;
    _absPathToCsvRawSourceFiles = absPathToCsvRawSourceFiles;
    setDriver(Utils::Database::SQLite);

    // test driver
    if (!QSqlDatabase::isDriverAvailable("QSQLITE")) {
        LOG_ERROR_FOR("DrcDatabase", tkTr(Trans::Constants::DATABASE_DRIVER_1_NOT_AVAILABLE).arg("SQLite"));
        Utils::warningMessageBox(tkTr(Trans::Constants::APPLICATION_FAILURE),
                                 tkTr(Trans::Constants::DATABASE_DRIVER_1_NOT_AVAILABLE_DETAIL).arg("SQLite"),
                                 "", qApp->applicationName());
        return false;
    }

    // Connect Database
    Utils::DatabaseConnector connector;
    connector.setAbsPathToReadOnlySqliteDatabase(_databasePath);
    connector.setHost(QFileInfo(_databasePath + QDir::separator() + Constants::DB_FILENAME).fileName());
    connector.setAccessMode(Utils::DatabaseConnector::ReadOnly);
    connector.setDriver(Utils::Database::SQLite);

    LOG_FOR("DrcDatabase", tkTr(Trans::Constants::SEARCHING_DATABASE_1_IN_PATH_2)
            .arg(connectionName()).arg(_databasePath));

    if (createIfNotExists) {
        createConnection(connectionName(), Constants::DB_FILENAME,
                         connector,
                         Utils::Database::CreateDatabase);
    } else {
        createConnection(connectionName(), Constants::DB_FILENAME,
                         connector,
                         Utils::Database::WarnOnly);
    }

    if (!database().isOpen()) {
        if (!database().open()) {
            LOG_ERROR_FOR("DrcDatabase",tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(connectionName()).arg(database().lastError().text()));
        } else {
            LOG_FOR("DrcDatabase",tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().databaseName()).arg(database().driverName()));
        }
    } else {
        LOG_FOR("DrcDatabase", tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().databaseName()).arg(database().driverName()));
    }

    if (!checkDatabaseScheme()) {
        LOG_ERROR_FOR("DrcDatabase",tkTr(Trans::Constants::DATABASE_1_SCHEMA_ERROR).arg(connectionName()));
        return false;
    }

    if (!checkDatabaseVersion()) {
        LOG_ERROR_FOR("DrcDatabase",
                      QString("Wrong database version. Db: %1; Current: %2")
                      .arg(getVersion(Utils::Field(Constants::Table_Version, Constants::VERSION_TEXT)))
                      .arg(Constants::DB_VERSION));
        return false;
    } else {
        LOG_FOR("DrcDatabase",
                QString("Using database version: %1")
                .arg(getVersion(Utils::Field(Constants::Table_Version, Constants::VERSION_TEXT)));
    }

    _initialized = true;
    return true;
}

/** Set the version of the database and return true in case of success */
bool DrcDatabase::setVersion(const Utils::Field &field, const QString &version)
{
    Q_UNUSED(field);
    QSqlDatabase DB = QSqlDatabase::database(connectionName());
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return false;
    executeSQL(prepareDeleteQuery(Constants::Table_Version, QHash<int,QString>()), DB);
    QSqlQuery query(DB);
    query.prepare(prepareInsertQuery(Constants::Table_Version));
    query.bindValue(Constants::VERSION_TEXT, version);
    query.bindValue(Constants::VERSION_DATE, QDate::currentDate().toString(Qt::ISODate));
    if (!query.exec()) {
        LOG_QUERY_ERROR_FOR("DrcDatabase", query);
        return false;
    }
    return true;
}

/** Check the current version (must be the last avaialble one) */
bool DrcDatabase::checkDatabaseVersion() const
{
    return (getVersion(Utils::Field(Constants::Table_Version, Constants::VERSION_TEXT))==Constants::DB_VERSION);
}

/**
 * This member is automatically called by intialize() if you set the \e createIfNotExists to \e true.\n
 * Creates an empty eDRC database into the Core::DatabaseConnector::absPathToSqliteReadOnlyDatabase()
 * path. \n
 * Populate it with CSV files if they are available on the computer. These files
 * are searched in the path notified in the ctor or in the initialize() member.
 * \sa DrcDatabase(), initialize()
 */
bool DrcDatabase::createDatabase(const QString &connection, const QString &prefixedDbName,
                                 const Utils::DatabaseConnector &connector,
                                 CreationOption createOption
                                 )
{
    if (connection != connectionName())
        return false;
    if (connector.driver() != SQLite)
        return false;

    if (createOption!=Utils::Database::CreateDatabase)
        return false;

    QString pathOrHostName = connector.absPathToSqliteReadOnlyDatabase() + QDir::separator() + QString(connectionName());
    LOG_FOR("DrcDatabase", tkTr(Trans::Constants::TRYING_TO_CREATE_1_PLACE_2).arg(prefixedDbName).arg(pathOrHostName));

    setConnectionName(connectionName());
    setDriver(connector.driver());

    // create an empty database and connect
    if (QSqlDatabase::connectionNames().contains(connectionName())) {
        QSqlDatabase::removeDatabase(connectionName());
    }

    if (!Utils::checkDir(pathOrHostName, true, "DrcDatabase")) {
        LOG_ERROR_FOR("DrcDatabase", tkTr(Trans::Constants::_1_ISNOT_AVAILABLE_CANNOTBE_CREATED).arg(pathOrHostName));
        return false;
    }

    QSqlDatabase DB;
    DB = QSqlDatabase::addDatabase("QSQLITE", connectionName());
    DB.setDatabaseName(QDir::cleanPath(pathOrHostName + QDir::separator() + prefixedDbName));
    if (!DB.open())
        LOG_FOR("DrcDatabase", tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2).arg(prefixedDbName).arg(DB.lastError().text()));
    setDriver(Utils::Database::SQLite);

    // create db structure
    if (createTables()) {
        LOG_FOR("DrcDatabase", tkTr(Trans::Constants::DATABASE_1_CORRECTLY_CREATED).arg(prefixedDbName));
    } else {
        LOG_ERROR_FOR("DrcDatabase", tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2)
                      .arg(prefixedDbName, DB.lastError().text()));
        return false;
    }

    setVersion(Utils::Field(), Constants::DB_VERSION);

    // Populate database with the csv files
    QList<int> tables;
    tables << Constants::Table_REF_RC
           << Constants::Table_Ref_RCItem
           << Constants::Table_Ref_RCE
           << Constants::Table_Ref_ClassRC
           << Constants::Table_Ref_CIM10
           << Constants::Table_RC_Link_RC_CIM10
           << Constants::Table_RC_Link_RC_Class
           << Constants::Table_RC_Link_RC_Criteres
           << Constants::Table_RC_Link_RC_RCE
           << Constants::Table_RC_Link_VA;

    if (QDir(_absPathToCsvRawSourceFiles).exists()) {
        Utils::DatabaseCsvImportator import;
        foreach(int table, tables) {
            Utils::ImportationJob job;
            job.absFilePath = QString("%1/%2.csv").arg(_absPathToCsvRawSourceFiles).arg(this->table(table));
            job.databaseConnectionName = connection;
            job.tableName = this->table(table);
            job.fieldSeparator = ',';
            job.textEscapingChar = '~';
            job.omitFirstLine = true;
            import.addJob(job);
        }
        Utils::ImportationResult result = import.startJobs();
        if (result.hasError)
            LOG_ERROR_FOR("DrcDatabase", result.errors.join("\n"));
    } else {
        LOG_ERROR_FOR("DrcDatabase", "Unable to import CSV files, path does not exists");
    }

    // database is readable/writable
    LOG_FOR("DrcDatabase", tkTr(Trans::Constants::DATABASE_1_CORRECTLY_CREATED).arg(pathOrHostName + QDir::separator() + prefixedDbName));
    return true;
}

/** Returns all RC Classes primKey & label in a hash */
QHash<int, QString> DrcDatabase::getCrClasses() const
{
    QHash<int, QString> toReturn;
    QSqlDatabase DB = database();
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return toReturn;
    QSqlQuery query(DB);
    if (query.exec(select(Constants::Table_Ref_ClassRC))) {
        while (query.next()) {
            toReturn.insert(query.value(Constants::REF_CLASSRC_SEQ).toInt(), query.value(Constants::REF_CLASSRC_LIB).toString());
        }
    }
    return toReturn;
}

/** Returns all RC ids and labels (unsorted) for a specific class in a hash */
QHash<int, QString> DrcDatabase::getCrForClasses(int classId) const
{
    QHash<int, QString> toReturn;
    QSqlDatabase DB = database();
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return toReturn;
    Utils::FieldList get;
    get << Utils::Field(Constants::Table_REF_RC, Constants::REF_RC_SEQ);
    get << Utils::Field(Constants::Table_REF_RC, Constants::LIB_RC_FR);
    Utils::FieldList conds;
    conds << Utils::Field(Constants::Table_REF_RC, Constants::VALIDITE, "='1'");
    conds << Utils::Field(Constants::Table_RC_Link_RC_Class, Constants::RC_LCLASS_REF_CLASSRC_ID, QString("='%1'").arg(classId));
    Utils::JoinList joins;
    joins << Utils::Join(Constants::Table_RC_Link_RC_Class, Constants::RC_LCLASS_REF_RC_ID,
                         Constants::Table_REF_RC, Constants::REF_RC_SEQ);
    QSqlQuery query(DB);
    if (query.exec(select(get, joins, conds))) {
        while (query.next()) {
            toReturn.insert(query.value(0).toInt(), query.value(1).toString());
        }
    }
    return toReturn;
}

/** Returns all classes id and name for a specific CR \e crId */
QHash<int, QString> DrcDatabase::getClassesForCr(int crId) const
{
    QHash<int, QString> toReturn;
    QSqlDatabase DB = database();
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return toReturn;
    Utils::FieldList get;
    get << Utils::Field(Constants::Table_Ref_ClassRC, Constants::REF_CLASSRC_SEQ);
    get << Utils::Field(Constants::Table_Ref_ClassRC, Constants::REF_CLASSRC_LIB);
    Utils::FieldList conds;
    conds << Utils::Field(Constants::Table_REF_RC, Constants::VALIDITE, "='1'");
    conds << Utils::Field(Constants::Table_RC_Link_RC_Class, Constants::RC_LCLASS_REF_RC_ID, QString("='%1'").arg(crId));
    Utils::JoinList joins;
    joins << Utils::Join(Constants::Table_Ref_ClassRC, Constants::REF_CLASSRC_SEQ,
                         Constants::Table_RC_Link_RC_Class, Constants::RC_LCLASS_REF_CLASSRC_ID);
    joins << Utils::Join(Constants::Table_REF_RC, Constants::REF_RC_SEQ,
                         Constants::Table_RC_Link_RC_Class, Constants::RC_LCLASS_REF_RC_ID);
    QSqlQuery query(DB);
    if (query.exec(select(get, joins, conds))) {
        while (query.next()) {
            if (query.value(1).toString() != "TOUTES") {
                toReturn.insert(query.value(0).toInt(), query.value(1).toString());
            }
        }
    }
    return toReturn;
}

/** Returns all SeeAlso (related) RC ids and labels (unsorted) for a specific RC in a hash */
QHash<int, QString> DrcDatabase::getSeeAlsoRcForCr(int rcId) const
{
    QHash<int, QString> toReturn;
    QSqlDatabase DB = database();
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return toReturn;
    Utils::FieldList get;
    get << Utils::Field(Constants::Table_REF_RC, Constants::REF_RC_SEQ);
    get << Utils::Field(Constants::Table_REF_RC, Constants::LIB_RC_FR);
    Utils::FieldList conds;
    conds << Utils::Field(Constants::Table_REF_RC, Constants::VALIDITE, "='1'");
    conds << Utils::Field(Constants::Table_RC_Link_VA, Constants::VOIR_AUSSI_REF_RC_ID, QString("='%1'").arg(rcId));
    Utils::JoinList joins;
    joins << Utils::Join(Constants::Table_RC_Link_VA, Constants::VOIR_AUSSI_ID,
                         Constants::Table_REF_RC, Constants::REF_RC_SEQ);
    QSqlQuery query(DB);
    if (query.exec(select(get, joins, conds))) {
        while (query.next()) {
            toReturn.insert(query.value(0).toInt(), query.value(1).toString());
        }
    }
    return toReturn;
}

/** Return the label of the RC with the id \e rcId. If \e onlyValid is true, filter only valid RC */
QString DrcDatabase::getCrLabel(const int rcId, bool onlyValid) const
{
    QSqlDatabase DB = database();
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return QString::null;

    QHash<int, QString> where;
    where.insert(Constants::REF_RC_SEQ, QString("=%1").arg(rcId));
    if (onlyValid)
        where.insert(Constants::VALIDITE, QString("=1"));

    QSqlQuery query(DB);
    if (query.exec(select(Constants::Table_REF_RC, Constants::LIB_RC_FR, where))) {
        if (query.next()) {
            return query.value(0).toString();
        }
    } else {
        LOG_QUERY_ERROR_FOR("DrcDatabase", query);
        return QString::null;
    }
    return QString::null;
}

/**
 * Return the arguments of the RC with the id \e rcId.
 * If \e onlyValid is true, filter only valid RC
 */
QString DrcDatabase::getCrArguments(const int rcId, bool toHtml, bool onlyValid) const
{
    QSqlDatabase DB = database();
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return QString::null;

    QHash<int, QString> where;
    where.insert(Constants::REF_RC_SEQ, QString("=%1").arg(rcId));
    if (onlyValid)
        where.insert(Constants::VALIDITE, QString("=1"));

    QSqlQuery query(DB);
    QString content;
    if (query.exec(select(Constants::Table_REF_RC, Constants::ARGUMENTAIRE, where))) {
        if (query.next()) {
            content = query.value(0).toString();
        }
    } else {
        LOG_QUERY_ERROR_FOR("DrcDatabase", query);
        return QString::null;
    }
    if (content.isEmpty() || !toHtml)
        return content;
    content = content.replace("RISQUES CRITIQUES EVITABLES (RCE)\n", "<div class=\"headtitle\">RISQUES CRITIQUES ÉVITABLES (RCE)</div>\n");
    content = content.replace("ARGUMENTAIRE\n", "<div class=\"headtitle\">ARGUMENTAIRE</div>\n");
    content = content.replace("DENOMINATION\n", "<div class=\"title\">DÉNOMINATION</div>\n");
    content = content.replace("CRITERES D’INCLUSION\n", "<div class=\"title\">CRITÈRES D’INCLUSION</div>\n");
    content = content.replace("INCLUSIONS\n", "<div class=\"title\">INCLUSIONS</div>\n");
    content = content.replace("VOIR AUSSI\n", "<div class=\"title\">VOIR AUSSI</div>\n");
    content = content.replace("POSITIONS DIAGNOSTIQUES\n", "<div class=\"title\">POSITIONS DIAGNOSTIQUES</div>\n");
    content = content.replace("POSITION(S) DIAGNOSTIQUE(S)\n", "<div class=\"title\">POSITION(S) DIAGNOSTIQUE(S)</div>\n");
    content = content.replace("COMPLEMENTS SEMIOLOGIQUES\n", "<div class=\"title\">COMPLEMENTS SÉMIOLOGIQUES</div>\n");

    content = content.replace("Critères obligatoires", "<span class=\"subtitle\">Critères obligatoires</span>");
    content = content.replace("Critères facultatifs", "<span class=\"subtitle\">Critères facultatifs</span>");

    content = content.replace("\n", "<br>");
    content.prepend("<style type=\"text/css\">"
                    ".content {"
                    "    margin: 0px;"
                    //                    "    font-size: 10pt;"  // TODO: use a setting
                    "}"
                    ".headtitle {"
                    "    margin-top: 10px;"
                    "    font-weight: 600;"
                    "    text-align: center;"
                    "}"
                    ".title {"
                    "    margin-top: 10px;"
                    "    font-weight: 600;"
                    //"    background-color: lightgray;"
                    "}"
                    ".subtitle {"
                    "    margin-top: 5px;"
                    "    font-weight: 600;"
                    "}"
                    "</style>"
                    "<div class=\"content\">"
                    );
    content = QString("<%1><%2>%3</div></%2></%1>").arg("html").arg("body").arg(content);
    // Utils::saveStringToFile(content, "/Users/eric/essai.htm", Utils::Overwrite, Utils::DontWarnUser);
    return content;
}

QStringList DrcDatabase::getCrAuthorizedDiagnosis(const int rcId, bool onlyValid) const
{
    QSqlDatabase DB = database();
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return QStringList();

    QHash<int, QString> where;
    where.insert(Constants::REF_RC_SEQ, QString("=%1").arg(rcId));
    if (onlyValid)
        where.insert(Constants::VALIDITE, QString("=1"));

    QSqlQuery query(DB);
    QStringList toReturn;
    if (query.exec(select(Constants::Table_REF_RC, QList<int>()
                          << Constants::PA
                          << Constants::PB
                          << Constants::PC
                          << Constants::PD
                          << Constants::PZ
                          << Constants::P1
                          << Constants::P2
                          << Constants::P3
                          << Constants::P4
                          << Constants::P5, where))) {
        if (query.next()) {
            if (query.value(0).toBool())
                toReturn << "A";
            if (query.value(1).toBool())
                toReturn << "B";
            if (query.value(2).toBool())
                toReturn << "C";
            if (query.value(3).toBool())
                toReturn << "D";
            if (query.value(4).toBool())
                toReturn << "Z";
            if (query.value(5).toBool())
                toReturn << "1";
            if (query.value(6).toBool())
                toReturn << "2";
            if (query.value(7).toBool())
                toReturn << "3";
            if (query.value(8).toBool())
                toReturn << "4";
            if (query.value(9).toBool())
                toReturn << "5";
        }
    } else {
        LOG_QUERY_ERROR_FOR("DrcDatabase", query);
        return toReturn;
    }
    return toReturn;
}

QStringList DrcDatabase::getCrIcd10RelatedCodes(const int rcId, bool onlyValid) const
{
    QSqlDatabase DB = database();
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return QStringList();

    Utils::FieldList get;
    get << Utils::Field(Constants::Table_Ref_CIM10, Constants::COD_CIM10);
    get << Utils::Field(Constants::Table_Ref_CIM10, Constants::CIM10_LIBELLE);
    Utils::FieldList conds;
    if (onlyValid)
        conds << Utils::Field(Constants::Table_REF_RC, Constants::VALIDITE, "='1'");
    conds << Utils::Field(Constants::Table_RC_Link_RC_CIM10, Constants::RC_LCIM_REF_RC_ID, QString("='%1'").arg(rcId));
    Utils::JoinList joins;
    joins << Utils::Join(Constants::Table_RC_Link_RC_CIM10, Constants::RC_LCIM_REF_CIM10_ID,
                         Constants::Table_Ref_CIM10, Constants::REF_CIM10_SEQ);

    QSqlQuery query(DB);
    QStringList toReturn;
    if (query.exec(select(get, joins, conds))) {
        if (query.next()) {
            if (!query.value(1).toString().isEmpty())
                toReturn << QString("%1 (%2)").arg(query.value(0).toString()).arg(query.value(1).toString());
            else
                toReturn << query.value(0).toString();
        }
    } else {
        LOG_QUERY_ERROR_FOR("DrcDatabase", query);
        return toReturn;
    }
    return toReturn;
}

QList<ConsultResultCriteria> DrcDatabase::getOrderedCriteriasForCr(int crId) const
{
    QList<ConsultResultCriteria> toReturn;
    QSqlDatabase DB = database();
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return toReturn;
    // Construct SQL query
    QHash<int, QString> where;
    where.insert(Constants::RC_LCRITERES_REF_RC_ID, QString("='%1'").arg(crId));
    QString req = select(Constants::Table_RC_Link_RC_Criteres, where);
    req += QString(" ORDER BY %1 ASC;").arg(fieldName(Constants::Table_RC_Link_RC_Criteres, Constants::RC_LCRITERES_AFFICH_ORDRE));
    // Get criterias
    QSqlQuery query(DB);
    if (query.exec(req)) {
        while (query.next()) {
            ConsultResultCriteria crit;
            crit.setId(query.value(Constants::RC_LCRITERES_REF_LRCCRITERES_SEQ).toInt());
            crit.setIndentation(query.value(Constants::RC_LCRITERES_REF_RETRAIT_ID).toInt());
            crit.setWeight(query.value(Constants::RC_LCRITERES_REF_PONDER_ID).toInt());
            crit.setSortIndex(query.value(Constants::RC_LCRITERES_AFFICH_ORDRE).toInt());
            crit.setLabel(query.value(Constants::RC_LCRITERES_LIB_CRITERES_FR).toString());

            int crItemId = query.value(Constants::RC_LCRITERES_REF_RCITEM_ID).toInt();
            where.clear();
            where.insert(Constants::REF_RCITEM_SEQ, QString("='%1'").arg(crItemId));
            req = select(Constants::Table_Ref_RCItem, where);
            QSqlQuery queryItem(DB);
            if (queryItem.exec(req)) {
                if (queryItem.next()) {
                    crit.setComment(queryItem.value(Constants::LIB_RCITEM_FR).toString());
                }
            } else {
                LOG_QUERY_ERROR_FOR("DrcDatabase", query);
            }
            toReturn << crit;
        }
    } else {
        LOG_QUERY_ERROR_FOR("DrcDatabase", query);
        return toReturn;
    }
    return toReturn;
}
