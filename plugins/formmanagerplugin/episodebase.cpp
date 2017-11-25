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
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \brief Create fmf_episodes database
 *
 */

#include "episodebase.h"
#include "constants_db.h"
#include "iformio.h"
#include "subforminsertionpoint.h"
#include "episodedata.h"

#include <utils/global.h>
#include <utils/log.h>
#include <utils/databaseconnector.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>
#include <translationutils/trans_database.h>
#include <translationutils/trans_msgerror.h>

#include <coreplugin/isettings.h>
#include <coreplugin/icore.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/iuser.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/icommandline.h>

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QDir>
#include <QProgressDialog>
#include <QTreeWidgetItem>
#include <QFont>

using namespace Form;
using namespace Internal;
using namespace Constants;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::IUser *user() {return Core::ICore::instance()->user();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline Core::ICommandLine *commandLine()  { return Core::ICore::instance()->commandLine(); }

static inline bool connectDatabase(QSqlDatabase &DB, const int line)
{
    if (!DB.isOpen()) {
        if (!DB.open()) {
            Utils::Log::addError("EpisodeBase", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                 .arg(DB.connectionName()).arg(DB.lastError().text()),
                                 __FILE__, line);
            return false;
        }
    }
    return true;
}

//namespace Form {
//namespace Internal {
//class EpisodeBasePrivate
//{
//public:
//    EpisodeBasePrivate(EpisodeBase *parent = 0) : q(parent) {}
//    ~EpisodeBasePrivate () {}
//
//    void checkDatabaseVersion()
//    {}
//
//private:
//    EpisodeBase *q;
//};
//}
//}

EpisodeBase *EpisodeBase::m_Instance = 0;

EpisodeBase *EpisodeBase::instance()
{
    Q_ASSERT(m_Instance);
    return m_Instance;
}

EpisodeBase::EpisodeBase(QObject *parent) :
    QObject(parent), Utils::Database(),
    m_initialized(false)
{
    m_Instance = this;
    setObjectName("EpisodeBase");

    using namespace Form::Constants;

    addTable(Table_EPISODES, "EPISODES");
    addTable(Table_EPISODES_MODIF, "MODIF_TRACE");
    addTable(Table_VALIDATION, "VALIDATION");
    addTable(Table_EPISODE_CONTENT, "EPISODES_CONTENT");
    addTable(Table_FORM, "FORM_FILES");
    addTable(Table_SCHEMA, "SCHEMA_CHANGES");

    addField(Table_EPISODES, EPISODES_ID, "EPISODE_ID", FieldIsUniquePrimaryKey);
    addField(Table_EPISODES, EPISODES_PATIENT_UID, "PATIENT_UID", FieldIsUUID);
    addField(Table_EPISODES, EPISODES_LK_TOPRACT_LKID, "LK_TOPRACT_LKID", FieldIsInteger);
    addField(Table_EPISODES, EPISODES_ISVALID, "ISVALID", FieldIsBoolean);
    addField(Table_EPISODES, EPISODES_FORM_PAGE_UID, "FORM_PAGE_UID", FieldIsShortText);
    addField(Table_EPISODES, EPISODES_LABEL, "LABEL", FieldIsShortText);
    addField(Table_EPISODES, EPISODES_USERDATETIME, "USERDATE", FieldIsIsoUtcDateTime);
    addField(Table_EPISODES, EPISODES_EPISODECREATIONDATETIME,
             "DATECREATION", FieldIsIsoUtcDateTime);
    addField(Table_EPISODES, EPISODES_USERCREATOR, "CREATOR", FieldIsUUID);
    addField(Table_EPISODES, EPISODES_PRIORITY, "PRIOR", FieldIsInteger, "1"); // Medium
    addIndex(Table_EPISODES, EPISODES_ID);
    addIndex(Table_EPISODES, EPISODES_PATIENT_UID);
    addIndex(Table_EPISODES, EPISODES_FORM_PAGE_UID, 190);

    addField(Table_EPISODES_MODIF, EP_MODIF_ID, "MOD_ID", FieldIsUniquePrimaryKey);
    addField(Table_EPISODES_MODIF, EP_MODIF_EPISODE_ID, "EPISODE_ID", FieldIsInteger);
    addField(Table_EPISODES_MODIF, EP_MODIF_DATE, "DATE", FieldIsDate);
    addField(Table_EPISODES_MODIF, EP_MODIF_USERUID, "USERUID", FieldIsUUID);
    addField(Table_EPISODES_MODIF, EP_MODIF_TRACE, "TRACE", FieldIsBlob);
    addIndex(Table_EPISODES_MODIF, EP_MODIF_ID);
    addIndex(Table_EPISODES_MODIF, EP_MODIF_EPISODE_ID);
    addIndex(Table_EPISODES_MODIF, EP_MODIF_USERUID);

    addField(Table_VALIDATION, VALIDATION_ID, "VAL_ID", FieldIsUniquePrimaryKey);
    addField(Table_VALIDATION, VALIDATION_EPISODE_ID, "EPISODE_ID", FieldIsInteger);
    addField(Table_VALIDATION, VALIDATION_DATEOFVALIDATION, "VAL_DATE", FieldIsDate);
    addField(Table_VALIDATION, VALIDATION_USERUID, "VAL_USERUUID", FieldIsUUID);
    addField(Table_VALIDATION, VALIDATION_ISVALID, "VAL_ISVALID", FieldIsBoolean);
    addIndex(Table_VALIDATION, VALIDATION_ID);
    addIndex(Table_VALIDATION, VALIDATION_EPISODE_ID);

    addField(Table_EPISODE_CONTENT, EPISODE_CONTENT_ID, "CONTENT_ID", FieldIsUniquePrimaryKey);
    addField(Table_EPISODE_CONTENT, EPISODE_CONTENT_EPISODE_ID, "EPISODE_ID", FieldIsLongInteger);
    addField(Table_EPISODE_CONTENT, EPISODE_CONTENT_XML, "XML_CONTENT", FieldIsBlob);
    addIndex(Table_EPISODE_CONTENT, EPISODE_CONTENT_ID);
    addIndex(Table_EPISODE_CONTENT, EPISODE_CONTENT_EPISODE_ID);

    addField(Table_FORM, FORM_ID,      "ID", FieldIsUniquePrimaryKey);
    addField(Table_FORM, FORM_VALID,   "VALID", FieldIsBoolean);
    addField(Table_FORM, FORM_GENERIC, "GENERIC", FieldIsShortText);
    addField(Table_FORM, FORM_PATIENTUID, "PATIENT", FieldIsUUID);
    // Uuid of the form to insert
    addField(Table_FORM, FORM_SUBFORMUID, "SUBUID", FieldIsShortText);
    // Insertion point = formuuid where to insert the form
    addField(Table_FORM, FORM_INSERTIONPOINT, "IP", FieldIsShortText);
    addField(Table_FORM, FORM_INSERTASCHILD, "CHILD", FieldIsBoolean, "true");
    addField(Table_FORM, FORM_APPEND, "APPEND", FieldIsBoolean, "false");
    // TODO: code here : manage user access restriction Table_FORM, FORM_USER_RESTRICTION_ID
    addField(Table_FORM, FORM_USER_RESTRICTION_ID, "USER_REST_ID", FieldIsInteger, "NULL");
    addIndex(Table_FORM, FORM_ID);
    addIndex(Table_FORM, FORM_PATIENTUID);
    addIndex(Table_FORM, FORM_SUBFORMUID, 190);
    addIndex(Table_FORM, FORM_INSERTIONPOINT, 190);

    // fields for table SCHEMA_CHANGES
    addField(Table_SCHEMA, SCHEMA_ID, "ID", FieldIsUniquePrimaryKey);
    addField(Table_SCHEMA, SCHEMA_VERSION, "VERSION_NUMBER", FieldIsInteger);
    addField(Table_SCHEMA, SCHEMA_SCRIPT, "SCRIPT_NAME", FieldIs255Chars);
    addField(Table_SCHEMA, SCHEMA_TIMESTAMP, "TIMESTAMP_UTC_APPLIED", FieldIs19Chars);
    addIndex(Table_SCHEMA, SCHEMA_ID);

    // Connect first run database creation requested
    connect(Core::ICore::instance(), SIGNAL(firstRunDatabaseCreation()), this, SLOT(onCoreFirstRunCreationRequested()));
}

EpisodeBase::~EpisodeBase()
{
//    if (d) {
//        delete d;
//        d = 0;
//    }
}

bool EpisodeBase::initialize()
{
    // only one base can be initialized
    if (m_initialized)
        return true;

    // connect
    if (commandLine()->value(Core::ICommandLine::ClearUserDatabases).toBool()) {
        createConnection(DB_NAME, DB_NAME,
                         settings()->databaseConnector(),
                         Utils::Database::DeleteAndRecreateDatabase);
    } else {
        createConnection(DB_NAME, DB_NAME,
                         settings()->databaseConnector(),
                         Utils::Database::CreateDatabase);
    }

    if (!database().isOpen()) {
        if (!database().open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(DB_NAME).arg(database().lastError().text()));
        } else {
            LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().databaseName()).arg(database().driverName()));
        }
    } else {
        LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().databaseName()).arg(database().driverName()));
    }

    int currentDatabaseVersion = getSchemaVersionNumber(Constants::DB_NAME);
    if (currentDatabaseVersion != Constants::DB_CURRENT_CODE_VERSION) {
        if(!updateDatabase()) {
            LOG_ERROR(QString("Couldn't upgrade database %1").arg(Constants::DB_NAME));
            return false;
        }
        initialize();
    }

    if (!checkDatabaseScheme()) {
        LOG_ERROR(tkTr(Trans::Constants::DATABASE_1_SCHEMA_ERROR).arg(DB_NAME));
        return false;
    }

    connect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
    m_initialized = true;
    return true;
}

