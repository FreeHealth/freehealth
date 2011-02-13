/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
  \class PMH::PmhBase
  \brief Provides all read/write access to the PMHx database.
*/

#include "pmhbase.h"
#include "constants.h"
#include "pmhdata.h"

#include <utils/global.h>
#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/iuser.h>

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
static inline QString currentUserUuid() {return Core::ICore::instance()->user()->value(Core::IUser::Uuid).toString();}


PmhBase *PmhBase::m_Instance = 0;
bool PmhBase::m_initialized = false;

/** \brief Returns the singleton. There should be only one instance of the PmhBase class. */
PmhBase *PmhBase::instance()
{
    if (!m_Instance) {
        m_Instance = new PmhBase(qApp);
        m_Instance->init();
    }
    return m_Instance;
}



namespace PMH {
namespace Internal {
class PmhBasePrivate
{
public:
    PmhBasePrivate() {}
    ~PmhBasePrivate() {}

//    void checkPmhDataContent(PmhData *pmh)
//    {
//        // MASTER_ID defined ?
//        if (pmh->data(PmhData::Uid).isNull()) {
//            LOG_ERROR_FOR("PmhBase", "MasterId not defined for PMHx " + pmh->data(PmhData::Label).toString());
//        }

//        // PmhEpisodeData MasterId defined ?
//        foreach(PmhEpisodeData *ep, pmh->episodes()) {

//        }
//    }

public:

};
}
}

PmhBase::PmhBase(QObject *parent) :
        QObject(parent), Utils::Database(), d(new Internal::PmhBasePrivate)

{
    using namespace PMH::Constants;
    addTable(Table_MASTER,        "PMH_MASTER");
    addTable(Table_EPISODE,       "PMH_EPISODE");
    addTable(Table_ICD,           "PMH_ICD_CODING");
    addTable(Table_CATEGORIES,    "PMH_CATEGORIES");
    addTable(Table_CATEGORY_LABEL,"PMH_CAT_LABEL");

//    addTable(Table_VERSION, "VERSION");

    addField(Table_MASTER, MASTER_ID,            "ID",             FieldIsUniquePrimaryKey);
    addField(Table_MASTER, MASTER_PATIENT_UID,   "PATIENT_UUID",   FieldIsUUID);
    addField(Table_MASTER, MASTER_USER_UID,      "USER_UUID",      FieldIsUUID);
    addField(Table_MASTER, MASTER_CATEGORY_ID,   "CATEGORY_ID",    FieldIsInteger);
    addField(Table_MASTER, MASTER_EPISODE_ID,    "MH_EPISODE_ID",  FieldIsInteger);
    addField(Table_MASTER, MASTER_CONTACTS_ID,   "MH_CONTACTS_ID", FieldIsInteger);
    addField(Table_MASTER, MASTER_LABEL,         "LABEL",          FieldIsShortText);
    addField(Table_MASTER, MASTER_TYPE,          "TYPE_ID",        FieldIsInteger);
    addField(Table_MASTER, MASTER_STATE,         "STATE_ID",       FieldIsInteger);
    addField(Table_MASTER, MASTER_CONFINDEX,     "GLOBAL_CONF_INDEX",FieldIsInteger);
    addField(Table_MASTER, MASTER_COMMENT,       "COMMENT",        FieldIsLongText);

    addField(Table_EPISODE, EPISODE_ID,            "ID",             FieldIsUniquePrimaryKey);
    addField(Table_EPISODE, EPISODE_MASTER_ID,     "MASTER_ID",      FieldIsInteger);
    addField(Table_EPISODE, EPISODE_LABEL,         "LABEL",          FieldIsShortText);
    addField(Table_EPISODE, EPISODE_DATE_START,    "DATE_START",     FieldIsDate);
    addField(Table_EPISODE, EPISODE_DATE_END,      "DATE_END",       FieldIsDate);
    addField(Table_EPISODE, EPISODE_CONF_INDEX,    "CONF_INDEX",     FieldIsInteger);
    addField(Table_EPISODE, EPISODE_ICD_CODES,     "XML_ICD",        FieldIsLongText);
    addField(Table_EPISODE, EPISODE_COMMENT,       "COMMENT",        FieldIsLongText);
    addField(Table_EPISODE, EPISODE_TRACE_ID,      "TRACE_ID",       FieldIsInteger);

    addField(Table_CATEGORIES, CATEGORY_ID,              "ID",         FieldIsUniquePrimaryKey);
    addField(Table_CATEGORIES, CATEGORY_PARENT,          "PARENT_ID",  FieldIsInteger);
    addField(Table_CATEGORIES, CATEGORY_LABEL_ID,        "LABEL_ID",   FieldIsInteger);
    addField(Table_CATEGORIES, CATEGORY_ISRISKFACTOR,    "IS_RF",      FieldIsBoolean);
    addField(Table_CATEGORIES, CATEGORY_ISCHONICDISEASE, "IS_CD",      FieldIsBoolean);
    addField(Table_CATEGORIES, CATEGORY_SORT_ID,         "SHORT_ID",   FieldIsInteger);
    addField(Table_CATEGORIES, CATEGORY_THEMEDICON,      "THEMED_ICON",FieldIsShortText);

    addField(Table_CATEGORY_LABEL, CATEGORYLABEL_ID,       "ID",       FieldIsUniquePrimaryKey);
    addField(Table_CATEGORY_LABEL, CATEGORYLABEL_LABEL_ID, "LID",      FieldIsInteger);
    addField(Table_CATEGORY_LABEL, CATEGORYLABEL_LANG,     "LANG",     FieldIsLanguageText);
    addField(Table_CATEGORY_LABEL, CATEGORYLABEL_VALUE,    "VALUE",    FieldIsShortText);

    connect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
}

