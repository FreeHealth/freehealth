/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
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
  \class DrugsBase
  \brief This class owns the drugs and dosages database and interactions mechanism.

  0. Terminology\n
  \e Substances are the chimic molecules that compose a drug.\n
  \e IamCode or \e INN are the INN codes and names.\n
  \e IamClass are the classes that regroups INNs into classes of pharmaceutics family.\n
  \e CIP : presentation code of a drug. A drug can be presented into different presentation (15 pills, 30 pills a box...).\n
  \e UID : speciality code of a drug. Everything is base on this code.
     One UID code can be associated to many CIP, many Substances, many INNs, and many IamClasses.

  1. Initialization\n
  This class is pure static, so you can not instanciate it. To initialize datas, just do once : init().
  isInitialized() alerts you of the state of intialization.
  These two members returns true if all is ok.

  2. Drugs retreiver\n
  You can retreive drugs using UID ou CIP code via getDrugByUID() and getDrufByCIP().

  3. Dosages retreiver / saver

  \todo Manage user rights when creating dosage database

  \sa database_structure
  \ingroup freediams drugswidget
*/

#include "drugsbase.h"

//include drugswidget headers
#include <drugsbaseplugin/drugsdata.h>
#include <drugsbaseplugin/drugsinteraction.h>
#include <drugsbaseplugin/constants.h>
#include <drugsbaseplugin/versionupdater.h>
#include <drugsbaseplugin/drugsdatabaseselector.h>
#include <drugsbaseplugin/drugsearchengine.h>

#include <utils/global.h>
#include <utils/log.h>

#include <coreplugin/isettings.h>
#include <coreplugin/icore.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/dialogs/settingsdialog.h>

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QFile>
#include <QDir>
#include <QMultiHash>
#include <QMap>
#include <QMultiMap>
#include <QList>
#include <QSet>

using namespace DrugsDB;
using namespace DrugsDB::Internal;
using namespace Trans::ConstantTranslations;


static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline DrugsDB::Internal::DrugSearchEngine *searchEngine()  { return DrugsDB::Internal::DrugSearchEngine::instance(); }
static inline QString defaultDatabaseFileName() {return settings()->databasePath() + QDir::separator() + QString(DB_DRUGS_NAME) + QDir::separator() + QString(DB_DRUGS_NAME) + "-fr_FR.db";}

namespace DrugsDB {
namespace Internal {
/**
  \brief Private part of DrugsBase
  \internal
*/
class DrugsBasePrivate
{
public:
    DrugsBasePrivate(DrugsBase * base) :
            q(base),
            m_ActualDBInfos(0),
            m_LogChrono(false),
            m_RefreshDrugsBase(false),
            m_RefreshDosageBase(false)
    {}

    ~DrugsBasePrivate()
    {
    }

public:
    DrugsBase *q;
    DatabaseInfos *m_ActualDBInfos;
    bool m_LogChrono, m_RefreshDrugsBase, m_RefreshDosageBase;
};
}  // End Internal
}  // End DrugsDB


//--------------------------------------------------------------------------------------------------------
//--------------------------------- Initialization of static members -------------------------------------
//--------------------------------------------------------------------------------------------------------
DrugsBase * DrugsBase::m_Instance = 0;
bool DrugsBase::m_initialized = false;

//--------------------------------------------------------------------------------------------------------
//-------------------------------------- Initializing Database -------------------------------------------
//--------------------------------------------------------------------------------------------------------
/** \brief Returns the unique instance of DrugsBase. If it does not exists, it is created */
DrugsBase *DrugsBase::instance()
{
    if (!m_Instance) {
        m_Instance = new DrugsBase(qApp);
        m_Instance->init();
    }
    return m_Instance;
}

