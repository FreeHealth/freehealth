#include "receiptsIO.h"
#include <accountbaseplugin/constants.h>

#include <QSqlDriver>
#include <QMessageBox>

#include <QDebug>

using namespace AccountDB;
using namespace Constants;



receiptsEngine::receiptsEngine(){
    m_mpmodel = new AccountModel(this);
}

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
    m_mpmodel->insertRow(rowBefore,QModelIndex());
    bool ret = true;
    QVariant data;
    
    for(int i = 1 ; i < ACCOUNT_MaxParam ; i ++){
         data = hashValues.value(i);
         if (!m_mpmodel-> setData(m_mpmodel->index(rowBefore+1,i), data ,Qt::EditRole))
            {
            	qWarning() << __FILE__ << QString::number(__LINE__) << " model account error = " 
                                                                    <<m_mpmodel->lastError().text() ;
                }
        }
        m_mpmodel->submit();


    if (m_mpmodel->rowCount(QModelIndex()) == rowBefore) {
        QMessageBox::warning(0,trUtf8("Warning ReceiptsEngine : "),trUtf8("Error = ") + m_mpmodel->lastError().text(),
                             QMessageBox::Ok);
        ret = false;
    }
    return ret;
}

