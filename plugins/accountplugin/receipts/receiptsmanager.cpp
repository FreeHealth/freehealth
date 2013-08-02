/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  The FreeAccount plugins are free, open source FreeMedForms' plugins.   *
 *  (C) 2010-2011 by Pierre-Marie Desombre, MD <pm.desombre@medsyn.fr>     *
 *  and Eric Maeker, MD <eric.maeker@gmail.com>                            *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developers:  Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#include "receiptsmanager.h"
#include "receiptsio.h"
#include "xmlcategoriesparser.h"
#include "constants.h"
//#include "tools.h"

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
#include <accountbaseplugin/medicalproceduremodel.h>

#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>

#ifndef FREEMEDFORMS
#include <coreplugin/icommandline.h>
#include <coreplugin/freeaccount_constants.h>
static inline Core::ICommandLine *commandLine() {return Core::ICore::instance()->commandLine();}
#endif

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_msgerror.h>

#include <QSqlDatabase>
#include <QSqlQuery>

enum { WarnDebugMessage = false };

using namespace AccountDB;
using namespace Constants;
using namespace Trans::ConstantTranslations;
using namespace ReceiptsConstants;

static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}

ReceiptsManager::ReceiptsManager(QObject *parent) :
    QObject(parent)
{
    setObjectName("ReceiptsManager");
    if (!getpreferredValues()) {
        if (WarnDebugMessage)
            qWarning() << __FILE__ << QString::number(__LINE__) << "Unable to get preferred values !" ;
    }
}

ReceiptsManager::~ReceiptsManager()
{
}

QHash<int,QString> ReceiptsManager::getPercentages()
{
  QHash<int,QString> hash;
  receiptsEngine io;
  QHash<QString,QString> hashFromModel;
  hashFromModel = io.getPercentagesAccordingToUser();
  QStringList values = hashFromModel.values();
  if (hashFromModel.size()==0)
  {
    hash.insert(1,"0.00");
        hash.insert(2,"10.00");
        hash.insert(3,"20.00");
        hash.insert(4,"30.00");
        hash.insert(5,"33.33");
        hash.insert(6,"70.00");
        hash.insert(7,"100.00");
      }
  else{
    int count = values.count();
    QStringList listOfPercents;
    listOfPercents << "0.00";
    while (listOfPercents.size() < count +1)
    {
       QString minValue ;
       for (int i = 0; i < values.size(); ++i)
       {
           for (int j = 0; j < values.size(); j += 1)
           {
            if (values[i].toDouble() < values[j].toDouble())
            {
                minValue = values[i];
                }
               }
            }
        if (values.size()<2)
        {
              minValue = values[0];
            }
        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << " minValue =" << minValue ;
        if (!listOfPercents.contains(minValue))
        {
              listOfPercents.append(minValue);
              values.removeOne(minValue)  ;
            }
        }
    listOfPercents << "100.00";
    for (int i = 0; i < listOfPercents.size() ; ++i)
    {
        hash.insert(i+1,listOfPercents[i]);
        }
    }

  return hash;
}

