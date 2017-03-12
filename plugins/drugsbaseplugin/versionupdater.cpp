/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/**
  \class DrugsDB::DatabaseUpdater
  \brief This class manages drugs and dosages databases updates from versions to versions
*/

/**
  \class DrugsIOUpdateStep
  \brief Interface to use for the update process of prescription files.
  Inform the versions, then you can update two ways :
  - after DrugsIO read the XML file and had the model informed
  - or before this step (the update works then on the XML content)
  These process are called in the same order each time :
  - First : tries to update the XML content (if updateFromXml() returns true)
  - Second : tries to update from the model (if updateFromModel() returns true)
  - If one of these process updates the prescription, the version is set to the toVersion().
  - You can not update either from the XML content and from the Model.
  \sa DrugsDb::DrugsIO
*/

#include "versionupdater.h"
#include "constants.h"

#include <drugsbaseplugin/drugbasecore.h>
#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/protocolsbase.h>
#include <drugsbaseplugin/drugsmodel.h>
#include <drugsbaseplugin/dailyschememodel.h>

#include <utils/global.h>
#include <utils/log.h>

#include <translationutils/constanttranslations.h>

#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>
#include <QMap>
#include <QVariant>

using namespace DrugsDB;
using namespace Trans::ConstantTranslations;

static inline DrugsDB::DrugBaseCore &core() {return DrugsDB::DrugBaseCore::instance();}
static inline DrugsDB::ProtocolsBase &protocolsBase() {return core().protocolsBase();}

///////////////////////////////////////////////////////////////////////
//////////////////////////// UPDATE STEPS /////////////////////////////
///////////////////////////////////////////////////////////////////////
namespace {

class Dosage_054_To_1 : public DrugsDB::DosageDatabaseUpdateStep
{
public:
    // From version 0.5.4 to version 1
    // MySQL only: in table DOSAGE, modify column POSO_ID to add AUTO_INCREMENT feature
    // This solves issue #80 "FMF EMR 0.9.6 Error saving 2nd protocol on MySQL"
    Dosage_054_To_1() : DrugsDB::DosageDatabaseUpdateStep() {}
    ~Dosage_054_To_1() {}

    QString userMessage() const                                                 
    {                                                                           
        return QApplication::translate("DatabaseUpdater", "Your dosage database needs to be "
                                       "updated from version 0.5.4 to version 1.\n"
                                       "This will be automatically done.");     
    }
    QString fromVersion() const { return "0.5.4"; }
    QString toVersion() const { return "1"; }
    void setConnectionName(const QString &name) { m_Name = name; }
    bool retrieveValuesToUpdate() const  {return true;}
    
    bool updateDatabaseScheme() const
    {
        QSqlDatabase db = QSqlDatabase::database(Dosages::Constants::DB_DOSAGES_NAME);
        if (!db.isOpen()) {                                                     
            if (!db.open()) {                                                   
                Utils::Log::addError("VersionUpdater", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                     .arg(db.connectionName()).arg(db.lastError().text()),
                                     __FILE__, __LINE__);                       
                return false;                                                   
            }                                                                   
        }
        QStringList req;
        if (db.driverName()=="QMYSQL") {                                 
            req << "ALTER TABLE DOSAGE MODIFY COLUMN `POSO_ID` INT NOT NULL AUTO_INCREMENT;";
        }
        // update SQLite DOSAGES DB version to 1 even though nothing has changed                                                                  
        req << "INSERT INTO `VERSION` (`ACTUAL`) VALUES('1');";           
                                                                                
        foreach(const QString &r, req) {                                        
            QSqlQuery q(r,db);                                                  
                if (q.isActive()) {                                                 
                    q.finish();                                                     
                } else {                                                            
                LOG_QUERY_ERROR_FOR("VersionUpdater", q);                       
                }                                                                      
        }                                                                       
        LOG_FOR("VersionUpdater", QString("Dosage Database SQL update done from %1 to %2").arg("0.5.4", "1"));
        return true;                                                            
    }            

    bool saveUpdatedValuesToDatabase() const {return true;}                     
                                                                                
private:                                                                        
    QString m_Name;                                                             
};

class Dosage_050_To_054 : public DrugsDB::DosageDatabaseUpdateStep
{
public:
    // From v 0.5.0 To v 0.5.4
    // - Renaming and redefining DRUG_UID_LK varchar(20) instead of CIS_LK integer
    Dosage_050_To_054() : DrugsDB::DosageDatabaseUpdateStep() {}
    ~Dosage_050_To_054() {}

    QString userMessage() const
    {
        return QApplication::translate("DatabaseUpdater", "Your dosage database needs to be "
                                       "updated from version 0.5.0 to version 0.5.4.\n"
                                       "This will be automatically done.");
    }

    QString fromVersion() const { return "0.5.0"; }
    QString toVersion() const { return "0.5.4"; }

    void setConnectionName(const QString &name) { m_Name = name; }

    bool retrieveValuesToUpdate() const  {return true;}

