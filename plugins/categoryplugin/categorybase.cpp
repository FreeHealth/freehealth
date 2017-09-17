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
 ***************************************************************************/
#include "categorybase.h"
#include "categoryitem.h"
#include "constants.h"

#include <utils/global.h>
#include <utils/log.h>
#include <utils/databaseconnector.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>
#include <translationutils/trans_database.h>
#include <translationutils/trans_msgerror.h>

#include <coreplugin/isettings.h>
#include <coreplugin/icore.h>
#include <coreplugin/constants.h>
#include <coreplugin/icommandline.h>

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QDir>
#include <QVector>

using namespace Category;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ICommandLine *commandLine()  { return Core::ICore::instance()->commandLine(); }

CategoryBase *CategoryBase::m_Instance = 0;

static inline bool connectDatabase(QSqlDatabase &DB, const int line)
{
    if (!DB.isOpen()) {
        if (!DB.open()) {
            Utils::Log::addError("CategoryBase", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                 .arg(DB.connectionName()).arg(DB.lastError().text()),
                                 __FILE__, line);
            return false;
        }
    }
    return true;
}

CategoryBase *CategoryBase::instance()
{
    if (!m_Instance) {
        m_Instance = new CategoryBase(qApp);
    }
    return m_Instance;
}

CategoryBase::CategoryBase(QObject *parent) :
    QObject(parent), Utils::Database(),
    m_initialized(false)
{
    setObjectName("CategoryBase");
    using namespace Category::Constants;
    addTable(Table_CATEGORIES, "CATEGORIES");
    addTable(Table_CATEGORY_LABEL, "LABEL");
    addTable(Table_PROTECTION, "PROTECTION");
    addTable(Table_SCHEMA, "SCHEMA_CHANGES");

    addField(Table_CATEGORIES, CATEGORY_ID,              "CID",        FieldIsUniquePrimaryKey);
    addField(Table_CATEGORIES, CATEGORY_UUID,            "UUID",       FieldIsShortText);
    addField(Table_CATEGORIES, CATEGORY_PARENT,          "PARENT_ID",  FieldIsInteger);
    addField(Table_CATEGORIES, CATEGORY_LABEL_ID,        "LID",        FieldIsInteger);
    addField(Table_CATEGORIES, CATEGORY_MIME,            "MIME",       FieldIsShortText);
    addField(Table_CATEGORIES, CATEGORY_PROTECTION_ID,   "PID",        FieldIsInteger);
    addField(Table_CATEGORIES, CATEGORY_SORT_ID,         "SORT_ID",    FieldIsInteger);
    addField(Table_CATEGORIES, CATEGORY_PASSWORD,        "PASSWORD",   FieldIsShortText);
    addField(Table_CATEGORIES, CATEGORY_ISVALID,         "VALID",      FieldIsBoolean, "1");
    addField(Table_CATEGORIES, CATEGORY_THEMEDICON,      "THEMED_ICON",FieldIsShortText);
    addField(Table_CATEGORIES, CATEGORY_EXTRAXML,        "EXTRA",      FieldIsLongText);
    addIndex(Table_CATEGORIES, CATEGORY_ID);
    addIndex(Table_CATEGORIES, CATEGORY_UUID, 190);
    addIndex(Table_CATEGORIES, CATEGORY_PARENT);
    addIndex(Table_CATEGORIES, CATEGORY_LABEL_ID);
    addIndex(Table_CATEGORIES, CATEGORY_MIME, 190);
    addIndex(Table_CATEGORIES, CATEGORY_PROTECTION_ID);
    addIndex(Table_CATEGORIES, CATEGORY_SORT_ID);

    addField(Table_CATEGORY_LABEL, CATEGORYLABEL_ID,       "ID",       FieldIsUniquePrimaryKey);
    addField(Table_CATEGORY_LABEL, CATEGORYLABEL_LABEL_ID, "LID",      FieldIsInteger);
    addField(Table_CATEGORY_LABEL, CATEGORYLABEL_LANG,     "LANG",     FieldIsLanguageText);
    addField(Table_CATEGORY_LABEL, CATEGORYLABEL_VALUE,    "VALUE",    FieldIsShortText);
    addField(Table_CATEGORY_LABEL, CATEGORYLABEL_ISVALID,  "VALID",    FieldIsBoolean);
    addIndex(Table_CATEGORY_LABEL, CATEGORYLABEL_ID);
    addIndex(Table_CATEGORY_LABEL, CATEGORYLABEL_LABEL_ID);
    addIndex(Table_CATEGORY_LABEL, CATEGORYLABEL_LANG);

    addField(Table_PROTECTION, PROTECTION_ID,                "ID",     FieldIsUniquePrimaryKey);
    addField(Table_PROTECTION, PROTECTION_PID,               "PID",    FieldIsInteger);
    addField(Table_PROTECTION, PROTECTION_RESTRICTEDTOUSER,  "USER_RESTRICTED", FieldIsUUID);
    addField(Table_PROTECTION, PROTECTION_RESTRICTEDTOGROUP, "GROUP_RESTRICTED",FieldIsUUID);
    addIndex(Table_PROTECTION, PROTECTION_ID);
    addIndex(Table_PROTECTION, PROTECTION_PID);

    // fields for table SCHEMA_CHANGES
    addField(Table_SCHEMA, SCHEMA_ID, "ID", FieldIsUniquePrimaryKey);
    addField(Table_SCHEMA, SCHEMA_VERSION, "VERSION_NUMBER", FieldIsInteger);
    addField(Table_SCHEMA, SCHEMA_SCRIPT, "SCRIPT_NAME", FieldIs255Chars);
    addField(Table_SCHEMA, SCHEMA_TIMESTAMP, "TIMESTAMP_UTC_APPLIED", FieldIs19Chars);
    addIndex(Table_SCHEMA, SCHEMA_ID);


    // Connect first run database creation requested
    connect(Core::ICore::instance(), SIGNAL(firstRunDatabaseCreation()), this, SLOT(onCoreFirstRunCreationRequested()));
}