QHash<QString,QVariant> ReceiptsManager::getParametersData(QString & userUid , int table)
{
   QHash<QString,QVariant> hashForReturn;
   if (WarnDebugMessage)
       qDebug() << __FILE__ << QString::number(__LINE__) << " ReceiptsManager: in getComboBoxesData";
   if (table == DEBTOR_ITEM) {
       InsuranceModel  model(this);
       for (int row = 0; row < model.rowCount(); row += 1) {
           QString str = model.data(model.index(row,INSURANCE_NAME),Qt::DisplayRole).toString();
           QVariant uid = model.data(model.index(row,INSURANCE_UID),Qt::DisplayRole);
           if (WarnDebugMessage)
               qDebug() << __FILE__ << QString::number(__LINE__) << " ReceiptsManager list = " << str;
           hashForReturn.insert(str,uid);
       }
       if (hashForReturn.size()< 1){
           hashForReturn.insert("patient","uid");
       }
   }
   if (table == SITES_ITEM) {
       WorkingPlacesModel model(this);
       for (int row = 0; row < model.rowCount(); row += 1) {
           QString str = model.data(model.index(row,SITES_NAME),Qt::DisplayRole).toString();
           QVariant uid = model.data(model.index(row,SITES_UID),Qt::DisplayRole);
           if (WarnDebugMessage)
               qDebug() << __FILE__ << QString::number(__LINE__) << " ReceiptsManager list = " << str;
           if (WarnDebugMessage)
               qDebug() << __FILE__ << QString::number(__LINE__) << " uid =" << uid.toString() ;
           hashForReturn.insert(str,uid);


       }
       if(hashForReturn.size()< 1){
           hashForReturn.insert("cabinet","uid");
       }

   }
   if (table == BANK_ITEM) {
       BankAccountModel model(this);
       for (int row = 0; row < model.rowCount(); row += 1) {
           QString str = model.data(model.index(row,BANKDETAILS_LABEL),Qt::DisplayRole).toString();
           QVariant uid = model.data(model.index(row,BANKDETAILS_ID),Qt::DisplayRole);
           if (WarnDebugMessage)
               qDebug() << __FILE__ << QString::number(__LINE__) << " ReceiptsManager list = " << str;
           hashForReturn.insert(str,uid);
       }
       if(hashForReturn.size()< 1){
           hashForReturn.insert("bank","uid");
       }

   }
   if (table == RULES_ITEM)
   {
       RulesModel model(this);
       for (int row = 0; row < model.rowCount(); row += 1)
       {
           QString str = model.data(model.index(row,RULES_TYPE),Qt::DisplayRole).toString();
           QVariant uid = model.data(model.index(row,RULES_UID),Qt::DisplayRole);
           if (WarnDebugMessage)
               qDebug() << __FILE__ << QString::number(__LINE__) << " ReceiptsManager list = " << str;
           hashForReturn.insert(str,uid);
       }
       if(hashForReturn.size()< 1){
           hashForReturn.insert("rule","uid");
       }

   }
   if (table == DISTANCE_RULES_ITEM)
   {
       DistanceRulesModel model(this);
       if (WarnDebugMessage)
           qDebug() << __FILE__ << QString::number(__LINE__) << " distrules =" << QString::number(model.rowCount()) ;
       for (int row = 0; row < model.rowCount(); row += 1)
       {
           QString str = model.data(model.index(row,DISTRULES_TYPE),Qt::DisplayRole).toString();
           QVariant uid = model.data(model.index(row,DISTRULES_UID),Qt::DisplayRole);
           if (WarnDebugMessage)
               qDebug() << __FILE__ << QString::number(__LINE__) << " ReceiptsManager list = " << str;
           hashForReturn.insertMulti(str,uid);
       }
       if(hashForReturn.size()< 1){
           hashForReturn.insert("distance_rules","uid");
       }

   }
   if (table == THESAURUS_ITEM)
   {
       ThesaurusModel model(this);
       QString userFilter = QString("%1 = '%2'").arg("THESAURUS_USERUID",userUid);
       model.setFilter(userFilter);
       for (int row = 0; row < model.rowCount(); row += 1)
       {
           QString str = model.data(model.index(row,THESAURUS_VALUES),Qt::DisplayRole).toString();
           QVariant uid = model.data(model.index(row,THESAURUS_UID),Qt::DisplayRole);
           if (WarnDebugMessage)
               qDebug() << __FILE__ << QString::number(__LINE__) << " ReceiptsManager list = " << str;
           hashForReturn.insert(str,uid);
           }
       if(hashForReturn.size()< 1){
           hashForReturn.insert("Act for example","uid");
           hashForReturn.insert("Please create your own series.","uid");
           }

   }

   return hashForReturn;;
}

QHash<QString,QVariant> ReceiptsManager::getHashOfSites(){
    QHash<QString,QVariant> hash;
    WorkingPlacesModel model(this);
    for (int row = 0; row < model.rowCount(); row += 1)
    {
        QString str = model.data(model.index(row,SITES_NAME),Qt::DisplayRole).toString();
        QVariant uid = model.data(model.index(row,SITES_UID),Qt::DisplayRole);
        //if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " ReceiptsManager list = " << str;
        //if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " uid =" << uid.toString() ;
        hash.insertMulti(str,uid);


    }
    if(hash.size()< 1){
        hash.insert("cabinet","uid");
    }
    return hash;
}

