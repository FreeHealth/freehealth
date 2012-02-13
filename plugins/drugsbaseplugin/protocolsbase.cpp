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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/

/**
  \class DrugsDB::Internal::ProtocolsBase
  This class manages all accesses to the protocol database. It also manages transmition to
  the central FreeMedForms server or recorded protocols.
*/

/**
  DB Relations Schema

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
#include "constants.h"
#include "versionupdater.h"

#include <utils/log.h>

using namespace DrugsDB;
using namespace Internal;

namespace DrugsDB {
namespace Internal {
class ProtocolsBasePrivate
{
public:
    ProtocolsBasePrivate() {}

    QString dosageCreateTableSqlQuery()
    {
        /** \todo add UserUuid field or lkid */
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

};
}
}


ProtocolsBase *ProtocolsBase::m_Instance;

ProtocolsBase *ProtocolsBase::instance(QObject *parent)
{
    if (!m_Instance)
        m_Instance = new ProtocolsBase(parent);
    return m_Instance;
}

ProtocolsBase::ProtocolsBase(QObject *parent) :
        QObject(parent), Utils::Database(), d(0)
{
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
}

ProtocolsBase::~ProtocolsBase()
{
    if (d)
        delete d;
    d = 0;
}


///** \brief Create the protocols database if it does not exists. */
//bool ProtocolsBase::createDatabase(const QString &connectionName , const QString &dbName,
//                               const QString &pathOrHostName,
//                               TypeOfAccess /*access*/, AvailableDrivers driver,
//                               const QString & login, const QString & pass,
//                               const int port,
//                               CreationOption /*createOption*/
//                              )
//{
//    /** \todo  ask user if he wants : 1. an empty dosage base ; 2. to retreive dosages from internet FMF website */
//    if (connectionName != Dosages::Constants::DB_DOSAGES_NAME)
//        return false;

//    LOG(tkTr(Trans::Constants::TRYING_TO_CREATE_1_PLACE_2).arg(dbName).arg(pathOrHostName));

//    // create an empty database and connect
//    QSqlDatabase DB;
//    if (driver == SQLite) {
//        DB = QSqlDatabase::addDatabase("QSQLITE" , connectionName);
//        if (!QDir(pathOrHostName).exists())
//            if (!QDir().mkpath(pathOrHostName))
//                tkTr(Trans::Constants::_1_ISNOT_AVAILABLE_CANNOTBE_CREATED).arg(pathOrHostName);
//        DB.setDatabaseName(QDir::cleanPath(pathOrHostName + QDir::separator() + dbName));
//        DB.open();
//        setDriver(Utils::Database::SQLite);
//    }
//    else if (driver == MySQL) {
//        DB = QSqlDatabase::database(connectionName);
//        if (!DB.open()) {
//            QSqlDatabase d = QSqlDatabase::addDatabase("QMYSQL", "CREATOR");
//            d.setHostName(pathOrHostName);
//            /** \todo retreive log/pass */
//            d.setUserName(login);
//            d.setPassword(pass);
//            d.setPort(port);
//            if (!d.open()) {
//                Utils::warningMessageBox(tr("Unable to create the Protocol database."),tr("Please contact dev team."));
//                return false;
//            }
//            QSqlQuery q(QString("CREATE DATABASE `%1`").arg(dbName), d);
//            if (!q.isActive()) {
//                LOG_QUERY_ERROR(q);
//                Utils::warningMessageBox(tr("Unable to create the Protocol database."),tr("Please contact dev team."));
//                return false;
//            }
//            DB.setDatabaseName(dbName);
//            if (!DB.open()) {
//                Utils::warningMessageBox(tr("Unable to create the Protocol database."),tr("Please contact dev team."));
//                return false;
//            }
//            DB.setDatabaseName(dbName);
//        }
//        if (QSqlDatabase::connectionNames().contains("CREATOR"))
//            QSqlDatabase::removeDatabase("CREATOR");
//        DB.open();
//        setDriver(Utils::Database::MySQL);
//    }

//    // create db structure
//    // before we need to inform Utils::Database of the connectionName to use
//    setConnectionName(connectionName);

//    // The SQL scheme MUST BE synchronized with the Dosages::Constants Model Enumerator !!!
//    /** \todo code here */
//    if (executeSQL(QStringList()
//        << dosageCreateTableSqlQuery().remove("AUTOINCREMENT")
//        << "CREATE TABLE IF NOT EXISTS `VERSION` ("
//           "`ACTUAL`                varchar(10)    NULL"
//           ");"
//        << QString("INSERT INTO `VERSION` (`ACTUAL`) VALUES('%1');").arg(VersionUpdater::instance()->lastDosageDabaseVersion())
//        , DB)) {
//        LOG(tkTr(Trans::Constants::DATABASE_1_CORRECTLY_CREATED).arg(dbName));
//        return true;
//    } else {
//        LOG_ERROR(tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2)
//                         .arg(dbName, DB.lastError().text()));
//    }
//    return false;
//}

