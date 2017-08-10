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

/**
  \class Agenda::CalendarEventQuery
  Define a query for the AgendaBase. All dates are included in the extraction.
*/

/**
 * \class Agenda::AgendaBase
 * \brief Management of all database access for the agendas.
 * Database schema:
 * - CALENDAR, USERCALENDARS: each user can own multiple calendars.
 *   Table USERCALENDARS list all calendars own by users.
 *   Where as CALENDAR contains all needed information to
 *   manage calendars.
 * - EVENTS, CYCLINGEVENTS, COMMON: management of agenda events.
 * - USER_WORKINGTIME: management of users availabilities, vacancies...
 * - VERSION : contains the current version of the database
*/

#include "agendabase.h"
#include "constants.h"
#include "appointment.h"
#include "calendaritemmodel.h"
#include "nextavailabiliystepviewer.h"
#include "usercalendar.h"
#include "dayavailability.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/iuser.h>
#include <coreplugin/icommandline.h>

#include <patientbaseplugin/patientmodel.h>

#include <usermanagerplugin/usercore.h>
#include <usermanagerplugin/usermodel.h>

#include <printerplugin/textdocumentextra.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/databaseconnector.h>
#include <translationutils/constants.h>
#include <translationutils/trans_agenda.h>
#include <translationutils/trans_database.h>
#include <translationutils/trans_msgerror.h>

#include <QCoreApplication>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QFile>
#include <QDir>
#include <QLocale>
#include <QDateTime>

enum { WarnNextAvailableTimeWarnings = false };

using namespace Agenda;
using namespace Internal;
using namespace Agenda::Constants;
using namespace Trans::ConstantTranslations;

static inline Core::ICommandLine *commandLine()  { return Core::ICore::instance()->commandLine(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::IUser *user() {return Core::ICore::instance()->user();}
static inline UserPlugin::UserCore &userCore() {return UserPlugin::UserCore::instance();}
static inline UserPlugin::UserModel *userModel() {return userCore().userModel();}

static inline bool connectDatabase(QSqlDatabase &DB, const int line)
{
    if (!DB.isOpen()) {
        if (!DB.open()) {
            Utils::Log::addError("AgendaBase", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                 .arg(DB.connectionName()).arg(DB.lastError().text()),
                                 __FILE__, line);
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

AgendaBase::AgendaBase(QObject *parent) :
    QObject(parent), Utils::Database(),
    m_initialized(false), m_Next(new NextAvailabiliyManager)
{
    // The default instance is own by AgendaCore
    setObjectName("AgendaBase");

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
    addIndex(Table_USERCALENDARS, USERCAL_ID);
    addIndex(Table_USERCALENDARS, USERCAL_USER_UUID);
    addIndex(Table_USERCALENDARS, USERCAL_CAL_ID);

    addField(Table_CALENDAR, CAL_ID, "CAL_ID", FieldIsUniquePrimaryKey);
    addField(Table_CALENDAR, CAL_UID, "CALUID", FieldIsUUID);
    addField(Table_CALENDAR, CAL_CATEGORYID, "CAT_ID", FieldIsInteger);
    addField(Table_CALENDAR, CAL_SORTID, "SORT_ID", FieldIsInteger);
    addField(Table_CALENDAR, CAL_ISVALID, "ISVALID", FieldIsBoolean);
    addField(Table_CALENDAR, CAL_ISVIRTUAL, "ISVIRTUAL", FieldIsBoolean, "0");
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
    addIndex(Table_CALENDAR, CAL_ID);
    addIndex(Table_CALENDAR, CAL_SITEUID);

    addField(Table_AVAILABILITIES, AVAIL_ID, "AVAIL_ID", FieldIsUniquePrimaryKey);
    addField(Table_AVAILABILITIES, AVAIL_CAL_ID, "CAL_ID", FieldIsInteger);
    addField(Table_AVAILABILITIES, AVAIL_WEEKDAY, "WEEKDAY", FieldIsInteger);
    addIndex(Table_AVAILABILITIES, AVAIL_ID);
    addIndex(Table_AVAILABILITIES, AVAIL_CAL_ID);

    addField(Table_AVAIL_TO_TIMERANGE, AVTOTR_AVID, "AVAIL_ID", FieldIsInteger);
    addField(Table_AVAIL_TO_TIMERANGE, AVTOTR_TRID, "TR_ID", FieldIsInteger);
    addIndex(Table_AVAIL_TO_TIMERANGE, AVTOTR_AVID);
    addIndex(Table_AVAIL_TO_TIMERANGE, AVTOTR_TRID);

    addField(Table_TIMERANGE, TIMERANGE_ID, "TR_ID", FieldIsUniquePrimaryKey);
    addField(Table_TIMERANGE, TIMERANGE_FROM, "FROM", FieldIsTime);
    addField(Table_TIMERANGE, TIMERANGE_TO, "TO", FieldIsTime);
    addIndex(Table_TIMERANGE, TIMERANGE_ID);

    addField(Table_COMMON, COMMON_ID, "COM_ID", FieldIsUniquePrimaryKey);
    addField(Table_COMMON, COMMON_CAT_ID, "CAT_ID", FieldIsInteger);
    addField(Table_COMMON, COMMON_TYPE_ID, "TYPE_ID", FieldIsInteger);
    addField(Table_COMMON, COMMON_STATUS_ID, "STATUS_ID", FieldIsInteger); // (like draft, validated, rejected, need more info, need to send letter…)
    addField(Table_COMMON, COMMON_SITE_UID, "SITE_UID", FieldIsUUID); // (for a later use)
    addField(Table_COMMON, COMMON_ISVIRTUAL, "ISVIRTUAL", FieldIsBoolean, "0");
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
    addIndex(Table_COMMON, COMMON_ID);
    addIndex(Table_COMMON, COMMON_CAT_ID);
    addIndex(Table_COMMON, COMMON_TYPE_ID);
    addIndex(Table_COMMON, COMMON_STATUS_ID);
    addIndex(Table_COMMON, COMMON_SITE_UID);

    addField(Table_EVENTS, EVENT_ID, "EV_ID", FieldIsUniquePrimaryKey);
    addField(Table_EVENTS, EVENT_CAL_ID, "CAL_ID", FieldIsInteger);
    addField(Table_EVENTS, EVENT_COMMON_ID, "COM_ID", FieldIsInteger);
    addField(Table_EVENTS, EVENT_ISVALID, "ISVALID", FieldIsBoolean);
    addField(Table_EVENTS, EVENT_DATESTART, "DTSTART", FieldIsDateTime);
    addField(Table_EVENTS, EVENT_DATEEND, "DTEND", FieldIsDateTime);
    addIndex(Table_EVENTS, EVENT_ID);
    addIndex(Table_EVENTS, EVENT_CAL_ID);
    addIndex(Table_EVENTS, EVENT_COMMON_ID);

    // People are related to one of the following:
    //  - Event --> EVENT_ID is populated, CAL_ID is null
    //  - Calendar --> CAL_ID is populated, EVENT_ID is null
    addField(Table_PEOPLE, PEOPLE_ID, "P_IP", FieldIsUniquePrimaryKey);
    addField(Table_PEOPLE, PEOPLE_CAL_ID, "P_CALID", FieldIsInteger, "NULL");
    addField(Table_PEOPLE, PEOPLE_EVENT_ID, "P_EVID", FieldIsInteger, "NULL");
    addField(Table_PEOPLE, PEOPLE_UID, "P_UID", FieldIsUUID);
    addField(Table_PEOPLE, PEOPLE_TYPE, "P_TYPE", FieldIsInteger);
    addIndex(Table_PEOPLE, PEOPLE_ID);
    addIndex(Table_PEOPLE, PEOPLE_EVENT_ID);
    addIndex(Table_PEOPLE, PEOPLE_UID);

    addField(Table_CYCLINGEVENTS, CYCLING_ID, "CYC_ID", FieldIsUniquePrimaryKey);
    addField(Table_CYCLINGEVENTS, CYCLING_CALENDAR_ID, "CAL_ID", FieldIsInteger);
    addField(Table_CYCLINGEVENTS, CYCLING_COMMON_ID, "COM_ID", FieldIsInteger);

    // TODO: field is obsolete
    addField(Table_CYCLINGEVENTS, CYCLING_PATIENT_UID, "PATIENT_UID", FieldIsUUID);

    addField(Table_CYCLINGEVENTS, CYCLING_ISNOTRELATEDTOPATIENT, "ISRELTOPATIENT", FieldIsBoolean);
    addField(Table_CYCLINGEVENTS, CYCLING_ISVALID, "ISVALID", FieldIsBoolean);
    addField(Table_CYCLINGEVENTS, CYCLING_DATESTART, "DTSTART", FieldIsDate);
    addField(Table_CYCLINGEVENTS, CYCLING_DATEEND, "DTEND", FieldIsDate);
    addField(Table_CYCLINGEVENTS, CYCLING_REPEATINTERVAL, "INTERVAL", FieldIsInteger);
    addField(Table_CYCLINGEVENTS, CYCLING_REPEATSCHEMA, "SCHEME", FieldIsInteger);
    addField(Table_CYCLINGEVENTS, CYCLING_REPEATTIMES, "TIMES", FieldIsInteger); // (eg repeat only seven times)

    // TODO: add indexes
//    addIndex(Table_PEOPLE, PEOPLE_ID);
//    addIndex(Table_PEOPLE, PEOPLE_ID);
//    addIndex(Table_PEOPLE, PEOPLE_ID);

    // information
    addField(Table_VERSION, VERSION_ACTUAL,  "ACTUAL", FieldIsShortText);

    // Connect first run database creation requested
    connect(Core::ICore::instance(), SIGNAL(firstRunDatabaseCreation()), this, SLOT(onCoreFirstRunCreationRequested()));
}

AgendaBase::~AgendaBase()
{
    delete m_Next;
    m_Next = 0;
}

/** \brief Initialize the database. */
bool AgendaBase::initialize()
{
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
            LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().databaseName()).arg(database().driverName()));
        }
    } else {
        LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().databaseName()).arg(database().driverName()));
    }

    if (!checkDatabaseScheme()) {
        LOG_ERROR(tkTr(Trans::Constants::DATABASE_1_SCHEMA_ERROR).arg(Constants::DB_NAME));
        return false;
    }

    if (!checkDatabaseVersion())
        return false;

    connect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
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
    // TODO: Code : AgendaBase::checkDatabaseVersion()
    return true;
}

