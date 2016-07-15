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
  \class PMH::PmhBase
  \brief Provides all read/write access to the PMHx database.
  No items are deleted from the database: they are excluded from view by setting
  IsValid to false.
*/

#include "pmhbase.h"
#include "constants.h"
#include "pmhdata.h"

#include <utils/global.h>
#include <utils/log.h>
#include <utils/databaseconnector.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>
#include <translationutils/trans_database.h>
#include <translationutils/trans_msgerror.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/iuser.h>
#include <coreplugin/icommandline.h>

#include <categoryplugin/categorycore.h>

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QDir>

using namespace PMH::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::IPatient *patient()  { return Core::ICore::instance()->patient(); }
static inline Category::CategoryCore *categoryCore() {return Category::CategoryCore::instance();}
static inline Core::ICommandLine *commandLine()  { return Core::ICore::instance()->commandLine(); }

PmhBase *PmhBase::m_Instance = 0;

static inline bool connectDatabase(QSqlDatabase &DB, const int line)
{
    if (!DB.isOpen()) {
        if (!DB.open()) {
            Utils::Log::addError("PmhBase", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                 .arg(DB.connectionName()).arg(DB.lastError().text()),
                                 __FILE__, line);
            return false;
        }
    }
    return true;
}

/** \brief Returns the singleton. There should be only one instance of the PmhBase class. */
PmhBase *PmhBase::instance()
{
    Q_ASSERT(m_Instance);
    return m_Instance;
}

//namespace PMH {
//namespace Internal {
//class PmhBasePrivate
//{
//public:
//    PmhBasePrivate() {}
//    ~PmhBasePrivate() {}

////    void checkPmhDataContent(PmhData *pmh)
////    {
////        // MASTER_ID defined ?
////        if (pmh->data(PmhData::Uid).isNull()) {
////            LOG_ERROR_FOR("PmhBase", "MasterId not defined for PMHx " + pmh->data(PmhData::Label).toString());
////        }

////        // PmhEpisodeData MasterId defined ?
////        foreach(PmhEpisodeData *ep, pmh->episodes()) {

////        }
////    }

//public:

//};
//}
//}

