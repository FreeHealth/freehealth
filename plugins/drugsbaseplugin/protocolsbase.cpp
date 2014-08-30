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

/**
  \class DrugsDB::ProtocolsBase
  This class manages all accesses to the protocol database. It also manages transmition to
  the central FreeMedForms server or recorded protocols.

  DB Relations Schema\n
  \n
                    O O                                 LABEL_LINK (MASTER_LID)
                    ._.                                      |1
                     |                                       |
 PROTO_PREC --- PROTO_MASTER ----- PROTO_INFO                |N
            N 1      |1      1   N                        LABELS (LID)
                     |
                     |N
                  PROTO_REL
                     |1
                     |
                     |1
                 PROTO_DIST
                 |1       |1
                 |        |
                 |N       |N
        PROTO_DRUG        PROTO_ATC


**/

#include "protocolsbase.h"
#include "drugbasecore.h"
#include "drugsbase.h"
#include "drugsbaseinfo.h"
#include "constants.h"
#include "constants_databaseschema.h"
#include "versionupdater.h"

#include <utils/global.h>
#include <utils/log.h>
#include <utils/databaseconnector.h>
#include <translationutils/constants.h>
#include <translationutils/trans_database.h>
#include <translationutils/trans_drugs.h>
#include <translationutils/trans_msgerror.h>
#include <translationutils/trans_current.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_tokensandsettings.h>

#include <QDir>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>

using namespace DrugsDB;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline DrugsDB::DrugBaseCore &core() {return DrugsDB::DrugBaseCore::instance();}
static inline DrugsDB::VersionUpdater &versionUpdater() {return core().versionUpdater();}
static inline DrugsDB::DrugsBase &drugsBase() {return core().drugsBase();}

namespace {
static inline bool connectDatabase(QSqlDatabase &DB, const QString &file, const int line)
{
    if (!DB.isOpen()) {
        if (!DB.open()) {
            Utils::Log::addError("ProtocolsBase", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                 .arg(DB.connectionName()).arg(DB.lastError().text()),
                                 file, line);
            return false;
        }
    }
    return true;
}

struct minimalCompo {
    bool isLike(const int _inn, const QString &_dosage) const {
        return (inn == _inn) && (dosage == _dosage);
    }
    int inn;
    QString dosage;
};
}  // namespace anonymous

namespace DrugsDB {
namespace Internal {
class ProtocolsBasePrivate
{
public:
    ProtocolsBasePrivate() :
        m_initialized(false)
    {}

public:
    bool m_initialized;
};
}  // namespace DrugsDB
}  // namespace Internal

