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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "simplecategorymodel.h"
#include "listviewbase.h"
#include "constants.h"

#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/iuser.h>
#include <coreplugin/itheme.h>
#include <coreplugin/translators.h>

#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <QSqlQuery>
#include <QSqlDatabase>
#include <QIcon>

#include <QDebug>

using namespace Views;

static inline Internal::ViewBase *base() {return Internal::ViewBase::instance();}
static inline Core::IPatient *patient()  { return Core::ICore::instance()->patient(); }
static inline QString currentUserUuid() {return Core::ICore::instance()->user()->value(Core::IUser::Uuid).toString();}
static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}

namespace Views {
namespace Internal {

    static int TranslatedLabel = -2;


class TreeItem
{
public:
    TreeItem(const QHash<int, QVariant> &datas, TreeItem *parent = 0) :
            m_Parent(parent),
            m_Datas(datas),
            m_IsModified(false)
    {
        if (m_Parent)
            m_Parent->addChildren(this);
    }
    ~TreeItem() { qDeleteAll(m_Children); }

    // Genealogy management
    TreeItem *child(int number) { return m_Children.value(number); }
    int childCount() const { return m_Children.count(); }
    int columnCount() const { return SimpleCategoryModel::ColumnCount; }
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
//    void sortChildren()
//    {
//        qSort(m_Children.begin(), m_Children.end(), TreeItem::lessThan);
//    }

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
        return m_Datas.value(column, QVariant());
    }

    bool setData(int column, const QVariant &value)
    {
        //        qWarning()<< data(column) << value << (data(column)==value);
        if (data(column)==value)
            return true;
        m_Datas.insert(column, value);
        m_IsModified = true;
        if (!m_DirtyRows.contains(column))
            m_DirtyRows.append(column);
        return true;
    }

    QVector<int> dirtyRows() const
    {
        return m_DirtyRows;
    }

    void setNonModified()
    {
        m_DirtyRows.clear();
    }

    // For sort functions
//    static bool lessThan(TreeItem *item1, TreeItem *item2)
//    {
//        // category goes first
//        // then sort by name
//        bool sameType = (((item1->isTemplate()) && (item2->isTemplate())) || ((!item1->isTemplate()) && (!item2->isTemplate())));
//        if (sameType)
//            return item1->data(Constants::Data_Label).toString() < item2->data(Constants::Data_Label).toString();
//        return item2->isTemplate();
//    }

private:
    TreeItem *m_Parent;
    QList<TreeItem*> m_Children;
    QHash<int, QVariant> m_Datas;
    QVector<int> m_DirtyRows;
    bool m_IsTemplate, m_IsModified;
};


class SimpleCategoryModelPrivate
{
public:
    SimpleCategoryModelPrivate(SimpleCategoryModel *parent) :
            m_Root(0),
            q(parent)
    {
        // create root item
        QHash<int, QVariant> datas;
        datas.insert(Constants::CATEGORY_ID, -1);
        datas.insert(Constants::CATEGORY_LABEL_ID, "ROOT");
        datas.insert(Constants::CATEGORY_PARENT, -1);
        m_Root = new TreeItem(datas, 0);
    }

    ~SimpleCategoryModelPrivate()
    {
        if (m_Root) {
            delete m_Root; m_Root=0;
        }
    }

    TreeItem *getItem(const QModelIndex &index) const
    {
        if (index.isValid()) {
            TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
            if (item) return item;
        }
        return m_Root;
    }

    void getCategories()
    {
        // Get all categories from database
        QSqlQuery query(base()->database());
        QString req;
        if (!mime.isEmpty()) {
            QHash<int, QString> where;
            where.insert(Constants::CATEGORY_MIME, QString("='%1'").arg(mime));
            req = base()->select(Constants::Table_CATEGORIES, where);
        } else {
            req = base()->select(Constants::Table_CATEGORIES);
        }
        QHash<int, TreeItem *> items;
        if (query.exec(req)) {
            while (query.next()) {
                QHash<int, QVariant> datas;
                datas.insert(Constants::CATEGORY_ID, query.value(Constants::CATEGORY_ID));
                datas.insert(Constants::CATEGORY_MIME, query.value(Constants::CATEGORY_MIME));
                datas.insert(Constants::CATEGORY_PARENT, query.value(Constants::CATEGORY_PARENT));
                datas.insert(Constants::CATEGORY_LABEL_ID, query.value(Constants::CATEGORY_LABEL_ID));
                datas.insert(Constants::CATEGORY_THEMEDICON, query.value(Constants::CATEGORY_THEMEDICON));
                items.insert(query.value(Constants::CATEGORY_ID).toInt(), new TreeItem(datas, 0));
            }
        } else {
            Utils::Log::addQueryError(q,query,__FILE__,__LINE__);
        }

        // Reparent items
        foreach(TreeItem *item, items.values()) {
            TreeItem *parent = items.value(item->data(Constants::CATEGORY_PARENT).toInt(), 0);
            if (!parent) {
                parent = m_Root;
            }
            item->setParent(parent);
            parent->addChildren(item);
        }
    }

