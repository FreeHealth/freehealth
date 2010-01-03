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
/**
  \class Templates::TemplatesModel
  Manages the templates database in/out and link to views.
  Datas are statically stored so that you can instanciate how many requiered models
  as you want without consuming RAM.
*/

#include "templatesmodel.h"
#include "itemplates.h"
#include "constants.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants.h>

#include <translationutils/constanttranslations.h>
#include <utils/log.h>
#include <utils/serializer.h>

#include <QList>
#include <QColor>
#include <QSqlTableModel>
#include <QDir>
#include <QMimeData>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>

#include <QDebug>

using namespace Templates;
using namespace Trans::ConstantTranslations;

enum { base64MimeDatas = true };

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
    {
        setHasTemplate(data.value(TemplatesModel::Data_IsTemplate).toBool());
    }
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
    bool insertChild(const int row, TreeItem *child)
    {
        if (row > m_Children.count())
            return false;
        m_Children.insert(row, child);
        return true;
    }
    int childNumber() const
    {
        if (m_Parent)
            return m_Parent->m_Children.indexOf(const_cast<TreeItem*>(this));
        return 0;
    }
    void sortChildren()
    {
        qSort(m_Children.begin(), m_Children.end(), TreeItem::lessThan);
    }

    // For category only tree
    int childCategoryCount() const
    {
        int n = 0;
        foreach(TreeItem *c, this->m_Children) {
            if (!c->isTemplate())
                ++n;
        }
        return n;
    }
    TreeItem *categoryChild(int number)
    {
        QList<TreeItem *> cat;
        foreach(TreeItem *c, this->m_Children) {
            if (!c->isTemplate())
                cat << c;
        }
        return cat.value(number);
    }
    int categoryChildNumber() const
    {
        if (m_Parent) {
            QList<TreeItem *> cat;
            foreach(TreeItem *c, m_Parent->m_Children) {
                if (!c->isTemplate())
                    cat << c;
            }
            return cat.indexOf(const_cast<TreeItem*>(this));
        }
        return 0;
    }


    // For tree management
    void setHasTemplate(bool isTemplate) {m_IsTemplate = isTemplate; setData(TemplatesModel::Data_IsTemplate, isTemplate); }
    bool isTemplate() const {return m_IsTemplate;}

    // For database management
    void setModified(bool state) {m_IsModified = state;}
    bool isModified() const {return m_IsModified;}
    void setNewlyCreated(bool state) {m_NewlyCreated = state;}
    bool isNewlyCreated() const {return m_NewlyCreated;}

    bool removeChild(TreeItem *child)
    {
        if (m_Children.contains(child)) {
            m_Children.removeAll(child);
            return true;
        }
        return false;
    }

    // For data management
    QVariant data(int column) const
    {
        return itemData.value(column);
    }
    bool setData(int column, const QVariant &value)
    {
        itemData.insert(column, value);
        if (column==TemplatesModel::Data_IsTemplate) {
            m_IsTemplate=value.toBool();
        }
        m_IsModified = true;
        return true;
    }
    QHash<int, QVariant> datas() const {return itemData;}

    // For sort functions
    static bool lessThan(TreeItem *item1, TreeItem *item2)
    {
        // category goes first
        // then sort by name
        bool sameType = (((item1->isTemplate()) && (item2->isTemplate())) || ((!item1->isTemplate()) && (!item2->isTemplate())));
        if (sameType)
            return item1->data(TemplatesModel::Data_Label).toString() < item2->data(TemplatesModel::Data_Label).toString();
        return item2->isTemplate();
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
    TemplatesModelPrivate(Templates::TemplatesModel *parent) :
            q(parent), m_RootItem(0),
            m_ShowOnlyCategories(false)
    {
        if (!m_ModelDatasRetreived) {
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
            datas.insert(TemplatesModel::Data_ParentId, -1);

            if (!m_Tree) {
                m_Tree = new TreeItem(datas,0);
            }
        }
        m_RootItem = m_Tree;
        ++m_Handle;
        m_Handles.insert(this);
    }

    ~TemplatesModelPrivate()
    {
        --m_Handle;
        m_Handles.remove(this);
        if (m_Handle==0) {
            if (m_Tree) {
                delete m_Tree;
                m_Tree = 0;
                m_RootItem = 0;
            }
            m_ModelDatasRetreived = false;
        }
    }

    void resetAllInstances() const
    {
        foreach(TemplatesModelPrivate *pr, m_Handles) {
            pr->q->reset();
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
                "`THEMED_ICON_FILENAME`     varchar(50)    NULL,"
                "`TRANSMISSION_DATE`        date           NULL"
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
                "`THEMED_ICON_FILENAME`     varchar(50)    NULL,"
                "`TRANSMISSION_DATE`        date           NULL"
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
        if (m_ModelDatasRetreived)
            return;

        Utils::Log::addMessage(q, "Getting Templates Categroies");
        QSqlDatabase DB = QSqlDatabase::database(DATABASE_NAME);
        if (!DB.open()) {
            Utils::Log::addError(q, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                 .arg(DATABASE_NAME)
                                 .arg(DB.lastError().text()));
            return;
        }

        // get categories
        m_IdToCategory.clear();
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
                m_IdToCategory.insert(datas.value(TemplatesModel::Data_Id).toInt(), new TreeItem(datas,0));
                datas.clear();
            }
        } else {
            Utils::Log::addQueryError(q, query);
        }
        query.finish();
        foreach(TreeItem *item, m_IdToCategory.values()) {
            // need to be reparented
            item->setParent(m_IdToCategory.value(item->data(TemplatesModel::Data_ParentId).toInt(), m_RootItem));
            // add item to the children of its parent
            item->parent()->addChildren(item);
        }

        // get templates
        Utils::Log::addMessage(q, "Getting Templates");
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
            item->setParent(m_IdToCategory.value(item->data(TemplatesModel::Data_ParentId).toInt(),m_RootItem));
            // add item to the children of its parent
            item->parent()->addChildren(item);
        }
        sortItems();
        m_ModelDatasRetreived = true;
    }

    void sortItems(TreeItem *root = 0)
    {
        if (!root)
            root = m_RootItem;
        int n = root->childCount();
        root->sortChildren();
        for(int i = 0; i < n; ++i) {
            sortItems(root->child(i));
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

    // For Drag and Drop
    static QModelIndexList allChildren(const QModelIndex &item)
    {
        QModelIndexList toReturn;
        const QAbstractItemModel *model = item.model();
        int row = 0;
        while (model->hasIndex(row,0,item)) {
            toReturn.append(model->index(row,0,item));
            ++row;
        }
        return toReturn;
    }
    QString itemAndChildrenMimeData(const QModelIndex &index)
    {
        QString toReturn;
        if (!index.isValid())
            return QString();
        Internal::TreeItem *item = getItem(index);
        toReturn += "<Template content=\"" + Utils::Serializer::toString(item->datas(), base64MimeDatas) + "\">";
        // add children
        QString tmp;
        QModelIndexList children = allChildren(index);
        foreach(const QModelIndex &child, children) {
            tmp.append(itemAndChildrenMimeData(child));
        }
        if (!tmp.isEmpty()) {
            toReturn.append("\n<Child>\n");
            toReturn.append(tmp);
            toReturn.append("</Child>\n");
        }
        toReturn.append("</Template>\n");
        return toReturn;
    }

    // For debugging
    void warnTree(Internal::TreeItem *root, int indent = 0)
    {
        QString space;
        space.fill(' ',indent);
        qWarning() << space << root->data(TemplatesModel::Data_Label);
        for(int i=0; i<root->childCount(); ++i) {
            warnTree(root->child(i), indent + 2);
        }
    }

private:
    Templates::TemplatesModel *q;

public:
    TreeItem *m_RootItem;
    bool m_ShowOnlyCategories;
    static TreeItem *m_Tree;
    static bool m_ModelDatasRetreived;
    static int m_Handle;
    static QSet<TemplatesModelPrivate *> m_Handles;
    static QHash<int, TreeItem *> m_IdToCategory;
};

TreeItem *TemplatesModelPrivate::m_Tree = 0;
bool TemplatesModelPrivate::m_ModelDatasRetreived = false;
int TemplatesModelPrivate::m_Handle = 0;
QSet<TemplatesModelPrivate *> TemplatesModelPrivate::m_Handles;
QHash<int, TreeItem *> TemplatesModelPrivate::m_IdToCategory;

}  // End Internal
}  // End Templates


