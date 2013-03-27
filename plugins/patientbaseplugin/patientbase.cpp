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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 * \class Patients::Internal::PatientBase
 * \internal
 * Own the database connection mecanisms. The Patients::PatientModel
 * is a wrapper to the database and allow you to save/update/create patient data.
 *
 * Avoid to access directly to the database, use the Patients::PatientModel
 * model or the Core::IPatient model.
 * \sa Patients::PatientModel, Core::IPatient
 */
#include "patientbase.h"
#include "constants_db.h"

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
#include <QByteArray>
#include <QBuffer>

using namespace Patients::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::IUser *user() {return Core::ICore::instance()->user();}
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

PatientBase *PatientBase::m_Instance = 0;

PatientBase *PatientBase::instance()
{
    Q_ASSERT(m_Instance);
    return m_Instance;
}

/** Unique instance is created and managed by the Patients::PatientCore object */
PatientBase::PatientBase(QObject *parent) :
    QObject(parent), Utils::Database(),
    m_initialized(false)
{
    m_Instance = this;
    setObjectName("PatientBase");

    using namespace Patients::Constants;
    addTable(Table_IDENT, "PATIENT_IDENTITY");

    // Identifiers
    addField(Table_IDENT, IDENTITY_ID, "IDENT_ID", FieldIsUniquePrimaryKey);
    addField(Table_IDENT, IDENTITY_UID, "IDENT_UID", FieldIsUUID);
    addField(Table_IDENT, IDENTITY_LK_TOPRACT_LKID, "TO_PRACT_LKID", FieldIsLongInteger);
    addField(Table_IDENT, IDENTITY_FAMILY_UID, "IDENT_FAMILY_UID", FieldIsUUID);
    addField(Table_IDENT, IDENTITY_ISACTIVE, "IDENT_ISACTIVE", FieldIsBoolean, "1");
    addField(Table_IDENT, IDENTITY_ISVIRTUAL, "IDENT_ISVIRTUAL", FieldIsBoolean, "0");
    addIndex(Table_IDENT, IDENTITY_UID);
    addIndex(Table_IDENT, IDENTITY_LK_TOPRACT_LKID);
    addIndex(Table_IDENT, IDENTITY_FAMILY_UID);

    // Identity
    addField(Table_IDENT, IDENTITY_USUALNAME, "NAME", FieldIsShortText);
    addField(Table_IDENT, IDENTITY_FIRSTNAME, "SURNAME", FieldIsShortText);
    addField(Table_IDENT, IDENTITY_OTHERNAMES, "SECONDNAME", FieldIsShortText);
    addField(Table_IDENT, IDENTITY_GENDER, "GENDER", FieldIsOneChar);
    addField(Table_IDENT, IDENTITY_TITLE, "TITLE", FieldIsInteger);
    addField(Table_IDENT, IDENTITY_DOB, "DOB", FieldIsDate);
    addField(Table_IDENT, IDENTITY_MARITAL_STATUS, "MARITAL_STATUS", FieldIsOneChar, "NULL");
    addField(Table_IDENT, IDENTITY_DATEOFDEATH, "DATEOFDEATH", FieldIsDate, "NULL");
    addField(Table_IDENT, IDENTITY_PROFESSION, "PROFESSION", FieldIsShortText, "NULL");
    addIndex(Table_IDENT, IDENTITY_USUALNAME);
    addIndex(Table_IDENT, IDENTITY_FIRSTNAME);
    addIndex(Table_IDENT, IDENTITY_OTHERNAMES);
    addIndex(Table_IDENT, IDENTITY_DOB);

    // Contact
    addField(Table_IDENT, IDENTITY_ADDRESS_STREET, "STREET", FieldIsShortText);
    addField(Table_IDENT, IDENTITY_ADDRESS_STREET_NUMBER, "STREET_NUMBER", FieldIsShortText);
    addField(Table_IDENT, IDENTITY_ADDRESS_ZIPCODE, "ZIP", FieldIsShortText);
    addField(Table_IDENT, IDENTITY_ADDRESS_CITY, "CITY", FieldIsShortText);
    addField(Table_IDENT, IDENTITY_ADDRESS_PROVINCE, "PROV", FieldIsShortText);
    addField(Table_IDENT, IDENTITY_ADDRESS_COUNTRY, "COUNTRY", FieldIsLanguageText);  // two chars field
    addField(Table_IDENT, IDENTITY_ADDRESS_NOTE, "NOTE", FieldIsLongText);
    addField(Table_IDENT, IDENTITY_MAILS, "MAILS", FieldIsLongText);  // Context:Value;Context;Value... Work:eric@work.fr...
    addField(Table_IDENT, IDENTITY_TELS, "TELS", FieldIsLongText);  // Context:Value;Context;Value...
    addField(Table_IDENT, IDENTITY_FAXES, "FAXES", FieldIsLongText);  // Context:Value;Context;Value...

    // Photo
    addTable(Table_PATIENT_PHOTO, "PATIENT_PHOTO");
    addField(Table_PATIENT_PHOTO, PHOTO_ID, "PHOTO_ID", FieldIsUniquePrimaryKey);
    addField(Table_PATIENT_PHOTO, PHOTO_UID, "PHOTO_UID", FieldIsUUID);
    addField(Table_PATIENT_PHOTO, PHOTO_PATIENT_UID, "PATIENT_UID", FieldIsUUID);
    addField(Table_PATIENT_PHOTO, PHOTO_BLOB, "PHOTO", FieldIsBlob);
    addIndex(Table_PATIENT_PHOTO, PHOTO_UID);
    addIndex(Table_PATIENT_PHOTO, PHOTO_PATIENT_UID);

    // Version
    addTable(Table_VERSION, "VERSION");
    addField(Table_VERSION, VERSION_TEXT, "VERSION", FieldIsShortText);

    // Connect first run database creation requested
    connect(Core::ICore::instance(), SIGNAL(firstRunDatabaseCreation()), this, SLOT(onCoreFirstRunCreationRequested()));
}