CategoryBase::~CategoryBase()
{
}

bool CategoryBase::initialize()
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
            LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().databaseName()).arg(database().driverName()));
        }
    } else {
        LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().databaseName()).arg(database().driverName()));
    }

    // compare database version with the version of the code, if not equal, update
    int currentDatabaseVersion = getSchemaVersionNumber(Constants::DB_NAME);
    if (currentDatabaseVersion != Constants::DB_CURRENT_CODE_VERSION) {
        if(!updateDatabase()) {
            LOG_ERROR(QString("Couldn't upgrade database %1").arg(Constants::DB_NAME));
            return false;
        }
        initialize();
    }

    if (!checkDatabaseScheme()) {
        LOG_ERROR(tkTr(Trans::Constants::DATABASE_1_SCHEMA_ERROR).arg(Constants::DB_NAME));
        return false;
    }

    connect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
    m_initialized = true;
    return true;
}

bool CategoryBase::createDatabase(const QString &connectionName , const QString &dbName,
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
            QSqlDatabase d = QSqlDatabase::addDatabase("QMYSQL", "__CATEGORY_CREATOR");
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
        if (QSqlDatabase::connectionNames().contains("__CATEGORY_CREATOR"))
            QSqlDatabase::removeDatabase("__CATEGORY_CREATOR");
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

    // Add version number
    if (!setSchemaVersion(Constants::DB_CURRENT_CODE_VERSION, Constants::DB_NAME)) {
        LOG_ERROR(QString("Couldn't set schema version for database %1").arg(Constants::DB_NAME));
        return false;
    }

    return true;
}

