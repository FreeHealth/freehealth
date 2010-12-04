#ifndef AMOUNTMODEL_H
#define AMOUNTMODEL_H

#include <accountbaseplugin/accountbase_exporter.h>
#include <QAbstractTableModel>

namespace AccountDB {
class AccountData;

class ACCOUNTBASE_EXPORT AmountModel : public QAbstractTableModel
{
public:
    enum ColumnRepresentation {
        Col_Value = 0,
        Col_Currency,  // Devise monétaire
        ColCount
    };

    enum RowRepresentation {
        Row_Cash = 0,
        Row_Visa,
        Row_Cheque,
        Row_Banking,  // Virement banquaire
        Row_Insurance,
        Row_Other,
        Row_Du,
        RowCount
    };

    AmountModel(AccountData *data, QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const {return RowCount;}
    int columnCount(const QModelIndex &parent = QModelIndex()) const {return ColCount;}

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;

private:
    QList<float> m_Values;
    AccountData *m_AccountData;
};

}  //  End namespace AccountDB


#endif // AMOUNTMODEL_H
