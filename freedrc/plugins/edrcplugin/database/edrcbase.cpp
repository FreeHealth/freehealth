/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

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

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

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

// Find the database to use. In priority order:
// - User datapack
// - Application installed datapack
static QString databasePath()
{
    QString dbRelPath = QString("/%1/%2").arg(Constants::DB_NAME).arg(Constants::DB_FILENAME);
    QString tmp;
    tmp = settings()->dataPackInstallPath() + dbRelPath;
    if (QFileInfo(tmp).exists())
        return settings()->dataPackInstallPath();
    return settings()->dataPackApplicationInstalledPath();
}

static QString databaseFileName()
{
    return databasePath() + QDir::separator() + Constants::DB_FILENAME;
}
} // namespace anonymous

DrcDatabase::DrcDatabase() :
    Utils::Database(),
    _initialized(false)
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
 * Initialize the database (create it is required). \n
 * By default, the connection is defined to eDRC::Constants::DB_NAME.
 * If you want your own connection name, you can set it with the
 * setConnectionName() and you must set it \b before calling initialize().
 */
bool DrcDatabase::initialize(bool createIfNotExists)
{
    if (_initialized)
        return true;
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
    connector.setAbsPathToReadOnlySqliteDatabase(databasePath());
    connector.setHost(QFileInfo(databaseFileName()).fileName());
    connector.setAccessMode(Utils::DatabaseConnector::ReadOnly);
    connector.setDriver(Utils::Database::SQLite);

    LOG_FOR("DrcDatabase", tkTr(Trans::Constants::SEARCHING_DATABASE_1_IN_PATH_2)
            .arg(connectionName()).arg(databasePath()));

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
            LOG_FOR("DrcDatabase",tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().connectionName()).arg(database().driverName()));
        }
    } else {
        LOG_FOR("DrcDatabase", tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().connectionName()).arg(database().driverName()));
    }

    if (!checkDatabaseScheme()) {
        LOG_ERROR_FOR("DrcDatabase",tkTr(Trans::Constants::DATABASE_1_SCHEMA_ERROR).arg(connectionName()));
        return false;
    }

    if (!checkDatabaseVersion()) {
        LOG_ERROR_FOR("DrcDatabase", QString("Wrong database version. Db: %1; Current: %2")
                      .arg(version()).arg(Constants::DB_VERSION));
        return false;
    } else {
        LOG_FOR("DrcDatabase", QString("Using database version " + version()));
    }


    //    importCsvToDatabase(Constants::DB_NAME,
    //                        "/Volumes/HDD/Users/eric/Desktop/Programmation/FreeMedForms-Releases/eDRC/csv/referentiel_2005-11901705bf10299b1.csv",
    //                        "REF_RC",
    //                        ",",
    //                        true);


    _initialized = true;
    return true;
}

/** Set the version of the database and return true in case of success */
bool DrcDatabase::setVersion(const QString &version)
{
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
    }
    return true;
}

/** Return the current version of the database */
QString DrcDatabase::version() const
{
    QSqlDatabase DB = QSqlDatabase::database(connectionName());
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return QString::null;
    QSqlQuery query(DB);
    query.prepare(select(Constants::Table_Version));
    if (!query.exec()) {
        LOG_QUERY_ERROR_FOR("DrcDatabase", query);
    } else {
        if (query.next()) {
            return query.value(Constants::VERSION_TEXT).toString();
        }
    }
    return QString::null;
}

/** Check the current version (must be the last avaialble one) */
bool DrcDatabase::checkDatabaseVersion() const
{
    return (version()==Constants::DB_VERSION);
}

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

    setVersion(Constants::DB_VERSION);

    // Populate database with the csv files
    QString pathToCsv = settings()->path(Core::ISettings::BundleResourcesPath);
    pathToCsv += "/nonfree/edrc";
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

    if (QDir(pathToCsv).exists()) {
        Utils::DatabaseCsvImportator import;
        foreach(int table, tables) {
            Utils::ImportationJob job;
            job.absFilePath = QString("%1/%2.csv").arg(pathToCsv).arg(this->table(table));
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
    }

    // database is readable/writable
    LOG_FOR("DrcDatabase", tkTr(Trans::Constants::DATABASE_1_CORRECTLY_CREATED).arg(pathOrHostName + QDir::separator() + prefixedDbName));
    return true;
}