/** Create the default users database if it does not exist. */
bool AgendaBase::createDatabase(const QString &connectionName, const QString &dbName,
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
                                    "Trying to create empty database.\n"
                                    "       Location: %1\n"
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
                LOG(tkTr(Trans::Constants::_1_ISNOT_AVAILABLE_CANNOTBE_CREATED).arg(pathOrHostName));
        DB.setDatabaseName(QDir::cleanPath(pathOrHostName + QDir::separator() + dbName));
        if (!DB.open())
            LOG(tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2).arg(dbName).arg(DB.lastError().text()));
        setDriver(Utils::Database::SQLite);
    } else if (driver == MySQL) {
        // TODO: test grants here or before?
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

    // Add version number
    if (!setVersion(Utils::Field(Constants::Table_VERSION, Constants::VERSION_ACTUAL), Constants::DB_VERSION)) {
        LOG_ERROR_FOR("AgendaBase", "Unable to set version");
    }

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
    disconnect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
    disconnect(Core::ICore::instance(), SIGNAL(firstRunDatabaseCreation()), this, SLOT(onCoreFirstRunCreationRequested()));
    initialize();
}

void AgendaBase::onCoreFirstRunCreationRequested()
{
    disconnect(Core::ICore::instance(), SIGNAL(firstRunDatabaseCreation()), this, SLOT(onCoreFirstRunCreationRequested()));
    initialize();
}


/**
 * Creates an return an empty Agenda::UserCalendar pointer.
 * Agenda::DayAvailability are added all days. The calendar
 * is not defines as the default one.
 */
Agenda::UserCalendar *AgendaBase::createEmptyCalendar(const QString &userUid)
{
    Agenda::UserCalendar *u = new Agenda::UserCalendar;
    u->setData(Constants::Db_UserCalId, -1);
    u->setData(Constants::Db_IsValid, 1);
    u->setData(UserCalendar::IsDefault, 0);
    u->setData(UserCalendar::UserOwnerUid, userUid);
    u->setData(UserCalendar::Uid, Utils::Database::createUid());
    u->setData(UserCalendar::Label, tkTr(Trans::Constants::AGENDA));
    u->setData(UserCalendar::DefaultDuration, 5);
    // Create day availabilities
    for(int j=1; j < 8; ++j) {
        DayAvailability av;
        av.addTimeRange(QTime(06,00,00), QTime(20,00,00));
        av.setWeekDay(j);
        u->addAvailabilities(av);
    }
    return u;
}

/**
 * Return true if the user \e userUid has recorded calendar(s)
 * in the database.
 */
bool AgendaBase::hasCalendar(const QString &userUuid)
{
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_NAME);
    if (!connectDatabase(DB, __LINE__)) {
        return false;
    }
    DB.transaction();
    QString uid = userUuid;
    if (uid.isEmpty())
        uid = user()->uuid();
    Utils::Field get(Constants::Table_CALENDAR, Constants::CAL_ID);
    Utils::Join join(Constants::Table_CALENDAR, Constants::USERCAL_USER_UUID, Constants::Table_CALENDAR, Constants::CAL_ID);
    Utils::Field cond(Constants::Table_USERCALENDARS, Constants::USERCAL_USER_UUID, QString("='%1'").arg(uid));
    QSqlQuery query(DB);
    if (query.exec(select(get, join, cond))) {
        if (query.next()) {
            query.finish();
            DB.commit();
            return true;
        }
    } else {
        LOG_QUERY_ERROR(query);
    }
    query.finish();
    DB.commit();
    return false;
}

/**
 * Retreive all calendars of the user (own calendars and delegated ones)
 * defined by its uuid \e userUuid. If the \e userUuid is empty, retrieve
 * all calendars of the currently connected user.
 */
