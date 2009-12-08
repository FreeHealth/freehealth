/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
/**
  \class DrugsDB::Internal::DatabaseUpdater
  \brief This class manages drugs and dosages databases updates from versions to versions
*/
#include "databaseupdater.h"
#include "constants.h"

#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/dailyschememodel.h>

#include <utils/global.h>
#include <utils/log.h>

#include <translationutils/constanttranslations.h>

#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>
#include <QMap>
#include <QVariant>


using namespace Trans::ConstantTranslations;

///////////////////////////////////////////////////////////////////////
//////////////////////////// UPDATE STEPS /////////////////////////////
///////////////////////////////////////////////////////////////////////
namespace {
class Dosage_One : public DrugsDB::UpdateStep
{
public:
    // From v 0.0.8 To v 0.2.0
    // - New Daily Scheme format (XML)
    // - Correcting MealTime Scheme (one more choice == 0 : '')
    Dosage_One() {}
    ~Dosage_One() {}

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

    bool retreiveValuesToUpdate() const
    {
        Q_ASSERT(!m_Name.isEmpty());
        QSqlDatabase db = QSqlDatabase::database(m_Name);
        if (!db.open()) {
            Utils::warningMessageBox(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(m_Name, db.lastError().text()),"","","");
            Utils::Log::addError("DatabaseUpdater", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(m_Name, db.lastError().text()));
            return false;
        }
        QString req = "SELECT `POSO_ID`, `DAILYSCHEME` FROM `DOSAGE` WHERE (`DAILYSCHEME`>0) ORDER BY `POSO_ID` ASC;";
        QSqlQuery q(req, db);
        if (q.isActive()) {
            while (q.next()) {
                m_Id_DailySchemes.insert(q.value(0).toInt(), q.value(1).toInt());
            }
        } else {
            Utils::Log::addQueryError("DatabaseUpdater", q);
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
            Utils::Log::addQueryError("DatabaseUpdater", q);
            return false;
        }
        return true;
    }

    bool updateDatabaseScheme() const
    {
        // these versions only use SQLite
        QSqlDatabase db = QSqlDatabase::database(Dosages::Constants::DOSAGES_DATABASE_NAME);
        if (!db.open()) {
            return false;
        }
        QStringList req;
        req << "﻿ALTER TABLE `DOSAGE` RENAME TO `OLD_DOSAGE`;";
        req << DrugsDB::Internal::DrugsBase::dosageCreateTableSqlQuery();
        req << QString("INSERT INTO `DOSAGE` (%1) SELECT %1 FROM `OLD_DOSAGE`")
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
        req << "INSERT INTO `VERSION` (`ACTUAL`) VALUES('0.2.0');";
        foreach(const QString &r, req) {
            QSqlQuery q(r,db);
            if (q.isActive()) {
                q.finish();
            } else
                Utils::Log::addQueryError("DatabaseUpdater", q);
        }
        return true;
    }

    bool saveUpdatedValuesToDatabase() const
    {
        // these versions only use SQLite
        QSqlDatabase db = QSqlDatabase::database(Dosages::Constants::DOSAGES_DATABASE_NAME);
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
                Utils::Log::addQueryError("DatabaseUpdater", q);
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
                Utils::Log::addQueryError("DatabaseUpdater", q);
            }
            req.clear();
        }
        return true;
    }

private:
    QString m_Name;
    mutable QMap<int, int> m_Id_DailySchemes;
    mutable QMap<int, int> m_Id_MealSchemes;
};

static void createDosageDatabaseUpdateSteps(QMap<QString, DrugsDB::UpdateStep *> &map)
{
    map.clear();
    Dosage_One *one = new ::Dosage_One;
    map.insert(one->fromVersion(), one);
}

}  // End private namespace


namespace DrugsDB {
namespace DatabaseUpdater {

QStringList dosageDatabaseVersions()
{
    return QStringList() << "0.0.8" << "0.2.0";
}

bool checkDosageDatabaseUpdates()
{
    QSqlDatabase db = QSqlDatabase::database(Dosages::Constants::DOSAGES_DATABASE_NAME);
    if (!db.open()) {
        Utils::warningMessageBox(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Dosages::Constants::DOSAGES_DATABASE_NAME, db.lastError().text()),"","","");
        Utils::Log::addError("DatabaseUpdater",tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Dosages::Constants::DOSAGES_DATABASE_NAME, db.lastError().text()));
        return false;
    }
    QString req = "﻿SELECT `ACTUAL` FROM `VERSION` ORDER BY `ACTUAL` ASC LIMIT 1;";
    QString version;
    QSqlQuery q(req, db);
    if (q.isActive()) {
        if (q.next()) {
            version = q.value(0).toString();
        }
    } else {
        Utils::Log::addQueryError("DatabaseUpdater", q);
        return false;
    }
    q.finish();
    if (version == dosageDatabaseVersions().last())
        return true;

    // here we have to update the dosages database
    QMap<QString, UpdateStep *> from;
    ::createDosageDatabaseUpdateSteps(from);
    int i = 0;
    while (version != dosageDatabaseVersions().last()) {
        UpdateStep *step = from.value(version, 0);
        if (!step)
            break;
        step->setConnectionName(Dosages::Constants::DOSAGES_DATABASE_NAME);
        if (!step->retreiveValuesToUpdate()) {
            Utils::Log::addError("DatabaseUpdater", QString("Error while updating %1 from %2 to %3 : %4")
                                 .arg(Dosages::Constants::DOSAGES_DATABASE_NAME, step->fromVersion(), step->toVersion()));
            return false;
        }
        if (!step->updateDatabaseScheme()) {
            Utils::Log::addError("DatabaseUpdater", QString("Error while updating %1 from %2 to %3 : %4")
                                 .arg(Dosages::Constants::DOSAGES_DATABASE_NAME, step->fromVersion(), step->toVersion()));
            return false;
        }
        if (!step->saveUpdatedValuesToDatabase()) {
            Utils::Log::addError("DatabaseUpdater", QString("Error while updating %1 from %2 to %3 : %4")
                                 .arg(Dosages::Constants::DOSAGES_DATABASE_NAME, step->fromVersion(), step->toVersion()));
            return false;
        }
        version = step->toVersion();
    }

    qDeleteAll(from);
    from.clear();
    return true;
}

}  // end namespace DatabaseUpdater
}  // end namespace DrugsDB

