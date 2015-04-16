/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developers:   Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
/**
 * \class Guard::Internal::GuardBase
 * Manage all freeguard database access.
 * Get the single instance of the object from Guard::GuardCore.
 */
#include "guardbase.h"
#include "constants_db.h"

#include <utils/log.h>
#include <utils/global.h>
#include <utils/randomizer.h>
#include <utils/databaseconnector.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>
#include <translationutils/trans_database.h>
#include <translationutils/trans_msgerror.h>

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants.h>
#include <coreplugin/icommandline.h>

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

using namespace Guard;
using namespace Internal;
using namespace Trans::ConstantTranslations;

enum { WarnDebugMessage = false };

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ICommandLine *commandLine()  { return Core::ICore::instance()->commandLine(); }
static inline Core::IUser *user()  { return Core::ICore::instance()->user(); }
//static inline Core::IPatient *patient()  { return Core::ICore::instance()->patient(); }

static inline bool connectDatabase(QSqlDatabase DB, const int line)
{
    if (!DB.isOpen()) {
        if (!DB.open()) {
            Utils::Log::addError("GuardBase", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                 .arg(DB.connectionName()).arg(DB.lastError().text()), __FILE__, line);
            return false;
        }
    }
    return true;
}

namespace Guard {
namespace Internal {
class GuardBasePrivate
{
public:
    GuardBasePrivate(GuardBase *base) :
        _logChrono(false),
        _initialized(false),
        _transaction(false),
        q(base)
    {
        r.setPathToFiles(settings()->path(Core::ISettings::BundleResourcesPath) + "/textfiles/");
    }

    ~GuardBasePrivate()
    {
    }

public:
    bool _logChrono, _initialized, _transaction;
    Utils::Randomizer r;

private:
    GuardBase *q;
};
}  // End Internal
}  // End Guard

