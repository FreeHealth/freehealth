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
 \class Agenda::AgendaBase
 \brief Management of all database access for the agendas.
 Database schema:
    - CALENDAR, USERCALENDARS: each user can own multiple calendars. Table USERCALENDARS
    list all calendars own by users. Where as CALENDAR contains all needed information to
    manage calendars.
    - EVENTS, CYCLINGEVENTS, COMMON: management of agenda events.
    - USER_WORKINGTIME: management of users availabilities, vacancies...
    - VERSION : contains the actual version of the database
*/

#include "agendabase.h"
#include "constants.h"
#include "icalendarevent.h"
#include "iusercalendar.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <printerplugin/textdocumentextra.h>
#include <coreplugin/iuser.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/databaseconnector.h>

#include <QCoreApplication>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QFile>
#include <QDir>
#include <QLocale>
#include <QDateTime>
#include <QUuid>

using namespace Agenda::Internal;
using namespace Agenda::Constants;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::IUser *user() {return Core::ICore::instance()->user();}

static inline bool connectDatabase(const QString &connectionName, const int line)
{
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    if (!db.isOpen()) {
        if (!db.open()) {
            LOG_ERROR_FOR("AgendaBase", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                      .arg(db.connectionName()).arg(db.lastError().text()));
            return false;
        }
    }
    return true;
}

CalendarEventQuery::CalendarEventQuery()
{
    m_DateStart = QDateTime::currentDateTime();
    m_DateEnd = QDateTime::currentDateTime();
    m_Limit = 10;
    m_StartItem = 0;
    m_UseCurrentUser = true;
}

CalendarEventQuery::~CalendarEventQuery()
{}

/** By default a query is made on the current user, by defining a user filter, the query will only filter the user identified with the UUID \e userUid */
void CalendarEventQuery::setUserFilter(const QString &userUid)
{
    m_UseCurrentUser = false;
    m_Users.clear();
    m_Users << userUid;
}

void CalendarEventQuery::setPatientFilter(const QStringList &limitToPatientUids)
{
    m_Patients = limitToPatientUids;
}

void CalendarEventQuery::setDateRangeForToday()
{
    m_DateStart = QDateTime(QDate::currentDate(), QTime(0,0,0));
    m_DateEnd = QDateTime(QDate::currentDate(), QTime(23,59,59));
}

void CalendarEventQuery::setDateRangeForTomorrow()
{
    m_DateStart = QDateTime(QDate::currentDate().addDays(1), QTime(0,0,0));
    m_DateEnd = QDateTime(QDate::currentDate().addDays(1), QTime(23,59,59));
}

void CalendarEventQuery::setDateRangeForYesterday()
{
    m_DateStart = QDateTime(QDate::currentDate().addDays(-1), QTime(0,0,0));
    m_DateEnd = QDateTime(QDate::currentDate().addDays(-1), QTime(23,59,59));
}

void CalendarEventQuery::setDateRangeForCurrentWeek()
{
    QDate monday = QDate::currentDate();
    monday = monday.addDays(-monday.dayOfWeek() + 1);
    m_DateStart = QDateTime(monday, QTime(0,0,0));
    m_DateEnd = QDateTime(monday.addDays(6), QTime(23,59,59));
}

bool CalendarEventQuery::hasDateRange() const
{
    return (m_DateStart != QDateTime::currentDateTime() && m_DateEnd != QDateTime::currentDateTime());
}

// Initializing static datas
bool AgendaBase::m_initialized = false;
AgendaBase *AgendaBase::m_Instance = 0;

/**
  \brief Returns the unique instance of AgendaBase. If the instance does not exist it is created.
  You should never construct a instance of this object using the constructor.
*/
AgendaBase *AgendaBase::instance()
{
    if (!m_Instance)
        m_Instance = new AgendaBase(qApp);
    return m_Instance;
}