/** Returns all RC Classes primKey & label in a hash */
QHash<int, QString> DrcDatabase::getRcClasses() const
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
QHash<int, QString> DrcDatabase::getRcForClasses(int classId) const
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

/** Returns all SeeAlso (related) RC ids and labels (unsorted) for a specific RC in a hash */
QHash<int, QString> DrcDatabase::getSeeAlsoRcForRc(int rcId) const
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
QString DrcDatabase::getRcLabel(const int rcId, bool onlyValid) const
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
QString DrcDatabase::getRcArguments(const int rcId, bool toHtml, bool onlyValid) const
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

QStringList DrcDatabase::getRcAuthprizedDiagnosis(const int rcId, bool onlyValid) const
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

QStringList DrcDatabase::getRcIcd10RelatedCodes(const int rcId, bool onlyValid) const
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

QList<ConsultResultCriteria> DrcDatabase::getOrderedCriteriasForCR(int crId) const
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
            toReturn << crit;
        }
    } else {
        LOG_QUERY_ERROR_FOR("DrcDatabase", query);
        return toReturn;
    }
    return toReturn;
}

//bool DrcDatabase::getRCE_byID(const QString RCE_ID , QString &RCE)
//{
//    if (!connectDatabase(DB, __FILE__, __LINE__))
//        return false;
//    RCE = "";
//    QString req = "SELECT `RCE_LIB` FROM `"+ m_NameTable_Ref_RCE +"` WHERE `REF_RCE_SEQ`='"+ RCE_ID +"';";
//    QSqlQuery sqlQuery (req , m_DataBase);
//    if (sqlQuery.isActive())
//    { if (sqlQuery.next())
//        { RCE = sqlQuery.value(0).toString();
//        }
//    }
//    m_DataBase.close();
//    return true;
//}

//bool DrcDatabase::getRC_byClassID(QListWidget &pListWidget, QString classeID)
//{
//    if (!connectDatabase(DB, __FILE__, __LINE__))
//        return false;
//    QString req = "SELECT `REF_RC_ID` FROM `"+m_NameLinkTable_RC_Class+"` WHERE `REF_CLASSRC_ID`="+ classeID + " ;";
//    QString RC_ID = "";
//    QString req2  = "";
//    QString nom = "";
//    QString id = "";
//    pListWidget.clear();
//    QListWidgetItem* i = 0;

//    QSqlQuery sqlQuery (req , m_DataBase);
//    if (sqlQuery.isActive())
//    { while (sqlQuery.next())
//        { RC_ID = sqlQuery.value(0).toString();
//            getRC_Lib(RC_ID , nom);
//            i = new QListWidgetItem(nom, &pListWidget);
//            i->setData(Qt::UserRole , RC_ID);
//        }
//    }
//    m_DataBase.close();
//    return true;
//}

//bool DrcDatabase::getResultatsConsultation_byLib(QListWidget &pListWidget, QString nom)
//{
//    if (!connectDatabase(DB, __FILE__, __LINE__))
//        return false;
//    //if (nom == "") {   pListWidget.clear(); return true; }
//    QString req = "SELECT `LIB_RC_FR`, `REF_RC_SEQ` FROM `"+ m_NameTable_Ref_RC +"` ";
//    req        += "WHERE  `LIB_RC_FR` LIKE '%"+ nom + "%' AND `VALIDITE`='1' ORDER BY `LIB_RC_FR` ASC;";
//    QString tmp = "";
//    QString id  = "";
//    pListWidget.clear();
//    QListWidgetItem* i = 0;