PmhBase::~PmhBase()
{
    if (d)
        delete d;
    d = 0;
}

bool PmhBase::init()
{
    // only one base can be initialized
    if (m_initialized)
        return true;

    // Check settings --> SQLite or MySQL ?
    if (settings()->value(Core::Constants::S_USE_EXTERNAL_DATABASE, false).toBool()) {
        createConnection(Constants::DB_NAME,
                         Constants::DB_NAME,
                         QString(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_HOST, QByteArray("localhost").toBase64()).toByteArray())),
                         Utils::Database::ReadWrite,
                         Utils::Database::MySQL,
                         QString(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_LOG, QByteArray("root").toBase64()).toByteArray())),
                         QString(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_PASS, QByteArray("").toBase64()).toByteArray())),
                         QString(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_PORT, QByteArray("").toBase64()).toByteArray())).toInt(),
                         Utils::Database::CreateDatabase);
    } else {
        createConnection(Constants::DB_NAME,
                         Constants::DB_FILENAME,
                         settings()->path(Core::ISettings::ReadWriteDatabasesPath) + QDir::separator() + QString(Constants::DB_NAME),
                         Utils::Database::ReadWrite,
                         Utils::Database::SQLite,
                         "log", "pas", 0,
                         Utils::Database::CreateDatabase);
    }
//    checkDatabaseVersion();

    m_initialized = true;
    return true;
}

