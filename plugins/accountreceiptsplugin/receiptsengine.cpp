#include "receiptsengine.h"
#include <accountbaseplugin/constants.h>

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
receiptsEngine::~receiptsEngine(){}

bool receiptsEngine::insertIntoAccount(QHash<QString,QString> & hashOfValues,QHash<int,QString> & hashOfParams){
    int rowBefore = m_mpmodel->rowCount(QModelIndex());
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
    while(it.hasNext()){
        it.next();
        int row = m_mpmodel->rowCount(QModelIndex());
        m_mpmodel->insertRow(row,QModelIndex());
        for(int i = 0 ; i < ACCOUNT_MaxParam ; i++){
            if (i == 0)
            {
            	data = QString::number(row+1);
            }
            else if( listValuesNumbers.contains(i) ){
                switch(i){
                    case ACCOUNT_MEDICALPROCEDURE_TEXT :
                        data = it.key();
                        break;
                    case ACCOUNT_COMMENT :
                    	data = hashOfParams.value(ACCOUNT_COMMENT);
                    	break;
                    case ACCOUNT_CASHAMOUNT :
                        if(hashOfParams.value(ACCOUNT_CASHAMOUNT) == QString("0")) {
                            data = "0";
                            }
                        else{
                            data = it.value();
                            }
                        break;
                    case ACCOUNT_CHEQUEAMOUNT:
                        if(hashOfParams.value(ACCOUNT_CHEQUEAMOUNT) == QString("0")) {
                            data = "0";
                            }
                        else{
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
            QModelIndex index = m_mpmodel->index(row,i,QModelIndex());
            m_mpmodel-> setData(index, data ,Qt::EditRole);
            qWarning() << __FILE__ << QString::number(__LINE__) /*<< " model account error = " 
                       <<m_mpmodel->lastError().text()*/;
            }
        m_mpmodel->submit();
        }
    if(m_mpmodel->rowCount(QModelIndex()) == rowBefore){
        QMessageBox::warning(0,trUtf8("Warning"),trUtf8("Error = ")/*+m_mpmodel->lastError().text()*/,QMessageBox::Ok);
        ret = false;
        }
    return ret;
}