//    QSqlQuery sqlQuery (req , m_DataBase);
//    if (sqlQuery.isActive())
//    { while (sqlQuery.next())
//        { tmp = sqlQuery.value(0).toString();
//            id  = sqlQuery.value(1).toString();
//            i = new QListWidgetItem(tmp, &pListWidget);
//            i->setData(Qt::UserRole , id);
//        }
//    }
//    m_DataBase.close();
//    return true;

//}

//bool DrcDatabase::getRC_byID(QListWidget &pListWidget, QString RC_ID)
//{
//    if (!connectDatabase(DB, __FILE__, __LINE__))
//        return false;
//    if (RC_ID == "") {   pListWidget.clear(); return false; }
//    pListWidget.clear();
//    QString tmp = "";
//    QListWidgetItem* i = 0;
//    getRC_Lib(RC_ID , tmp);
//    i = new QListWidgetItem(tmp, &pListWidget);
//    i->setData(Qt::UserRole , RC_ID);
//    return true;
//}

//bool DrcDatabase::getCIM10_byID(const QString _ID , QString &code)
//{
//    if (!connectDatabase(DB, __FILE__, __LINE__))
//        return false;
//    if (_ID == "") { code = ""; return false; }
//    code = "";
//    QString tmp = "";
//    QString req = "SELECT `COD_CIM10` FROM `"+ m_NameTable_Ref_CIM10 +"` ";
//    req        += "WHERE  `REF_CIM10_SEQ`= '"+ _ID + "';";
//    QSqlQuery sqlQuery (req , m_DataBase);
//    if (sqlQuery.isActive())
//    { if (sqlQuery.next())
//        { code = sqlQuery.value(0).toString();
//        }
//        else { return false; }
//    }
//    else { return false; }

//    return true;
//}

//bool DrcDatabase::getRC_PosDiag_byRCID(QString RC_ID, bool &posA, bool &posB, bool &posC, bool &posD, bool &posZ)
//{
//    if (!connectDatabase(DB, __FILE__, __LINE__))
//        return false;
//    posA=false; posB=false; posC=false; posD=false;
//    if (RC_ID == "") { return false; }
//    QString req = "SELECT PA, PB, PC, PD, PZ FROM `"+ m_NameTable_Ref_RC +"` ";
//    req        += "WHERE `REF_RC_SEQ`='"+ RC_ID + "' AND `VALIDITE`='1';";
//    QString lib = "";
//    QString id  = "";

//    QSqlQuery sqlQuery (req , m_DataBase);
//    if (sqlQuery.isActive())
//    { if (sqlQuery.next())
//        { posA = sqlQuery.value(0).toBool();
//            posB = sqlQuery.value(1).toBool();
//            posC = sqlQuery.value(2).toBool();
//            posD = sqlQuery.value(3).toBool();
//            posZ = sqlQuery.value(4).toBool();
//        }
//    }
//    m_DataBase.close();
//    return true;
//}

//bool DrcDatabase::getFullRCFrom_RC_ID(QString RC_ID, RC_Elements  &el)
//{
//    bool retour = true;
//    el.clear();
//    el.setId(RC_ID);
//    QString tmp="";
//    if (!getRC_Lib(QString::number(el.getId()) , tmp, DoNotCloseBase))
//        retour = false;

//    el.setLibelle(tmp);
//    if (!getCIM10_byID (QString::number(el.getId()) , tmp))
//        retour = false;

//    el.setCIM10(tmp);
//    // Pos diag autorisées
//    bool a,b,c,d,z;
//    if (!getRC_PosDiag_byRCID(QString::number(el.getId()), a,b,c,d,z))
//        retour = false;

//    el.setAuthorizedPosDiag(a,b,c,d,z);
//    el.setAuthorizedSuivi(true, true, true);

//    if (!getCriteresFrom_RC_ID(QString::number(el.getId()), el.m_Criteres))
//        retour = false;

//    return retour;
//}


