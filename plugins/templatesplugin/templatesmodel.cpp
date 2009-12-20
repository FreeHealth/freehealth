/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "templatesmodel.h"
#include "itemplates.h"
#include "constants.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants.h>

#include <translationutils/constanttranslations.h>
#include <utils/log.h>

#include <QList>
#include <QColor>
#include <QSqlTableModel>
#include <QDir>

#include <QDebug>

using namespace Templates;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}

namespace Templates {
namespace Internal {

    static const char *const DATABASE_NAME = "templates";
    static const char *const DATABASE_FILENAME = "templates.db";
    static const char *const DATABASE_ACTUAL_VERSION = "0.3.0";


class TreeItem
{
public:
    TreeItem(const QHash<int, QVariant> &data, TreeItem *parent = 0) :
            m_Parent(parent), itemData(data), m_IsTemplate(false),
            m_IsModified(false), m_NewlyCreated(false)
    {}
    ~TreeItem() { qDeleteAll(m_Children); }

    // Genealogy management
    TreeItem *child(int number) { return m_Children.value(number); }
    int childCount() const { return m_Children.count(); }
    int columnCount() const { return itemData.count(); }
    TreeItem *parent() { return m_Parent; }
    void setParent(TreeItem *parent) { m_Parent = parent; }
    bool addChildren(TreeItem *child)
    {
        if (!m_Children.contains(child))
            m_Children.append(child);
        return true;
    }
    int childNumber() const
    {
        if (m_Parent)
            return m_Parent->m_Children.indexOf(const_cast<TreeItem*>(this));
        return 0;
    }

    // For tree management
    void setHasTemplate(bool isTemplate) { m_IsTemplate=true; }
    bool isTemplate() const {return m_IsTemplate;}

    // For database management
    void setModified(bool state) {m_IsModified = state;}
    bool isModified() const {return m_IsModified;}
    void setNewlyCreated(bool state) {m_NewlyCreated = state;}
    bool isNewlyCreated() const {return m_NewlyCreated;}

    bool insertColumns(int position, int columns)
    {
        if (position < 0 || position > itemData.size())
            return false;
        for (int column = 0; column < columns; ++column)
            itemData.insert(position, QVariant());
        foreach (TreeItem *child, m_Children)
            child->insertColumns(position, columns);
        return true;
    }

//    bool removeChildren(int position, int count)
//    {
//        if (position < 0 || position + count > childItems.size())
//            return false;
//        for (int row = 0; row < count; ++row)
//            delete childItems.takeAt(position);
//        return true;
//    }

//    bool removeColumns(int position, int columns)
//    {
//        if (position < 0 || position + columns > itemData.size())
//            return false;
//        for (int column = 0; column < columns; ++column)
//            itemData.remove(position);
//        foreach (TreeItem *child, childItems)
//            child->removeColumns(position, columns);
//        return true;
//    }


    // For data management
    QVariant data(int column) const
    {
        return itemData.value(column);
    }

    bool setData(int column, const QVariant &value)
    {
        itemData.insert(column, value);
        m_IsModified = true;
        return true;
    }

private:
    TreeItem *m_Parent;
    QList<TreeItem*> m_Children;
    QHash<int, QVariant> itemData;
    bool m_IsTemplate, m_IsModified, m_NewlyCreated;
};


class TemplatesModelPrivate
{
public:
    TemplatesModelPrivate(Templates::TemplatesModel *parent) : q(parent), m_RootItem(0)
    {
        QSqlDatabase DB;
        DB = QSqlDatabase::addDatabase("QSQLITE" , Internal::DATABASE_NAME);
        if (!databasePath().exists()) {
            if (!QDir().mkpath(databasePath().absolutePath())) {
                tkTr(Trans::Constants::_1_ISNOT_AVAILABLE_CANNOTBE_CREATED).arg(databasePath().absolutePath());
            }
        }
        DB.setDatabaseName(QDir::cleanPath(databasePath().absolutePath() + QDir::separator() + Internal::DATABASE_FILENAME));
        if (!DB.open()) {
            Utils::Log::addError(q, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                 .arg(Internal::DATABASE_NAME)
                                 .arg(DB.lastError().text()));
        }
        // Test if database already created or need to be created
        if (DB.tables(QSql::Tables).count() == 0) {
            createDatabase();
        }
        QHash<int, QVariant> datas;
        datas.insert(TemplatesModel::Data_Label, "ROOT");
        m_RootItem = new TreeItem(datas,0);
    }

    ~TemplatesModelPrivate()
    {
        if (m_RootItem) {
            delete m_RootItem;
            m_RootItem = 0;
        }
    }

    QDir databasePath() const
    {
        return QDir(settings()->path(Core::ISettings::ReadWriteDatabasesPath) + QDir::separator() + DATABASE_NAME);
    }

