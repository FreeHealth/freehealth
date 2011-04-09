#include "ledgerIO.h"

#include <QDate>
#include <QDebug>

//todo : verify system of filter of accountmodel

LedgerIO::LedgerIO(QObject * parent){
    //qDebug() << __FILE__ << QString::number(__LINE__) << " IO constructeur 1" ;
    if ((m_accountModel = new AccountModel(parent)) == NULL)
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "AccountModel is null" ;
        }
    m_userUuid = m_accountModel->getUserUuid();
    if ((m_movementModel = new MovementModel(parent))== NULL)
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "MovementModel is null" ;
        }
    //qDebug() << __FILE__ << QString::number(__LINE__) << " IO constructeur 2" ;
}

LedgerIO::~LedgerIO(){}

QStringList LedgerIO::getListOfYears(){
    QStringList list;
    QString filter = QString("%1='%2'").arg("USER_UID",m_userUuid);
    m_accountModel->setFilter(filter);
    int rows = m_accountModel->rowCount();
    qDebug() << __FILE__ << QString::number(__LINE__) << " rows =" << QString::number(rows) ;
    for (int i = 0; i < rows; i += 1)
    {
    	QString dateS = m_accountModel->data(m_accountModel->index(i,ACCOUNT_DATE),Qt::DisplayRole).toString();
    	qDebug() << __FILE__ << QString::number(__LINE__) << " dateS =" << dateS ;
    	QString yearOfDate = dateS.split(" ").last();
    	list << yearOfDate;
    	qDebug() << __FILE__ << QString::number(__LINE__) << " yearOfDate = " << yearOfDate ;
    }
    qDebug() << __FILE__ << QString::number(__LINE__) << " listOfYears =" << QString::number(list.size()) ;
    list.removeDuplicates();
    return list;
}
    

AccountModel * LedgerIO::getModelMonthlyReceiptsIO(QObject * parent,QString & month , QString & year){
    QString dateBeginStr = year+"-"+month+"-01";
    QDate dateBegin = QDate::fromString(dateBeginStr,"yyyy-MM-dd");
    QDate dateMonthAfter;
    if (month.toInt() < 12)
    {
    	   dateMonthAfter = dateBegin.addMonths(1);
        }
    QString dateEndStr = year+"-"+month+"-"+QString::number(dateBegin.daysTo(dateMonthAfter));
    QString filter = QString("DATE BETWEEN '%1' AND '%2'").arg(dateBeginStr,dateEndStr);
    m_accountModel->setFilter(filter);
    qDebug() << __FILE__ << QString::number(__LINE__) << " filter  =" << m_accountModel->filter() ;
    qDebug() << __FILE__ << QString::number(__LINE__) << " rows =" << m_accountModel->rowCount() ;
    return m_accountModel;
}



QStringList LedgerIO::getTypesByMonth(QString & month,QString & year){
    QStringList list;
    QString dateBeginStr = year+"-"+month+"-01";
    QDate dateBegin = QDate::fromString(dateBeginStr,"yyyy-MM-dd");
    QDate dateMonthAfter;
    if (month.toInt() > 12)
    {
    	   dateMonthAfter = dateBegin.addMonths(1);
        }
    QString dateEndStr = year+"-"+month+"-"+QString::number(dateBegin.daysTo(dateMonthAfter));
    QString filter = QString("DATE BETWEEN '%1' AND '%2'").arg(dateBeginStr,dateEndStr);
    m_accountModel->setFilter(filter);
    int rowsNumber = m_accountModel->rowCount();
    for (int i = 0; i < rowsNumber; i += 1)
    {
    	QString dataType = m_accountModel->data(m_accountModel->index(i,ACCOUNT_MEDICALPROCEDURE_TEXT),Qt::DisplayRole)
    	                   .toString();
    	if (dataType.contains("+"))
    	{
    		  list << dataType.split("+");
    	    }
    	else{
    	          list << dataType;
    	}
    }
    list.removeDuplicates();
    return list;
}

