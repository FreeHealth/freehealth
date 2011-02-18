#include "pmhcategoryonlyproxymodel.h"
#include "pmhcategorymodel.h"

#include <QItemSelectionRange>
#include <QDebug>

using namespace PMH;
using namespace Internal;

namespace PMH {
namespace Internal {
class PmhCategoryOnlyModelPrivate
{
public:
    PmhCategoryOnlyModelPrivate() : m_PmhModel(0) {}
    ~PmhCategoryOnlyModelPrivate() {}

public:
    PmhCategoryModel *m_PmhModel;
    QMap<QPersistentModelIndex, QPersistentModelIndex> mapping;
    QMap<QPersistentModelIndex, QPersistentModelIndex> proxySourceParent;
    bool m_HidePmh;
};

}  // End namespace Internal
}  // End namespace PMH


PmhCategoryOnlyModel::PmhCategoryOnlyModel(PmhCategoryModel *parent) :
        QAbstractProxyModel(parent), d(new PmhCategoryOnlyModelPrivate)
{
    d->m_PmhModel = parent;
    d->m_HidePmh = true;
    setSourceModel(parent);
    updateModel();
    connect(parent, SIGNAL(modelReset()), this, SLOT(updateModel()));
    connect(parent, SIGNAL(layoutChanged()), this, SLOT(updateModel()));
}

PmhCategoryOnlyModel::~PmhCategoryOnlyModel()
{
    delete d; d=0;
}

int PmhCategoryOnlyModel::rowCount(const QModelIndex &parent) const
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

int PmhCategoryOnlyModel::columnCount(const QModelIndex &index) const
{
    return 1; // d->m_PmhModel->columnCount(index); // Causes views to be non selectables...
}

QModelIndex PmhCategoryOnlyModel::index(int row, int column, const QModelIndex &parent) const
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

QModelIndex PmhCategoryOnlyModel::parent(const QModelIndex &child) const
{
    QModelIndex mi = d->proxySourceParent.value(child);
    if (mi.isValid())
        return mapFromSource(mi);
    return QModelIndex();
}

QModelIndex PmhCategoryOnlyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if (!proxyIndex.isValid())
        return QModelIndex();
    return d->mapping.key(proxyIndex);
}

QModelIndex PmhCategoryOnlyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    if (!sourceIndex.isValid())
        return QModelIndex();
    return d->mapping.value(sourceIndex);
}

//QItemSelection PmhCategoryOnlyModel::mapSelectionFromSource(const QItemSelection &sourceSelection) const
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

//QItemSelection PmhCategoryOnlyModel::mapSelectionToSource(const QItemSelection &proxySelection) const
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

//QVariant PmhCategoryOnlyModel::headerData(int section, Qt::Orientation orientation, int role) const
//{
//    return section;
//}

//Qt::ItemFlags PmhCategoryOnlyModel::flags(const QModelIndex &index) const
//{
//    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
//}

void PmhCategoryOnlyModel::hidePmh(bool hide)
{
    d->m_HidePmh = hide;
    // Now we set up the proxy <-> source mappings
    emit layoutAboutToBeChanged();
    updateModel();
    emit layoutChanged();
}

void PmhCategoryOnlyModel::updateBranch(QModelIndex &rootIndex)
{
    if (d->m_PmhModel->isCategory(rootIndex)) {
        QModelIndex proxy = createIndex(rootIndex.row(), rootIndex.column(), rootIndex.internalPointer());
        d->mapping.insert(QPersistentModelIndex(rootIndex), proxy);
        QModelIndex sourceParent;
        if (rootIndex.parent().isValid())
            sourceParent = rootIndex.parent();
        d->proxySourceParent.insert(proxy, sourceParent);
    }
    for(int i = 0; i < d->m_PmhModel->rowCount(rootIndex); ++i) {
        QModelIndex idx = d->m_PmhModel->index(i, 0, rootIndex);
        updateBranch(idx);
    }
}

void PmhCategoryOnlyModel::updateModel()
{
    d->mapping.clear();
    d->proxySourceParent.clear();
    for (int i = 0; i<d->m_PmhModel->rowCount(); ++i) {
        QModelIndex idx = d->m_PmhModel->index(i, 0);
        updateBranch(idx);
//        if (hideThem) {
//            if (!si->text().startsWith("A") || !si->parent())
//                continue;
//            QModelIndex proxy = createIndex(si->row(), si->column(), si->index().internalPointer());
//            mapping.insert(QPersistentModelIndex(si->index()), proxy);
//            QModelIndex sourceParent;
//            if (si->parent()->parent())
//                sourceParent = si->parent()->index();
//            proxySourceParent.insert(proxy, sourceParent);
//        } else {
//            QModelIndex proxy = createIndex(si->row(), si->column(), si->index().internalPointer());
//            mapping.insert(QPersistentModelIndex(si->index()), proxy);
//            QModelIndex sourceParent;
//            if (si->parent())
//                sourceParent = si->parent()->index();
//            proxySourceParent.insert(proxy, sourceParent);
//        }
    }
}

