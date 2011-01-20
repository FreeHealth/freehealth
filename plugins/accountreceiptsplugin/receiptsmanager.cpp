#include "receiptsmanager.h"
#include "xmlcategoriesparser.h"
#include <accountbaseplugin/constants.h>
#include <accountbaseplugin/accountmodel.h>
#include <accountbaseplugin/amountmodel.h>
#include <accountbaseplugin/insurancemodel.h>
#include <accountbaseplugin/workingplacesmodel.h>
#include <accountbaseplugin/bankaccountmodel.h>
#include <accountbaseplugin/percentmodel.h>
#include <accountbaseplugin/rulesmodel.h>
#include <accountbaseplugin/distancerulesmodel.h>
#include <accountbaseplugin/thesaurusmodel.h>
#include <QMessageBox>
static  QString freeaccount = "freeaccount";
using namespace AccountDB;
using namespace Constants;

receiptsManager::receiptsManager()
{
}

receiptsManager::~receiptsManager()
{
}

QList<QMultiHash<int,QString> > receiptsManager::getPercentages()
{
  QList<QMultiHash<int,QString> > rList;
  QMultiHash<int,QString> type,percentage;
  QStringList idList;
    idList << "0" << "1" << "2" << "3";
  QStringList typeList;
    typeList << "zero" << "one third" << "two third"  << "Complete"   ;
  QStringList percentageList; 
    percentageList << "0" << "30" << "70" << "100"   ;
  QString strId;
  foreach(strId,idList){
     int key = strId.toInt();
     type.insert(key,typeList[key]);
     percentage.insert(key,percentageList[key]);
     }
  rList << type << percentage;
  return rList;
}