AgendaBase::AgendaBase(QObject *parent) :
        QObject(parent), Utils::Database()
{
    setObjectName("AgendaBase");
    m_initialized = false;

    // populate tables and fields of database
    addTable(Table_VERSION,"VERSION");
    addTable(Table_CALENDAR, "CALENDAR");
    addTable(Table_USERCALENDARS, "USERCAL");
    addTable(Table_COMMON, "COMMON");
    addTable(Table_EVENTS, "EVENTS");
    addTable(Table_CYCLINGEVENTS, "CYCLING");
//    addTable(Table_USER_WORKINGTIME, "USERWORK");

    addField(Table_USERCALENDARS, USERCAL_ID, "USERCAL_ID", FieldIsUniquePrimaryKey);
    addField(Table_USERCALENDARS, USERCAL_USER_UUID, "USER_UUID", FieldIsUUID);
    addField(Table_USERCALENDARS, USERCAL_CAL_ID, "CAL_ID", FieldIsInteger);

    addField(Table_CALENDAR, CAL_ID, "CAL_ID", FieldIsUniquePrimaryKey);
    addField(Table_CALENDAR, CAL_CATEGORYID, "CAT_ID", FieldIsInteger);
    addField(Table_CALENDAR, CAL_ISVALID, "ISVALID", FieldIsBoolean);
    addField(Table_CALENDAR, CAL_STATUS, "STATUS", FieldIsInteger);
    addField(Table_CALENDAR, CAL_TYPE, "TYPE", FieldIsInteger);
    addField(Table_CALENDAR, CAL_SITEUID, "SITE_UUID", FieldIsUUID);
    addField(Table_CALENDAR, CAL_DEFAULT, "DEFAULT", FieldIsBoolean);
    addField(Table_CALENDAR, CAL_ISPRIVATE, "ISPRIV", FieldIsBoolean);
    addField(Table_CALENDAR, CAL_PASSWORD, "PASSWORD", FieldIsShortText);
    addField(Table_CALENDAR, CAL_LABEL, "LABEL", FieldIsShortText);
    addField(Table_CALENDAR, CAL_THEMEDICON, "THEMEDICON", FieldIsShortText);
    addField(Table_CALENDAR, CAL_FULLCONTENT, "FULLCONTENT", FieldIsBlob);
    addField(Table_CALENDAR, CAL_XMLOPTIONS, "XMLOPTIONS", FieldIsBlob);

    addField(Table_COMMON, COMMON_ID, "COM_ID", FieldIsUniquePrimaryKey);
    addField(Table_COMMON, COMMON_CAT_ID, "CAT_ID", FieldIsInteger);
    addField(Table_COMMON, COMMON_TYPE_ID, "TYPE_ID", FieldIsInteger);
    addField(Table_COMMON, COMMON_STATUS_ID, "STATUS_ID", FieldIsInteger); // (like draft, validated, rejected, need more info, need to send letter…)
    addField(Table_COMMON, COMMON_SITE_UID, "SITE_UID", FieldIsUUID); // (for a later use)
    addField(Table_COMMON, COMMON_ISPRIVATE, "ISPRIVATE", FieldIsBoolean);
    addField(Table_COMMON, COMMON_PASSWORD, "PASSWORD", FieldIsShortText); // (private events can be protected by a password)
    addField(Table_COMMON, COMMON_ISBUSY, "ISBUSY", FieldIsBoolean);
    addField(Table_COMMON, COMMON_ISAGROUPEVENT, "ISGROUP", FieldIsBoolean); // (event can be defined by and for a group of users)
    addField(Table_COMMON, COMMON_LABEL, "LABEL", FieldIsShortText);
    addField(Table_COMMON, COMMON_FULLCONTENT, "CONTENT", FieldIsBlob);
    addField(Table_COMMON, COMMON_TEXTUAL_SITE, "TEXT_SITE", FieldIsShortText);
    addField(Table_COMMON, COMMON_THEMEDICON, "THEMEDICON", FieldIsShortText);
    addField(Table_COMMON, COMMON_XMLVIEWOPTIONS, "XML_VIEW", FieldIsBlob);// (color, margins, spacing…)
    addField(Table_COMMON, COMMON_XMLCALOPTIONS, "XML_OPTIONS", FieldIsBlob); // (can be changed…)

    addField(Table_EVENTS, EVENT_ID, "EV_ID", FieldIsUniquePrimaryKey);
    addField(Table_EVENTS, EVENT_CAL_ID, "CAL_ID", FieldIsInteger);
    addField(Table_EVENTS, EVENT_COMMON_ID, "COM_ID", FieldIsInteger);
    addField(Table_EVENTS, EVENT_PATIENT_UID, "PATIENT_UID", FieldIsUUID);
    addField(Table_EVENTS, EVENT_ISVALID, "ISVALID", FieldIsBoolean);
    addField(Table_EVENTS, EVENT_DATESTART, "DTSTART", FieldIsDate);
    addField(Table_EVENTS, EVENT_DATEEND, "DTEND", FieldIsDate);

    addField(Table_CYCLINGEVENTS, CYCLING_ID, "CYC_ID", FieldIsUniquePrimaryKey);
    addField(Table_CYCLINGEVENTS, CYCLING_CALENDAR_ID, "CAL_ID", FieldIsInteger);
    addField(Table_CYCLINGEVENTS, CYCLING_COMMON_ID, "COM_ID", FieldIsInteger);
    addField(Table_CYCLINGEVENTS, CYCLING_PATIENT_UID, "PATIENT_UID", FieldIsUUID);
    addField(Table_CYCLINGEVENTS, CYCLING_ISNOTRELATEDTOPATIENT, "ISRELTOPATIENT", FieldIsBoolean);
    addField(Table_CYCLINGEVENTS, CYCLING_ISVALID, "ISVALID", FieldIsBoolean);
    addField(Table_CYCLINGEVENTS, CYCLING_DATESTART, "DTSTART", FieldIsDate);
    addField(Table_CYCLINGEVENTS, CYCLING_DATEEND, "DTEND", FieldIsDate);
    addField(Table_CYCLINGEVENTS, CYCLING_REPEATINTERVAL, "INTERVAL", FieldIsInteger);
    addField(Table_CYCLINGEVENTS, CYCLING_REPEATSCHEMA, "SCHEME", FieldIsInteger);
    addField(Table_CYCLINGEVENTS, CYCLING_REPEATTIMES, "TIMES", FieldIsInteger); // (eg repeat only seven times)

    // informations
    addField(Table_VERSION, VERSION_ACTUAL,  "ACTUAL", FieldIsShortText);

    initialize();

    connect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
}