/**
 * Update category database
 * Old versioning (fhio version <= 0.10): version string = "0.1"
 * New versioning (fhio version >= 0.11.0): The version number is an integer,
 * starting from 1 for fhio version 0.11.0
 * The field VERSION_NUMBER of the last row of the table SCHEMA_CHANGES.
 * By definition, this number must be a positive, non null integer.
 * This function will run all sql update scripts to update the database to
 * the current code version
 */
bool CategoryBase::updateDatabase()
{
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_NAME);
    QString updateScriptFileName;
    for (int i = 1; i <= Constants::DB_CURRENT_CODE_VERSION; i++) {
        if (driver()==MySQL) {
            updateScriptFileName= QString(":/sql/update/update%1%2.sql")
                    .arg(Constants::DB_NAME)
                    .arg(QString::number(i));
        } else if (driver()==SQLite) {
            updateScriptFileName= QString(":/sql/update/updatesqlite%1%2.sql")
                    .arg(Constants::DB_NAME)
                    .arg(QString::number(i));
        }
        QFile updateScriptFile(updateScriptFileName);
        if(!executeQueryFile(updateScriptFile, DB)) {
            LOG_ERROR(QString("Error during update of database %1 with update script %2").arg(Constants::DB_NAME).arg(updateScriptFileName));
            return false;
        }
    }
    return true;
}

/** Return a list of CategoryItem tree extracted from database. */
QVector<CategoryItem *> CategoryBase::getCategories(const QString &mime, const QStringList &uuids) const
{
    QVector<CategoryItem *> cats;
    QMultiHash<CategoryItem *, QString> childrenUuid;
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_NAME);
    if (!connectDatabase(DB, __LINE__))
        return cats;

    QString req;
    DB.transaction();
    QSqlQuery query(DB);
    QStringList correctedUids = uuids;
    QHash<int, QString> where;
    if (uuids.isEmpty()) {
        // get root uuids
        QHash<int, QString> where;
        where.insert(Constants::CATEGORY_ISVALID, "=1");
        where.insert(Constants::CATEGORY_PARENT, "=-1");
        where.insert(Constants::CATEGORY_MIME, QString("='%1'").arg(mime));
        req = select(Constants::Table_CATEGORIES, Constants::CATEGORY_UUID, where);
        if (query.exec(req)) {
            while (query.next()) {
                correctedUids << query.value(0).toString();
            }
        } else {
            LOG_QUERY_ERROR(query);
        }
        query.finish();
    }

    foreach(const QString &uuid, correctedUids) {
        where.clear();
        where.insert(Constants::CATEGORY_ISVALID, "=1");
        where.insert(Constants::CATEGORY_MIME, QString("='%1'").arg(mime));
        if (uuid.contains("%"))
            where.insert(Constants::CATEGORY_UUID, QString("like '%1'").arg(uuid));
        else
            where.insert(Constants::CATEGORY_UUID, QString("='%1'").arg(uuid));

        // Get Category table
        req = select(Constants::Table_CATEGORIES, where);
        if (query.exec(req)) {
            while (query.next()) {
                CategoryItem *cat = new CategoryItem;
                cat->setData(CategoryItem::DbOnly_Id, query.value(Constants::CATEGORY_ID));
                cat->setData(CategoryItem::DbOnly_LabelId, query.value(Constants::CATEGORY_LABEL_ID));
                cat->setData(CategoryItem::DbOnly_ParentId, query.value(Constants::CATEGORY_PARENT));
                cat->setData(CategoryItem::DbOnly_Mime, query.value(Constants::CATEGORY_MIME));
                cat->setData(CategoryItem::Uuid, query.value(Constants::CATEGORY_UUID));
                cat->setData(CategoryItem::ThemedIcon, query.value(Constants::CATEGORY_THEMEDICON));
                cat->setData(CategoryItem::Password, query.value(Constants::CATEGORY_PASSWORD));
                cat->setData(CategoryItem::SortId, query.value(Constants::CATEGORY_SORT_ID));
                cat->setData(CategoryItem::ExtraXml, query.value(Constants::CATEGORY_EXTRAXML));
                cats << cat;
            }
        } else {
            LOG_QUERY_ERROR(query);
        }
        query.finish();

        // Get CategoryItem labels
        for(int i = 0; i < cats.count(); ++i) {
            where.clear();
            CategoryItem *cat = cats.at(i);
            where.insert(Constants::CATEGORYLABEL_LABEL_ID, QString("='%1'").arg(cat->data(CategoryItem::DbOnly_LabelId).toInt()));
            req = select(Constants::Table_CATEGORY_LABEL, where);
            query.prepare(req);
            if (query.exec()) {
                while (query.next()) {
                    cat->setLabel(QString::fromUtf8(query.value(Constants::CATEGORYLABEL_VALUE).toByteArray()),
                                  query.value(Constants::CATEGORYLABEL_LANG).toString());
                }
            } else {
                LOG_QUERY_ERROR(query);
            }
            query.finish();
        }
        //    qWarning() << "getting categories" << mime << cats.count();

    }

    for(int i = 0; i < cats.count(); ++i) {
        // Get Category children
        CategoryItem *cat = cats.at(i);
        where.clear();
        where.insert(Constants::CATEGORY_ISVALID, "=1");
        where.insert(Constants::CATEGORY_PARENT, QString("='%1'").arg(cat->id()));
        req = select(Constants::Table_CATEGORIES, Constants::CATEGORY_UUID, where);
        if (query.exec(req)) {
            while (query.next()) {
                childrenUuid.insertMulti(cat, query.value(0).toString());
            }
        } else {
            LOG_QUERY_ERROR(query);
        }
        query.finish();
    }
    DB.commit();

    // Get the children of the category
    foreach(CategoryItem *item, cats) {
        const QStringList &uids = childrenUuid.values(item);
        if (uids.isEmpty())
            continue;
        item->addChildren(getCategories(mime, uids));
    }

    // Sort items
    qSort(cats.begin(), cats.end(), CategoryItem::lessThan);

    return cats;
}