QHash<QString,QVariant> receiptsManager::getParametersDatas(QString & values , const QString & table)
{
   QHash<QString,QVariant> hashForReturn;
//   qDebug() << __FILE__ << QString::number(__LINE__) << " receiptsManager : in getComboBoxesDatas";
   if (table == "insurance")
   {
   	  InsuranceModel  model(this);
   	  for (int row = 0; row < model.rowCount(); row += 1)
   	  {
   	  	QString str = model.data(model.index(row,INSURANCE_NAME),Qt::DisplayRole).toString();
   	  	QVariant uid = model.data(model.index(row,INSURANCE_UID),Qt::DisplayRole);
   	  	qDebug() << __FILE__ << QString::number(__LINE__) << " receiptsManager list = " << str;
   	  	hashForReturn.insert(str,uid);
   	  }
   	  if(hashForReturn.size()< 1){
   	      hashForReturn.insert("patient","uid");
   	      }
       }
   if (table == "sites")
   {
   	  WorkingPlacesModel model(this);
   	  for (int row = 0; row < model.rowCount(); row += 1)
   	  {
   	  	QString str = model.data(model.index(row,SITES_NAME),Qt::DisplayRole).toString();
   	  	QVariant uid = model.data(model.index(row,SITES_UID),Qt::DisplayRole);
   	  	qDebug() << __FILE__ << QString::number(__LINE__) << " receiptsManager list = " << str;
   	  	qDebug() << __FILE__ << QString::number(__LINE__) << " uid =" << uid.toString() ;
   	  	hashForReturn.insert(str,uid);
   	  	
   	  	  	  	
   	  }
   	  if(hashForReturn.size()< 1){
   	      hashForReturn.insert("cabinet","uid");
   	      }
   	  
       }
    if (table == "bank_details")
    {
   	  BankAccountModel model(this);
   	  for (int row = 0; row < model.rowCount(); row += 1)
   	  {
   	  	QString str = model.data(model.index(row,BANKDETAILS_LABEL),Qt::DisplayRole).toString();
   	  	QVariant uid = model.data(model.index(row,BANKDETAILS_ID),Qt::DisplayRole);
   	  	qDebug() << __FILE__ << QString::number(__LINE__) << " receiptsManager list = " << str;
   	  	hashForReturn.insert(str,uid);
   	  }
   	  if(hashForReturn.size()< 1){
   	      hashForReturn.insert("bank","uid");
   	      }
   	  
       }
    if (table == "rules")
    {
   	  RulesModel model(this);
   	  for (int row = 0; row < model.rowCount(); row += 1)
   	  {
   	  	QString str = model.data(model.index(row,RULES_TYPE),Qt::DisplayRole).toString();
   	  	QVariant uid = model.data(model.index(row,RULES_UID),Qt::DisplayRole);
   	  	qDebug() << __FILE__ << QString::number(__LINE__) << " receiptsManager list = " << str;
   	  	hashForReturn.insert(str,uid);
   	  }
   	  if(hashForReturn.size()< 1){
   	      hashForReturn.insert("rule","uid");
   	      }
   	  
       }
    if (table == "distance_rules")
    {
   	  DistanceRulesModel model(this);
   	  for (int row = 0; row < model.rowCount(); row += 1)
   	  {
   	  	QString str = model.data(model.index(row,DISTRULES_TYPE),Qt::DisplayRole).toString();
   	  	QVariant uid = model.data(model.index(row,DISTRULES_UID),Qt::DisplayRole);
   	  	qDebug() << __FILE__ << QString::number(__LINE__) << " receiptsManager list = " << str;
   	  	hashForReturn.insert(str,uid);
   	  }
   	  if(hashForReturn.size()< 1){
   	      hashForReturn.insert("distance_rules","uid");
   	      }
   	  
       }
    if (table == "thesaurus")
    {
   	  ThesaurusModel model(this);
   	  for (int row = 0; row < model.rowCount(); row += 1)
   	  {
   	  	QString str = model.data(model.index(row,THESAURUS_VALUES),Qt::DisplayRole).toString();
   	  	QVariant uid = model.data(model.index(row,THESAURUS_UID),Qt::DisplayRole);
   	  	qDebug() << __FILE__ << QString::number(__LINE__) << " receiptsManager list = " << str;
   	  	hashForReturn.insert(str,uid);
   	  }
   	  if(hashForReturn.size()< 1){
   	      hashForReturn.insert("thesaurus","uid");
   	      }
   	  
       }
       
   return hashForReturn;;
}

QHash<QString,QVariant> receiptsManager::getHashOfSites(){
    QHash<QString,QVariant> hash;
       	  WorkingPlacesModel model(this);
   	  for (int row = 0; row < model.rowCount(); row += 1)
   	  {
   	  	QString str = model.data(model.index(row,SITES_NAME),Qt::DisplayRole).toString();
   	  	QVariant uid = model.data(model.index(row,SITES_UID),Qt::DisplayRole);
   	  	qDebug() << __FILE__ << QString::number(__LINE__) << " receiptsManager list = " << str;
   	  	qDebug() << __FILE__ << QString::number(__LINE__) << " uid =" << uid.toString() ;
   	  	hash.insert(str,uid);
   	  	
   	  	  	  	
   	  }
   	  if(hash.size()< 1){
   	      hash.insert("cabinet","uid");
   	      }
    return hash; 
}