/**
   \brief Constructor.
   \private
*/
DrugsBase::DrugsBase(QObject *parent)
    : QObject(parent), InteractionsBase(), Utils::Database(), d(0)
{
    d = new DrugsBasePrivate(this);
    setObjectName("DrugsBase");

    // DRUGS DATABASE
    addTable(Table_DRUGS,       "DRUGS");
    addTable(Table_COMPO,       "COMPOSITION");
    addTable(Table_PACKAGING,   "PACKAGING");
    addTable(Table_LK_MOL_ATC,  "LK_MOL_ATC");
    addTable(Table_INFORMATION, "INFORMATIONS");
    addTable(Table_SEARCHENGINES, "SEARCH_ENGINES");

    addField(Table_DRUGS, DRUGS_UID ,           "UID");
    addField(Table_DRUGS, DRUGS_NAME ,          "NAME");
    addField(Table_DRUGS, DRUGS_FORM,           "FORM");
    addField(Table_DRUGS, DRUGS_ROUTE,          "ROUTE");
    addField(Table_DRUGS, DRUGS_ATC,            "ATC");
    addField(Table_DRUGS, DRUGS_STRENGTH,       "GLOBAL_STRENGTH");
    addField(Table_DRUGS, DRUGS_TYPE_MP,        "TYPE_MP");
    addField(Table_DRUGS, DRUGS_AUTHORIZATION,  "AUTHORIZATION");
    addField(Table_DRUGS, DRUGS_MARKET,         "MARKETED");
    addField(Table_DRUGS, DRUGS_LINK_SPC,       "LINK_SPC");

    addField(Table_PACKAGING, PACK_DRUG_UID,    "UID");
    addField(Table_PACKAGING, PACK_PACK_UID,    "PACKAGE_UID");
    addField(Table_PACKAGING, PACK_LABEL,       "LABEL");
    addField(Table_PACKAGING, PACK_STATUS,      "STATUS");
    addField(Table_PACKAGING, PACK_MARKET,      "MARKETING");
    addField(Table_PACKAGING, PACK_DATE,        "DATE");
    addField(Table_PACKAGING, PACK_OPTION_CODE, "OPTIONAL_CODE");

    addField(Table_COMPO, COMPO_UID,          "UID");
    addField(Table_COMPO, COMPO_MOL_FORM,     "MOLECULE_FORM");
    addField(Table_COMPO, COMPO_MOL_CODE,     "MOLECULE_CODE");
    addField(Table_COMPO, COMPO_MOL_NAME,     "MOLECULE_NAME");
//    addField(Table_COMPO, COMPO_MOL_ATC,      "MOLECULE_ATC");
    addField(Table_COMPO, COMPO_DOSAGE,       "DOSAGE");
    addField(Table_COMPO, COMPO_REF_DOSAGE,   "DOSAGE_REF");
    addField(Table_COMPO, COMPO_NATURE,       "NATURE");
    addField(Table_COMPO, COMPO_LK_NATURE,    "LK_NATURE");

    addField(Table_LK_MOL_ATC, LK_MOLECULECODE,"MOLECULE_CODE");
    addField(Table_LK_MOL_ATC, LK_ATC_ID,      "ATC_ID");

    addField(Table_INFORMATION, INFO_VERSION,           "VERSION");
    addField(Table_INFORMATION, INFO_NAME,              "NAME");
    addField(Table_INFORMATION, INFO_IDENTIFIANT,       "IDENTIFIANT");
    addField(Table_INFORMATION, INFO_COMPAT_VERSION,    "COMPAT_VERSION");
    addField(Table_INFORMATION, INFO_PROVIDER,          "PROVIDER");
    addField(Table_INFORMATION, INFO_WEBLINK,           "WEBLINK");
    addField(Table_INFORMATION, INFO_COMPLEMENTARY_WWW, "COMPLEMENTARY_WEBSITE");
    addField(Table_INFORMATION, INFO_AUTHOR,            "AUTHOR");
    addField(Table_INFORMATION, INFO_LICENSE,           "LICENSE");
    addField(Table_INFORMATION, INFO_LICENSE_TERMS,     "LICENSE_TERMS");
    addField(Table_INFORMATION, INFO_DATE,              "DATE");
    addField(Table_INFORMATION, INFO_DRUG_UID_NAME,     "DRUG_UID_NAME");
    addField(Table_INFORMATION, INFO_PACK_MAIN_CODE_NAME, "PACK_MAIN_CODE_NAME");
    addField(Table_INFORMATION, INFO_ATC,               "ATC");
    addField(Table_INFORMATION, INFO_INTERACTIONS,      "INTERACTIONS");
    addField(Table_INFORMATION, INFO_AUTHOR_COMMENTS,   "AUTHOR_COMMENTS");
    addField(Table_INFORMATION, INFO_LANGUAGE_COUNTRY,  "LANGUAGE_COUNTRY");
    addField(Table_INFORMATION, INFO_DRUGS_NAME_CONSTRUCTOR, "DRUGS_NAME_CONSTRUCTOR");

    addField(Table_SEARCHENGINES, SEARCHENGINE_ID, "ID");
    addField(Table_SEARCHENGINES, SEARCHENGINE_LABEL, "LABEL");
    addField(Table_SEARCHENGINES, SEARCHENGINE_URL, "URL");

    connect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
}

/** \brief Destructor. */
DrugsBase::~DrugsBase()
{
    if (d) delete d;
    d=0;
}


