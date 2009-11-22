#ifndef GLOBALDRUGSMODEL_H
#define GLOBALDRUGSMODEL_H

#include <QSqlTableModel>
#include <QObject>

namespace DrugsDB {
namespace Internal {
class GlobalDrugsModelPrivate;
}  // end namespace Internal


class GlobalDrugsModel : public QSqlTableModel
{
    Q_OBJECT
    GlobalDrugsModel(QObject * parent = 0);
public:
    static GlobalDrugsModel *instance(QObject * parent = 0);

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) { return false; }
    QVariant data(const QModelIndex & item, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

public Q_SLOTS:
    void updateCachedAvailableDosage();

private:
    static GlobalDrugsModel *m_Instance;
    Internal::GlobalDrugsModelPrivate *d;
};


}  // end namespace DrugsDB


#endif // GLOBALDRUGSMODEL_H