/*QString receiptsManager::createTablesAndFields(){
    QString result = "Ok";
    qDebug() <<  __FILE__ << QString::number(__LINE__) ;
    QSqlDatabase db = QSqlDatabase::database("freeaccount");
    QString reqMP = QString("create table if not exists %1 (%2) ")
                                          .arg("medical_procedure",
                                               "MP_ID INTEGER PRIMARY KEY,"
                                               "MP_UUID varchar(50),"
                                               "MP_USER_UID varchar(50),"
                                               "NAME varchar(50),"
                                               "ABSTRACT varchar(100),"
                                               "TYPE varchar(50),"
                                               "AMOUNT real,"
                                               "REIMBOURSEMENT TEXT,"
                                               "DATE DATETIME");
    QString reqAccount = QString("create table if not exists %1 (%2)") //honoraires
                            .arg("account",
                                 "ACCOUNT_ID INTEGER PRIMARY KEY,"
                                 "ACCOUNT_UID varchar(50),"
                                 "USER_UID varchar(50),"
                                 "PATIENT_UID BIGINT,"
                                 "PATIENT_NAME VARCHAR(100),"
                                 "SITE_ID BIGINT,"
                                 "INSURANCE_ID BIGINT,"
                                 "DATE DATETIME ,"
                                 "MP_XML BLOB,"
                                 "MP_TXT LONGTEXT,"
                                 "COMMENT LONGTEXT,"
                                 "CASH REAL,"
                                 "CHEQUE REAL,"
                                 "VISA REAL,"
                                 "INSURANCE REAL,"
                                 "OTHER REAL,"
                                 "DUE REAL,"
                                 "DUE_BY REAL,"
                                 "ISVALID BOOL,"
                                 "TRACE BLOB");
    QString reqSites = QString("create table if not exists %1 (%2)")
                       .arg("sites",
                            "SITE_ID INTEGER PRIMARY KEY,"
                            "SITE_UID BIGINT,"
                            "NAME TEXT,"
                            "ADRESS LONGTEXT,"
                            "CITY TEXT,"
                            "ZIPCODE TEXT,"
                            "COUNTRY TEXT,"
                            "TEL TEXT,"
                            "FAX TEXT,"
                            "MAIL TEXT,"
                            "CONTACT TEXT");
    QString reqWho = QString("create table if not exists %1 (%2)")
                        .arg("users",
                             "USER_ID INTEGER PRIMARY KEY,"
                             "USER_UID BIGINT,"
                             "LOG TEXT,"
                             "PASS TEXT,"
                             "EMR_USER_UID TEXT,"
                             "ID_TYPE_RULES INT");
    QString reqBankDetails = QString("create table if not exists %1 (%2)")
                        .arg("bank_details",
                             "BD_ID INTEGER PRIMARY KEY,"
                             "BD_USER_UID TEXT,"
                             "BD_LABEL TEXT,"
                             "BD_OWNER TEXT,"
                             "BD_OWNERADRESS TEXT,"
                             "BD_ACCNUMB TEXT,"
                             "BD_IBAN TEXT,"
                             "BD_BALANCE REAL,"
                             "BD_BALANCEDATE DATETIME,"
                             "BD_COMMENT LONGTEXT,"
                             "BD_ISDEFAULT BOOL");
    
    QString reqRules = QString("create table if not exists %1 (%2)")
                          .arg("rules",
                               "ID_RULES INTEGER PRIMARY KEY,"
                               "ID_TYPE_RULES INT,"
                               "NAME_OF_RULE TEXT,"
                               "TYPE TEXT");
    QString reqDistanceRules = QString("create table if not exists %1 (%2)")
                                  .arg("distance_rules",
                                       "ID_DISTANCE_RULE INTEGER PRIMARY KEY,"
                                       "NAME_DIST_RULE TEXT,"
                                       "TYPE_DIST_RULE TEXT");
                                       
    QString reqDebtor = QString("create table if not exists %1 (%2)")
                           .arg("insurance",
                                "INSURANCE_ID INTEGER PRIMARY KEY,"
                                "INSURANCE_UID BIGINT,"
                                "NAME TEXT,"
                                "ADRESS LONGTEXT,"
                                "CITY TEXT,"
                                "ZIPCODE TEXT,"
                                "COUNTRY TEXT,"
                                "TEL TEXT,"
                                "FAX TEXT,"
                                "MAIL TEXT,"
                                "CONTACT TEXT");
                          

    QStringList listOfReq;
    listOfReq << reqMP 
              << reqAccount << reqSites << reqWho << reqBankDetails << reqRules << reqDistanceRules << reqDebtor ;
    QString strReq;
    foreach(strReq,listOfReq){
        QSqlQuery q(db);
        if(!q.exec(strReq)){
            qWarning() << __FILE__ << QString::number(__LINE__) << q.lastError().text() ;
            result = q.lastError().text() ;
            
            }
    
         }                                              
        qDebug() <<  __FILE__ << QString::number(__LINE__) ;
        return result;
}*/