/** \brief Initializer for the database. Return the error state. */
bool DrugsBase::init()
{
    // only one base can be initialized
    if (m_initialized)
        return true;

    // remove drugs database connection if exists
    if (d->m_RefreshDrugsBase) {
        if (QSqlDatabase::connectionNames().contains(Constants::DB_DRUGS_NAME)) {
            QSqlDatabase::database(Constants::DB_DRUGS_NAME).close();
            QSqlDatabase::removeDatabase(Constants::DB_DRUGS_NAME);
        }
        d->m_RefreshDrugsBase = false;
    }

    // create drugs database connection
    if (!QSqlDatabase::connectionNames().contains(Constants::DB_DRUGS_NAME)) {
        // test driver
        if (!QSqlDatabase::isDriverAvailable("QSQLITE")) {
            Utils::Log::addError(this, tkTr(Trans::Constants::SQLITE_DRIVER_NOT_AVAILABLE));
            Utils::warningMessageBox(tkTr(Trans::Constants::APPLICATION_FAILURE),
                                     tkTr(Trans::Constants::SQLITE_DRIVER_NOT_AVAILABLE_DETAIL),
                                     "", qApp->applicationName());
            return false;
        }

        QString dbFileName = settings()->value(Constants::S_SELECTED_DATABASE_FILENAME).toString();
        if (dbFileName.startsWith(Core::Constants::TAG_APPLICATION_RESOURCES_PATH)) {
            dbFileName.replace(Core::Constants::TAG_APPLICATION_RESOURCES_PATH, settings()->path(Core::ISettings::ReadOnlyDatabasesPath));
        }

        // define is default drugs database (fr_FR)
        // if settings drugs database is wrong --> use the default database
        if (dbFileName == DrugsDB::Constants::DB_DEFAULT_IDENTIFIANT) {
            m_IsDefaultDB = true;
            dbFileName = defaultDatabaseFileName();
        } else if ((dbFileName.isEmpty())
            || (!QFile(dbFileName).exists())) {
            Utils::Log::addMessage(this, "Using default drugs database because drugs database settings is not correct.");
            m_IsDefaultDB = true;
            dbFileName = defaultDatabaseFileName();
        } else {
            m_IsDefaultDB = false;
        }

        // log the path of the database
        QString pathToDb = QFileInfo(dbFileName).absolutePath();
        Utils::Log::addMessage(this, tr("Searching databases into dir %1").arg(pathToDb));

        // Connect Drugs Database
        if (createConnection(DB_DRUGS_NAME, QFileInfo(dbFileName).fileName(), pathToDb,
                             Utils::Database::ReadOnly, Utils::Database::SQLite)) {
            d->m_ActualDBInfos = getDatabaseInformations(DB_DRUGS_NAME);
        } else {
            Utils::Log::addError(this, "No drugs database found.");
            return false;
        }

        // refresh InteractionsBase cache
        InteractionsBase::init(true);
    }

    // Connect and check Dosage Database
    // Check settings --> SQLite or MySQL ?
    // remove drugs database connection if exists
    if (d->m_RefreshDosageBase) {
        if (QSqlDatabase::connectionNames().contains(Dosages::Constants::DB_DOSAGES_NAME)) {
            QSqlDatabase::removeDatabase(Dosages::Constants::DB_DOSAGES_NAME);
        }
        d->m_RefreshDosageBase = false;
    }

    // create drugs database connection
    if (!QSqlDatabase::connectionNames().contains(Dosages::Constants::DB_DOSAGES_NAME)) {
        if (settings()->value(Core::Constants::S_USE_EXTERNAL_DATABASE, false).toBool()) {
            createConnection(Dosages::Constants::DB_DOSAGES_NAME,
                             Dosages::Constants::DB_DOSAGES_NAME,
                             QString(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_HOST, QByteArray("localhost").toBase64()).toByteArray())),
                             Utils::Database::ReadWrite,
                             Utils::Database::MySQL,
                             QString(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_LOG, QByteArray("root").toBase64()).toByteArray())),
                             QString(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_PASS, QByteArray("").toBase64()).toByteArray())),
                             QString(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_PORT, QByteArray("").toBase64()).toByteArray())).toInt(),
                             Utils::Database::CreateDatabase);
        } else {
            createConnection(Dosages::Constants::DB_DOSAGES_NAME,
                             Dosages::Constants::DB_DOSAGES_FILENAME,
                             settings()->path(Core::ISettings::ReadWriteDatabasesPath) + QDir::separator() + QString(DB_DRUGS_NAME),
                             Utils::Database::ReadWrite, Utils::Database::SQLite,
                             "log", "pas", 0,
                             Utils::Database::CreateDatabase);
        }
        checkDosageDatabaseVersion();
    }

    // get all Drugs search engines
    QSqlQuery search(QSqlDatabase::database(Constants::DB_DRUGS_NAME));
    QString req = select(Constants::Table_SEARCHENGINES);
    if (search.exec(req)) {
        while (search.next()) {
            searchEngine()->addNewEngine(search.value(Constants::SEARCHENGINE_LABEL).toString(),
                                         search.value(Constants::SEARCHENGINE_URL).toString());
        }
    } else {
        Utils::Log::addQueryError(this, search);
    }
    Utils::Log::addMessage(this, QString("Getting %1 Drugs Search Engines").arg(searchEngine()->numberOfEngines()));

    // Initialize
    InteractionsBase::init();
    m_initialized = true;
    return true;
}

/**
  \brief This is for debugging purpose. Log timers for some crucial functions.
  \sa checkInteractions(), getDrugsByUID()
*/
void DrugsBase::logChronos(bool state)
{
    d->m_LogChrono = state;
    InteractionsBase::logChronos(state);
}

const DatabaseInfos *DrugsBase::actualDatabaseInformations() const
{
    if (!d->m_ActualDBInfos)
        return new DatabaseInfos;
    return d->m_ActualDBInfos;
}

bool DrugsBase::isDatabaseTheDefaultOne() const
{
    return m_IsDefaultDB;
}