///*! \brief Récupère toutes les classes disponibles pour les insérer dans le combobox passé en paramètre. Dans le dataUser de chaque Item = QString de l'id de la classe.
//*/
//bool DrcDatabase::getClassesForComboBox(QComboBox* cbx)
//{
//    if (!connectDatabase(DB, __FILE__, __LINE__))
//        return false;
//    QString req = "SELECT * FROM "+ m_NameTable_Ref_ClassRC;
//    QString lib = "";
//    QString id  = "";

//    QSqlQuery sqlQuery (req , m_DataBase);
//    if (sqlQuery.isActive())
//    { while (sqlQuery.next())
//        { lib = sqlQuery.value(1).toString();
//            id  = sqlQuery.value(0).toString();
//            cbx->addItem(lib, id);
//        }
//    }
//    else qWarning() << sqlQuery.executedQuery() << sqlQuery.lastError().text() << sqlQuery.lastError().driverText ();
//    m_DataBase.close();
//    return true;
//}


//bool DrcDatabase::getCriteresFrom_RC_ID(QListWidget &pListWidget, QString RC_ID)
//{
//    if (!connectDatabase(DB, __FILE__, __LINE__))
//        return false;
//    QString req = "SELECT * FROM `"+m_NameLinkTable_RC_Criteres+"` WHERE `REF_RC_ID`='"+RC_ID+"' ORDER BY AFFICH_ORDRE ASC;";
//    QString tmp = "";
//    QString id  = "";
//    pListWidget.clear();
//    QListWidgetItem* i = 0;

//    QSqlQuery sqlQuery (req , m_DataBase);
//    if (sqlQuery.isActive())
//    { while (sqlQuery.next())
//        { tmp = sqlQuery.value(6).toString();
//            id  = sqlQuery.value(0).toString();
//            i = new QListWidgetItem(tmp, &pListWidget);
//            i->setData(Qt::UserRole , id);
//        }
//    }
//    m_DataBase.close();
//    return true;
//}

///**
// * \brief Récupère la liste des critères liés à la clé primaire du RC passé en paramètre. Il ne restera plus qu'à construire l'arborescence des critères pour faciliter les vérifications.
// * @param listCrit : Pointeur sur une liste de Critères
// * @param RC_ID : Clé primaire de la base de données du RC à récupérer.
// * @return true si tout est OK, false en cas d'erreur.
// * @sa RC_Elements::createCriteresTree() , eDRC_Qt4::on_listWidgetRC_itemSelectionChanged()
// */
//bool DrcDatabase::getCriteresFrom_RC_ID(const QString RC_ID, QList<Criteres_Elements>  &list)
//{
//    list.clear();
//    if (!connectDatabase(DB, __FILE__, __LINE__)) return false;
//    QString req = "SELECT REF_LRCCRITERES_SEQ, AFFICH_ORDRE, LIB_CRITERES_FR, "
//            "REF_RETRAIT_ID, REF_PONDER_ID FROM "
//            "`"+m_NameLinkTable_RC_Criteres+"` "
//            "WHERE `REF_RC_ID`='"+RC_ID+
//            "' ORDER BY AFFICH_ORDRE ASC;";
//    QString tmp = "";
//    QString id  = "";
//    Criteres_Elements *el = 0;

//    QSqlQuery sqlQuery (req , m_DataBase);
//    if (sqlQuery.isActive())
//    {
//        while (sqlQuery.next())
//        {
//            tmp = sqlQuery.value(1).toString();
//            id  = sqlQuery.value(0).toInt();
//            el = new Criteres_Elements(sqlQuery.value(0).toInt(), // ID
//                                       sqlQuery.value(2).toString(), // Lib
//                                       sqlQuery.value(1).toInt(), // Ordre
//                                       sqlQuery.value(4).toInt(), // Ponderation
//                                       sqlQuery.value(3).toInt()      // Retrait
//                                       );
//            if (el)
//            { list.append(*el);
//                delete el; el=0;
//            }
//        }
//    }
//    m_DataBase.close();
//    return true;
//}

