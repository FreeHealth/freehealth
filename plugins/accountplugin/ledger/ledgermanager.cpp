#include "ledgermanager.h"
#include <QDebug>

	
LedgerManager::LedgerManager(QObject * parent):m_sums(0.00){}

LedgerManager::~LedgerManager(){}

AccountModel * LedgerManager::getModelMonthlyReceiptsAnalysis(QObject * parent,QString & month , QString & year){
    LedgerIO lio(this);
    m_sums = 0.00;
    AccountModel * model = lio.getModelMonthlyReceiptsIO(parent,month,year);
    //qDebug() << __FILE__ << QString::number(__LINE__) << "model->rowCount()  =" <<  QString::number(model->rowCount());
    for (int i = 0; i < model->rowCount(); i += 1)
    {
    	m_sums += model->data(model->index(i,ACCOUNT_CASHAMOUNT),Qt::DisplayRole).toDouble();
    	m_sums += model->data(model->index(i,ACCOUNT_CHEQUEAMOUNT),Qt::DisplayRole).toDouble();
    	m_sums += model->data(model->index(i,ACCOUNT_VISAAMOUNT),Qt::DisplayRole).toDouble();
    	m_sums += model->data(model->index(i,ACCOUNT_INSURANCEAMOUNT),Qt::DisplayRole).toDouble();
    	m_sums += model->data(model->index(i,ACCOUNT_OTHERAMOUNT),Qt::DisplayRole).toDouble();
    }
    return model;
}

QStandardItemModel * LedgerManager::getModelMonthlyAndTypeReceiptsAnalysis(QObject * parent,
                                                                       QString & month , 
                                                                       QString & year){
    QStandardItemModel * model = new QStandardItemModel(parent);
    m_sums = 0.00;
    LedgerIO lio(this);
    AccountModel * modelAccount = lio.getModelMonthlyReceiptsIO(parent,month,year);
    qDebug() << __FILE__ << QString::number(__LINE__) << "modelAccount->rowCount()  =" << QString::number(modelAccount->rowCount()) ;
    for (int i = 0; i < modelAccount->rowCount(); i += 1)
    {
    	m_sums += modelAccount->data(modelAccount->index(i,ACCOUNT_CASHAMOUNT),Qt::DisplayRole).toDouble();
    	m_sums += modelAccount->data(modelAccount->index(i,ACCOUNT_CHEQUEAMOUNT),Qt::DisplayRole).toDouble();
    	m_sums += modelAccount->data(modelAccount->index(i,ACCOUNT_VISAAMOUNT),Qt::DisplayRole).toDouble();
    	m_sums += modelAccount->data(modelAccount->index(i,ACCOUNT_INSURANCEAMOUNT),Qt::DisplayRole).toDouble();
    	m_sums += modelAccount->data(modelAccount->index(i,ACCOUNT_OTHERAMOUNT),Qt::DisplayRole).toDouble();
    }
    QStringList listOfTypesByMonth;
    listOfTypesByMonth = lio.getTypesByMonth(parent,month,year);
    qDebug() << __FILE__ << QString::number(__LINE__) << " listOfTypesByMonth.size() =" << QString::number(listOfTypesByMonth.size()) ;
    int listOfTypesSize = listOfTypesByMonth.size();
    for (int i = 0; i < listOfTypesSize; i += 1)
    {
    	int nbrOfRowByType = lio.getNbrOfRowsByTypeAndMonth(parent,month,year,listOfTypesByMonth[i]);
    	qDebug() << __FILE__ << QString::number(__LINE__) << " nbrOfRowByType =" << QString::number(nbrOfRowByType) ;
    	qDebug() << __FILE__ << QString::number(__LINE__) << "listOfTypesByMonth[i]  =" << listOfTypesByMonth[i] ;
    	QStandardItem * itemRows = new QStandardItem(QString::number(nbrOfRowByType));
    	QStandardItem * itemTypes = new QStandardItem(listOfTypesByMonth[i]);
        QList<QStandardItem*> list;
        list << itemTypes << itemRows;
        model->appendRow(list);
    }
    delete modelAccount;
    qDebug() << __FILE__ << QString::number(__LINE__) << " model->rowCount() =" << QString::number(model->rowCount()) ;
    return model;//type,number
}

QStandardItemModel * LedgerManager::getModelYearlyAndTypeReceiptsAnalysis(QObject * parent,QString & year){
    QStandardItemModel * model = new QStandardItemModel(parent);
    m_sums = 0.00;
    LedgerIO lio(this);
    m_sums = lio.getYearlyReceiptsSum(this,year);
    QStringList listOfTypesByYear;
    listOfTypesByYear = lio.listOfTypesByYear(year);
    for (int i = 0; i < listOfTypesByYear.size(); i += 1)
    {
    	int nbrOfRowByType = lio.getNbrOfRowsByTypeAndYear(parent,year,listOfTypesByYear[i]);
    	QStandardItem * itemRows = new QStandardItem(QString::number(nbrOfRowByType));
    	QStandardItem * itemTypes = new QStandardItem(listOfTypesByYear[i]);
        QList<QStandardItem*> list;
        list << itemTypes << itemRows;
        model->appendRow(list);
    }
    return model;//type,number
}

MovementModel * LedgerManager::getModelMonthlyMovementsAnalysis(QObject * parent,QString & month , QString & year){
    m_sums = 0.00;
    LedgerIO lio(this);
    MovementModel * model = lio.getModelMonthlyMovementsIO(parent,month,year);
    m_sums = lio.getMovementSum(parent,month,year);
    qDebug() << __FILE__ << QString::number(__LINE__) << " m_sums =" << QString::number(m_sums) ;
    return model;
}

QStandardItemModel * LedgerManager::getModelMonthlyAndTypeMovementAnalysis(QObject * parent,
                                                                           QString & month,
                                                                           QString & year){
    m_sums = 0.00;
    LedgerIO lio(this);
    QStandardItemModel * model = lio.getModelMonthlyAndTypeMovementsIO(this,month,year);
    int rows = model->rowCount();
    for (int i = 0; i < rows; i += 1)
    {
    	m_sums += model->data(model->index(i,MOVEMENT_VALUE),Qt::DisplayRole).toDouble();
    }
    qDebug() << __FILE__ << QString::number(__LINE__) << " m_sums =" << QString::number(m_sums) ;
    return  model;
}

QStandardItemModel * LedgerManager::getModelYearlyAndTypeMovementAnalysis(QObject * parent,QString & year){
    m_sums = 0.00;
    LedgerIO lio(this);
    QStandardItemModel * model = lio.getModelYearlyAndTypeMovementsIO(this,year);
    int rows = model->rowCount();
    for (int i = 0; i < rows; i += 1)
    {
    	m_sums += model->data(model->index(i,MOVEMENT_VALUE),Qt::DisplayRole).toDouble();
    }
    qDebug() << __FILE__ << QString::number(__LINE__) << " m_sums =" << QString::number(m_sums) ;
    return model;
}

QStringList LedgerManager::getListOfYears(){
    LedgerIO lio(this);
    return lio.getListOfYears();
}

QStringList LedgerManager::getListOfMonths(){
    QStringList list;
    list << "01" << "02"<< "03"<< "04"<< "05"<< "06"<< "07"<< "08"<< "09"<< "10"<< "11" << "12";
    return list;
}
