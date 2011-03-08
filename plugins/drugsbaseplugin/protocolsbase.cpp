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
class ProtocolsBasePrivate{
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