TemplatesModel::TemplatesModel(QObject *parent) :
        QAbstractItemModel(parent),
        d(new Internal::TemplatesModelPrivate(this))
{
    setObjectName("TemplatesModel");
    d->setupModelData();
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

QModelIndex TemplatesModel::index(int row, int column, const QModelIndex &parent) const
 {
     if (parent.isValid() && parent.column() != 0)
         return QModelIndex();

     Internal::TreeItem *parentItem = d->getItem(parent);
     Internal::TreeItem *childItem = 0;
     if (d->m_ShowOnlyCategories) {
         childItem = parentItem->categoryChild(row);
     } else {
         childItem = parentItem->child(row);
     }

     if (childItem) {
         return createIndex(row, column, childItem);
     }
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

     if (d->m_ShowOnlyCategories) {
         return createIndex(parentItem->categoryChildNumber(), 0, parentItem);
     }
     return createIndex(parentItem->childNumber(), 0, parentItem);
 }

bool TemplatesModel::reparentIndex(const QModelIndex &item, const QModelIndex &parent)
{
    /** \todo buggy +++ */
//    if (!item.isValid())
//        return false;
//    if ((item.parent().data().toString()==parent.data().toString()) &&
//        (index(item.row(), Data_ParentId, item.parent()).data().toInt()==index(parent.row(), Data_Id, parent.parent()).data().toInt()))
//        return true;
//    int row = rowCount(parent);
//
//    qWarning() << "reparentIndex" << index(item.row(), Data_ParentId, item.parent()).data().toInt() << item.parent().data().toString()
//               << "to" << index(parent.row(), Data_Id, parent.parent()).data().toInt() << parent.data().toString();
//    qWarning() << d->getItem(item.parent()) << d->getItem(parent) << (item.parent()==parent);
//    // add the item
//    insertRow(row, parent);
//    for(int i=0; i<Data_Max_Param; ++i) {
//        setData(index(row, i, parent), index(item.row(), i, item.parent()).data());
//    }

    // append its children
//    while (hasIndex(0, 0, item)) {
//        qWarning() << "reparentIndex row" << row << index(row, Data_Label, item).data().toString();
//        reparentIndex(index(0, 0, item), index(row, 0, parent));
//    }
//    removeRow(item.row(), item.parent());
    return true;
}

int TemplatesModel::rowCount(const QModelIndex &parent) const
{
    Internal::TreeItem *item = d->getItem(parent);
    if (item) {
        if (d->m_ShowOnlyCategories) {
            return item->childCategoryCount();
        } else {
            return item->childCount();
        }
    }
    return 0;
}

int TemplatesModel::columnCount(const QModelIndex &parent) const
{
    return Data_Max_Param;
}

bool TemplatesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    Internal::TreeItem *it = d->getItem(index);
    switch (role)
    {
        case Qt::EditRole :
        case Qt::DisplayRole :
        {
            it->setData(index.column(),value);
        }
    }
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
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}