/** \brief Recreate the category tree and return a QList of root categories. */
QList<CategoryItem *> CategoryBase::createCategoryTree(const QVector<CategoryItem *> &cats) const
{
    QList<CategoryItem *> toReturn;
    // Reparent categories
    for(int i = 0; i < cats.count(); ++i) {
        CategoryItem *parent = cats.at(i);
        int id = parent->id();
        //        qWarning() << "finding all children of" << id << parent->label();
        // Find all its children
        for(int j = 0; j < cats.count(); ++j) {
            CategoryItem *child = cats.at(j);
            //            qWarning() << "  testing child" << child->label()<< "parent" << child->parentId() << "actual" << id;
            if (child->parentId() == id) {
                if (parent->children().contains(child))
                    continue;
                //                qWarning() << "    reparent" << child->label() << "as child of" << parent->label();
                parent->addChild(child);
                //                child->setParent(parent);
            }
        }
        // Find roots categories
        if (parent->parentId() < 0) {
            toReturn << parent;
        }
        parent->sortChildren();
    }

    // Sort root items
    qSort(toReturn.begin(), toReturn.end(), CategoryItem::lessThan);

    return toReturn;
}

/**
  \brief Save a CategoryItem pointer to database. If CategoryItem already exists in database, CategoryItem is updated.
  \sa updateCategory()
*/
bool CategoryBase::saveCategory(CategoryItem *category)
{
    return saveCategories(QVector<CategoryItem *>() << category);
}

