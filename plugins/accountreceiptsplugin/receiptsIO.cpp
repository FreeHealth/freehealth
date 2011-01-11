#include "receiptsIO.h"
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

bool receiptsEngine::insertIntoAccount(QHash<int,QVariant> & hashValues)
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
    
            ACCOUNT_ID = 0,
        ACCOUNT_UID,
        ACCOUNT_USER_UID,
        ACCOUNT_PATIENT_UID,
        ACCOUNT_PATIENT_NAME,
        ACCOUNT_SITE_ID,
        ACCOUNT_INSURANCE_ID,
        ACCOUNT_DATE,
        ACCOUNT_MEDICALPROCEDURE_XML,
        ACCOUNT_MEDICALPROCEDURE_TEXT,
        ACCOUNT_COMMENT,
        ACCOUNT_CASHAMOUNT,
        ACCOUNT_CHEQUEAMOUNT,
        ACCOUNT_VISAAMOUNT,
        ACCOUNT_INSURANCEAMOUNT,
        ACCOUNT_OTHERAMOUNT,
        ACCOUNT_DUEAMOUNT,
        ACCOUNT_DUEBY,
        ACCOUNT_ISVALID,
        ACCOUNT_TRACE,
        ACCOUNT_MaxParam
    
       /*     if (!m_mpmodel-> setData(index, data ,Qt::EditRole))
            {
            	              qWarning() << __FILE__ << QString::number(__LINE__) << " model account error = " 
                    <<m_mpmodel->lastError().text() ;
                }

            qDebug() << __FILE__ << QString::number(__LINE__) << " inserted data "<< m_mpmodel->data(index,Qt::DisplayRole).toString();
        
        m_mpmodel->submit();
        qWarning() << __FILE__ << QString::number(__LINE__) << " model account submit error = "
                << m_mpmodel->lastError().text();
    }*/

    if (m_mpmodel->rowCount(QModelIndex()) == rowBefore) {
        QMessageBox::warning(0,trUtf8("Warning ReceiptsEngine : "),trUtf8("Error = ") + m_mpmodel->lastError().text(),QMessageBox::Ok);
        ret = false;
    }
    return ret;
}

