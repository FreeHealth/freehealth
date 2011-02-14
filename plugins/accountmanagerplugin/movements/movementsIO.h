#ifndef MOVEMENTSIO_H
#define MOVEMENTSIO_H
#include "../../accountbaseplugin/availablemovementmodel.h"
#include "../../accountbaseplugin/movementmodel.h"
#include <QStandardItemModel>
#include <QHash>

using namespace AccountDB;
class movementsIODb : public QObject{
    Q_OBJECT
    public :
        movementsIODb(QObject * parent);
        ~movementsIODb();
        MovementModel * getModelMovements();
        QStandardItemModel * getMovementsComboBoxModel(QObject * parent);
        QStringList getYearComboBoxModel();
        bool insertIntoMovements(QHash<int,QVariant> & hashValues);
        bool deleteMovement(int row);
        bool validMovement(int row);
        int getAvailableMovementId(QString & movementsComboBoxText);
    private :
        MovementModel * m_modelMovements;
};

#endif