bool EpisodeBase::createDatabase(const QString &connectionName , const QString &dbName,
                    const QString &pathOrHostName,
                    TypeOfAccess access, AvailableDrivers driver,
                    const QString & login, const QString & pass,
                    const int port,
                    CreationOption createOption
                   )
{
    Q_UNUSED(access);
    Q_UNUSED(createOption);
    // TODO: manage access
    // TODO: manage createOption

    if (connectionName != DB_NAME)
        return false;

    LOG(tkTr(Trans::Constants::TRYING_TO_CREATE_1_PLACE_2)
        .arg(dbName).arg(pathOrHostName));

    // create an empty database and connect
    QSqlDatabase DB;
    if (driver == SQLite) {
        DB = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        if (!QDir(pathOrHostName).exists())
            if (!QDir().mkpath(pathOrHostName))
                LOG(tkTr(Trans::Constants::_1_ISNOT_AVAILABLE_CANNOTBE_CREATED).arg(pathOrHostName));
        DB.setDatabaseName(QDir::cleanPath(pathOrHostName + QDir::separator() + dbName));
        if (!DB.open())
            LOG(tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2).arg(dbName).arg(DB.lastError().text()));
        setDriver(Utils::Database::SQLite);
    }
    else if (driver == MySQL) {
        DB = QSqlDatabase::database(connectionName);
        if (!DB.open()) {
            QSqlDatabase d = QSqlDatabase::addDatabase("QMYSQL", "__EPISODE_CREATOR");
            d.setHostName(pathOrHostName);
            d.setUserName(login);
            d.setPassword(pass);
            d.setPort(port);
            if (!d.open()) {
                Utils::warningMessageBox(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                         .arg(DB.connectionName()).arg(DB.lastError().text()),
                                         tkTr(Trans::Constants::CONTACT_DEV_TEAM));
                return false;
            }
            QSqlQuery q(QString("CREATE DATABASE `%1`").arg(dbName), d);
            if (!q.isActive()) {
                LOG_QUERY_ERROR(q);
                Utils::warningMessageBox(tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2)
                                         .arg(DB.connectionName()).arg(DB.lastError().text()),
                                         tkTr(Trans::Constants::CONTACT_DEV_TEAM));
                return false;
            }
            if (!DB.open()) {
                Utils::warningMessageBox(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                         .arg(DB.connectionName()).arg(DB.lastError().text()),
                                         tkTr(Trans::Constants::CONTACT_DEV_TEAM));
                return false;
            }
            DB.setDatabaseName(dbName);
        }
        if (QSqlDatabase::connectionNames().contains("__EPISODE_CREATOR"))
            QSqlDatabase::removeDatabase("__EPISODE_CREATOR");
        if (!DB.open()) {
            Utils::warningMessageBox(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                     .arg(DB.connectionName()).arg(DB.lastError().text()),
                                     tkTr(Trans::Constants::CONTACT_DEV_TEAM));
            return false;
        }
        setDriver(Utils::Database::MySQL);
    }

    // create db structure
    // before we need to inform Utils::Database of the connectionName to use
    setConnectionName(connectionName);

    if (createTables()) {
        LOG(tkTr(Trans::Constants::DATABASE_1_CORRECTLY_CREATED).arg(dbName));
    } else {
        LOG_ERROR(tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2)
                  .arg(dbName, DB.lastError().text()));
        return false;
    }
    // Add version number
    if (!setSchemaVersion(Constants::DB_CURRENT_CODE_VERSION, Constants::DB_NAME)) {
        LOG_ERROR(QString("Couldn't set schema version for database %1").arg(Constants::DB_NAME));
        return false;
    }
    populateWithDefaultValues();

    return true;
}