    bool updateDatabaseScheme() const
    {
        // these versions only use SQLite
        QSqlDatabase db = QSqlDatabase::database(Dosages::Constants::DB_DOSAGES_NAME);
        if (!db.isOpen()) {
            if (!db.open()) {
                Utils::Log::addError("VersionUpdater", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                     .arg(db.connectionName()).arg(db.lastError().text()),
                                     __FILE__, __LINE__);
                return false;
            }
        }
        QStringList req;
        if (db.driverName() == "QSQLITE") {
            req << "﻿ALTER TABLE `DOSAGE` RENAME TO `OLD_DOSAGE`;";
            req << protocolsBase().dosageCreateTableSqlQuery();
            req << QString("INSERT INTO `DOSAGE` (%1) SELECT %1 FROM `OLD_DOSAGE`;")
                    .arg("`POSO_ID`,"
                         "`POSO_UUID`,"
                         "`DRUGS_DATABASE_IDENTIFIANT`,"
                         "`INN_LK`,"
                         "`INN_DOSAGE`,"
                         "`DRUG_UID_LK`,"
                         "`CIP_LK`,"
                         "`LABEL`,"
                         "`INTAKEFROM`,"
                         "`INTAKETO`,"
                         "`INTAKEFROMTO`,"
                         "`INTAKESCHEME`,"
                         "`INTAKESINTERVALOFTIME`,"
                         "`INTAKESINTERVALSCHEME`,"
                         "`DURATIONFROM`,"
                         "`DURATIONTO`,"
                         "`DURATIONFROMTO`,"
                         "`DURATIONSCHEME`,"
                         "`PERIOD`,"
                         "`PERIODSCHEME`,"
                         "`ADMINCHEME`,"
                         "`DAILYSCHEME`,"
                         "`MEALSCHEME`,"
                         "`ISALD`,"
                         "`TYPEOFTREATEMENT`,"
                         "`MINAGE`,"
                         "`MAXAGE`,"
                         "`MINAGEREFERENCE`,"
                         "`MAXAGEREFERENCE`,"
                         "`MINWEIGHT`,"
                         "`SEXLIMIT`,"
                         "`MINCLEARANCE`,"
                         "`MAXCLEARANCE`,"
                         "`PREGNANCYLIMITS`,"
                         "`BREASTFEEDINGLIMITS`,"
                         "`PHYSIOLOGICALLIMITS`,"
                         "`NOTE`,"
                         "`CIM10_LK`,"
                         "`CIM10_LIMITS_LK`,"
                         "`EDRC_LK`,"
                         "`EXTRAS`,"
                         "`USERVALIDATOR`,"
                         "`CREATIONDATE`,"
                         "`MODIFICATIONDATE`,"
                         "`TRANSMITTED`,"
                         "`ORDER`");
            req << "DROP TABLE `OLD_DOSAGE`;";
        } else if (db.driverName()=="QMYSQL") {
            req << "ALTER TABLE `DOSAGE` ADD `ROUTE` integer AFTER `INTAKESINTERVALSCHEME`;";
        }
        req << "﻿DELETE FROM `VERSION`;";
        req << "INSERT INTO `VERSION` (`ACTUAL`) VALUES('0.5.4');";

        foreach(const QString &r, req) {
            QSqlQuery q(r,db);
            if (q.isActive()) {
                q.finish();
            } else {
                LOG_QUERY_ERROR_FOR("VersionUpdater", q);
            }
        }
        LOG_FOR("VersionUpdater", QString("Dosage Database SQL update done from %1 to %2").arg("0.4.0", "0.5.0"));
        return true;
    }

    bool saveUpdatedValuesToDatabase() const {return true;}

private:
    QString m_Name;
};

class Dosage_040_To_050 : public DrugsDB::DosageDatabaseUpdateStep
{
public:
    // From v 0.4.0 To v 0.5.0
    // - Renaming and redefining DRUG_UID_LK varchar(20) instead of CIS_LK integer
    Dosage_040_To_050() : DrugsDB::DosageDatabaseUpdateStep() {}
    ~Dosage_040_To_050() {}

    QString userMessage() const
    {
        return QApplication::translate("DatabaseUpdater", "Your dosage database needs to be "
                                       "updated from version 0.4.0 to version 0.5.0.\n"
                                       "This will be automatically done.");
    }

    QString fromVersion() const { return "0.4.0"; }
    QString toVersion() const { return "0.5.0"; }

    void setConnectionName(const QString &name) { m_Name = name; }

    bool retrieveValuesToUpdate() const  {return true;}