Qt::DropActions TemplatesModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

bool TemplatesModel::insertTemplate(const Templates::ITemplate *t)
{
    // find parent
//    Internal::TreeItem *parent = d->m_IdToCategory.value(t->parentId());
    // insertRow in parentIndex
    // setDatas of newly created row
//    Internal::TreeItem *parentItem = d->getItem(item)->parent();
//    QHash<int,QVariant> datas;
//    /** \todo manage user */
//    datas.insert(TemplatesModel::Data_Label, t->label());
//    datas.insert(TemplatesModel::Data_Id, t->id());
//    datas.insert(TemplatesModel::Data_CreationDate, QDateTime::currentDateTime());
//    datas.insert(TemplatesModel::Data_Content, t->content());
//    datas.insert(TemplatesModel::Data_Summary, t->summary());
//    datas.insert(TemplatesModel::Data_ParentId, parentItem->data(TemplatesModel::Data_Id));
//    datas.insert(TemplatesModel::Data_Uuid, QUuid());
//    datas.insert(TemplatesModel::Data_Summary, t->summary());
//    Internal::TreeItem *newItem = new Internal::TreeItem(datas,parentItem);
//    /** \todo reset all instances of the model */
//    d->resetAllInstances();
    return true;
}

bool TemplatesModel::insertRows(int row, int count, const QModelIndex &parent)
{
//    qWarning() << "insertRows" << row << count << parent.data();
    Internal::TreeItem *parentItem = 0;
    if (!parent.isValid())
        parentItem = d->m_RootItem;
    else
        parentItem = d->getItem(parent);
//    if (!parent.isValid())
//        return false;
//    Internal::TreeItem *parentItem = d->getItem(parent);
    QHash<int, QVariant> datas;
    datas.insert(TemplatesModel::Data_Label, tr("New"));
    datas.insert(TemplatesModel::Data_ParentId, parentItem->data(TemplatesModel::Data_Id));
    datas.insert(TemplatesModel::Data_CreationDate, QDateTime::currentDateTime());
    datas.insert(TemplatesModel::Data_IsTemplate, false);
    beginInsertRows(parent, row, row+count-1);
    for(int i=0; i<count; ++i) {
        Internal::TreeItem *item = new Internal::TreeItem(datas, parentItem);
        if (!parentItem->insertChild(row+i, item))
            return false;
    }
    endInsertRows();
    return true;
}