int EpisodeBase::currentDatabaseVersion() const
{
int currentDatabaseVersion = Database::getSchemaVersionNumber(Constants::DB_NAME);
if (currentDatabaseVersion > 0)
    return currentDatabaseVersion;

QString oldVersion = getOldVersionField();
bool ok;
if (!oldVersion.isEmpty()) {
    if (oldVersion.contains(".")) {
        currentDatabaseVersion = 0;
    } else {
    currentDatabaseVersion = oldVersion.toInt(&ok, 10);
    }
}
return currentDatabaseVersion;
}

/**
 * Update episode database
 * Old versioning (fhio version <= 0.10): version string = "0.1" or "0.2"
 * New versioning (fhio version >= 0.11.0): The version number is an integer,
 * starting from 3 for fhio version 0.11.0
 * The field VERSION_NUMBER of the last row of the table SCHEMA_CHANGES.
 * By definition, this number must be a positive, non null integer.
 * This function will run all sql update scripts to update the database to
 * the current code version
 * Update script 1 takes care of update from version 0.1 to version 1
 * Update from 0.1 to 0.2 is not necessary because the change it introduced was
 * erased by a subsequent update.
 * Script 2 takes care of update from version 1 to 2 and so on and so forth.
 */
bool EpisodeBase::updateDatabase()
{
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_NAME);
    QString updateScriptFileName;
    int currentVersion = currentDatabaseVersion();
    currentVersion++;
    for (int i = currentVersion; i <= Constants::DB_CURRENT_CODE_VERSION; i++) {
        if (driver()==MySQL) {
            updateScriptFileName= QString(":/sql/update/update%1%2.sql")
                    .arg(Constants::DB_NAME)
                    .arg(QString::number(i));
        } else if (driver()==SQLite) {
            updateScriptFileName= QString(":/sql/update/updatesqlite%1%2.sql")
                    .arg(Constants::DB_NAME)
                    .arg(QString::number(i));
        }
        QFile updateScriptFile(updateScriptFileName);
        if(!executeQueryFile(updateScriptFile, DB)) {
            LOG_ERROR(QString("Error during update of database %1 with update script %2").arg(Constants::DB_NAME).arg(updateScriptFileName));
            return false;
        }
    }
    return true;
}

/**
 * @brief AlertBase::getOldVersionField return the old style database version
 * @return string representing the old style version number
 */
QString EpisodeBase::getOldVersionField() const
{
    WARN_FUNC;
    QString oldVersionValue;
    QSqlDatabase DB = QSqlDatabase::database(DB_NAME);
    if (!connectDatabase(DB, __LINE__)) {
        LOG_ERROR("Can't connect to DB");
        return QString();
    }
    QSqlQuery query("SELECT `VERSION` FROM `VERSION` ORDER BY `VERSION` ASC LIMIT 1", DB);
    int fieldNo = query.record().indexOf("VERSION");
    while (query.next()) {
        oldVersionValue = query.value(fieldNo).toString();
    }
    return oldVersionValue;
}

/** Populate the database with the default value after its creation. */
void EpisodeBase::populateWithDefaultValues()
{
    // set default patient FormFile
    setGenericPatientFormFile(QString("%1/%2")
                              .arg(Core::Constants::TAG_APPLICATION_COMPLETEFORMS_PATH)
                              .arg(Core::Constants::S_DEF_PATIENTFORMS_FILENAME));
}

/**
 * Checks the current version of the database.
 * Returns \e true if the version is the latest one.

bool EpisodeBase::checkDatabaseVersion()
{
    Utils::Field vField(Constants::Table_VERSION, Constants::VERSION_TEXT);
    QString currentVersion = getVersion(vField);
    if (currentVersion == Constants::DB_CURRENTVERSION) {
        return true;
    }
    // Updates from 0.1 to 0.2
    if (currentVersion == "0.1") {
        if (!alterTableForNewField(Constants::Table_EPISODES, Constants::EPISODES_PRIORITY))
            return false;
        setVersion(vField, "0.2");
        LOG(tr("Episode database updated from version %1 to version: %2")
            .arg("0.1")
            .arg("0.2"));
    }

    // Update from version 0.2 to version 1
    if (currentVersion == "0.2") {
        QSqlDatabase db = QSqlDatabase::database(DB_NAME);
        if (!db.isOpen()) {                                                     
            if (!db.open()) {                                                   
                Utils::Log::addError("Update of episode database", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                     .arg(db.connectionName()).arg(db.lastError().text()),
                                     __FILE__, __LINE__);                       
                return false;                                                   
            }                                                                   
        }                                                                       
        if (db.driverName()=="QMYSQL") {
            QString req = "ALTER TABLE EPISODES MODIFY USERDATE TIMESTAMP DEFAULT CURRENT_TIMESTAMP;";
            QSqlQuery q(db);
            if (!q.exec(req)) {                                                 
                LOG_QUERY_ERROR_FOR("Update database episodes from version 0.2 to 1", q);
                return false;                       
            }
        setVersion(vField, "1");
        LOG(tr("Episode database updated from version %1 to version: %2")       
            .arg("0.2")                                                         
            .arg("1"));
        }
        if (db.driverName()=="QSQLITE") {
        setVersion(vField, "1");
        LOG(tr("No change happened but episode database updated from version %1 to version: %2")       
            .arg("0.2")                                                         
            .arg("1"));
        }
    }
    // Update from version 1 to version 2
    if (currentVersion == "1") {
        QSqlDatabase db = QSqlDatabase::database(DB_NAME);
        if (!db.isOpen()) {
            if (!db.open()) {
                Utils::Log::addError("Update of episode database", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                     .arg(db.connectionName()).arg(db.lastError().text()),
                                     __FILE__, __LINE__);
                return false;
            }
        }
        if (db.driverName()=="QMYSQL") {
            QString req = "ALTER TABLE EPISODES CHANGE DATECREATION"
                          " DATECREATION VARCHAR(20);";
            QSqlQuery q(db);
            if (!q.exec(req)) {
                LOG_QUERY_ERROR(q);
                return false;
            }

            req = "ALTER TABLE EPISODES CHANGE USERDATE"
                              " USERDATE VARCHAR(20)";
            if (!q.exec(req)) {
                LOG_QUERY_ERROR(q);
                return false;
            }
        LOG(tr("Episode database updated from version %1 to version: %2")
            .arg("1")
            .arg("2"));
        }
        if (db.driverName()=="QSQLITE") {
        // SQLite can accept any type of string data, no change required
        // Name of colum remains the same (code is using ENUM)
        LOG(tr("No change happened but episode database updated from version %1 to version: %2")
            .arg("1")
            .arg("2"));
        }
    }
    // Update the database version
    return setVersion(vField, Constants::DB_CURRENTVERSION);
}
*/

void EpisodeBase::onCoreDatabaseServerChanged()
{
    m_initialized = false;
    if (QSqlDatabase::connectionNames().contains(DB_NAME)) {
        QSqlDatabase::removeDatabase(DB_NAME);
    }
    disconnect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
    disconnect(Core::ICore::instance(), SIGNAL(firstRunDatabaseCreation()), this, SLOT(onCoreFirstRunCreationRequested()));
    initialize();
}

