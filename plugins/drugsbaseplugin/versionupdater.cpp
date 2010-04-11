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
  - If one of these process updates the prescription, the version is setted to the toVersion().
  - You can not update either from the XML content and from the Model.
  \sa DrugsDb::DrugsIO
  \ingroup freediams drugswidget
*/

#include "versionupdater.h"
#include "constants.h"

#include <drugsbaseplugin/drugsbase.h>
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

using namespace Trans::ConstantTranslations;
using namespace DrugsDB;

///////////////////////////////////////////////////////////////////////
//////////////////////////// UPDATE STEPS /////////////////////////////
///////////////////////////////////////////////////////////////////////
namespace {

    class Dosage_030_To_040 : public DrugsDB::DosageDatabaseUpdateStep
    {
    public:
        // From v 0.0.8 To v 0.2.0
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

        bool retreiveValuesToUpdate() const  {return true;}

        bool updateDatabaseScheme() const
        {
            // these versions only use SQLite
            QSqlDatabase db = QSqlDatabase::database(Dosages::Constants::DB_DOSAGES_NAME);
            if (!db.open()) {
                return false;
            }
            QStringList req;
            req << "﻿ALTER TABLE `DOSAGE` RENAME TO `OLD_DOSAGE`;";
            req << DrugsDB::Internal::DrugsBase::dosageCreateTableSqlQuery();
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
                    Utils::Log::addQueryError("VersionUpdater", q);
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
        QSqlDatabase db = QSqlDatabase::database(Dosages::Constants::DB_DOSAGES_NAME);
        if (!db.open()) {
            return false;
        }
        QStringList req;
        req << "﻿ALTER TABLE `DOSAGE` RENAME TO `OLD_DOSAGE`;";
        req << DrugsDB::Internal::DrugsBase::dosageCreateTableSqlQuery();
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
                Utils::Log::addQueryError("VersionUpdater", q);
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
    // Unfortunatly DailyScheme was not saved before 0.2.0 --> so no update is available
    // MealTime scheme must be update since an empty choice has been added at index (0)
    IO_Update_From_0008_To_020() : DrugsDB::DrugsIOUpdateStep() {}
    ~IO_Update_From_0008_To_020() {}

    QString fromVersion() const {return "0.0.8";}
    QString toVersion() const {return "0.2.0";}

    bool updateFromXml() const {return false;}
    bool executeXmlUpdate(QString &xml) const {Q_UNUSED(xml); return true;}

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

}  // End anonymous namespace

namespace DrugsDB {
VersionUpdater *VersionUpdater::m_Instance = 0;

class VersionUpdaterPrivate
{
public:
    VersionUpdaterPrivate() {}
    ~VersionUpdaterPrivate()
    {
        qDeleteAll(m_Updaters);
    }

    static QStringList dosageDatabaseVersions() { return QStringList() << "0.0.8" << "0.2.0" << "0.4.0"; }
    static QStringList xmlIoVersions() {return QStringList() << "0.0.8" << "0.2.0" << "0.4.0"; }

    QString xmlVersion(const QString &xml)
    {
        QString v;
        if (xml.startsWith("<?xml version=\"1.0\"")) {
            int begin = xml.indexOf("<FullPrescription version=\"") + 27;
            int end = xml.indexOf("\">", begin);
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

VersionUpdater *VersionUpdater::instance()
{
    if (!m_Instance)
        m_Instance = new VersionUpdater();
    return m_Instance;
}

VersionUpdater::VersionUpdater() : d(0)
{
    d = new VersionUpdaterPrivate;
    // Here is the good place to create updaters objects
    d->m_Updaters.append(new ::Dosage_008_To_020);
    d->m_Updaters.append(new ::Dosage_030_To_040);
    d->m_Updaters.append(new ::IO_Update_From_0008_To_020);
    d->m_Updaters.append(new ::IO_Update_From_020_To_040);
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
    if (!d->m_DosageDatabaseVersion.isEmpty())
        return (d->m_DosageDatabaseVersion==d->dosageDatabaseVersions().last());

    QSqlDatabase db = QSqlDatabase::database(Dosages::Constants::DB_DOSAGES_NAME);
    if (!db.open()) {
        Utils::warningMessageBox(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Dosages::Constants::DB_DOSAGES_NAME, db.lastError().text()),"","","");
        Utils::Log::addError("VersionUpdater",tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Dosages::Constants::DB_DOSAGES_NAME, db.lastError().text()));
        return true;
    }
    QString req = "﻿SELECT `ACTUAL` FROM `VERSION` ORDER BY `ACTUAL` LIMIT 1;";
    QSqlQuery q(req, db);
    if (q.isActive()) {
        if (q.next()) {
            d->m_DosageDatabaseVersion = q.value(0).toString();
        }
    } else {
        Utils::Log::addQueryError("VersionUpdater", q);
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
        if (!step->retreiveValuesToUpdate()) {
            Utils::Log::addError("VersionUpdater", QString("Error while updating %1 from %2 to %3 : %4")
                                 .arg(Dosages::Constants::DB_DOSAGES_NAME, step->fromVersion(), step->toVersion()));
            return false;
        }
        if (!step->updateDatabaseScheme()) {
            Utils::Log::addError("VersionUpdater", QString("Error while updating %1 from %2 to %3 : %4")
                                 .arg(Dosages::Constants::DB_DOSAGES_NAME, step->fromVersion(), step->toVersion()));
            return false;
        }
        if (!step->saveUpdatedValuesToDatabase()) {
            Utils::Log::addError("VersionUpdater", QString("Error while updating %1 from %2 to %3 : %4")
                                 .arg(Dosages::Constants::DB_DOSAGES_NAME, step->fromVersion(), step->toVersion()));
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
    Utils::Log::addMessage("VersionUpdater", "Updating XML IO content version");
    QMap<QString, DrugsIOUpdateStep *> from = d->ioSteps();
    QString version = d->xmlVersion(xmlContent);
    QString xml = xmlContent;
    while (version != d->xmlIoVersions().last()) {
        DrugsIOUpdateStep *step = from.value(version, 0);
        if (!step)
            break;
        if (step->updateFromXml()) {
            if (step->fromVersion() == version) {
                if (!step->executeXmlUpdate(xml))
                    Utils::Log::addError("VersionUpdater", QString("Error when updating from %1 to %2").arg(version).arg(step->toVersion()));
                else
                    version = step->toVersion();
            }
        } else {
            version = step->toVersion();
        }
    }
    return xml;
}

bool VersionUpdater::updateXmlIOModel(const QString &fromVersion, DrugsDB::DrugsModel *model, const QList<int> &rowsToUpdate)
{
    Utils::Log::addMessage("VersionUpdater", "Updating IO model version from version " + fromVersion);
    QMap<QString, DrugsIOUpdateStep *> from = d->ioSteps();
    QString version = fromVersion;
    while (version != d->xmlIoVersions().last()) {
        DrugsIOUpdateStep *step = from.value(version, 0);
        if (!step)
            break;
        if (step->updateFromModel()) {
            if (step->fromVersion() == version) {
                if (!step->executeUpdate(model, rowsToUpdate))
                    Utils::Log::addError("VersionUpdater", QString("Error when updating from %1 to %2").arg(version).arg(step->toVersion()));
                else
                    version = step->toVersion();
            }
        } else {
            version = step->toVersion();
        }
    }
    return true;
}