ProtocolsBase::ProtocolsBase(QObject *parent) :
    QObject(parent), Utils::Database(),
    d(new ProtocolsBasePrivate)
{
    setObjectName("ProtocolsBase");
//    addTable(Table_PROTOMASTER,   "PROTO_MASTER");
//    addTable(Table_PROTORELAT,    "PROTO_REL");
//    addTable(Table_PROTODISTRIB,  "PROTO_DIST");
//    addTable(Table_PROTOINFO,     "PROTO_INFO");
//    addTable(Table_PROTODRUG,     "PROTO_DRUG");
//    addTable(Table_PROTOATC,      "PROTO_ATC");
//    addTable(Table_PROTOPREC,     "PROTO_PREC");

//    addField(Table_PROTOMASTER, PROTOMASTER_ID, "PID", FieldIsUniquePrimaryKey);
//    addField(Table_PROTOMASTER, PROTOMASTER_LABEL, "LABEL", FieldIsShortText);
//    addField(Table_PROTOMASTER, PROTOMASTER_INTAKEFROM, "INTAKEFROM", FieldIsReal );
//    addField(Table_PROTOMASTER, PROTOMASTER_INTAKETO, "INTAKETO", FieldIsReal);
//    addField(Table_PROTOMASTER, PROTOMASTER_INTAKEFROMTO, "INTAKEFROMTO", FieldIsBoolean);
//    addField(Table_PROTOMASTER, PROTOMASTER_INTAKESCHEME, "INTAKESCHEME", FieldIsShortText);
//    addField(Table_PROTOMASTER, PROTOMASTER_INTAKEINTERVALOFTIME, "INTAKE_IT", FieldIsInteger);
//    addField(Table_PROTOMASTER, PROTOMASTER_INTAKESINTERVALSCHEME, "INTAKE_ITSCHEME", FieldIsShortText);
//    addField(Table_PROTOMASTER, PROTOMASTER_ROUTEID, "RID", FieldIsInteger);
//    addField(Table_PROTOMASTER, PROTOMASTER_DURATIONFROM, "DURATIONFROM", FieldIsReal);
//    addField(Table_PROTOMASTER, PROTOMASTER_DURATIONTO, "DURATIONTO", FieldIsReal);
//    addField(Table_PROTOMASTER, PROTOMASTER_DURATIONFROMTO, "DURATIONFROMTO", FieldIsBoolean);
//    addField(Table_PROTOMASTER, PROTOMASTER_DURATIONSCHEME, "DURATIONSCHEME", FieldIs);
//    addField(Table_PROTOMASTER, PROTOMASTER_PERIOD, "PERIOD", FieldIsInteger);
//    addField(Table_PROTOMASTER, PROTOMASTER_PERIODSCHEME, "PERIODSCHEME", FieldIs);
//    addField(Table_PROTOMASTER, PROTOMASTER_ADMINSCHEME, "ADMINSCHEME", FieldIs);
//    addField(Table_PROTOMASTER, PROTOMASTER_DAILYSCHEME, "DAILYSCHEME", FieldIs);
//    addField(Table_PROTOMASTER, PROTOMASTER_MEALSCHEME, "MEALSCHEME", FieldIs);
//    addField(Table_PROTOMASTER, PROTOMASTER_ISALD, "ISALD", FieldIsBoolean);
//    addField(Table_PROTOMASTER, PROTOMASTER_TYPEOFTREATEMENT, "TYPEOFTREATEMENT", FieldIs);
//    addField(Table_PROTOMASTER, PROTOMASTER_NOTE, "NOTE", FieldIsLongText);

//    addField(Table_PROTORELAT, PROTORELAT_, "", FieldIs);

//    addField(Table_PROTODISTRIB, PROTODISTRIB_, "", FieldIs);

//    addField(Table_PROTOINFO, PROTOINFO_, "", FieldIs);
//    "`USERVALIDATOR`         varchar(200)   NULL,"
//    "`CREATIONDATE`          date           NULL,"    // put NOT NULL
//    "`MODIFICATIONDATE`      date           NULL,"
//    "`TRANSMITTED`           date           NULL,"

//    addField(Table_PROTODRUG, PROTODRUG_, "", FieldIs);
//    "`DRUGS_DATABASE_IDENTIFIANT` varchar(200) NULL,   "
//    "`DRUG_UID1`           varchar(20)    NULL,";
//    "`DRUG_UID2`           varchar(20)    NULL,";
//    "`DRUG_UID3`           varchar(20)    NULL,";
//    "`DRUG_OLDUID`         varchar(20)    NULL,";

//    addField(Table_PROTOATC, PROTOATC_, "", FieldIs);
//    "`ATC_CODE`              varchar(7)     ,";
//    "`STRENGTH`            varchar(100)   NULL,";
//    "`STRENGTH_UNIT`            varchar(100)   NULL,";

//    addField(Table_PROTOPREC, PROTOPREC_, "", FieldIs);
//    "`MINAGE`                int(10)        NULL,"
//    "`MAXAGE`                int(10)        NULL,"
//    "`MINAGEREFERENCE`       int(10)        NULL,"
//    "`MAXAGEREFERENCE`       int(10)        NULL,"
//    "`MINWEIGHT`             int(10)        NULL,"
//    "`SEXLIMIT`              int(10)        NULL,"
//    "`MINCLEARANCE`          int(10)        NULL,"
//    "`MAXCLEARANCE`          int(10)        NULL,"
//    "`PREGNANCYLIMITS`       int(10)        NULL,"
//    "`BREASTFEEDINGLIMITS`   int(10)        NULL,"



//    "`CIM10_LK`              varchar(150)   NULL,"
//    "`CIM10_LIMITS_LK`       varchar(150)   NULL,"
//    "`EDRC_LK`               varchar(150)   NULL,"

//    "`EXTRAS`                blob           NULL,"
//    "`ORDER`                 int(10)        NULL"


    // Connect first run database creation requested
    connect(Core::ICore::instance(), SIGNAL(firstRunDatabaseCreation()), this, SLOT(onCoreFirstRunCreationRequested()));
}

