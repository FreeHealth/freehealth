#ifndef MOVEMENTSIO_H
#define MOVEMENTSIO_H

#include <QStandardItemModel>
#include <QHash>

namespace AccountDB {
    class MovementModel;
}

class MovementsIODb : public QObject
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
    QString getUserUid();
    QHash<QString,QString> hashChildrenAndParentsAvailableMovements();

private:
    QStringList listOfParents();
    AccountDB::MovementModel *m_modelMovements;
};

#endif