PmhBase::PmhBase(QObject *parent) :
    QObject(parent), Utils::Database(),
    m_initialized(false)
{
    m_Instance = this;
    setObjectName("PmhBase");
    using namespace PMH::Constants;
    addTable(Table_MASTER,        "PMH_MASTER");
    addTable(Table_EPISODE,       "PMH_EPISODE");
    // TODO: add table version && version management of the database
//    addTable(Table_VERSION, "VERSION");

    addField(Table_MASTER, MASTER_ID,            "ID",             FieldIsUniquePrimaryKey);
    addField(Table_MASTER, MASTER_PATIENT_UID,   "PATIENT_UUID",   FieldIsUUID);
    addField(Table_MASTER, MASTER_USER_UID,      "USER_UUID",      FieldIsUUID);
    // TODO: change the category ID to the category uuid? should be better.
    addField(Table_MASTER, MASTER_CATEGORY_ID,   "CATEGORY_ID",    FieldIsInteger);
    addField(Table_MASTER, MASTER_EPISODE_ID,    "MH_EPISODE_ID",  FieldIsInteger);
    addField(Table_MASTER, MASTER_CONTACTS_ID,   "MH_CONTACTS_ID", FieldIsInteger);
    addField(Table_MASTER, MASTER_LABEL,         "LABEL",          FieldIsShortText);
    addField(Table_MASTER, MASTER_TYPE,          "TYPE_ID",        FieldIsInteger);
    addField(Table_MASTER, MASTER_STATE,         "STATE_ID",       FieldIsInteger);
    addField(Table_MASTER, MASTER_CONFINDEX,     "GLOBAL_CONF_INDEX",FieldIsInteger);
    addField(Table_MASTER, MASTER_ISVALID,       "VALID",          FieldIsBoolean, "1");
    addField(Table_MASTER, MASTER_PRIVATE,       "PRIV",           FieldIsBoolean);
    addField(Table_MASTER, MASTER_COMMENT,       "COMMENT",        FieldIsLongText);
    addIndex(Table_MASTER, MASTER_ID);
    addIndex(Table_MASTER, MASTER_PATIENT_UID);
    addIndex(Table_MASTER, MASTER_USER_UID);

    addField(Table_EPISODE, EPISODE_ID,            "ID",             FieldIsUniquePrimaryKey);
    addField(Table_EPISODE, EPISODE_MASTER_ID,     "MASTER_ID",      FieldIsInteger);
    addField(Table_EPISODE, EPISODE_LABEL,         "LABEL",          FieldIsShortText);
    addField(Table_EPISODE, EPISODE_DATE_START,    "DATE_START",     FieldIsDate);
    addField(Table_EPISODE, EPISODE_DATE_END,      "DATE_END",       FieldIsDate);
    addField(Table_EPISODE, EPISODE_CONF_INDEX,    "CONF_INDEX",     FieldIsInteger);
    addField(Table_EPISODE, EPISODE_ICD_CODES,     "XML_ICD",        FieldIsLongText);
    addField(Table_EPISODE, EPISODE_ISVALID,       "VALID",          FieldIsBoolean, "1");
    addField(Table_EPISODE, EPISODE_COMMENT,       "COMMENT",        FieldIsLongText);
    addField(Table_EPISODE, EPISODE_TRACE_ID,      "TRACE_ID",       FieldIsInteger);
    addIndex(Table_EPISODE, EPISODE_ID);
    addIndex(Table_EPISODE, EPISODE_MASTER_ID);
    addIndex(Table_EPISODE, EPISODE_TRACE_ID);

    // Connect first run database creation requested
    connect(Core::ICore::instance(), SIGNAL(firstRunDatabaseCreation()), this, SLOT(onCoreFirstRunCreationRequested()));
}

PmhBase::~PmhBase()
{
}

bool PmhBase::initialize()
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

    if (!checkDatabaseScheme()) {
        LOG_ERROR(tkTr(Trans::Constants::DATABASE_1_SCHEMA_ERROR).arg(Constants::DB_NAME));
        return false;
    }

    connect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
    connect(Core::ICore::instance(), SIGNAL(firstRunDatabaseCreation()), this, SLOT(onCoreFirstRunCreationRequested()));
    m_initialized = true;
    return true;
}

bool PmhBase::createDatabase(const QString &connectionName , const QString &dbName,
                    const QString &pathOrHostName,
                    TypeOfAccess , AvailableDrivers driver,
                    const QString & login, const QString & pass,
                    const int port,
                    CreationOption /*createOption*/
                   )
{
    if (connectionName != Constants::DB_NAME)
        return false;

    setConnectionName(connectionName);
    setDriver(driver);

    LOG(tkTr(Trans::Constants::TRYING_TO_CREATE_1_PLACE_2).arg(dbName).arg(pathOrHostName));

    // create an empty database and connect
    QSqlDatabase DB;
    if (driver == SQLite) {
        DB = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        if (!QDir(pathOrHostName).exists())
            if (!QDir().mkpath(pathOrHostName))
                LOG(tkTr(Trans::Constants::_1_ISNOT_AVAILABLE_CANNOTBE_CREATED).arg(pathOrHostName));
        DB.setDatabaseName(QDir::cleanPath(pathOrHostName + QDir::separator() + dbName));
        DB.open();
    }
    else if (driver == MySQL) {
        DB = QSqlDatabase::database(connectionName);
        if (!DB.open()) {
            QSqlDatabase dbcreate = QSqlDatabase::addDatabase("QMYSQL", "__PMH_CREATOR");
            dbcreate.setHostName(pathOrHostName);
            dbcreate.setUserName(login);
            dbcreate.setPassword(pass);
            dbcreate.setPort(port);
            if (!dbcreate.open()) {
                Utils::warningMessageBox(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                         .arg(DB.connectionName()).arg(DB.lastError().text()),
                                         tkTr(Trans::Constants::CONTACT_DEV_TEAM));
                return false;
            }
            QSqlQuery q(QString("CREATE DATABASE `%1`").arg(dbName), dbcreate);
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
        if (QSqlDatabase::connectionNames().contains("__PMH_CREATOR"))
            QSqlDatabase::removeDatabase("__PMH_CREATOR");
        DB.open();
    }

    if (createTables()) {
        LOG(tkTr(Trans::Constants::DATABASE_1_CORRECTLY_CREATED).arg(dbName));
    } else {
        LOG_ERROR(tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2)
                         .arg(dbName, DB.lastError().text()));
        return false;
    }

    // TODO: add database versionning
//    // Add version number
//    if (!setVersion(Utils::Field(Constants::Table_VERSION, Constants::VERSION_TEXT), Constants::DB_ACTUALVERSION)) {
//        LOG_ERROR_FOR("PmhBase", "Unable to set version");
//    }

    return true;
}