    bool updateDatabaseScheme() const
    {
        // these versions only use SQLite
        QSqlDatabase db = QSqlDatabase::database(Dosages::Constants::DB_DOSAGES_NAME);
        if (!db.isOpen()) {
            if (!db.open()) {
                LOG_ERROR_FOR("VersionUpdater", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                     .arg(db.connectionName()).arg(db.lastError().text()));
                return false;
            }
        }
        QStringList req;
        if (db.driverName() == "QSQLITE") {
            req << "﻿ALTER TABLE `DOSAGE` RENAME TO `OLD_DOSAGE`;";
            req << protocolsBase().dosageCreateTableSqlQuery();
            req << QString("INSERT INTO `DOSAGE` (%1, `DRUG_UID_LK`) SELECT %1, `CIS_LK` FROM `OLD_DOSAGE`;")
                    .arg("`POSO_ID`,"
                         "`POSO_UUID`,"
                         "`DRUGS_DATABASE_IDENTIFIANT`,"
                         "`INN_LK`,"
                         "`INN_DOSAGE`,"
                         "`CIP_LK`,"
                         "`LABEL`,"
                         "`INTAKEFROM`,"
                         "`INTAKETO`,"
                         "`INTAKEFROMTO`,"
                         "`INTAKESCHEME`,"
                         "`INTAKESINTERVALOFTIME`,"
                         "`INTAKESINTERVALSCHEME`,"
                         "`DURATIONFROM`,"
                         "`DURATIONTO`,"
                         "`DURATIONFROMTO`,"
                         "`DURATIONSCHEME`,"
                         "`PERIOD`,"
                         "`PERIODSCHEME`,"
                         "`ADMINCHEME`,"
                         "`DAILYSCHEME`,"
                         "`MEALSCHEME`,"
                         "`ISALD`,"
                         "`TYPEOFTREATEMENT`,"
                         "`MINAGE`,"
                         "`MAXAGE`,"
                         "`MINAGEREFERENCE`,"
                         "`MAXAGEREFERENCE`,"
                         "`MINWEIGHT`,"
                         "`SEXLIMIT`,"
                         "`MINCLEARANCE`,"
                         "`MAXCLEARANCE`,"
                         "`PREGNANCYLIMITS`,"
                         "`BREASTFEEDINGLIMITS`,"
                         "`PHYSIOLOGICALLIMITS`,"
                         "`NOTE`,"
                         "`CIM10_LK`,"
                         "`CIM10_LIMITS_LK`,"
                         "`EDRC_LK`,"
                         "`EXTRAS`,"
                         "`USERVALIDATOR`,"
                         "`CREATIONDATE`,"
                         "`MODIFICATIONDATE`,"
                         "`TRANSMITTED`,"
                         "`ORDER`");
            req << "DROP TABLE `OLD_DOSAGE`;";
        } else if (db.driverName()=="QMYSQL") {
            req << "ALTER TABLE `DOSAGE` ADD `DRUG_UID_LK` varchar(20) AFTER `INN_DOSAGE`;";
            req << "ALTER TABLE `DOSAGE` DROP COLUMN `CIS_LK`;";
        }
        req << "﻿DELETE FROM `VERSION`;";
        req << "INSERT INTO `VERSION` (`ACTUAL`) VALUES('0.5.0');";

        foreach(const QString &r, req) {
            QSqlQuery q(r,db);
            if (q.isActive()) {
                q.finish();
            } else {
                LOG_QUERY_ERROR_FOR("VersionUpdater", q);
            }
        }
        LOG_FOR("VersionUpdater", QString("Dosage Database SQL update done from %1 to %2").arg("0.4.0", "0.5.0"));
        return true;
    }

    bool saveUpdatedValuesToDatabase() const {return true;}

private:
    QString m_Name;
};

class Dosage_030_To_040 : public DrugsDB::DosageDatabaseUpdateStep
{
public:
    // From v 0.3.0 To v 0.4.0
    // - Adding DRUGS_DATABASE_FILENAME
    Dosage_030_To_040() : DrugsDB::DosageDatabaseUpdateStep() {}
    ~Dosage_030_To_040() {}

    QString userMessage() const
    {
        return QApplication::translate("DatabaseUpdater", "Your dosage database needs to be "
                                       "updated from version 0.2.0 to version 0.4.0.\n"
                                       "This will be automatically done.");
    }

    QString fromVersion() const { return "0.2.0"; }
    QString toVersion() const { return "0.4.0"; }

    void setConnectionName(const QString &name) { m_Name = name; }

    bool retrieveValuesToUpdate() const  {return true;}

    bool updateDatabaseScheme() const
    {
        // these versions only use SQLite
        QSqlDatabase db = QSqlDatabase::database(Dosages::Constants::DB_DOSAGES_NAME);
        if (!db.open()) {
            return false;
        }
        QStringList req;
        req << "﻿ALTER TABLE `DOSAGE` RENAME TO `OLD_DOSAGE`;";
        req << protocolsBase().dosageCreateTableSqlQuery();
        req << QString("INSERT INTO `DOSAGE` (%1) SELECT %1 FROM `OLD_DOSAGE`;")
                .arg("`POSO_ID`,"
                     "`POSO_UUID`,"
                     // "`DRUGS_DATABASE_IDENTIFIANT`,"
                     "`INN_LK`,"
                     "`INN_DOSAGE`,"
                     "`CIS_LK`,"
                     "`CIP_LK`,"
                     "`LABEL`,"
                     "`INTAKEFROM`,"
                     "`INTAKETO`,"
                     "`INTAKEFROMTO`,"
                     "`INTAKESCHEME`,"
                     "`INTAKESINTERVALOFTIME`,"
                     "`INTAKESINTERVALSCHEME`,"
                     "`DURATIONFROM`,"
                     "`DURATIONTO`,"
                     "`DURATIONFROMTO`,"
                     "`DURATIONSCHEME`,"
                     "`PERIOD`,"
                     "`PERIODSCHEME`,"
                     "`ADMINCHEME`,"
                     "`DAILYSCHEME`,"
                     "`MEALSCHEME`,"
                     "`ISALD`,"
                     "`TYPEOFTREATEMENT`,"
                     "`MINAGE`,"
                     "`MAXAGE`,"
                     "`MINAGEREFERENCE`,"
                     "`MAXAGEREFERENCE`,"
                     "`MINWEIGHT`,"
                     "`SEXLIMIT`,"
                     "`MINCLEARANCE`,"
                     "`MAXCLEARANCE`,"
                     "`PREGNANCYLIMITS`,"
                     "`BREASTFEEDINGLIMITS`,"
                     "`PHYSIOLOGICALLIMITS`,"
                     "`NOTE`,"
                     "`CIM10_LK`,"
                     "`CIM10_LIMITS_LK`,"
                     "`EDRC_LK`,"
                     "`EXTRAS`,"
                     "`USERVALIDATOR`,"
                     "`CREATIONDATE`,"
                     "`MODIFICATIONDATE`,"
                     "`TRANSMITTED`,"
                     "`ORDER`");
        req << QString("UPDATE `DOSAGE` SET `DRUGS_DATABASE_IDENTIFIANT`=\"%1\";").arg(DrugsDB::Constants::DB_DEFAULT_IDENTIFIANT);
        req << "DROP TABLE `OLD_DOSAGE`;";
        req << "﻿DELETE FROM `VERSION`;";
        req << "INSERT INTO `VERSION` (`ACTUAL`) VALUES('0.4.0');";
        foreach(const QString &r, req) {
            QSqlQuery q(r,db);
            if (q.isActive()) {
                q.finish();
            } else {
                Utils::Log::addQueryError("VersionUpdater", q, __FILE__, __LINE__);
            }
        }
        Utils::Log::addMessage("VersionUpdater",QString("Dosage Database SQL update done from %1 to %2").arg("0.2.0", "0.4.0"));
        return true;
    }