ProtocolsBase::~ProtocolsBase()
{
    if (d)
        delete d;
    d = 0;
}

bool ProtocolsBase::isInitialized() const
{
    return d->m_initialized;
}

void ProtocolsBase::onCoreFirstRunCreationRequested()
{
    disconnect(Core::ICore::instance(), SIGNAL(firstRunDatabaseCreation()), this, SLOT(onCoreFirstRunCreationRequested()));
    initialize();
}

QString ProtocolsBase::dosageCreateTableSqlQuery()
{
    // TODO: add UserUuid field or lkid
    return "CREATE TABLE IF NOT EXISTS `DOSAGE` ("
           "`POSO_ID`               INTEGER        PRIMARY KEY AUTOINCREMENT,"
           "`POSO_UUID`             varchar(40)    NULL,"    // put NOT NULL
           "`DRUGS_DATABASE_IDENTIFIANT` varchar(200) NULL,   "
           "`INN_LK`                int(11)        DEFAULT -1,"
           "`INN_DOSAGE`            varchar(100)   NULL,"    // contains the dosage of the SA INN
           "`DRUG_UID_LK`           varchar(20)    NULL,"
           "`CIP_LK`                int(11)        DEFAULT -1,"
           "`LABEL`                 varchar(300)   NULL,"    // put NOT NULL

           "`INTAKEFROM`            double         NULL,"    // put NOT NULL
           "`INTAKETO`              double         NULL,"
           "`INTAKEFROMTO`          bool           NULL,"
           "`INTAKESCHEME`          varchar(200)   NULL,"    // put NOT NULL
           "`INTAKESINTERVALOFTIME` int(10)        NULL,"
           "`INTAKESINTERVALSCHEME` varchar(200)   NULL,"
           "`ROUTE_ID`              integer        NULL,"

           "`DURATIONFROM`          double         NULL,"    // put NOT NULL
           "`DURATIONTO`            double         NULL,"
           "`DURATIONFROMTO`        bool           NULL,"
           "`DURATIONSCHEME`        varchar(200)   NULL,"    // put NOT NULL

           "`PERIOD`                int(10)        NULL,"    // put NOT NULL
           "`PERIODSCHEME`          varchar(200)   NULL,"    // put NOT NULL
           "`ADMINCHEME`            varchar(100)   NULL,"    // put NOT NULL
           "`DAILYSCHEME`           varchar(250)   NULL,"
           "`MEALSCHEME`            int(10)        NULL,"
           "`ISALD`                 bool           NULL,"
           "`TYPEOFTREATEMENT`      int(10)        NULL,"

           "`MINAGE`                int(10)        NULL,"
           "`MAXAGE`                int(10)        NULL,"
           "`MINAGEREFERENCE`       int(10)        NULL,"
           "`MAXAGEREFERENCE`       int(10)        NULL,"
           "`MINWEIGHT`             int(10)        NULL,"
           "`SEXLIMIT`              int(10)        NULL,"
           "`MINCLEARANCE`          int(10)        NULL,"
           "`MAXCLEARANCE`          int(10)        NULL,"
           "`PREGNANCYLIMITS`       int(10)        NULL,"
           "`BREASTFEEDINGLIMITS`   int(10)        NULL,"
           "`PHYSIOLOGICALLIMITS`   int(10)        NULL,"  // Is this really needed ?

           "`NOTE`                  varchar(500)   NULL,"

           "`CIM10_LK`              varchar(150)   NULL,"
           "`CIM10_LIMITS_LK`       varchar(150)   NULL,"
           "`EDRC_LK`               varchar(150)   NULL,"

           "`EXTRAS`                blob           NULL,"
           "`USERVALIDATOR`         varchar(200)   NULL,"
           "`CREATIONDATE`          date           NULL,"    // put NOT NULL
           "`MODIFICATIONDATE`      date           NULL,"
           "`TRANSMITTED`           date           NULL,"
           "`ORDER`                 int(10)        NULL"
           ");";
}

void ProtocolsBase::forceReinitialization()
{
    d->m_initialized = false;
}

