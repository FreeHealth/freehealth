#ifndef LEDGERIO_H
#define LEDGERIO_H
#include "../../accountbaseplugin/constants.h"//<accountbaseplugin/constants.h>
#include "../../accountbaseplugin/accountmodel.h"//<accountbaseplugin/availablemovementmodel>
#include "../../accountbaseplugin/movementmodel.h"//<accountbaseplugin/movementmodel>
#include <QObject>
#include <QAbstractTableModel>
#include <QStringList>
#include <QStandardItemModel>
#include <QVector>

using namespace AccountDB;
using namespace Constants;
class LedgerIO : public QObject {
    Q_OBJECT
    public :
        LedgerIO(QObject * parent);
        ~LedgerIO();
        QStringList getListOfYears();
        AccountModel * getModelMonthlyReceiptsIO(QObject * parent,QString & month , QString & year);
        QStringList getTypesByMonth(QString & month,QString & year);
        int getNbrOfRowsByTypeAndMonth(QString & month,QString & year,QString & type);
        QStringList listOfTypesByYear(QString & year);
        int getNbrOfRowsByTypeAndYear(QString & year,QString & type);
        double getYearlyReceiptsSum(QObject * parent,QString & year);
        
        MovementModel * getModelMonthlyMovementsIO(QObject * parent,QString & month, QString & year);
        QStandardItemModel * getModelMonthlyAndTypeMovementsIO(QObject * parent,QString & month, QString & year);
        QStandardItemModel * getModelYearlyAndTypeMovementsIO(QObject * parent,QString & year);
        
        double getMovementSum(MovementModel * model);
        //ledger
        QStringList getListOfSumsMonthlyReceiptsIO(QObject * parent,QString & dateBegin , QString & dateEnd);
        QStringList getListOfSumsMonthlyMovementsIO(QObject * parent,QString & dateBegin , QString & dateEnd);
        QStringList listOfReceiptsTypes();
        QStringList listOfMovementsTypes();
        QList<QVector<QString> > getDatasReceitsInVector(QString & dateBegin,QString & dateEnd);
        QList<QVector<QString> > getDatasMovementsInVector(QString & dateBegin,QString & dateEnd);
    private :
        AccountModel * m_accountModel;
        MovementModel * m_movementModel;
};

#endif

