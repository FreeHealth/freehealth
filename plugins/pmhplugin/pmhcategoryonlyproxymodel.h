#ifndef PMHCATEGORYONLYPROXYMODEL_H
#define PMHCATEGORYONLYPROXYMODEL_H

#include <QAbstractProxyModel>

namespace PMH {
class PmhCategoryModel;
namespace Internal {
class PmhCategoryOnlyModelPrivate;

class PmhCategoryOnlyModel : public QAbstractProxyModel
{
    Q_OBJECT
public:
    PmhCategoryOnlyModel(PmhCategoryModel *parent);
    ~PmhCategoryOnlyModel();

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
//    QItemSelection mapSelectionFromSource(const QItemSelection &sourceSelection) const;
//    QItemSelection mapSelectionToSource(const QItemSelection &proxySelection) const;

//    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

//    Qt::ItemFlags flags(const QModelIndex &index) const;

    void hidePmh(bool hide);

private:
    void updateBranch(QModelIndex &rootIndex);

private Q_SLOTS:
    void updateModel();

private:
    PmhCategoryOnlyModelPrivate *d;
};

}  // End namespace Internal
}  // End namespace PMH


#endif // PMHCATEGORYONLYPROXYMODEL_H
