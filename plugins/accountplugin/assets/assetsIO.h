#ifndef ASSETSIO_H
#define ASSETSIO_H

#include <QObject>
#include <QVariant>
#include <QHash>
#include <QStandardItemModel>

// #include "../../accountbaseplugin/constants.h"//<accountbaseplugin/constants.h>  A eviter+++

// #include "../../accountbaseplugin/assetmodel.h"
// NON à la place déclare la class comme ça et met le include dans le cpp
// Intérêt == si modification du assetModel header pas de recompilation de cet objet avec ma méthode
// alors qu'il aurait été recompilé avec ton include.

namespace AccountDB {
class AssetModel;
}

class AssetsIO : public QObject
{
    Q_OBJECT
public:
    AssetsIO(QObject *parent);
    ~AssetsIO();
    AccountDB::AssetModel *getModelAssets();
    QString getUserUid();
    bool insertIntoAssets(QHash<int,QVariant> &hashValues);
    bool insertIntoMovements(QHash<int,QVariant> &hashValues);
    bool deleteAsset(int row);
    QStandardItemModel * getListsOfValuesForRefresh(QObject * parent);
    QStandardItemModel * getBankComboBoxModel(QObject * parent);
    int getLastMovementId();
    bool deleteMovement(int idMovement, int idBank);    
    int getMovementId(int row);
    int getIdFromBankName(const QString & bankName);
    double getResidualValueWhenRefresh(int & row);
    bool deleteOneYearToRun(int & row);
    double getRate(QDate & date,double & duration);
    QStandardItemModel * getYearlyValues(const QDate & year,QObject * parent);
    double getValueFromRow(int & row);
    int getModeFromRow(int & row);
    double getDurationFromRow(int & row);
    QDate getDateFromRow(int & row);
    QString getLabelFromRow(int & row);
private:
    QString getBankNameFromId(int id);
    bool debitOrCreditInBankBalance(const QString & bank, double & value);
    bool creditValueDeletedToBankAccount(double & value, int & idBank);
    AccountDB::AssetModel *m_assetModel;
    QString m_user_uid;
};


#endif