/**
  Save a QVector of CategoryItem pointers to database.
  If CategoryItem already exists in database, CategoryItem is updated.
  \sa updateCategory()
*/
bool CategoryBase::saveCategories(const QVector<CategoryItem *> &categories, bool createTransaction)
{
    //    QTime chr;
    //    chr.start();
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_NAME);
    if (!connectDatabase(DB, __LINE__)) {
        return false;
    }
    if (createTransaction)
        DB.transaction();

    QSqlQuery query(DB);

    //    if (createTransaction)
    //        Utils::Log::logTimeElapsed(chr, "---", "create transaction");
    for(int i=0; i < categories.count(); ++i) {
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        CategoryItem *category = categories.at(i);

        // save or update ?
        if (categoryNeedsUpdate(category)) {
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
            //            Utils::Log::logTimeElapsed(chr, "---", "needs update");
            if (!updateCategory(category)) {
                if (createTransaction)
                    DB.rollback();
                return false;
            }
            //            Utils::Log::logTimeElapsed(chr, "---", "update");
            continue;
        }

        // save labels
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        if (!saveCategoryLabels(category)) {
            if (createTransaction)
                DB.rollback();
            return false;
        }
        //        Utils::Log::logTimeElapsed(chr, "---", "save labels");

        // save category itself
        query.prepare(prepareInsertQuery(Constants::Table_CATEGORIES));
        query.bindValue(Constants::CATEGORY_ID, QVariant());
        query.bindValue(Constants::CATEGORY_UUID, category->data(CategoryItem::Uuid));
        query.bindValue(Constants::CATEGORY_PARENT, category->parentId());
        query.bindValue(Constants::CATEGORY_LABEL_ID, category->data(CategoryItem::DbOnly_LabelId));
        query.bindValue(Constants::CATEGORY_MIME, category->mime());
        query.bindValue(Constants::CATEGORY_PROTECTION_ID, QVariant());
        query.bindValue(Constants::CATEGORY_SORT_ID, category->sortId());
        query.bindValue(Constants::CATEGORY_PASSWORD, category->cryptedPassword());
        query.bindValue(Constants::CATEGORY_ISVALID, category->data(CategoryItem::DbOnly_IsValid).toInt());
        query.bindValue(Constants::CATEGORY_THEMEDICON, category->data(CategoryItem::ThemedIcon));
        query.bindValue(Constants::CATEGORY_EXTRAXML, category->data(CategoryItem::ExtraXml));
        if (query.exec()) {
            category->setData(CategoryItem::DbOnly_Id, query.lastInsertId());
        } else {
            LOG_QUERY_ERROR(query);
            query.finish();
            if (createTransaction) {
                DB.rollback();
            }
            return false;
        }
        query.finish();
        category->setDirty(false);
        //        Utils::Log::logTimeElapsed(chr, "---", "save");

        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        for(int i=0; i < category->childCount(); ++i)
            category->child(i)->setData(CategoryItem::DbOnly_ParentId, category->id());

        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        if (!saveCategories(category->children().toVector(), false)) {
            if (createTransaction)
                DB.rollback();
            return false;
        }
    }
    //    Utils::Log::logTimeElapsed(chr, "---", "commit");

    if (createTransaction)
        DB.commit();
    return true;
}

/** Check the database from already existing category. Return true if the category already exists in the database (in this case, the CategoryItem::DbOnly_Id will be populated with the id of the item). */
bool CategoryBase::categoryNeedsUpdate(CategoryItem *category)
{
    // we are inside a transaction
    int id = -1;
    bool isDirty = category->isDirty();
    if ((category->data(CategoryItem::DbOnly_Id).isNull() || category->id()==-1) &&
            (!category->uuid().isEmpty())) {
        // try to find the category using the uuid
        const QString &uuid = category->data(CategoryItem::Uuid).toString();
        QSqlQuery query(database());
        QHash<int, QString> where;
        where.insert(Constants::CATEGORY_UUID, QString("='%1'").arg(uuid));
        if (query.exec(select(Constants::Table_CATEGORIES, QList<int>()
                              << Constants::CATEGORY_ID
                              << Constants::CATEGORY_LABEL_ID, where))) {
            if (query.next()) {
                id = query.value(0).toInt();
                category->setData(CategoryItem::DbOnly_Id, id);
                category->setData(CategoryItem::DbOnly_LabelId, query.value(1).toInt());
                category->setDirty(isDirty);
            }
        } else {
            LOG_QUERY_ERROR(query);
            return false;
        }
        // id found --> return true
        return (id >= 0);
    }
    return false;
}