    bool createDatabase() const
    {
        Utils::Log::addMessage(q, tkTr(Trans::Constants::TRYING_TO_CREATE_1_PLACE_2)
                               .arg(DATABASE_NAME).arg(databasePath().absolutePath()));
        QSqlDatabase DB = QSqlDatabase::database(DATABASE_NAME);
        if (!DB.open()) {
            Utils::Log::addError(q, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                 .arg(DATABASE_NAME)
                                 .arg(DB.lastError().text()));
            return false;
        }
        QStringList req;
        req <<  "CREATE TABLE IF NOT EXISTS `TEMPLATES` ("
                "`TEMPLATE_ID`              INTEGER        PRIMARY KEY AUTOINCREMENT,"
                "`TEMPLATE_UUID`            varchar(40)    NULL,"
                "`USER_UUID`                int(11)        NULL,"
                "`ID_CATEGORY`              int(11)        -1,"
                "`LABEL`                    varchar(300)   NULL,"
                "`SUMMARY`                  varchar(500)   NULL,"
                "`CONTENT`                  blob           NULL,"
                "`DATE_CREATION`            date           NULL,"
                "`DATE_MODIFICATION`        date           NULL,"
                "`THEMED_ICON_FILENAME`     varchar(50)    NULL"
                ");";
        req <<  "CREATE TABLE IF NOT EXISTS `CATEGORIES` ("
                "`CATEGORY_ID`              INTEGER        PRIMARY KEY AUTOINCREMENT,"
                "`CATEGORY_UUID`            varchar(40)    NULL,"
                "`USER_UUID`                int(11)        NULL,"
                "`PARENT_CATEGORY`          int(11)        -1,"
                "`LABEL`                    varchar(300)   NULL,"
                "`SUMMARY`                  varchar(500)   NULL,"
                "`DATE_CREATION`            date           NULL,"
                "`DATE_MODIFICATION`        date           NULL,"
                "`THEMED_ICON_FILENAME`     varchar(50)    NULL"
                ");";
        req <<  "CREATE TABLE IF NOT EXISTS `VERSION` ("
                "`ACTUAL`                  varchar(10)"
                ");";
        req <<  QString("INSERT INTO `VERSION` (`ACTUAL`) VALUES('%1');").arg(DATABASE_ACTUAL_VERSION);
        bool toReturn = true;
        foreach(const QString &r, req) {
            QSqlQuery query(r,DB);
            if (query.isActive()) {
                query.next();
            } else {
                Utils::Log::addQueryError(q,query);
                toReturn = false;
            }
        }
        return toReturn;
    }

    void setupModelData()
    {
        Utils::Log::addMessage(q, "Getting Templates Categroies");
        QSqlDatabase DB = QSqlDatabase::database(DATABASE_NAME);
        if (!DB.open()) {
            Utils::Log::addError(q, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                 .arg(DATABASE_NAME)
                                 .arg(DB.lastError().text()));
            return;
        }
        QHash<int, TreeItem*> categories;

        // get categories
        QString req = "SELECT `CATEGORY_ID`, `CATEGORY_UUID`, `USER_UUID`, "
                      "`PARENT_CATEGORY`, `LABEL`, `SUMMARY`,`DATE_CREATION`, `DATE_MODIFICATION` "
                      "FROM `CATEGORIES`";
        QSqlQuery query(req,DB);
        if (query.isActive()) {
            QHash<int, QVariant> datas;
            while (query.next()) {
                datas.insert(TemplatesModel::Data_Id, query.value(0));
                datas.insert(TemplatesModel::Data_Uuid, query.value(1));
                datas.insert(TemplatesModel::Data_UserUuid, query.value(2));
                datas.insert(TemplatesModel::Data_ParentId, query.value(3));
                datas.insert(TemplatesModel::Data_Label, query.value(4));
                datas.insert(TemplatesModel::Data_Summary, query.value(5));
                datas.insert(TemplatesModel::Data_CreationDate, query.value(6));
                datas.insert(TemplatesModel::Data_ModifDate, query.value(7));
                categories.insert(datas.value(TemplatesModel::Data_Id).toInt(), new TreeItem(datas,0));
                datas.clear();
            }
        } else {
            Utils::Log::addQueryError(q, query);
        }
        query.finish();
        foreach(TreeItem *item, categories.values()) {
            // need to be reparented
            item->setParent(categories.value(item->data(TemplatesModel::Data_ParentId).toInt(), m_RootItem));
            // add item to the children of its parent
            item->parent()->addChildren(item);
        }

        // get templates
        QList<TreeItem *> templates;
        req = "SELECT `TEMPLATE_ID`, `TEMPLATE_UUID`, `USER_UUID`, `ID_CATEGORY`, `LABEL`, "
                      "`SUMMARY`, `DATE_CREATION`, `DATE_MODIFICATION`"
                      "FROM `TEMPLATES`";
        query.exec(req);
        if (query.isActive()) {
            QHash<int, QVariant> datas;
            while (query.next()) {
                datas.insert(TemplatesModel::Data_Id, query.value(0));
                datas.insert(TemplatesModel::Data_Uuid, query.value(1));
                datas.insert(TemplatesModel::Data_UserUuid, query.value(2));
                datas.insert(TemplatesModel::Data_ParentId, query.value(3));
                datas.insert(TemplatesModel::Data_Label, query.value(4));
                datas.insert(TemplatesModel::Data_Summary, query.value(5));
                datas.insert(TemplatesModel::Data_CreationDate, query.value(6));
                datas.insert(TemplatesModel::Data_ModifDate, query.value(7));
                TreeItem *it = new TreeItem(datas,0);
                it->setHasTemplate(true);
                templates.insert(datas.value(TemplatesModel::Data_Id).toInt(), it);
                datas.clear();
            }
        } else {
            Utils::Log::addQueryError(q, query);
        }
        query.finish();
        // add templates to categories
        foreach(TreeItem *item, templates) {
            // need to be reparented
            item->setParent(categories.value(item->data(TemplatesModel::Data_ParentId).toInt(),m_RootItem));
            // add item to the children of its parent
            item->parent()->addChildren(item);
        }

    }