/** \brief Retrieve all PMHx related to a patient. */
QVector<PmhData *> PmhBase::getPmh(const QString &patientUid) const
{
    QVector<PmhData *> pmhs;
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_NAME);
    if (!connectDatabase(DB, __LINE__)) {
        return pmhs;
    }
    QString req;
    DB.transaction();
    QSqlQuery query(DB);
    QHash<int, QString> where;
    // Get Master table
    // TODO: Manage User's Private PMHx
    if (patientUid.isEmpty())
        where.insert(Constants::MASTER_PATIENT_UID,
                     QString("='%1'").arg(patient()->uuid()));
    else
        where.insert(Constants::MASTER_PATIENT_UID,
                     QString("='%1'").arg(patientUid));

    where.insert(Constants::MASTER_ISVALID, "=1");
    //    where.insert(Constants::MASTER_USER_UID,
    //                 QString("='%1'").arg(currentUserUuid()));
    req = select(Constants::Table_MASTER, where);
    if (query.exec(req)) {
        while (query.next()) {
            PmhData *pmh = new PmhData;
            pmh->setData(PmhData::Uid, query.value(Constants::MASTER_ID));
            pmh->setData(PmhData::Label, query.value(Constants::MASTER_LABEL));
            pmh->setData(PmhData::State, query.value(Constants::MASTER_STATE));
            pmh->setData(PmhData::PatientUid, query.value(Constants::MASTER_PATIENT_UID));
            pmh->setData(PmhData::UserOwner, query.value(Constants::MASTER_USER_UID));
            pmh->setData(PmhData::Type, query.value(Constants::MASTER_TYPE));
            pmh->setData(PmhData::ConfidenceIndex, query.value(Constants::MASTER_CONFINDEX));
            pmh->setData(PmhData::Comment, query.value(Constants::MASTER_COMMENT));
            pmh->setData(PmhData::CategoryId, query.value(Constants::MASTER_CATEGORY_ID));
            pmh->setData(PmhData::IsPrivate, query.value(Constants::MASTER_PRIVATE));
            pmh->setData(PmhData::DbOnly_MasterEpisodeId, query.value(Constants::MASTER_EPISODE_ID));
            pmh->setData(PmhData::DbOnly_MasterContactId, query.value(Constants::MASTER_CONTACTS_ID));
            pmhs << pmh;
        }
    } else {
        LOG_QUERY_ERROR(query);
        query.finish();
        DB.rollback();
        return pmhs;
    }
    query.finish();

    // Get Episodes
    where.clear();
    for(int i = 0; i < pmhs.count(); ++i) {
        PmhData *pmh = pmhs.at(i);
        where.insert(Constants::EPISODE_MASTER_ID, QString("='%1'").arg(pmh->data(PmhData::Uid).toString()));
        where.insert(Constants::EPISODE_ISVALID, "=1");
        req = select(Constants::Table_EPISODE, where);
        if (query.exec(req)) {
            while (query.next()) {
                PmhEpisodeData *ep = new PmhEpisodeData;
                ep->setData(PmhEpisodeData::DbOnly_MasterId, query.value(Constants::EPISODE_MASTER_ID));
                ep->setData(PmhEpisodeData::DbOnly_Id, query.value(Constants::EPISODE_ID));
                ep->setData(PmhEpisodeData::Label, query.value(Constants::EPISODE_LABEL));
                ep->setData(PmhEpisodeData::DateStart, query.value(Constants::EPISODE_DATE_START));
                ep->setData(PmhEpisodeData::DateEnd, query.value(Constants::EPISODE_DATE_END));
                ep->setData(PmhEpisodeData::IcdXml, query.value(Constants::EPISODE_ICD_CODES));
                ep->setData(PmhEpisodeData::ConfidenceIndex, query.value(Constants::EPISODE_CONF_INDEX));
                ep->setData(PmhEpisodeData::Comment, query.value(Constants::EPISODE_COMMENT));
                pmh->addEpisode(ep);
            }
        } else {
            LOG_QUERY_ERROR(query);
            query.finish();
            DB.rollback();
            return pmhs;
        }
    }

    // Get Contacts
    where.clear();
    query.finish();
    DB.commit();
    return pmhs;
}

