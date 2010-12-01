#ifndef RECEIPTSENGINE_H
#define RECEIPTSENGINE_H
#include <accountbaseplugin/accountmodel.h>
#include <QtCore>
#include <QSqlError>
#include <QMessageBox>
#include <QtSql>
using namespace AccountDB;
class receiptsEngine : public QObject {

  Q_OBJECT
  public :
    /*enum TableAcc_Fields {//FROM accountbaseplugin/constants.h
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
    };*/
    receiptsEngine();
    ~receiptsEngine();
    bool insertIntoAccount(QHash<QString,QString> & hashOfValues,QHash<int,QString> & hashOfParams);
    AccountModel * m_mpmodel;
  private :
    
    QSqlDatabase  m_db;
};
#endif
