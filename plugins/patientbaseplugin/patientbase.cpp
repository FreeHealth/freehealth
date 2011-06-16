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
#include "patientbase.h"
#include "constants_db.h"

#include <utils/global.h>
#include <utils/log.h>
#include <utils/databaseconnector.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/isettings.h>
#include <coreplugin/icore.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/iuser.h>

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QDir>
#include <QUuid>
#include <QProgressDialog>
#include <QTreeWidgetItem>
#include <QFont>

using namespace Patients::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::IUser *user() {return Core::ICore::instance()->user();}


/** \todo move getLkId into UserModel/UserBase */

//namespace Patients {
//namespace Internal {
//class PatientBasePrivate
//{
//public:
//    PatientBasePrivate(PatientBase *parent = 0) : q(parent) {}
//    ~PatientBasePrivate () {}
//
//    void checkDatabaseVersion()
//    {}
//
//private:
//    PatientBase *q;
//};
//}
//}

PatientBase *PatientBase::m_Instance = 0;
bool PatientBase::m_initialized = false;

PatientBase *PatientBase::instance()
{
    if (!m_Instance) {
        m_Instance = new PatientBase(qApp);
        m_Instance->init();
    }
    return m_Instance;
}

PatientBase::PatientBase(QObject *parent) :
        QObject(parent), Utils::Database()
//        d_prt(new PatientBasePrivate(this))
{
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

    // Identity
    addField(Table_IDENT, IDENTITY_NAME, "NAME", FieldIsShortText);
    addField(Table_IDENT, IDENTITY_FIRSTNAME, "SURNAME", FieldIsShortText);
    addField(Table_IDENT, IDENTITY_SECONDNAME, "SECONDNAME", FieldIsShortText);
    addField(Table_IDENT, IDENTITY_GENDER, "GENDER", FieldIsOneChar);
    addField(Table_IDENT, IDENTITY_TITLE, "TITLE", FieldIsInteger);
    addField(Table_IDENT, IDENTITY_DOB, "DOB", FieldIsDate);
    addField(Table_IDENT, IDENTITY_MARITAL_STATUS, "MARITAL_STATUS", FieldIsOneChar, "NULL");
    addField(Table_IDENT, IDENTITY_DATEOFDEATH, "DATEOFDEATH", FieldIsDate, "NULL");
    addField(Table_IDENT, IDENTITY_PROFESSION, "PROFESSION", FieldIsShortText, "NULL");

    // Contact
    addField(Table_IDENT, IDENTITY_ADDRESS_STREET, "STREET", FieldIsShortText);
    addField(Table_IDENT, IDENTITY_ADDRESS_STREET_NUMBER, "STREET_NUMBER", FieldIsShortText);
    addField(Table_IDENT, IDENTITY_ADDRESS_ZIPCODE, "ZIP", FieldIsShortText);
    addField(Table_IDENT, IDENTITY_ADRESS_CITY, "CITY", FieldIsShortText);
    addField(Table_IDENT, IDENTITY_ADDRESS_COUNTRY, "COUNTRY", FieldIsShortText);
    addField(Table_IDENT, IDENTITY_ADDRESS_NOTE, "NOTE", FieldIsLongText);
    addField(Table_IDENT, IDENTITY_MAILS, "MAILS", FieldIsLongText);  // Context:Value;Context;Value... Work:eric@work.fr...
    addField(Table_IDENT, IDENTITY_TELS, "TELS", FieldIsLongText);  // Context:Value;Context;Value...
    addField(Table_IDENT, IDENTITY_FAXES, "FAXES", FieldIsLongText);  // Context:Value;Context;Value...

    // Automate Indexes creation ? or hard coding of indexes
    // Needed : documents, prescriptions...
    // Insurances ?

    // Photo
    addTable(Table_PATIENT_PHOTO, "PATIENT_PHOTO");
    addField(Table_PATIENT_PHOTO, PHOTO_ID, "PHOTO_ID", FieldIsUniquePrimaryKey);
    addField(Table_PATIENT_PHOTO, PHOTO_UID, "PHOTO_UID", FieldIsUUID);
    addField(Table_PATIENT_PHOTO, PHOTO_PATIENT_UID, "PATIENT_UID", FieldIsUUID);
    addField(Table_PATIENT_PHOTO, PHOTO_BLOB, "PHOTO", FieldIsBlob);

    // Version
    addTable(Table_VERSION, "VERSION");
    addField(Table_VERSION, VERSION_TEXT, "VERSION", FieldIsShortText);

    connect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
}

PatientBase::~PatientBase()
{
//    if (d) {
//        delete d;
//        d = 0;
//    }
}