bool DrugsBase::refreshAllDatabases()
{
    m_initialized = false;
    d->m_RefreshDrugsBase = true;
    d->m_RefreshDosageBase = true;
    Q_EMIT drugsBaseIsAboutToChange();
    Q_EMIT dosageBaseIsAboutToChange();
    bool r = init();
    if (r) {
        Q_EMIT drugsBaseHasChanged();
        Q_EMIT dosageBaseHasChanged();
    }
    return r;
}

bool DrugsBase::refreshDrugsBase()
{
    m_initialized = false;
    d->m_RefreshDrugsBase = true;
    Q_EMIT drugsBaseIsAboutToChange();
    bool r = init();
    if (r)
        Q_EMIT drugsBaseHasChanged();
    return r;
}

bool DrugsBase::refreshDosageBase()
{
    m_initialized = false;
    d->m_RefreshDosageBase = true;
    Q_EMIT dosageBaseIsAboutToChange();
    bool r = init();
    if (r)
        Q_EMIT dosageBaseHasChanged();
    return r;
}

DatabaseInfos *DrugsBase::getDatabaseInformations(const QString &connectionName)
{
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    if (!db.open())
        return 0;

    // check table INFORMATIONS
    if (!db.tables().contains(DrugsDB::Internal::DrugsBase::instance()->table(Constants::Table_INFORMATION))) {
        Utils::Log::addMessage(this, "Database " + connectionName + " does not contain a INFORMATION Table");
        return 0;
    }

    // read last row of table INFORMATIONS
    DatabaseInfos *info = 0;
    QString req = select(Constants::Table_INFORMATION);
    req += " LIMIT 1";
    QSqlQuery q(req,db);
    if (q.isActive()) {
        Utils::Log::addMessage(this, "Drugs database informations correctly read " + connectionName);
        if (q.next()) {
            info = new DatabaseInfos;
            info->version = q.value(Constants::INFO_VERSION).toString();
            info->name = q.value(Constants::INFO_NAME).toString();
            info->identifiant = q.value(Constants::INFO_IDENTIFIANT).toString();
            info->compatVersion = q.value(Constants::INFO_COMPAT_VERSION).toString();
            info->provider = q.value(Constants::INFO_PROVIDER).toString();
            info->weblink = q.value(Constants::INFO_WEBLINK).toString();
            info->author = q.value(Constants::INFO_AUTHOR).toString();
            info->license = q.value(Constants::INFO_LICENSE).toString();
            info->licenseTerms = q.value(Constants::INFO_LICENSE_TERMS).toString();
            info->date = q.value(Constants::INFO_DATE).toDate();
            info->drugsUidName = q.value(Constants::INFO_DRUG_UID_NAME).toString();
            info->packUidName = q.value(Constants::INFO_PACK_MAIN_CODE_NAME).toString();
            info->atcCompatible = q.value(Constants::INFO_ATC).toBool();
            info->iamCompatible = q.value(Constants::INFO_INTERACTIONS).toBool();
            info->authorComments = q.value(Constants::INFO_AUTHOR_COMMENTS).toString();
            info->lang_country = q.value(Constants::INFO_LANGUAGE_COUNTRY).toString();
            info->setDrugsNameConstructor(q.value(Constants::INFO_DRUGS_NAME_CONSTRUCTOR).toString());
            info->complementaryWebsite = q.value(Constants::INFO_COMPLEMENTARY_WWW).toString();
            info->connectionName = db.connectionName();
            if (db.driverName() == "QSQLITE") {
                info->fileName = db.databaseName();
            }
        }
    } else {
        Utils::Log::addQueryError(this, q);
    }
    return info;
}