//bool DrcDatabase::getVoirAussiFrom_RC_ID(QListWidget &pListWidget, QString RC_ID)
//{
//    if (!connectDatabase(DB, __FILE__, __LINE__))
//        return false;
//    QString req = "SELECT `VOIR_AUSSI_ID` "
//            "FROM `"+m_NameLinkTable_RC_VA+"` "
//            "WHERE `REF_RC_ID`='"+RC_ID+"';";
//    QString tmp = "";
//    QString VA_ID  = "";
//    pListWidget.clear();
//    QListWidgetItem* i = 0;
//    QSqlQuery va_query (req , m_DataBase);

//    if (va_query.isActive())
//    { while (va_query.next())
//        { VA_ID  = va_query.value(0).toString();
//            getRC_Lib(VA_ID , tmp, DoNotCloseBase);
//            i = new QListWidgetItem(tmp, &pListWidget);
//            i->setData(Qt::UserRole , VA_ID);
//        }
//    }

//    pListWidget.sortItems(Qt::AscendingOrder);
//    m_DataBase.close();
//    return true;
//}

//bool DrcDatabase::getRisquesFrom_RC_ID(QListWidget &pListWidget, QString RC_ID)
//{
//    if (!connectDatabase(DB, __FILE__, __LINE__))
//        return false;
//    QString req = "SELECT `REF_RCE_ID` FROM `"+ m_NameLinkTable_RC_RCE +"` WHERE `REF_RC_ID`='"+RC_ID+"';";
//    QString tmp = "";
//    QString RCE_ID  = "";
//    pListWidget.clear();
//    QListWidgetItem* i = 0;

//    QSqlQuery sqlQuery (req , m_DataBase);
//    if (sqlQuery.isActive())
//    { while (sqlQuery.next())
//        { RCE_ID = sqlQuery.value(0).toString();
//            getRCE_byID(RCE_ID , tmp);
//            i = new QListWidgetItem(tmp, &pListWidget);
//            i->setData(Qt::UserRole , RCE_ID);
//        }
//    }
//    m_DataBase.close();
//    return true;
//}


//bool DrcDatabase::getCIM10From_RC_ID (QString RC_ID, QString &code)
//{
//    if (!connectDatabase(DB, __FILE__, __LINE__))
//        return false;
//    QString req = "SELECT `REF_CIM10_ID` FROM `"+ m_NameLinkTable_RC_CIM10 +"` WHERE `REF_RC_ID`='"+RC_ID+"';";
//    QString tmp = "";
//    QString _ID  = "";

//    QSqlQuery sqlQuery (req , m_DataBase);
//    if (sqlQuery.isActive())
//    { if (sqlQuery.next())
//        { _ID = sqlQuery.value(0).toString();
//            getCIM10_byID(_ID , code);
//        }
//    }

//    m_DataBase.close();
//    return true;

//}

//bool DrcDatabase::getCritere_byID(const QString _ID , QString &retour)
//{
//    if (!connectDatabase(DB, __FILE__, __LINE__))
//        return false;
//    QString req = "SELECT `LIB_CRITERES_FR` FROM `"+ m_NameTable_Ref_Criteres +"` ";
//    req        += "WHERE  `REF_LRCCRITERES_SEQ`='" + _ID + "';";
//    retour = "";

//    QSqlQuery sqlQuery (req , m_DataBase);
//    if (sqlQuery.isActive())
//    { if (sqlQuery.next())
//        { retour = sqlQuery.value(0).toString();
//        }
//    }
//    m_DataBase.close();
//    return true;

//}

//bool DrcDatabase::getArgumentaire_byID(const QString RC_ID , QString &retour)
//{
//    if (!connectDatabase(DB, __FILE__, __LINE__))
//        return false;
//    QString req = "SELECT `ARGUMENTAIRE` FROM `"+ m_NameTable_Ref_RC +"` WHERE `REF_RC_SEQ`='"+RC_ID+"';";
//    retour = "";

//    QSqlQuery sqlQuery (req , m_DataBase);
//    if (sqlQuery.isActive())
//    { if (sqlQuery.next())
//        { retour = sqlQuery.value(0).toString();
//        }
//    }
//    m_DataBase.close();
//    return true;
//}