    void updateLabels(TreeItem *item = 0)
    {
        if (!item)
            item = m_Root;

        // Translate item
        if (item!=m_Root) {
            QSqlQuery query(base()->database());
            QHash<int, QString> where;
            where.insert(Constants::CATEGORYLABEL_ID, QString("=%1").arg(item->data(Constants::CATEGORY_LABEL_ID).toString()));
            where.insert(Constants::CATEGORYLABEL_LANG, QString("='%1'").arg(QLocale().name().left(2)));
            if (query.exec(base()->select(Constants::Table_CATEGORY_LABEL, where))) {
                if (query.next()) {
                    item->setData(TranslatedLabel, query.value(Constants::CATEGORYLABEL_VALUE));
                    item->setNonModified();
                }
            } else {
                Utils::Log::addQueryError(q, query, __FILE__, __LINE__);
            }
        }

        // Translate all children
        for(int i = 0; i < item->childCount(); ++i) {
            updateLabels(item->child(i));
        }
    }

    bool needToSave(TreeItem *item = 0)
    {
        if (!item)
            item = m_Root;

        if (item != m_Root) {
            if (item->dirtyRows().count() > 0)
                return true;

            if (item->data(Constants::CATEGORY_ID).toInt() == -2)
                return true;
        }

        for(int i = 0; i < item->childCount(); ++i) {
            if (needToSave(item->child(i)))
                return true;
        }
        return false;
    }

    bool save(TreeItem *item = 0)
    {
        if (!item)
            item = m_Root;

        // is newly created
        if (item->data(Constants::CATEGORY_ID).toInt() == -2) {
            qWarning() << "newly created" << item->data(Internal::TranslatedLabel);
            // Create the CATEGORY_LABEL
            QSqlQuery query(base()->database());
            int id = base()->max(Constants::Table_CATEGORY_LABEL, Constants::CATEGORYLABEL_ID);
            ++id;
            QString req;
            req = base()->prepareInsertQuery(Constants::Table_CATEGORY_LABEL);
            if (!query.prepare(req)) {
                Utils::Log::addQueryError(q, query, __FILE__, __LINE__);
            }
            query.bindValue(Constants::CATEGORYLABEL_ID, id);
            query.bindValue(Constants::CATEGORYLABEL_LANG, QLocale().name().left(2));
            query.bindValue(Constants::CATEGORYLABEL_VALUE, item->data(Internal::TranslatedLabel).toString());
            if (!query.exec()) {
                Utils::Log::addQueryError(q, query, __FILE__, __LINE__);
                return false;
            }
            item->setData(Constants::CATEGORYLABEL_ID, id);
            item->setNonModified();
            query.finish();

            // Create the CATEGORY
            req = base()->prepareInsertQuery(Constants::Table_CATEGORIES);
            if (!query.prepare(req)) {
                Utils::Log::addQueryError(q, query, __FILE__, __LINE__);
            }
            query.bindValue(Constants::CATEGORY_ID, QVariant());
            query.bindValue(Constants::CATEGORY_LABEL_ID, id);
            query.bindValue(Constants::CATEGORY_MIME, mime);
            query.bindValue(Constants::CATEGORY_PARENT, item->data(Constants::CATEGORY_PARENT));
            query.bindValue(Constants::CATEGORY_THEMEDICON, QVariant());
            if (!query.exec()) {
                Utils::Log::addQueryError(q, query, __FILE__, __LINE__);
                return false;
            }

        }

        // is dirty ?
        if (item->dirtyRows().count()) {
            // Update CATEGORY_LABEL using lang, label_id
            QSqlQuery query(base()->database());
            QHash<int, QString> where;
            QString req;
            where.insert(Constants::CATEGORYLABEL_ID, QString("='%1'").arg(item->data(Constants::CATEGORY_LABEL_ID).toString()));
            where.insert(Constants::CATEGORYLABEL_LANG, QString("='%1'").arg(QLocale().name().left(2)));
            req = base()->prepareUpdateQuery(Constants::Table_CATEGORY_LABEL,
                                             QList<int>()
                                             << Constants::CATEGORYLABEL_VALUE,
                                             where);
            if (!query.prepare(req)) {
                Utils::Log::addQueryError(q, query, __FILE__, __LINE__);
            }
            query.bindValue(0, item->data(Internal::TranslatedLabel).toString());
            if (!query.exec()) {
                Utils::Log::addQueryError(q, query, __FILE__, __LINE__);
                return false;
            }
            item->setNonModified();
        }

        // test children
        for(int i = 0; i < item->childCount(); ++i) {
            if (!save(item->child(i)))
                return false;
        }
        return true;
    }


public:
    TreeItem *m_Root;
    QString mime;

private:
    SimpleCategoryModel *q;

};

}  // End namespace Internal
}  // End namespace Views



SimpleCategoryModel::SimpleCategoryModel(const QString &mime, QObject *parent) :
        QAbstractItemModel(parent), d(new Internal::SimpleCategoryModelPrivate(this))
{
    // Check database
    base();

    // Get categories
    d->mime = mime;
    d->getCategories();
    d->updateLabels();

    // Connect to language change
    connect(Core::ICore::instance()->translators(), SIGNAL(languageChanged()), this, SLOT(updateLabels()));
}

