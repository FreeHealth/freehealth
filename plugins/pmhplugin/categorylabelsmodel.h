#ifndef CATEGORYLABELSMODEL_H
#define CATEGORYLABELSMODEL_H

#include <QAbstractTableModel>


namespace PMH {
namespace Internal {
class CategoryLabelsModelPrivate;
class PmhCategory;
}  // End namespace Internal


class CategoryLabelsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum DataRepresentation {
        Label = 0,
        Lang,
        ColumnCount
    };

    explicit CategoryLabelsModel(QObject *parent = 0);
    ~CategoryLabelsModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex & = QModelIndex()) const {return ColumnCount;}

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    Qt::ItemFlags flags(const QModelIndex &index) const;

    // Data
    bool setPmhCategoryData(Internal::PmhCategory *cat);
    Internal::PmhCategory *pmhCategory() const;

private:
    Internal::CategoryLabelsModelPrivate *d;
};

}  // End namespace PMH


#endif // CATEGORYLABELSMODEL_H