QList<Agenda::UserCalendar *> AgendaBase::getUserCalendars(const QString &userUuid)
{
    QList<Agenda::UserCalendar *> toReturn;
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_NAME);
    if (!connectDatabase(DB, __LINE__)) {
        return toReturn;
    }

    QString uid = userUuid;
    if (userUuid.isEmpty())
        uid = user()->uuid();

    QStringList reqs;
    Utils::JoinList joins;
    joins << Utils::Join(Constants::Table_CALENDAR, Constants::CAL_ID, Constants::Table_USERCALENDARS, Constants::USERCAL_CAL_ID);
    Utils::FieldList conds;
    conds << Utils::Field(Constants::Table_CALENDAR, Constants::CAL_ISVALID, "=1");
    conds << Utils::Field(Constants::Table_USERCALENDARS, Constants::USERCAL_USER_UUID, QString("='%1'").arg(uid));
    reqs << select(Constants::Table_CALENDAR, joins, conds);

    // get delegated calendars
    joins.clear();
    joins << Utils::Join(Constants::Table_PEOPLE, Constants::PEOPLE_CAL_ID, Constants::Table_CALENDAR, Constants::CAL_ID);
    joins << Utils::Join(Constants::Table_USERCALENDARS, Constants::USERCAL_CAL_ID, Constants::Table_CALENDAR, Constants::CAL_ID);
    conds.clear();
    conds << Utils::Field(Constants::Table_CALENDAR, Constants::CAL_ISVALID, "=1");
    conds << Utils::Field(Constants::Table_PEOPLE, Constants::PEOPLE_UID, QString("='%1'").arg(uid));
    conds << Utils::Field(Constants::Table_PEOPLE, Constants::PEOPLE_TYPE, QString("=%1").arg(Calendar::People::PeopleUserDelegate));
    Utils::FieldList get;
    get << fields(Constants::Table_CALENDAR);
    get << Utils::Field(Constants::Table_USERCALENDARS, Constants::USERCAL_USER_UUID);
    reqs << select(get, joins, conds);

    DB.transaction();
    QSqlQuery query(DB);
    for(int i = 0; i < reqs.count(); ++i) {
        if (query.exec(reqs.at(i))) {
            while (query.next()) {
                Agenda::UserCalendar *u = new Agenda::UserCalendar;
                // set private data
                //            u->setData(IUserCalendar::DbOnly_UserCalId, query.value());
                u->setData(Constants::Db_CalId, query.value(Constants::CAL_ID));
                u->setData(Agenda::UserCalendar::Uid, query.value(Constants::CAL_UID));
                u->setData(Constants::Db_CatId, query.value(Constants::CAL_CATEGORYID));
                u->setData(Agenda::UserCalendar::SortId, query.value(Constants::CAL_SORTID));
                u->setData(Constants::Db_IsValid, 1);
                //            CAL_SITEUID,
                //            CAL_XMLOPTIONS
                u->setData(Agenda::UserCalendar::Label, query.value(Constants::CAL_LABEL));
                u->setData(Agenda::UserCalendar::Description, query.value(Constants::CAL_FULLCONTENT));
                u->setData(Agenda::UserCalendar::Type, query.value(Constants::CAL_TYPE));
                u->setData(Agenda::UserCalendar::Status, query.value(Constants::CAL_STATUS));
                u->setData(Agenda::UserCalendar::IsDefault, query.value(Constants::CAL_DEFAULT));
                u->setData(Agenda::UserCalendar::IsPrivate, query.value(Constants::CAL_ISPRIVATE));
                u->setData(Agenda::UserCalendar::Password, query.value(Constants::CAL_PASSWORD));
                u->setData(Agenda::UserCalendar::DefaultDuration, query.value(Constants::CAL_DEFAULTDURATION));
                u->setData(Agenda::UserCalendar::AbsPathIcon, query.value(Constants::CAL_THEMEDICON));

                if (i==0) {
                    u->setData(Agenda::UserCalendar::UserOwnerUid, uid);
                    u->setData(Agenda::UserCalendar::IsDelegated, false);
                } else {
                    u->setData(Agenda::UserCalendar::IsDelegated, true);
                    QString uid = query.value(CAL_MaxParam).toString();
                    u->setData(Agenda::UserCalendar::UserOwnerUid, uid);
                    QHash<QString, QString> n = userModel()->getUserNames(QStringList() << uid);
                    u->setData(Agenda::UserCalendar::UserOwnerFullName, n.value(uid));
                }
                u->setModified(false);
                toReturn << u;
            }
        } else {
            LOG_QUERY_ERROR(query);
            query.finish();
            DB.rollback();
            return toReturn;
        }
        query.finish();
    }

    // Get availabilities
    get.clear();
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
        Agenda::UserCalendar *u = toReturn.at(i);
        int lastAvId = -1;
        int lastWeekDay = -1;
        QVector<Agenda::TimeRange> tr;
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
                    Agenda::DayAvailability av;
                    av.setId(lastAvId);
                    av.setWeekDay(lastWeekDay);
                    av.setTimeRanges(tr);
                    u->addAvailabilities(av);
                    tr.clear();
                    lastAvId = query.value(0).toInt();
                    lastWeekDay = query.value(1).toInt();
                }
                Agenda::TimeRange t;
                t.id = query.value(2).toInt();
                t.from = QTime::fromString(query.value(3).toString());
                t.to = QTime::fromString(query.value(4).toString());
                tr.append(t);
            }
            // Create last availability
            Agenda::DayAvailability av;
            av.setId(lastAvId);
            av.setWeekDay(lastWeekDay);
            av.setTimeRanges(tr);
            u->addAvailabilities(av);
            tr.clear();
        } else {
            LOG_QUERY_ERROR(query);
            query.finish();
            DB.rollback();
            return toReturn;
        }
    }
    query.finish();
    DB.commit();

    // Get People
    for(int i = 0; i < toReturn.count(); ++i) {
        Agenda::UserCalendar *u = toReturn[i];
        getRelatedPeoples(RelatedToCalendar, u->data(Constants::Db_CalId).toInt(), u);
    }

    return toReturn;
}

/**
 * Save the user's calendar availabilities for the specified \e calendar
 * to database. The \e calendar is modified during this process
 * (ids are set if needed).
 */
