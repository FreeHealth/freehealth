#include "ledgermanager.h"


	
LedgerManager::LedgerManager(QObject * parent):m_sums(0.00){}

LedgerManager::~LedgerManager(){}

AccountModel * LedgerManager::getModelMonthlyReceiptsAnalysis(QObject * parent,QString & month , QString & year){
    LedgerIO lio(this);
    m_sums = 0.00;
    AccountModel * model = lio.getModelMonthlyReceiptsIO(this,month,year);
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

QSqlTableModel * LedgerManager::getModelMonthlyAndTypeReceiptsAnalysis(QObject * parent,
                                                                       QString & month , 
                                                                       QString & year){
    QSqlTableModel * model = new QSqlTableModel(this);
    m_sums = 0.00;
    LedgerIO lio(this);
    AccountModel * modelAccount = lio.getModelMonthlyReceiptsIO(this,month,year);
    for (int i = 0; i < modelAccount->rowCount(); i += 1)
    {
    	m_sums += modelAccount->data(modelAccount->index(i,ACCOUNT_CASHAMOUNT),Qt::DisplayRole).toDouble();
    	m_sums += modelAccount->data(modelAccount->index(i,ACCOUNT_CHEQUEAMOUNT),Qt::DisplayRole).toDouble();
    	m_sums += modelAccount->data(modelAccount->index(i,ACCOUNT_VISAAMOUNT),Qt::DisplayRole).toDouble();
    	m_sums += modelAccount->data(modelAccount->index(i,ACCOUNT_INSURANCEAMOUNT),Qt::DisplayRole).toDouble();
    	m_sums += modelAccount->data(modelAccount->index(i,ACCOUNT_OTHERAMOUNT),Qt::DisplayRole).toDouble();
    }
    QStringList listOfTypesByMonth;
    listOfTypesByMonth = lio.getTypesByMonth(month,year);
    for (int i = 0; i < listOfTypesByMonth.size(); i += 1)
    {
    	int nbrOfRowByType = lio.getNbrOfRowsByTypeAndMonth(month,year,listOfTypesByMonth[i]);
    	model->setData(model->index(i,0),listOfTypesByMonth[i],Qt::EditRole);
    	model->setData(model->index(i,1),nbrOfRowByType,Qt::EditRole);
    }
    return model;//type,number
}

QSqlTableModel * LedgerManager::getModelYearlyAndTypeReceiptsAnalysis(QObject * parent,QString & year){
    QSqlTableModel * model = new QSqlTableModel(this);
    m_sums = 0.00;
    LedgerIO lio(this);
    m_sums = lio.getYearlyReceiptsSum(this,year);
    QStringList listOfTypesByYear;
    listOfTypesByYear = lio.listOfTypesByYear(year);
    for (int i = 0; i < listOfTypesByYear.size(); i += 1)
    {
    	int nbrOfRowByType = lio.getNbrOfRowsByTypeAndYear(year,listOfTypesByYear[i]);
    	model->setData(model->index(i,0),listOfTypesByYear[i],Qt::EditRole);
    	model->setData(model->index(i,1),nbrOfRowByType,Qt::EditRole);
    }
    return model;//type,number
}

MovementModel * LedgerManager::getModelMonthlyMovementsAnalysis(QObject * parent,QString & month , QString & year){
    m_sums = 0.00;
    LedgerIO lio(this);
    MovementModel * model = lio.getModelMonthlyMovementsIO(this,month,year);
    m_sums = lio.getMovementSum(model);
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
    return model;
}

QStringList LedgerManager::getListOfYears(){
    LedgerIO lio(this);
    return lio.getListOfYears();
}

QStringList LedgerManager::getListOfMonths(){
    QStringList list;
    list << "O1" << "02"<< "03"<< "04"<< "05"<< "06"<< "07"<< "08"<< "09"<< "10"<< "11" << "12";
    return list;
}