/** \brief Initialize the database. */
bool AgendaBase::initialize()
{
    if (m_initialized)
        return true;

    // connect
    createConnection(Constants::DB_NAME, Constants::DB_NAME,
                     settings()->databaseConnector(),
                     Utils::Database::CreateDatabase);

    if (!database().isOpen()) {
        if (!database().open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_NAME).arg(database().lastError().text()));
        } else {
            LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().connectionName()).arg(database().driverName()));
        }
    } else {
        LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().connectionName()).arg(database().driverName()));
    }

    if (!checkDatabaseScheme()) {
        LOG_ERROR(tkTr(Trans::Constants::DATABASE_1_SCHEMA_ERROR).arg(Constants::DB_NAME));
        return false;
    }

    if (!checkDatabaseVersion())
        return false;

    m_initialized = true;
    return true;
}

/** Return true if the AgendaBase is initialized and usable */
bool AgendaBase::isInitialized() const
{
    return m_initialized;
}

/** Return true if the AgendaBase is the last version (database is updated by this member if needed) */
bool AgendaBase::checkDatabaseVersion()
{
    /** \todo Code : AgendaBase::checkDatabaseVersion() */
    return true;
}

/** Create the default users database if it does not exists. */
bool AgendaBase::createDatabase(const QString &connectionName , const QString &dbName,
                    const QString &pathOrHostName,
                    TypeOfAccess access, AvailableDrivers driver,
                    const QString &login, const QString &pass,
                    const int port,
                    CreationOption createOption
                   )
{
    Q_UNUSED(access);
    Q_UNUSED(createOption);

    if (connectionName != Constants::DB_NAME)
        return false;

    LOG(QCoreApplication::translate("AgendaBase",
                                    "Trying to create empty database. \n"
                                    "       Location : %1 \n"
                                    "       FileName: %2\n"
                                    "       Driver: %3")
        .arg(pathOrHostName, dbName).arg(driver));

    setConnectionName(connectionName);
    setDriver(driver);

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
    } else if (driver == MySQL) {
        /** \todo test grants here or before ? */
        if (QSqlDatabase::connectionNames().contains(connectionName)) {
            DB = QSqlDatabase::database(connectionName);
        } else {
            DB = QSqlDatabase::addDatabase("QMYSQL" , connectionName);
            DB.setHostName(pathOrHostName);
            DB.setUserName(login);
            DB.setPassword(pass);
            DB.setPort(port);
        }
        DB.setDatabaseName("mysql");
        if (!DB.open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(DB.connectionName()).arg(DB.lastError().text()));
            return false;
        }
        //        qWarning() << "createMySQLDatabase(dbName);";
        createMySQLDatabase(dbName);
        // change database connection
        DB.setDatabaseName(dbName);
        if (!DB.open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(DB.connectionName()).arg(DB.lastError().text()));
            return false;
        }
    }

    // create db structure
    if (createTables()) {
        LOG(tkTr(Trans::Constants::DATABASE_1_CORRECTLY_CREATED).arg(dbName));
    } else {
        LOG_ERROR(tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2)
                  .arg(dbName, DB.lastError().text()));
        return false;
    }

    // Table INFORMATIONS
    QSqlQuery query(DB);
    query.prepare(prepareInsertQuery(Constants::Table_VERSION));
    query.bindValue(Constants::VERSION_ACTUAL, Constants::DB_VERSION);
    if (!query.exec()) {
        LOG_QUERY_ERROR(query);
    }
    query.finish();

    // database is readable/writable
    LOG(tkTr(Trans::Constants::DATABASE_1_CORRECTLY_CREATED).arg(pathOrHostName + QDir::separator() + dbName));

    return true;
}