void EpisodeBase::onCoreFirstRunCreationRequested()
{
    disconnect(Core::ICore::instance(), SIGNAL(firstRunDatabaseCreation()), this, SLOT(onCoreFirstRunCreationRequested()));
    initialize();
}

/**
 * Store the central patient form file into the database.
 * This Form File will be used for all patient as central form.
 * Some sub-forms can then be added.
 */
bool EpisodeBase::setGenericPatientFormFile(const QString &absPathOrUid)
{
    QSqlDatabase DB = QSqlDatabase::database(DB_NAME);
    if (!connectDatabase(DB, __LINE__))
        return false;
    DB.transaction();
    QHash<int, QString> where;
    where.insert(FORM_GENERIC, QString("IS NOT NULL"));
    if (count(Table_FORM, FORM_GENERIC, getWhereClause(Table_FORM, where))) {
        // update
        QSqlQuery query(DB);
        QString req = prepareUpdateQuery(Table_FORM, FORM_GENERIC, where);
        query.prepare(req);
        query.bindValue(0, absPathOrUid);
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            query.finish();
            DB.rollback();
            return false;
        }
    } else {
        // save
        QSqlQuery query(DB);
        QString req = prepareInsertQuery(Table_FORM);
        query.prepare(req);
        query.bindValue(FORM_ID, QVariant());
        query.bindValue(FORM_VALID, 1);
        query.bindValue(FORM_GENERIC, absPathOrUid);
        query.bindValue(FORM_PATIENTUID, QVariant());
        query.bindValue(FORM_SUBFORMUID, QVariant());
        query.bindValue(FORM_INSERTIONPOINT, QVariant());
        query.bindValue(FORM_INSERTASCHILD, QVariant());
        query.bindValue(FORM_APPEND, QVariant());
        query.bindValue(FORM_USER_RESTRICTION_ID, QVariant());
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            query.finish();
            DB.rollback();
            return false;
        }
    }
    DB.commit();
    return true;
}

/**
 * Return the central patient form file into the database.
 * This Form File will be used for all patient as central form.
 * Some sub-forms can then be added.
*/
QString EpisodeBase::getGenericFormFile()
{
    QSqlDatabase DB = QSqlDatabase::database(DB_NAME);
    if (!connectDatabase(DB, __LINE__)) {
        return QString();
    }
    DB.transaction();
    QHash<int, QString> where;
    where.insert(FORM_GENERIC, QString("IS NOT NULL"));
    where.insert(FORM_VALID, QString("=1"));
    QSqlQuery query(DB);
    QString req = select(Table_FORM, FORM_GENERIC, where);
    QString path;
    if (query.exec(req)) {
        if (query.next()) {
            path = query.value(0).toString();
        }
    } else {
        LOG_QUERY_ERROR(query);
        query.finish();
        DB.rollback();
        return QString();
    }
    query.finish();
    DB.commit();
    // Fix a bug introduced in 0.7.6
    if (path.endsWith("/central.xml"))
        path = path.remove("/central.xml");
    return path;
}

/**
  Return all sub-form additions.
  \sa Form::SubFormInsertionPoint, Form::FormManager::insertSubForm()
*/
QVector<Form::SubFormInsertionPoint> EpisodeBase::getSubFormFiles()
{
    QVector<SubFormInsertionPoint> toReturn;
    QSqlDatabase DB = QSqlDatabase::database(DB_NAME);
    if (!connectDatabase(DB, __LINE__)) {
        return toReturn;
    }
    Utils::FieldList where;
    where.append(Utils::Field(Table_FORM, FORM_PATIENTUID, QString("='%1'").arg(patient()->uuid())));
    where.append(Utils::Field(Table_FORM, FORM_PATIENTUID, QString("IS NULL")));
    QString whereClause = getWhereClause(where, Utils::Database::OR);
    where.clear();
    where.append(Utils::Field(Table_FORM, FORM_GENERIC, QString("IS NULL")));
    where.append(Utils::Field(Table_FORM, FORM_VALID, QString("=1")));
    whereClause = QString("(%1) AND (%2)").arg(whereClause).arg(getWhereClause(where, Utils::Database::AND));

    DB.transaction();
    QSqlQuery query(DB);
    QString req = select(Table_FORM, QList<int>()
                         << FORM_SUBFORMUID
                         << FORM_INSERTIONPOINT
                         << FORM_INSERTASCHILD
                         << FORM_APPEND) + " WHERE " + whereClause;
    if (query.exec(req)) {
        while (query.next()) {
            QString insertUid = query.value(1).toString();
            insertUid.replace(Core::Constants::TAG_APPLICATION_COMPLETEFORMS_PATH, settings()->path(Core::ISettings::CompleteFormsPath));
            insertUid.replace(Core::Constants::TAG_APPLICATION_SUBFORMS_PATH, settings()->path(Core::ISettings::SubFormsPath));
            if (insertUid.endsWith("/central.xml"))
                insertUid = insertUid.remove("/central.xml");
            QString subFormUid = query.value(0).toString();
            if (subFormUid.endsWith("/central.xml"))
                subFormUid = subFormUid.remove("/central.xml");
            SubFormInsertionPoint point(insertUid, subFormUid);
            point.setAddAsChild(query.value(2).toBool());
            point.setAppendToForm(query.value(3).toBool());
            toReturn << point;
        }
    } else {
        LOG_QUERY_ERROR(query);
        query.finish();
        DB.rollback();
        return toReturn;
    }
    query.finish();
    DB.commit();
    return toReturn;
}

/**
  Save subForm insertions to the database.
  \sa Form::SubFormInsertionPoint, Form::FormManager::insertSubForm()
*/
bool EpisodeBase::addSubForms(const QVector<SubFormInsertionPoint> &insertions)
{
    QSqlDatabase DB = QSqlDatabase::database(DB_NAME);
    if (!connectDatabase(DB, __LINE__))
        return false;
    DB.transaction();
    QSqlQuery query(DB);
    for(int i = 0; i < insertions.count(); ++i) {
        const SubFormInsertionPoint &ip = insertions.at(i);
        query.prepare(prepareInsertQuery(Table_FORM));
        query.bindValue(FORM_ID, QVariant());
        query.bindValue(FORM_VALID, 1);
        query.bindValue(FORM_GENERIC, QVariant());
        if (ip.isForAllPatients())
            query.bindValue(FORM_PATIENTUID, QVariant());
        else
            query.bindValue(FORM_PATIENTUID, patient()->uuid());
        query.bindValue(FORM_SUBFORMUID, ip.subFormUid());
        query.bindValue(FORM_INSERTIONPOINT, ip.receiverUidForDatabase());
        query.bindValue(FORM_INSERTASCHILD, int(ip.addAsChild()));
        query.bindValue(FORM_APPEND, int(ip.appendToForm()));
        query.bindValue(FORM_USER_RESTRICTION_ID, QVariant());
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            query.finish();
            DB.rollback();
            return false;
        }
        query.finish();
    }
    query.finish();
    DB.commit();
    return true;
}

