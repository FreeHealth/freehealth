/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
  \class Agenda::CalendarEventQuery
  Define a query for the AgendaBase. All dates are included in the extraction.
*/

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

#include <patientbaseplugin/patientmodel.h>

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

enum { WarnNextAvailableTimeWarnings = false };

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
            Utils::Log::addError("AgendaBase", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                      .arg(db.connectionName()).arg(db.lastError().text()), __FILE__, line);
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

void CalendarEventQuery::setDateRange(const QDate &start, const QDate &end)
{
    m_DateStart = QDateTime(start, QTime(0,0,0));
    m_DateEnd = QDateTime(end, QTime(23,59,59));
}

void CalendarEventQuery::setDateRangeForDay(const QDate &day)
{
    m_DateStart = QDateTime(day, QTime(0,0,0));
    m_DateEnd = QDateTime(day, QTime(23,59,59));
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

void CalendarEventQuery::setDateRangeForCurrentMonth()
{
    QDate first = QDate(QDate::currentDate().year(), QDate::currentDate().month(), 1);
    m_DateStart = QDateTime(first, QTime(0,0,0));;
    m_DateEnd = m_DateStart.addMonths(1);
}

void CalendarEventQuery::setDateRangeForCurrentYear()
{
    QDate first = QDate(QDate::currentDate().year(), 1, 1);
    m_DateStart = QDateTime(first, QTime(0,0,0));;
    m_DateEnd = m_DateStart.addMonths(12);
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
    addTable(Table_AVAILABILITIES, "AVAIL");
    addTable(Table_TIMERANGE, "TR");
    addTable(Table_AVAIL_TO_TIMERANGE, "AV_TO_TR");
    addTable(Table_USERCALENDARS, "USERCAL");
    addTable(Table_COMMON, "COMMON");
    addTable(Table_EVENTS, "EVENTS");
    addTable(Table_CYCLINGEVENTS, "CYCLING");
    addTable(Table_PEOPLE, "PEOPLE");
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
    addField(Table_CALENDAR, CAL_DEFAULTDURATION, "DEFDURATION", FieldIsInteger);
    addField(Table_CALENDAR, CAL_LABEL, "LABEL", FieldIsShortText);
    addField(Table_CALENDAR, CAL_THEMEDICON, "THEMEDICON", FieldIsShortText);
    addField(Table_CALENDAR, CAL_FULLCONTENT, "FULLCONTENT", FieldIsBlob);
    addField(Table_CALENDAR, CAL_XMLOPTIONS, "XMLOPTIONS", FieldIsBlob);

    addField(Table_AVAILABILITIES, AVAIL_ID, "AVAIL_ID", FieldIsUniquePrimaryKey);
    addField(Table_AVAILABILITIES, AVAIL_CAL_ID, "CAL_ID", FieldIsInteger);
    addField(Table_AVAILABILITIES, AVAIL_WEEKDAY, "WEEKDAY", FieldIsInteger);

    addField(Table_AVAIL_TO_TIMERANGE, AVTOTR_AVID, "AVAIL_ID", FieldIsInteger);
    addField(Table_AVAIL_TO_TIMERANGE, AVTOTR_TRID, "TR_ID", FieldIsInteger);

    addField(Table_TIMERANGE, TIMERANGE_ID, "TR_ID", FieldIsUniquePrimaryKey);
    addField(Table_TIMERANGE, TIMERANGE_FROM, "FROM", FieldIsShortText);
    addField(Table_TIMERANGE, TIMERANGE_TO, "TO", FieldIsShortText);

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

    addField(Table_PEOPLE, PEOPLE_ID, "P_IP", FieldIsUniquePrimaryKey);
    addField(Table_PEOPLE, PEOPLE_EVENT_ID, "P_EVID", FieldIsInteger);
    addField(Table_PEOPLE, PEOPLE_UID, "P_UID", FieldIsUUID);
    addField(Table_PEOPLE, PEOPLE_TYPE, "P_TYPE", FieldIsInteger);

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
            u->setData(Calendar::UserCalendar::DefaultDuration, query.value(Constants::CAL_DEFAULTDURATION));
            u->setData(Calendar::UserCalendar::AbsPathIcon, query.value(Constants::CAL_THEMEDICON));
            u->setModified(false);
            toReturn << u;
        }
    } else {
        LOG_QUERY_ERROR(query);
    }
    query.finish();

    // Get availabilities
    Utils::FieldList get;
    get << field(Constants::Table_AVAILABILITIES, Constants::AVAIL_ID);
    get << field(Constants::Table_AVAILABILITIES, Constants::AVAIL_WEEKDAY);
    get << field(Constants::Table_TIMERANGE, Constants::TIMERANGE_ID);
    get << field(Constants::Table_TIMERANGE, Constants::TIMERANGE_FROM);
    get << field(Constants::Table_TIMERANGE, Constants::TIMERANGE_TO);
    get << field(Constants::Table_AVAILABILITIES, Constants::AVAIL_CAL_ID);
    joins.clear();