void AgendaBase::onCoreDatabaseServerChanged()
{
    m_initialized = false;
    if (QSqlDatabase::connectionNames().contains(Constants::DB_NAME)) {
        QSqlDatabase::removeDatabase(Constants::DB_NAME);
    }
    initialize();
}

/** Retreive all calendars of the user defined by its uuid \e userUuid. If the \e userUuid is empty, retrieve all calendars of the currently connected user. */
QList<Calendar::UserCalendar *> AgendaBase::getUserCalendars(const QString &userUuid)
{
    QList<Calendar::UserCalendar *> toReturn;
    if (!connectDatabase(Constants::DB_NAME, __LINE__))
        return toReturn;

    QString uid = userUuid;
    if (userUuid.isEmpty())
        uid = user()->uuid();

    Utils::JoinList joins;
    joins << Utils::Join(Constants::Table_CALENDAR, Constants::CAL_ID, Constants::Table_USERCALENDARS, Constants::USERCAL_CAL_ID);
    Utils::FieldList conds;
    conds << Utils::Field(Constants::Table_CALENDAR, Constants::CAL_ISVALID, "=1");
    conds << Utils::Field(Constants::Table_USERCALENDARS, Constants::USERCAL_USER_UUID, QString("='%1'").arg(uid));
    QSqlQuery query(database());
    if (query.exec(select(Constants::Table_CALENDAR, joins, conds))) {
        while (query.next()) {
            Calendar::UserCalendar *u = new Calendar::UserCalendar;
            // set private datas
//            u->setData(IUserCalendar::DbOnly_UserCalId, query.value());
            u->setData(Constants::Db_CalId, query.value(Constants::CAL_ID));
            u->setData(Constants::Db_CatId, query.value(Constants::CAL_CATEGORYID));
            u->setData(Constants::Db_IsValid, 1);
//            CAL_SITEUID,
//            CAL_XMLOPTIONS

            // set public datas
            u->setData(Calendar::UserCalendar::UserOwnerUid, uid);
            u->setData(Calendar::UserCalendar::Label, query.value(Constants::CAL_LABEL));
            u->setData(Calendar::UserCalendar::Description, query.value(Constants::CAL_FULLCONTENT));
            u->setData(Calendar::UserCalendar::Type, query.value(Constants::CAL_TYPE));
            u->setData(Calendar::UserCalendar::Status, query.value(Constants::CAL_STATUS));
            u->setData(Calendar::UserCalendar::IsDefault, query.value(Constants::CAL_DEFAULT));
            u->setData(Calendar::UserCalendar::IsPrivate, query.value(Constants::CAL_ISPRIVATE));
            u->setData(Calendar::UserCalendar::Password, query.value(Constants::CAL_PASSWORD));
            u->setData(Calendar::UserCalendar::AbsPathIcon, query.value(Constants::CAL_THEMEDICON));
            u->setModified(false);
            toReturn << u;
        }
    } else {
        LOG_QUERY_ERROR(query);
    }
    return toReturn;
}

