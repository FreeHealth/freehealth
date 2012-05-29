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
 *   Main Developpers:                                                     *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *       Pierre-Marie Desombre <pm.desombre@gmail.com>                     *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
  \class Alert::Internal::AlertBase
  Alert database manager. For internal use.
  \sa Alert::AlertCore
*/

/**
  \class Alert::Internal::AlertBaseQuery
  Construct queries for the Alert::Internal::AlertBase.
*/

#include "alertbase.h"
#include "alertitem.h"
#include "constants.h"

#include <utils/log.h>
#include <utils/global.h>
#include <utils/randomizer.h>
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

#include <QDir>
#include <QTreeWidget>
#include <QHash>
#include <QSqlQuery>

using namespace Alert;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::IUser *user() {return Core::ICore::instance()->user();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline Core::ICommandLine *commandLine()  { return Core::ICore::instance()->commandLine(); }

static inline bool connectDatabase(const QString &connectionName, const int line)
{
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    if (!db.isOpen()) {
        if (!db.open()) {
            Utils::Log::addError("AlertBase", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                      .arg(db.connectionName()).arg(db.lastError().text()), __FILE__, line);
            return false;
        }
    }
    return true;
}

enum DbValues {
    ItemId = 0,
    RelatedId,
    CategoryUid,
    ScriptId,
    ValidationId,
    TimingId,
    LabelLID,
    DescrLID,
    CommentLID
};

/** Create an empty query. */
AlertBaseQuery::AlertBaseQuery() :
    _validity(AlertBaseQuery::ValidAlerts)
{}

AlertBaseQuery::~AlertBaseQuery()
{}

/** Define the validity of queried alerts \sa Alert::Internal::AlertBaseQuery::AlertValidity */
void AlertBaseQuery::setAlertValidity(AlertBaseQuery::AlertValidity validity)
{
    _validity = validity;
}

/** Return the queried validity \sa Alert::Internal::AlertBaseQuery::AlertValidity */
AlertBaseQuery::AlertValidity AlertBaseQuery::alertValidity() const
{
    return _validity;
}

/** Query alerts for the current logged user. */
void AlertBaseQuery::addCurrentUserAlerts()
{
    const QString &u = user()->uuid();
    if (!_userUids.contains(u))
        _userUids << u;
}

/** Query alerts for the user according to its \e uuid. */
void AlertBaseQuery::addUserAlerts(const QString &uuid)
{
    if (!_userUids.contains(uuid))
        _userUids << uuid;
}

/** Query alerts for the current editing patient. */
void AlertBaseQuery::addCurrentPatientAlerts()
{
    const QString &u = patient()->uuid();
    if (!_patientUids.contains(u))
        _patientUids << u;
}

/** Query alerts for the patient according to its \e uuid. */
void AlertBaseQuery::addPatientAlerts(const QString &uuid)
{
    if (!_patientUids.contains(uuid))
        _patientUids << uuid;
}

/** Returns all queried user uuids. */
QStringList AlertBaseQuery::userUids() const
{
    return _userUids;
}

/** Returns all queried patient uuids. */
QStringList AlertBaseQuery::patientUids() const
{
    return _patientUids;
}

/** Query alerts for a specific date range (\e start, \e end). */
void AlertBaseQuery::setDateRange(const QDate &start, const QDate &end)
{
    _start = start;
    _end = end;
}

/** Return true if a date is defined. */
bool AlertBaseQuery::dateRangeDefined() const
{
    return (_start.isValid() && _end.isValid());
}

/** Returns the date range. */
QDate AlertBaseQuery::dateRangeStart() const
{
    return _start;
}

/** Returns the date range. */
QDate AlertBaseQuery::dateRangeEnd() const
{
    return _end;
}



AlertBase::AlertBase(QObject *parent) :
    QObject(parent),
    Utils::Database()
{
    setObjectName("AlertBase");

    using namespace Alert::Constants;
    addTable(Table_ALERT, "ALR");
    addTable(Table_ALERT_LABELS, "LBL");
    addTable(Table_ALERT_SCRIPTS, "SCR");
    addTable(Table_ALERT_TIMING, "TIM");
    addTable(Table_ALERT_VERSION, "VER");

    addField(Table_ALERT, ALERT_ID, "A_ID", FieldIsUniquePrimaryKey);
    addField(Table_ALERT, ALERT_UID, "A_UID", FieldIsUUID);
    addField(Table_ALERT, ALERT_REL_ID, "R_ID", FieldIsUUID);
    addField(Table_ALERT, ALERT_CATEGORY_UID, "C_UID", FieldIsUUID);
    addField(Table_ALERT, ALERT_SID, "SCR_ID", FieldIsInteger);
    addField(Table_ALERT, ALERT_ISVALID, "ISV", FieldIsInteger);
    addField(Table_ALERT, ALERT_VAL_ID, "VAL", FieldIsInteger);

    addField(Table_ALERT, ALERT_VIEW_TYPE, "VIEW_ID", FieldIsInteger);
    addField(Table_ALERT, ALERT_CONTENT_TYPE, "CONTENT_ID", FieldIsInteger);
    addField(Table_ALERT, ALERT_TIM_ID, "TIM_ID", FieldIsInteger);
    addField(Table_ALERT, ALERT_CONDITION_TYPE, "COND_ID", FieldIsInteger);
    addField(Table_ALERT, ALERT_PRIORITY, "PRIOR", FieldIsInteger);

    addField(Table_ALERT, ALERT_LABEL_LABELID, "LBL_LID", FieldIsInteger);
    addField(Table_ALERT, ALERT_DESCRIPTION_LABELID, "DES_LID", FieldIsInteger);
    addField(Table_ALERT, ALERT_COMMENT_LABELID, "COM_LID", FieldIsInteger);
    addField(Table_ALERT, ALERT_CREATION_DATE, "C_DATE", FieldIsDate);
    addField(Table_ALERT, ALERT_LAST_UPDATE_DATE, "U_DATE", FieldIsDate);
    addField(Table_ALERT, ALERT_THEMED_ICON, "THM_ICON", FieldIsDate);
    addField(Table_ALERT, ALERT_THEME_CSS, "CSS", FieldIsLongText);
    addField(Table_ALERT, ALERT_CRYPTED_PASSWORD, "PASS", FieldIsShortText);
    addField(Table_ALERT, ALERT_EXTRA_XML, "EXTRA", FieldIsLongText);

    addIndex(Table_ALERT, ALERT_ID);
    addIndex(Table_ALERT, ALERT_UID);
    addIndex(Table_ALERT, ALERT_REL_ID);
    addIndex(Table_ALERT, ALERT_CATEGORY_UID);

    // Alert relations to user/patient/family/groups and other...
    addField(Table_ALERT_RELATED, ALERT_RELATED_ID, "ID", FieldIsUniquePrimaryKey);
    addField(Table_ALERT_RELATED, ALERT_RELATED_REL_ID, "RID", FieldIsInteger);
    addField(Table_ALERT_RELATED, ALERT_RELATED_RELATED_TO, "TO", FieldIsInteger);
    addField(Table_ALERT_RELATED, ALERT_RELATED_RELATED_UID, "UID", FieldIsUUID);
    addIndex(Table_ALERT_RELATED, ALERT_RELATED_ID);
    addIndex(Table_ALERT_RELATED, ALERT_RELATED_REL_ID);

    // Translatable description items
    addField(Table_ALERT_LABELS, ALERT_LABELS_ID, "ID", FieldIsUniquePrimaryKey);
    addField(Table_ALERT_LABELS, ALERT_LABELS_LABELID, "LID", FieldIsInteger);
    addField(Table_ALERT_LABELS, ALERT_LABELS_LANG, "LNG", FieldIsLanguageText);
    addField(Table_ALERT_LABELS, ALERT_LABELS_VALUE, "VAL", FieldIsShortText);
    addField(Table_ALERT_LABELS, ALERT_LABELS_ISVALID, "ISV", FieldIsBoolean, "1");
    addIndex(Table_ALERT_LABELS, ALERT_LABELS_ID);
    addIndex(Table_ALERT_LABELS, ALERT_LABELS_LABELID);
    addIndex(Table_ALERT_LABELS, ALERT_LABELS_LANG);

    // Timing
    addField(Table_ALERT_TIMING, ALERT_TIMING_TIMINGID, "ID", FieldIsUniquePrimaryKey);
    addField(Table_ALERT_TIMING, ALERT_TIMING_TIMING_TIM_ID, "TIM_ID", FieldIsInteger);
    addField(Table_ALERT_TIMING, ALERT_TIMING_ISVALID, "ISV", FieldIsBoolean, "1");
    addField(Table_ALERT_TIMING, ALERT_TIMING_STARTDATETIME, "STR", FieldIsDateTime);
    addField(Table_ALERT_TIMING, ALERT_TIMING_ENDDATETIME, "END", FieldIsDateTime);
    addField(Table_ALERT_TIMING, ALERT_TIMING_CYCLES, "CYC", FieldIsInteger);
    addField(Table_ALERT_TIMING, ALERT_TIMING_CYCLINGDELAY, "CDY", FieldIsInteger);
    addField(Table_ALERT_TIMING, ALERT_TIMING_NEXTCYCLE, "NCY", FieldIsDateTime);
    addIndex(Table_ALERT_TIMING, ALERT_TIMING_TIMINGID);
    addIndex(Table_ALERT_TIMING, ALERT_TIMING_TIMING_TIM_ID);
    addIndex(Table_ALERT_TIMING, ALERT_TIMING_STARTDATETIME);
    addIndex(Table_ALERT_TIMING, ALERT_TIMING_ENDDATETIME);

    // Scripting
    addField(Table_ALERT_SCRIPTS, ALERT_SCRIPTS_ID, "ID", FieldIsUniquePrimaryKey);
    addField(Table_ALERT_SCRIPTS, ALERT_SCRIPTS_SID, "SID", FieldIsInteger);
    addField(Table_ALERT_SCRIPTS, ALERT_SCRIPT_UID, "S_UID", FieldIsUUID);
    addField(Table_ALERT_SCRIPTS, ALERT_SCRIPT_ISVALID, "ISV", FieldIsBoolean, "1");
    addField(Table_ALERT_SCRIPTS, ALERT_SCRIPT_TYPE, "S_TP", FieldIsShortText);
    addField(Table_ALERT_SCRIPTS, ALERT_SCRIPT_CONTENT, "S_CT", FieldIsLongText);
    addIndex(Table_ALERT_SCRIPTS, ALERT_SCRIPTS_ID);
    addIndex(Table_ALERT_SCRIPTS, ALERT_SCRIPTS_SID);
    addIndex(Table_ALERT_SCRIPTS, ALERT_SCRIPT_UID);

    // Validations
    addField(Table_ALERT_VALIDATION, ALERT_VALIDATION_VID, "ID", FieldIsUniquePrimaryKey);
    addField(Table_ALERT_VALIDATION, ALERT_VALIDATION_VAL_ID, "VAL_UID", FieldIsUUID);
    addField(Table_ALERT_VALIDATION, ALERT_VALIDATION_DATEOFVALIDATION, "DT", FieldIsDateTime);
    addField(Table_ALERT_VALIDATION, ALERT_VALIDATION_USER_UUID, "U_U", FieldIsUUID);
    addField(Table_ALERT_VALIDATION, ALERT_VALIDATION_USER_COMMENT, "U_C", FieldIsLongText);
    addIndex(Table_ALERT_VALIDATION, ALERT_VALIDATION_VID);
    addIndex(Table_ALERT_VALIDATION, ALERT_VALIDATION_VAL_ID);
    addIndex(Table_ALERT_VALIDATION, ALERT_VALIDATION_USER_UUID);

    addField(Table_ALERT_VERSION, VERSION_TEXT, "TXT", FieldIsShortText);
}

AlertBase::~AlertBase()
{
}

/** Connect to the alert database. Return the connection state. */
bool AlertBase::init()
{
    // only one base can be initialized
    if (m_initialized)
        return true;

    // connect
    if (commandLine()->value(Core::ICommandLine::ClearUserDatabases).toBool()) {
        createConnection(Constants::DB_NAME, Constants::DB_NAME,
                         settings()->databaseConnector(),
                         Utils::Database::DeleteAndRecreateDatabase);
    } else {
        createConnection(Constants::DB_NAME, Constants::DB_NAME,
                         settings()->databaseConnector(),
                         Utils::Database::CreateDatabase);
    }

    if (!database().isOpen()) {
        if (!database().open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_NAME).arg(database().lastError().text()));
        } else {
            LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().connectionName()).arg(database().driverName()));
        }
    } else {
        LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().connectionName()).arg(database().driverName()));
    }