int LedgerIO::getNbrOfRowsByTypeAndMonth(QString & month,QString & year,QString & type){
    int rows = 0;
    QString dateBeginStr = year+"-"+month+"-01";
    QDate dateBegin = QDate::fromString(dateBeginStr,"yyyy-MM-dd");
    QDate dateMonthAfter;
    if (month.toInt() > 12)
    {
    	   dateMonthAfter = dateBegin.addMonths(1);
        }
    QString dateEndStr = year+"-"+month+"-"+QString::number(dateBegin.daysTo(dateMonthAfter));
    QString filter = QString("DATE BETWEEN '%1' AND '%2'").arg(dateBeginStr,dateEndStr);
    m_accountModel->setFilter(filter);
    int rowsNumber = m_accountModel->rowCount();
    for (int i = 0; i < rowsNumber; i += 1)
    {
    	QString dataType = m_accountModel->data(m_accountModel->index(i,ACCOUNT_MEDICALPROCEDURE_TEXT),Qt::DisplayRole)
    	                  .toString();
    	if (dataType.contains("+"))
    	{
    		  QStringList list = dataType.split("+");
    		  QString str;
    		  foreach(str,list){
    		      if (str == type)
    		      {
    		      	  rows++;
    		          }
    		      }
    	    }
    	else{
    	    if (dataType == type)
    	    {
    	    	  rows++;
    	        }
    	}
    }
    return rows;
}

QStringList LedgerIO::listOfTypesByYear(QString & year){
    QStringList list;
    QString dateBeginStr = year+"-01-01";
    QString dateEndStr = year+"-12-31";
    QString filter = QString("DATE BETWEEN '%1' AND '%2'").arg(dateBeginStr,dateEndStr);
    m_accountModel->setFilter(filter);
    int rowsNumber = m_accountModel->rowCount();
    for (int i = 0; i < rowsNumber; i += 1)
    {
    	QString dataType = m_accountModel->data(m_accountModel->index(i,ACCOUNT_MEDICALPROCEDURE_TEXT),Qt::DisplayRole)
    	                   .toString();
    	if (dataType.contains("+"))
    	{
    		  list << dataType.split("+");
    	    }
    	else{
    	          list << dataType;
    	}
    }
    list.removeDuplicates();
    return list;
}
int LedgerIO::getNbrOfRowsByTypeAndYear(QString & year,QString & type){
    QStringList list;
    int rows = 0;
    QString dateBeginStr = year+"-01-01";
    QString dateEndStr = year+"-12-31";
    QString filter = QString("DATE BETWEEN '%1' AND '%2'").arg(dateBeginStr,dateEndStr);
    m_accountModel->setFilter(filter);
    int rowsNumber = m_accountModel->rowCount();
    for (int i = 0; i < rowsNumber; i += 1)
    {
    	QString dataType = m_accountModel->data(m_accountModel->index(i,ACCOUNT_MEDICALPROCEDURE_TEXT),Qt::DisplayRole)
    	                   .toString();
    	if (dataType.contains("+"))
    	{
    		  QStringList list = dataType.split("+");
    		  QString str;
    		  foreach(str,list){
    		      if (str == type)
    		      {
    		      	  rows++;
    		          }
    		      }
    	    }
    	else{
    	    if (dataType == type)
    	    {
    	    	  rows++;
    	        }
    	}
    }
    return rows;

}

double LedgerIO::getYearlyReceiptsSum(QObject * parent,QString & year){
    double totalValue = 0.00;
    QString dateBeginStr = year+"-01-01";
    QString dateEndStr = year+"-12-31";
    QString filter = QString("DATE BETWEEN '%1' AND '%2'").arg(dateBeginStr,dateEndStr);
    m_accountModel->setFilter(filter);
    int rows = 0;
    rows = m_accountModel->rowCount();    
    for (int i = 0; i < rows; i += 1)
    {
    	totalValue += m_accountModel->data(m_accountModel->index(i,ACCOUNT_CASHAMOUNT),Qt::DisplayRole).toDouble();
    	totalValue += m_accountModel->data(m_accountModel->index(i,ACCOUNT_CHEQUEAMOUNT),Qt::DisplayRole).toDouble();
    	totalValue += m_accountModel->data(m_accountModel->index(i,ACCOUNT_VISAAMOUNT),Qt::DisplayRole).toDouble();
    	totalValue += m_accountModel->data(m_accountModel->index(i,ACCOUNT_INSURANCEAMOUNT),Qt::DisplayRole).toDouble();
    	totalValue += m_accountModel->data(m_accountModel->index(i,ACCOUNT_OTHERAMOUNT),Qt::DisplayRole).toDouble();    	
    }
    return totalValue;
}