/** Save the user's calendar Agenda::IUserCalendar \e calendar to database */
bool AgendaBase::saveUserCalendar(Calendar::UserCalendar *calendar)
{
    if (!connectDatabase(Constants::DB_NAME, __LINE__))
        return false;
    if (calendar->data(Constants::Db_CalId).isNull() ||
        !calendar->data(Constants::Db_CalId).isValid()) {
        // save
        QSqlQuery query(database());
        query.prepare(prepareInsertQuery(Constants::Table_CALENDAR));
        query.bindValue(Constants::CAL_ID, QVariant());
        query.bindValue(Constants::CAL_CATEGORYID, calendar->data(Constants::Db_CatId).toInt());
        query.bindValue(Constants::CAL_ISVALID, calendar->data(Constants::Db_IsValid).toInt());
        query.bindValue(Constants::CAL_STATUS, calendar->data(Calendar::UserCalendar::Type).toInt());
        query.bindValue(Constants::CAL_TYPE, calendar->data(Calendar::UserCalendar::Status).toInt());
        query.bindValue(Constants::CAL_DEFAULT, calendar->data(Calendar::UserCalendar::IsDefault).toInt());
        query.bindValue(Constants::CAL_ISPRIVATE, calendar->data(Calendar::UserCalendar::IsPrivate).toInt());
        query.bindValue(Constants::CAL_PASSWORD, calendar->data(Calendar::UserCalendar::Password));
        query.bindValue(Constants::CAL_SITEUID, calendar->data(Calendar::UserCalendar::LocationUid).toInt());
        query.bindValue(Constants::CAL_LABEL, calendar->data(Calendar::UserCalendar::Label));
        query.bindValue(Constants::CAL_THEMEDICON, calendar->data(Calendar::UserCalendar::AbsPathIcon));
        query.bindValue(Constants::CAL_XMLOPTIONS, calendar->xmlOptions());
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            return false;
        }
        calendar->setData(Constants::Db_CalId, query.lastInsertId());
        calendar->setModified(false);
        query.finish();

        if (calendar->data(Calendar::UserCalendar::UserOwnerUid).isNull() ||
            !calendar->data(Calendar::UserCalendar::UserOwnerUid).isValid()) {
            calendar->setData(Calendar::UserCalendar::UserOwnerUid, user()->uuid());
        }
        query.prepare(prepareInsertQuery(Constants::Table_USERCALENDARS));
        query.bindValue(Constants::USERCAL_ID, QVariant());
        query.bindValue(Constants::USERCAL_CAL_ID, calendar->data(Constants::Db_CalId).toInt());
        query.bindValue(Constants::USERCAL_USER_UUID, calendar->data(Calendar::UserCalendar::UserOwnerUid));
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            return false;
        }
        calendar->setData(Constants::Db_UserCalId, query.lastInsertId());
        calendar->setModified(false);
        query.finish();
    } else {
        // update
        QSqlQuery query(database());
        QHash<int, QString> where;
        where.insert(Constants::CAL_ID, "=" + calendar->data(Constants::Db_CalId).toString());
        query.prepare(prepareUpdateQuery(Constants::Table_CALENDAR,
                                         QList<int>()
                                         << Constants::CAL_CATEGORYID
                                         << Constants::CAL_ISVALID
                                         << Constants::CAL_ISPRIVATE
                                         << Constants::CAL_TYPE
                                         << Constants::CAL_STATUS
                                         << Constants::CAL_DEFAULT
                                         << Constants::CAL_PASSWORD
                                         << Constants::CAL_SITEUID
                                         << Constants::CAL_LABEL
                                         << Constants::CAL_THEMEDICON
                                         << Constants::CAL_XMLOPTIONS
                                         , where));
        query.bindValue(0, calendar->data(Constants::Db_CalId));
        query.bindValue(1, calendar->data(Constants::Db_IsValid).toInt());
        query.bindValue(2, calendar->data(Calendar::UserCalendar::IsPrivate).toInt());
        query.bindValue(3, calendar->data(Calendar::UserCalendar::Type).toInt());
        query.bindValue(4, calendar->data(Calendar::UserCalendar::Status).toInt());
        query.bindValue(5, calendar->data(Calendar::UserCalendar::IsDefault).toInt());
        query.bindValue(6, calendar->data(Calendar::UserCalendar::Password));
        query.bindValue(7, calendar->data(Calendar::UserCalendar::LocationUid));
        query.bindValue(8, calendar->data(Calendar::UserCalendar::Label));
        query.bindValue(9, calendar->data(Calendar::UserCalendar::AbsPathIcon));
        query.bindValue(10, calendar->xmlOptions());
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            return false;
        }
        calendar->setModified(false);
        query.finish();
    }
    return true;
}