/** \brief Initializer for the database. Return the error state. */
bool ProtocolsBase::initialize()
{
    // only one base can be initialized
    if (d->m_initialized)
        return true;

    // Connect and check Dosage Database
    // Check settings --> SQLite or MySQL ?
    // remove drugs database connection if exists
    if (QSqlDatabase::connectionNames().contains(Dosages::Constants::DB_DOSAGES_NAME)) {
        QSqlDatabase::removeDatabase(Dosages::Constants::DB_DOSAGES_NAME);
    }

    // create dosage database connection
    createConnection(Dosages::Constants::DB_DOSAGES_NAME, Dosages::Constants::DB_DOSAGES_NAME,
                     settings()->databaseConnector(),
                     Utils::Database::CreateDatabase);

    QSqlDatabase dosageDb = QSqlDatabase::database(Dosages::Constants::DB_DOSAGES_NAME);
    if (!dosageDb.isOpen()) {
        if (!dosageDb.open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Dosages::Constants::DB_DOSAGES_NAME).arg(dosageDb.lastError().text()));
        } else {
            LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(dosageDb.databaseName()).arg(dosageDb.driverName()));
        }
    } else {
        LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(dosageDb.databaseName()).arg(dosageDb.driverName()));
    }

    checkDosageDatabaseVersion();

    // Core signals are connected into the drugbasecore
    // Initialize
    d->m_initialized = true;
    return true;
}

/** \brief Create the protocols database if it does not exist. */
bool ProtocolsBase::createDatabase(const QString &connectionName , const QString &dbName,
                               const QString &pathOrHostName,
                               TypeOfAccess /*access*/, AvailableDrivers driver,
                               const QString & login, const QString & pass,
                               const int port,
                               CreationOption /*createOption*/
                              )
{
    // TODO:  ask user if he wants : 1. an empty dosage base ; 2. to retrieve dosages from internet FMF website
    if (connectionName != Dosages::Constants::DB_DOSAGES_NAME)
        return false;

    LOG(tkTr(Trans::Constants::TRYING_TO_CREATE_1_PLACE_2).arg(dbName).arg(pathOrHostName));

    // create an empty database and connect
    QSqlDatabase DB;
    if (driver == SQLite) {
        if (!Utils::checkDir(pathOrHostName, true, objectName())) {
            LOG_ERROR(tkTr(Trans::Constants::_1_ISNOT_AVAILABLE_CANNOTBE_CREATED).arg(pathOrHostName));
            return false;
        }
        DB = QSqlDatabase::addDatabase("QSQLITE" , connectionName);
        DB.setDatabaseName(QDir::cleanPath(pathOrHostName + QDir::separator() + dbName));
        DB.open();
        setDriver(Utils::Database::SQLite);
    }
    else if (driver == MySQL) {
        DB = QSqlDatabase::database(connectionName);
        if (!DB.open()) {
            QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", "__DRUGS__PROTOCOLS__CREATOR");
            db.setHostName(pathOrHostName);
            // TODO: retrieve log/pass
            db.setUserName(login);
            db.setPassword(pass);
            db.setPort(port);
            if (!db.open()) {
                Utils::warningMessageBox(tr("Unable to create the Protocol database."), tkTr(Trans::Constants::CONTACT_DEV_TEAM));
                return false;
            }
            QSqlQuery q(QString("CREATE DATABASE `%1`").arg(dbName), db);
            if (!q.isActive()) {
                LOG_QUERY_ERROR(q);
                Utils::warningMessageBox(tr("Unable to create the Protocol database."), tkTr(Trans::Constants::CONTACT_DEV_TEAM));
                return false;
            }
            DB.setDatabaseName(dbName);
            if (!DB.open()) {
                Utils::warningMessageBox(tr("Unable to create the Protocol database."), tkTr(Trans::Constants::CONTACT_DEV_TEAM));
                return false;
            }
            DB.setDatabaseName(dbName);
        }
        if (QSqlDatabase::connectionNames().contains("__DRUGS__PROTOCOLS__CREATOR"))
            QSqlDatabase::removeDatabase("__DRUGS__PROTOCOLS__CREATOR");
        DB.open();
        setDriver(Utils::Database::MySQL);
    }

    // create db structure
    // before we need to inform Utils::Database of the connectionName to use
    setConnectionName(connectionName);

    // The SQL scheme MUST BE synchronized with the Dosages::Constants Model Enumerator !!!
    // TODO: code here
    if (executeSQL(QStringList()
        << dosageCreateTableSqlQuery().remove("AUTOINCREMENT")
        << "CREATE TABLE IF NOT EXISTS `VERSION` ("
           "`ACTUAL`                varchar(10)    NULL"
           ");"
        << QString("INSERT INTO `VERSION` (`ACTUAL`) VALUES('%1');").arg(versionUpdater().lastDosageDabaseVersion())
        , DB)) {
        LOG(tkTr(Trans::Constants::DATABASE_1_CORRECTLY_CREATED).arg(dbName));
        return true;
    } else {
        LOG_ERROR(tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2)
                         .arg(dbName, DB.lastError().text()));
    }
    return false;
}