QHash<QString,QVariant> ReceiptsManager::getHashOfInsurance()
{
    QHash<QString,QVariant> hash;
    InsuranceModel model(this);
    for (int row = 0; row < model.rowCount(); row += 1) {
        QString str = model.data(model.index(row,INSURANCE_NAME),Qt::DisplayRole).toString();
        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << " str =" << str ;
        QVariant uid = model.data(model.index(row,INSURANCE_UID),Qt::DisplayRole);
        hash.insertMulti(str,uid);
    }
    if(hash.size()< 1) {
        hash.insert("patient","uid");
    }
    return hash;
}

QHash<QString,QVariant> ReceiptsManager::getDistanceRules()
{
    QHash<QString,QVariant> hash;
    DistanceRulesModel model(this);
    for (int row = 0; row < model.rowCount(); ++row) {
        QString str = model.data(model.index(row,DISTRULES_TYPE),Qt::DisplayRole).toString();
        QVariant uid = model.data(model.index(row,DISTRULES_VALUES),Qt::DisplayRole);
        hash.insertMulti(str,uid);
    }
    if(hash.size()< 1){
        hash.insert("DistPrice",0.50);
    }
    return hash;
}

QHash<QString,QVariant> ReceiptsManager::getHashOfThesaurus()
{
    QHash<QString,QVariant> hash;
    ThesaurusModel model(this);
    for (int row = 0; row < model.rowCount(); ++row)
    {
        QString str = model.data(model.index(row,THESAURUS_VALUES),Qt::DisplayRole).toString();
        QVariant uid = model.data(model.index(row,THESAURUS_USERUID),Qt::DisplayRole);
        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << " ReceiptsManager list = " << str;
        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << " uid =" << uid.toString() ;
        hash.insertMulti(str,uid);
    }
    if (hash.size()< 1) {
        hash.insert("thesaurus","userUuid");
    }
    return hash;
}

QHash<QString,QString> ReceiptsManager::getPreferentialActFromThesaurus(const QString & userUuid){
    QHash<QString,QString> hash;
    receiptsEngine rIO;
    ThesaurusModel model(this);
    QString filter = QString("%1 = '%2'").arg("PREFERRED",QString::number(true));
    filter += QString(" AND ");
    filter += QString("%1 = '%2'").arg("THESAURUS_USERUID",userUuid);
    model.setFilter(filter);
    model.select();
    QString data = model.data(model.index(0,THESAURUS_VALUES)).toString();

    double value = 0.00;
    QString MPfilter ;
    QStringList list;
    if(!data.isEmpty()){
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " data is not empty " ;
        if (data.contains("+"))
        {
            if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << "data.contains(+)"   ;

            list = data.split("+");
          }
        else{
              list << data;
            }
        QString str;
        foreach(str,list){
            const QString field = "NAME";
            QHash<QString,double> hashActAmount = rIO.getFilteredValueFromMedicalProcedure(str,field);
            if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " str =" << str ;
            if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " valueBef =" << QString::number(hashActAmount.value(str)) ;
            value += hashActAmount.value(str);
            }
        hash.insert(data,QString::number(value));
    }
    else
    {
        qWarning() << __FILE__ << QString::number(__LINE__) << "data is empty !" ;
        data = "NULL";
        value = 0.00;
        hash.insert(data,QString::number(value));
        const QString information = tr("You have to create a preferred value in Thesaurus.");
        Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), information);
    }
    return hash;
}

/*QString ReceiptsManager::createTablesAndFields(){
    QString result = "Ok";
    if (WarnDebugMessage)
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
        if (WarnDebugMessage)
              qDebug() <<  __FILE__ << QString::number(__LINE__) ;
        return result;
}*/