/** Retrieve all events from database according to the Agenda::CalendarEventQuery \e calQuery. */
QList<Calendar::CalendarItem *> AgendaBase::getCalendarEvents(const CalendarEventQuery &calQuery)
{
    QList<Calendar::CalendarItem *> toReturn;
    if (!connectDatabase(Constants::DB_NAME, __LINE__)) {
        return toReturn;
    }
    QString req;
    QSqlQuery query(database());
    // get events according to the query
    Utils::JoinList joins;
//    joins << Utils::Join(Constants::Table_COMMON, Constants::COMMON_ID, Constants::Table_EVENTS, Constants::EVENT_COMMON_ID);
    Utils::FieldList conds;
    conds << Utils::Field(Constants::Table_CALENDAR, Constants::CAL_ISVALID, "=1");
    conds << Utils::Field(Constants::Table_EVENTS, Constants::EVENT_ISVALID, "=1");
    // Date conditions
    if (calQuery.hasDateRange()) {
        /** \todo code here: better management of dates in filters */
        conds << Utils::Field(Constants::Table_EVENTS, Constants::EVENT_DATESTART, QString(">='%1'").arg(calQuery.dateStart().toString(Qt::ISODate)));
        conds << Utils::Field(Constants::Table_EVENTS, Constants::EVENT_DATEEND, QString("<='%1'").arg(calQuery.dateEnd().toString(Qt::ISODate)));
    }
    // Calendars && User conditions
    if (calQuery.calendarIds().count()) {
        const QList<int> &ids = calQuery.calendarIds();
        Utils::Field calIdField = field(Constants::Table_CALENDAR, Constants::CAL_ID);
        QString w = "=";
        QString r = QString(" OR `%1`.`%2`=").arg(calIdField.tableName).arg(calIdField.fieldName);
        for(int i = 0; i < ids.count(); ++i) {
            w += QString::number(ids.at(i)) + r;
        }
        w.chop(r.length());
        conds << Utils::Field(Constants::Table_CALENDAR, Constants::CAL_ID, w);
    } else if (calQuery.useCurrentUser()) {
        joins << Utils::Join(Constants::Table_EVENTS, Constants::EVENT_CAL_ID, Constants::Table_USERCALENDARS, Constants::USERCAL_CAL_ID);
        joins << Utils::Join(Constants::Table_CALENDAR, Constants::CAL_ID, Constants::Table_USERCALENDARS, Constants::USERCAL_CAL_ID);
        conds << Utils::Field(Constants::Table_USERCALENDARS, Constants::USERCAL_USER_UUID, QString("='%1'").arg(user()->uuid()));
        conds << Utils::Field(Constants::Table_CALENDAR, Constants::CAL_DEFAULT, "=1");
    } else if (calQuery.usersUuid().count()) {
        const QStringList &uids = calQuery.usersUuid();
        Utils::Field uidField = field(Constants::Table_USERCALENDARS, Constants::USERCAL_USER_UUID);
        joins << Utils::Join(Constants::Table_EVENTS, Constants::EVENT_CAL_ID, Constants::Table_USERCALENDARS, Constants::USERCAL_CAL_ID);
        joins << Utils::Join(Constants::Table_CALENDAR, Constants::CAL_ID, Constants::Table_USERCALENDARS, Constants::USERCAL_CAL_ID);
        QString w = "=";
        QString r = QString(" OR `%1`.`%2`=").arg(uidField.tableName).arg(uidField.fieldName);
        for(int i = 0; i < uids.count(); ++i) {
            w += "'" + uids.at(i) + "'" + r;
        }
        w.chop(r.length());
        conds << Utils::Field(Constants::Table_USERCALENDARS, Constants::USERCAL_USER_UUID, w);
    }

    // Manage pagination
    //     Add order
    QString order = QString("\n ORDER BY `%1`.`%2` ASC").arg(table(Constants::Table_EVENTS)).arg(fieldName(Constants::Table_EVENTS, Constants::EVENT_ID));


    // Get Event table
    req = select(Constants::Table_EVENTS, joins, conds) + order;

//    qWarning() << req;

    if (query.exec(req)) {
        while (query.next()) {
            Calendar::CalendarItem *ev = new Calendar::CalendarItem;
            ev->setData(Constants::Db_EvId, query.value(Constants::EVENT_ID));
            ev->setData(Constants::Db_CalId, query.value(Constants::EVENT_CAL_ID));
            ev->setData(Constants::Db_ComId, query.value(Constants::EVENT_COMMON_ID));
            ev->setData(Constants::Db_IsValid, query.value(Constants::EVENT_ISVALID));
//            ev->setData(ICalendarEvent::PatientUid, query.value(Constants::EVENT_PATIENT_UID));
            ev->setData(Calendar::CalendarItem::Uid, query.value(Constants::EVENT_CAL_ID));
            ev->setData(Calendar::CalendarItem::DateStart, query.value(Constants::EVENT_DATESTART));
            ev->setData(Calendar::CalendarItem::DateEnd, query.value(Constants::EVENT_DATEEND));
            toReturn << ev;
        }
    } else {
        LOG_QUERY_ERROR(query);
    }
    query.finish();

    // Get common data
    QHash<int, QString> where;
    for(int i = 0; i < toReturn.count(); ++i) {
        Calendar::CalendarItem *ev = toReturn.at(i);
        where.insert(Constants::COMMON_ID, "=" + ev->data(Constants::Db_EvId).toString());
        req = select(Constants::Table_COMMON, where);
        if (query.exec(req)) {
            if (query.next()) {
                ev->setData(Constants::Db_CatId, query.value(Constants::COMMON_CAT_ID));
                ev->setData(Calendar::CalendarItem::Type, query.value(Constants::COMMON_TYPE_ID));
                ev->setData(Calendar::CalendarItem::Status, query.value(Constants::COMMON_STATUS_ID));
                ev->setData(Calendar::CalendarItem::LocationUid, query.value(Constants::COMMON_SITE_UID));
                ev->setData(Calendar::CalendarItem::IsPrivate, query.value(Constants::COMMON_ISPRIVATE));
                ev->setData(Calendar::CalendarItem::Password, query.value(Constants::COMMON_PASSWORD));
                ev->setData(Calendar::CalendarItem::IsBusy, query.value(Constants::COMMON_ISBUSY));
                ev->setData(Calendar::CalendarItem::IsAGroupEvent, query.value(Constants::COMMON_ISAGROUPEVENT));
                ev->setData(Calendar::CalendarItem::Label, query.value(Constants::COMMON_LABEL));
                ev->setData(Calendar::CalendarItem::Description, query.value(Constants::COMMON_FULLCONTENT));
                ev->setData(Calendar::CalendarItem::Location, query.value(Constants::COMMON_TEXTUAL_SITE));
                ev->setData(Constants::Db_XmlViewOptions, query.value(Constants::COMMON_XMLVIEWOPTIONS));
                ev->setData(Constants::Db_XmlOptions, query.value(Constants::COMMON_XMLCALOPTIONS));
                ev->setModified(false);
            }
        } else {
            LOG_QUERY_ERROR(query);
        }
        query.finish();
    }
    return toReturn;
}