bool AgendaBase::saveCalendarAvailabilities(Agenda::UserCalendar *calendar)
{
    if (calendar->data(Constants::Db_CalId).isNull() ||
        !calendar->data(Constants::Db_CalId).isValid() ||
        calendar->data(Constants::Db_CalId).toInt()==-1) {
        LOG_ERROR("No calendar id, can not save availabilities");
        return false;
    }

    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_NAME);
    if (!connectDatabase(DB, __LINE__)) {
        return false;
    }

    // delete all recorded availabilities of this calendar
    // get all availIds for the calId
    DB.transaction();
    QSqlQuery query(DB);
    QHash<int, QString> where;
    QStringList availIds;
    // select avail_id from avail where cal_id=6
    where.insert(Constants::AVAIL_CAL_ID, QString("=%1").arg(calendar->data(Constants::Db_CalId).toInt()));
    if (query.exec(this->select(Constants::Table_AVAILABILITIES, Constants::AVAIL_ID, where))) {
        while (query.next()) {
            availIds << query.value(0).toString();
        }
    } else {
        LOG_QUERY_ERROR(query);
        query.finish();
        DB.rollback();
        return false;
    }
    query.finish();

    if (!availIds.isEmpty()) {
        // get all tr_id for deleted availIds
        QStringList trIds;
        where.clear();
        where.insert(Constants::AVTOTR_AVID, QString("IN (%1)").arg(availIds.join(",")));
        if (query.exec(this->select(Constants::Table_AVAIL_TO_TIMERANGE, Constants::AVTOTR_TRID, where))) {
            while (query.next()) {
                trIds << query.value(0).toString();
            }
        } else {
            LOG_QUERY_ERROR(query);
            DB.rollback();
            query.finish();
            return false;
        }
        query.finish();

        // delete availabilities records
        where.clear();
        where.insert(Constants::AVAIL_ID, QString("IN (%1)").arg(availIds.join(",")));
        if (!query.exec(this->prepareDeleteQuery(Constants::Table_AVAILABILITIES, where))) {
            LOG_QUERY_ERROR(query);
            query.finish();
            database().rollback();
            return false;
        }
        query.finish();
        where.clear();
        where.insert(Constants::AVTOTR_AVID, QString("IN (%1)").arg(availIds.join(",")));
        if (!query.exec(this->prepareDeleteQuery(Constants::Table_AVAIL_TO_TIMERANGE, where))) {
            LOG_QUERY_ERROR(query);
            query.finish();
            database().rollback();
            return false;
        }
        query.finish();
        where.clear();
        where.insert(Constants::TIMERANGE_ID, QString("IN (%1)").arg(trIds.join(",")));
        if (!query.exec(this->prepareDeleteQuery(Constants::Table_TIMERANGE, where))) {
            LOG_QUERY_ERROR(query);
            query.finish();
            DB.rollback();
            return false;
        }
        query.finish();
    }

    // No availabilities -> finished
    if (!calendar->hasAvailability()) {
        DB.commit();
        return true;
    }

    // recreate availabilities
    QVector<Agenda::DayAvailability> av = calendar->availabilities();
    QHash<int, Agenda::DayAvailability> hashAv;
    // fusion availabilities by days
    for(int availabilityIndex = 0; availabilityIndex < av.count(); ++availabilityIndex) {
        int weekDay = av.at(availabilityIndex).weekDay();
        if (hashAv.contains(weekDay)) {
            for(int timeRangeIndex = 0; timeRangeIndex < av.at(availabilityIndex).timeRangeCount(); ++timeRangeIndex) {
                hashAv[weekDay].addTimeRange(av.at(availabilityIndex).timeRangeAt(timeRangeIndex));
            }
        } else {
            hashAv.insert(av.at(availabilityIndex).weekDay(), av.at(availabilityIndex));
        }
    }

    QHashIterator<int, Agenda::DayAvailability> it(hashAv);
    while (it.hasNext()) {
        it.next();
        query.prepare(prepareInsertQuery(Constants::Table_AVAILABILITIES));
        query.bindValue(Constants::AVAIL_ID, QVariant());
        query.bindValue(Constants::AVAIL_CAL_ID, calendar->data(Constants::Db_CalId));
        query.bindValue(Constants::AVAIL_WEEKDAY, it.key());
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            query.finish();
            DB.rollback();
            return false;
        }
        int avId = query.lastInsertId().toInt();
        query.finish();

        // save all timeRanges
        QVector<Agenda::TimeRange> newRanges;
        for(int z = 0; z < it.value().timeRangeCount(); ++z) {
            Agenda::TimeRange range = it.value().timeRangeAt(z);
            query.prepare(prepareInsertQuery(Constants::Table_TIMERANGE));
            query.bindValue(Constants::TIMERANGE_ID, QVariant());
            query.bindValue(Constants::TIMERANGE_FROM, range.from.toString());
            query.bindValue(Constants::TIMERANGE_TO, range.to.toString());
            if (!query.exec()) {
                LOG_QUERY_ERROR(query);
                query.finish();
                database().rollback();
                return false;
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
                query.finish();
                DB.rollback();
                return false;
            }
            query.finish();
        }
        hashAv[it.key()].setTimeRanges(newRanges);
        hashAv[it.key()].setId(avId);
    }
    calendar->setAvailabilities(hashAv.values());
    DB.commit();
    LOG("User agenda correctly saved");
    return true;
}

/**
 * Save the user's calendar Agenda::IUserCalendar \e calendar
 * to database. The \e calendar is modified during this process
 * (ids are set if needed).
 */
bool AgendaBase::saveUserCalendar(Agenda::UserCalendar *calendar)
{
    if (!calendar->isModified())
        return true;
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_NAME);
    if (!connectDatabase(DB, __LINE__)) {
        return false;
    }
    DB.transaction();
    if (calendar->data(Constants::Db_CalId).isNull() ||
        !calendar->data(Constants::Db_CalId).isValid() ||
        calendar->data(Constants::Db_CalId).toInt()==-1) {
        // save
        QSqlQuery query(DB);
        query.prepare(prepareInsertQuery(Constants::Table_CALENDAR));
        query.bindValue(Constants::CAL_ID, QVariant());
        query.bindValue(Constants::CAL_UID, calendar->data(Agenda::UserCalendar::Uid).toString());
        query.bindValue(Constants::CAL_CATEGORYID, calendar->data(Constants::Db_CatId).toInt());
        query.bindValue(Constants::CAL_SORTID, calendar->data(Agenda::UserCalendar::SortId).toInt());
        query.bindValue(Constants::CAL_ISVALID, calendar->data(Constants::Db_IsValid).toInt());
        query.bindValue(Constants::CAL_ISVIRTUAL, calendar->data(Constants::Db_IsVirtual).toInt());
        query.bindValue(Constants::CAL_STATUS, calendar->data(Agenda::UserCalendar::Type).toInt());
        query.bindValue(Constants::CAL_TYPE, calendar->data(Agenda::UserCalendar::Status).toInt());
        query.bindValue(Constants::CAL_DEFAULT, calendar->data(Agenda::UserCalendar::IsDefault).toInt());
        query.bindValue(Constants::CAL_ISPRIVATE, calendar->data(Agenda::UserCalendar::IsPrivate).toInt());
        query.bindValue(Constants::CAL_PASSWORD, calendar->data(Agenda::UserCalendar::Password));
        query.bindValue(Constants::CAL_DEFAULTDURATION, calendar->data(Agenda::UserCalendar::DefaultDuration));
        query.bindValue(Constants::CAL_FULLCONTENT, calendar->data(Agenda::UserCalendar::Description));
        query.bindValue(Constants::CAL_SITEUID, calendar->data(Agenda::UserCalendar::LocationUid).toInt());
        query.bindValue(Constants::CAL_LABEL, calendar->data(Agenda::UserCalendar::Label));
        query.bindValue(Constants::CAL_THEMEDICON, calendar->data(Agenda::UserCalendar::AbsPathIcon));
        query.bindValue(Constants::CAL_XMLOPTIONS, calendar->xmlOptions());
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            query.finish();
            DB.rollback();
            return false;
        }
        calendar->setData(Constants::Db_CalId, query.lastInsertId());
        calendar->setModified(false);
        query.finish();

        if (calendar->data(Agenda::UserCalendar::UserOwnerUid).isNull() ||
            !calendar->data(Agenda::UserCalendar::UserOwnerUid).isValid()) {
            calendar->setData(Agenda::UserCalendar::UserOwnerUid, user()->uuid());
        }
        query.prepare(prepareInsertQuery(Constants::Table_USERCALENDARS));
        query.bindValue(Constants::USERCAL_ID, QVariant());
        query.bindValue(Constants::USERCAL_CAL_ID, calendar->data(Constants::Db_CalId).toInt());
        query.bindValue(Constants::USERCAL_USER_UUID, calendar->data(Agenda::UserCalendar::UserOwnerUid));
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            query.finish();
            DB.rollback();
            return false;
        }
        calendar->setData(Constants::Db_UserCalId, query.lastInsertId());
        calendar->setModified(false);
        query.finish();

        if (!saveCalendarAvailabilities(calendar)) {
            DB.rollback();
            return false;
        }

        if (!saveRelatedPeoples(RelatedToCalendar, calendar->data(Constants::Db_CalId).toInt(), calendar)) {
            DB.rollback();
            return false;
        }
    } else {
        // update
        QSqlQuery query(DB);
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
                                         << Constants::CAL_SORTID
                                         << Constants::CAL_ISVIRTUAL
                                         , where));
        query.bindValue(0, calendar->data(Constants::Db_CalId));
        query.bindValue(1, calendar->data(Constants::Db_IsValid).toInt());
        query.bindValue(2, calendar->data(Agenda::UserCalendar::IsPrivate).toInt());
        query.bindValue(3, calendar->data(Agenda::UserCalendar::Type).toInt());
        query.bindValue(4, calendar->data(Agenda::UserCalendar::Status).toInt());
        query.bindValue(5, calendar->data(Agenda::UserCalendar::IsDefault).toInt());
        query.bindValue(6, calendar->data(Agenda::UserCalendar::Password));
        query.bindValue(7, calendar->data(Agenda::UserCalendar::LocationUid));
        query.bindValue(8, calendar->data(Agenda::UserCalendar::Label));
        query.bindValue(9, calendar->data(Agenda::UserCalendar::AbsPathIcon));
        query.bindValue(10, calendar->xmlOptions());
        query.bindValue(11, calendar->data(Agenda::UserCalendar::DefaultDuration));
        query.bindValue(12, calendar->data(Agenda::UserCalendar::Description));
        query.bindValue(13, calendar->data(Agenda::UserCalendar::SortId));
        query.bindValue(14, calendar->data(Constants::Db_IsVirtual).toInt());
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            query.finish();
            DB.rollback();
            return false;
        }
        calendar->setModified(false);
        query.finish();

        if (!saveCalendarAvailabilities(calendar)) {
            DB.rollback();
            return false;
        }

        if (!saveRelatedPeoples(RelatedToCalendar, calendar->data(Constants::Db_CalId).toInt(), calendar)) {
            DB.rollback();
            return false;
        }
    }
    DB.commit();
    return true;
}