MovementModel * LedgerIO::getModelMonthlyMovementsIO(QObject * parent,QString & month, QString & year){
    QString dateBeginStr = year+"-"+month+"-01";
    QDate dateBegin = QDate::fromString(dateBeginStr,"yyyy-MM-dd");
    QDate dateMonthAfter;
    if (month.toInt() > 12)
    {
    	   dateMonthAfter = dateBegin.addMonths(1);
        }
    QString dateEndStr = year+"-"+month+"-"+QString::number(dateBegin.daysTo(dateMonthAfter));
    QString filter = QString("DATE BETWEEN '%1' AND '%2'").arg(dateBeginStr,dateEndStr);
    m_movementModel->setFilter(filter);
    return m_movementModel;
}

double LedgerIO::getMovementSum(MovementModel * model){
    double value = 0.00;
    int rows = model->rowCount();
    for (int i = 0; i < rows; i += 1)
    {
    	value += model->data(model->index(i,MOV_AMOUNT),Qt::DisplayRole).toDouble();
    }
    return value;
}

QStandardItemModel * LedgerIO::getModelMonthlyAndTypeMovementsIO(QObject * parent,QString & month, QString & year){
    QStandardItemModel * model = new QStandardItemModel(parent);
    QHash<QString,double> hash;
    int rows = 0;
    QString dateBeginStr = year+"-"+month+"-01";
    QDate dateBegin = QDate::fromString(dateBeginStr,"yyyy-MM-dd");
    QDate dateMonthAfter;
    if (month.toInt() > 12)
    {
    	   dateMonthAfter = dateBegin.addMonths(1);
        }
    QString dateEndStr = year+"-"+month+"-"+QString::number(dateBegin.daysTo(dateMonthAfter));
    QString filter = QString("DATE BETWEEN '%1' AND '%2'").arg(dateBeginStr,dateEndStr);
    m_movementModel->setFilter(filter);
    rows = m_movementModel->rowCount();
    for (int i = 0; i < rows; i += 1)
    {
    	QString labelOfMovement = m_movementModel->data(m_movementModel->index(i,MOV_LABEL),Qt::DisplayRole).toString();
    	double value = m_movementModel->data(m_movementModel->index(i,MOV_AMOUNT),Qt::DisplayRole).toDouble();
    	hash.insertMulti(labelOfMovement,value);
    }
    QStringList keysList = hash.uniqueKeys();
    QString keyStr;
    foreach(keyStr,keysList){
        double valueAssocWithKey = 0.00;
        QList<double> valuesList = hash.values(keyStr);
        for (int i = 0; i < valuesList.size(); i += 1)
        {
        	valueAssocWithKey += valuesList[i];
        }
        QStandardItem * itemLabel = new QStandardItem(keyStr);
        QStandardItem * itemAmount = new QStandardItem(QString::number(valueAssocWithKey));
        QList<QStandardItem*> items;
        items << itemLabel << itemAmount;
        model->appendRow(items);
    }
    return model;
}