/**
  Update a CategoryItem pointer to database.
  This member should not be call directly.
  The Category::CategoryBase::saveCategory() manages the whole save process (save/update).
  \sa saveCategory()
*/
bool CategoryBase::updateCategory(CategoryItem *category)
{
    // we are inside a transaction
    int id = category->id();
    if (id < 0)
        return false;

    // update needed ?
    if (!category->isDirty())
        return true;

    QHash<int, QString> where;
    where.insert(Constants::CATEGORY_ID, QString("=%1").arg(category->id()));
    QSqlQuery query(database());
    query.prepare(prepareUpdateQuery(Constants::Table_CATEGORIES, QList<int>()
                                     << Constants::CATEGORY_MIME
                                     << Constants::CATEGORY_PASSWORD
                                     << Constants::CATEGORY_ISVALID
                                     << Constants::CATEGORY_PARENT
                                     << Constants::CATEGORY_SORT_ID
                                     << Constants::CATEGORY_THEMEDICON
                                     << Constants::CATEGORY_EXTRAXML
                                     << Constants::CATEGORY_UUID
                                     // << Constants::CATEGORY_LABEL_ID
                                     , where));
    query.bindValue(0, category->mime());
    query.bindValue(1, category->cryptedPassword());
    query.bindValue(2, category->data(CategoryItem::DbOnly_IsValid).toInt());
    query.bindValue(3, category->data(CategoryItem::DbOnly_ParentId));
    query.bindValue(4, category->data(CategoryItem::SortId));
    query.bindValue(5, category->data(CategoryItem::ThemedIcon));
    query.bindValue(6, category->data(CategoryItem::ExtraXml));
    query.bindValue(7, category->data(CategoryItem::Uuid));
    //    query.bindValue(6, category->data(CategoryItem::DbOnly_LabelId));
    if (!query.exec()) {
        LOG_QUERY_ERROR(query);
        return false;
    }
    query.finish();

    // update labels
    if (!saveCategoryLabels(category))
        return false;
    category->setDirty(false);
    return true;
}