QString DrugsBase::dosageCreateTableSqlQuery()
{
    /** \todo add UserUuid field or lkid */
    return "CREATE TABLE IF NOT EXISTS `DOSAGE` ("
           "`POSO_ID`               INTEGER        PRIMARY KEY AUTOINCREMENT,"
           "`POSO_UUID`             varchar(40)    NULL,"    // put NOT NULL
           "`DRUGS_DATABASE_IDENTIFIANT` varchar(200) NULL,   "
           "`INN_LK`                int(11)        DEFAULT -1,"
           "`INN_DOSAGE`            varchar(100)   NULL,"    // contains the dosage of the SA INN
           "`CIS_LK`                int(11)        DEFAULT -1,"
           "`CIP_LK`                int(11)        DEFAULT -1,"
           "`LABEL`                 varchar(300)   NULL,"    // put NOT NULL

           "`INTAKEFROM`            double         NULL,"    // put NOT NULL
           "`INTAKETO`              double         NULL,"
           "`INTAKEFROMTO`          bool           NULL,"
           "`INTAKESCHEME`          varchar(200)   NULL,"    // put NOT NULL
           "`INTAKESINTERVALOFTIME` int(10)        NULL,"
           "`INTAKESINTERVALSCHEME` varchar(200)   NULL,"

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

/**
  \brief Create the protocols database if it does not exists.
*/
bool DrugsBase::createDatabase(const QString &connectionName , const QString &dbName,
                               const QString &pathOrHostName,
                               TypeOfAccess /*access*/, AvailableDrivers driver,
                               const QString & login, const QString & pass,
                               const int port,
                               CreationOption /*createOption*/
                              )
{
    /** \todo  ask user if he wants : 1. an empty dosage base ; 2. to retreive dosages from internet FMF website */
    if (connectionName != Dosages::Constants::DB_DOSAGES_NAME)
        return false;

    Utils::Log::addMessage(this, tkTr(Trans::Constants::TRYING_TO_CREATE_1_PLACE_2)
                           .arg(dbName).arg(pathOrHostName));

    // create an empty database and connect
    QSqlDatabase DB;
    if (driver == SQLite) {
        DB = QSqlDatabase::addDatabase("QSQLITE" , connectionName);
        if (!QDir(pathOrHostName).exists())
            if (!QDir().mkpath(pathOrHostName))
                tkTr(Trans::Constants::_1_ISNOT_AVAILABLE_CANNOTBE_CREATED).arg(pathOrHostName);
        DB.setDatabaseName(QDir::cleanPath(pathOrHostName + QDir::separator() + dbName));
        DB.open();
        setDriver(Utils::Database::SQLite);
    }
    else if (driver == MySQL) {
        DB = QSqlDatabase::database(connectionName);
        if (!DB.open()) {
            QSqlDatabase d = QSqlDatabase::addDatabase("QMYSQL", "CREATOR");
            d.setHostName(pathOrHostName);
            /** \todo retreive log/pass */
            d.setUserName(login);
            d.setPassword(pass);
            d.setPort(port);
            if (!d.open()) {
                Utils::warningMessageBox(tr("Unable to create the Protocol database."),tr("Please contact dev team."));
                return false;
            }
            QSqlQuery q(QString("CREATE DATABASE `%1`").arg(dbName), d);
            if (!q.isActive()) {
                Utils::Log::addQueryError("Database", q);
                Utils::warningMessageBox(tr("Unable to create the Protocol database."),tr("Please contact dev team."));
                return false;
            }
            DB.setDatabaseName(dbName);
            if (!DB.open()) {
                Utils::warningMessageBox(tr("Unable to create the Protocol database."),tr("Please contact dev team."));
                return false;
            }
            DB.setDatabaseName(dbName);
        }
        if (QSqlDatabase::connectionNames().contains("CREATOR"))
            QSqlDatabase::removeDatabase("CREATOR");
        DB.open();
        setDriver(Utils::Database::MySQL);
    }

    // create db structure
    // before we need to inform Utils::Database of the connectionName to use
    setConnectionName(connectionName);

    // The SQL scheme MUST BE synchronized with the Dosages::Constants Model Enumerator !!!
    if (executeSQL(QStringList()
        << dosageCreateTableSqlQuery().remove("AUTOINCREMENT")
        << "CREATE TABLE IF NOT EXISTS `VERSION` ("
           "`ACTUAL`                varchar(10)    NULL"
           ");"
        << QString("INSERT INTO `VERSION` (`ACTUAL`) VALUES('%1');").arg(VersionUpdater::instance()->lastDosageDabaseVersion())
        , DB)) {
        Utils::Log::addMessage(this, tkTr(Trans::Constants::DATABASE_1_CORRECTLY_CREATED).arg(dbName));
        return true;
    } else {
        Utils::Log::addError(this, tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2)
                         .arg(dbName, DB.lastError().text()));
    }
    return false;
}

void DrugsBase::onCoreDatabaseServerChanged()
{
    refreshDosageBase();
}

/**
  \brief Check the version of the doage database. Do the necessary updates for that database according to the application version number.
  Added from freediams version 0.0.8 stable
*/
void DrugsBase::checkDosageDatabaseVersion()
{
    if (!DrugsDB::VersionUpdater::instance()->isDosageDatabaseUpToDate())
        if (!DrugsDB::VersionUpdater::instance()->updateDosageDatabase())
            Utils::Log::addError(this, "Dosage database can not be correctly updated");
}

/** \brief Returns hash that contains dosage uuid has key and the xml'd dosage to transmit as value */
QHash<QString, QString> DrugsBase::getDosageToTransmit()
{
    QHash<QString, QString> toReturn;
    QSqlDatabase DB = QSqlDatabase::database(Dosages::Constants::DB_DOSAGES_NAME);
    if (!DB.open()) {
        Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                             .arg(Dosages::Constants::DB_DOSAGES_NAME).arg(DB.lastError().text()));
        return toReturn;
    }
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
            Utils::Log::addQueryError(this, query);
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
            Utils::Log::addQueryError(this, query);
        }
    }
    return toReturn;
}