QStandardItemModel * LedgerIO::getModelYearlyAndTypeMovementsIO(QObject * parent,QString & year){
    QStandardItemModel * model = new QStandardItemModel(parent);
    QHash<QString,double> hash;
    int rows = 0;
    QString dateBeginStr = year+"-01-01";
    QString dateEndStr = year+"-12-31";
    QString filter = QString("DATE BETWEEN '%1' AND '%2'").arg(dateBeginStr,dateEndStr);
    m_movementModel->setFilter(filter);
    rows = m_movementModel->rowCount();
    for (int i = 0; i < rows; i += 1)
    {
    	QString labelOfMovement = m_movementModel->data(m_movementModel->index(i,MOV_LABEL),Qt::DisplayRole).toString();
    	double value = m_movementModel->data(m_movementModel->index(i,MOV_AMOUNT),Qt::DisplayRole).toDouble();
    	hash.insertMulti(labelOfMovement,value);    	
    }
    QStringList keysList = hash.uniqueKeys();
    QString keyStr;
    foreach(keyStr,keysList){
        double valueAssocWithKey = 0.00;
        QList<double> valuesList = hash.values(keyStr);
        for (int i = 0; i < valuesList.size(); i += 1)
        {
        	valueAssocWithKey += valuesList[i];
        }
        QStandardItem * itemLabel = new QStandardItem(keyStr);
        QStandardItem * itemAmount = new QStandardItem(QString::number(valueAssocWithKey));
        QList<QStandardItem*> items;
        items << itemLabel << itemAmount;
        model->appendRow(items);
    }
    return model;
}
//for ledger
QStringList LedgerIO::getListOfSumsMonthlyReceiptsIO(QObject * parent,QString & dateBegin , QString & dateEnd){
    QStringList list;
    QString dateBeginStr = dateBegin;
    QString dateEndStr = dateEnd;
    QString filter = QString("DATE BETWEEN '%1' AND '%2'").arg(dateBeginStr,dateEndStr);
    AccountModel accountModel(parent);
    accountModel.setFilter(filter);
    double sum = 0.00;
    double cash = 0.00;
    double checks = 0.00;
    double creditCard = 0.00;
    double banking = 0.00;
    int rowsNumber = accountModel.rowCount();
    for (int i = 0; i < rowsNumber; i += 1)
    {
    	cash        += accountModel.data(accountModel.index(i,ACCOUNT_CASHAMOUNT),Qt::DisplayRole).toDouble();
    	checks      += accountModel.data(accountModel.index(i,ACCOUNT_CHEQUEAMOUNT),Qt::DisplayRole).toDouble();
    	creditCard  += accountModel.data(accountModel.index(i,ACCOUNT_VISAAMOUNT),Qt::DisplayRole).toDouble();
    	banking     += accountModel.data(accountModel.index(i,ACCOUNT_INSURANCEAMOUNT),Qt::DisplayRole).toDouble();
    	sum += cash+checks+creditCard+banking;
    }
    list << QString::number(sum)
         << QString::number(cash) 
         << QString::number(checks) 
         << QString::number(creditCard) 
         << QString::number(banking);
    return list;
}

QStringList LedgerIO::getListOfSumsMonthlyMovementsIO(QObject * parent,QString & dateBegin , QString & dateEnd){
    QStringList list;
    QHash<QString,double> hash;
    int rows = 0;
    QString dateBeginStr = dateBegin;
    QString dateEndStr = dateEnd;
    QString filter = QString("DATE BETWEEN '%1' AND '%2'").arg(dateBeginStr,dateEndStr);
    m_movementModel->setFilter(filter);
    rows = m_movementModel->rowCount();
    for (int i = 0; i < rows; i += 1)
    {
    	QString labelOfMovement = m_movementModel->data(m_movementModel->index(i,MOV_LABEL),Qt::DisplayRole).toString();
    	double value = m_movementModel->data(m_movementModel->index(i,MOV_AMOUNT),Qt::DisplayRole).toDouble();
    	hash.insertMulti(labelOfMovement,value);
    }
    QStringList keysList = hash.uniqueKeys();
    QString keyStr;
    foreach(keyStr,keysList){
        double valueAssocWithKey = 0.00;
        QList<double> valuesList = hash.values(keyStr);
        for (int i = 0; i < valuesList.size(); i += 1)
        {
        	valueAssocWithKey += valuesList[i];
        }
        QString strPair = keyStr+"="+QString::number(valueAssocWithKey);
        list << strPair;
    }
    return list;
}

QStringList LedgerIO::listOfReceiptsTypes(){
    QStringList list;
    AccountModel accountModel(this);
    //accountModel.setFilterUserUuid();
    accountModel.setFilter("");
    int rowsNumber = accountModel.rowCount();
    for (int i = 0; i < rowsNumber; i += 1)
    {
    	QString type = accountModel.data(accountModel.index(i,ACCOUNT_MEDICALPROCEDURE_TEXT),Qt::DisplayRole)
    	               .toString();
    	qDebug() << __FILE__ << QString::number(__LINE__) << " type =" << type ;
    	list << type;
    }
    return list;
}

QStringList LedgerIO::listOfMovementsTypes(){
    QStringList list;
    int rowsNumber = m_movementModel->rowCount();
    for (int i = 0; i < rowsNumber; i += 1)
    {
    	QString type = m_movementModel->data(m_movementModel->index(i,MOV_LABEL),Qt::DisplayRole)
    	               .toString();
    	list << type;
    }
    return list;    
}

