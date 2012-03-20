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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "categorybase.h"
#include "categoryitem.h"
#include "constants.h"

#include <utils/global.h>
#include <utils/log.h>
#include <utils/databaseconnector.h>
#include <translationutils/constants.h>
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
bool CategoryBase::m_initialized = false;

CategoryBase *CategoryBase::instance()
{
    if (!m_Instance) {
        m_Instance = new CategoryBase(qApp);
        m_Instance->init();
    }
    return m_Instance;
}

CategoryBase::CategoryBase(QObject *parent) :
        QObject(parent), Utils::Database()
{
    setObjectName("CategoryBase");
    using namespace Category::Constants;
    addTable(Table_CATEGORIES, "CATEGORIES");
    addTable(Table_CATEGORY_LABEL, "LABEL");
    addTable(Table_PROTECTION, "PROTECTION");
    addTable(Table_VERSION, "VERSION");

    addField(Table_CATEGORIES, CATEGORY_ID,              "CID",        FieldIsUniquePrimaryKey);
    addField(Table_CATEGORIES, CATEGORY_UUID,            "UUID",       FieldIsUUID);
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
    addIndex(Table_CATEGORIES, CATEGORY_UUID);
    addIndex(Table_CATEGORIES, CATEGORY_PARENT);
    addIndex(Table_CATEGORIES, CATEGORY_LABEL_ID);
    addIndex(Table_CATEGORIES, CATEGORY_MIME);
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

    addField(Table_VERSION, VERSION_TEXT, "VERSION",  FieldIsShortText);

    connect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
}

CategoryBase::~CategoryBase()
{
}

bool CategoryBase::init()
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

    if (!checkDatabaseScheme()) {
        LOG_ERROR(tkTr(Trans::Constants::DATABASE_1_SCHEMA_ERROR).arg(Constants::DB_NAME));
        return false;
    }

//    checkDatabaseVersion();

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
                tkTr(Trans::Constants::_1_ISNOT_AVAILABLE_CANNOTBE_CREATED).arg(pathOrHostName);
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
                                         tr("Please contact the development team."));
                return false;
            }
            QSqlQuery q(QString("CREATE DATABASE `%1`").arg(dbName), d);
            if (!q.isActive()) {
                LOG_QUERY_ERROR(q);
                Utils::warningMessageBox(tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2)
                                         .arg(DB.connectionName()).arg(DB.lastError().text()),
                                         tr("Please contact the development team."));
                return false;
            }
            if (!DB.open()) {
                Utils::warningMessageBox(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                         .arg(DB.connectionName()).arg(DB.lastError().text()),
                                         tr("Please contact the development team."));
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
    QSqlQuery query(DB);
    query.prepare(prepareInsertQuery(Constants::Table_VERSION));
    query.bindValue(Constants::VERSION_TEXT, Constants::DB_ACTUALVERSION);
    if (!query.exec()) {
        LOG_QUERY_ERROR(query);
        return false;
    }

    return true;
}

/** \brief Return flatten list of CategoryItem extracted from database. */
QVector<CategoryItem *> CategoryBase::getCategories(const QString &mime) const
{
    QVector<CategoryItem *> cats;
    if (!database().isOpen()) {
        if (!database().open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                      .arg(database().connectionName()).arg(database().lastError().text()));
            return cats;
        }
    }
    QString req;
    QSqlQuery query(database());
    QHash<int, QString> where;
    where.insert(Constants::CATEGORY_ISVALID, "=1");
    where.insert(Constants::CATEGORY_MIME, QString("='%1'").arg(mime));

    // Get Category table
    req = select(Constants::Table_CATEGORIES, where);
    if (query.exec(req)) {
        while (query.next()) {
            CategoryItem *cat = new CategoryItem;
            cat->setData(CategoryItem::DbOnly_Id, query.value(Constants::CATEGORY_ID));
            cat->setData(CategoryItem::DbOnly_LabelId, query.value(Constants::CATEGORY_LABEL_ID));
            cat->setData(CategoryItem::DbOnly_ParentId, query.value(Constants::CATEGORY_PARENT));
            cat->setData(CategoryItem::DbOnly_Mime, query.value(Constants::CATEGORY_MIME));
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
//    qWarning() << "getting categories" << mime << cats.count();
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
                child->setParent(parent);
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
//    qWarning() << "SaveCategory" << category->isDirty();
//    category->warn();

    // save or update ?
    if (!(category->data(CategoryItem::DbOnly_Id).isNull() || category->id()==-1)) {
        return updateCategory(category);
    }
    // save labels
    if (!saveCategoryLabels(category))
        return false;
    // save category itself
    QSqlQuery query(database());
    query.prepare(prepareInsertQuery(Constants::Table_CATEGORIES));
    query.bindValue(Constants::CATEGORY_ID, QVariant());
    query.bindValue(Constants::CATEGORY_UUID, QVariant());
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
    }
    category->setDirty(false);
    return false;
}

