#ifndef PMHEPISODEMODEL_H
#define PMHEPISODEMODEL_H

#include <QAbstractListModel>

namespace PMH {
namespace Internal {
class PmhEpisodeModelPrivate;
class PmhData;
}  // End namespace Internal


class PmhEpisodeModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum DataRepresentation {
        DateStart = 0,
        DateEnd,
        Label,
        IcdCodeList,
        IcdXml,
        Contact,
        EmptyColumn,
        ColumnCount
    };

    explicit PmhEpisodeModel(Internal::PmhData *pmh, QObject *parent = 0);
    ~PmhEpisodeModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    Qt::ItemFlags flags(const QModelIndex &index) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

private:
    Internal::PmhEpisodeModelPrivate *d;
};


}  // End namespace PMH


#endif // PMHEPISODEMODEL_H