/**
   \brief Constructor.
   \private
*/
GuardBase::GuardBase(QObject *parent) :
    QObject(parent),
    Utils::Database(),
    d(new GuardBasePrivate(this))
{
    setObjectName("GuardBase");

    // Tables:
    // - Guardian: represents all people that can be include in guard lines.
    // - GardLine: represents the guard scheme
    // - GardianPref: include all guardian preferences for a specific guard line
    // - GuardTable: represents the distribution of guards for a specific guard line
    // - GuardLineInclusion: links lines to guardians

    using namespace Guard::Constants;
    addTable(Table_Guardian,       "GUARDIAN");
    addTable(Table_GuardLine,      "GUARDLINE");
    addTable(Table_GuardianPref,   "GUARDIANPREF");
    addTable(Table_GuardTable,     "GUARDTABLE");
    addTable(Table_GuardLineInclusion, "GUARDLINE_INCL");
    addTable(Table_GuardLineTimeSlot, "GUARDLINE_SLOTS");
    addTable(Table_Location,        "LOCATION");
    addTable(Table_LocationLink,    "LOCATION_LINK");

    addTable(Table_VERSION,         "VERSION");
    addField(Table_VERSION, VERSION_TEXT, "TXT", FieldIsShortText);

    addField(Table_Guardian, GUARDIAN_ID,          "ID",    FieldIsUniquePrimaryKey);
    addField(Table_Guardian, GUARDIAN_UID,         "UUID",  FieldIsUUID);
    addField(Table_Guardian, GUARDIAN_ISVALID,     "ISVALID", FieldIsBoolean, "1");
    addField(Table_Guardian, GUARDIAN_FULLNAME,    "NAME",  FieldIsShortText);
    addField(Table_Guardian, GUARDIAN_EMAIL,       "EMAIL", FieldIsShortText);
    addField(Table_Guardian, GUARDIAN_ACCEPT_MAILS,"MAILS", FieldIsBoolean, "1");
    addField(Table_Guardian, GUARDIAN_MOBILEPHONE, "PHONE", FieldIsShortText);
    addField(Table_Guardian, GUARDIAN_ACCEPT_SMS,  "SMS",   FieldIsBoolean, "1");
    addField(Table_Guardian, GUARDIAN_GENERALPREF_UID,  "PREF_FAKELINE_UID",   FieldIsUUID); // Links to Table_GuardianPref, GUARDIAN_PREFS_GUARDLINE_UID
    addField(Table_Guardian, GUARDIAN_LOCATION_FK, "LOCATION_FK", FieldIsLongInteger);
    addIndex(Table_Guardian, GUARDIAN_ID);
    addIndex(Table_Guardian, GUARDIAN_UID);
    addIndex(Table_Guardian, GUARDIAN_FULLNAME);

    addField(Table_GuardLine, GUARDLINE_ID,          "ID",          FieldIsUniquePrimaryKey);
    addField(Table_GuardLine, GUARDLINE_UID,         "UID",         FieldIsUUID);
    addField(Table_GuardLine, GUARDLINE_ISVALID,     "ISVALID",     FieldIsBoolean);
    addField(Table_GuardLine, GUARDLINE_LABEL,       "LABEL",       FieldIsShortText);
    addField(Table_GuardLine, GUARDLINE_LOCATION,    "LOCATION",    FieldIsShortText);
    addField(Table_GuardLine, GUARDLINE_INFO,        "INFO",        FieldIsLongText);
    addField(Table_GuardLine, GUARDLINE_MAILINGLIST, "MAILINGLIST", FieldIsShortText);
    addField(Table_GuardLine, GUARDLINE_LINESLOT_FK, "TIMESLOT_FK", FieldIsLongInteger);
    addField(Table_GuardLine, GUARDLINE_PRIORITY,    "PRIOR",       FieldIsOneChar);
    addField(Table_GuardLine, GUARDLINE_LOCATION_FK, "LOCATION_FK", FieldIsLongInteger);
    addIndex(Table_GuardLine, GUARDLINE_ID);
    addIndex(Table_GuardLine, GUARDLINE_UID);
    addIndex(Table_GuardLine, GUARDLINE_LINESLOT_FK);

    addField(Table_GuardianPref, GUARDIAN_PREFS_ID,            "ID",    FieldIsUniquePrimaryKey);
    addField(Table_GuardianPref, GUARDIAN_PREFS_GUARDIAN_UID,  "G_UID", FieldIsUUID);
    addField(Table_GuardianPref, GUARDIAN_PREFS_GUARDLINE_UID, "GL_UID", FieldIsUUID);
    addField(Table_GuardianPref, GUARDIAN_PREFS_CONTENT,       "CONTENT", FieldIsLongText);
    addIndex(Table_GuardianPref, GUARDIAN_PREFS_ID);
    addIndex(Table_GuardianPref, GUARDIAN_PREFS_GUARDIAN_UID);
    addIndex(Table_GuardianPref, GUARDIAN_PREFS_GUARDLINE_UID);

    addField(Table_GuardLineInclusion, LINEINCL_ID,          "ID",    FieldIsUniquePrimaryKey);
    addField(Table_GuardLineInclusion, LINEINCL_LINE_UID,    "L_UID", FieldIsUUID);
    addField(Table_GuardLineInclusion, LINEINCL_GARDIAN_UID, "G_UID", FieldIsUUID);
    addField(Table_GuardLineInclusion, LINEINCL_DATESTART,   "DT_START", FieldIsDate);
    addField(Table_GuardLineInclusion, LINEINCL_DATEEND,     "DT_END", FieldIsDate);
    addField(Table_GuardLineInclusion, LINEINCL_MONTH,       "MONTH", FieldIsShortText);
    addField(Table_GuardLineInclusion, LINEINCL_YEARS,       "YEARS", FieldIsShortText);
    addField(Table_GuardLineInclusion, LINEINCL_CYCLING_START, "CYC_START", FieldIsDate);
    addField(Table_GuardLineInclusion, LINEINCL_CYCLING_END, "CYC_END", FieldIsDate);
    addIndex(Table_GuardLineInclusion, LINEINCL_ID);
    addIndex(Table_GuardLineInclusion, LINEINCL_LINE_UID);
    addIndex(Table_GuardLineInclusion, LINEINCL_GARDIAN_UID);

    addField(Table_GuardTable, GUARDTABLE_ID,     "ID",         FieldIsUniquePrimaryKey);
    addField(Table_GuardTable, GUARDTABLE_UID,    "UID",        FieldIsUUID);
    addField(Table_GuardTable, GUARDTABLE_ISVALID,"ISVALID",    FieldIsBoolean, "1");
    addField(Table_GuardTable, GUARDTABLE_STATE,  "STATE",      FieldIsTwoChars);
    addIndex(Table_GuardTable, GUARDTABLE_ID);
    addIndex(Table_GuardTable, GUARDTABLE_UID);
    addIndex(Table_GuardTable, GUARDTABLE_STATE);

    addField(Table_GuardLineTimeSlot, LINESLOT_ID,     "ID",    FieldIsUniquePrimaryKey);
    addField(Table_GuardLineTimeSlot, LINESLOT_ISVALID,"ISVALID", FieldIsBoolean);
    addField(Table_GuardLineTimeSlot, LINESLOT_FK,     "GUARDLINE_FK", FieldIsLongInteger);
    addField(Table_GuardLineTimeSlot, LINESLOT_MONDAY, "MONDAY", FieldIsBoolean, "0");
    addField(Table_GuardLineTimeSlot, LINESLOT_TUESDAY,"TUESDAY", FieldIsBoolean, "0");
    addField(Table_GuardLineTimeSlot, LINESLOT_WEDNESDAY,"WEDNESDAY", FieldIsBoolean, "0");
    addField(Table_GuardLineTimeSlot, LINESLOT_THURSDAY, "THURSDAY", FieldIsBoolean, "0");
    addField(Table_GuardLineTimeSlot, LINESLOT_FRIDAY,   "FRIDAY", FieldIsBoolean, "0");
    addField(Table_GuardLineTimeSlot, LINESLOT_SATURDAY, "SATURDAY", FieldIsBoolean, "0");
    addField(Table_GuardLineTimeSlot, LINESLOT_SUNDAY,   "SUNDAY", FieldIsBoolean, "0");
    addField(Table_GuardLineTimeSlot, LINESLOT_SPECIFIEDDATES,"DATES", FieldIsShortText);
    addField(Table_GuardLineTimeSlot, LINESLOT_BEGINHOUR,     "BEGINHOUR", FieldIsTime);
    addField(Table_GuardLineTimeSlot, LINESLOT_ENDHOUR,       "ENDHOUR", FieldIsTime);
    addField(Table_GuardLineTimeSlot, LINESLOT_PERIOD_TYPE,   "PERIOD_TYPE", FieldIsTwoChars);
    addField(Table_GuardLineTimeSlot, LINESLOT_PERIOD_INFORMATION, "PERIOD_INFO", FieldIsShortText);
    addIndex(Table_GuardLineTimeSlot, LINESLOT_ID);
    addIndex(Table_GuardLineTimeSlot, LINESLOT_FK);

    addField(Table_Location, LOCATION_ID,   "ID", FieldIsUniquePrimaryKey);
    addField(Table_Location, LOCATION_UID,  "UID", FieldIsUUID);
    addField(Table_Location, LOCATION_LABEL,"LABEL", FieldIsShortText);

    addField(Table_LocationLink, LOCATIONLINK_FK,   "ID", FieldIsTwoChars);
    addField(Table_LocationLink, LOCATIONLINK_LOCATION_ID, "LOCATION_ID", FieldIsTwoChars);
    addIndex(Table_LocationLink, LOCATIONLINK_FK);
    addIndex(Table_LocationLink, LOCATIONLINK_LOCATION_ID);

    // Connect first run database creation requested
    connect(Core::ICore::instance(), SIGNAL(firstRunDatabaseCreation()), this, SLOT(onCoreFirstRunCreationRequested()));
}