    bool saveUpdatedValuesToDatabase() const {return true;}

private:
    QString m_Name;
    mutable QMap<int, int> m_Id_DailySchemes;
    mutable QMap<int, int> m_Id_MealSchemes;
};

class Dosage_008_To_020 : public DrugsDB::DosageDatabaseUpdateStep
{
public:
    // From v 0.0.8 To v 0.2.0
    // - New Daily Scheme format (XML)
    // - Correcting MealTime Scheme (one more choice == 0 : '')
    Dosage_008_To_020() : DrugsDB::DosageDatabaseUpdateStep() {}
    ~Dosage_008_To_020() {}

    QString userMessage() const
    {
        return QApplication::translate("DatabaseUpdater", "Your dosage database needs to be "
                                       "updated from version 0.0.8 to version 0.2.0.\n"
                                       "This will be automatically done.\n"
                                       "Please verify your daily scheme prescriptions and relations to meal.");
    }

    QString fromVersion() const { return "0.0.8"; }
    QString toVersion() const { return "0.2.0"; }

    void setConnectionName(const QString &name) { m_Name = name; }

    bool retrieveValuesToUpdate() const
    {
        Q_ASSERT(!m_Name.isEmpty());
        QSqlDatabase db = QSqlDatabase::database(m_Name);
        if (!db.open()) {
            Utils::warningMessageBox(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(m_Name, db.lastError().text()),"","","");
            Utils::Log::addError("DatabaseUpdater", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(m_Name, db.lastError().text()),
                                 __FILE__, __LINE__);
            return false;
        }
        QString req = "SELECT `POSO_ID`, `DAILYSCHEME` FROM `DOSAGE` WHERE (`DAILYSCHEME`>0) ORDER BY `POSO_ID` ASC;";
        QSqlQuery q(req, db);
        if (q.isActive()) {
            while (q.next()) {
                m_Id_DailySchemes.insert(q.value(0).toInt(), q.value(1).toInt());
            }
        } else {
            Utils::Log::addQueryError("DatabaseUpdater", q, __FILE__, __LINE__);
            return false;
        }
        q.finish();
        req = "SELECT `POSO_ID`, `MEALSCHEME` FROM `DOSAGE` WHERE (`MEALSCHEME` IS NOT NULL) ORDER BY `POSO_ID` ASC;";
        q.exec(req);
        if (q.isActive()) {
            while (q.next()) {
                m_Id_MealSchemes.insert(q.value(0).toInt(), q.value(1).toInt());
            }
        } else {
            Utils::Log::addQueryError("DatabaseUpdater", q, __FILE__, __LINE__);
            return false;
        }
        return true;
    }

    bool updateDatabaseScheme() const
    {
        // these versions only use SQLite
        QSqlDatabase db = QSqlDatabase::database(Dosages::Constants::DB_DOSAGES_NAME);
        if (!db.open()) {
            return false;
        }
        QStringList req;
        req << "﻿ALTER TABLE `DOSAGE` RENAME TO `OLD_DOSAGE`;";
        req << protocolsBase().dosageCreateTableSqlQuery();
        req << QString("INSERT INTO `DOSAGE` (%1) SELECT %1 FROM `OLD_DOSAGE`;")
                      .arg("`POSO_ID`,"
                           "`POSO_UUID`,"
                           "`INN_LK`,"
                           "`INN_DOSAGE`,"
                           "`CIS_LK`,"
                           "`CIP_LK`,"
                           "`LABEL`,"
                           "`INTAKEFROM`,"
                           "`INTAKETO`,"
                           "`INTAKEFROMTO`,"
                           "`INTAKESCHEME`,"
                           "`INTAKESINTERVALOFTIME`,"
                           "`INTAKESINTERVALSCHEME`,"
                           "`DURATIONFROM`,"
                           "`DURATIONTO`,"
                           "`DURATIONFROMTO`,"
                           "`DURATIONSCHEME`,"
                           "`PERIOD`,"
                           "`PERIODSCHEME`,"
                           "`ADMINCHEME`,"
                           "`DAILYSCHEME`,"
                           "`MEALSCHEME`,"
                           "`ISALD`,"
                           "`TYPEOFTREATEMENT`,"
                           "`MINAGE`,"
                           "`MAXAGE`,"
                           "`MINAGEREFERENCE`,"
                           "`MAXAGEREFERENCE`,"
                           "`MINWEIGHT`,"
                           "`SEXLIMIT`,"
                           "`MINCLEARANCE`,"
                           "`MAXCLEARANCE`,"
                           "`PREGNANCYLIMITS`,"
                           "`BREASTFEEDINGLIMITS`,"
                           "`PHYSIOLOGICALLIMITS`,"
                           "`NOTE`,"
                           "`CIM10_LK`,"
                           "`CIM10_LIMITS_LK`,"
                           "`EDRC_LK`,"
                           "`EXTRAS`,"
                           "`USERVALIDATOR`,"
                           "`CREATIONDATE`,"
                           "`MODIFICATIONDATE`,"
                           "`TRANSMITTED`,"
                           "`ORDER`");
        req << "DROP TABLE `OLD_DOSAGE`;";
        req << "﻿DELETE FROM `VERSION`;";
        req << "INSERT INTO `VERSION` (`ACTUAL`) VALUES('0.2.0');";
        foreach(const QString &r, req) {
            QSqlQuery q(r,db);
            if (q.isActive()) {
                q.finish();
            } else {
                Utils::Log::addQueryError("VersionUpdater", q, __FILE__, __LINE__);
            }
        }
        Utils::Log::addMessage("VersionUpdater",QString("Dosage Database SQL update done from %1 to %2").arg("0.0.8", "0.2.0"));
        return true;
    }