/** Save or update episode modifications records for the \e episode. */
bool EpisodeBase::saveEpisodeValidations(Internal::EpisodeData *episode)
{
    if (!episode->isModified())
        return false;
    QSqlDatabase DB = QSqlDatabase::database(DB_NAME);
    if (!connectDatabase(DB, __LINE__)) {
        return false;
    }

    // we are inside a transaction
    QSqlQuery query(DB);
    QHash<int, QString> where;
    for(int i = 0; i < episode->validations().count(); ++i) {
        EpisodeValidationData &val = episode->validations()[i];
        if (!val.isModified())
            continue;
        where.clear();
        if (val.validationId()!=-1) {
            // update
            where.insert(EP_MODIF_ID, QString("=%1").arg(val.validationId()));
            query.prepare(prepareUpdateQuery(Table_VALIDATION,
                                             QList<int>()
                                             << VALIDATION_DATEOFVALIDATION
                                             << VALIDATION_USERUID
                                             << VALIDATION_ISVALID
                                             , where));
            query.bindValue(0, val.data(EpisodeValidationData::ValidationDate));
            query.bindValue(1, val.data(EpisodeValidationData::UserUid));
            query.bindValue(2, val.data(EpisodeValidationData::IsValid));
            if (!query.exec()) {
                LOG_QUERY_ERROR(query);
                return false;
            } else {
                val.setModified(false);
            }
        } else {
            // save
            query.prepare(prepareInsertQuery(Table_VALIDATION));
            query.bindValue(VALIDATION_ID, QVariant());
            query.bindValue(VALIDATION_EPISODE_ID, val.data(EpisodeValidationData::EpisodeId));
            query.bindValue(VALIDATION_DATEOFVALIDATION, val.data(EpisodeValidationData::ValidationDate));
            query.bindValue(VALIDATION_USERUID, val.data(EpisodeValidationData::UserUid));
            query.bindValue(VALIDATION_ISVALID, val.data(EpisodeValidationData::IsValid).toInt());
            if (!query.exec()) {
                LOG_QUERY_ERROR(query);
                return false;
            } else {
                val.setData(EpisodeValidationData::ValidationId, query.lastInsertId());
                val.setModified(false);
            }
            query.finish();
        }
    }
    return true;
}

/** Save or update episode modifications records for the \e episode. */
bool EpisodeBase::saveEpisodeModifications(Internal::EpisodeData *episode)
{
    if (!episode->isModified())
        return false;
    QSqlDatabase DB = QSqlDatabase::database(DB_NAME);
    if (!connectDatabase(DB, __LINE__)) {
        return false;
    }

    // we are inside a transaction
    QSqlQuery query(DB);
    QHash<int, QString> where;

    for(int i = 0; i < episode->modifications().count(); ++i) {
        EpisodeModificationData &m = episode->modifications()[i];
        if (!m.isModified())
            continue;
        where.clear();
        if (m.modificationId()!=-1) {
            // update
            where.insert(EP_MODIF_ID, QString("=%1").arg(m.modificationId()));
            query.prepare(prepareUpdateQuery(Table_EPISODES_MODIF,
                                             QList<int>()
                                             << EP_MODIF_DATE
                                             << EP_MODIF_USERUID
                                             << EP_MODIF_TRACE
                                             , where));
            query.bindValue(0, m.data(EpisodeModificationData::Date));
            query.bindValue(1, m.data(EpisodeModificationData::UserUid));
            query.bindValue(2, m.data(EpisodeModificationData::Trace));
            if (!query.exec()) {
                LOG_QUERY_ERROR(query);
                return false;
            } else {
                m.setModified(false);
            }
        } else {
            // save
            query.prepare(prepareInsertQuery(Table_EPISODES_MODIF));
            query.bindValue(EP_MODIF_ID, QVariant());
            query.bindValue(EP_MODIF_EPISODE_ID, m.data(EpisodeModificationData::EpisodeId));
            query.bindValue(EP_MODIF_USERUID, m.data(EpisodeModificationData::UserUid));
            query.bindValue(EP_MODIF_DATE, m.data(EpisodeModificationData::Date));
            query.bindValue(EP_MODIF_TRACE, m.data(EpisodeModificationData::Trace));
            if (!query.exec()) {
                LOG_QUERY_ERROR(query);
                return false;
            } else {
                m.setData(EpisodeModificationData::ModificationId, query.lastInsertId());
                m.setModified(false);
            }
        }
    }
    return true;
}

/** Save or update a Form::Internal::EpisodeData \e episode to the database. Return true if all goes fine. */
bool EpisodeBase::saveEpisode(Internal::EpisodeData *episode)
{
    return saveEpisode(QList<EpisodeData *>() << episode);
}

/** Save or update a list of Form::Internal::EpisodeData \e episodes to the
 * database. Return true if all goes fine. */