/**
 * Retrieve all events from database according to the
 * Agenda::CalendarEventQuery \e calQuery.
 */
QList<Appointment *> AgendaBase::getCalendarEvents(const CalendarEventQuery &calQuery)
{
    QList<Appointment *> toReturn;
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_NAME);
    if (!connectDatabase(DB, __LINE__)) {
        return toReturn;
    }
    QString req;
    // get events according to the query
    Utils::JoinList joins;
//    joins << Utils::Join(Constants::Table_COMMON, Constants::COMMON_ID, Constants::Table_EVENTS, Constants::EVENT_COMMON_ID);
    Utils::FieldList conds;
    conds << Utils::Field(Constants::Table_CALENDAR, Constants::CAL_ISVALID, "=1");
    conds << Utils::Field(Constants::Table_EVENTS, Constants::EVENT_ISVALID, "=1");
    QString order;
    // get a specific appointment ?
    if (!calQuery.appointmentId().isNull()) {
        if (calQuery.appointmentId().toInt() == -1)
            return toReturn;
        conds << Utils::Field(Constants::Table_EVENTS, Constants::EVENT_ID, QString("='%1'").arg(calQuery.appointmentId().toString()));
    } else {
        // TODO: add virtual appointment filtering
        // Date conditions
        if (calQuery.hasDateRange()) {
            // TODO: code here: better management of dates in filters
            conds << Utils::Field(Constants::Table_EVENTS, Constants::EVENT_DATESTART, QString(">='%1'").arg(calQuery.dateStart().toString(Qt::ISODate)));
            conds << Utils::Field(Constants::Table_EVENTS, Constants::EVENT_DATEEND, QString("<='%1'").arg(calQuery.dateEnd().toString(Qt::ISODate)));
        }
        // Calendars && User conditions
        if (calQuery.calendarIds().count() > 0) {
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
        order = QString("\n ORDER BY `%1`.`%2` ASC").arg(table(Constants::Table_EVENTS)).arg(fieldName(Constants::Table_EVENTS, Constants::EVENT_ID));
    }

    // Get Event table
    req = select(Constants::Table_EVENTS, joins, conds) + order;

//    qWarning() << req;

    DB.transaction();
    QSqlQuery query(DB);
    if (query.exec(req)) {
        while (query.next()) {
            Appointment *ev = new Appointment;
            ev->setData(Constants::Db_EvId, query.value(Constants::EVENT_ID));
            ev->setData(Constants::Db_CalId, query.value(Constants::EVENT_CAL_ID));
            ev->setData(Constants::Db_ComId, query.value(Constants::EVENT_COMMON_ID));
            ev->setData(Constants::Db_IsValid, query.value(Constants::EVENT_ISVALID));
            ev->setData(CalendarItemModel::Uid, query.value(Constants::EVENT_ID));
            ev->setData(CalendarItemModel::DateStart, query.value(Constants::EVENT_DATESTART));
            ev->setData(CalendarItemModel::DateEnd, query.value(Constants::EVENT_DATEEND));
            toReturn << ev;
        }
    } else {
        LOG_QUERY_ERROR(query);
        query.finish();
        DB.rollback();
        return toReturn;
    }
    query.finish();
    if (toReturn.isEmpty()) {
        DB.commit();
        return toReturn;
    }

    // Get common data
    QHash<int, QString> where;
    for(int i = 0; i < toReturn.count(); ++i) {
        Appointment *ev = toReturn.at(i);
        where.insert(Constants::COMMON_ID, "=" + ev->data(Constants::Db_EvId).toString());
        req = select(Constants::Table_COMMON, where);
        if (query.exec(req)) {
            if (query.next()) {
                ev->setData(Constants::Db_CatId, query.value(Constants::COMMON_CAT_ID));
                ev->setData(CalendarItemModel::Type, query.value(Constants::COMMON_TYPE_ID));
                ev->setData(Constants::Db_IsVirtual, query.value(Constants::COMMON_ISVIRTUAL));
                ev->setData(CalendarItemModel::Status, query.value(Constants::COMMON_STATUS_ID));
                ev->setData(CalendarItemModel::LocationUid, query.value(Constants::COMMON_SITE_UID));
                ev->setData(CalendarItemModel::IsPrivate, query.value(Constants::COMMON_ISPRIVATE));
                ev->setData(CalendarItemModel::Password, query.value(Constants::COMMON_PASSWORD));
                ev->setData(CalendarItemModel::IsBusy, query.value(Constants::COMMON_ISBUSY));
                ev->setData(CalendarItemModel::IsAGroupEvent, query.value(Constants::COMMON_ISAGROUPEVENT));
                ev->setData(CalendarItemModel::Label, query.value(Constants::COMMON_LABEL));
                ev->setData(CalendarItemModel::Description, query.value(Constants::COMMON_FULLCONTENT));
                ev->setData(CalendarItemModel::Location, query.value(Constants::COMMON_TEXTUAL_SITE));
                ev->setData(Constants::Db_XmlViewOptions, query.value(Constants::COMMON_XMLVIEWOPTIONS));
                ev->setData(Constants::Db_XmlOptions, query.value(Constants::COMMON_XMLCALOPTIONS));
                ev->setModified(false);
            }
        } else {
            LOG_QUERY_ERROR(query);
            query.finish();
            DB.rollback();
            return toReturn;
        }
        query.finish();

        // Get peoples
        getRelatedPeoples(RelatedToAppointment, ev->eventId(), ev);
    }

//    getPatientNames(toReturn);
    DB.commit();
    return toReturn;
}

/**
 * Save or update the common part of events and set the commonId()
 * in the event
 */