PatientBase::~PatientBase()
{
}

/** Initialize and connect to the database */
bool PatientBase::initialize()
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

    connect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
    m_initialized = true;
    return true;
}

/** Creates a virtual patient with the specified data. Virtual patient can be hidden from the ui using a preference setting. */
bool PatientBase::createVirtualPatient(const QString &usualName, const QString &otherNames, const QString &firstname,
                          const QString &gender, const int title, const QDate &dob,
                          const QString &country, const QString &note,
                          const QString &street, const QString &zip, const QString &city,
                          QString uuid, const int lkid,
                          const QString &photoFile, const QDate &death)
{
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_NAME);
    if (!connectDatabase(DB, __LINE__)) {
        return false;
    }
    using namespace Patients::Constants;
    if (uuid.isEmpty()) {
        uuid = Utils::Database::createUid();
    } else {
        QHash<int, QString> where;
        where.insert(IDENTITY_UID, QString("='%1'").arg(uuid));
        int alreadyInBase = count(Table_IDENT, IDENTITY_UID, getWhereClause(Table_IDENT, where));
        if (alreadyInBase!=0) {
            LOG_ERROR("Patient is already in base, virtual patient not created");
            return false;
        }
    }
    DB.transaction();
    QSqlQuery query(DB);
    query.prepare(prepareInsertQuery(Table_IDENT));
    query.bindValue(IDENTITY_ID, QVariant());
    query.bindValue(IDENTITY_UID, uuid);
    query.bindValue(IDENTITY_LK_TOPRACT_LKID, lkid);
    query.bindValue(IDENTITY_FAMILY_UID, "Not yet implemented");
    query.bindValue(IDENTITY_ISVIRTUAL, 1);
    query.bindValue(IDENTITY_USUALNAME, usualName);
    query.bindValue(IDENTITY_FIRSTNAME, firstname);
    if (otherNames.isEmpty())
        query.bindValue(IDENTITY_OTHERNAMES, QVariant());
    else
        query.bindValue(IDENTITY_OTHERNAMES, otherNames);
    query.bindValue(IDENTITY_GENDER, gender);
    query.bindValue(IDENTITY_TITLE, title);
    query.bindValue(IDENTITY_DOB, dob);
    query.bindValue(IDENTITY_MARITAL_STATUS, QVariant());
    if (death.isValid()) {
        query.bindValue(IDENTITY_ISACTIVE, 0);
        query.bindValue(IDENTITY_DATEOFDEATH, death);
    } else {
        query.bindValue(IDENTITY_ISACTIVE, 1);
        query.bindValue(IDENTITY_DATEOFDEATH, QVariant());
    }
    query.bindValue(IDENTITY_PROFESSION, QVariant());
    query.bindValue(IDENTITY_ADDRESS_STREET, street);
    query.bindValue(IDENTITY_ADDRESS_STREET_NUMBER, QVariant());
    query.bindValue(IDENTITY_ADDRESS_ZIPCODE, zip);
    query.bindValue(IDENTITY_ADDRESS_CITY, city);
    query.bindValue(IDENTITY_ADDRESS_COUNTRY, country);
    query.bindValue(IDENTITY_ADDRESS_PROVINCE, QVariant());
    query.bindValue(IDENTITY_ADDRESS_NOTE, note);
    query.bindValue(IDENTITY_MAILS, QVariant());
    query.bindValue(IDENTITY_TELS, QVariant());
    query.bindValue(IDENTITY_FAXES, QVariant());

    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    if (!query.exec()) {
        LOG_QUERY_ERROR_FOR("PatientBase", query);
        query.finish();
        DB.rollback();
        return false;
    }
    query.finish();

    if (!photoFile.isEmpty()) {
        QPixmap pix(photoFile);
        if (pix.isNull())
            return false;
        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        pix.save(&buffer, "PNG"); // writes image into ba in PNG format {6a247e73-c241-4556-8dc8-c5d532b8457e}
        query.prepare(prepareInsertQuery(Table_PATIENT_PHOTO));
        query.bindValue(PHOTO_ID, QVariant());
        query.bindValue(PHOTO_UID, Utils::Database::createUid());
        query.bindValue(PHOTO_PATIENT_UID, uuid);
        query.bindValue(PHOTO_BLOB, ba);
        if (!query.exec()) {
            LOG_QUERY_ERROR_FOR("PatientBase", query);
            query.finish();
            DB.rollback();
            return false;
        }
    }
    query.finish();
    DB.commit();
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    return true;
}

