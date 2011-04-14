#ifndef MOVEMENTSIO_H
#define MOVEMENTSIO_H

#include <accountplugin/account_exporter.h>

#include <QStandardItemModel>
#include <QHash>

namespace AccountDB {
    class MovementModel;
}

class ACCOUNT_EXPORT MovementsIODb : public QObject
{
    Q_OBJECT
public:
    MovementsIODb(QObject *parent);
    ~MovementsIODb();

    AccountDB::MovementModel *getModelMovements(QString &year);
    QStandardItemModel *getMovementsComboBoxModel(QObject *parent);

    QStringList getYearComboBoxModel();
    QStandardItemModel *getBankComboBoxModel(QObject * parent);

    bool insertIntoMovements(QHash<int,QVariant> &hashValues);
    bool deleteMovement(int row);
    bool validMovement(int row);
    int getAvailableMovementId(QString & movementsComboBoxText);
    int getTypeOfMovement(QString & movementsComboBoxText);
    int getBankId(QString & bankComboBoxText);
    QString getBankNameFromId(int id);
    QString getUserUid();
    QHash<QString,QString> hashChildrenAndParentsAvailableMovements();
    bool containsFixAsset(int & row);

private:
    QStringList listOfParents();
    bool debitOrCreditInBankBalance(const QString & bank, double & value);
    AccountDB::MovementModel *m_modelMovements;
    QString m_user_uid;
};

#endif