/**
  Return list of Category::CategoryItem extracted from database linked with the root form \e uid.
  \sa Category::CategoryCore
*/
QVector<Category::CategoryItem *> PmhBase::getPmhCategory(const QString &uid) const
{
    return categoryCore()->getCategories(QString("%1@%2").arg(Constants::CATEGORY_MIME).arg(uid));
}

/** \brief Recreate the category tree and return a QList of root categories. \sa Category::CategoryCore */
QList<Category::CategoryItem *> PmhBase::createCategoryTree(const QVector<Category::CategoryItem *> &cats) const
{
    return categoryCore()->createCategoryTree(cats);
}

/** \brief Link PMH with their category. \sa Category::CategoryCore */
bool PmhBase::linkPmhWithCategory(const QVector<Category::CategoryItem *> &cats, const QVector<PmhData *> &pmhs) const
{
    QVector<Category::ICategoryContentItem *> contents;
    for(int i = 0; i < pmhs.count(); ++i) {
        contents.append(static_cast<Category::ICategoryContentItem*>(pmhs.at(i)));
    }
    return categoryCore()->linkContentItemWithCategoryItem(cats, contents);
}

/**
  \brief Save a PmhData pointer to database. If PmhData already exists in database, PmhData is updated.
  \sa updatePmhData()
*/
bool PmhBase::savePmhData(PmhData *pmh)
{
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_NAME);
    if (!connectDatabase(DB, __LINE__)) {
        return false;
    }
    // save or update ?
    if (!pmh->data(PmhData::Uid).isNull()) {
        return updatePmhData(pmh);
    }
    // save pmh
    DB.transaction();
    QSqlQuery query(DB);
    query.prepare(prepareInsertQuery(Constants::Table_MASTER));
    query.bindValue(Constants::MASTER_ID, QVariant());
    query.bindValue(Constants::MASTER_LABEL, pmh->data(PmhData::Label));
    query.bindValue(Constants::MASTER_TYPE, pmh->data(PmhData::Type));
    query.bindValue(Constants::MASTER_PATIENT_UID, pmh->data(PmhData::PatientUid));
    query.bindValue(Constants::MASTER_USER_UID, pmh->data(PmhData::UserOwner));
    query.bindValue(Constants::MASTER_STATE, pmh->data(PmhData::State));
    query.bindValue(Constants::MASTER_CATEGORY_ID, pmh->data(PmhData::CategoryId));
    query.bindValue(Constants::MASTER_CONFINDEX, pmh->data(PmhData::ConfidenceIndex));
    query.bindValue(Constants::MASTER_COMMENT, pmh->data(PmhData::Comment));
    query.bindValue(Constants::MASTER_CONTACTS_ID, QVariant());
    query.bindValue(Constants::MASTER_ISVALID, pmh->data(PmhData::IsValid).toInt());
    query.bindValue(Constants::MASTER_PRIVATE, pmh->data(PmhData::IsPrivate).toInt());
    query.bindValue(Constants::MASTER_EPISODE_ID, QVariant());
    if (query.exec()) {
        pmh->setData(PmhData::Uid, query.lastInsertId());
    } else {
        LOG_QUERY_ERROR(query);
        query.finish();
        DB.rollback();
    }

    foreach(PmhEpisodeData *ep, pmh->episodes()) {
        savePmhEpisodeData(ep);
    }

    query.finish();
    DB.commit();
    return true;
}