//    d->checkDatabaseVersion();

    if (!checkDatabaseScheme()) {
        LOG_ERROR(tkTr(Trans::Constants::DATABASE_1_SCHEMA_ERROR).arg(Constants::DB_NAME));
        return false;
    }

    m_initialized = true;
    return true;
}

/** Private part of the Patients::PatientBase that creates the database. \sa Utils::Database. */
bool AlertBase::createDatabase(const QString &connectionName , const QString &dbName,
                    const QString &pathOrHostName,
                    TypeOfAccess access, AvailableDrivers driver,
                    const QString & login, const QString & pass,
                    const int port,
                    CreationOption /*createOption*/
                   )
{
    Q_UNUSED(access);
    if (connectionName != Constants::DB_NAME)
        return false;

    LOG(tkTr(Trans::Constants::TRYING_TO_CREATE_1_PLACE_2).arg(dbName).arg(pathOrHostName));

    // create an empty database and connect
    QSqlDatabase DB;
    if (driver == SQLite) {
        DB = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        if (!QDir(pathOrHostName).exists())
            if (!QDir().mkpath(pathOrHostName))
                tkTr(Trans::Constants::_1_ISNOT_AVAILABLE_CANNOTBE_CREATED).arg(pathOrHostName);
        DB.setDatabaseName(QDir::cleanPath(pathOrHostName + QDir::separator() + dbName));
        if (!DB.open())
            LOG(tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2).arg(dbName).arg(DB.lastError().text()));
        setDriver(Utils::Database::SQLite);
    }
    else if (driver == MySQL) {
        DB = QSqlDatabase::database(connectionName);
        if (!DB.open()) {
            QSqlDatabase dbcreate = QSqlDatabase::addDatabase("QMYSQL", "__ALERTS_CREATOR");
            dbcreate.setHostName(pathOrHostName);
            dbcreate.setUserName(login);
            dbcreate.setPassword(pass);
            dbcreate.setPort(port);
            if (!dbcreate.open()) {
                Utils::warningMessageBox(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                         .arg(dbcreate.connectionName()).arg(dbcreate.lastError().text()),
                                         tkTr(Trans::Constants::CONTACT_DEV_TEAM));
                return false;
            }
            QSqlQuery q(QString("CREATE DATABASE `%1`").arg(dbName), dbcreate);
            if (!q.isActive()) {
                LOG_QUERY_ERROR(q);
                Utils::warningMessageBox(tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2)
                                         .arg(dbcreate.connectionName()).arg(dbcreate.lastError().text()),
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
        if (QSqlDatabase::connectionNames().contains("__ALERTS_CREATOR"))
            QSqlDatabase::removeDatabase("__ALERTS_CREATOR");
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

    // inform the version
    QSqlQuery query(database());
    query.prepare(prepareInsertQuery(Constants::Table_ALERT_VERSION));
    query.bindValue(Constants::VERSION_TEXT, Constants::DB_ACTUALVERSION);
    if (!query.exec()) {
        LOG_QUERY_ERROR(query);
        return false;
    }

    return true;
}

/** Create a virtual item. For debugging purpose. */
AlertItem AlertBase::createVirtualItem() const
{
    Utils::Randomizer r;
    r.setPathToFiles(settings()->path(Core::ISettings::BundleResourcesPath) + "/textfiles/");
    QDir pix(settings()->path(Core::ISettings::SmallPixmapPath));

    AlertItem item;
    item.setValidity(true);
    item.setUuid(createUid());
    if (r.randomBool())
        item.setCryptedPassword(r.randomWords(1).toUtf8().toBase64());

    // fr, de, en, xx
    QStringList langs;
    langs << "en" << "fr" << "de" << "xx";
    foreach(const QString &l, langs) {
        item.setLabel(r.randomWords(r.randomInt(2, 10)), l);
        item.setCategory(r.randomWords(r.randomInt(2, 10)), l);
        item.setDescription(r.randomWords(r.randomInt(2, 10)), l);
        item.setComment(r.randomWords(r.randomInt(2, 10)), l);
    }

    item.setViewType(AlertItem::ViewType(r.randomInt(0, AlertItem::StaticStatusBar)));
    item.setContentType(AlertItem::ContentType(r.randomInt(0, AlertItem::UserNotification)));
    item.setPriority(AlertItem::Priority(r.randomInt(0, AlertItem::Low)));
    item.setCreationDate(r.randomDateTime(QDateTime::currentDateTime()));
    if (r.randomBool())
        item.setLastUpdate(r.randomDateTime(item.creationDate()));
    item.setThemedIcon(r.randomFile(pix, QStringList() << "*.png").fileName());
    if (r.randomBool())
        item.setStyleSheet(r.randomWords(10));
    if (r.randomBool())
        item.setExtraXml(QString("<xml>%1</xml>").arg(r.randomWords(r.randomInt(0, r.randomInt(2, 20)))));

    // Add 1 relation
    AlertRelation rel;
    rel.setRelatedTo(AlertRelation::RelatedToAllPatients);
    item.addRelation(rel);

    // Add timing
    AlertTiming time;
    time.setValid(true);
    time.setStart(r.randomDateTime(QDateTime::currentDateTime()));
    time.setEnd(time.start().addDays(r.randomInt(10, 5000)));
    if (r.randomBool()) {
        time.setCycling(true);
        time.setCyclingDelayInDays(r.randomInt(10, 100));
        time.setNumberOfCycles(r.randomInt(1, 100));
    }
    item.addTiming(time);

    // TODO : Add random script
//    item.addScript();

    // TODO : Add random validation
//    item.addValidation();

    item.setModified(false);
    return item;
}

/** Save or update the Alert::AlertItem in the alert database. Return true in case of success. The AlertItem is modified during this process. */
bool AlertBase::saveAlertItem(AlertItem &item)
{
    if (!connectDatabase(Constants::DB_NAME, __LINE__))
        return false;
    // update or save ?
    if (!item.db(ItemId).isValid()) {
        // try to catch the id using the uuid
        QHash<int, QString> where;
        where.insert(Constants::ALERT_UID, QString("='%1'").arg(item.uuid()));
        QString req = select(Constants::Table_ALERT, Constants::ALERT_ID, where);
        QSqlQuery query(database());
        if (query.exec(req)) {
            if (query.next())
                item.setDb(ItemId, query.value(0).toInt());
        } else {
            LOG_QUERY_ERROR(query);
        }
        if (!item.db(ItemId).isValid())
            return updateAlertItem(item);
    }

    database().transaction();

    if (!saveItemRelations(item)) {
        database().rollback();
        return false;
    }
    if (!saveItemScripts(item)) {
        database().rollback();
        return false;
    }
    if (!saveItemTimings(item)) {
        database().rollback();
        return false;
    }
    if (!saveItemValidations(item)) {
        database().rollback();
        return false;
    }
    if (!saveItemLabels(item)) {
        database().rollback();
        return false;
    }

    if (item.uuid().isEmpty())
        item.setUuid(Database::createUid());
    QSqlQuery query(database());
    QString req = prepareInsertQuery(Constants::Table_ALERT);
    query.prepare(req);
    query.bindValue(Constants::ALERT_ID, QVariant());
    query.bindValue(Constants::ALERT_UID, item.uuid());
    query.bindValue(Constants::ALERT_CATEGORY_UID, item.db(CategoryUid));
    query.bindValue(Constants::ALERT_REL_ID, item.db(RelatedId));
    query.bindValue(Constants::ALERT_SID, item.db(ScriptId));
    query.bindValue(Constants::ALERT_VAL_ID, item.db(ValidationId));
    query.bindValue(Constants::ALERT_TIM_ID, item.db(TimingId));
    query.bindValue(Constants::ALERT_ISVALID, item.isValid());
    query.bindValue(Constants::ALERT_VIEW_TYPE, item.viewType());
    query.bindValue(Constants::ALERT_CONTENT_TYPE, item.contentType());
    query.bindValue(Constants::ALERT_CONDITION_TYPE, QVariant());
    query.bindValue(Constants::ALERT_PRIORITY, item.priority());
    query.bindValue(Constants::ALERT_LABEL_LABELID, item.db(LabelLID));
    query.bindValue(Constants::ALERT_DESCRIPTION_LABELID, item.db(DescrLID));
    query.bindValue(Constants::ALERT_COMMENT_LABELID, item.db(CommentLID));
    query.bindValue(Constants::ALERT_CREATION_DATE, item.creationDate());
    query.bindValue(Constants::ALERT_LAST_UPDATE_DATE, item.lastUpdate());
    query.bindValue(Constants::ALERT_THEMED_ICON, item.themedIcon());
    query.bindValue(Constants::ALERT_THEME_CSS, item.styleSheet());
    query.bindValue(Constants::ALERT_CRYPTED_PASSWORD, item.cryptedPassword());
    query.bindValue(Constants::ALERT_EXTRA_XML, item.extraXml());
    if (query.exec()) {
        item.setDb(ItemId, query.lastInsertId());
        item.setModified(false);
    } else {
        LOG_QUERY_ERROR(query);
        database().rollback();
        return false;
    }
    query.finish();
    database().commit();
    return true;
}

bool AlertBase::updateAlertItem(AlertItem &item)
{
    if (!item.db(ItemId).isValid())
        return false;

    database().transaction();

    if (!saveItemRelations(item)) {
        database().rollback();
        return false;
    }
    if (!saveItemScripts(item)) {
        database().rollback();
        return false;
    }
    if (!saveItemTimings(item)) {
        database().rollback();
        return false;
    }
    if (!saveItemValidations(item)) {
        database().rollback();
        return false;
    }
    if (!saveItemLabels(item)) {
        database().rollback();
        return false;
    }

    if (item.uuid().isEmpty())
        item.setUuid(Database::createUid());
    QSqlQuery query(database());
    QString req = prepareUpdateQuery(Constants::Table_ALERT);
    query.prepare(req);
    query.bindValue(Constants::ALERT_ID, item.db(ItemId).toInt());
    query.bindValue(Constants::ALERT_UID, item.uuid());
    query.bindValue(Constants::ALERT_CATEGORY_UID, item.db(CategoryUid));
    query.bindValue(Constants::ALERT_REL_ID, item.db(RelatedId));
    query.bindValue(Constants::ALERT_SID, item.db(ScriptId));
    query.bindValue(Constants::ALERT_VAL_ID, item.db(ValidationId));
    query.bindValue(Constants::ALERT_TIM_ID, item.db(TimingId));
    query.bindValue(Constants::ALERT_ISVALID, item.isValid());
    query.bindValue(Constants::ALERT_VIEW_TYPE, item.viewType());
    query.bindValue(Constants::ALERT_CONTENT_TYPE, item.contentType());
    query.bindValue(Constants::ALERT_CONDITION_TYPE, QVariant());
    query.bindValue(Constants::ALERT_PRIORITY, item.priority());
    query.bindValue(Constants::ALERT_LABEL_LABELID, item.db(LabelLID));
    query.bindValue(Constants::ALERT_DESCRIPTION_LABELID, item.db(DescrLID));
    query.bindValue(Constants::ALERT_COMMENT_LABELID, item.db(CommentLID));
    query.bindValue(Constants::ALERT_CREATION_DATE, item.creationDate());
    query.bindValue(Constants::ALERT_LAST_UPDATE_DATE, item.lastUpdate());
    query.bindValue(Constants::ALERT_THEMED_ICON, item.themedIcon());
    query.bindValue(Constants::ALERT_THEME_CSS, item.styleSheet());
    query.bindValue(Constants::ALERT_CRYPTED_PASSWORD, item.cryptedPassword());
    query.bindValue(Constants::ALERT_EXTRA_XML, item.extraXml());
    if (query.exec()) {
        item.setModified(false);
    } else {
        LOG_QUERY_ERROR(query);
        database().rollback();
        return false;
    }
    query.finish();
    database().commit();
    return true;
}

bool AlertBase::saveItemRelations(AlertItem &item)
{
    // we are inside a transaction opened by saveAlertItem
    if (!connectDatabase(Constants::DB_NAME, __LINE__))
        return false;
    // get the related REL_ID
    int id = -1;
    if (item.db(RelatedId).isValid()) {
        id = item.db(RelatedId).toInt();
        // delete all old relations
        QHash<int, QString> where;
        where.insert(Constants::ALERT_RELATED_REL_ID, QString("=%1").arg(id));
        QString req = prepareDeleteQuery(Constants::Table_ALERT_RELATED, where);
        QSqlQuery query(database());
        if (!query.exec(req)) {
            LOG_QUERY_ERROR(query);
            return false;
        }
        query.finish();
    } else {
        id = max(Constants::Table_ALERT_RELATED, Constants::ALERT_RELATED_REL_ID).toInt() + 1;
        item.setDb(RelatedId, id);
    }
    // save all relations
    for(int i=0; i<item.relations().count(); ++i) {
        AlertRelation &rel = item.relationAt(i);
        QSqlQuery query(database());
        QString req = prepareInsertQuery(Constants::Table_ALERT_RELATED);
        query.prepare(req);
        query.bindValue(Constants::ALERT_RELATED_ID, QVariant());
        query.bindValue(Constants::ALERT_RELATED_REL_ID, id);
        query.bindValue(Constants::ALERT_RELATED_RELATED_TO, rel.relatedTo());
        query.bindValue(Constants::ALERT_RELATED_RELATED_UID, rel.relatedToUid());
        if (query.exec()) {
            rel.setId(query.lastInsertId().toInt());
        } else {
            LOG_QUERY_ERROR(query);
            return false;
        }
        query.finish();
    }
    return true;
}

bool AlertBase::saveItemScripts(AlertItem &item)
{
    // we are inside a transaction opened by saveAlertItem
    if (!connectDatabase(Constants::DB_NAME, __LINE__))
        return false;
    // get the script script_id
    int id = -1;
    if (item.db(ScriptId).isValid()) {
        id = item.db(ScriptId).toInt();
        // delete all old relations
        QHash<int, QString> where;
        where.insert(Constants::ALERT_SCRIPTS_SID, QString("=%1").arg(id));
        QString req = prepareDeleteQuery(Constants::Table_ALERT_SCRIPTS, where);
        QSqlQuery query(database());
        if (!query.exec(req)) {
            LOG_QUERY_ERROR(query);
            return false;
        }
        query.finish();
    } else {
        id = max(Constants::Table_ALERT_SCRIPTS, Constants::ALERT_SCRIPTS_SID).toInt() + 1;
        item.setDb(ScriptId, id);
    }
    // save all scripts
    for(int i=0; i<item.relations().count(); ++i) {
        AlertScript &script = item.scriptAt(i);
        QSqlQuery query(database());
        QString req = prepareInsertQuery(Constants::Table_ALERT_SCRIPTS);
        query.prepare(req);
        query.bindValue(Constants::ALERT_SCRIPTS_ID, QVariant());
        query.bindValue(Constants::ALERT_SCRIPTS_SID, id);
        query.bindValue(Constants::ALERT_SCRIPT_UID, script.uuid());
        query.bindValue(Constants::ALERT_SCRIPT_ISVALID, script.isValid());
        query.bindValue(Constants::ALERT_SCRIPT_TYPE, script.type());
        query.bindValue(Constants::ALERT_SCRIPT_CONTENT, script.script());
        if (query.exec()) {
            script.setId(query.lastInsertId().toInt());
        } else {
            LOG_QUERY_ERROR(query);
            return false;
        }
        query.finish();
    }
    return true;
}

bool AlertBase::saveItemTimings(AlertItem &item)
{
    // we are inside a transaction opened by saveAlertItem
    if (!connectDatabase(Constants::DB_NAME, __LINE__))
        return false;
    // get the timind timing_id
    int id = -1;
    if (item.db(TimingId).isValid()) {
        id = item.db(TimingId).toInt();
        // delete all old relations
        QHash<int, QString> where;
        where.insert(Constants::ALERT_TIMING_TIMING_TIM_ID, QString("=%1").arg(id));
        QString req = prepareDeleteQuery(Constants::Table_ALERT_TIMING, where);
        QSqlQuery query(database());
        if (!query.exec(req)) {
            LOG_QUERY_ERROR(query);
            return false;
        }
        query.finish();
    } else {
        id = max(Constants::Table_ALERT_TIMING, Constants::ALERT_TIMING_TIMING_TIM_ID).toInt() + 1;
        item.setDb(TimingId, id);
    }
    // save all timings
    for(int i=0; i<item.timings().count(); ++i) {
        AlertTiming &timing = item.timingAt(i);
        QSqlQuery query(database());
        QString req = prepareInsertQuery(Constants::Table_ALERT_TIMING);
        query.prepare(req);
        query.bindValue(Constants::ALERT_TIMING_TIMINGID, QVariant());
        query.bindValue(Constants::ALERT_TIMING_TIMING_TIM_ID, id);
        query.bindValue(Constants::ALERT_TIMING_ISVALID, timing.isValid());
        query.bindValue(Constants::ALERT_TIMING_STARTDATETIME, timing.start());
        query.bindValue(Constants::ALERT_TIMING_ENDDATETIME, timing.end());
        query.bindValue(Constants::ALERT_TIMING_CYCLES, timing.numberOfCycles());
        query.bindValue(Constants::ALERT_TIMING_CYCLINGDELAY, timing.cyclingDelayInDays());
        query.bindValue(Constants::ALERT_TIMING_NEXTCYCLE, timing.nextDate());
        if (query.exec()) {
            timing.setId(query.lastInsertId().toInt());
        } else {
            LOG_QUERY_ERROR(query);
            return false;
        }
        query.finish();
    }
    return true;
}

bool AlertBase::saveItemValidations(AlertItem &item)
{
    // we are inside a transaction opened by saveAlertItem
    if (!connectDatabase(Constants::DB_NAME, __LINE__))
        return false;
    // get the validations val_id
    int id = -1;
    if (item.db(ValidationId).isValid()) {
        id = item.db(ValidationId).toInt();
        // delete all old relations
        QHash<int, QString> where;
        where.insert(Constants::ALERT_VALIDATION_VAL_ID, QString("=%1").arg(id));
        QString req = prepareDeleteQuery(Constants::Table_ALERT_VALIDATION, where);
        QSqlQuery query(database());
        if (!query.exec(req)) {
            LOG_QUERY_ERROR(query);
            return false;
        }
        query.finish();
    } else {
        id = max(Constants::Table_ALERT_VALIDATION, Constants::ALERT_VALIDATION_VAL_ID).toInt() + 1;
        item.setDb(TimingId, id);
    }
    // save all validations
    for(int i=0; i<item.validations().count(); ++i) {
        AlertValidation &validation = item.validationAt(i);
        QSqlQuery query(database());
        QString req = prepareInsertQuery(Constants::Table_ALERT_VALIDATION);
        query.prepare(req);
        query.bindValue(Constants::ALERT_VALIDATION_VID, QVariant());
        query.bindValue(Constants::ALERT_VALIDATION_VAL_ID, id);
        query.bindValue(Constants::ALERT_VALIDATION_DATEOFVALIDATION, validation.dateOfValidation());
        query.bindValue(Constants::ALERT_VALIDATION_USER_UUID, validation.userUid());
        query.bindValue(Constants::ALERT_VALIDATION_USER_COMMENT, validation.userComment());
        if (query.exec()) {
            validation.setId(query.lastInsertId().toInt());
        } else {
            LOG_QUERY_ERROR(query);
            return false;
        }
        query.finish();
    }
    return true;
}

bool AlertBase::saveItemLabels(AlertItem &item)
{
    // we are inside a transaction opened by saveAlertItem
    if (!connectDatabase(Constants::DB_NAME, __LINE__))
        return false;
    QSqlQuery query(database());
    QList<int> lids;
    QList<int> vals;
    const int LABEL = 0;
    const int DESCR = 1;
    const int COMMENT = 2;
    // get the labels lid for label, descr && comment
    lids << -1 << -1 << -1;
    vals << LabelLID << DescrLID << CommentLID;
    for(int i=0; i < vals.count(); ++i) {
        if (item.db(vals.at(i)).isValid()) {
            lids[i] = item.db(LabelLID).toInt();
            // delete all old relations
            QHash<int, QString> where;
            where.insert(Constants::ALERT_LABEL_LABELID, QString("=%1").arg(lids[i]));
            QString req = prepareDeleteQuery(Constants::Table_ALERT_LABELS, where);
            if (!query.exec(req)) {
                LOG_QUERY_ERROR(query);
                return false;
            }
        } else {
            lids[i] = max(Constants::Table_ALERT_LABELS, Constants::ALERT_LABEL_LABELID).toInt() + 1;
            item.setDb(vals.at(i), lids.at(i));
        }
        query.finish();
    }
    // save all labels
    foreach(const QString &l, item.availableLanguages()) {
        const QString &label = item.label(l);
        if (!label.isEmpty()) {
            QString req = prepareInsertQuery(Constants::Table_ALERT_LABELS);
            query.prepare(req);
            query.bindValue(Constants::ALERT_LABELS_ID, QVariant());
            query.bindValue(Constants::ALERT_LABELS_LABELID, lids[LABEL]);
            query.bindValue(Constants::ALERT_LABELS_LANG, l);
            query.bindValue(Constants::ALERT_LABELS_VALUE, label);
            query.bindValue(Constants::ALERT_LABELS_ISVALID, 1);
            if (!query.exec()) {
                LOG_QUERY_ERROR(query);
                return false;
            }
            query.finish();
        }
        const QString &descr = item.description(l);
        if (!descr.isEmpty()) {
            QString req = prepareInsertQuery(Constants::Table_ALERT_LABELS);
            query.prepare(req);
            query.bindValue(Constants::ALERT_LABELS_ID, QVariant());
            query.bindValue(Constants::ALERT_LABELS_LABELID, lids[DESCR]);
            query.bindValue(Constants::ALERT_LABELS_LANG, l);
            query.bindValue(Constants::ALERT_LABELS_VALUE, descr);
            query.bindValue(Constants::ALERT_LABELS_ISVALID, 1);
            if (!query.exec()) {
                LOG_QUERY_ERROR(query);
                return false;
            }
            query.finish();
        }
        const QString &comment = item.comment(l);
        if (!comment.isEmpty()) {
            QString req = prepareInsertQuery(Constants::Table_ALERT_LABELS);
            query.prepare(req);
            query.bindValue(Constants::ALERT_LABELS_ID, QVariant());
            query.bindValue(Constants::ALERT_LABELS_LABELID, lids[COMMENT]);
            query.bindValue(Constants::ALERT_LABELS_LANG, l);
            query.bindValue(Constants::ALERT_LABELS_VALUE, comment);
            query.bindValue(Constants::ALERT_LABELS_ISVALID, 1);
            if (!query.exec()) {
                LOG_QUERY_ERROR(query);
                return false;
            }
            query.finish();
        }
    }
    return true;
}

/** Return the Alert::AlertItem corresponding to the Alert::Internal::AlertBaseQuery \e query */
QVector<AlertItem> AlertBase::getAlertItems(const AlertBaseQuery &query)
{
    QVector<AlertItem> alerts;
    return alerts;
}

/** Reconnect the database when the database server changes. \sa Core::ICore::databaseServerChanged(), Core::ISettings::databaseConnector() */
void AlertBase::onCoreDatabaseServerChanged()
{
    m_initialized = false;
    if (QSqlDatabase::connectionNames().contains(Constants::DB_NAME)) {
        QSqlDatabase::removeDatabase(Constants::DB_NAME);
    }
    init();
}

/** For debugging purpose */
void AlertBase::toTreeWidget(QTreeWidget *tree)
{
    Database::toTreeWidget(tree);
    tree->expandAll();
}


