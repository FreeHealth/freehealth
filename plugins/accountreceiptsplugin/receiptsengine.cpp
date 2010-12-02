#include "receiptsengine.h"
#include <accountbaseplugin/constants.h>

#include <QSqlDriver>
#include <QMessageBox>

#include <QDebug>

using namespace AccountDB;
using namespace Constants;



receiptsEngine::receiptsEngine(){
    m_mpmodel = new AccountModel(this);
    
    //m_db = QSqlDatabase::database("Account");
    //m_model = new QSqlTableModel(this,m_db);
    //m_model->setTable("account");
    //m_model->select();
    
}
/*  hash.insert(receiptsEngine::ACCOUNT_MEDICALPROCEDURE_TEXT,"");
              hash.insert(receiptsEngine::ACCOUNT_COMMENT,"");
              hash.insert(receiptsEngine::ACCOUNT_CASHAMOUNT,ACCOUNT_CASHAMOUNT);
              hash.insert(receiptsEngine::CHEQUE,cheque);
              hash.insert(receiptsEngine::VISA,visa);
              hash.insert(receiptsEngine::ACCOUNT_INSURANCEAMOUNT,ACCOUNT_INSURANCEAMOUNT);
              hash.insert(receiptsEngine::OTHER,other);
              hash.insert(receiptsEngine::ACCOUNT_DUEAMOUNT,ACCOUNT_DUEAMOUNT);*/

receiptsEngine::~receiptsEngine()
{
}

bool receiptsEngine::insertIntoAccount(const QHash<QString,QString> &hashOfValues, const QHash<int,QString> &hashOfParams)
{
    // fetch all the account model
    while (m_mpmodel->canFetchMore(QModelIndex())) {
        qDebug() << __FILE__ << QString::number(__LINE__)<< " while ";
        m_mpmodel->QAbstractItemModel::fetchMore(QModelIndex());
    }

    QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    QSqlDriver *driver = db.driver();
    if (driver->hasFeature(QSqlDriver::QuerySize)) {
        qDebug() << __FILE__ << QString::number(__LINE__) << "driver has feature";
    }
    int rowBefore = m_mpmodel->AccountModel::rowCount(QModelIndex());
    qDebug() << __FILE__ << QString::number(__LINE__) << " rowBefore = " << QString::number(rowBefore);
    bool ret = true;
    QString data;
    QHashIterator<QString,QString> it (hashOfValues);
    QList<int> listValuesNumbers;
    listValuesNumbers << ACCOUNT_MEDICALPROCEDURE_TEXT // exemple C, V, MNO, ...
            << ACCOUNT_COMMENT // exemple consultation, ...
            << ACCOUNT_CASHAMOUNT
            << ACCOUNT_CHEQUEAMOUNT
            << ACCOUNT_VISAAMOUNT
            << ACCOUNT_INSURANCEAMOUNT // <-> virement
            << ACCOUNT_DUEAMOUNT;
    while(it.hasNext()){//on prend chaque valeur
        qDebug() << __FILE__ << QString::number(__LINE__) << " it hasNext";
        it.next();
        int row = m_mpmodel->rowCount();
        qDebug() << __FILE__ << QString::number(__LINE__) << " row = " << QString::number(row);
        m_mpmodel->insertRow(row,QModelIndex());
        for(int i = 0 ; i < ACCOUNT_MaxParam ; i++){
            qDebug() << __FILE__ << QString::number(__LINE__) << " for " << QString::number(i) ;
            if (i == 0) { //ACCOUNT_ID
            	int idBefore = m_mpmodel->data(m_mpmodel->index(row,ACCOUNT_ID),Qt::DisplayRole).toInt();
            	data = QString::number(idBefore+1);
            	qDebug() << __FILE__ << QString::number(__LINE__) << " data ACCOUNT_ID = " << data;
            }
            else if (listValuesNumbers.contains(i)) {
                switch(i) {
                case ACCOUNT_MEDICALPROCEDURE_TEXT :
                    data = it.key();
                    break;
                case ACCOUNT_COMMENT :
                    data = hashOfParams.value(ACCOUNT_COMMENT);
                    break;
                case ACCOUNT_CASHAMOUNT :
                    if(hashOfParams.value(ACCOUNT_CASHAMOUNT) == QString("0")) {
                        data = "0";
                    } else {
                        data = it.value();
                    }
                    break;
                    case ACCOUNT_CHEQUEAMOUNT:
                    if(hashOfParams.value(ACCOUNT_CHEQUEAMOUNT) == QString("0")) {
                        data = "0";
                    } else {
                        data = it.value();
                    }
                    break;
                    case ACCOUNT_VISAAMOUNT:
                    if(hashOfParams.value(ACCOUNT_VISAAMOUNT) == QString("0")) {
                        data = "0";
                    }
                    else{
                        data = it.value();
                    }
                    break;
                    case ACCOUNT_INSURANCEAMOUNT :
                    if(hashOfParams.value(ACCOUNT_INSURANCEAMOUNT) == QString("0")) {
                        data = "0";
                    }
                    else{
                        data = it.value();
                    }
                    break;
                    case ACCOUNT_DUEAMOUNT :
                    if(hashOfParams.value(ACCOUNT_DUEAMOUNT) == QString("0")) {
                        data = "0";
                    }
                    else{
                        data = it.value();
                    }
                    break;
                    default :
                            break;
                }

            }
            else{
                data = hashOfParams.value(i);
            }
            qDebug() << __FILE__ << QString::number(__LINE__) << " data = " << data;
            QModelIndex index = m_mpmodel->index(row,i,QModelIndex());
            m_mpmodel-> setData(index, data ,Qt::EditRole);
            qWarning() << __FILE__ << QString::number(__LINE__) << " model account error = " 
                    <<m_mpmodel->lastError().text();
        }
        //m_mpmodel->submit();
        qWarning() << __FILE__ << QString::number(__LINE__) << " model account submit error = "
                << m_mpmodel->lastError().text();
    }

    if (m_mpmodel->rowCount(QModelIndex()) == rowBefore) {
        QMessageBox::warning(0,trUtf8("Warning ReceiptsEngine : "),trUtf8("Error = ") + m_mpmodel->lastError().text(),QMessageBox::Ok);
        ret = false;
    }
    return ret;
}

