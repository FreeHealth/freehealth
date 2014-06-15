/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "categoryonlyproxymodel.h"
#include "icategorymodelhelper.h"
#include "categoryitem.h"

#include <translationutils/constanttranslations.h>

#include <QItemSelectionRange>
#include <QDebug>

using namespace Category;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace Category {
namespace Internal {
class CategoryOnlyProxyModelPrivate
{
public:
    CategoryOnlyProxyModelPrivate() : m_Model(0) {}
    ~CategoryOnlyProxyModelPrivate() {}

public:
    ICategoryModelHelper *m_Model;
    QMap<QPersistentModelIndex, QPersistentModelIndex> mapping;
    QMap<QPersistentModelIndex, QPersistentModelIndex> proxySourceParent;
    bool m_HidePmh;
};

}  // End namespace Internal
}  // End namespace PMH


CategoryOnlyProxyModel::CategoryOnlyProxyModel(ICategoryModelHelper *parent) :
        QAbstractProxyModel(parent), d(new CategoryOnlyProxyModelPrivate)
{
    d->m_Model = parent;
    d->m_HidePmh = true;
    setSourceModel(parent);
    updateModel();
    connect(parent, SIGNAL(modelReset()), this, SLOT(updateModel()));
    connect(parent, SIGNAL(layoutChanged()), this, SLOT(updateModel()));
    connect(parent, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(updateModel()));
    connect(parent, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(updateModel()));
    connect(parent, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)), this, SLOT(updateModel()));
    connect(parent, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(emitDataChanged(QModelIndex,QModelIndex)));
}

CategoryOnlyProxyModel::~CategoryOnlyProxyModel()
{
    if (d)
        delete d;
    d=0;
}

int CategoryOnlyProxyModel::rowCount(const QModelIndex &parent) const
{
    QModelIndex sourceParent;
    if (parent.isValid())
        sourceParent = mapToSource(parent);
    int count = 0;
    QMapIterator<QPersistentModelIndex, QPersistentModelIndex> it(d->proxySourceParent);
    while (it.hasNext()) {
        it.next();
        if (it.value() == sourceParent)
            count++;
    }
    return count;
}

int CategoryOnlyProxyModel::columnCount(const QModelIndex &) const
{
    return 1; // d->m_Model->columnCount(index); // Causes views to be non selectables...
}

QModelIndex CategoryOnlyProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    QModelIndex sourceParent;
    if (parent.isValid())
        sourceParent = mapToSource(parent);
    QMapIterator<QPersistentModelIndex, QPersistentModelIndex> it(d->proxySourceParent);
    while (it.hasNext()) {
        it.next();
        if (it.value() == sourceParent && it.key().row() == row &&
            it.key().column() == column)
            return it.key();
    }
    return QModelIndex();
}

QModelIndex CategoryOnlyProxyModel::parent(const QModelIndex &child) const
{
    QModelIndex mi = d->proxySourceParent.value(child);
    if (mi.isValid())
        return mapFromSource(mi);
    return QModelIndex();
}

QModelIndex CategoryOnlyProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if (!proxyIndex.isValid())
        return QModelIndex();
    return d->mapping.key(proxyIndex);
}

QModelIndex CategoryOnlyProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    if (!sourceIndex.isValid())
        return QModelIndex();
    return d->mapping.value(sourceIndex);
}

//QItemSelection CategoryOnlyProxyModel::mapSelectionFromSource(const QItemSelection &sourceSelection) const
//{
//    QItemSelection proxySelection;

//    if (!sourceModel())
//        return proxySelection;

//    QItemSelection::const_iterator it = sourceSelection.constBegin();
//    const QItemSelection::const_iterator end = sourceSelection.constEnd();
//    for ( ; it != end; ++it) {
//        Q_ASSERT(it->model() == sourceModel());
//        const QItemSelectionRange range(mapFromSource(it->topLeft()), mapFromSource(it->bottomRight()));
//        proxySelection.append(range);
//    }

//    return proxySelection;
//}

//QItemSelection CategoryOnlyProxyModel::mapSelectionToSource(const QItemSelection &proxySelection) const
//{
//    QItemSelection sourceSelection;

//    if (!sourceModel())
//      return sourceSelection;

//    QItemSelection::const_iterator it = proxySelection.constBegin();
//    const QItemSelection::const_iterator end = proxySelection.constEnd();
//    for ( ; it != end; ++it) {
//        Q_ASSERT(it->model() == this);
//        const QItemSelectionRange range(mapToSource(it->topLeft()), mapToSource(it->bottomRight()));
//        sourceSelection.append(range);
//    }

//    return sourceSelection;
//}

//QVariant CategoryOnlyProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
//{
//    return section;
//}

//Qt::ItemFlags CategoryOnlyProxyModel::flags(const QModelIndex &index) const
//{
//    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
//}

bool CategoryOnlyProxyModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(row);
    Q_UNUSED(count);
    Q_UNUSED(parent);
    return true;
}

bool CategoryOnlyProxyModel::insertRows(int row, int count, const QModelIndex &parent)
{
    for(int i=0; i < count; ++i) {
        Category::CategoryItem *cat = new Category::CategoryItem;
        cat->setParent(d->m_Model->categoryForIndex(mapToSource(parent)));
        cat->setData(CategoryItem::DbOnly_Mime, d->m_Model->mime());
        cat->setLabel(tkTr(Trans::Constants::FILENEW_TEXT).remove("&"), QLocale().name().left(2));
        d->m_Model->addCategory(cat, row+i, parent);
    }
    return true;
}

//void CategoryOnlyProxyModel::hidePmh(bool hide)
//{
//    d->m_HidePmh = hide;
//    // Now we set up the proxy <-> source mappings
//    Q_EMIT layoutAboutToBeChanged();
//    updateModel();
//    Q_EMIT layoutChanged();
//}

void CategoryOnlyProxyModel::updateBranch(QModelIndex &rootIndex)
{
    if (d->m_Model->isCategory(rootIndex)) {
        QModelIndex proxy = createIndex(rootIndex.row(), rootIndex.column(), rootIndex.internalPointer());
        d->mapping.insert(QPersistentModelIndex(rootIndex), proxy);
        QModelIndex sourceParent;
        if (rootIndex.parent().isValid())
            sourceParent = rootIndex.parent();
        d->proxySourceParent.insert(proxy, sourceParent);

        // Check children
        for(int i = 0; i < d->m_Model->rowCount(rootIndex); ++i) {
            QModelIndex idx = d->m_Model->index(i, 0, rootIndex);
            updateBranch(idx);
        }
    }
}

void CategoryOnlyProxyModel::updateModel()
{
    d->mapping.clear();
    d->proxySourceParent.clear();
    for (int i = 0; i<d->m_Model->rowCount(); ++i) {
        QModelIndex idx = d->m_Model->index(i, 0);
        updateBranch(idx);
    }
    Q_EMIT layoutChanged();
}

void CategoryOnlyProxyModel::emitDataChanged(const QModelIndex &left, const QModelIndex &right)
{
    Q_EMIT dataChanged(mapFromSource(left), mapFromSource(right));
}