bool EpisodeBase::saveEpisode(const QList<EpisodeData *> &episodes)
{
    if (episodes.isEmpty())
        return true;

    QSqlDatabase DB = QSqlDatabase::database(DB_NAME);
    if (!connectDatabase(DB, __LINE__)) {
        return false;
    }
    DB.transaction();
    for(int i = 0; i < episodes.count(); ++i) {
        EpisodeData *episode = episodes.at(i);
        if (!episode)
            continue;
        // No patient uid? -> error
        if (episode->data(EpisodeData::PatientUuid).toString().isEmpty()) {
            LOG_ERROR("No patient Uid defined in episode. Episode not saved.");
            continue;
        }
        if (!episode->isModified())
            continue;
        if (episode->episodeId()==-1) {
            // save
            QSqlQuery query(DB);
            query.prepare(prepareInsertQuery(Table_EPISODES));
            query.bindValue(EPISODES_ID, QVariant());
            query.bindValue(EPISODES_PATIENT_UID, episode->data(EpisodeData::PatientUuid));
            query.bindValue(EPISODES_LK_TOPRACT_LKID, QVariant());
            query.bindValue(EPISODES_ISVALID, episode->data(EpisodeData::IsValid).toInt());
            query.bindValue(EPISODES_FORM_PAGE_UID, episode->data(EpisodeData::FormUuid));
            query.bindValue(EPISODES_LABEL, episode->data(EpisodeData::Label));
            query.bindValue(EPISODES_USERDATETIME, episode->data(EpisodeData::UserDate));
            query.bindValue(EPISODES_EPISODECREATIONDATETIME,
                            episode->data(EpisodeData::EpisodeCreationDateTime));
            query.bindValue(EPISODES_USERCREATOR, episode->data(EpisodeData::UserCreatorUuid));
            if (!query.exec()) {
                LOG_QUERY_ERROR(query);
                query.finish();
                DB.rollback();
                return false;
            } else {
                episode->setData(EpisodeData::Id, query.lastInsertId());
            }
            query.finish();

            // save content
            if (episode->data(EpisodeData::IsXmlContentPopulated).toBool()) {
                query.prepare(prepareInsertQuery(Table_EPISODE_CONTENT));
                query.bindValue(EPISODE_CONTENT_ID, QVariant());
                query.bindValue(EPISODE_CONTENT_EPISODE_ID, episode->data(EpisodeData::Id));
                query.bindValue(EPISODE_CONTENT_XML, episode->data(EpisodeData::XmlContent));
                if (!query.exec()) {
                    LOG_QUERY_ERROR(query);
                    query.finish();
                    DB.rollback();
                    return false;
                } else {
                    episode->setData(EpisodeData::ContentId, query.lastInsertId());
                }
                query.finish();
            }

            // save validation
            if (!saveEpisodeValidations(episode)) {
                DB.rollback();
                return false;
            }

            // save modifications (there should be no modifications at this point)
            if (!saveEpisodeModifications(episode)) {
                DB.rollback();
                return false;
            }

            episode->setModified(true);
        } else {
            // update
            QSqlQuery query(DB);
            QHash<int, QString> where;
            where.insert(EPISODES_ID, QString("=%1").arg(episode->episodeId()));
            query.prepare(prepareUpdateQuery(Table_EPISODES,
                                             QList<int>()
                                             << EPISODES_PATIENT_UID
                                             << EPISODES_ISVALID
                                             << EPISODES_FORM_PAGE_UID
                                             << EPISODES_LABEL
                                             << EPISODES_USERDATETIME
                                             << EPISODES_EPISODECREATIONDATETIME
                                             << EPISODES_USERCREATOR
                                             , where));
            query.bindValue(0, episode->data(EpisodeData::PatientUuid));
            query.bindValue(1, episode->data(EpisodeData::IsValid).toInt());
            query.bindValue(2, episode->data(EpisodeData::FormUuid));
            query.bindValue(3, episode->data(EpisodeData::Label));
            query.bindValue(4, episode->data(EpisodeData::UserDate));
            query.bindValue(5, episode->data(EpisodeData::EpisodeCreationDateTime));
            query.bindValue(6, episode->data(EpisodeData::UserCreatorUuid));
            if (!query.exec()) {
                LOG_QUERY_ERROR(query);
                query.finish();
                DB.rollback();
                return false;
            }
            query.finish();

            where.clear();
            if (episode->data(EpisodeData::IsXmlContentPopulated).toBool()) {
                where.insert(EPISODE_CONTENT_EPISODE_ID, QString("=%1").arg(episode->episodeId()));
                query.prepare(prepareUpdateQuery(Table_EPISODE_CONTENT,
                                                 QList<int>()
                                                 << EPISODE_CONTENT_XML
                                                 , where));
                query.bindValue(0, episode->data(EpisodeData::XmlContent));
                if (!query.exec()) {
                    LOG_QUERY_ERROR(query);
                    query.finish();
                    DB.rollback();
                    return false;
                }
                query.finish();
            }

            // save validation
            if (!saveEpisodeValidations(episode)) {
                DB.rollback();
                return false;
            }

            // save modifications
            if (!saveEpisodeModifications(episode)) {
                DB.rollback();
                return false;
            }

            episode->setModified(true);
        }
    }
    DB.commit();
    return true;
}

/** Save the raw XML episode content. Does not manage modification traces. */
bool EpisodeBase::saveEpisodeContent(const QVariant &uid, const QString &xml)
{
    if (!uid.isValid())
        return false;
    if (uid.toInt() < 0)
        return false;
    QSqlDatabase DB = QSqlDatabase::database(DB_NAME);
    if (!connectDatabase(DB, __LINE__)) {
        return false;
    }
    QHash<int, QString> where;
    where.insert(Constants::EPISODE_CONTENT_EPISODE_ID, QString("=%1").arg(uid.toString()));
    if (count(Constants::Table_EPISODE_CONTENT, Constants::EPISODE_CONTENT_EPISODE_ID, getWhereClause(Constants::Table_EPISODE_CONTENT, where)) == 0) {
        // save
        DB.transaction();
        QSqlQuery query(DB);
        query.prepare(prepareInsertQuery(Table_EPISODE_CONTENT));
        query.bindValue(EPISODE_CONTENT_ID, QVariant());
        query.bindValue(EPISODE_CONTENT_EPISODE_ID, uid.toInt());
        query.bindValue(EPISODE_CONTENT_XML, xml);
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            query.finish();
            DB.rollback();
            return false;
        }
        query.finish();
        DB.commit();
    } else {
        // update
        // TODO: manage modification traces
        DB.transaction();
        QSqlQuery query(DB);
        query.prepare(prepareUpdateQuery(Table_EPISODE_CONTENT, EPISODE_CONTENT_XML, where));
        query.bindValue(0, xml);
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            query.finish();
            DB.rollback();
            return false;
        }
        query.finish();
        DB.commit();
    }
    return true;
}

/**
 * Save a specific episode validation \e validation.
 * If needed, the Form::EpisodeValidationData::ValidationId is defined in the validation data.
 * This member manages its own transaction.
 */
bool EpisodeBase::saveEpisodeValidation(EpisodeValidationData *validation)
{
    if (!validation->isModified())
        return false;
    QSqlDatabase DB = QSqlDatabase::database(DB_NAME);
    if (!connectDatabase(DB, __LINE__)) {
        return false;
    }
    DB.transaction();
    QSqlQuery query(DB);
    QHash<int, QString> where;
    if (validation->validationId()!=-1) {
        // update
        where.insert(EP_MODIF_ID, QString("=%1").arg(validation->validationId()));
        query.prepare(prepareUpdateQuery(Table_VALIDATION,
                                         QList<int>()
                                         << VALIDATION_DATEOFVALIDATION
                                         << VALIDATION_USERUID
                                         << VALIDATION_ISVALID
                                         , where));
        query.bindValue(0, validation->data(EpisodeValidationData::ValidationDate));
        query.bindValue(1, validation->data(EpisodeValidationData::UserUid));
        query.bindValue(2, validation->data(EpisodeValidationData::IsValid));
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            query.finish();
            DB.rollback();
            return false;
        } else {
            validation->setModified(false);
        }
    } else {
        // save
        query.prepare(prepareInsertQuery(Table_VALIDATION));
        query.bindValue(VALIDATION_ID, QVariant());
        query.bindValue(VALIDATION_EPISODE_ID, validation->data(EpisodeValidationData::EpisodeId));
        query.bindValue(VALIDATION_DATEOFVALIDATION, validation->data(EpisodeValidationData::ValidationDate));
        query.bindValue(VALIDATION_USERUID, validation->data(EpisodeValidationData::UserUid));
        query.bindValue(VALIDATION_ISVALID, validation->data(EpisodeValidationData::IsValid).toInt());
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            query.finish();
            DB.rollback();
            return false;
        } else {
            validation->setData(EpisodeValidationData::ValidationId, query.lastInsertId());
            validation->setModified(false);
        }
    }
    query.finish();
    DB.commit();
    return true;
}