/** \brief Destructor. */
GuardBase::~GuardBase()
{
    if (Utils::Log::debugPluginsCreation())
        WARN_FUNC;
    if (d)
        delete d;
    d=0;
}

bool GuardBase::initialize()
{
    // only one base can be initialized
    if (d->_initialized)
        return true;

    // connect
    if (commandLine()->value(Core::ICommandLine::ClearUserDatabases).toBool()) {
        createConnection(Constants::DB_GUARD, Constants::DB_GUARD,
                         settings()->databaseConnector(),
                         Utils::Database::DeleteAndRecreateDatabase);
    } else {
        createConnection(Constants::DB_GUARD, Constants::DB_GUARD,
                         settings()->databaseConnector(),
                         Utils::Database::CreateDatabase);
    }

    if (!database().isOpen()) {
        if (!database().open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_GUARD).arg(database().lastError().text()));
        } else {
            LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().databaseName()).arg(database().driverName()));
        }
    } else {
        LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().databaseName()).arg(database().driverName()));
    }

    if (!checkDatabaseVersion()) {
        LOG_ERROR("Wrong database version");
        return false;
    }

    if (!checkDatabaseScheme()) {
        LOG_ERROR("Wrong database schema");
        return false;
    }

    connect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
    d->_initialized = true;
    return true;
}

bool GuardBase::isInitialized() const
{
    return d->_initialized;
}

void GuardBase::logChronos(bool log)
{
    d->_logChrono = log;
}

/**
 * Checks the current version of the database.\n
 * If required, update the database scheme.\n
 * Returns \e true if the version is the lastest one.
 */
bool GuardBase::checkDatabaseVersion()
{
    Utils::Field vField(Constants::Table_VERSION, Constants::VERSION_TEXT);
    return (getVersion(vField).compare(Constants::DB_ACTUALVERSION) == 0);
//    // Updates from 0.1
//    if (currentVersion == "0.1") {
//        if (!alterTableForNewField(Constants::Table_EPISODES, Constants::EPISODES_PRIORITY))
//            return false;
//        LOG(tr("Episode database updated from version %1 to version: %2")
//            .arg("0.1")
//            .arg(Constants::DB_ACTUALVERSION));
//    }
//    // Update the database version
//    return setVersion(vField, Constants::DB_ACTUALVERSION);
}