bool PmhBase::createDatabase(const QString &connectionName , const QString &dbName,
                    const QString &pathOrHostName,
                    TypeOfAccess access, AvailableDrivers driver,
                    const QString & login, const QString & pass,
                    const int port,
                    CreationOption /*createOption*/
                   )
{
    if (connectionName != Constants::DB_NAME)
        return false;

    Utils::Log::addMessage(this, tkTr(Trans::Constants::TRYING_TO_CREATE_1_PLACE_2)
                           .arg(dbName).arg(pathOrHostName));

    // create an empty database and connect
    QSqlDatabase DB;
    if (driver == SQLite) {
        DB = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        if (!QDir(pathOrHostName).exists())
            if (!QDir().mkpath(pathOrHostName))
                tkTr(Trans::Constants::_1_ISNOT_AVAILABLE_CANNOTBE_CREATED).arg(pathOrHostName);
        DB.setDatabaseName(QDir::cleanPath(pathOrHostName + QDir::separator() + dbName));
        DB.open();
        setDriver(Utils::Database::SQLite);
    }
    else if (driver == MySQL) {
        DB = QSqlDatabase::database(connectionName);
        if (!DB.open()) {
            QSqlDatabase d = QSqlDatabase::addDatabase("QMYSQL", "CREATOR");
            d.setHostName(pathOrHostName);
            d.setUserName(login);
            d.setPassword(pass);
            d.setPort(port);
            if (!d.open()) {
                Utils::warningMessageBox(tr("Unable to connect the Templates host."),tr("Please contact dev team."));
                return false;
            }
            QSqlQuery q(QString("CREATE DATABASE `%1`").arg(dbName), d);
            if (!q.isActive()) {
                Utils::Log::addQueryError("Database", q);
                Utils::warningMessageBox(tr("Unable to create the Templates database."),tr("Please contact dev team."));
                return false;
            }
            if (!DB.open()) {
                Utils::warningMessageBox(tr("Unable to connect the Templates database."),tr("Please contact dev team."));
                return false;
            }
            DB.setDatabaseName(dbName);
        }
        if (QSqlDatabase::connectionNames().contains("CREATOR"))
            QSqlDatabase::removeDatabase("CREATOR");
        DB.open();
        setDriver(Utils::Database::MySQL);
    }

    // create db structure
    // before we need to inform Utils::Database of the connectionName to use
    setConnectionName(connectionName);

    if (createTables()) {
        Utils::Log::addMessage(this, tkTr(Trans::Constants::DATABASE_1_CORRECTLY_CREATED).arg(dbName));
    } else {
        Utils::Log::addError(this, tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2)
                         .arg(dbName, DB.lastError().text()));
        return false;
    }

    return true;
}

/** \brief Retreive all PMHx related to a patient. */
QVector<PmhData *> PmhBase::getPmh(const QString &patientUid) const
{
    QVector<PmhData *> pmhs;
    if (!database().isOpen()) {
        if (!database().open()) {
            Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                 .arg(database().connectionName()).arg(database().lastError().text()), __FILE__, __LINE__);
            return pmhs;
        }
    }
    QString req;
    QSqlQuery query(database());
    QHash<int, QString> where;

    // Get Master table
    where.insert(Constants::MASTER_PATIENT_UID,
                 QString("='%1'").arg(patient()->data(Core::IPatient::Uid).toString()));
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
            pmh->setData(PmhData::DbOnly_MasterEpisodeId, query.value(Constants::MASTER_EPISODE_ID));
            pmh->setData(PmhData::DbOnly_MasterContactId, query.value(Constants::MASTER_CONTACTS_ID));
            pmhs << pmh;
        }
    } else {
        Utils::Log::addQueryError(this, query, __FILE__, __LINE__);
    }
    query.finish();

    // Get Episodes
    where.clear();
    for(int i = 0; i < pmhs.count(); ++i) {
        PmhData *pmh = pmhs.at(i);
        where.insert(Constants::EPISODE_MASTER_ID, QString("='%1'").arg(pmh->data(PmhData::Uid).toString()));
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
            Utils::Log::addQueryError(this, query, __FILE__, __LINE__);
        }
    }

    // Get Contacts
    where.clear();

    return pmhs;
}

/** \brief Return flatten list of PmhCategory extracted from database. */
QVector<PmhCategory *> PmhBase::getPmhCategory(const QString &patientUid) const
{
    QVector<PmhCategory *> cats;
    if (!database().isOpen()) {
        if (!database().open()) {
            Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                 .arg(database().connectionName()).arg(database().lastError().text()), __FILE__, __LINE__);
            return cats;
        }
    }
    QString req;
    QSqlQuery query(database());
    QHash<int, QString> where;

    // Get Category table
    req = select(Constants::Table_CATEGORIES);
    if (query.exec(req)) {
        while (query.next()) {
            PmhCategory *cat = new PmhCategory;
            cat->setData(PmhCategory::DbOnly_Id, query.value(Constants::CATEGORY_ID));
            cat->setData(PmhCategory::DbOnly_LabelId, query.value(Constants::CATEGORY_LABEL_ID));
            cat->setData(PmhCategory::DbOnly_ParentId, query.value(Constants::CATEGORY_PARENT));
            cat->setData(PmhCategory::ThemedIcon, query.value(Constants::CATEGORY_THEMEDICON));
            cat->setData(PmhCategory::SortId, query.value(Constants::CATEGORY_SORT_ID));
            cats << cat;
        }
    } else {
        LOG_QUERY_ERROR(query);
    }
    query.finish();

    // Get Category labels
    for(int i = 0; i < cats.count(); ++i) {
        where.clear();
        PmhCategory *cat = cats.at(i);
        where.insert(Constants::CATEGORYLABEL_LABEL_ID, QString("='%1'").arg(cat->data(PmhCategory::DbOnly_LabelId).toInt()));
        req = select(Constants::Table_CATEGORY_LABEL, where);
        if (query.exec(req)) {
            while (query.next()) {
                cat->setLabel(query.value(Constants::CATEGORYLABEL_VALUE).toString(),
                              query.value(Constants::CATEGORYLABEL_LANG).toString());
            }
        } else {
            LOG_QUERY_ERROR(query);
        }
        query.finish();
    }
    return cats;
}