/** Return a patient uuid in the database or a QString::null. */
QString PatientBase::patientUuid(const QString &usualname,
                                 const QString &othernames,
                                 const QString &firstname,
                                 const QString &gender,
                                 const QDate &dob) const
{
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_NAME);
    if (!connectDatabase(DB, __LINE__)) {
        return QString::null;
    }
    using namespace Patients::Constants;
    QHash<int, QString> where;
    where.insert(IDENTITY_USUALNAME, QString("='%1'").arg(usualname));
    where.insert(IDENTITY_FIRSTNAME, QString("='%1'").arg(firstname));
    where.insert(IDENTITY_OTHERNAMES, QString("='%1'").arg(othernames));
    where.insert(IDENTITY_GENDER, QString("='%1'").arg(gender));
    where.insert(IDENTITY_DOB, QString("='%1'").arg(dob.toString(Qt::ISODate)));
    QString req = select(Table_IDENT, IDENTITY_UID, where);
    DB.transaction();
    QSqlQuery query(DB);
    QString toReturn;
    if (query.exec(req)) {
        if (query.next()) {
            toReturn = query.value(0).toString();
        }
    } else {
        LOG_QUERY_ERROR_FOR("PatientBase", query);
    }
    query.finish();
    DB.commit();
    return toReturn;
}

/** Test the existence of a patient in the database. */
bool PatientBase::isPatientExists(const QString &usualname,
                                  const QString &othernames,
                                  const QString &firstname,
                                  const QString &gender,
                                  const QDate &dob) const
{
    return (!patientUuid(usualname, othernames, firstname, gender, dob).isNull());
}

/**
 * Activate or desactivate a patient (eq to remove patient).
 * Manage its own transaction.
 */