/** Marks all dosages as transmitted now. \e dosageUuids contains dosage uuid. */
bool DrugsBase::markAllDosageTransmitted(const QStringList &dosageUuids)
{
    if (dosageUuids.count()==0)
        return true;
    QSqlDatabase DB = QSqlDatabase::database(Dosages::Constants::DB_DOSAGES_NAME);
    if (!DB.open()) {
        Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                             .arg(Dosages::Constants::DB_DOSAGES_NAME).arg(DB.lastError().text()));
        return false;
    }
    QStringList reqs;
    foreach(const QString &s, dosageUuids) {
        QString req = QString("UPDATE `DOSAGE` SET `TRANSMITTED`='%1' WHERE %2")
                      .arg(QDateTime::currentDateTime().toString(Qt::ISODate))
                      .arg(QString("`POSO_UUID`='%1'").arg(s));
        reqs << req;
    }
    if (!executeSQL(reqs,DB)) {
        Utils::Log::addError(this, tr("Unable to update the protocol's transmission date."));
        return false;
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

/**
  \brief Returns all CIS that have a recorded dosage. Manages INN dosage type.
  \todo put this in a thread...
*/
QList<int> DrugsBase::getAllUIDThatHaveRecordedDosages() const
{
    QList<int> toReturn;

    if (!actualDatabaseInformations())
        return toReturn;

    QSqlDatabase DosageDB = QSqlDatabase::database(Dosages::Constants::DB_DOSAGES_NAME);
    if ((DosageDB.isOpen()) && (!DosageDB.open())) {
        Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                             .arg(Dosages::Constants::DB_DOSAGES_NAME).arg(DosageDB.lastError().text()));
        return toReturn;
    }
    QString req;
    if (m_IsDefaultDB) {
        req = QString("SELECT DISTINCT CIS_LK FROM `DOSAGE` WHERE `DRUGS_DATABASE_IDENTIFIANT` = \"%1\";")
              .arg(Constants::DB_DEFAULT_IDENTIFIANT);
    } else {
        req = QString("SELECT DISTINCT CIS_LK FROM `DOSAGE` WHERE `DRUGS_DATABASE_IDENTIFIANT` = \"%1\";")
              .arg(actualDatabaseInformations()->identifiant);
    }
    {
        QSqlQuery query(req,DosageDB);
        if (query.isActive()) {
            while (query.next()) {
                toReturn << query.value(0).toInt();
            }
        } else {
            Utils::Log::addQueryError(this, query);
        }
    }

    // Get all CIS that contains INN that have available dosage
    QMultiHash<int, QString> inn_dosageRef = getAllINNThatHaveRecordedDosages();

    // get all code_subst from INNs
    QHash<int, QString> where;
    QString tmp;
    QList<int> code_subst;
    QSqlDatabase DrugsDB = QSqlDatabase::database(Constants::DB_DRUGS_NAME);
    if ((DrugsDB.isOpen()) && (!DrugsDB.open())) {
        Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                             .arg(Constants::DB_DRUGS_NAME).arg(DrugsDB.lastError().text()));
        return toReturn;
    }

    // get all needed datas from database
    QMultiHash<int, minimalCompo> cis_compo;
    foreach(int inn, inn_dosageRef.keys()) {
//            foreach(int code, d->m_Lk_iamCode_substCode.values(inn)) {
            foreach(int code, getLinkedMoleculeCodes(inn)) {
            if (!code_subst.contains(code))
                code_subst << code;
        }
    }
    foreach(int code, code_subst) {
        tmp += QString::number(code) + ", ";
    }
    tmp.chop(2);

    // Get all Drugs UID that contains the substance + dosage
    where.clear();
    req.clear();
    where.insert(Constants::COMPO_MOL_CODE, QString("IN (%1)").arg(tmp));
    req = select(Constants::Table_COMPO,
                 QList<int>() << Constants::COMPO_UID
                 << Constants::COMPO_MOL_CODE
                 << Constants::COMPO_DOSAGE,
                 where);
    QSqlQuery query(req, DrugsDB);
    if (query.isActive()) {
        while (query.next()) {
            int cis = query.value(0).toInt();
            minimalCompo compo;
//            compo.inn = d->m_Lk_iamCode_substCode.key(query.value(1).toInt());
            compo.inn = getLinkedAtcIds(query.value(1).toInt()).at(0);
            compo.dosage = query.value(2).toString();
            cis_compo.insertMulti(cis, compo);
        }
    } else {
        Utils::Log::addQueryError(this, query);
    }

    // now check every drugs
    foreach(const int cis, cis_compo.uniqueKeys()) {
        QList<int> innsOfThisDrug;
        foreach(const minimalCompo &compo, cis_compo.values(cis)) {
            if (!innsOfThisDrug.contains(compo.inn))
                innsOfThisDrug << compo.inn;
            QString d = compo.dosage;
            foreach(const QString &r, inn_dosageRef.values(compo.inn)) {
                // remove unneeded strings
                if (d == r)
                    toReturn << cis;
                else if (d.remove(",000") == r)
                    toReturn << cis;
                else if (d.remove(",00") == r)
                    toReturn << cis;

                // try unit conversion
                if (d.replace("000 mg", " g") == r) {
                    toReturn << cis;
                }
                QString t = r;
                if (t.replace("000 mg", " g") == d) {
                    toReturn << cis;
                }
            }
        }
        if (innsOfThisDrug.count() > 1)
            toReturn.removeAll(cis);
    }
    return toReturn;
}