QList<QVector<QString> > LedgerIO::getDatasReceiptsInVector(QString & dateBegin,QString & dateEnd){
    qDebug() << __FILE__ << QString::number(__LINE__) << " dateBegin =" << dateBegin ;
    qDebug() << __FILE__ << QString::number(__LINE__) << " dateEnd =" << dateEnd ;
    QList<QVector<QString> > tableLedgerMonth;
    QString filter = QString("DATE BETWEEN '%1' AND '%2'").arg(dateBegin,dateEnd);
    //QString filter = "DATE BETWEEN '2011-01-01' AND '2011-01-31'";
    AccountModel accountModel(this);
    accountModel.setFilter(filter);
    qDebug() << __FILE__ << QString::number(__LINE__) << " filter =" << accountModel.filter() ;
    int rows = accountModel.rowCount();
    qDebug() << __FILE__ << QString::number(__LINE__) << " rowCount =" << QString::number(rows) ;
    for (int i = 0; i < rows; i += 1)
    {
    	QString vectorDate = accountModel.data(accountModel.index(i,ACCOUNT_DATE),Qt::DisplayRole).toString();
        QString vectorPatient = accountModel.data(accountModel.index(i,ACCOUNT_PATIENT_NAME),Qt::DisplayRole)
                                                                                                     .toString();       
        QString vectorCash = accountModel.data(accountModel.index(i,ACCOUNT_CASHAMOUNT),Qt::DisplayRole)
    	                                                                                        .toString();
        QString vectorChecks = accountModel.data(accountModel.index(i,ACCOUNT_CHEQUEAMOUNT),Qt::DisplayRole)
                                                                                                   .toString();
        QString vectorVisa = accountModel.data(accountModel.index(i,ACCOUNT_VISAAMOUNT),Qt::DisplayRole).toString();
        QString vectorBanking = accountModel.data(accountModel.index(i,ACCOUNT_INSURANCEAMOUNT),Qt::DisplayRole)
                                                                                                         .toString();
        QString vectorActs = accountModel.data(accountModel.index(i,ACCOUNT_MEDICALPROCEDURE_TEXT),Qt::DisplayRole)
    	                                                                                         .toString();//acts
        qDebug() << __FILE__ << QString::number(__LINE__) << " vectorDate =" << vectorDate ;
        qDebug() << __FILE__ << QString::number(__LINE__) << " vectorPatient =" << vectorPatient ;
        QVector<QString> vector;
        vector << vectorDate 
               << vectorPatient 
               << vectorCash 
               << vectorChecks 
               << vectorVisa 
               << vectorBanking 
               << vectorActs;
        tableLedgerMonth << vector;
    }
//on a le tableau date,patient,esp,chq,cb,virement,acte du mois
    return tableLedgerMonth;
}

QList<QVector<QString> > LedgerIO::getDatasMovementsInVector(QString & dateBegin,QString & dateEnd){
    QList<QVector<QString> > tableLedgerMonth;
    QString filter = QString("DATE BETWEEN '%1' AND '%2'").arg(dateBegin,dateEnd);
    m_movementModel->setFilter(filter);
    int rows = m_movementModel->rowCount();
    for (int i = 0; i < rows; i += 1)
    {
    	QString vectorDate = m_movementModel->data(m_movementModel->index(i,MOV_DATE),Qt::DisplayRole).toString();
        QString vectorLabel = m_movementModel->data(m_movementModel->index(i,MOV_LABEL),Qt::DisplayRole).toString();
        QString vectorAmount = m_movementModel->data(m_movementModel->index(i,MOV_AMOUNT),Qt::DisplayRole).toString();
        QString vectorComment = m_movementModel->data(m_movementModel->index(i,MOV_COMMENT),Qt::DisplayRole).toString();
        QString vectorDetails = m_movementModel->data(m_movementModel->index(i,MOV_DETAILS),Qt::DisplayRole).toString();
        QVector<QString> vector;
        vector << vectorDate << vectorLabel << vectorAmount << vectorComment << vectorDetails;
        tableLedgerMonth << vector;
    }
    return tableLedgerMonth;
}

