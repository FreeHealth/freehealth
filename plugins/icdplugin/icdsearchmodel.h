#ifndef ICDSEARCHMODEL_H
#define ICDSEARCHMODEL_H

#include <QAbstractTableModel>

namespace ICD {
namespace Internal {
class IcdSearchModelPrivate;
}

class IcdSearchModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum DataRepresentation {
        SID_Code = 0,
        ICD_Code,
        ICD_CodeWithDagetAndStar,
        Label,
        Daget,
        Type,
        ColumnCount
    };

    enum SearchModes {
        SearchByLabel,
        SearchByCode
    };

    explicit IcdSearchModel(QObject *parent = 0);
    ~IcdSearchModel();

    void setSearchMethod(SearchModes mode);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    void fetchMore(const QModelIndex &parent);
    bool canFetchMore(const QModelIndex &parent) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const;

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

public Q_SLOTS:
    void setFilter(const QString &searchLabel);

private Q_SLOTS:
    void databaseInitialized();

private:
    Internal::IcdSearchModelPrivate *d;

};

}  // End namespace ICD

#endif // ICDSEARCHMODEL_H