/**
  \brief Check the version of the doage database. Do the necessary updates for that database according to the application version number.
  Added from freediams version 0.0.8 stable
*/
void ProtocolsBase::checkDosageDatabaseVersion()
{
    if (!versionUpdater().isDosageDatabaseUpToDate())
        if (!versionUpdater().updateDosageDatabase())
            LOG_ERROR(tr("Standardized protocols database can not be updated successfully"));
}

/** \brief Returns hash that contains dosage uuid has key and the xml'd dosage to transmit as value */
QHash<QString, QString> ProtocolsBase::getDosageToTransmit()
{
    QHash<QString, QString> toReturn;
    QSqlDatabase DB = database();
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return toReturn;

    QString req = QString("SELECT * FROM `DOSAGE` WHERE (`TRANSMITTED` IS NULL);");
    {
        QSqlQuery query(req,DB);
        if (query.isActive()) {
            while (query.next()) {
                QHash<QString,QString> toXml;
                int i=0;
                for (i=0;i<query.record().count();++i) {
                    // create a XML of the dosage
                    toXml.insert(query.record().field(i).name(), query.value(i).toString());
                }
                toReturn.insert(toXml.value("POSO_UUID"), Utils::createXml(Dosages::Constants::DB_DOSAGES_TABLE_NAME,toXml,4,false));
            }
        } else
            LOG_QUERY_ERROR(query);
    }

    req = QString("SELECT * FROM `DOSAGE` WHERE (`TRANSMITTED`<`MODIFICATIONDATE`);");
    {
        QSqlQuery query(req,DB);
        if (query.isActive()) {
            while (query.next()) {
                QHash<QString,QString> toXml;
                int i=0;
                for (i=0;i<query.record().count();++i) {
                    // create a XML of the dosage
                    toXml.insert(query.record().field(i).name(), query.value(i).toString());
                }
                toReturn.insert(toXml.value("POSO_UUID"), Utils::createXml(Dosages::Constants::DB_DOSAGES_TABLE_NAME,toXml,4,false));
            }
        } else {
            LOG_QUERY_ERROR(query);
        }
    }
    return toReturn;
}

/** Marks all dosages as transmitted now. \e dosageUuids contains dosage uuid. */
bool ProtocolsBase::markAllDosageTransmitted(const QStringList &dosageUuids)
{
    if (dosageUuids.count()==0)
        return true;
    QSqlDatabase DB = database();
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return false;

    QStringList reqs;
    foreach(const QString &s, dosageUuids) {
        QString req = QString("UPDATE `DOSAGE` SET `TRANSMITTED`='%1' WHERE %2")
                      .arg(QDateTime::currentDateTime().toString(Qt::ISODate))
                      .arg(QString("`POSO_UUID`='%1'").arg(s));
        reqs << req;
    }
    if (!executeSQL(reqs,DB)) {
        LOG_ERROR(tr("Unable to update the protocol's transmission date."));
        return false;
    }
    return true;
}