///**
// * \brief Récupère la clé primaire de la base eDRC pour le patient depuis son GUID MedinTux. Le GUID doit être renseigné dans la base statique (DRC_Static_Data::m_MedinTux_GUIDPatient).
// * @return true si tout est OK, false cas contraire.
// */
//bool DrcDatabase::getPatientDRCPrimKey()
//{
//    if (!connectDatabase(DB, __FILE__, __LINE__))
//        return false;
//    bool retour = true;

//    QString req = "SELECT `ID_PATIENT` FROM `"+ m_NameTable_Patient +"` "
//            "WHERE  `GUID`='"+ DRC_Static_Data::m_MedinTux_GUIDPatient +"';";

//    QSqlQuery q (req , m_DataBase);
//    if (q.isActive())
//    { if (q.next())
//        {  DRC_Static_Data::m_PatientIdFromBD = q.value(0).toInt();
//        }
//        else
//        {  if (!createNewPatient()) retour = false;
//        }
//    }
//    else
//    {  DRC_Static_Data::m_PatientIdFromBD = -1;
//        retour = false;
//    }
//    m_DataBase.close();
//    return retour;
//}

//bool DrcDatabase::createNewPatient()
//{
//    if (!connectDatabase(DB, __FILE__, __LINE__))
//        return false;
//    bool retour = true;

//    QString req = "INSERT INTO `eDRC`.`"+ m_NameTable_Patient +"` (\n"
//            "`ID_PATIENT` ,\n"
//            "`GUID` ,\n"
//            "`DATE_NAISS` ,\n"
//            "`SEXE`\n"
//            ")\n"
//            "VALUES (\n"
//            "NULL , :GUID, :DDN, :SEXE\n"
//            ");\n";

//    QSqlQuery q(m_DataBase);
//    q.prepare(req);
//    q.bindValue(":GUID", DRC_Static_Data::m_MedinTux_GUIDPatient);
//    q.bindValue(":DDN", DRC_Static_Data::m_PatientDDN.toString("yyyy-MM-dd"));
//    q.bindValue(":SEXE", DRC_Static_Data::m_PatientSexe);
//    if (q.exec()) retour = true; else retour = false;

//    DRC_Static_Data::m_PatientIdFromBD = q.lastInsertId().toInt();
//    qWarning() << DRC_Static_Data::m_PatientIdFromBD;
//    return retour;
//}

//QList<int> DrcDatabase::getSelectedCriterias(const QString  &consult_rc_id)
//{
//    QList<int> crit_id;
//    if (!connectDatabase(DB, __FILE__, __LINE__)) return crit_id;
//    crit_id.clear();

//    QString req = "SELECT `REF_L_RC_CRITERES_FK` FROM `"+ m_NameTable_Consults_Criteres+"` "
//            "WHERE `ID_L_CONSULT_RC` = '" + consult_rc_id + "';";

//    QSqlQuery q (m_DataBase);
//    if (q.exec(req))
//    { while (q.next())
//        {  crit_id << q.value(0).toInt();
//        }
//    }
//    else
//    {  qWarning() << "DrcDatabase::getLinkedCriterias" << q.lastError().text();
//        return crit_id;
//    }

//    return crit_id;
//}

//QString DrcDatabase::getRCView(const QString  &consultID)
//{
//    if (!connectDatabase(DB, __FILE__, __LINE__))
//        return 0;

//    QString retour = "";
//    QString req = "SELECT `VIEW` FROM `"+ m_NameTable_Consults +"` "
//            "WHERE  `ID_CONSULT`='"+ consultID +"';";
//    QSqlQuery sqlQuery (req , m_DataBase);
//    if (sqlQuery.isActive())
//    { if (sqlQuery.next())
//        {  retour.append(sqlQuery.value(0).toString());
//        }
//    }
//    else
//    {  qWarning() << "DrcDatabase::getRCView" << sqlQuery.lastError().text();
//        return 0;
//    }