bool TemplatesModel::removeRows(int row, int count, const QModelIndex &parent)
{
//    qWarning() << "removeRows" << row << count;
    Internal::TreeItem *parentItem = 0;
    if (!parent.isValid())
        parentItem = d->m_RootItem;
    else
        parentItem = d->getItem(parent);
    beginRemoveRows(parent, row, row+count-1);
    for(int i=0; i<count; ++i) {
        Internal::TreeItem *item = parentItem->child(row+i);
        parentItem->removeChild(item);
        qWarning() << "removeRows deleting item" << item->data(TemplatesModel::Data_Label) << "from" << parent.data().toString();
        delete item;
        item = 0;
    }
    endRemoveRows();
    return true;
}

QMimeData *TemplatesModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QString tmp;
    QModelIndexList fullList;
    foreach (const QModelIndex &index, indexes) {
        tmp += d->itemAndChildrenMimeData(index);
    }
    mimeData->setData(mimeTypes().at(0), tmp.toUtf8());
    return mimeData;
}

static void parseMimeData(const QDomElement &element, const QModelIndex &parent, const int row, QAbstractItemModel *model)
{
    static int indent = 0;
    indent += 2;
//    qWarning() << QString().fill(' ', indent) + "parse ; add to modelIndex" << parent.data().toString();
    QDomNode n = element;
    QDomNode child = element;
    QModelIndex newItem = parent;
    int zz = 0;

    while (!n.isNull()) {
        QDomElement e = n.toElement();
        if (!e.isNull()) {
//            qWarning() << QString().fill(' ', indent) + "  " + e.tagName() + " : " + e.text().mid(14, e.text().indexOf(")#", 14) - 14) << e.childNodes().count();
            // Manage templates
            if (e.tagName().compare("Template", Qt::CaseInsensitive) == 0) {
                QHash<int, QVariant> datas = Utils::Serializer::toVariantHash(e.attribute("content"), base64MimeDatas);
//                qWarning() << e.attribute("content");
//                qWarning() << datas;
                datas.insert(TemplatesModel::Data_ParentId, model->index(parent.row(), TemplatesModel::Data_Id, parent.parent()).data());
                model->insertRow(row+zz, parent);
                for(int i=0; i<TemplatesModel::Data_Max_Param; ++i) {
                    model->setData(model->index(row+zz, i, parent), datas.value(i, QVariant()));
                }
                ++zz;
            }
            if (e.tagName().compare("Child", Qt::CaseInsensitive) == 0) {
//                qWarning() << QString().fill(' ', indent) + "    -> Child";
                newItem = model->index(0, 0, newItem);
                parseMimeData(child.firstChildElement(), newItem, 0, model);
                child = child.firstChildElement("Child");
            }
            // Manage all children
            child = n.firstChildElement("Child");
            while (!child.isNull()) {
//                qWarning() << QString().fill(' ', indent) + "    -> Manage Child";
                newItem = model->index(0, 0, newItem);
                parseMimeData(child.firstChildElement("Template"), newItem, 0, model);
                child = child.firstChildElement("Child");
            }
        }
        n = n.nextSibling();
    }
    
}

bool TemplatesModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
//    qWarning() << "dropMimeData" << row;
    if (action == Qt::IgnoreAction)
        return true;
    
    if (!data->hasFormat(mimeTypes().at(0)))
        return false;
    
    QModelIndex parentIndex = parent;
    while ((isTemplate(parentIndex)) || (parentIndex.isValid())) {
        parentIndex = parentIndex.parent();
    }
    int beginRow = 0;
    if (row != -1)
        beginRow = row;
    
    QDomDocument doc;
    {
        QString error;
        int errorLine, errorColumn;
        if (!doc.setContent(data->data(mimeTypes().at(0)), &error, &errorLine, &errorColumn)) {
            Utils::Log::addError(this, QString("XML Error : %1 , line %2 , column %3 \n%4")
                                 .arg(error)
                                 .arg(errorLine).arg(errorColumn)
                                 .arg(QString(data->data(mimeTypes().at(0)))));
        }
    }
    QDomNode n = doc.documentElement();
    int zz = 0;
    while (!n.isNull()) {
        parseMimeData(n.toElement(), parentIndex, beginRow, this);
        n = n.nextSibling();
    }
    return true;
}


bool TemplatesModel::isTemplate(const QModelIndex &index) const
{
    if (!index.isValid())
        return false;

    const Internal::TreeItem *it = d->getItem(index);
    return it->isTemplate();
}

void TemplatesModel::categoriesOnly() const
{
    d->m_ShowOnlyCategories = true;
}