/**
  \brief Update a CategoryItem pointer to database. If CategoryItem does not already exist in database, CategoryItem is saved.
  \sa saveCategory()
*/
bool CategoryBase::updateCategory(CategoryItem *category)
{
//    qWarning() << "UpdateCategory" << category->isDirty();
//    category->warn();

    if (category->data(CategoryItem::DbOnly_Id).isNull() || category->id()==-1) {
        return saveCategory(category);
    }
    // update episode
    if (!category->isDirty())
        return true;
    QSqlQuery query(database());
    QHash<int, QString> where;
    where.insert(Constants::CATEGORY_ID, QString("=%1").arg(category->id()));
    query.prepare(prepareUpdateQuery(Constants::Table_CATEGORIES, QList<int>()
                                     << Constants::CATEGORY_MIME
                                     << Constants::CATEGORY_PASSWORD
                                     << Constants::CATEGORY_ISVALID
                                     << Constants::CATEGORY_PARENT
                                     << Constants::CATEGORY_SORT_ID
                                     << Constants::CATEGORY_THEMEDICON
                                     << Constants::CATEGORY_LABEL_ID
                                     << Constants::CATEGORY_EXTRAXML, where));
    query.bindValue(0, category->mime());
    query.bindValue(1, category->cryptedPassword());
    query.bindValue(2, category->data(CategoryItem::DbOnly_IsValid).toInt());
    query.bindValue(3, category->data(CategoryItem::DbOnly_ParentId));
    query.bindValue(4, category->data(CategoryItem::SortId));
    query.bindValue(5, category->data(CategoryItem::ThemedIcon));
    query.bindValue(6, category->data(CategoryItem::DbOnly_LabelId));
    query.bindValue(7, category->data(CategoryItem::ExtraXml));

    if (!query.exec()) {
        LOG_QUERY_ERROR(query);
    }
    query.finish();

    // update labels
    saveCategoryLabels(category);
    category->setDirty(false);
    return false;
}

/** \brief Save or update categories labels. */
bool CategoryBase::saveCategoryLabels(CategoryItem *category)
{
    if (!category->isDirty())
        return true;
    // get label_id
    int labelId = -1;
    if (category->data(CategoryItem::DbOnly_LabelId).isNull() ||
        category->data(CategoryItem::DbOnly_LabelId).toInt()==-1) {
        labelId = max(Constants::Table_CATEGORY_LABEL, Constants::CATEGORYLABEL_LABEL_ID).toInt();
        ++labelId;
        category->setData(CategoryItem::DbOnly_LabelId, labelId);
        // create an empty label using this LabelId
        QSqlQuery query(database());
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
        query.finish();
    } else {
        labelId = category->data(CategoryItem::DbOnly_LabelId).toInt();
    }
    // delete all labels related to this LabelId
    QHash<int, QString> where;
    where.clear();
    where.insert(Constants::CATEGORYLABEL_LABEL_ID, QString("=%1").arg(labelId));
    QSqlQuery query(database());
    /** \todo improve this, no need to delete before adding */
    if (!category->allLanguagesForLabel().isEmpty())
        query.exec(prepareDeleteQuery(Constants::Table_CATEGORY_LABEL, where));
    // save labels
    foreach(const QString &lang, category->allLanguagesForLabel()) {
        where.clear();
        where.insert(Constants::CATEGORYLABEL_LABEL_ID, QString("=%1").arg(labelId));
        where.insert(Constants::CATEGORYLABEL_LANG, QString("='%1'").arg(lang));
        bool update = count(Constants::Table_CATEGORY_LABEL, Constants::CATEGORYLABEL_ID, getWhereClause(Constants::Table_CATEGORY_LABEL, where));
        if (update) {
            query.prepare(prepareUpdateQuery(Constants::Table_CATEGORY_LABEL, QList<int>()
                                             << Constants::CATEGORYLABEL_VALUE
                                             << Constants::CATEGORYLABEL_ISVALID, where));
            query.bindValue(0, category->label(lang));
            query.bindValue(1, 1);
        } else {
            query.prepare(prepareInsertQuery(Constants::Table_CATEGORY_LABEL));
            query.bindValue(Constants::CATEGORYLABEL_ID, QVariant());
            query.bindValue(Constants::CATEGORYLABEL_LABEL_ID, labelId);
            query.bindValue(Constants::CATEGORYLABEL_LANG, lang);
            query.bindValue(Constants::CATEGORYLABEL_VALUE, category->label(lang));
            query.bindValue(Constants::CATEGORYLABEL_ISVALID, 1);
        }
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            return false;
        }
        query.finish();
    }
    return true;
}

bool CategoryBase::removeAllExistingCategories(const QString &mime)
{
    QSqlQuery query(database());
    QHash<int, QString> where;
    where.insert(Constants::CATEGORY_MIME, QString("='%1'").arg(mime));
    query.prepare(prepareUpdateQuery(Constants::Table_CATEGORIES, Constants::CATEGORY_ISVALID, where));
    query.bindValue(0, 0);
    if (!query.exec()) {
        LOG_QUERY_ERROR(query);
    }
    return false;
}

void CategoryBase::onCoreDatabaseServerChanged()
{
    m_initialized = false;
    if (QSqlDatabase::connectionNames().contains(Constants::DB_NAME)) {
        QSqlDatabase::removeDatabase(Constants::DB_NAME);
    }
    init();
}

