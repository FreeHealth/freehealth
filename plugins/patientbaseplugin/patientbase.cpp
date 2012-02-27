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
 *   Main Developer : Eric MAEKER, <eric.maeker@gmail.com>                *
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
    addIndex(Table_IDENT, IDENTITY_UID);
    addIndex(Table_IDENT, IDENTITY_LK_TOPRACT_LKID);
    addIndex(Table_IDENT, IDENTITY_FAMILY_UID);

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
    addIndex(Table_IDENT, IDENTITY_NAME);
    addIndex(Table_IDENT, IDENTITY_FIRSTNAME);
    addIndex(Table_IDENT, IDENTITY_SECONDNAME);
    addIndex(Table_IDENT, IDENTITY_DOB);

    // Contact
    addField(Table_IDENT, IDENTITY_ADDRESS_STREET, "STREET", FieldIsShortText);
    addField(Table_IDENT, IDENTITY_ADDRESS_STREET_NUMBER, "STREET_NUMBER", FieldIsShortText);
    addField(Table_IDENT, IDENTITY_ADDRESS_ZIPCODE, "ZIP", FieldIsShortText);
    addField(Table_IDENT, IDENTITY_ADRESS_CITY, "CITY", FieldIsShortText);
    addField(Table_IDENT, IDENTITY_ADRESS_PROVINCE, "PROV", FieldIsShortText);
    addField(Table_IDENT, IDENTITY_ADDRESS_COUNTRY, "COUNTRY", FieldIsLanguageText);  // two chars field
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
    addIndex(Table_PATIENT_PHOTO, PHOTO_UID);
    addIndex(Table_PATIENT_PHOTO, PHOTO_PATIENT_UID);

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

static inline Patients::Internal::PatientBase *patientBase()  { return Patients::Internal::PatientBase::instance(); }

void PatientBase::createVirtualPatient(const QString &name, const QString &secondname, const QString &firstname,
                          const QString &gender, const int title, const QDate &dob,
                          const QString &country, const QString &note,
                          const QString &street, const QString &zip, const QString &city,
                          QString uuid, const int lkid,
                          const QString &photoFile, const QDate &death)
{
    using namespace Patients::Constants;
    if (uuid.isEmpty()) {
        uuid = Utils::Database::createUid();
    } else {
        QHash<int, QString> where;
        where.insert(IDENTITY_UID, QString("='%1'").arg(uuid));
        int alreadyInBase = count(Table_IDENT, IDENTITY_UID, getWhereClause(Table_IDENT, where));
        if (alreadyInBase!=0) {
            LOG_ERROR("Patient is alreday in base, virtual patient not created");
            return;
        }
    }
    QSqlQuery query(patientBase()->database());
    query.prepare(patientBase()->prepareInsertQuery(Table_IDENT));
    query.bindValue(IDENTITY_ID, QVariant());
    query.bindValue(IDENTITY_UID, uuid);
    query.bindValue(IDENTITY_LK_TOPRACT_LKID, lkid);
    query.bindValue(IDENTITY_FAMILY_UID, "Not yet implemented");
    query.bindValue(IDENTITY_ISVIRTUAL, 1);
    query.bindValue(IDENTITY_NAME, name);
    query.bindValue(IDENTITY_FIRSTNAME, firstname);
    if (secondname.isEmpty())
        query.bindValue(IDENTITY_SECONDNAME, QVariant());
    else
        query.bindValue(IDENTITY_SECONDNAME, secondname);
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
    query.bindValue(IDENTITY_ADRESS_CITY, city);
    query.bindValue(IDENTITY_ADDRESS_COUNTRY, country);
    query.bindValue(IDENTITY_ADRESS_PROVINCE, QVariant());
    query.bindValue(IDENTITY_ADDRESS_NOTE, note);
    query.bindValue(IDENTITY_MAILS, QVariant());
    query.bindValue(IDENTITY_TELS, QVariant());
    query.bindValue(IDENTITY_FAXES, QVariant());

    if (!query.exec()) {
        LOG_QUERY_ERROR_FOR("PatientBase", query);
    }
    query.finish();

    if (!photoFile.isEmpty()) {
        QPixmap pix(photoFile);
        if (pix.isNull())
            return;
        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        pix.save(&buffer, "PNG"); // writes image into ba in PNG format {6a247e73-c241-4556-8dc8-c5d532b8457e}
        query.prepare(patientBase()->prepareInsertQuery(Table_PATIENT_PHOTO));
        query.bindValue(PHOTO_ID, QVariant());
        query.bindValue(PHOTO_UID, Utils::Database::createUid());
        query.bindValue(PHOTO_PATIENT_UID, uuid);
        query.bindValue(PHOTO_BLOB, ba);
        if (!query.exec()) {
            LOG_QUERY_ERROR_FOR("PatientBase", query);
        }
    }
}

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
                tkTr(Trans::Constants::_1_ISNOT_AVAILABLE_CANNOTBE_CREATED).arg(pathOrHostName);
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
                                         tr("Please contact dev team."));
                return false;
            }
            QSqlQuery q(QString("CREATE DATABASE `%1`").arg(dbName), dbcreate);
            if (!q.isActive()) {
                LOG_QUERY_ERROR(q);
                Utils::warningMessageBox(tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2)
                                         .arg(dbcreate.connectionName()).arg(dbcreate.lastError().text()),
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