/**
  \brief Update a PmhData pointer to database. If PmhData does not already exist in database, PmhData is saved.
  \sa savePmhData()
*/
bool PmhBase::updatePmhData(PmhData *pmh)
{
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_NAME);
    if (!connectDatabase(DB, __LINE__)) {
        return false;
    }
    // save or update ?
    if (pmh->data(PmhData::Uid).isNull()) {
        return savePmhData(pmh);
    }
    // update pmh
    DB.transaction();
    QSqlQuery query(DB);
    QHash<int, QString> where;
    where.insert(Constants::MASTER_ID, QString("=%1").arg(pmh->data(PmhData::Uid).toString()));
    query.prepare(prepareUpdateQuery(Constants::Table_MASTER, QList<int>()
                                     << Constants::MASTER_LABEL
                                     << Constants::MASTER_TYPE
                                     << Constants::MASTER_STATE
                                     << Constants::MASTER_CATEGORY_ID
                                     << Constants::MASTER_CONFINDEX
                                     << Constants::MASTER_COMMENT
                                     << Constants::MASTER_CONTACTS_ID
                                     << Constants::MASTER_EPISODE_ID
                                     << Constants::MASTER_ISVALID
                                     << Constants::MASTER_PRIVATE, where));
    query.bindValue(0, pmh->data(PmhData::Label));
    query.bindValue(1, pmh->data(PmhData::Type));
    query.bindValue(2, pmh->data(PmhData::State));
    query.bindValue(3, pmh->data(PmhData::CategoryId));
    query.bindValue(4, pmh->data(PmhData::ConfidenceIndex));
    query.bindValue(5, pmh->data(PmhData::Comment));
    query.bindValue(6, QVariant());
    query.bindValue(7, QVariant());
    query.bindValue(8, pmh->data(PmhData::IsValid).toInt());
    query.bindValue(9, pmh->data(PmhData::IsPrivate).toInt());
    if (!query.exec()) {
        LOG_QUERY_ERROR(query);
        query.finish();
        DB.rollback();
        return false;
    }

    foreach(PmhEpisodeData *ep, pmh->episodes()) {
        updatePmhEpsisodeData(ep);
    }

    return true;
}