///**
//  \brief Check the version of the doage database. Do the necessary updates for that database according to the application version number.
//  Added from freediams version 0.0.8 stable
//*/
//void ProtocolsBase::checkDosageDatabaseVersion()
//{
//    if (!DrugsDB::VersionUpdater::instance()->isDosageDatabaseUpToDate())
//        if (!DrugsDB::VersionUpdater::instance()->updateDosageDatabase())
//            LOG_ERROR(tr("Standardized protocols database can not be correctly updated"));
//}

///** \brief Returns hash that contains dosage uuid has key and the xml'd dosage to transmit as value */
//QHash<QString, QString> ProtocolsBase::getDosageToTransmit()
//{
//    QHash<QString, QString> toReturn;
//    QSqlDatabase DB = QSqlDatabase::database(Dosages::Constants::DB_DOSAGES_NAME);
//    if (!connectDatabase(DB, __FILE__, __LINE__))
//        return toReturn;

//    QString req = QString("SELECT * FROM `DOSAGE` WHERE (`TRANSMITTED` IS NULL);");
//    {
//        QSqlQuery query(req,DB);
//        if (query.isActive()) {
//            while (query.next()) {
//                QHash<QString,QString> toXml;
//                int i=0;
//                for (i=0;i<query.record().count();++i) {
//                    // create a XML of the dosage
//                    toXml.insert(query.record().field(i).name(), query.value(i).toString());
//                }
//                toReturn.insert(toXml.value("POSO_UUID"), Utils::createXml(Dosages::Constants::DB_DOSAGES_TABLE_NAME,toXml,4,false));
//            }
//        } else
//            LOG_QUERY_ERROR(query);
//    }

//    req = QString("SELECT * FROM `DOSAGE` WHERE (`TRANSMITTED`<`MODIFICATIONDATE`);");
//    {
//        QSqlQuery query(req,DB);
//        if (query.isActive()) {
//            while (query.next()) {
//                QHash<QString,QString> toXml;
//                int i=0;
//                for (i=0;i<query.record().count();++i) {
//                    // create a XML of the dosage
//                    toXml.insert(query.record().field(i).name(), query.value(i).toString());
//                }
//                toReturn.insert(toXml.value("POSO_UUID"), Utils::createXml(Dosages::Constants::DB_DOSAGES_TABLE_NAME,toXml,4,false));
//            }
//        } else {
//            LOG_QUERY_ERROR(query);
//        }
//    }
//    return toReturn;
//}

///** Marks all dosages as transmitted now. \e dosageUuids contains dosage uuid. */
//bool ProtocolsBase::markAllDosageTransmitted(const QStringList &dosageUuids)
//{
//    if (dosageUuids.count()==0)
//        return true;
//    QSqlDatabase DB = QSqlDatabase::database(Dosages::Constants::DB_DOSAGES_NAME);
//    if (!connectDatabase(DB, __FILE__, __LINE__))
//        return false;

//    QStringList reqs;
//    foreach(const QString &s, dosageUuids) {
//        QString req = QString("UPDATE `DOSAGE` SET `TRANSMITTED`='%1' WHERE %2")
//                      .arg(QDateTime::currentDateTime().toString(Qt::ISODate))
//                      .arg(QString("`POSO_UUID`='%1'").arg(s));
//        reqs << req;
//    }
//    if (!executeSQL(reqs,DB)) {
//        LOG_ERROR(tr("Unable to update the protocol's transmission date."));
//        return false;
//    }
//    return true;
//}

///**
//  \brief Returns all CIS that have a recorded dosage. Manages INN dosage type.
//  \todo put this in a thread...
//*/
//QList<QVariant> DrugsBase::getAllUIDThatHaveRecordedDosages() const
//{
//    QList<QVariant> toReturn;
//    /** \todo recode this */

//    if (!actualDatabaseInformations())
//        return toReturn;

//    QSqlDatabase DosageDB = QSqlDatabase::database(Dosages::Constants::DB_DOSAGES_NAME);
//    if (!connectDatabase(DosageDB, __FILE__, __LINE__))
//        return toReturn;

//    QString req;
//    if (m_IsDefaultDB) {
//        req = QString("SELECT DISTINCT `DRUG_UID_LK` FROM `DOSAGE` WHERE `DRUGS_DATABASE_IDENTIFIANT` = \"%1\";")
//              .arg(Constants::DB_DEFAULT_IDENTIFIANT);
//    } else {
//        req = QString("SELECT DISTINCT `DRUG_UID_LK` FROM `DOSAGE` WHERE `DRUGS_DATABASE_IDENTIFIANT` = \"%1\";")
//              .arg(actualDatabaseInformations()->identifiant);
//    }
//    {
//        QSqlQuery query(req,DosageDB);
//        if (query.isActive()) {
//            while (query.next()) {
//                toReturn << query.value(0);
//            }
//        } else {
//            LOG_QUERY_ERROR(query);
//        }
//    }