/*bool receiptsManager::writeAllDefaultsValues(){
  bool ret = true;
  QSqlDatabase db = QSqlDatabase::database(freeaccount);
  xmlCategoriesParser xml;
  QStringList listOfReq;
  QList<QHash<QString,QString> > hashList;
  hashList = xml.readXmlFile();
  QHash<QString,QString> hash = hashList[0];
  QStringList nameOfActsList = hash.keys();
  nameOfActsList.removeAll("typesOfReceipts");
  QString nameOfKeys = nameOfActsList.join(",");
  qDebug() << __FILE__ << QString::number(__LINE__) << nameOfKeys;
  
  QString strAct;
  foreach(strAct,nameOfActsList){
      QHash<QString,QString> hashValues;
      QString xmlValue = hash.value(strAct);
      qDebug() << __FILE__ << QString::number(__LINE__) << "strAct ="+strAct+" values = "+xmlValue;
      QStringList valuesList = xmlValue.split(",");
      QString strValue;
      foreach(strValue,valuesList){
            if(strValue.contains("=")){
                QStringList pair = strValue.replace(" ","").split("=");
                qDebug() << __FILE__ << QString::number(__LINE__) << pair[0] << " "<<pair[1];
                hashValues.insert(pair[0],pair[1]);
            }
            
            }
            qDebug() << __FILE__ << QString::number(__LINE__) << " TYPE = "+hashValues.value("TYPE");
      listOfReq << QString("INSERT INTO %1 (%2) VALUES(%3)")
                          .arg("medical_procedure",
                               "NAME,ABSTRACT,TYPE,AMOUNT,REIMBOURSEMENT,DATE",
                               "'"+strAct+"',"
                               "'"+hashValues.value("ABSTRACT")+"',"
                               "'"+hashValues.value("TYPE")+"',"
                               "'"+hashValues.value("AMOUNT")+"',"
                               "'"+hashValues.value("REIMBOURSEMENT")+"',"
                               "'"+hashValues.value("DATE")+"'");
      }
   QSqlQuery q(db);
   QString req;
   foreach(req,listOfReq){
       qDebug() << __FILE__ << QString::number(__LINE__) << "requetes = "+req;
       m_rbmReq += req;
       if(!q.exec(req)){
           qWarning()  << __FILE__ << QString::number(__LINE__) << q.lastError().text();
           ret = false;
           }
       }
   return ret;
}*/

QStringList receiptsManager::getChoiceFromCategories(QString & categoriesItem){
    QStringList listOfItems;
    QSqlDatabase db = QSqlDatabase::database(freeaccount);
    QString item = categoriesItem;
    qDebug()  << __FILE__ << QString::number(__LINE__) << " categories item ="+item;
    if(item == "thesaurus"){QMessageBox::information(0,"Info","item = "+item,QMessageBox::Ok);}
    else if(item == "CCAM"){QMessageBox::information(0,"Info","show CCAM widget",QMessageBox::Ok);}
    else{
        QString req = QString("SELECT %1 FROM %2 WHERE %3 = '%4'").arg("NAME","medical_procedure","TYPE",item);
        QSqlQuery q(db);
        if(!q.exec(req)){
           qWarning()  << __FILE__ << QString::number(__LINE__) << q.lastError().text();
           listOfItems << trUtf8("Error");
           }
        while(q.next()){
            QString name = q.value(0).toString();
            qDebug()  << __FILE__ << QString::number(__LINE__) << " choice item ="+name;
            listOfItems << name;
            }
       }
    return listOfItems;
}