/** \brief Save or update categories labels. */
bool CategoryBase::saveCategoryLabels(CategoryItem *category)
{
    //    QTime chr;
    //    chr.start();
    // we are inside a transaction
    if (!category->isDirty())
        return true;
    // get label_id
    int labelId = -1;
    bool deleteOldValues = true;
    QSqlQuery query(database());
    if (category->data(CategoryItem::DbOnly_LabelId).isNull() ||
            category->data(CategoryItem::DbOnly_LabelId).toInt()==-1) {
        deleteOldValues = false;
        if (query.exec(maxSqlCommand(Constants::Table_CATEGORY_LABEL, Constants::CATEGORYLABEL_LABEL_ID))) {
            if (query.next())
                labelId = query.value(0).toInt() + 1;
        } else {
            LOG_QUERY_ERROR(query);
            return false;
        }
        query.finish();
        //        Utils::Log::logTimeElapsed(chr, "--- label", "get max");

        category->setData(CategoryItem::DbOnly_LabelId, labelId);
        // create an empty label using this LabelId
        query.prepare(prepareInsertQuery(Constants::Table_CATEGORY_LABEL));
        query.bindValue(Constants::CATEGORYLABEL_ID, QVariant());
        query.bindValue(Constants::CATEGORYLABEL_LABEL_ID, labelId);
        query.bindValue(Constants::CATEGORYLABEL_LANG, QLocale().name().left(2));
        query.bindValue(Constants::CATEGORYLABEL_VALUE, category->label(QLocale().name().left(2)));
        query.bindValue(Constants::CATEGORYLABEL_ISVALID, 1);
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            return false;
        }
        //        Utils::Log::logTimeElapsed(chr, "--- label", "create LabelLID:" + QString::number(labelId));
    } else {
        labelId = category->data(CategoryItem::DbOnly_LabelId).toInt();
    }
    query.finish();

    // delete all labels related to this LabelId
    QHash<int, QString> where;
    where.clear();
    where.insert(Constants::CATEGORYLABEL_LABEL_ID, QString("=%1").arg(labelId));
    // TODO: improve this, no need to delete before adding
    if (deleteOldValues) {
        query.exec(prepareDeleteQuery(Constants::Table_CATEGORY_LABEL, where));
        query.finish();
        //        Utils::Log::logTimeElapsed(chr, "--- label", "delete old values");
    }

    // save labels
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    foreach(const QString &lang, category->allLanguagesForLabel()) {
        //        where.clear();
        //        where.insert(Constants::CATEGORYLABEL_LABEL_ID, QString("=%1").arg(labelId));
        //        where.insert(Constants::CATEGORYLABEL_LANG, QString("='%1'").arg(lang));
        //        bool update = count(Constants::Table_CATEGORY_LABEL, Constants::CATEGORYLABEL_ID, getWhereClause(Constants::Table_CATEGORY_LABEL, where));
        //        if (update) {
        //            query.prepare(prepareUpdateQuery(Constants::Table_CATEGORY_LABEL, QList<int>()
        //                                             << Constants::CATEGORYLABEL_VALUE
        //                                             << Constants::CATEGORYLABEL_ISVALID, where));
        //            query.bindValue(0, category->label(lang));
        //            query.bindValue(1, 1);
        //        } else {
        query.prepare(prepareInsertQuery(Constants::Table_CATEGORY_LABEL));
        query.bindValue(Constants::CATEGORYLABEL_ID, QVariant());
        query.bindValue(Constants::CATEGORYLABEL_LABEL_ID, labelId);
        query.bindValue(Constants::CATEGORYLABEL_LANG, lang);
        query.bindValue(Constants::CATEGORYLABEL_VALUE, category->label(lang));
        query.bindValue(Constants::CATEGORYLABEL_ISVALID, 1);
        //        }
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            return false;
        }
        query.finish();
        //        Utils::Log::logTimeElapsed(chr, "--- label", "saved one label");
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    }
    return true;
}

/** Set the valid tag all categories from the \e mime to 'invalid'. */
bool CategoryBase::removeAllExistingCategories(const QString &mime)
{
    QSqlQuery query(database());
    QHash<int, QString> where;
    where.insert(Constants::CATEGORY_MIME, QString("='%1'").arg(mime));
    query.prepare(prepareUpdateQuery(Constants::Table_CATEGORIES, Constants::CATEGORY_ISVALID, where));
    query.bindValue(0, 0);
    if (!query.exec()) {
        LOG_QUERY_ERROR(query);
        return false;
    }
    return true;
}

/** Reacts to Core::ICore::databaseServerChanged(). */
void CategoryBase::onCoreDatabaseServerChanged()
{
    m_initialized = false;
    if (QSqlDatabase::connectionNames().contains(Constants::DB_NAME)) {
        QSqlDatabase::removeDatabase(Constants::DB_NAME);
    }
    disconnect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
    disconnect(Core::ICore::instance(), SIGNAL(firstRunDatabaseCreation()), this, SLOT(onCoreFirstRunCreationRequested()));
    initialize();
}

void CategoryBase::onCoreFirstRunCreationRequested()
{
    disconnect(Core::ICore::instance(), SIGNAL(firstRunDatabaseCreation()), this, SLOT(onCoreFirstRunCreationRequested()));
    initialize();
}