    TreeItem *getItem(const QModelIndex &index) const
    {
        if (index.isValid()) {
            TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
            if (item) return item;
        }
        return m_RootItem;
    }


private:
    Templates::TemplatesModel *q;

public:
    TreeItem *m_RootItem;
};

}  // End Internal
}  // End Templates


TemplatesModel::TemplatesModel(QObject *parent) :
        QAbstractItemModel(parent),
        d(new Internal::TemplatesModelPrivate(this))
{
    setObjectName("TemplatesModel");
    d->setupModelData();

//    setTable(drugsBase()->table(Constants::Table_CIS));
//    setEditStrategy( QSqlTableModel::OnManualSubmit );
//    QHashWhere where;
//    where.insert(Constants::CIS_COMMERCIALISATION, "='O'");
//    setFilter(drugsBase()->getWhereClause(Constants::Table_CIS, where));
//    select();
}

TemplatesModel::~TemplatesModel()
{
    if (d) {
        delete d;
        d = 0;
    }
}

bool TemplatesModel::setCurrentUser(const QString &uuid)
{
    return true;
}

//int TemplatesModel::columnCount(const QModelIndex & /* parent */) const
// {
//     return d->m_RootItem->columnCount();
// }

QModelIndex TemplatesModel::index(int row, int column, const QModelIndex &parent) const
 {
     if (parent.isValid() && parent.column() != 0)
         return QModelIndex();

     Internal::TreeItem *parentItem = d->getItem(parent);
     Internal::TreeItem *childItem = parentItem->child(row);
     if (childItem)
         return createIndex(row, column, childItem);
     else
         return QModelIndex();
 }

QModelIndex TemplatesModel::parent(const QModelIndex &index) const
 {
     if (!index.isValid())
         return QModelIndex();

     Internal::TreeItem *childItem = d->getItem(index);
     Internal::TreeItem *parentItem = childItem->parent();

     if (parentItem == d->m_RootItem)
         return QModelIndex();

     return createIndex(parentItem->childNumber(), 0, parentItem);
 }

int TemplatesModel::rowCount(const QModelIndex &parent) const
{
    Internal::TreeItem *item = d->getItem(parent);
    if (item)
        return item->childCount();
    return 0;
}

int TemplatesModel::columnCount(const QModelIndex &parent) const
{
    return Max_Param;
}


bool TemplatesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return true;
}

QVariant TemplatesModel::data(const QModelIndex &item, int role) const
{
    if (!item.isValid())
        return QVariant();

    const Internal::TreeItem *it = d->getItem(item);
    switch (role)
    {
        case Qt::EditRole :
        case Qt::DisplayRole :
        {
            return it->data(item.column());
        }
        case Qt::ToolTipRole :
        {
            return it->data(TemplatesModel::Data_Summary);
        }
        case Qt::ForegroundRole :
        {
            if (it->isTemplate()) {
                return QColor(settings()->value(Constants::S_FOREGROUND_TEMPLATES, "#000").toString());
            } else {
                return QColor(settings()->value(Constants::S_FOREGROUND_CATEGORIES, "darkblue").toString());
            }
        }
    }
    return QVariant();
}

Qt::ItemFlags TemplatesModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

bool TemplatesModel::setCurrentUser(const QString &uuid)
{}


