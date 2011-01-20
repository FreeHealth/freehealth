#ifndef PMHCATEGORYMODEL_H
#define PMHCATEGORYMODEL_H

#include <QAbstractItemModel>

namespace PMH {
namespace Internal {
class PmhCategoryModelPrivate;
}  // End namespace Internal

class PmhCategoryModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum DataRepresentation {
        Label = 0,
        Type,
        Id,
        EmptyColumn,
        ColumnCount
    };

    explicit PmhCategoryModel(QObject *parent = 0);
    ~PmhCategoryModel();

    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &index) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    void fetchMore(const QModelIndex &parent);
    bool canFetchMore(const QModelIndex &parent) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    Qt::ItemFlags flags(const QModelIndex &index) const;

private:
    Internal::PmhCategoryModelPrivate *d;
};

}  // End namespace PMH


#endif // PMHCATEGORYMODEL_H