bool GuardBase::createVirtuals(int nb)
{
//    if (max(Constants::Table_BankDetails, Constants::BANKDETAILS_ID).toInt() < 1) {
//        QSqlQuery query(database());
//        query.prepare(prepareInsertQuery(Constants::Table_BankDetails));
//        query.bindValue(Constants::BANKDETAILS_ID, QVariant());
//        query.bindValue(Constants::BANKDETAILS_USER_UID, user()->uuid());
//        query.bindValue(Constants::BANKDETAILS_LABEL, d->r.randomWords(d->r.randomInt(0, 10)));
//        query.bindValue(Constants::BANKDETAILS_OWNER, d->r.randomName());
//        query.bindValue(Constants::BANKDETAILS_OWNERADRESS, d->r.randomWords(d->r.randomInt(0, 10)));
//        query.bindValue(Constants::BANKDETAILS_ACCOUNTNUMBER, d->r.randomString(16));
//        query.bindValue(Constants::BANKDETAILS_IBAN, d->r.randomString(16));
//        query.bindValue(Constants::BANKDETAILS_COMMENT, d->r.randomWords(d->r.randomInt(0, 10)));
//        query.bindValue(Constants::BANKDETAILS_DEFAULT, 1);
//        if (!query.exec()) {
//            LOG_QUERY_ERROR(query);
//        }
//    }
//    QList<Fee> fees;
//    QList<Payment> payments;
//    for(int i=0; i < nb; ++i) {
//        Fee fee;
//        // fee.setId();
//        fee.setValid(d->r.randomBool());
//        fee.setPatientUuid("all");
//        fee.setUserUuid("all");
//        fee.setType(d->r.randomString(d->r.randomInt(10)));
//        fee.setAmount(d->r.randomDouble(10., 1000.));
//        fee.setLabel(d->r.randomWords(d->r.randomInt(10)));
//        fee.setComment(d->r.randomWords(d->r.randomInt(20)));
//        fee.setDate(VariableDatesItem::Date_Creation, d->r.randomDateTime(QDateTime::currentDateTime().addDays(-5)));
//        fees << fee;
//    }

//    save(fees);
//    for(int i=0; i < nb; ++i) {
//        Payment pay;
//        // pay.setId();
//        pay.setValid(d->r.randomBool());
//        pay.setType(Payment::PaymentType(d->r.randomInt(0, Payment::Other)));
//        pay.setDate(VariableDatesItem::Date_Creation, d->r.randomDateTime(QDateTime::currentDateTime().addDays(-5)));
//        double percent = d->r.randomDouble(30., 100.);
//        pay.addPaidFee(PaidFee(fees.at(i), percent));
//        pay.setAmount(fees.at(i).amount() * (percent / 100.));
//        payments << pay;
//    }
//    save(payments);

//    Banking bkg;
//    bkg.setDate(VariableDatesItem::Date_Creation, d->r.randomDateTime(QDateTime::currentDateTime()));
//    for(int i=0; i < payments.count(); ++i) {
//        if (d->r.randomBool() && d->r.randomBool() && d->r.randomBool())
//            bkg.addPayment(payments.at(i));
//    }
//    bkg.computeTotalAmount();
//    save(bkg);
    return true;
}

bool GuardBase::createDatabase(const QString &connectionName,
                                 const QString &dbName,
                                 const QString &pathOrHostName,
                                 TypeOfAccess access,
                                 AvailableDrivers driver,
                                 const QString &login,
                                 const QString &pass,
                                 const int port,
                                 CreationOption createOption
                                 )
{
    // TODO: manage createOption
    // TODO: manage access
    Q_UNUSED(createOption);
    Q_UNUSED(access);

    if (connectionName != Constants::DB_GUARD)
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
        DB.open();
        setDriver(Utils::Database::SQLite);
    }
    else if (driver == MySQL) {
        DB = QSqlDatabase::database(connectionName);
        if (!DB.open()) {
            QSqlDatabase d = QSqlDatabase::addDatabase("QMYSQL", "__GUARD_CREATOR");
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
        if (QSqlDatabase::connectionNames().contains("__GUARD_CREATOR"))
            QSqlDatabase::removeDatabase("__GUARD_CREATOR");
        DB.open();
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
    if (!setVersion(Utils::Field(Constants::Table_VERSION, Constants::VERSION_TEXT), Constants::DB_ACTUALVERSION)) {
        LOG_ERROR_FOR("GuardBase", "Unable to set version");
    }
    return true;
}

void GuardBase::onCoreDatabaseServerChanged()
{
    d->_initialized = false;
    if (QSqlDatabase::connectionNames().contains(Constants::DB_GUARD)) {
        QSqlDatabase::removeDatabase(Constants::DB_GUARD);
    }
    disconnect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
    initialize();
}

void GuardBase::onCoreFirstRunCreationRequested()
{
    disconnect(Core::ICore::instance(), SIGNAL(firstRunDatabaseCreation()), this, SLOT(onCoreFirstRunCreationRequested()));
    initialize();
}