bool AgendaBase::saveCommonEvent(Appointment *event)
{
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_NAME);
    if (!connectDatabase(DB, __LINE__)) {
        return false;
    }
    DB.transaction();
    QSqlQuery query(DB);
    if (event->eventId() == -1) {
        // save
        query.prepare(prepareInsertQuery(Constants::Table_COMMON));
        query.bindValue(Constants::COMMON_ID, QVariant());
        query.bindValue(Constants::COMMON_CAT_ID, event->data(Constants::Db_CalId));
        query.bindValue(Constants::COMMON_TYPE_ID, event->data(CalendarItemModel::Type));
        query.bindValue(Constants::COMMON_ISVIRTUAL, event->data(Constants::Db_IsVirtual).toInt());
        query.bindValue(Constants::COMMON_STATUS_ID, event->data(CalendarItemModel::Status).toInt());
        query.bindValue(Constants::COMMON_SITE_UID, event->data(CalendarItemModel::LocationUid));
        query.bindValue(Constants::COMMON_ISPRIVATE, event->data(CalendarItemModel::IsPrivate).toInt());
        query.bindValue(Constants::COMMON_PASSWORD, event->data(CalendarItemModel::Password));
        query.bindValue(Constants::COMMON_ISBUSY, event->data(CalendarItemModel::IsBusy).toInt());
        query.bindValue(Constants::COMMON_ISAGROUPEVENT, event->data(CalendarItemModel::IsAGroupEvent).toInt());
        query.bindValue(Constants::COMMON_LABEL, event->data(CalendarItemModel::Label));
        query.bindValue(Constants::COMMON_FULLCONTENT, event->data(CalendarItemModel::Description));
        query.bindValue(Constants::COMMON_TEXTUAL_SITE, event->data(CalendarItemModel::Location));
        query.bindValue(Constants::COMMON_THEMEDICON, event->data(CalendarItemModel::IconPath));
        query.bindValue(Constants::COMMON_XMLVIEWOPTIONS, event->data(Constants::Db_XmlViewOptions));
        query.bindValue(Constants::COMMON_XMLCALOPTIONS, event->data(Constants::Db_XmlOptions));
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            query.finish();
            DB.rollback();
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
                                         << Constants::COMMON_ISVIRTUAL
                                         , where));
        query.bindValue(0, event->calendarId());
        query.bindValue(1, event->data(CalendarItemModel::Type));
        query.bindValue(2, event->data(CalendarItemModel::Status));
        query.bindValue(3, event->data(CalendarItemModel::LocationUid));
        query.bindValue(4, event->data(CalendarItemModel::IsPrivate).toInt());
        query.bindValue(5, event->data(CalendarItemModel::Password));
        query.bindValue(6, event->data(CalendarItemModel::IsBusy).toInt());
        query.bindValue(7, event->data(CalendarItemModel::IsAGroupEvent));
        query.bindValue(8, event->data(CalendarItemModel::Label));
        query.bindValue(9, event->data(CalendarItemModel::Description));
        query.bindValue(10, event->data(CalendarItemModel::Location));
        query.bindValue(11, event->data(CalendarItemModel::IconPath));
        query.bindValue(12, event->data(Constants::Db_XmlViewOptions));
        query.bindValue(13, event->data(Constants::Db_XmlOptions));
        query.bindValue(14, event->data(Constants::Db_IsVirtual).toInt());
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            query.finish();
            DB.rollback();
            return false;
        }
    }
    query.finish();
    DB.commit();
    return true;
}

/**
 * Save the peoples related to an \e Calendar::CalendarPeople
 * \e people inherited class.
 */
bool AgendaBase::saveRelatedPeoples(RelatedEventFor relatedTo, const int eventOrCalendarId, const Calendar::CalendarPeople *peopleClass)
{
    if (eventOrCalendarId == -1) {
        LOG_ERROR("No Event/Calendar ID");
        return false;
    }
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_NAME);
    if (!connectDatabase(DB, __LINE__)) {
        return false;
    }

    // Delete old entries
    DB.transaction();
    QSqlQuery query(DB);
    QHash<int, QString> where;
    if (relatedTo == RelatedToCalendar) {
        where.insert(PEOPLE_CAL_ID, QString("=%1").arg(eventOrCalendarId));
    } else {
        where.insert(PEOPLE_EVENT_ID, QString("=%1").arg(eventOrCalendarId));
    }
    if (!query.exec(prepareDeleteQuery(Table_PEOPLE, where))) {
        LOG_QUERY_ERROR(query);
        query.finish();
        DB.rollback();
        return false;
    }
    query.finish();

    // Save new entries (all peoples)
    for(int i = 0; i < Calendar::People::PeopleCount; ++i) {
        // People
        const QStringList &peoples = peopleClass->peopleUids(i);
        for(int j = 0; j < peoples.count(); ++j) {
            query.prepare(prepareInsertQuery(Table_PEOPLE));
            query.bindValue(PEOPLE_ID, QVariant());
            if (relatedTo == RelatedToCalendar) {
                query.bindValue(PEOPLE_CAL_ID, eventOrCalendarId);
                query.bindValue(PEOPLE_EVENT_ID, QVariant());
            } else {
                query.bindValue(PEOPLE_CAL_ID, QVariant());
                query.bindValue(PEOPLE_EVENT_ID, eventOrCalendarId);
            }
            query.bindValue(PEOPLE_UID, peoples.at(j));
            query.bindValue(PEOPLE_TYPE, i);
            if (!query.exec()) {
                LOG_QUERY_ERROR(query);
                query.finish();
                DB.rollback();
                return false;
            }
            query.finish();
        }
    }
    query.finish();
    DB.commit();
    return true;
}

/** Retrieve the peoples related to an \e event. */
bool AgendaBase::getRelatedPeoples(RelatedEventFor relatedTo, const int eventOrCalendarId, Calendar::CalendarPeople *event)
{
    if (eventOrCalendarId == -1) {
        LOG_ERROR("No Event/Calendar ID");
        return false;
    }
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_NAME);
    if (!connectDatabase(DB, __LINE__)) {
        return false;
    }
    DB.transaction();
    QSqlQuery query(DB);
    QHash<int, QString> where;
    if (relatedTo == RelatedToCalendar) {
        where.insert(Constants::PEOPLE_CAL_ID, QString("=%1").arg(eventOrCalendarId));
    } else {
        where.insert(Constants::PEOPLE_EVENT_ID, QString("=%1").arg(eventOrCalendarId));
    }
    QString req = select(Table_PEOPLE, where);
    if (query.exec(req)) {
        while (query.next()) {
            event->addPeople(Calendar::People(Calendar::People::TypeOfPeople(query.value(PEOPLE_TYPE).toInt()),
                             "", query.value(PEOPLE_UID).toString()));
        }
    } else {
        LOG_QUERY_ERROR(query);
        query.finish();
        DB.rollback();
        return false;
    }
    query.finish();

    // get people names (for users only, patient's names are retrieved by the CalendarItemModel)
    QList<int> types;
    types << Calendar::People::PeopleOwner << Calendar::People::PeopleUser << Calendar::People::PeopleUserDelegate;
    for(int i = 0; i < types.count(); ++i) {
        if (event->peopleCount(types.at(i))) {
            QHash<QString,QString> names = userModel()->getUserNames(event->peopleUids(types.at(i), true));
            foreach(const QString &u, names.keys()) {
                event->setPeopleName(types.at(i), u, names.value(u));
            }
        }
    }
    DB.commit();
    return true;
}