/** \brief Recreate the category tree and return a QList of root categories. */
QList<PmhCategory *> PmhBase::createCategoryTree(const QVector<PmhCategory *> &cats) const
{
    QList<PmhCategory *> toReturn;
    // Reparent categories
    for(int i = 0; i < cats.count(); ++i) {
        PmhCategory *cat = cats.at(i);
        int id = cat->id();
        // Find all its children
        for(int j = 0; j < cats.count(); ++j) {
            PmhCategory *child = cats.at(j);
            if (child->parentId() == id) {
                qWarning() << "reparent" << child->label() << "as child of" << cat->label();
                cat->addChild(child);
                child->setParent(cat);
            }
        }
        // Find roots categories
        if (cat->parentId() < 0) {
            toReturn << cat;
        }
        cat->sortChildren();
    }

    // Sort root items
    qSort(toReturn.begin(), toReturn.end(), PmhCategory::lessThan);

    return toReturn;
}

/** \brief Link PMH with their category. createCateogryTree() must be called first. */
bool PmhBase::linkPmhWithCategory(const QVector<PmhCategory *> &cats, const QVector<PmhData *> &pmhs) const
{
    for(int i = 0; i < pmhs.count(); ++i) {
        // PMHx has a category ID ?
        if (pmhs.at(i)->data(PmhData::CategoryId).isNull())
            continue;
        int id = pmhs.at(i)->data(PmhData::CategoryId).toInt();
        if (id==-1)
            continue;

        // Add PMHx to the category
        for(int j = 0; j < cats.count(); ++j) {
            if (cats.at(j)->id() == id) {
                cats.at(j)->addPhmData(pmhs.at(i));
                break;
            }
        }
    }
    return true;
}

/**
  \brief Save a PmhData pointer to database. If PmhData already exists in database, PmhData is updated.
  \sa updatePmhData()
*/
bool PmhBase::savePmhData(PmhData *pmh)
{
    // save or update ?
    if (!pmh->data(PmhData::Uid).isNull()) {
        return updatePmhData(pmh);
    }
    // save pmh
    QSqlQuery query(database());
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
    query.bindValue(Constants::MASTER_EPISODE_ID, QVariant());
    if (query.exec()) {
        pmh->setData(PmhData::Uid, query.lastInsertId());
    } else {
        LOG_QUERY_ERROR(query);
    }

    foreach(PmhEpisodeData *ep, pmh->episodes()) {
        savePmhEpisodeData(ep);
    }

    return false;
}

/**
  \brief Update a PmhData pointer to database. If PmhData does not already exist in database, PmhData is saved.
  \sa savePmhData()
*/
bool PmhBase::updatePmhData(PmhData *pmh)
{
    // save or update ?
    if (pmh->data(PmhData::Uid).isNull()) {
        return savePmhData(pmh);
    }
    // update pmh
    QSqlQuery query(database());
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
                                     << Constants::MASTER_EPISODE_ID, where));
    query.bindValue(0, pmh->data(PmhData::Label));
    query.bindValue(1, pmh->data(PmhData::Type));
    query.bindValue(2, pmh->data(PmhData::State));
    query.bindValue(3, pmh->data(PmhData::CategoryId));
    query.bindValue(4, pmh->data(PmhData::ConfidenceIndex));
    query.bindValue(5, pmh->data(PmhData::Comment));
    query.bindValue(6, QVariant());
    query.bindValue(7, QVariant());
    if (!query.exec()) {
        LOG_QUERY_ERROR(query);
    }

    foreach(PmhEpisodeData *ep, pmh->episodes()) {
        updatePmhEpsisodeData(ep);
    }

    return false;
}