/**
  \brief Save a PmhEpisodeData pointer to database. If PmhEpisodeData already exists in database, PmhEpisodeData is updated.
  \sa updatePmhEpisodeData()
*/
bool PmhBase::savePmhEpisodeData(PmhEpisodeData *episode)
{
    // we are inside a transaction
    // save or update ?
    if (!episode->data(PmhEpisodeData::DbOnly_Id).isNull()) {
        return updatePmhEpsisodeData(episode);
    }
    // save episode
    QSqlQuery query(database());
    query.prepare(prepareInsertQuery(Constants::Table_EPISODE));
    query.bindValue(Constants::EPISODE_ID, QVariant());
    query.bindValue(Constants::EPISODE_MASTER_ID, episode->data(PmhEpisodeData::DbOnly_MasterId));
    query.bindValue(Constants::EPISODE_LABEL, episode->data(PmhEpisodeData::Label));
    query.bindValue(Constants::EPISODE_DATE_START, episode->data(PmhEpisodeData::DateStart));
    query.bindValue(Constants::EPISODE_DATE_END, episode->data(PmhEpisodeData::DateEnd));
    query.bindValue(Constants::EPISODE_CONF_INDEX, episode->data(PmhEpisodeData::ConfidenceIndex));
    query.bindValue(Constants::EPISODE_ICD_CODES, episode->data(PmhEpisodeData::IcdXml));
    query.bindValue(Constants::EPISODE_ISVALID, episode->data(PmhEpisodeData::DbOnly_IsValid).toInt());
    query.bindValue(Constants::EPISODE_COMMENT, episode->data(PmhEpisodeData::Comment));
    query.bindValue(Constants::EPISODE_TRACE_ID, QVariant());
    if (query.exec()) {
        episode->setData(PmhEpisodeData::DbOnly_Id, query.lastInsertId());
        return true;
    } else {
        LOG_QUERY_ERROR(query);
        return false;
    }
    return false;
}

/**
  \brief Update a PmhEpisodeData pointer to database. If PmhEpisodeData does not already exist in database, PmhEpisodeData is saved.
  \sa savePmhEpisodeData()
*/
bool PmhBase::updatePmhEpsisodeData(PmhEpisodeData *episode)
{
    // we are inside a transaction
    if (episode->data(PmhEpisodeData::DbOnly_Id).isNull()) {
        return savePmhEpisodeData(episode);
    }
    // update episode
    QSqlQuery query(database());
    QHash<int, QString> where;
    where.insert(Constants::EPISODE_ID, QString("=%1").arg(episode->data(PmhEpisodeData::DbOnly_Id).toString()));
    query.prepare(prepareUpdateQuery(Constants::Table_EPISODE, QList<int>()
                                     << Constants::EPISODE_DATE_START
                                     << Constants::EPISODE_DATE_END
                                     << Constants::EPISODE_LABEL
                                     << Constants::EPISODE_CONF_INDEX
                                     << Constants::EPISODE_COMMENT
                                     << Constants::EPISODE_ICD_CODES
                                     << Constants::EPISODE_ISVALID, where));
    query.bindValue(0, episode->data(PmhEpisodeData::DateStart));
    query.bindValue(1, episode->data(PmhEpisodeData::DateEnd));
    query.bindValue(2, episode->data(PmhEpisodeData::Label));
    query.bindValue(3, episode->data(PmhEpisodeData::ConfidenceIndex));
    query.bindValue(4, episode->data(PmhEpisodeData::Comment));
    query.bindValue(5, episode->data(PmhEpisodeData::IcdXml));
    query.bindValue(6, episode->data(PmhEpisodeData::DbOnly_IsValid).toInt());

    if (query.exec()) {
        return true;
    } else {
        LOG_QUERY_ERROR(query);
        return false;
    }
    return false;
}

/** \brief Save or update a Category::CategoryItem pointer to database. \sa Category::CategoryCore */
bool PmhBase::savePmhCategory(Category::CategoryItem *category)
{
    return categoryCore()->saveCategory(category);
}

void PmhBase::onCoreDatabaseServerChanged()
{
    m_initialized = false;
    if (QSqlDatabase::connectionNames().contains(Constants::DB_NAME)) {
        QSqlDatabase::removeDatabase(Constants::DB_NAME);
    }
    disconnect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
    disconnect(Core::ICore::instance(), SIGNAL(firstRunDatabaseCreation()), this, SLOT(onCoreFirstRunCreationRequested()));
    initialize();
}

void PmhBase::onCoreFirstRunCreationRequested()
{
    disconnect(Core::ICore::instance(), SIGNAL(firstRunDatabaseCreation()), this, SLOT(onCoreFirstRunCreationRequested()));
    initialize();
}
