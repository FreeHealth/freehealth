#ifndef WORKINGPLACES_MODEL_H
#define WORKINGPLACES_MODEL_H

#include <QAbstractTableModel>
#include <QSqlError>


namespace AccountDB {
namespace Internal {
class WorkingPlacesModelPrivate;
}

class WorkingPlacesModel : public QAbstractTableModel
{
public:
    WorkingPlacesModel(QObject *parent);
    ~WorkingPlacesModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
//    bool hasChildren(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const;
//    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value,
//                               int role = Qt::EditRole);

    void setFilter(const QString & filter);

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    bool insertRow(int arow, const QModelIndex &aparent = QModelIndex())        { return insertRows(arow, 1, aparent); }
    bool insertColumn(int acolumn, const QModelIndex &aparent = QModelIndex())  { return insertColumns(acolumn, 1, aparent); }
    bool removeRow(int arow, const QModelIndex &aparent = QModelIndex())        { return removeRows(arow, 1, aparent); }
    bool removeColumn(int acolumn, const QModelIndex &aparent = QModelIndex())  { return removeColumns(acolumn, 1, aparent); }

    bool isDirty() const;
    QSqlError lastError();
    
public Q_SLOTS:
    bool submit();
    void revert();

private:
    Internal::WorkingPlacesModelPrivate *d;
};

}  // End namespace AccountDB


#endif // WORKINGPLACES_MODEL_H