//    /*
//  // Récupère tous les RC de la consult
//  QList<int> crit_id;
//  crit_id = getSelectedCriterias(consultID);

//  foreach(int i, crit_id)
//  {  QString req2 = "SELECT `REF_L_RC_CRITERES_FK` FROM `CONSULTS_CRITERES` "
//                    "WHERE ID_L_CONSULT_RC = '" + QString::number(i) + "';";
//     QSqlQuery q (m_DataBase);
//     if (!m_DataBase.open()) return 0;
//     if (q.exec(req2))
//     {  while (q.next())
//        { QString lib;
//          getCritere_byID(q.value(0).toString() , lib);
//          retour.append(lib+"<br />");
//        }
//     }
//     else
//     { qWarning() << q.lastError().text() ; return 0; }
//       retour.append("<br /><br />");
//    }
//*/

//    // m_DataBase.close();
//    return retour;
//}


///**
// *
// * @param list
// * @return
// * @todo GESTION DES ERREURS
// */
//bool DrcDatabase::saveConsultToBase(QList<RC_Elements>  &list)
//{
//    if (!connectDatabase(DB, __FILE__, __LINE__))
//        return false;
//    bool retour = true;
//    QString req="";
//    QString tmp="";

//    // Créer une consultation
//    req = "INSERT INTO `eDRC`.`"+m_NameTable_Consults+"` ("
//            "`ID_CONSULT` , `ID_PATIENT` , `DATE` , `TRANSMIT` , `DATE_TRANS` , `ID_L_CONSULT_RC` ,"
//            "`ID_TUX_USER`) "
//            "VALUES (NULL , :ID_PATIENT, :DATE_C, '0', NULL , :ID_L_CONSULT_RC, :ID_TUX_USER"
//            ");";

//    QSqlQuery q (m_DataBase);
//    q.prepare(req);
//    q.bindValue(":ID_PATIENT" , DRC_Static_Data::m_PatientIdFromBD);
//    q.bindValue(":DATE_C" , DRC_Static_Data::m_ObservDate.toString("yyyy-MM-dd hh:mm:ss"));
//    q.bindValue(":ID_L_CONSULT_RC" , 1);
//    q.bindValue(":ID_TUX_USER" , DRC_Static_Data::m_MedinTux_Id_User);
//    if (!q.exec()) return false;
//    int id_consult = q.lastInsertId().toInt();

//    // Récupère l'Id de la consultation nouvellement créée

//    // Insère les RC_Elements dans la base
//    req = "INSERT INTO `eDRC`.`"+m_NameTable_Consults_RC+"` ("
//            "`ID_CONSULT_RC` , `ID_L_CONSULT_RC` , `REF_RC_FK`, `ALD` , `SYMPTO` , `DIAG` , `SUIVI` ,"
//            "`COMMENTAIRE`, `CIM10` )"
//            "VALUES (NULL , :ID_L_CONSULT_RC, :DRC_LINK, :ALD, :SYMPTO, :DIAG, :SUIVI, :COMMENTAIRE,"
//            ":CIM10);";

//    QString vi = "";
//    foreach(RC_Elements el, list)
//    {  QSqlQuery q (m_DataBase) ;
//        q.prepare(req);
//        q.bindValue(":ID_L_CONSULT_RC" , id_consult);
//        q.bindValue(":DRC_LINK" , el.getId());
//        q.bindValue(":ALD" , el.isALD_Value());
//        q.bindValue(":SYMPTO" , el.isSymptomatique_Value());
//        q.bindValue(":DIAG" , el.getSelectedPosDiag());
//        q.bindValue(":SUIVI" , el.getSelectedSuivi());
//        q.bindValue(":COMMENTAIRE" , el.getCommentaire());
//        q.bindValue(":CIM10" , el.getCIM10());
//        if (!q.exec()) qWarning() << q.lastError().text();
//        int  id_rc = q.lastInsertId().toInt();
//        vi.append(el.toFullHtml());