//    joins << Utils::Join(Constants::Table_CALENDAR, Constants::CAL_ID, Constants::Table_AVAILABILITIES, Constants::AVAIL_CAL_ID);
    joins << Utils::Join(Constants::Table_AVAIL_TO_TIMERANGE, Constants::AVTOTR_AVID, Constants::Table_AVAILABILITIES, Constants::AVAIL_ID);
    joins << Utils::Join(Constants::Table_TIMERANGE, Constants::TIMERANGE_ID, Constants::Table_AVAIL_TO_TIMERANGE, Constants::AVTOTR_TRID);
    for(int i = 0; i < toReturn.count(); ++i) {
        Calendar::UserCalendar *u = toReturn.at(i);
        int lastAvId = -1;
        int lastWeekDay = -1;
        QVector<Calendar::TimeRange> tr;
        conds.clear();
        conds << Utils::Field(Constants::Table_AVAILABILITIES, Constants::AVAIL_CAL_ID, QString("=%1").arg(u->data(Constants::Db_CalId).toString()));
        const Utils::Field &f = field(Constants::Table_AVAILABILITIES, Constants::AVAIL_ID);
        QString req = select(get, joins, conds) + QString(" ORDER BY `%1`.`%2`").arg(f.tableName).arg(f.fieldName);
        if (query.exec(req)) {
            while (query.next()) {
//                qWarning() << "day" << query.value(1).toInt() << "calId" << query.value(5).toInt();
                if (lastAvId==-1) {
                    lastAvId = query.value(0).toInt();
                    lastWeekDay = query.value(1).toInt();
                } else if (lastAvId != query.value(0).toInt()) {
                    // Create availability
                    Calendar::DayAvailability av;
                    av.setId(lastAvId);
                    av.setWeekDay(lastWeekDay);
                    av.setTimeRanges(tr);
                    u->addAvailabilities(av);
                    tr.clear();
                    lastAvId = query.value(0).toInt();
                    lastWeekDay = query.value(1).toInt();
                }
                Calendar::TimeRange t;
                t.id = query.value(2).toInt();
                t.from = QTime::fromString(query.value(3).toString());
                t.to = QTime::fromString(query.value(4).toString());
                tr.append(t);
            }
            // Create last availability
            Calendar::DayAvailability av;
            av.setId(lastAvId);
            av.setWeekDay(lastWeekDay);
            av.setTimeRanges(tr);
            u->addAvailabilities(av);
            tr.clear();
        } else {
            LOG_QUERY_ERROR(query);
        }
    }
    query.finish();

    return toReturn;
}

/** Save the user's calendar availabilities for the specified \e calendar to database. The \e calendar is modified during this process (ids are set if needed). */
bool AgendaBase::saveCalendarAvailabilities(Calendar::UserCalendar *calendar)
{
    return true;
}