//bool AgendaBase::updateCyclingEvent(Agenda::ICalendarCyclingEvent *event)
//{
//    if (!event->isCycling())
//        return false;

//    return true;
//}

//bool AgendaBase::updateNonCyclingEvent(Agenda::ICalendarEvent *event)
//{
//    if (event->isCycling())
//        return false;

//    return true;
//}

/** Save the common part of events and set the commonId() in the event */
bool AgendaBase::saveCommonEvent(Calendar::CalendarItem *event)
{
    QSqlQuery query(database());
    if (event->data(Constants::Db_ComId).isNull() ||
        !event->data(Constants::Db_ComId).isValid()) {
        // save
        query.prepare(prepareInsertQuery(Constants::Table_COMMON));
        query.bindValue(Constants::COMMON_ID, QVariant());
        query.bindValue(Constants::COMMON_CAT_ID, event->data(Constants::Db_CalId));
        query.bindValue(Constants::COMMON_TYPE_ID, event->data(Calendar::CalendarItem::Type));
        query.bindValue(Constants::COMMON_STATUS_ID, event->data(Calendar::CalendarItem::Status));
        query.bindValue(Constants::COMMON_SITE_UID, event->data(Calendar::CalendarItem::LocationUid));
        query.bindValue(Constants::COMMON_ISPRIVATE, event->data(Calendar::CalendarItem::IsPrivate).toInt());
        query.bindValue(Constants::COMMON_PASSWORD, event->data(Calendar::CalendarItem::Password));
        query.bindValue(Constants::COMMON_ISBUSY, event->data(Calendar::CalendarItem::IsBusy).toInt());
        query.bindValue(Constants::COMMON_ISAGROUPEVENT, event->data(Calendar::CalendarItem::IsAGroupEvent).toInt());
        query.bindValue(Constants::COMMON_LABEL, event->data(Calendar::CalendarItem::Label));
        query.bindValue(Constants::COMMON_FULLCONTENT, event->data(Calendar::CalendarItem::Description));
        query.bindValue(Constants::COMMON_TEXTUAL_SITE, event->data(Calendar::CalendarItem::Location));
        query.bindValue(Constants::COMMON_THEMEDICON, event->data(Calendar::CalendarItem::IconPath));
        query.bindValue(Constants::COMMON_XMLVIEWOPTIONS, event->data(Constants::Db_XmlViewOptions));
        query.bindValue(Constants::COMMON_XMLCALOPTIONS, event->data(Constants::Db_XmlOptions));
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            return false;
        }
        int commonId = query.lastInsertId().toInt();
        event->setData(Constants::Db_ComId, commonId);
        event->setModified(false);
    } else {
        // update
        QHash<int, QString> where;
        where.insert(Constants::COMMON_ID, "=" + event->data(Constants::Db_ComId).toString());
        query.prepare(prepareUpdateQuery(Constants::Table_COMMON,
                                         QList<int>()
                                         << Constants::COMMON_CAT_ID
                                         << Constants::COMMON_TYPE_ID
                                         << Constants::COMMON_STATUS_ID
                                         << Constants::COMMON_SITE_UID
                                         << Constants::COMMON_ISPRIVATE
                                         << Constants::COMMON_PASSWORD
                                         << Constants::COMMON_ISBUSY
                                         << Constants::COMMON_ISAGROUPEVENT
                                         << Constants::COMMON_LABEL
                                         << Constants::COMMON_FULLCONTENT
                                         << Constants::COMMON_TEXTUAL_SITE
                                         << Constants::COMMON_THEMEDICON
                                         << Constants::COMMON_XMLVIEWOPTIONS
                                         << Constants::COMMON_XMLCALOPTIONS
                                         , where));
        query.bindValue(0, event->data(Constants::Db_CalId));
        query.bindValue(1, event->data(Calendar::CalendarItem::Type));
        query.bindValue(2, event->data(Calendar::CalendarItem::Status));
        query.bindValue(3, event->data(Calendar::CalendarItem::LocationUid));
        query.bindValue(4, event->data(Calendar::CalendarItem::IsPrivate).toInt());
        query.bindValue(5, event->data(Calendar::CalendarItem::Password));
        query.bindValue(6, event->data(Calendar::CalendarItem::IsBusy));
        query.bindValue(7, event->data(Calendar::CalendarItem::IsAGroupEvent));
        query.bindValue(8, event->data(Calendar::CalendarItem::Label));
        query.bindValue(9, event->data(Calendar::CalendarItem::Description));
        query.bindValue(10, event->data(Calendar::CalendarItem::Location));
        query.bindValue(11, event->data(Calendar::CalendarItem::IconPath));
        query.bindValue(12, event->data(Constants::Db_XmlViewOptions));
        query.bindValue(13, event->data(Constants::Db_XmlOptions));
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            return false;
        }
    }
    query.finish();
    return true;
}