QMultiHash<int,QString> DrugsBase::getAllINNThatHaveRecordedDosages() const
{
    QMultiHash<int,QString> toReturn;
    QSqlDatabase DB = QSqlDatabase::database(Dosages::Constants::DB_DOSAGES_NAME);
    if (!DB.open()) {
        Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                             .arg(Dosages::Constants::DB_DOSAGES_NAME).arg(DB.lastError().text()));
        return toReturn;
    }
    QString req;
    if (m_IsDefaultDB) {
        req = QString("SELECT DISTINCT `INN_LK`, `INN_DOSAGE` FROM `DOSAGE` WHERE `DRUGS_DATABASE_IDENTIFIANT` = \"%1\";")
              .arg(Constants::DB_DEFAULT_IDENTIFIANT);
    } else {
        req = QString("SELECT DISTINCT `INN_LK`, `INN_DOSAGE` FROM `DOSAGE` WHERE `DRUGS_DATABASE_IDENTIFIANT` = \"%1\";")
              .arg(actualDatabaseInformations()->identifiant);
    }
    {
        QSqlQuery query(req,DB);
        if (query.isActive()) {
            while (query.next()) {
                toReturn.insertMulti(query.value(0).toInt(), query.value(1).toString());
            }
        } else {
            Utils::Log::addQueryError(this, query);
        }
    }
    return toReturn;
}


//--------------------------------------------------------------------------------------------------------
//----------------------------------------- Managing Link Tables -----------------------------------------
//--------------------------------------------------------------------------------------------------------

/** \brief Return true if all the INNs of the drug are known. */
bool DrugsBase::drugsINNIsKnown(const DrugsData *drug)
{
    const QList<int> &list = drug->listOfCodeMolecules(); //->value(Table_COMPO, COMPO_MOL_CODE).toList();
    foreach(QVariant q, list)
        if (getLinkedAtcIds(q.toInt()).count() == 0)
//            if (d->m_Lk_iamCode_substCode.key(q.toInt()).count() == 0)
            return false;
    return true;
}

//--------------------------------------------------------------------------------------------------------
//-------------------------------- Retreive drugs from database ------------------------------------------
//--------------------------------------------------------------------------------------------------------
/** \brief Returns the unique code CIS for the CIP code \e CIP. */
int DrugsBase::getUIDFromCIP(int CIP)
{
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_DRUGS_NAME);
    if (!DB.isOpen())
        DB.open();
    // prepare where clause
    QHash<int, QString> where;
    where.insert(PACK_PACK_UID, "=" + QString::number(CIP));
    // prepare query
    QString req = this->select(Table_PACKAGING, PACK_DRUG_UID, where);
    QSqlQuery q(req , DB);
    if (q.isActive())
        if (q.next())
            return q.value(0).toInt();
    return -1;
}

/** \brief Retrieve and return the drug designed by the CIP code \e CIP_id. This function is not used. */
DrugsData *DrugsBase::getDrugByCIP(const QVariant & CIP_id)
{
    // retreive corresponding CIS
    int u = getUIDFromCIP(CIP_id.toInt());
    if (u == -1) return 0;

    // retreive drugs from CIS
    return getDrugByUID(u);
}