    bool saveUpdatedValuesToDatabase() const
    {
        // these versions only use SQLite
        QSqlDatabase db = QSqlDatabase::database(Dosages::Constants::DB_DOSAGES_NAME);
        if (!db.open()) {
            return false;
        }
        QMap<int, QString> newDS;
        DrugsDB::DailySchemeModel *model = new DrugsDB::DailySchemeModel;
        model->setMethod(DrugsDB::DailySchemeModel::Repeat);
        foreach(int k, m_Id_DailySchemes.keys()) {
            int day = m_Id_DailySchemes.value(k);
            if (day & 0X00001) {
                model->setData(model->index(Trans::Constants::Time::DS_Morning, DrugsDB::DailySchemeModel::Value), Qt::Checked, Qt::CheckStateRole);
            }
            if (day & 0X00010) {
                model->setData(model->index(Trans::Constants::Time::DS_MidDay, DrugsDB::DailySchemeModel::Value), Qt::Checked, Qt::CheckStateRole);
            }
            if (day & 0X00100) {
                model->setData(model->index(Trans::Constants::Time::DS_TeaTime, DrugsDB::DailySchemeModel::Value), Qt::Checked, Qt::CheckStateRole);
            }
            if (day & 0X01000) {
                model->setData(model->index(Trans::Constants::Time::DS_Evening, DrugsDB::DailySchemeModel::Value), Qt::Checked, Qt::CheckStateRole);
            }
            if (day & 0X10000) {
                model->setData(model->index(Trans::Constants::Time::DS_BedTime, DrugsDB::DailySchemeModel::Value), Qt::Checked, Qt::CheckStateRole);
            }
            newDS.insert(k, model->serializedContent());
            model->clear();
        }
        delete model;
        QString req;
        foreach(int k, newDS.keys()) {
            req = QString("UPDATE `DOSAGE` SET `DAILYSCHEME`='%1' WHERE (POSO_ID=%2);")
                   .arg(newDS.value(k)).arg(k);
            QSqlQuery q(req,db);
            if (q.isActive()) {
                q.finish();
            } else {
                Utils::Log::addQueryError("DatabaseUpdater", q, __FILE__, __LINE__);
            }
            req.clear();
        }
        foreach(int k, m_Id_MealSchemes.keys()) {
            req = QString("UPDATE `DOSAGE` SET `MEALSCHEME`='%1' WHERE (POSO_ID=%2);")
                   .arg(m_Id_MealSchemes.value(k)+1).arg(k);
            QSqlQuery q(req,db);
            if (q.isActive()) {
                q.finish();
            } else {
                Utils::Log::addQueryError("DatabaseUpdater", q, __FILE__, __LINE__);
            }
            req.clear();
        }
        Utils::Log::addMessage("VersionUpdater", QString("Dosage Database values update done from %1 to %2").arg("0.0.8", "0.2.0"));
        return true;
    }

private:
    QString m_Name;
    mutable QMap<int, int> m_Id_DailySchemes;
    mutable QMap<int, int> m_Id_MealSchemes;
};

class IO_Update_From_0008_To_020 : public DrugsDB::DrugsIOUpdateStep
{
public:
    // Adding root FreeRx tag
    // Unfortunatly DailyScheme was not saved before 0.2.0 --> so no update is available
    // MealTime scheme must be update since an empty choice has been added at index (0)
    IO_Update_From_0008_To_020() : DrugsDB::DrugsIOUpdateStep() {}
    ~IO_Update_From_0008_To_020() {}

    QString fromVersion() const {return "0.0.8";}
    QString toVersion() const {return "0.2.0";}

    bool updateFromXml() const {return true;}
    bool executeXmlUpdate(QString &xml) const
    {
        int begin = xml.indexOf("<FullPrescription");
        xml.insert(begin, "<FreeDiams>");
        xml.append("</FreeDiams>");
        return true;
    }

    bool updateFromModel() const {return true;}
    bool executeUpdate(DrugsDB::DrugsModel *model, QList<int> rows) const
    {
        foreach(int r, rows) {
            // Retreive meal time
            int meal = model->index(r, Constants::Prescription::MealTimeSchemeIndex).data().toInt();
            if (meal!=0) {
                meal++;
                model->setData(model->index(r, Constants::Prescription::MealTimeSchemeIndex),meal);
            }
        }
        return true;
    }
};

class IO_Update_From_020_To_040 : public DrugsDB::DrugsIOUpdateStep
{
public:
    // Unfortunatly DailyScheme was not saved before 0.2.0 --> so no update is available
    // MealTime scheme must be update since an empty choice has been added at index (0)
    IO_Update_From_020_To_040() : DrugsDB::DrugsIOUpdateStep() {}
    ~IO_Update_From_020_To_040() {}

    QString fromVersion() const {return "0.2.0";}
    QString toVersion() const {return "0.4.0";}