//bool AgendaBase::saveCyclingEvent(Agenda::ICalendarCyclingEvent *event)
//{
//    if (!event->isCycling())
//        return false;

//    return true;
//}

bool AgendaBase::saveNonCyclingEvent(Calendar::CalendarItem *event)
{
    if (event->isCycling())
        return false;

    if (!saveCommonEvent(event))
        return false;

    QSqlQuery query(database());
    query.prepare(prepareInsertQuery(Constants::Table_EVENTS));
    query.bindValue(Constants::EVENT_ID, QVariant());
    query.bindValue(Constants::EVENT_CAL_ID, event->data(Constants::Db_CalId));
    query.bindValue(Constants::EVENT_COMMON_ID, event->data(Constants::Db_ComId));
    query.bindValue(Constants::EVENT_PATIENT_UID, QVariant()); //event->patients());
    query.bindValue(Constants::EVENT_ISVALID, event->data(Constants::Db_IsValid));
    query.bindValue(Constants::EVENT_DATESTART, event->data(Calendar::CalendarItem::DateStart));
    query.bindValue(Constants::EVENT_DATEEND, event->data(Calendar::CalendarItem::DateEnd));

    /**
      \todo 1 event == N patients
      \todo 1 event == N users
    */

    if (!query.exec()) {
        LOG_QUERY_ERROR(query);
        return false;
    }
    event->setData(Constants::Db_EvId, query.lastInsertId());
    event->setModified(false);
    query.finish();
    return true;
}

/** Save or update events in the database, when saved events are set to non modified. Return false in case of an error. */
bool AgendaBase::saveCalendarEvents(const QList<Calendar::CalendarItem *> &events)
{
    if (!connectDatabase(Constants::DB_NAME, __LINE__))
        return false;
    // update or save ?
    QList<Calendar::CalendarItem *> save, update;
    bool ok = true;
    for(int i = 0; i < events.count(); ++i) {
        Calendar::CalendarItem *ev = events.at(i);
        if (ev->isCycling()) {
            //                if (!updateCyclingEvent(static_cast<ICalendarCyclingEvent*>(ev)))
            //                    ok = false;
        } else {
            if (ev->data(Constants::Db_EvId).isNull() || ev->data(Constants::Db_EvId).toInt()==-1) {
                if (!saveNonCyclingEvent(ev))
                    ok = false;
//                else if (!updateNonCyclingEvent(ev))
//                    ok = false;
            }
        }
    }
    return ok;
}

/** Save or update one event in the database, when saved events are set to non modified. Return false in case of an error. */
bool AgendaBase::saveCalendarEvent(Calendar::CalendarItem *event)
{
    return saveCalendarEvents(QList<Calendar::CalendarItem *>() << event);
}