/** \brief Retrieve and return the drug designed by the UID code \e drug_UID. */
DrugsData *DrugsBase::getDrugByUID(const QVariant &drug_UID)
{
    if (!d->m_ActualDBInfos) {
        DrugsData * toReturn = new DrugsData;
        toReturn->setValue(Table_DRUGS, DRUGS_NAME, QString("No drugs database loaded."));
        return toReturn;
    }
    QTime t;
    t.start();

    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_DRUGS_NAME);
    if ((!DB.open()) && (!DB.isOpen())) {
        Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                             .arg(Constants::DB_DRUGS_NAME).arg(DB.lastError().text()));
        return 0;
    }

    // construct the where clause
    QHash<int, QString> where;
    int newUID = drug_UID.toInt();
    if (newUID == -1) {
        QString req = select(Table_DRUGS, DRUGS_UID, where);
        req = req.remove("WHERE ") + " LIMIT 1";
        QSqlQuery q(req,DB);
        if (q.isActive()) {
            if (q.next()) {
                newUID = q.value(0).toInt();
            }
        } else {
            Utils::Log::addError(this, "Can find a valid DRUGS_UID in getDrugByUID where uid==-1");
            Utils::Log::addQueryError("DrugsBase", q);
            return 0;
        }
    }
    where.insert(DRUGS_UID, QString("=%1").arg(newUID));

    // get DRUGS table
    QString req = select(Table_DRUGS, where);
    DrugsData *toReturn = 0;
    {
        QSqlQuery q(req , DB);
        if (q.isActive()) {
            if (q.next()) {
                if (q.record().count() != DRUGS_MaxParam)
                    Utils::Log::addError("DrugsBase", QCoreApplication::translate("DrugsBase",
                                                                                  "ERROR : will retreiving %1. Wrong number of fields")
                                         .arg("DRUGS table"));
                int i = 0;
                toReturn = new DrugsData();
                for (i = 0; i < DRUGS_MaxParam; ++i)
                    toReturn->setValue(Table_DRUGS, i, q.value(i));
            }
            // manage drugs denomination according to the database informations
            if (d->m_ActualDBInfos) {
                QString tmp = d->m_ActualDBInfos->drugsNameConstructor;
                if (!tmp.isEmpty()) {
                    tmp.replace(field(Table_DRUGS, DRUGS_NAME), toReturn->denomination());
                    tmp.replace(field(Table_DRUGS, DRUGS_FORM), toReturn->form());
                    tmp.replace(field(Table_DRUGS, DRUGS_ROUTE), toReturn->route());
                    // limit strength to three maximum --> if > 3 do not add strength
                    if (toReturn->strength().count(";") >= 3)
                        tmp.replace(field(Table_DRUGS, DRUGS_STRENGTH), "");
                    else
                        tmp.replace(field(Table_DRUGS, DRUGS_STRENGTH), toReturn->strength());
                    toReturn->setValue(Table_DRUGS, DRUGS_NAME, tmp);
                }
            }
        } else {
            Utils::Log::addQueryError(this, q);
        }
    }

    // get COMPO table
    where.clear();
    where.insert(COMPO_UID, QString("=%1").arg(newUID));
    QString sort = QString(" ORDER BY %1 ASC").arg(field(Table_COMPO,COMPO_LK_NATURE));
    req = select(Table_COMPO, where) + sort;
    QSet<int> codeMols;
    {
        DrugComposition *compo = 0;
        DrugComposition *precedent = 0;
        QSqlQuery q(req , DB);
        if (q.isActive()) {
            QList<DrugComposition *> list;
            while (q.next()) {
                if (q.record().count() != COMPO_MaxParam)
                    Utils::Log::addError("DrugsBase", QCoreApplication::translate("DrugsBase",
                                                                                  "ERROR : will retreiving %1. Wrong number of fields")
                                         .arg("COMPOSITION table"));
                compo = new DrugComposition();
                int i = 0;
                for (i = 0; i < COMPO_MaxParam; ++i)
                    compo->setValue(i, q.value(i));
                compo->setValue(COMPO_IAM_DENOMINATION, getInnDenominationFromSubstanceCode(compo->m_CodeMolecule));
                compo->setValue(COMPO_IAM_CLASS_DENOMINATION, getIamClassDenomination(compo->m_CodeMolecule));
                compo->setInnCode(getInnCodeForCodeMolecule(q.value(COMPO_MOL_CODE).toInt()));
                list << compo;
                codeMols << q.value(COMPO_MOL_CODE).toInt();
                if (precedent) {
                    if (q.value(COMPO_LK_NATURE) == precedent->linkId()) {
                        compo->setLinkedSubstance(list.at(list.count()-2));
                    }
                }
                precedent = compo;
            }
            foreach(DrugComposition *c, list)
                toReturn->addComposition(c);
        } else {
            Utils::Log::addQueryError(this, q);
        }
    }
    foreach(const int i, codeMols) {
        toReturn->addInnAndIamClasses(getAllInnAndIamClassesIndex(i)) ;
    }

//    if (toReturn)
//        toReturn->warn();

    if (d->m_LogChrono)
        Utils::Log::logTimeElapsed(t, "DrugsBase", "getDrugByUID");

    return toReturn;
}

QStringList DrugsBase::getDrugCompositionAtcCodes(const int uid)
{
    /** \todo create a cache */
    QStringList toReturn;
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_DRUGS_NAME);
    if (!DB.open()) {
        Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                             .arg(Constants::DB_DRUGS_NAME).arg(DB.lastError().text()));
        return toReturn;
    }
    // get all molecule_code for the drug
    QHash<int, QString> where;
    where.insert(Constants::COMPO_UID, QString("='%1'").arg(uid));
    QString req = select(Constants::Table_COMPO, Constants::COMPO_MOL_CODE, where);
    QList<int> molecule_codes;
    {
        QSqlQuery query(req, DB);
        if (query.isActive()) {
            while (query.next()) {
                molecule_codes.append(query.value(0).toInt());
            }
        }
    }
    foreach(int id, getLinkedAtcIds(molecule_codes)) {
        const QString &n = getAtcCode(id);
        if (!toReturn.contains(n))
            toReturn.append(n);
    }
    return toReturn;
}

QStringList DrugsBase::getDrugInns(const QVariant &uid)
{
    QStringList toReturn;
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_DRUGS_NAME);
    if (!DB.open()) {
        Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                             .arg(Constants::DB_DRUGS_NAME).arg(DB.lastError().text()));
        return toReturn;
    }
    // get all code_subst for the drug
    QHash<int, QString> where;
    where.insert(Constants::COMPO_UID, QString("='%1'").arg(uid.toString()));
    QString req = select(Constants::Table_COMPO, Constants::COMPO_MOL_CODE, where);
    QList<int> codes;
    {
        QSqlQuery query(req, DB);
        if (query.isActive()) {
            while (query.next()) {
                codes.append(query.value(0).toInt());
            }
        }
    }
    foreach(int id, codes) {
        const QString &n = getInnDenominationFromSubstanceCode(id);
        if (!toReturn.contains(n))
            toReturn.append(n);
    }
    return toReturn;
}