    bool updateFromXml() const {return true;}
    bool executeXmlUpdate(QString &xml) const
    {
        // add root tag <FreeDiams> ?
        if (!xml.contains("<FreeDiams>", Qt::CaseInsensitive)) {
            int begin = xml.indexOf("<?xml");
            begin = xml.indexOf(">", begin) + 1;
            xml.insert(begin, "<FreeDiams>");
            xml.append("</FreeDiams>");
        }
        // Rename some tags
        xml.replace("<CIS>","<Drug_UID>");
        xml.replace("</CIS>", "</Drug_UID>");
        xml.replace("<CIP>","<Pack_UID>");
        xml.replace("</CIP>", "</Pack_UID>");
        // Get the old version
        if (!xml.startsWith("<?xml version=\""))
            return false;
        int begin = 15;
        int end = xml.indexOf("\"", begin);
        if (xml.mid(begin,end-begin) != "1.0") {
            // Add the new version tag
            xml.replace("<FullPrescription>", "<FullPrescription version=\"0.4.0\">");
            // Remove the olf version number
            xml.replace("<?xml version=\"0.2.0\"", "<?xml version=\"1.0\"");
            xml.replace("<?xml version=\"0.0.8\"", "<?xml version=\"1.0\"");
        }
        return true;
    }

    bool updateFromModel() const {return false;}
    bool executeUpdate(DrugsDB::DrugsModel *model, QList<int> rows) const
    {
        Q_UNUSED(model);
        Q_UNUSED(rows);
        return true;
    }
};

class IO_Update_From_040_To_050 : public DrugsDB::DrugsIOUpdateStep
{
public:
    // Add missing main root tag
    IO_Update_From_040_To_050() : DrugsDB::DrugsIOUpdateStep() {}
    ~IO_Update_From_040_To_050() {}

    QString fromVersion() const {return "0.4.0";}
    QString toVersion() const {return "0.5.0";}

    bool updateFromXml() const {return true;}
    bool executeXmlUpdate(QString &xml) const
    {
        // add root tag <FreeDiams> ?
        if (!xml.contains("<FreeDiams>", Qt::CaseInsensitive)) {
            int begin = xml.indexOf("<?xml");
            begin = xml.indexOf(">", begin) + 1;
            xml.insert(begin, "<FreeDiams>");
            xml.append("</FreeDiams>");
        }
        return true;
    }

    bool updateFromModel() const {return false;}
    bool executeUpdate(DrugsDB::DrugsModel *model, QList<int> rows) const
    {
        Q_UNUSED(model);
        Q_UNUSED(rows);
        return true;
    }
};

class IO_Update_From_050_To_060 : public DrugsDB::DrugsIOUpdateStep
{
public:
    // Update XML code to the new drugs database structure
    // Drugs == 3 uids + oldUid
    IO_Update_From_050_To_060() : DrugsDB::DrugsIOUpdateStep() {}
    ~IO_Update_From_050_To_060() {}

    QString fromVersion() const {return "0.5.0";}
    QString toVersion() const {return "0.6.0";}

    bool updateFromXml() const {return true;}

    QString extractValue(const QString &xml, const QString &tagName, int start = 0) const
    {
        int begin = xml.indexOf(QString("<%1").arg(tagName), start);
        int end = xml.indexOf(QString("</%1>").arg(tagName), begin);
        if (end == -1)
            return QString();
        QString ex;
        if (begin != -1) {
            begin = xml.indexOf(">", begin + tagName.length()) + 1;
            ex = xml.mid(begin, end-begin);
        }
        return ex;
    }
    QString extractBlock(const QString &xml, const QString &tagName, int start = 0) const
    {
        int begin = xml.indexOf(QString("<%1").arg(tagName), start);
        int end = xml.indexOf(QString("</%1>").arg(tagName), begin);
        if (end == -1) {
            end = xml.indexOf(QString("/>"), begin);
            if (end == -1) {
                return QString();
            }
            end += 2;
        } else {
            end += QString("</%1>").arg(tagName).length();
        }
        QString ex;
        if (begin != -1) {
            m_LastExtractionEnd = end;
            ex = xml.mid(begin, end-begin);
        }
        return ex;
    }

    bool executeXmlUpdate(QString &xml) const
    {
        m_LastExtractionEnd = 0;
        QString db = extractValue(xml, "DrugsDatabaseName");

        // go to FullPrescription
        QStringList prescriptionItems;
        prescriptionItems
                << "OnlyForTest"
                << "Id"
                << "RefDosage"
                << "Pack_UID"
                << "IntakeFrom"
                << "IntakeTo"
                << "IntakeScheme"
                << "IntakeFromTo"
                << "IntakeIntervalTime"
                << "IntakeIntervalScheme"
                << "IntakeFullString"
                << "DurationFrom"
                << "DurationTo"
                << "DurationScheme"
                << "DurationFromTo"
                << "Period"
                << "PeriodScheme"
                << "Daily"
                << "MealTime"
                << "Note"
                << "INN"
                << "SpecifyForm"
                << "SpecifyPresentation"
                << "IsAld";

        QString fullPrescription = extractBlock(xml, "FullPrescription", m_LastExtractionEnd);
        QString prescription = extractBlock(fullPrescription, "Prescription");
        QString newPrescription;
        int begin = m_LastExtractionEnd;

        while (!prescription.isEmpty()) {
            // Get drug infos
            QString oldUid = extractValue(prescription, "Drug_UID");
            QString route = extractValue(prescription, "DrugRoute");
            QString form = extractValue(prescription, "DrugForm");
            QString strength = extractValue(prescription, "DrugStrength");
            QString name = extractValue(prescription, "DrugName");
            // Get composition
            QString fullCompo;
            QString composition = extractBlock(prescription, "Composition");
            while (!composition.isEmpty()) {
                fullCompo += composition;
                composition = extractBlock(prescription, "Composition", m_LastExtractionEnd);
            }

            // Extract Prescription's items
            QString presXml;
            foreach(const QString &item, prescriptionItems) {
                QString value = extractValue(prescription, item);
                if (!value.isEmpty())
                    presXml += QString(" %1=\"%2\"").arg(item).arg(value.replace("\"","&quote;"));
            }

            // Recreate the v0.6.0 block
            QString block = QString("<Drug u1=\"\" u2=\"\" u3=\"\" old=\"%1\" db=\"%2\">\n"
                                   "<DrugRoute>%3</DrugRoute>\n"
                                   "<DrugForm>%4</DrugForm>\n"
                                   "<DrugStrength>%5</DrugStrength>\n"
                                   "<DrugName>%6</DrugName>\n"
                                   "%7\n"
                                   "</Drug>\n")
                    .arg(oldUid)
                    .arg(db)
                    .arg(route)
                    .arg(form)
                    .arg(strength)
                    .arg(name)
                    .arg(fullCompo);
            block += QString("<Dose %1/>\n").arg(presXml);

            newPrescription += QString("<Prescription>\n%1</Prescription>\n").arg(block);

            // Go to next Prescription block
            prescription.clear();
            prescription = extractBlock(fullPrescription, "Prescription", begin);
            begin = m_LastExtractionEnd;
        }

        // Replace content of FullPrescription with the new prescription
        begin = xml.indexOf("<FullPrescription", 0, Qt::CaseInsensitive);
        int end = xml.indexOf("</FullPrescription>", begin, Qt::CaseInsensitive);
        newPrescription.prepend("<FullPrescription version=\"0.6.0\">\n");
        xml.replace(begin, end-begin, newPrescription);
        return true;
    }