/** Save the user's calendar Agenda::IUserCalendar \e calendar to database. The \e calendar is modified during this process (ids are set if needed). */
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
        query.bindValue(Constants::CAL_DEFAULTDURATION, calendar->data(Calendar::UserCalendar::DefaultDuration));
        query.bindValue(Constants::CAL_FULLCONTENT, calendar->data(Calendar::UserCalendar::Description));
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

        if (calendar->hasAvailability()) {
            QVector<Calendar::DayAvailability> av = calendar->availabilities();
            QHash<int, Calendar::DayAvailability> hashAv;
            // fusion availabilties by days
            for(int i = 0; i < av.count(); ++i) {
                if (hashAv.contains(av.at(i).weekDay())) {
                    for(int z = 0; z < av.at(i).timeRangeCount(); ++z) {
                        hashAv[i].addTimeRange(av.at(i).timeRange(z));
                    }
                } else {
                    hashAv.insert(av.at(i).weekDay(), av.at(i));
                }
            }

            QHashIterator<int, Calendar::DayAvailability> it(hashAv);
            while (it.hasNext()) {
                it.next();
                query.prepare(prepareInsertQuery(Constants::Table_AVAILABILITIES));
                query.bindValue(Constants::AVAIL_ID, QVariant());
                query.bindValue(Constants::AVAIL_CAL_ID, calendar->data(Constants::Db_CalId));
                query.bindValue(Constants::AVAIL_WEEKDAY, it.key());
                if (!query.exec()) {
                    LOG_QUERY_ERROR(query);
                    continue;
                }
                int avId = query.lastInsertId().toInt();
                query.finish();

                // save all timeRanges
                QVector<Calendar::TimeRange> newRanges;
                for(int z = 0; z < it.value().timeRangeCount(); ++z) {
                    Calendar::TimeRange range = it.value().timeRange(z);
                    query.prepare(prepareInsertQuery(Constants::Table_TIMERANGE));
                    query.bindValue(Constants::TIMERANGE_ID, QVariant());
                    query.bindValue(Constants::TIMERANGE_FROM, range.from.toString());
                    query.bindValue(Constants::TIMERANGE_TO, range.to.toString());
                    if (!query.exec()) {
                        LOG_QUERY_ERROR(query);
                        continue;
                    }
                    int trId = query.lastInsertId().toInt();
                    range.id = trId;
                    query.finish();
                    newRanges << range;

                    query.prepare(prepareInsertQuery(Constants::Table_AVAIL_TO_TIMERANGE));
                    query.bindValue(Constants::AVTOTR_AVID, avId);
                    query.bindValue(Constants::AVTOTR_TRID, trId);
                    if (!query.exec()) {
                        LOG_QUERY_ERROR(query);
                        continue;
                    }
                }
                hashAv[it.key()].setTimeRanges(newRanges);
                hashAv[it.key()].setId(avId);
            }
            calendar->setAvailabilities(hashAv.values());
        }

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
                                         << Constants::CAL_DEFAULTDURATION
                                         << Constants::CAL_FULLCONTENT
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
        query.bindValue(11, calendar->data(Calendar::UserCalendar::DefaultDuration));
        query.bindValue(12, calendar->data(Calendar::UserCalendar::Description));
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            return false;
        }
        calendar->setModified(false);
        query.finish();

        /** \todo update availabilities */
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
        joins << Utils::Join(Constants::Table_EVENTS, Constants::EVENT_CAL_ID, Constants::Table_CALENDAR, Constants::CAL_ID);
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

        // Get peoples
        getRelatedPeoples(ev);
    }

//    getPatientNames(toReturn);

    return toReturn;
}

/** Save or update the common part of events and set the commonId() in the event */
bool AgendaBase::saveCommonEvent(Calendar::CalendarItem *event)
{
    QSqlQuery query(database());
    if (event->data(Constants::Db_EvId).isNull() ||
            !event->data(Constants::Db_EvId).isValid() ||
            event->data(Constants::Db_EvId).toInt() == -1) {
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

/** Save the peoples related to an \e event. */
bool AgendaBase::saveEventRelatedPeoples(const Calendar::CalendarItem *event)
{
    QSqlQuery query(database());
    if (event->data(Constants::Db_EvId).isNull() ||
            !event->data(Constants::Db_EvId).isValid() ||
            event->data(Constants::Db_EvId).toInt() == -1) {
        LOG_ERROR("No Event ID");
        return false;
    }

    // People
    const QStringList &attendees = event->peopleUids(Calendar::CalendarItem::PeopleAttendee);
    for(int i = 0; i < attendees.count(); ++i) {
        query.prepare(prepareInsertQuery(Table_PEOPLE));
        query.bindValue(PEOPLE_ID, QVariant());
        query.bindValue(PEOPLE_EVENT_ID, event->data(Constants::Db_EvId));
        query.bindValue(PEOPLE_UID, attendees.at(i));
        query.bindValue(PEOPLE_TYPE, Calendar::CalendarItem::PeopleAttendee);
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            return false;
        }
        query.finish();
    }
    const QStringList &users = event->peopleUids(Calendar::CalendarItem::PeopleUser);
    for(int i = 0; i < users.count(); ++i) {
        query.prepare(prepareInsertQuery(Table_PEOPLE));
        query.bindValue(PEOPLE_ID, QVariant());
        query.bindValue(PEOPLE_EVENT_ID, event->data(Constants::Db_EvId));
        query.bindValue(PEOPLE_UID, users.at(i));
        query.bindValue(PEOPLE_TYPE, Calendar::CalendarItem::PeopleUser);
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            return false;
        }
        query.finish();
    }
    return true;
}