//        foreach (Criteres_Elements cel, el.getSelectedCriteria())
//        {  if (!cel.isSelected()) continue;
//            QString r = "INSERT INTO `eDRC`.`"+m_NameTable_Consults_Criteres+"` ("
//                    "`ID_CONSULT_CRITERE` ,`ID_L_CONSULT_RC` ,"
//                    "`REF_L_RC_CRITERES_FK` ,`COMMENTAIRE`)"
//                    "VALUES (NULL , :LINK_RC, :LINK_DRC, :COMM);";
//            QSqlQuery cq (m_DataBase);
//            cq.prepare (r);
//            cq.bindValue(":LINK_RC", id_rc);
//            cq.bindValue(":LINK_DRC", cel.getId());
//            cq.bindValue(":COMM", cel.getCommentaire());
//            if (!cq.exec()) qWarning() << cq.lastError().text();
//        }
//    } // Fin foreach

//    // Sauvegarder la vue de la Consultation dans la base
//    // Préparer le blob pour insertion correcte
//    vi.replace ("'", "\'");
//    req = "UPDATE `eDRC`.`"+m_NameTable_Consults+"` "
//            "SET `VIEW` = '" + vi + "' "
//            "WHERE `ID_CONSULT` = " + QString::number(id_consult) +
//            ";";

//    QSqlQuery v (m_DataBase);
//    if (!v.exec(req)) { qWarning() << v.lastError().text(); qWarning() << v.executedQuery();}

//    //   m_DataBase.close();
//    return retour;
//}

//void DrcDatabase::getConsultToListRC(QList<RC_Elements>  &list, const QString  &id_consult)
//{
//    if (!connectDatabase(DB, __FILE__, __LINE__))
//        return ;
//    QSqlQuery q (m_DataBase);
//    QString req = "";
//    list.clear();
//    QList<int> consult_rc_id;

//    // Récupère les RC de la consultation
//    req = "SELECT * FROM `"+ m_NameTable_Consults_RC +"` "
//            "WHERE  `ID_L_CONSULT_RC`='"+ id_consult +"';";

//    if (q.exec(req))
//    { while (q.next())
//        {  RC_Elements el;
//            getFullRCFrom_RC_ID(q.value(ConsultsRCTable_DRC_Link).toString(), el);
//            // Récupère toutes les valeurs validées par l'utilisateur
//            el.setALD(q.value(ConsultsRCTable_ALD).toBool());
//            el.setSymptomatique(q.value(ConsultsRCTable_Sympto).toBool());
//            el.setQVariant(q.value(ConsultsRCTable_CIM10) , RC_Elements::Col_CIM10);
//            el.setQVariant(q.value(ConsultsRCTable_Diag)  , RC_Elements::Col_Pos_Diag);
//            el.setQVariant(q.value(ConsultsRCTable_Suivi) , RC_Elements::Col_Pos_Suivi);
//            //         el.setQVariant(q.value(ConsultsRCTable_Commentaire).toString() , RC_Elements::Col_Commentaire);
//            list.append(el);
//            consult_rc_id << q.value(ConsultsRCTable_Id).toInt();
//        }
//    }
//    else
//    {  qWarning() << "DrcDatabase::getConsultToListRC" << q.lastError().text();
//        return ;
//    }

//    // Récupère les Critères sélectionnés pour tous les RC
//    int i=0;
//    foreach (int l_id, consult_rc_id)
//    {  QList<int> crit_id;
//        crit_id = getSelectedCriterias(QString::number(l_id));
//        // Injecte les dans le RC
//        foreach(int c_id, crit_id)
//        {  Criteres_Elements proto;
//            proto.setId(c_id);
//            if (list[i].m_Criteres.contains(proto))
//            {  int idInList = list[i].m_Criteres.indexOf(proto);
//                list[i].m_Criteres[idInList].setSelection(true);
//            }
//        } // Fin  foreach
//        i++; // Avance dans la liste des RC
//    }

//}