    bool updateFromModel() const {return false;}
    bool executeUpdate(DrugsDB::DrugsModel *model, QList<int> rows) const
    {
        Q_UNUSED(model);
        Q_UNUSED(rows);
        return true;
    }

private:
    mutable int m_LastExtractionEnd;
};

class IO_Update_From_060_To_072 : public DrugsDB::DrugsIOUpdateStep
{
public:
    // Update XML code:
    // - DrugUidName is included in the 0.7.2 output file (just for external EMR information)
    IO_Update_From_060_To_072() : DrugsDB::DrugsIOUpdateStep() {}
    ~IO_Update_From_060_To_072() {}

    QString fromVersion() const {return "0.6.0";}
    QString toVersion() const {return "0.7.2";}

    bool updateFromXml() const {return true;}

    bool executeXmlUpdate(QString &xml) const
    {
        // Replace version numbering
        xml.replace("<FullPrescription version=\"0.6.0\">", "<FullPrescription version=\"0.7.2\">");
        return true;
    }

    bool updateFromModel() const {return false;}
    bool executeUpdate(DrugsDB::DrugsModel *model, QList<int> rows) const
    {
        Q_UNUSED(model);
        Q_UNUSED(rows);
        return true;
    }
};

}  // End anonymous namespace

namespace DrugsDB {

class VersionUpdaterPrivate
{
public:
    VersionUpdaterPrivate() {}
    ~VersionUpdaterPrivate()
    {
        qDeleteAll(m_Updaters);
    }

    static QStringList dosageDatabaseVersions() { return QStringList() << "0.0.8" << "0.2.0" << "0.4.0" << "0.5.0" << "0.5.4" << "1"; }
    static QStringList xmlIoVersions() {return QStringList() << "0.0.8" << "0.2.0" << "0.4.0" << "0.5.0" << "0.6.0" << "0.7.2"; }

    QString xmlVersion(const QString &xml)
    {
        // TODO: read with QDomDoc
        QString v;
        if (xml.startsWith("<?xml version=\"1.0\"") ||
            xml.startsWith("<?xml version='1.0'")) {
            int begin = xml.indexOf("<FullPrescription version=\"") + 27;
            int end = xml.indexOf("\">", begin);
            if (end==-1)
                end = xml.indexOf("\"/>", begin);
            v = xml.mid(begin,end-begin).simplified();
        } else {
            int begin = 15;
            int end = xml.indexOf("\"", begin);
            v = xml.mid(begin,end-begin).simplified();
        }
        return v;
    }

    QMap<QString, DrugsIOUpdateStep *> ioSteps()
    {
        QMap<QString, DrugsIOUpdateStep *> list;
        foreach(GenericUpdateStep *step, m_Updaters) {
            DrugsIOUpdateStep *iostep = dynamic_cast<DrugsIOUpdateStep *>(step);
            if (iostep)
                list.insert(iostep->fromVersion(), iostep);
        }
        return list;
    }

    QMap<QString, DosageDatabaseUpdateStep *> dosageDatabaseSteps()
    {
        QMap<QString, DosageDatabaseUpdateStep *> list;
        foreach(GenericUpdateStep *step, m_Updaters) {
            DosageDatabaseUpdateStep *dbstep = dynamic_cast<DosageDatabaseUpdateStep *>(step);
            if (dbstep)
                list.insert(dbstep->fromVersion(), dbstep);
        }
        return list;
    }

    QList<GenericUpdateStep *> m_Updaters;
    QString m_DosageDatabaseVersion;
    QString m_IOVersion;
};

}  //  end namespace DrugsDB

VersionUpdater::VersionUpdater() :
    d(new VersionUpdaterPrivate)
{
    // Here is the good place to create updaters objects
    d->m_Updaters.append(new ::Dosage_008_To_020);
    d->m_Updaters.append(new ::Dosage_030_To_040);
    d->m_Updaters.append(new ::Dosage_040_To_050);
    d->m_Updaters.append(new ::Dosage_050_To_054);
    d->m_Updaters.append(new ::Dosage_054_To_1);
    
    d->m_Updaters.append(new ::IO_Update_From_0008_To_020);
    d->m_Updaters.append(new ::IO_Update_From_020_To_040);
    d->m_Updaters.append(new ::IO_Update_From_040_To_050);
    d->m_Updaters.append(new ::IO_Update_From_050_To_060);
    d->m_Updaters.append(new ::IO_Update_From_060_To_072);
}