/** Retrieve the peoples related to an \e event. */
bool AgendaBase::getRelatedPeoples(Calendar::CalendarItem *event)
{
    if (!connectDatabase(Constants::DB_NAME, __LINE__)) {
        return false;
    }
    QSqlQuery query(database());
    QHash<int, QString> where;
    where.insert(Constants::PEOPLE_EVENT_ID, "=" + event->data(Constants::Db_EvId).toString());
    QString req = select(Table_PEOPLE, where);
    if (query.exec(req)) {
        while (query.next()) {
            event->addPeople(Calendar::CalendarItem::PeopleType(query.value(PEOPLE_TYPE).toInt()),
                             "", query.value(PEOPLE_UID).toString());
        }
    } else {
        LOG_QUERY_ERROR(query);
        return false;
    }
    return true;
}

bool AgendaBase::getPatientNames(const QList<Calendar::CalendarItem *> &items)
{
    QTime chr;
    chr.start();

    // get all patient uids
    QStringList uids;
    for(int i = 0; i < items.count(); ++i) {
        uids << items.at(i)->peopleUids(Calendar::CalendarItem::PeopleAttendee);
    }
    uids.removeAll("");

    // retrieve names
    QHash<QString, QString> names = Patients::PatientModel::patientName(uids);

    qWarning() << chr.elapsed();
    chr.restart();

    // feed calendaritems
    for(int i = 0; i < items.count(); ++i) {
        QStringList l = items.at(i)->peopleUids(Calendar::CalendarItem::PeopleAttendee);
        for(int j = 0; j < l.count(); ++j) {
            QString name = names.value(l.at(j));
            items.at(i)->setPeopleName(Calendar::CalendarItem::PeopleAttendee, l.at(j), name);
        }
    }

    qWarning() << chr.elapsed();
    return true;
}

bool AgendaBase::getPatientNames(Calendar::CalendarItem *item)
{
    return getPatientNames(QList<Calendar::CalendarItem *>() << item);
}

/** Save or update a Calendar::CalendarItem to the agenda database */
bool AgendaBase::saveNonCyclingEvent(Calendar::CalendarItem *event)
{
    if (event->isCycling())
        return false;

    if (!saveCommonEvent(event))
        return false;

    QSqlQuery query(database());
    if (event->data(Constants::Db_EvId).isNull() ||
            !event->data(Constants::Db_EvId).isValid() ||
            event->data(Constants::Db_EvId).toInt() == -1) {
        // save
        query.prepare(prepareInsertQuery(Constants::Table_EVENTS));
        query.bindValue(Constants::EVENT_ID, QVariant());
        query.bindValue(Constants::EVENT_CAL_ID, event->data(Constants::Db_CalId));
        query.bindValue(Constants::EVENT_COMMON_ID, event->data(Constants::Db_ComId));
        query.bindValue(Constants::EVENT_PATIENT_UID, QVariant()); //event->patients());
        query.bindValue(Constants::EVENT_ISVALID, event->data(Constants::Db_IsValid));
        query.bindValue(Constants::EVENT_DATESTART, event->data(Calendar::CalendarItem::DateStart));
        query.bindValue(Constants::EVENT_DATEEND, event->data(Calendar::CalendarItem::DateEnd));
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            return false;
        }
        event->setData(Constants::Db_EvId, query.lastInsertId());
        query.finish();

        event->setModified(false);

        if (!saveEventRelatedPeoples(event))
            return false;

    } else {
        // update
        QHash<int, QString> where;
        where.insert(Constants::EVENT_ID, "=" + event->data(Constants::Db_EvId).toString());
        query.prepare(prepareUpdateQuery(Constants::Table_EVENTS,
                                         QList<int>()
                                         << Constants::EVENT_CAL_ID
                                         << Constants::EVENT_ISVALID
                                         << Constants::EVENT_DATESTART
                                         << Constants::EVENT_DATEEND
                                         , where));
        query.bindValue(0, event->data(Constants::Db_CalId).toInt());
        query.bindValue(1, event->data(Constants::Db_IsValid).toInt());
        query.bindValue(2, event->data(Calendar::CalendarItem::DateStart));
        query.bindValue(3, event->data(Calendar::CalendarItem::DateEnd));
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            return false;
        }
        query.finish();

        // update peoples
        where.clear();
        where.insert(Constants::PEOPLE_EVENT_ID, "=" + event->data(Constants::Db_EvId).toString());
        query.exec(prepareDeleteQuery(Table_PEOPLE, where));
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            return false;
        }
        query.finish();

        if (!saveEventRelatedPeoples(event))
            return false;
    }
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

