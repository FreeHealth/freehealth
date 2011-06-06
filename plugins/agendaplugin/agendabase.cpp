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
#include "agendabase.h"
#include "constants.h"
#include "icalendarevent.h"

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

CalendarEventQuery::CalendarEventQuery()
{
    m_DateStart = QDateTime::currentDateTime();
    m_DateEnd = QDateTime::currentDateTime();
    m_Limit = 10;
    m_StartItem = 0;
    m_UseCurrentUser = true;
}

CalendarEventQuery::CalendarEventQuery(const QDateTime &start, const QDateTime end)
{
    m_DateStart = start;
    m_DateEnd = end;
    m_Limit = 10;
    m_StartItem = 0;
    m_UseCurrentUser = true;
}

CalendarEventQuery::CalendarEventQuery(const QDateTime &start, const QDateTime end, const int limit)
{
    m_DateStart = start;
    m_DateEnd = end;
    m_Limit = limit;
    m_StartItem = 0;
    m_UseCurrentUser = true;
}

CalendarEventQuery::CalendarEventQuery(const QDateTime &start, const QDateTime end, const int limit, const int startItem)
{
    m_DateStart = start;
    m_DateEnd = end;
    m_Limit = limit;
    m_StartItem = startItem;
    m_UseCurrentUser = true;
}

CalendarEventQuery::~CalendarEventQuery()
{}

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
    addTable(Table_EVENTTYPES, "TYPES");
    addTable(Table_EVENTSTATUS, "STATUS");
    addTable(Table_USER_WORKINGTIME, "USERWORK");

    addField(Table_USERCALENDARS, USERCAL_ID, "USERCAL_ID", FieldIsUniquePrimaryKey);
    addField(Table_USERCALENDARS, USERCAL_USER_UUID, "USER_UUID", FieldIsUUID);
    addField(Table_USERCALENDARS, USERCAL_CAL_ID, "CAL_ID", FieldIsInteger);

    addField(Table_CALENDAR, CAL_ID, "CAL_ID", FieldIsUniquePrimaryKey);
    addField(Table_CALENDAR, CAL_ISVALID, "ISVALID", FieldIsBoolean);
    addField(Table_CALENDAR, CAL_SITEUID, "SITE_UUID", FieldIsUUID);
    addField(Table_CALENDAR, CAL_LABEL, "LABEL", FieldIsShortText);
    addField(Table_CALENDAR, CAL_XMLOPTIONS, "XMLOPTIONS", FieldIsBlob);

    addField(Table_COMMON, COMMON_ID, "COM_ID", FieldIsUniquePrimaryKey);
    addField(Table_COMMON, COMMON_CAT_ID, "CAL_ID", FieldIsInteger);
    addField(Table_COMMON, COMMON_TYPE_ID, "TYPE_ID", FieldIsInteger);
    addField(Table_COMMON, COMMON_STATUS_ID, "STATUS_ID", FieldIsInteger); // (like draft, validated, rejected, need more info, need to send letter…)
    addField(Table_COMMON, COMMON_SITE_UID, "SITE_UID", FieldIsUUID); // (for a later use)
    addField(Table_COMMON, COMMON_ISPRIVATE, "ISPRIVATE", FieldIsBoolean);
    addField(Table_COMMON, COMMON_PASSWORD, "PASSWORD", FieldIsShortText); // (private events can be protected by a password)
    addField(Table_COMMON, COMMON_ISBUSY, "ISBUSY", FieldIsBoolean);
    addField(Table_COMMON, COMMON_ISAGROUPEVENT, "ISGROUP", FieldIsBoolean); // (event can be defined by and for a group of users)
    addField(Table_COMMON, COMMON_LABEL, "LABEL", FieldIsShortText);
    addField(Table_COMMON, COMMON_FULLCONTENT, "CONTENT", FieldIsLongText);
    addField(Table_COMMON, COMMON_TEXTUAL_SITE, "TEXT_SITE", FieldIsShortText);

    addField(Table_EVENTS, EVENT_ID, "EV_ID", FieldIsUniquePrimaryKey);
    addField(Table_EVENTS, EVENT_CAL_ID, "CAL_ID", FieldIsInteger);
    addField(Table_EVENTS, EVENT_COMMON_ID, "COM_ID", FieldIsInteger);
    addField(Table_EVENTS, EVENT_PATIENT_UID, "PATIENT_UID", FieldIsUUID);
    addField(Table_EVENTS, EVENT_ISNOTRELATEDTOPATIENT, "ISRELTOPATIENT", FieldIsBoolean);
    addField(Table_EVENTS, EVENT_ISVALID, "ISVALID", FieldIsBoolean);
    addField(Table_EVENTS, EVENT_DATESTART, "DTSTART", FieldIsDate);
    addField(Table_EVENTS, EVENT_DATEEND, "DTEND", FieldIsDate);
    addField(Table_EVENTS, EVENT_XMLVIEWOPTIONS, "XML_VIEW", FieldIsBlob);// (color, margins, spacing…)
    addField(Table_EVENTS, EVENT_XMLCALOPTIONS, "XML_OPTIONS", FieldIsBlob); // (can be changed…)

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

//    addField(TYPE_ID = 0,
//    TYPE_LABEL,
//    TYPE_DEFAULT_DURING_TIME,
//    TYPE_COLOR,
//    TYPE_XML_OPTIONS

//    STATUS_ID,
//    STATUS_LABEL

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

QList<Agenda::ICalendarEvent *> AgendaBase::getCalendarEvents(const CalendarEventQuery &calQuery)
{
    QList<ICalendarEvent *> toReturn;
    if (!database().isOpen()) {
        if (!database().open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                      .arg(database().connectionName()).arg(database().lastError().text()));
            return toReturn;
        }
    }
    QString req;
    QSqlQuery query(database());
//    QHash<int, QString> where;
//    where.insert(Constants::CATEGORY_ISVALID, "=1");
//    where.insert(Constants::CATEGORY_MIME, QString("='%1'").arg(mime));

//    // Get Category table
//    req = select(Constants::Table_CATEGORIES, where);
//    if (query.exec(req)) {
//        while (query.next()) {
//            CategoryItem *cat = new CategoryItem;
//            cat->setData(CategoryItem::DbOnly_Id, query.value(Constants::CATEGORY_ID));
//            cat->setData(CategoryItem::DbOnly_LabelId, query.value(Constants::CATEGORY_LABEL_ID));
//            cat->setData(CategoryItem::DbOnly_ParentId, query.value(Constants::CATEGORY_PARENT));
//            cat->setData(CategoryItem::DbOnly_Mime, query.value(Constants::CATEGORY_MIME));
//            cat->setData(CategoryItem::ThemedIcon, query.value(Constants::CATEGORY_THEMEDICON));
//            cat->setData(CategoryItem::Password, query.value(Constants::CATEGORY_PASSWORD));
//            cat->setData(CategoryItem::SortId, query.value(Constants::CATEGORY_SORT_ID));
//            cat->setData(CategoryItem::ExtraXml, query.value(Constants::CATEGORY_EXTRAXML));
//            cats << cat;
//        }
//    } else {
//        LOG_QUERY_ERROR(query);
//    }
    query.finish();
    return toReturn;
}