//    // Get all CIS that contains INN that have available dosage
//    QMultiHash<int, QString> inn_dosageRef = getAllINNThatHaveRecordedDosages();

//    // get all code_subst from INNs
//    QHash<int, QString> where;
//    QString tmp;
//    QList<int> code_subst;
//    QSqlDatabase DrugsDB = QSqlDatabase::database(Constants::DB_DRUGS_NAME);
//    if (!connectDatabase(DrugsDB, __FILE__, __LINE__))
//        return toReturn;

//    // get all needed datas from database
//    QMultiHash<QString, minimalCompo> cis_compo;
//    foreach(int inn, inn_dosageRef.keys()) {
//        const QVector<int> &molIds = getLinkedMoleculeCodes(inn);
//        for(int i = 0; i < molIds.count(); ++i) {
//            int code = molIds.at(i);
//            if (!code_subst.contains(code))
//                code_subst << code;
//        }
//    }
//    foreach(int code, code_subst) {
//        tmp += QString::number(code) + ", ";
//    }
//    tmp.chop(2);

//    // Get all Drugs UID that contains the substance + dosage
//    where.clear();
//    req.clear();
//    where.insert(Constants::COMPO_MOL_CODE, QString("IN (%1)").arg(tmp));
//    req = select(Constants::Table_COMPO,
//                 QList<int>() << Constants::COMPO_UID
//                 << Constants::COMPO_MOL_CODE
//                 << Constants::COMPO_DOSAGE,
//                 where);
//    QSqlQuery query(req, DrugsDB);
//    if (query.isActive()) {
//        while (query.next()) {
//            QString cis = query.value(0).toString();
//            minimalCompo compo;
//            compo.inn = getLinkedAtcIds(query.value(1).toInt()).at(0);
//            compo.dosage = query.value(2).toString();
//            cis_compo.insertMulti(cis, compo);
//        }
//    } else {
//        LOG_QUERY_ERROR(query);
//    }

//    // now check every drugs
//    foreach(const QString &uid, cis_compo.uniqueKeys()) {
//        QList<int> innsOfThisDrug;
//        foreach(const minimalCompo &compo, cis_compo.values(uid)) {
//            if (!innsOfThisDrug.contains(compo.inn))
//                innsOfThisDrug << compo.inn;
//            QString d = compo.dosage;
//            foreach(const QString &r, inn_dosageRef.values(compo.inn)) {
//                // remove unneeded strings
//                if (d == r)
//                    toReturn << uid;
//                else if (d.remove(",000") == r)
//                    toReturn << uid;
//                else if (d.remove(",00") == r)
//                    toReturn << uid;

//                // try unit conversion
//                if (d.replace("000 mg", " g") == r) {
//                    toReturn << uid;
//                }
//                QString t = r;
//                if (t.replace("000 mg", " g") == d) {
//                    toReturn << uid;
//                }
//            }
//        }
//        if (innsOfThisDrug.count() > 1)
//            toReturn.removeAll(uid);
//    }
//    return toReturn;
//}

//QMultiHash<int,QString> DrugsBase::getAllINNThatHaveRecordedDosages() const
//{
//    QMultiHash<int,QString> toReturn;
//    QSqlDatabase DB = QSqlDatabase::database(Dosages::Constants::DB_DOSAGES_NAME);
//    if (!connectDatabase(DB, __FILE__, __LINE__))
//        return toReturn;

//    QString req;
//    if (m_IsDefaultDB) {
//        req = QString("SELECT DISTINCT `INN_LK`, `INN_DOSAGE` FROM `DOSAGE` WHERE `DRUGS_DATABASE_IDENTIFIANT` = \"%1\";")
//              .arg(Constants::DB_DEFAULT_IDENTIFIANT);
//    } else {
//        req = QString("SELECT DISTINCT `INN_LK`, `INN_DOSAGE` FROM `DOSAGE` WHERE `DRUGS_DATABASE_IDENTIFIANT` = \"%1\";")
//              .arg(actualDatabaseInformations()->identifiant);
//    }
//    QSqlQuery query(req,DB);
//    if (query.isActive()) {
//        while (query.next()) {
//            toReturn.insertMulti(query.value(0).toInt(), query.value(1).toString());
//        }
//    } else {
//        LOG_QUERY_ERROR(query);
//    }
//    return toReturn;
//}