/**
  \brief Returns all drug DID that have a recorded dosage. Manages INN dosage type.
  \todo put this in a thread...
*/
QList<QVariant> ProtocolsBase::getAllUIDThatHaveRecordedDosages() const
{
    QList<QVariant> toReturn;

    if (!drugsBase().actualDatabaseInformation())
        return toReturn;

    QSqlDatabase DosageDB = QSqlDatabase::database(Dosages::Constants::DB_DOSAGES_NAME);
    if (!connectDatabase(DosageDB, __FILE__, __LINE__))
        return toReturn;

    QString req;
    req = QString("SELECT DISTINCT `DRUG_UID_LK` FROM `DOSAGE` WHERE `DRUGS_DATABASE_IDENTIFIANT` = \"%1\";")
            .arg(drugsBase().actualDatabaseInformation()->identifier);

    QSqlQuery query(req, DosageDB);
    if (query.isActive()) {
        while (query.next()) {
            toReturn << query.value(0);
        }
    } else {
        LOG_QUERY_ERROR(query);
    }
    query.finish();

    // Get all drugs that contains INN that have available dosage
    QMultiHash<int, QString> inn_dosageRef = getAllINNThatHaveRecordedDosages();

    // get all code_subst from INNs
    QHash<int, QString> where;
    QString tmp;
    QList<int> code_subst;
    QSqlDatabase DrugsDB = QSqlDatabase::database(Constants::DB_DRUGS_NAME);
    if (!connectDatabase(DrugsDB, __FILE__, __LINE__))
        return toReturn;

    // get all needed data from database
    foreach(int inn, inn_dosageRef.keys()) {
        const QVector<int> &molIds = drugsBase().getLinkedMoleculeCodes(inn);
        for(int i = 0; i < molIds.count(); ++i) {
            int code = molIds.at(i);
            if (!code_subst.contains(code))
                code_subst << code;
        }
    }
    foreach(int code, code_subst) {
        tmp += QString::number(code) + ", ";
    }
    tmp.chop(2);

    // Get all Drugs UID that contains the substance + dosage
    QMultiHash<QString, minimalCompo> did_compo;
    where.clear();
    req.clear();
    where.insert(Constants::COMPO_MID, QString("IN (%1)").arg(tmp));
    req = drugsBase().select(Constants::Table_COMPO,
                             QList<int>() << Constants::COMPO_DID
                             << Constants::COMPO_MID
                             << Constants::COMPO_STRENGTH
                             << Constants::COMPO_DOSE_REF,
                             where);
    QSqlQuery drugsQuery(DrugsDB);
    if (drugsQuery.exec(req)) {
        while (drugsQuery.next()) {
            QString did = drugsQuery.value(0).toString();
            minimalCompo compo;
            compo.inn = drugsBase().getLinkedAtcIds(drugsQuery.value(1).toInt()).at(0);
            compo.dosage = drugsQuery.value(2).toString() + "/" + drugsQuery.value(3).toString();
            did_compo.insertMulti(did, compo);
        }
    } else {
        LOG_QUERY_ERROR(drugsQuery);
    }
    drugsQuery.finish();
    // now check every drugs
    foreach(const QString &uid, did_compo.uniqueKeys()) {
        QList<int> innsOfThisDrug;
        foreach(const minimalCompo &compo, did_compo.values(uid)) {
            if (!innsOfThisDrug.contains(compo.inn))
                innsOfThisDrug << compo.inn;
            QString d = compo.dosage;
            foreach(const QString &r, inn_dosageRef.values(compo.inn)) {
                // remove unneeded strings
                if (d == r)
                    toReturn << uid;
                else if (d.remove(",000") == r)
                    toReturn << uid;
                else if (d.remove(",00") == r)
                    toReturn << uid;

                // try unit conversion
                if (d.replace("000 mg", " g") == r) {
                    toReturn << uid;
                }
                QString t = r;
                if (t.replace("000 mg", " g") == d) {
                    toReturn << uid;
                }
            }
        }
        if (innsOfThisDrug.count() > 1)
            toReturn.removeAll(uid);
    }
    return toReturn;
}

/** Return a multihash containing as key the Inn code, as value the recorded dosage. */
QMultiHash<int,QString> ProtocolsBase::getAllINNThatHaveRecordedDosages() const
{
    QMultiHash<int,QString> toReturn;
    QSqlDatabase DB = QSqlDatabase::database(Dosages::Constants::DB_DOSAGES_NAME);
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return toReturn;

    QString req;
    req = QString("SELECT DISTINCT `INN_LK`, `INN_DOSAGE` FROM `DOSAGE` WHERE `DRUGS_DATABASE_IDENTIFIANT` = \"%1\";")
            .arg(drugsBase().actualDatabaseInformation()->identifier);
    QSqlQuery query(req,DB);
    if (query.isActive()) {
        while (query.next()) {
            toReturn.insertMulti(query.value(0).toInt(), query.value(1).toString());
        }
    } else {
        LOG_QUERY_ERROR(query);
    }
    return toReturn;
}

bool ProtocolsBase::onCoreDatabaseServerChanged()
{
    d->m_initialized = false;
//    d->m_RefreshDosageBase = true;
    Q_EMIT protocolsBaseIsAboutToChange();
    bool r = initialize();
    if (r)
        Q_EMIT protocolsBaseHasChanged();
    return r;
}