/** Retrieve the patients name for the specified list of Appointments \e items */
bool AgendaBase::getPatientNames(const QList<Appointment *> &items)
{
    // get all patient uids
    QStringList uids;
    for(int i = 0; i < items.count(); ++i) {
        uids << items.at(i)->peopleUids(Calendar::People::PeopleAttendee);
    }
    uids.removeAll("");

    // retrieve names
    QHash<QString, QString> names = Patients::PatientModel::patientName(uids);

    // feed calendaritems
    for(int i = 0; i < items.count(); ++i) {
        QStringList l = items.at(i)->peopleUids(Calendar::People::PeopleAttendee);
        for(int j = 0; j < l.count(); ++j) {
            QString name = names.value(l.at(j));
            items.at(i)->setPeopleName(Calendar::People::PeopleAttendee, l.at(j), name);
        }
    }

    return true;
}

/** Retrieve the patients name for the specified Appointment \e item */
bool AgendaBase::getPatientNames(Appointment *item)
{
    return getPatientNames(QList<Appointment *>() << item);
}

/** Create a virtual UserCalendar and return it. Pointer must be deleted. */
Agenda::UserCalendar *AgendaBase::createVirtualUserCalendar(const QString &ownerUid, const QString &label, const QString &description,
                                           int defaultDurationInMinutes, int sortId,
                                           int type, int status, bool isDefault, bool isPrivate,
                                           const QString &password, const QString &iconPath,
                                           const QList<Calendar::People> &peoples)
{
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    QHash<int, QString> where;
    where.insert(CAL_LABEL, QString("='%1'").arg(label));
    where.insert(CAL_FULLCONTENT, QString("='%1'").arg(description));
    where.insert(CAL_DEFAULTDURATION, QString("=%1").arg(defaultDurationInMinutes));
    int alreadyInBase = count(Table_CALENDAR, CAL_ID, getWhereClause(Table_CALENDAR, where));
    if (alreadyInBase!=0) {
        LOG("Calendar is already in base, virtual user calendar not created");
        return 0;
    }
    UserCalendar *u = new UserCalendar;
    u->setData(UserCalendar::Uid, Utils::Database::createUid());
    u->setData(UserCalendar::UserOwnerUid, ownerUid);
    u->setData(UserCalendar::Label, label);
    u->setData(UserCalendar::Description, description);
    u->setData(UserCalendar::Type, type);
    u->setData(UserCalendar::Status, status);
    u->setData(UserCalendar::IsDefault, isDefault);
    u->setData(UserCalendar::IsPrivate, isPrivate);
    u->setData(UserCalendar::Password, password);
    u->setData(UserCalendar::AbsPathIcon, iconPath);
    u->setData(UserCalendar::DefaultDuration, defaultDurationInMinutes);
    u->setData(UserCalendar::SortId, sortId);
    u->setData(Constants::Db_IsVirtual, true);
    u->setData(Constants::Db_IsValid, true);
    u->setPeopleList(peoples);
    // Create availabilities
    for(int i = 1; i < 6; ++i) {
        Agenda::DayAvailability av;
        av.setWeekDay(i);
        av.addTimeRange(QTime(7,0,0), QTime(20,0,0));
        u->addAvailabilities(av);
    }
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    if (!saveUserCalendar(u)) {
        delete u;
        u = 0;
    }
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    return u;
}

/** Save or update a Calendar::CalendarItem to the agenda database */
bool AgendaBase::saveNonCyclingEvent(Appointment *event)
{
//    WARN_FUNC << event->eventId();
    if (!event->isModified())
        return true;

    if (event->isCycling())
        return false;

    if (!saveCommonEvent(event))
        return false;

    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_NAME);
    if (!connectDatabase(DB, __LINE__)) {
        return false;
    }
    DB.transaction();
    QSqlQuery query(DB);
    if (event->eventId() == -1) {
        // save
        query.prepare(prepareInsertQuery(Constants::Table_EVENTS));
        query.bindValue(Constants::EVENT_ID, QVariant());
        query.bindValue(Constants::EVENT_CAL_ID, event->data(Constants::Db_CalId));
        query.bindValue(Constants::EVENT_COMMON_ID, event->data(Constants::Db_ComId));
        query.bindValue(Constants::EVENT_ISVALID, event->data(Constants::Db_IsValid).toInt());
        query.bindValue(Constants::EVENT_DATESTART, event->beginning());
        query.bindValue(Constants::EVENT_DATEEND, event->ending());
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            query.finish();
            DB.rollback();
            return false;
        }
        event->setData(Constants::Db_EvId, query.lastInsertId());
        query.finish();

        event->setModified(false);

        if (!saveRelatedPeoples(RelatedToAppointment, event->eventId(), event)) {
            DB.rollback();
            return false;
        }

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
        query.bindValue(2, event->beginning());
        query.bindValue(3, event->ending());
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            query.finish();
            DB.rollback();
            return false;
        }
        query.finish();

        // update peoples
        where.clear();
        where.insert(Constants::PEOPLE_EVENT_ID, "=" + event->data(Constants::Db_EvId).toString());
        query.exec(prepareDeleteQuery(Table_PEOPLE, where));
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            query.finish();
            DB.rollback();
            return false;
        }
        query.finish();

        if (!saveRelatedPeoples(RelatedToAppointment, event->eventId(), event)) {
            DB.rollback();
            return false;
        }
    }
    DB.commit();
    return true;
}

/** Save or update events in the database, when saved events are set to non modified. Return false in case of an error. */
bool AgendaBase::saveCalendarEvents(const QList<Appointment *> &events)
{
    bool ok = true;
    for(int i = 0; i < events.count(); ++i) {
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        Appointment *ev = events.at(i);
        if (!saveNonCyclingEvent(ev))
            ok = false;
    }
    return ok;
}

