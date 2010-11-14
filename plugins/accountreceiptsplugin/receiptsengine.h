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
     enum headers {
     ACCOUNT_ID = 0,
     ACCOUNT_UID,//1
     USER_UID,//2
     PATIENT_UID,//3
     PATIENT_NAME,//4
     SITE_ID,//5
     INSURANCE_ID,//6
     DATE,//7
     MP_XML,//8
     MP_TXT,//9
     COMMENT,//10
     CASH,//11
     CHEQUE,//12
     VISA,//13
     INSURANCE,//14
     OTHER,//15
     DUE,//16
     DUE_BY,
     ISVALID,
     TRACE,
     LAST_ENUM_HEADERS
     };
    receiptsEngine();
    ~receiptsEngine();
    bool insertIntoAccount(QHash<QString,QString> & hashOfValues,QHash<int,QString> & hashOfParams);
  private :
    AccountModel * m_mpmodel;
    //QSqlTableModel * m_model;
    QSqlDatabase  m_db;
};
#endif