/** Remove the episode from the database. The episode is marked as non-valid
 *  (IsValid field) */
bool EpisodeBase::removeEpisode(const QVariant &uid)
{
    QSqlDatabase DB = QSqlDatabase::database(DB_NAME);
    if (!connectDatabase(DB, __LINE__))
        return false;
    using namespace Constants;
    QHash<int, QString> where;
    where.insert(EPISODES_ID, QString("='%1'").arg(uid.toString()));
    DB.transaction();
    QSqlQuery query(DB);
    query.prepare(prepareUpdateQuery(Table_EPISODES, EPISODES_ISVALID, where));
    query.bindValue(0, "0");
    if (!query.exec()) {
        LOG_QUERY_ERROR(query);
        query.finish();
        DB.rollback();
        return false;
    }
    query.finish();
    DB.commit();
    return true;
}

/** Remove all the recorded episode for the form \e formUid and for the patient \e patientUid in the database */
bool EpisodeBase::removeAllEpisodeForForm(const QVariant &formUid, const QString &patientUid)
{
    if (!formUid.isValid())
        return false;
    if (patientUid.isEmpty())
        return false;
    QSqlDatabase DB = QSqlDatabase::database(DB_NAME);
    if (!connectDatabase(DB, __LINE__))
        return false;
    using namespace Constants;
    QHash<int, QString> where;
    where.insert(EPISODES_FORM_PAGE_UID, QString("='%1'").arg(formUid.toString()));
    where.insert(EPISODES_PATIENT_UID, QString("='%1'").arg(patientUid));
    DB.transaction();
    QSqlQuery query(DB);
    query.prepare(prepareUpdateQuery(Table_EPISODES, EPISODES_ISVALID, where));
    query.bindValue(0, "0");
    if (!query.exec()) {
        LOG_QUERY_ERROR(query);
        query.finish();
        DB.rollback();
        return false;
    }
    query.finish();
    DB.commit();
    return true;
}

/** Return all recorded episodes form the database according to the Form::Internal::EpisodeBaseQuery \e baseQuery. Episodes are sorted by UserDate. */
QList<EpisodeData *> EpisodeBase::getEpisodes(const EpisodeBaseQuery &baseQuery)
{
    QList<EpisodeData *> toReturn;
    QSqlDatabase DB = QSqlDatabase::database(DB_NAME);
    if (!connectDatabase(DB, __LINE__)) {
        return toReturn;
    }

    // Manage filter
    QString order, limit, req;
    Utils::FieldList get;
    get << fields(Table_EPISODES);
    Utils::JoinList joins;
    Utils::FieldList conds;

    if (baseQuery.getEpisodeContent()) {
        joins << Utils::Join(Table_EPISODE_CONTENT, EPISODE_CONTENT_EPISODE_ID, Table_EPISODES, EPISODES_ID);
        get << fields(Table_EPISODE_CONTENT);
    }

    if (baseQuery.validEpisodes())
        conds << Utils::Field(Table_EPISODES, EPISODES_ISVALID, QString("=1"));
    if (baseQuery.deletedEpisodes())
        conds << Utils::Field(Table_EPISODES, EPISODES_ISVALID, QString("=0"));

    if (!baseQuery.patientUid().isNull())
        conds << Utils::Field(Table_EPISODES, EPISODES_PATIENT_UID, QString("='%1'").arg(baseQuery.patientUid().toString()));

    if (!baseQuery.userUid().isNull()) {
        joins << Utils::Join(Table_EPISODES_MODIF, EP_MODIF_EPISODE_ID, Table_EPISODES, EPISODES_ID);
        conds << Utils::Field(Table_EPISODES_MODIF, EP_MODIF_USERUID, QString("='%1'").arg(baseQuery.userUid().toString()));
    }

    if (!baseQuery.formUids().isEmpty()) {
        QString f;
        foreach(const QString &uid, baseQuery.formUids()) {
            f += QString("'%1',").arg(uid);
        }
        f.chop(1);
        conds << Utils::Field(Table_EPISODES, EPISODES_FORM_PAGE_UID, QString("IN (%1)").arg(f));
    }

//        joins << Utils::Join(Table_VALIDATION, VALIDATION_EPISODE_ID, Table_EPISODES, EPISODES_ID);

    order = QString(" ORDER BY `%1`.`%2` DESC\n").arg(table(Table_EPISODES)).arg(fieldName(Table_EPISODES, EPISODES_USERDATETIME));
    if (baseQuery.useLimit()) {
        limit = QString(" LIMIT %1, %2\n").arg(baseQuery.limitStart()).arg(baseQuery.limitEnd());
    }

    req = select(get, joins, conds) + order + limit;

    DB.transaction();
    QSqlQuery query(DB);
    if (query.exec(req)) {
        while (query.next()) {
            EpisodeData *e = new EpisodeData;
            e->setData(EpisodeData::Id , query.value(EPISODES_ID));
            e->setData(EpisodeData::PatientUuid, query.value(EPISODES_PATIENT_UID));
            e->setData(EpisodeData::Label , query.value(EPISODES_LABEL));
            e->setData(EpisodeData::UserDate , query.value(EPISODES_USERDATETIME));
            e->setData(EpisodeData::EpisodeCreationDateTime,
                       query.value(EPISODES_EPISODECREATIONDATETIME));
            e->setData(EpisodeData::IsValid , query.value(EPISODES_ISVALID));
            e->setData(EpisodeData::IsNewlyCreated , false);
            e->setData(EpisodeData::FormUuid , query.value(EPISODES_FORM_PAGE_UID));
            e->setData(EpisodeData::UserCreatorUuid , query.value(EPISODES_USERCREATOR));
            // content
            if (baseQuery.getEpisodeContent()) {
                e->setData(EpisodeData::ContentId , query.value(EPISODES_MaxParam + EPISODE_CONTENT_EPISODE_ID));
                e->setData(EpisodeData::XmlContent , query.value(EPISODES_MaxParam + EPISODE_CONTENT_XML));
                e->setData(EpisodeData::IsXmlContentPopulated, true);
            }

            // modifications
            QSqlQuery query2(DB);
            QHash<int, QString> where;
            where.insert(EP_MODIF_EPISODE_ID, QString("=%1").arg(e->episodeId()));
            req = select(Table_EPISODES_MODIF, where);
            if (query2.exec(req)) {
                while (query2.next()) {
                    EpisodeModificationData m;
                    m.setData(EpisodeModificationData::ModificationId, query2.value(EP_MODIF_ID));
                    m.setData(EpisodeModificationData::Date, query2.value(EP_MODIF_DATE));
                    m.setData(EpisodeModificationData::UserUid, query2.value(EP_MODIF_USERUID));
                    e->addEpisodeModification(m);
                    m.setModified(false);
                }
            } else {
                LOG_QUERY_ERROR(query2);
                query2.finish();
                DB.rollback();
                return toReturn;
            }
            query2.finish();

            // validations
            where.clear();
            where.insert(VALIDATION_EPISODE_ID, QString("=%1").arg(e->episodeId()));
            req = select(Table_VALIDATION, where);
            if (query2.exec(req)) {
                while (query2.next()) {
                    EpisodeValidationData v;
                    v.setData(EpisodeValidationData::ValidationId, query2.value(VALIDATION_ID));
                    v.setData(EpisodeValidationData::ValidationDate, query2.value(VALIDATION_DATEOFVALIDATION));
                    v.setData(EpisodeValidationData::UserUid, query2.value(VALIDATION_USERUID));
                    v.setData(EpisodeValidationData::IsValid, query2.value(VALIDATION_ISVALID));
                    e->addEpisodeValidation(v);
                    v.setModified(false);
                }
            } else {
                LOG_QUERY_ERROR(query2);
                query2.finish();
                DB.rollback();
                return toReturn;
            }
            query2.finish();

            e->setModified(false);
            toReturn << e;
        }
    } else {
        LOG_QUERY_ERROR(query);
        query.finish();
        DB.rollback();
        return toReturn;
    }
    query.finish();
    DB.commit();
    return toReturn;
}