/*bool ReceiptsManager::writeAllDefaultsValues(){
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
  if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << nameOfKeys;

  QString strAct;
  foreach(strAct,nameOfActsList){
      QHash<QString,QString> hashValues;
      QString xmlValue = hash.value(strAct);
      if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << "strAct ="+strAct+" values = "+xmlValue;
      QStringList valuesList = xmlValue.split(",");
      QString strValue;
      foreach(strValue,valuesList){
            if(strValue.contains("=")){
                QStringList pair = strValue.replace(" ","").split("=");
                if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << pair[0] << " "<<pair[1];
                hashValues.insert(pair[0],pair[1]);
            }

            }
            if (WarnDebugMessage)
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
       if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << "requetes = "+req;
       m_rbmReq += req;
       if(!q.exec(req)){
           qWarning()  << __FILE__ << QString::number(__LINE__) << q.lastError().text();
           ret = false;
           }
       }
   return ret;
}*/

bool ReceiptsManager::getpreferredValues(){
    bool b = true;
    DistanceRulesModel modelDR(this);
    modelDR.setFilter("PREFERRED = '1'");
    m_preferredDistanceRule = modelDR.data(modelDR.index(0,DISTRULES_TYPE),Qt::DisplayRole);
    m_preferredDistanceValue = modelDR.data(modelDR.index(0,DISTRULES_VALUES),Qt::DisplayRole);
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " m_preferredDistanceRule =" << m_preferredDistanceRule.toString() ;
    WorkingPlacesModel modelWP(this);
    modelWP.setFilter("PREFERRED = '1'");
    m_preferredSite = modelWP.data(modelWP.index(0,SITES_NAME),Qt::DisplayRole);
    InsuranceModel modelINS(this);
    modelINS.setFilter("PREFERRED = '1'");
    m_preferredInsurance = modelINS.data(modelINS.index(0,INSURANCE_NAME),Qt::DisplayRole);
    m_preferredInsuranceUid = modelINS.data(modelINS.index(0,INSURANCE_UID),Qt::DisplayRole);
    if(m_preferredDistanceRule.isNull() || m_preferredInsurance.isNull() || m_preferredSite.isNull()){
        b = false;
        }
    return b;
}

QStringList ReceiptsManager::getChoiceFromCategories(QString & categoriesItem){
    QStringList listOfItems;
    QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    QString item = categoriesItem;
    if (WarnDebugMessage)
              qDebug()  << __FILE__ << QString::number(__LINE__) << " categories item ="+item;
    if (item == "thesaurus") {
        Utils::informativeMessageBox(tkTr(Trans::Constants::INFORMATION), "item = "+item);
    } else if (item == "CCAM") {
        Utils::informativeMessageBox(tkTr(Trans::Constants::INFORMATION), "show CCAM widget");
    } else {
        QString req = QString("SELECT %1 FROM %2 WHERE %3 = '%4'").arg("NAME","medical_procedure","TYPE",item);
        QSqlQuery q(db);
        if(!q.exec(req)){
           LOG_QUERY_ERROR(q);
           listOfItems << tr("Error");
           }
        while(q.next()){
            QString name = q.value(0).toString();
            if (WarnDebugMessage)
              qDebug()  << __FILE__ << QString::number(__LINE__) << " choice item ="+name;
            listOfItems << name;
            }
       }
    return listOfItems;
}

QVariant ReceiptsManager::getpreferredDistanceRule(){
      return m_preferredDistanceRule;
  }

QString ReceiptsManager::getStringPerferedActAndValues(const QString & act){
    receiptsEngine r;
    return r.getStringOfpreferredActAndHisValue(act);
}

bool ReceiptsManager::isMedintuxArg()
{
#ifdef FREEMEDFORMS
    return false;
#else
    return commandLine()->value(Core::Constants::CL_MedinTux).toBool();
#endif
}

QString ReceiptsManager::getFullName()
{
    // Simply use the coreplugin IPatient to get these data.
    // CommandLineParser feed this object
    return patient()->data(Core::IPatient::FullName).toString();
}

bool ReceiptsManager::isFreaccountIndependant()
{
#ifdef FREEMEDFORMS
    return false;
#else
    return commandLine()->value(Core::Constants::CL_Independant).toBool();
#endif
}