bool PatientBase::init()
{
    // only one base can be initialized
    if (m_initialized)
        return true;

    // connect
    createConnection(Constants::DB_NAME, Constants::DB_NAME,
                     settings()->databaseConnector(),
                     Utils::Database::CreateDatabase);

//    // Check settings --> SQLite or MySQL ?
//    if (settings()->value(Core::Constants::S_USE_EXTERNAL_DATABASE, false).toBool()) {
//        if (!QSqlDatabase::isDriverAvailable("QMYSQL")) {
//            LOG_ERROR(tkTr(Trans::Constants::DATABASE_DRIVER_1_NOT_AVAILABLE).arg("MySQL"));
//            Utils::warningMessageBox(tkTr(Trans::Constants::APPLICATION_FAILURE),
//                                     tkTr(Trans::Constants::DATABASE_DRIVER_1_NOT_AVAILABLE_DETAIL).arg("MySQL"),
//                                     "", qApp->applicationName());
//            return false;
//        }
//        createConnection(Constants::DB_NAME,
//                         Constants::DB_NAME,
//                         QString(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_HOST, QByteArray("localhost").toBase64()).toByteArray())),
//                         Utils::Database::ReadWrite,
//                         Utils::Database::MySQL,
//                         QString(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_LOG, QByteArray("root").toBase64()).toByteArray())),
//                         QString(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_PASS, QByteArray("").toBase64()).toByteArray())),
//                         QString(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_PORT, QByteArray("").toBase64()).toByteArray())).toInt(),
//                         Utils::Database::CreateDatabase);
//    } else {
//        if (!QSqlDatabase::isDriverAvailable("QSQLITE")) {
//            LOG_ERROR(tkTr(Trans::Constants::DATABASE_DRIVER_1_NOT_AVAILABLE).arg("SQLite"));
//            Utils::warningMessageBox(tkTr(Trans::Constants::APPLICATION_FAILURE),
//                                     tkTr(Trans::Constants::DATABASE_DRIVER_1_NOT_AVAILABLE_DETAIL).arg("SQLite"),
//                                     "", qApp->applicationName());
//            return false;
//        }
//        createConnection(Constants::DB_NAME,
//                         Constants::DB_FILENAME,
//                         settings()->path(Core::ISettings::ReadWriteDatabasesPath) + QDir::separator() + QString(Constants::DB_NAME),
//                         Utils::Database::ReadWrite,
//                         Utils::Database::SQLite,
//                         "log", "pas", 0,
//                         Utils::Database::CreateDatabase);
//    }

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

bool PatientBase::createDatabase(const QString &connectionName , const QString &dbName,
                    const QString &pathOrHostName,
                    TypeOfAccess access, AvailableDrivers driver,
                    const QString & login, const QString & pass,
                    const int port,
                    CreationOption /*createOption*/
                   )
{
    if (connectionName != Constants::DB_NAME)
        return false;

    LOG(tkTr(Trans::Constants::TRYING_TO_CREATE_1_PLACE_2)
        .arg(dbName).arg(pathOrHostName));

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
            QSqlDatabase d = QSqlDatabase::addDatabase("QMYSQL", "__PATIENTS_CREATOR");
            d.setHostName(pathOrHostName);
            d.setUserName(login);
            d.setPassword(pass);
            d.setPort(port);
            if (!d.open()) {
                Utils::warningMessageBox(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                         .arg(DB.connectionName()).arg(DB.lastError().text()),
                                         tr("Please contact dev team."));
                return false;
            }
            QSqlQuery q(QString("CREATE DATABASE `%1`").arg(dbName), d);
            if (!q.isActive()) {
                LOG_QUERY_ERROR(q);
                Utils::warningMessageBox(tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2)
                                         .arg(DB.connectionName()).arg(DB.lastError().text()),
                                         tr("Please contact dev team."));
                return false;
            }
            if (!DB.open()) {
                Utils::warningMessageBox(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                         .arg(DB.connectionName()).arg(DB.lastError().text()),
                                         tr("Please contact dev team."));
                return false;
            }
            DB.setDatabaseName(dbName);
        }
        if (QSqlDatabase::connectionNames().contains("__PATIENTS_CREATOR"))
            QSqlDatabase::removeDatabase("__PATIENTS_CREATOR");
        if (!DB.open()) {
            Utils::warningMessageBox(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                     .arg(DB.connectionName()).arg(DB.lastError().text()),
                                     tr("Please contact dev team."));
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
    query.prepare(prepareInsertQuery(Constants::Table_VERSION));
    query.bindValue(Constants::VERSION_TEXT, Constants::DB_ACTUALVERSION);
    if (!query.exec()) {
        LOG_QUERY_ERROR(query);
        return false;
    }

    return true;
}

void PatientBase::onCoreDatabaseServerChanged()
{
    m_initialized = false;
    if (QSqlDatabase::connectionNames().contains(Constants::DB_NAME)) {
        QSqlDatabase::removeDatabase(Constants::DB_NAME);
    }
    init();
}

void PatientBase::toTreeWidget(QTreeWidget *tree)
{
    Database::toTreeWidget(tree);
    QString uuid = user()->uuid();
    QHash<int, QString> where;
    /** \todo here */
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
