#include "receiptsengine.h"

receiptsEngine::receiptsEngine(){
    m_mpmodel = new AccountModel(this);
    //m_db = QSqlDatabase::database("Account");
    //m_model = new QSqlTableModel(this,m_db);
    //m_model->setTable("account");
    //m_model->select();
    
}
            /*  hash.insert(receiptsEngine::MP_TXT,"");
              hash.insert(receiptsEngine::COMMENT,"");
              hash.insert(receiptsEngine::CASH,cash);
              hash.insert(receiptsEngine::CHEQUE,cheque);
              hash.insert(receiptsEngine::VISA,visa);
              hash.insert(receiptsEngine::INSURANCE,insurance);
              hash.insert(receiptsEngine::OTHER,other);
              hash.insert(receiptsEngine::DUE,due);*/
receiptsEngine::~receiptsEngine(){}

bool receiptsEngine::insertIntoAccount(QHash<QString,QString> & hashOfValues,QHash<int,QString> & hashOfParams){
    int rowBefore = m_mpmodel->rowCount(QModelIndex());
    bool ret = true;
    QString data;
    QHashIterator<QString,QString> it (hashOfValues);
    QList<int> listValuesNumbers;
    listValuesNumbers << MP_TXT << COMMENT << CASH << CHEQUE << VISA << INSURANCE << DUE;
    while(it.hasNext()){
        it.next();
        int row = m_mpmodel->rowCount(QModelIndex());
        m_mpmodel->insertRow(row,QModelIndex());
        for(int i = 0 ; i < LAST_ENUM_HEADERS ; i++){
            if (i == 0)
            {
            	data = QString::number(row+1);
            }
            else if( listValuesNumbers.contains(i) ){
                switch(i){
                    case MP_TXT :
                        data = it.key();
                        break;
                    case COMMENT :
                    	data = hashOfParams.value(COMMENT);
                    	break;
                    case CASH :
                        if(hashOfParams.value(CASH) == QString("0")) {
                            data = "0";
                            }
                        else{
                            data = it.value();
                            }
                        break;
                    case CHEQUE :
                        if(hashOfParams.value(CHEQUE) == QString("0")) {
                            data = "0";
                            }
                        else{
                            data = it.value();
                            }
                        break;
                    case VISA :
                        if(hashOfParams.value(VISA) == QString("0")) {
                            data = "0";
                            }
                        else{
                            data = it.value();
                            }
                        break;
                    case INSURANCE :
                        if(hashOfParams.value(INSURANCE) == QString("0")) {
                            data = "0";
                            }
                        else{
                            data = it.value();
                            }
                        break;
                    case DUE :
                        if(hashOfParams.value(DUE) == QString("0")) {
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