/** Get the content of an episode. Return true if all goes fine. */
bool EpisodeBase::getEpisodeContent(EpisodeData *episode)
{
    QSqlDatabase DB = QSqlDatabase::database(DB_NAME);
    if (!connectDatabase(DB, __LINE__)) {
        return false;
    }
    bool episodeWasModified = episode->isModified();
    QHash<int, QString> where;
    where.insert(Constants::EPISODE_CONTENT_EPISODE_ID, QString("=%1").arg(episode->data(EpisodeData::Id).toString()));
    QString req = select(Constants::Table_EPISODE_CONTENT, Constants::EPISODE_CONTENT_XML, where);
    DB.transaction();
    QSqlQuery query(DB);
    if (query.exec(req)) {
        if (query.next()) {
            episode->setData(EpisodeData::XmlContent, query.value(0));
            episode->setData(EpisodeData::IsXmlContentPopulated, true);
            if (!episodeWasModified)
                episode->setModified(false);
        }
    } else {
        LOG_QUERY_ERROR(query);
        query.finish();
        DB.rollback();
        return false;
    }
    query.finish();
    DB.commit();
    return true;
}

/** Return the raw xml content of a episode identified by its \e uid */
QString EpisodeBase::getEpisodeContent(const QVariant &uid)
{
    if (!uid.isValid())
        return QString::null;
    if (uid.toInt() < 0)
        return QString::null;
    QSqlDatabase DB = QSqlDatabase::database(DB_NAME);
    if (!connectDatabase(DB, __LINE__)) {
        return QString::null;
    }
    QHash<int, QString> where;
    where.insert(Constants::EPISODE_CONTENT_EPISODE_ID, QString("=%1").arg(uid.toString()));
    QString req = select(Constants::Table_EPISODE_CONTENT, Constants::EPISODE_CONTENT_XML, where);
    DB.transaction();
    QSqlQuery query(DB);
    if (query.exec(req)) {
        if (query.next()) {
            const QString &xml = query.value(0).toString();
            query.finish();
            DB.commit();
            return xml;
        }
    } else {
        LOG_QUERY_ERROR(query);
        query.finish();
        DB.rollback();
        return QString::null;
    }
    query.finish();
    DB.commit();
    return QString::null;
}

/** Return all the episode validation of an episode. */
QList<EpisodeValidationData *> EpisodeBase::getEpisodeValidations(const QVariant &uid)
{
    QList<EpisodeValidationData*> toReturn;
    if (!uid.isValid())
        return toReturn;
    if (uid.toInt() < 0)
        return toReturn;
    QSqlDatabase DB = QSqlDatabase::database(DB_NAME);
    if (!connectDatabase(DB, __LINE__)) {
        return toReturn;
    }
    DB.transaction();
    QSqlQuery query(DB);
    QHash<int, QString> where;
    where.insert(VALIDATION_EPISODE_ID, QString("=%1").arg(uid.toInt()));
    QString req = select(Table_VALIDATION, where);
    if (query.exec(req)) {
        while (query.next()) {
            EpisodeValidationData *v = new EpisodeValidationData;
            v->setData(EpisodeValidationData::ValidationId, query.value(VALIDATION_ID));
            v->setData(EpisodeValidationData::ValidationDate, query.value(VALIDATION_DATEOFVALIDATION));
            v->setData(EpisodeValidationData::UserUid, query.value(VALIDATION_USERUID));
            v->setData(EpisodeValidationData::IsValid, query.value(VALIDATION_ISVALID));
            v->setModified(false);
            toReturn << v;
        }
    } else {
        LOG_QUERY_ERROR(query);
        query.finish();
        DB.rollback();
        return toReturn;
    }
    query.finish();
    DB.commit();
    return toReturn;
}

/**
 * Returns the total number of episodes recorded for one Form identified by its
 * \e formUid for the current patient.
 * Counts also all form equivalents.
 */
int EpisodeBase::getNumberOfEpisodes(const QString &formUid, const QStringList &equivalents)
{
    // Filter the form uuid (and equivalents)
    Utils::FieldList uuid;
    uuid << Utils::Field(Constants::Table_EPISODES, Constants::EPISODES_FORM_PAGE_UID, QString("='%1'").arg(formUid));
    if (!equivalents.isEmpty()) {
        foreach(const QString &eq, equivalents)
            uuid << Utils::Field(Constants::Table_EPISODES, Constants::EPISODES_FORM_PAGE_UID, QString("='%1'").arg(eq));
    }

    // Filter valid episodes
    QHash<int, QString> where;
    where.insert(Constants::EPISODES_ISVALID, "=1");
    where.insert(Constants::EPISODES_PATIENT_UID, QString("='%1'").arg(patient()->uuid()));

    QString filter = getWhereClause(Constants::Table_EPISODES, where).remove("WHERE") +
            " AND (" + getWhereClause(uuid, Utils::Database::OR) + ")";
    return count(Constants::Table_EPISODES, Constants::EPISODES_ID, filter);
}

/** For debugging purpose */
void EpisodeBase::toTreeWidget(QTreeWidget *tree) const
{
    Database::toTreeWidget(tree);
    QFont bold;
    bold.setBold(true);
    QTreeWidgetItem *db = new QTreeWidgetItem(tree, QStringList() << "Episodes count");
    db->setFont(0, bold);
    new QTreeWidgetItem(db, QStringList() << "Total episodes" << QString::number(count(Table_EPISODES, EPISODES_ID)));
    tree->expandAll();
}
