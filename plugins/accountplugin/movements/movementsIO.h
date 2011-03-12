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

    bool insertIntoMovements(QHash<int,QVariant> &hashValues);
    bool deleteMovement(int row);
    bool validMovement(int row);
    int getAvailableMovementId(QString &movementsComboBoxText);
    QString getUserUid();

private:
    QStringList listOfParents();
    AccountDB::MovementModel *m_modelMovements;
};

#endif