SimpleCategoryModel::~SimpleCategoryModel()
{
    qWarning() << "SimpleCategoryModel::~SimpleCategoryModel()";
    if (d->needToSave()) {
        qWarning() << "save";
        d->save();
    }
    if (d)
        delete d;
    d = 0;
}

QModelIndex SimpleCategoryModel::index(int row, int column, const QModelIndex &parent) const
 {
     if (parent.isValid() && parent.column() != 0)
         return QModelIndex();

//     if (!parent.isValid())
//         return QModelIndex();

     Internal::TreeItem *parentItem = d->getItem(parent);
     Internal::TreeItem *childItem = parentItem->child(row);

     if (childItem) {
         return createIndex(row, column, childItem);
     }
     return QModelIndex();
 }

QModelIndex SimpleCategoryModel::parent(const QModelIndex &index) const
 {
     if (!index.isValid())
         return QModelIndex();

     Internal::TreeItem *childItem = d->getItem(index);
     Internal::TreeItem *parentItem = childItem->parent();

     if (parentItem == d->m_Root)
         return QModelIndex();

     return createIndex(parentItem->childNumber(), 0, parentItem);
 }

int SimpleCategoryModel::rowCount(const QModelIndex &parent) const
{
    Internal::TreeItem *item = d->getItem(parent);
    if (item) {
        return item->childCount();
    }
    return 0;
}

int SimpleCategoryModel::columnCount(const QModelIndex &) const
{
    return ColumnCount;
}

void SimpleCategoryModel::fetchMore(const QModelIndex &parent)
{
}

bool SimpleCategoryModel::canFetchMore(const QModelIndex &parent) const
{
    return false;
}

QVariant SimpleCategoryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.column() == EmptyColumn)
        return QVariant();

    const Internal::TreeItem *it = d->getItem(index);

    if (role==Qt::DisplayRole || role==Qt::EditRole) {
        if (index.column() == Label)
            return it->data(Internal::TranslatedLabel);
    } else if (role==Qt::DecorationRole) {
        if (index.column() == Label) {
            if (!it->data(Constants::CATEGORY_THEMEDICON).toString().isEmpty())
                return theme()->icon(it->data(Constants::CATEGORY_THEMEDICON).toString()).pixmap(16,16);
        }
    }

    return QVariant();
}

bool SimpleCategoryModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    if (index.column() == EmptyColumn)
        return false;

    Internal::TreeItem *it = d->getItem(index);

    if (role==Qt::EditRole || role==Qt::DisplayRole) {
        switch (index.column()) {
        case Label: it->setData(Internal::TranslatedLabel, value);
        }
    }

    return true;
}

Qt::ItemFlags SimpleCategoryModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

bool SimpleCategoryModel::insertRows(int row, int count, const QModelIndex &parent)
{
    qWarning() << "insertRows" << row << count << parent;
    Internal::TreeItem *parentItem = 0;

    if (!parent.isValid())
        parentItem = d->m_Root;
    else
        parentItem = d->getItem(parent);

    beginInsertRows(parent, row, row+count-1);
    for(int i=0; i<count; ++i) {
        QHash<int, QVariant> datas;
        datas.insert(Constants::CATEGORY_ID, -2);
        datas.insert(Constants::CATEGORY_MIME, d->mime);
        datas.insert(Constants::CATEGORY_PARENT, parentItem->data(Constants::CATEGORY_ID));
        datas.insert(Constants::CATEGORY_LABEL_ID, -1);
        datas.insert(Constants::CATEGORY_THEMEDICON, QVariant());
        new Internal::TreeItem(datas, parentItem);
        qWarning() << "adding to" << parentItem->data(Internal::TranslatedLabel).toString() << datas.value(Constants::CATEGORY_PARENT);
    }
    endInsertRows();
//    reset();
    return true;
}

bool SimpleCategoryModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Internal::TreeItem *parentItem = 0;
    if (!parent.isValid())
        parentItem = d->m_Root;
    else
        parentItem = d->getItem(parent);

//    beginRemoveRows(parent, row, row+count-1);
//    for(int i=0; i<count; ++i) {
//        Internal::TreeItem *item = parentItem->child(row+i);
//        int id = item->id();
//        if (item->isTemplate() && (!d->m_TemplatesToDelete.contains(id)))
//            d->m_TemplatesToDelete.append(id);
//        else if ((!item->isTemplate())  && (!d->m_CategoriesToDelete.contains(id)))
//            d->m_CategoriesToDelete.append(id);
//        parentItem->removeChild(item);
//        delete item;
//        item = 0;
//    }
//    endRemoveRows();

//    qWarning() << "removeRows" << d->m_CategoriesToDelete << d->m_TemplatesToDelete;
    return true;
}

bool SimpleCategoryModel::submit()
{
    if (d->needToSave()) {
        qWarning() << "save";
        d->save();
    }
    return true;
}
void SimpleCategoryModel::updateLabels()
{
    d->updateLabels();
}
