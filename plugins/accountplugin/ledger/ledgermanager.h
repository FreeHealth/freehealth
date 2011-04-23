#ifndef LEDGERMANAGER_H
#define LEDGERMANAGER_H
#include "ledgerIO.h"
#include <QObject>
#include <QStandardItemModel>


class LedgerManager : public QObject {
    Q_OBJECT
    enum MovAnalysisValues {
        MOVEMENT_LABEL = 0,
        MOVEMENT_VALUE
        };
    public :
        LedgerManager(QObject * parent);
        ~LedgerManager();
        AccountModel * getModelMonthlyReceiptsAnalysis(QObject * parent,QString & month , QString & year);
        QStandardItemModel * getModelMonthlyAndTypeReceiptsAnalysis(QObject * parent,QString & month,QString & year);
        QStandardItemModel * getModelYearlyAndTypeReceiptsAnalysis(QObject * parent,QString & year);
        
        MovementModel * getModelMonthlyMovementsAnalysis(QObject * parent,QString & month , QString & year);
        QStandardItemModel * getModelMonthlyAndTypeMovementAnalysis(QObject * parent,QString & month,QString & year);
        QStandardItemModel * getModelYearlyAndTypeMovementAnalysis(QObject * parent,QString & year);
        
        QStringList getListOfYears();
        QStringList getListOfMonths();
        double m_sums;
       
};

#endif