/** Save or update one event in the database, when saved events are set to non modified. Return false in case of an error. */
bool AgendaBase::saveCalendarEvent(Appointment *event)
{
    return saveCalendarEvents(QList<Appointment *>() << event);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/** Return the next available appointments by searching for
  - \e startSearch date time,
  - a duration of \e duration minutes
  - for the agenda \e calendar
  - limiting search to \e numberOfDates appointments
*/
QList<QDateTime> AgendaBase::nextAvailableTime(const QDateTime &startSearch, const int durationInMinutes, const Agenda::UserCalendar &calendar, const int numberOfDates)
{
    QList<QDateTime> toReturn;

    // Some checkings
    if (!calendar.isValid())
        return toReturn;
    if (durationInMinutes <= 0)
        return toReturn;
    if (numberOfDates <= 0)
        return toReturn;
    if (calendar.data(Constants::Db_CalId).toString().isEmpty())
        return toReturn;
    if (calendar.data(Constants::Db_CalId).toString()=="-1")
        return toReturn;
    // prevent division by 0
    if (calendar.data(UserCalendar::DefaultDuration).toInt() == 0)
        return toReturn;

    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_NAME);
    if (!connectDatabase(DB, __LINE__)) {
        return toReturn;
    }

    // Here we can go on
    QDateTime start, currentStart, currentEnd;
    start = Utils::roundDateTime(startSearch, calendar.data(UserCalendar::DefaultDuration).toInt());

    Utils::FieldList get;
    get << Utils::Field(Table_EVENTS, EVENT_DATESTART);
    get << Utils::Field(Table_EVENTS, EVENT_DATEEND);
    Utils::JoinList joins;
    // no joins
    // TODO: code here : condition with the type/status of appointments
    Utils::FieldList conds;
    conds << Utils::Field(Table_EVENTS, EVENT_ISVALID, "=1");
    conds << Utils::Field(Table_EVENTS, EVENT_CAL_ID, QString("=%1").arg(calendar.data(Constants::Db_CalId).toString()));
    conds << Utils::Field(Table_EVENTS, EVENT_DATESTART, QString("> '%1'").arg(start.toString(Qt::ISODate)));
    QString req = select(get, joins, conds);
    QString order = QString("\n ORDER BY `%1`.`%2`")
            .arg(table(Table_EVENTS))
            .arg(fieldName(Table_EVENTS, EVENT_DATESTART));
    QString limit = QString("\n LIMIT 0,1");

    // get the first event and make tests
    DB.transaction();
    QSqlQuery query(DB);
    QRect nextAppointment;
    if (query.exec(req+order+limit)) {
        if (query.next()) {
            currentStart = query.value(0).toDateTime();
            currentEnd = query.value(1).toDateTime();
            nextAppointment = NextAvailabiliyManager::dateToRect(currentStart, currentEnd);
        }
    } else {
        LOG_QUERY_ERROR(query);
        query.finish();
        DB.rollback();
        return toReturn;
    }

    // Transform availabilities to QRect
    QList<QRect> avs;
    for(int i=0; i < calendar.availabilities().count(); ++i) {
        DayAvailability av = calendar.availabilities().at(i);
        for(int j = 0; j < av.timeRangeCount(); ++j) {
            const TimeRange &tr = av.timeRangeAt(j);
            avs.append(NextAvailabiliyManager::simplifiedDateToRect(av.weekDay(), tr.from, tr.to));
        }
    }
    m_Next->setAvaibilitiesToRect(avs);

    // Computation
    int limitComputation = 10000;//224640; // 10min, 6day a week, 12hours a day :: 1 full year == 12*6 *10 *6 *52 == 224640
    int defaultDuration = calendar.data(UserCalendar::DefaultDuration).toInt();
    while (toReturn.count() < numberOfDates) {
        --limitComputation;
        if (limitComputation <= 0)
            break;
        QList<QDateTime> dates = m_Next->nextAvailableTime(start, durationInMinutes, defaultDuration, nextAppointment, numberOfDates-toReturn.count());
        toReturn.append(dates);

        if (m_Next->hasReachedNextAppointment()) {
            start = m_Next->requestingNewAppointmentLaterThan();
            // Get next appointment
            nextAppointment = QRect();
            conds.removeLast();
            conds << Utils::Field(Table_EVENTS, EVENT_DATESTART, QString(">= '%1'").arg(start.toString(Qt::ISODate)));
            if (query.exec(select(get, joins, conds)+order+limit)) {
                if (query.next()) {
                    currentStart = query.value(0).toDateTime();
                    currentEnd = query.value(1).toDateTime();
                    nextAppointment = NextAvailabiliyManager::dateToRect(currentStart, currentEnd);
                }
            } else {
                LOG_QUERY_ERROR(query);
                query.finish();
                DB.rollback();
                return toReturn;
            }
        }
    }



//    QRect nextAppointemnt;

//    int nbFound = 0;
//    int durationInSeconds = durationInMinutes * 60;
//    TODO: manage algorythm error here
//    int limitComputation = 10000;//224640; // 10min, 6day a week, 12hours a day :: 1 full year == 12*6 *10 *6 *52 == 224640
//    QList<QRect> avList = availabilitiesToRect(calendar.availabilities());

//    while (nbFound < numberOfDates) {
//        --limitComputation;
//        if (limitComputation <= 0)
//            break;
//        if (start.isNull())
//            break;
////        if (WarnNextAvailableTimeWarnings)
////            qWarning() << "start" << start
////                       << "\n  currentStart" << currentStart
////                       << "\n  currentEnd" << currentEnd
////                       << "\n     duration" << durationInMinutes
////                       << "\n     limitation" << limitComputation << "\n";

//        QRect testDate = dateToRect(start, durationInMinutes);

//        // rect does not intersect the next recorded appointment ? -> go next appointment
//        while (testDate.intersect(nextAppointment).height() < 1) {

//            if (WarnNextAvailableTimeWarnings)
//                qWarning() << "\ntest" << testDate << "next" << nextAppointment << testDate.intersect(nextAppointment).height();

//            if (nbFound == numberOfDates) {
//                DB.commit();
//                return toReturn;
//            }
//            if (nextAppointment.top() <= testDate.top())
//                break;

//            // rect in availabilities ?
//            bool inAvail = isInAvailabilities(avList, testDate);

//            if (WarnNextAvailableTimeWarnings)
//                qWarning() << "inAvail" << inAvail;

//            if (inAvail) {
//                // Add date
//                if (WarnNextAvailableTimeWarnings)
//                    qWarning() << "Adding" << start;
//                toReturn << start;
//                ++nbFound;
//                // Add duration to start and/or go to beginning of next availability and cycle
//                start = start.addSecs(durationInSeconds);
//                testDate = dateToRect(start, durationInMinutes);
//            } else {
//                // go to the next avail beginning
//                int minutesToNextAvail = minutesToNextAvailability(avList, testDate);
//                start = start.addSecs(minutesToNextAvail*60 + testDate.height()*60);
//                currentEnd = start;
//                if (WarnNextAvailableTimeWarnings)
//                    qWarning() << "minToAv" << minutesToNextAvail << start << dateToRect(start, durationInMinutes);
//                break;
//            }
//        }

//        // go at the end of nextAppointment and retry

//        // go next week ?

////        // add before the currentDate (enough time or no appointment) ?
////        if (start.secsTo(currentStart) >= durationInSeconds || currentStart.isNull()) {
////            // does it feet the userCalendar availability ?
////            if (calendar.canBeAvailable(start, durationInMinutes)) {
////                toReturn << start;
////                if (WarnNextAvailableTimeWarnings)
////                    qWarning() << "      added" << start;
////                start = start.addSecs(durationInSeconds);
////                ++nbFound;
////                continue;
////            }
////        }

//        // get next appointment
//        //    remove the date from the SQL conditions
//        start = currentEnd;
//        conds.removeLast();
//        conds << Utils::Field(Table_EVENTS, EVENT_DATESTART, QString(">= '%1'").arg(start.toString(Qt::ISODate)));
//        if (query.exec(select(get, joins, conds)+order+limit)) {
//            if (query.next()) {
//                currentStart = query.value(0).toDateTime();
//                currentEnd = query.value(1).toDateTime();
//                nextAppointment = dateToRect(currentStart, currentEnd);
//            } else {
//                // no next appointment -> continue with usercalendar only
//                if (WarnNextAvailableTimeWarnings)
//                    qWarning() <<  "no next";
//                start = Utils::roundDateTime(start, calendar.data(UserCalendar::DefaultDuration).toInt());
//                currentStart = QDateTime();
//                continue;
//            }
//        } else {
//            LOG_QUERY_ERROR(query);
//            DB.rollback();
//            return toReturn;
//        }
//        start = Utils::roundDateTime(start, calendar.data(UserCalendar::DefaultDuration).toInt());
//    }

//    if (WarnNextAvailableTimeWarnings)
//        qWarning() << "No available time found";

    DB.commit();

    return toReturn;
}

/** Return the next available appointment by searching for
  - \e startSearch date time,
  - a duration of \e duration minutes
  - for the agenda \e calendar
*/
QDateTime AgendaBase::nextAvailableTime(const QDateTime &startSearch, const int durationInMinutes, const Agenda::UserCalendar &calendar)
{
    QList<QDateTime> l = nextAvailableTime(startSearch, durationInMinutes, calendar, 1);
    if (l.count())
        return l.at(0);
    return QDateTime();
}