/**
  \brief Save a PmhEpisodeData pointer to database. If PmhEpisodeData already exists in database, PmhEpisodeData is updated.
  \sa updatePmhEpisodeData()
*/
bool PmhBase::savePmhEpisodeData(PmhEpisodeData *episode)
{
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
                                     << Constants::EPISODE_ICD_CODES, where));
    query.bindValue(0, episode->data(PmhEpisodeData::DateStart));
    query.bindValue(1, episode->data(PmhEpisodeData::DateEnd));
    query.bindValue(2, episode->data(PmhEpisodeData::Label));
    query.bindValue(3, episode->data(PmhEpisodeData::ConfidenceIndex));
    query.bindValue(4, episode->data(PmhEpisodeData::Comment));
    query.bindValue(5, episode->data(PmhEpisodeData::IcdXml));
    if (query.exec()) {
        return true;
    } else {
        LOG_QUERY_ERROR(query);
        return false;
    }
    return false;
}

/**
  \brief Save a PmhCategory pointer to database. If PmhCategory already exists in database, PmhCategory is updated.
  \sa updatePmhCategory()
*/
bool PmhBase::savePmhCategory(PmhCategory *category)
{
    // save or update ?
    if (!category->data(PmhCategory::DbOnly_Id).isNull()) {
//        return updatePmhCategory(category);
    }
    // save labels
    if (!savePmhCategoryLabels(category))
        return false;
    // save category itself
    QSqlQuery query(database());
    query.prepare(prepareInsertQuery(Constants::Table_CATEGORIES));
    query.bindValue(Constants::CATEGORY_ID, QVariant());
    query.bindValue(Constants::CATEGORY_PARENT, category->parentId());
    query.bindValue(Constants::CATEGORY_SORT_ID, category->sortId());
    query.bindValue(Constants::CATEGORY_THEMEDICON, category->data(PmhCategory::ThemedIcon));
    query.bindValue(Constants::CATEGORY_ISCHONICDISEASE, QVariant());
    query.bindValue(Constants::CATEGORY_ISRISKFACTOR, QVariant());
    query.bindValue(Constants::CATEGORY_LABEL_ID, category->data(PmhCategory::DbOnly_LabelId));
    if (query.exec()) {
        category->setData(PmhEpisodeData::DbOnly_Id, query.lastInsertId());
        return true;
    } else {
        LOG_QUERY_ERROR(query);
        return false;
    }
    return false;
}

/** \brief Save or update categories labels. */
bool PmhBase::savePmhCategoryLabels(PmhCategory *category)
{
    // save or update ?
//    if (!category->data(PmhCategory::DbOnly_LabelId).isNull()) {
//        return updatePmhCategory(episode);
//    }
    QSqlQuery query(database());
    // get label_id
    int labelId = -1;
    if (category->data(PmhCategory::DbOnly_LabelId).isNull()) {
        labelId = max(Constants::Table_CATEGORY_LABEL, Constants::CATEGORY_LABEL_ID);
        ++labelId;
        category->setData(PmhCategory::DbOnly_LabelId, labelId);
    } else {
        labelId = category->data(PmhCategory::DbOnly_LabelId).toInt();
    }
    // save labels
    foreach(const QString &lang, category->allLanguagesForLabel()) {
        query.prepare(prepareInsertQuery(Constants::Table_CATEGORY_LABEL));
        query.bindValue(Constants::CATEGORYLABEL_ID, QVariant());
        query.bindValue(Constants::CATEGORYLABEL_LABEL_ID, labelId);
        query.bindValue(Constants::CATEGORYLABEL_LANG, lang);
        query.bindValue(Constants::CATEGORYLABEL_VALUE, category->label(lang));
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            return false;
        }
        query.finish();
    }
    return true;
}

void PmhBase::onCoreDatabaseServerChanged()
{
//    m_initialized = false;
//    if (QSqlDatabase::connectionNames().contains(Templates::Constants::DB_TEMPLATES_NAME)) {
//        QSqlDatabase::removeDatabase(Templates::Constants::DB_TEMPLATES_NAME);
//    }
//    init();
}