VersionUpdater::~VersionUpdater()
{
    if (d) {
        delete d;
        d=0;
    }
}

bool VersionUpdater::isDosageDatabaseUpToDate() const
{
//    if (!d->m_DosageDatabaseVersion.isEmpty())
//        return (d->m_DosageDatabaseVersion==d->dosageDatabaseVersions().last());

    QSqlDatabase db = QSqlDatabase::database(Dosages::Constants::DB_DOSAGES_NAME);
    if (!db.open()) {
        Utils::warningMessageBox(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Dosages::Constants::DB_DOSAGES_NAME, db.lastError().text()),"","","");
        Utils::Log::addError("VersionUpdater",tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Dosages::Constants::DB_DOSAGES_NAME, db.lastError().text()),
                             __FILE__, __LINE__);
        return true;
    }
    QString req = "﻿SELECT `ACTUAL` FROM `VERSION` ORDER BY `ACTUAL` LIMIT 1;";
    QSqlQuery q(req, db);
    if (q.isActive()) {
        if (q.next()) {
            d->m_DosageDatabaseVersion = q.value(0).toString();
        }
    } else {
        Utils::Log::addQueryError("VersionUpdater", q, __FILE__, __LINE__);
        return true;
    }
    q.finish();
    return (d->m_DosageDatabaseVersion==d->dosageDatabaseVersions().last());
}

bool VersionUpdater::updateDosageDatabase()
{
    QMap<QString, DosageDatabaseUpdateStep *> from = d->dosageDatabaseSteps();
    QString version = d->m_DosageDatabaseVersion;
    while (version != d->dosageDatabaseVersions().last()) {
        DosageDatabaseUpdateStep *step = from.value(version, 0);
        if (!step)
            break;
        step->setConnectionName(Dosages::Constants::DB_DOSAGES_NAME);
        if (!step->retrieveValuesToUpdate()) {
            Utils::Log::addError("VersionUpdater", QString("Error while updating %1 from %2 to %3: %4")
                                 .arg(Dosages::Constants::DB_DOSAGES_NAME, step->fromVersion(), step->toVersion()),
                                 __FILE__, __LINE__);
            return false;
        }
        if (!step->updateDatabaseScheme()) {
            Utils::Log::addError("VersionUpdater", QString("Error while updating %1 from %2 to %3: %4")
                                 .arg(Dosages::Constants::DB_DOSAGES_NAME, step->fromVersion(), step->toVersion()),
                                 __FILE__, __LINE__);
            return false;
        }
        if (!step->saveUpdatedValuesToDatabase()) {
            Utils::Log::addError("VersionUpdater", QString("Error while updating %1 from %2 to %3: %4")
                                 .arg(Dosages::Constants::DB_DOSAGES_NAME, step->fromVersion(), step->toVersion()),
                                 __FILE__, __LINE__);
            return false;
        }
        version = step->toVersion();
    }
    if (version==d->dosageDatabaseVersions().last())
        d->m_DosageDatabaseVersion=version;
    return true;
}

QString VersionUpdater::lastDosageDabaseVersion() const
{
    return d->dosageDatabaseVersions().last();
}

QString VersionUpdater::lastXmlIOVersion() const
{
    return d->xmlIoVersions().last();
}

QString VersionUpdater::xmlVersion(const QString &xmlContent) const
{
    return d->xmlVersion(xmlContent);
}

bool VersionUpdater::isXmlIOUpToDate(const QString &xmlContent) const
{
    return (d->xmlVersion(xmlContent) == d->xmlIoVersions().last());
}

QString VersionUpdater::updateXmlIOContent(const QString &xmlContent)
{
    LOG_FOR("VersionUpdater", "Updating XML IO content version");
    QMap<QString, DrugsIOUpdateStep *> from = d->ioSteps();
    QString version = d->xmlVersion(xmlContent);
    QString xml = xmlContent;
    while (version != d->xmlIoVersions().last()) {
        DrugsIOUpdateStep *step = from.value(version, 0);
        if (!step)
            break;
        if (step->updateFromXml()) {
            if (step->fromVersion() == version) {
                if (!step->executeXmlUpdate(xml)) {
                    LOG_ERROR_FOR("VersionUpdater", QString("Error when updating from %1 to %2").arg(version).arg(step->toVersion()));
                } else {
                    version = step->toVersion();
                }
            }
        } else {
            version = step->toVersion();
        }
    }
    return xml;
}

bool VersionUpdater::updateXmlIOModel(const QString &fromVersion, DrugsDB::DrugsModel *model, const QList<int> &rowsToUpdate)
{
    LOG_FOR("VersionUpdater", "Updating IO model version");
    QMap<QString, DrugsIOUpdateStep *> from = d->ioSteps();
    QString version = fromVersion;
    while (version != d->xmlIoVersions().last()) {
        DrugsIOUpdateStep *step = from.value(version, 0);
        if (!step)
            break;
        if (step->updateFromModel()) {
            if (step->fromVersion() == version) {
                if (!step->executeUpdate(model, rowsToUpdate))
                    LOG_ERROR_FOR("VersionUpdater", QString("Error when updating from %1 to %2").arg(version).arg(step->toVersion()));
                else
                    version = step->toVersion();
            }
        } else {
            version = step->toVersion();
        }
    }
    return true;
}