QList<QDateTime> AgendaBase::nextAvailableTime(const QDateTime &startSearch, const int durationInMinutes, const Calendar::UserCalendar &calendar, const int numberOfDates)
{
    QList<QDateTime> toReturn;
    if (!connectDatabase(Constants::DB_NAME, __LINE__))
        return toReturn;

    if (WarnNextAvailableTimeWarnings)
        qWarning() << Q_FUNC_INFO<< startSearch << durationInMinutes << calendar.data(Constants::Db_CalId).toInt();

    QSqlQuery query(database());
    int limit = 0;
    int durationInSeconds = durationInMinutes * 60;
    QDateTime lastEnd, newStart;
//    lastEnd = startSearch;
    Utils::FieldList get;
    get << Utils::Field(Table_EVENTS, EVENT_DATESTART);
    get << Utils::Field(Table_EVENTS, EVENT_DATEEND);
    Utils::JoinList joins;
    // no joins
    Utils::FieldList conds;
    conds << Utils::Field(Table_EVENTS, EVENT_ISVALID, "=1");
    conds << Utils::Field(Table_EVENTS, EVENT_CAL_ID, QString("=%1").arg(calendar.data(Constants::Db_CalId).toString()));
    conds << Utils::Field(Table_EVENTS, EVENT_DATESTART, QString("> '%1'").arg(startSearch.toString(Qt::ISODate)));
    QString req = select(get, joins, conds);
    req + QString("\n ORDER BY `%1`.`%2`\n")
            .arg(table(Table_EVENTS))
            .arg(fieldName(Table_EVENTS, EVENT_DATESTART));

    // get the first event and make tests
    QString l = QString(" LIMIT 0,1");
    if (query.exec(req+l)) {
        if (query.next()) {
            newStart = query.value(0).toDateTime();
            lastEnd = query.value(1).toDateTime();
        }
    } else {
        LOG_QUERY_ERROR(query);
        return toReturn;
    }
    if (lastEnd.isNull()) {
        // No events recorded -> use calendar dayAvailabilities only
        if (WarnNextAvailableTimeWarnings)
            qWarning() << "  UseCal limits" << lastEnd << newStart << startSearch;

//    } else if (newStart.isValid() && startSearch.addSecs(durationInSeconds) < newStart)) {
    }

    ++limit;

    // Test with the next 100000 events
    for(int i = 0; i < 1000; ++i) {
        l = QString(" LIMIT %1, %2\n").arg(limit).arg(limit+100);
        limit += 100;
        if (!query.exec(req+l)) {
            LOG_QUERY_ERROR(query);
            break;
        }
        if (query.size()==0) {
            break;
        }
        while (query.next()) {
            newStart = query.value(0).toDateTime();

            // Skip intersected date
            if ((newStart < lastEnd) || (lastEnd > newStart)) {
                lastEnd = query.value(1).toDateTime();
                continue;
            }

            // time between the two dates >= duration ?
            QDateTime supposedEnd = lastEnd.addSecs(durationInSeconds);

            // supposedStart and supposedEnd must be the same day
            if (newStart.date().dayOfWeek() != supposedEnd.date().dayOfWeek()) {
                lastEnd = query.value(1).toDateTime();
                continue;
            }

            if (WarnNextAvailableTimeWarnings)
                qWarning() << "lastEnd" << lastEnd << "newStart" << newStart << "supposedEnd" << supposedEnd;

            if (supposedEnd < newStart) {
                if (calendar.canBeAvailable(lastEnd) && calendar.canBeAvailable(supposedEnd)) {
                    if (WarnNextAvailableTimeWarnings)
                        qWarning() << "   -> OK: start" << lastEnd  << "end" << supposedEnd;
                    toReturn << lastEnd;
                    if (toReturn.count() == numberOfDates)
                        return toReturn;
                }
            }

            lastEnd = query.value(1).toDateTime();
        }
    }

    if (WarnNextAvailableTimeWarnings)
        qWarning() << "No available time found";

    return toReturn;
}

QDateTime AgendaBase::nextAvailableTime(const QDateTime &startSearch, const int durationInMinutes, const Calendar::UserCalendar &calendar)
{
    QList<QDateTime> l = nextAvailableTime(startSearch, durationInMinutes, calendar, 1);
    if (l.count())
        return l.at(0);
    return QDateTime();
}