bool PatientBase::setPatientActiveProperty(const QString &uuid, bool active)
{
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_NAME);
    if (!connectDatabase(DB, __LINE__)) {
        return false;
    }
    using namespace Patients::Constants;
    QHash<int, QString> where;
    where.insert(IDENTITY_UID, QString("='%1'").arg(uuid));
    if (count(Table_IDENT, IDENTITY_UID, getWhereClause(Table_IDENT, where)) <= 0) {
        LOG_ERROR(QString("When trying to change the active property of patient: %1; patient does not exist").arg(uuid));
        return false;
    }
    DB.transaction();
    QSqlQuery query(DB);
    query.prepare(prepareUpdateQuery(Table_IDENT, IDENTITY_ISACTIVE, where));
    query.bindValue(0, int(active));
    if (!query.exec()) {
        LOG_QUERY_ERROR_FOR("PatientBase", query);
        query.finish();
        DB.rollback();
        return false;
    }
    query.finish();
    DB.commit();
    return true;
}

/** Private part of the Patients::PatientBase that creates the database. \sa Utils::Database. */
bool PatientBase::createDatabase(const QString &connectionName , const QString &dbName,
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
                LOG(tkTr(Trans::Constants::_1_ISNOT_AVAILABLE_CANNOTBE_CREATED).arg(pathOrHostName));
        DB.setDatabaseName(QDir::cleanPath(pathOrHostName + QDir::separator() + dbName));
        if (!DB.open())
            LOG(tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2).arg(dbName).arg(DB.lastError().text()));
        setDriver(Utils::Database::SQLite);
    }
    else if (driver == MySQL) {
        DB = QSqlDatabase::database(connectionName);
        if (!DB.open()) {
            QSqlDatabase dbcreate = QSqlDatabase::addDatabase("QMYSQL", "__PATIENTS_CREATOR");
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
        if (QSqlDatabase::connectionNames().contains("__PATIENTS_CREATOR"))
            QSqlDatabase::removeDatabase("__PATIENTS_CREATOR");
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
    DB.transaction();
    QSqlQuery query(DB);
    query.prepare(prepareInsertQuery(Constants::Table_VERSION));
    query.bindValue(Constants::VERSION_TEXT, Constants::DB_ACTUALVERSION);
    if (!query.exec()) {
        LOG_QUERY_ERROR(query);
        DB.rollback();
        return false;
    }
    query.finish();
    DB.commit();
    return true;
}

/** Reconnect the database when the database server changes. \sa Core::ICore::databaseServerChanged(), Core::ISettings::databaseConnector() */
void PatientBase::onCoreDatabaseServerChanged()
{
    m_initialized = false;
    if (QSqlDatabase::connectionNames().contains(Constants::DB_NAME)) {
        QSqlDatabase::removeDatabase(Constants::DB_NAME);
    }
    disconnect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
    disconnect(Core::ICore::instance(), SIGNAL(firstRunDatabaseCreation()), this, SLOT(onCoreFirstRunCreationRequested()));
    initialize();
}

void PatientBase::onCoreFirstRunCreationRequested()
{
    disconnect(Core::ICore::instance(), SIGNAL(firstRunDatabaseCreation()), this, SLOT(onCoreFirstRunCreationRequested()));
    initialize();
}


/** For debugging purpose */
void PatientBase::toTreeWidget(QTreeWidget *tree) const
{
    Database::toTreeWidget(tree);
    QString uuid = user()->uuid();
    QHash<int, QString> where;
    // TODO: here
//    where.clear();
//    where.insert(Constants::LK_TOPRACT_PRACT_UUID, QString("='%1'").arg(uuid));
//    QString req = select(Constants::Table_LK_TOPRACT, Constants::LK_TOPRACT_LKID, where);
//    where.clear();
//    where.insert(Constants::IDENTITY_LK_TOPRACT_LKID, QString("IN (%1)").arg(req));
//    req = getWhereClause(Constants::Table_IDENT, where);
    QFont bold;
    bold.setBold(true);
    QTreeWidgetItem *db = new QTreeWidgetItem(tree, QStringList() << "Patients count");
    db->setFont(0, bold);
    new QTreeWidgetItem(db, QStringList() << "Total patients" << QString::number(count(Constants::Table_IDENT, Constants::IDENTITY_ID)));
//    new QTreeWidgetItem(db, QStringList() << "User's patients" << QString::number(count(Constants::Table_IDENT, Constants::IDENTITY_ID, req)));
    tree->expandAll();
}
