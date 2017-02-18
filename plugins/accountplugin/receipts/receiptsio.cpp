/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developers:  Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#include "receiptsio.h"

#include <accountbaseplugin/constants.h>
#include <accountbaseplugin/percentmodel.h>

#include <utils/database.h>
#include <utils/global.h>
#include <coreplugin/isettings.h>
#include <coreplugin/icore.h>
#include <translationutils/constants.h>
#include <translationutils/trans_msgerror.h>

#include <QSqlDriver>
#include <QDate>
#include <QSqlQuery>
#include <QDir>

#include <QDebug>

enum { WarnDebugMessage = false };

using namespace AccountDB;
using namespace Constants;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static QString databasePath()
{
    QString dbRelPath = QString("/%1/%2").arg(Constants::DATAPACK_ACCOUNTANCY).arg(Constants::DATAPACK_ACCOUNTANCY_FILENAME);
    QString tmp;
    tmp = settings()->dataPackInstallPath() + dbRelPath;
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " tmp =" << tmp ;
    if (QFileInfo(tmp).exists())
        return settings()->dataPackInstallPath()+ QDir::separator() + Constants::DATAPACK_DB;
    tmp = settings()->dataPackApplicationInstalledPath() + dbRelPath;
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " databasePath =" << settings()->dataPackApplicationInstalledPath()+ QDir::separator() + Constants::DATAPACK_ACCOUNTANCY;
    return settings()->dataPackApplicationInstalledPath()+ QDir::separator() + Constants::DATAPACK_DB;
}

static QString databaseFileName()
{
    return databasePath() + QDir::separator() + Constants::DATAPACK_ACCOUNTANCY_FILENAME;
}

receiptsEngine::receiptsEngine()
{
    m_mpmodel = new AccountModel(this);
    m_db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    m_dbMP = QSqlDatabase::database(Constants::DATAPACK_DB);
}

receiptsEngine::~receiptsEngine()
{
}

bool receiptsEngine::insertIntoAccount(const QHash<int,QVariant> &hashValues, const QString &userUuid)
{
    // fetch all the account model
    /*while (m_mpmodel->canFetchMore(QModelIndex())) {
        if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__)<< " while ";
        m_mpmodel->QAbstractItemModel::fetchMore(QModelIndex());
    }*/

    QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    QSqlDriver *driver = db.driver();
    if (driver->hasFeature(QSqlDriver::QuerySize)) {
        if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << "driver has feature";
    }
    QString filterUser = QString("%1='%2'").arg("USER_UID",userUuid);
    m_mpmodel->AccountModel::setFilter(filterUser);
    int rowBefore = m_mpmodel->AccountModel::rowCount(QModelIndex());
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " rowBefore = " << QString::number(rowBefore);
    if (m_mpmodel->insertRows(rowBefore,1,QModelIndex()))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << "Row inserted !" ;
        }
    bool ret = true;
    QVariant data;

    for(int i = 1 ; i < ACCOUNT_MaxParam ; i ++){
         data = hashValues.value(i);
         if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " data + i =" << data.toString()+" "+QString::number(i);
         if (i == ACCOUNT_PATIENT_NAME)
         {
            QString dataString = data.toString();
            dataString.replace("'","''");
            data = QVariant(dataString);
             }
             if (WarnDebugMessage)
             qDebug() << __FILE__ << QString::number(__LINE__) << " data after =" << data.toString()+" "+QString::number(i);
         if (!m_mpmodel-> setData(m_mpmodel->index(rowBefore,i), data ,Qt::EditRole))
            {
                qWarning() << __FILE__ << QString::number(__LINE__) << " model account error = "
                                                                    << m_mpmodel->lastError().text() ;
                }
        }
        if (!m_mpmodel->AccountModel::submit())
        {
              qWarning() << __FILE__ << QString::number(__LINE__) << " submit error = "
                                     << m_mpmodel->lastError().text() ;
              ret = false;
            }

    return ret;
}

/*QHash<QString,QVariant> receiptsEngine::getNamesAndValuesFromMP(){//obsolete
    QHash<QString,QVariant> hash;
    MedicalProcedureModel model(this);
    int rows = model.rowCount(QModelIndex());
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " MP row count =" << QString::number(rows) ;
    for (int i = 0; i < rows; i += 1)
    {
        QString name = model.data(model.index(i,MP_NAME),Qt::DisplayRole).toString();
        QVariant value = model.data(model.index(i,MP_AMOUNT),Qt::DisplayRole);
        hash.insert(name,value);
    }
    return hash;
}*/

bool receiptsEngine::insertInThesaurus(const QString &listOfValuesStr, const QString &userUuid){
    bool ret = true;
    QString uuidStr = Utils::Database::createUid();
    ThesaurusModel model(this);
    int rowBefore = model.ThesaurusModel::rowCount(QModelIndex());
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " rowCount thesaurus =" << QString::number(rowBefore) ;
    if (model.insertRows(rowBefore,1,QModelIndex())) {
          qWarning() << __FILE__ << QString::number(__LINE__) << "Row inserted !" ;
        }
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " rowCount thesaurus =" << QString::number(model.ThesaurusModel::rowCount(QModelIndex())) ;
    model.setData(model.index(rowBefore,THESAURUS_UID), uuidStr,Qt::EditRole);
    model.setData(model.index(rowBefore,THESAURUS_USERUID), userUuid,Qt::EditRole);
    model.setData(model.index(rowBefore,THESAURUS_VALUES),listOfValuesStr,Qt::EditRole);
    model.setData(model.index(rowBefore,THESAURUS_PREF),0,Qt::EditRole);
    if (!model.submit())
    {
          ret = false;
        }
    return ret;
}

bool receiptsEngine::deleteFromThesaurus(const QString & data , const QString & userUid ){
    bool ret = true;
    ThesaurusModel model(this);
    QString userFilter = QString("%1 = '%2'").arg("THESAURUS_USERUID",userUid);
    model.setFilter(userFilter);
    int row = 0;
    for (int i = 0; i < model.rowCount(); i += 1)
    {
        QString dataReturned = model.data(model.index(i,THESAURUS_VALUES)).toString();
        if (dataReturned == data)
        {
              row = i;
            }
    }
    if (!model.removeRow(row))
    {
          ret = false;
        }
    return ret;
}

bool receiptsEngine::addBoolTrue(QString & data){
    bool ret = true;
    ThesaurusModel model(this);
    int row = 0;
    for (int i = 0; i < model.rowCount(); i += 1)
    {
        QString dataReturned = model.data(model.index(i,THESAURUS_VALUES)).toString();
        int b = model.data(model.index(i,THESAURUS_PREF)).toInt();
        if (dataReturned == data)
        {
              row = i;
            }
        if(b == 1){
            if (!model.setData(model.index(i,THESAURUS_PREF),0,Qt::EditRole))
            {
                  qWarning() << __FILE__ << QString::number(__LINE__) << "thesaurusModel cannot set bool to false !" ;

                  ret = false;

                }
        }
    }
    if (!model.setData(model.index(row,THESAURUS_PREF),1,Qt::EditRole))
    {
          ret = false;
        }
    return ret;
}

double receiptsEngine::getMinDistanceValue(const QString & data){
    double minDistance = 0.00;
    DistanceRulesModel model(this);
    QString filter = QString("%1 = '%2'").arg("TYPE",data);
    model.setFilter(filter);
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " filter =" << model.filter() ;
    minDistance = model.data(model.index(0,DISTRULES_MIN_KM),Qt::DisplayRole).toDouble();
    return minDistance;
}

QHash<int,QVariant> receiptsEngine::getListOfpreferredValues(QString & userUuid,
                                                             QString & patientUid,
                                                             QString & patientName,
                                                             int choice){
    QHash<int,QVariant> hash;
    double cash = 0.00;
    double check = 0.00;
    double visa = 0.00;
    double banking = 0.00;
    double other = 0.00;
    double due = 0.00;
    WorkingPlacesModel modelWP(this);
    modelWP.setFilter("PREFERRED = '1'");
    QVariant preferredSiteUid = modelWP.data(modelWP.index(0,SITES_UID),Qt::DisplayRole);
    InsuranceModel modelINS(this);
    modelINS.setFilter("PREFERRED = '1'");
    QVariant preferredInsurance = modelINS.data(modelINS.index(0,INSURANCE_NAME),Qt::DisplayRole);
    QVariant preferredInsuranceUid = modelINS.data(modelINS.index(0,INSURANCE_UID),Qt::DisplayRole);
    QVariant dateThisDay = QVariant(QDate::currentDate().toString("yyyy-MM-dd"));
    ThesaurusModel model(this);
    QString filter = QString("%1 = '%2'").arg("PREFERRED",QString::number(true));
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
            list = data.split("+");
        }
        else{
            list << data;
        }
        QString str;
        foreach(str,list){
            str = str.trimmed();
            QString field = "NAME";
            QHash<QString,double> hashTypeAndValue = getFilteredValueFromMedicalProcedure(str,field);
            double val = hashTypeAndValue.value(str);
            value += val;
        }
    }
    else
    {
        Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("You have to insert your preferred "
                                                                   "value\nin thesaurus\nand choose it as preferred."));
        value = -1.13;
    }
    QVariant preferredAct = QVariant(data);
    double preferredValue = value;
    switch(choice){
        case CASH :
            cash = preferredValue;
            break;
        case CHECK :
            check = preferredValue;
            break;
        case VISA :
            visa = preferredValue;
            break;
        case BANKING :
            banking = preferredValue;
            break;
        case OTHER :
            other = preferredValue;
            break;
        case DUE :
            due = preferredValue;
            break;
        default :
            break;
        }

    QVariant comment = QVariant(tr("preferred act"));
    hash.insert(ACCOUNT_UID,"UID");
    hash.insert(ACCOUNT_USER_UID,userUuid);
    hash.insert(ACCOUNT_PATIENT_UID,patientUid);
    hash.insert(ACCOUNT_PATIENT_NAME,patientName);
    hash.insert(ACCOUNT_SITE_ID,preferredSiteUid);//AccountDB::Constants::SITES_UID
    hash.insert(ACCOUNT_INSURANCE_ID,preferredInsuranceUid);
    hash.insert(ACCOUNT_DATE,dateThisDay);
    hash.insert(ACCOUNT_MEDICALPROCEDURE_XML, QVariant());
    hash.insert(ACCOUNT_MEDICALPROCEDURE_TEXT,preferredAct);
    hash.insert(ACCOUNT_COMMENT, comment);
    hash.insert(ACCOUNT_CASHAMOUNT,cash);
    hash.insert(ACCOUNT_CHEQUEAMOUNT,check);
    hash.insert(ACCOUNT_VISAAMOUNT,visa);
    hash.insert(ACCOUNT_INSURANCEAMOUNT,banking);
    hash.insert(ACCOUNT_OTHERAMOUNT,other);
    hash.insert(ACCOUNT_DUEAMOUNT,due);
    hash.insert(ACCOUNT_DUEBY,preferredInsurance);
    hash.insert(ACCOUNT_ISVALID,0);
    hash.insert(ACCOUNT_TRACE, QVariant());
    return hash;
}

QHash<QString,double> receiptsEngine::getFilteredValueFromMedicalProcedure(const QString & act,
                                                                           const QString & field){
    QHash<QString,double> hash;
    const QString baseName = "medical_procedure";
    const QString name = act;
    const QString amount = "AMOUNT";
    const QString type = field;
    QString filter = QString("WHERE %1 = '%2'").arg(type,act);
    QString req = QString("SELECT %1 FROM %2 ").arg(amount,baseName )+filter;
    QSqlDatabase db = m_dbMP;
    if (!datapackIsAvalaible())
    {
          db = m_db;
        }
    QSqlQuery q(db);
    if (!q.exec(req))
    {
         qWarning() << __FILE__ << QString::number(__LINE__)
                                << "Error "+QString::number(__LINE__)+" "+q.lastError().text() ;
         if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " " << q.lastQuery();
        }
    while (q.next())
    {
        QString valueStr = q.value(0).toString();
        if (valueStr.toDouble() == 0.0)
        {
              qWarning() << __FILE__ << QString::number(__LINE__) << "value null" ;
              if (valueStr.contains(","))
              {
                  if (WarnDebugMessage)
                  qDebug() << __FILE__ << QString::number(__LINE__) << " in , "  ;
                  valueStr.replace(",",QLocale::c().decimalPoint ());
                  }
              else if (valueStr.contains("."))
              {
                  valueStr.replace(".",QLocale::c().decimalPoint ());
                  }
            }
        double amount = valueStr.toDouble();
        hash.insertMulti(act,amount);
        }
    if (hash.size()>1) {
        Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("More than one value")+__FILE__+QString::number(__LINE__));
    }
    return hash;
}

QString receiptsEngine::getStringFromInsuranceUid(const QVariant & insuranceUid){
    QString debtor;
    const QString baseName = "insurance";
    const QString insuranceUidField = "INSURANCE_UID";
    const QString name = "NAME";
    QString filter = QString("WHERE %1 = '%2'").arg(insuranceUidField,insuranceUid.toString());
    QString req = QString("SELECT %1 FROM %2 ").arg(name,baseName )+filter;
    QSqlQuery q(m_db);
    if (!q.exec(req))
    {
         qWarning() << __FILE__ << QString::number(__LINE__)
                                << "Error "+QString::number(__LINE__)+" "+q.lastError().text() ;
        }
    while (q.next())
    {
        debtor = q.value(0).toString();
        }
    return debtor;
}

QVariant receiptsEngine::getSiteUidFromSite(const QString & site){
    QVariant uid = QVariant();
    const QString baseName = "sites";
    const QString uidField = "SITE_UID";
    const QString name = "NAME";
    QString filter = QString("WHERE %1 = '%2'").arg(name,site);
    QString req = QString("SELECT %1 FROM %2 ").arg(uidField,baseName )+filter;
    QSqlQuery q(m_db);
    if (!q.exec(req))
    {
         qWarning() << __FILE__ << QString::number(__LINE__)
                                << "Error "+QString::number(__LINE__)+" "+q.lastError().text() ;
        }
    while (q.next())
    {
        uid = q.value(0);
        }
    return uid;
}

QVariant receiptsEngine::getInsuranceUidFromInsurance(const QString & insurance){
    QVariant uid = QVariant();
    const QString baseName = "insurance";
    const QString uidField = "INSURANCE_UID";
    const QString name = "NAME";
    QString filter = QString("WHERE %1 = '%2'").arg(name,insurance);
    QString req = QString("SELECT %1 FROM %2 ").arg(uidField,baseName )+filter;
    QSqlQuery q(m_db);
    if (!q.exec(req))
    {
         qWarning() << __FILE__ << QString::number(__LINE__)
                                << "Error "+QString::number(__LINE__)+" "+q.lastError().text() ;
        }
    while (q.next())
    {
        uid = q.value(0);
        }
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " uid insuranceUid =" << uid.toString() ;
    return uid;
}

QString receiptsEngine::getStringOfpreferredActAndHisValue(const QString & preferredAct){
    QString data = preferredAct;
    if (WarnDebugMessage)
    qDebug() << __FILE__ << QString::number(__LINE__) << " prefact=" << data ;
    QStringList listOfActs;
    if (data.contains("+"))
    {
          listOfActs = data.split("+");
        }
    else
    {
        listOfActs << data;
        }
    QString act;
    double totalValue = 0.00;
    foreach(act,listOfActs){
        QHash<QString,double> hashTypeAndValue = getFilteredValueFromMedicalProcedure(act,"NAME");
        double val = hashTypeAndValue.value(act);
        totalValue += val;
        }
    QString text = data+" = "+QString::number(totalValue);
    return text;
}

QHash<QString,QString> receiptsEngine::getPercentagesAccordingToUser()
{
    QHash<QString,QString> hash;
    PercentModel model(this);
    for (int i = 0; i < model.rowCount(); i += 1)
    {
        QString type = model.data(model.index(i,AccountDB::Constants::PERCENT_TYPE),Qt::DisplayRole).toString();
        QString value = model.data(model.index(i,AccountDB::Constants::PERCENT_VALUES),Qt::DisplayRole).toString();
        hash.insert(type,value);
    }
    return hash;
}

QString receiptsEngine::getJustDayBeforeLastRelease()
{
    QString lastDateBefore;
    AccountDB::MedicalProcedureModel MPmodel(this);
    QStringList listOfDates;
        for (int r = 0; r < MPmodel.rowCount(); ++r)
        {
            QString date = MPmodel.data(MPmodel.index(r,AccountDB::Constants::MP_DATE),Qt::DisplayRole).toString();
            listOfDates << date;
            }
    listOfDates.removeDuplicates();
    listOfDates.sort();
    QDate lastDate = QDate::fromString("1903-01-01","yyyy-MM-dd");
    foreach(QString dateStr,listOfDates){
        QDate date = QDate::fromString(dateStr,"yyyy-MM-dd");
        if (date > lastDate)
        {
              lastDate = date;
            }
        }
    lastDateBefore = lastDate.addDays(-1).toString("yyyy-MM-dd");
    return lastDateBefore;
}

bool receiptsEngine::datapackIsAvalaible()
{
    bool b = true;
    QString datapack = databaseFileName();
    QFile file(datapack);
    if (!file.exists())
    {
          b = false;
        }
    return b;
}

//only to manage a database to try
/*bool receiptsEngine::hideNamesOfPatients()
{
    bool success = true;
    enum nameEnum {NAME=0,SURNAME};
    enum queryEnum {ACCOUNT_ID=0,PATIENT_NAME};
    QSqlQuery qy(m_db);
    QString req = QString("SELECT %1,%2 FROM %3").arg("ACCOUNT_ID","PATIENT_NAME","account");
    if (!qy.exec(req))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << qy.lastError().text() ;
    	  return false;
        }
    while (qy.next())
    {
    	QString id = qy.value(ACCOUNT_ID).toString();
    	QString nameSurname = qy.value(PATIENT_NAME).toString();
    	QStringList nameList;
    	if (nameSurname.contains(","))
    	{
    		nameList = nameSurname.split(",");
    	    }
    	if (nameSurname.contains(" ")&& !nameSurname.contains(","))
    	{
    		nameList = nameSurname.split(" ");
    	    }

    	QString name;
    	QString surname;
    	if (nameList.size()>1)
    	{
    		  name = nameList[NAME];
    		  surname = nameList[SURNAME];
    		  QStringList listOfPrefix;
    		  listOfPrefix << "DI" << "DA" << "DE" << "LE" << "LA"  ;
    		  foreach(QString prefix,listOfPrefix){
    		      if (name == prefix && nameList.size() > 2)
    		      {
    		  	  name = surname;
    		  	  surname = nameList[SURNAME+1];
    		          }
    		      }

    	    }
    	for (int i = 1; i < name.size(); ++i)
    	{
    		  name.replace(i,1,"x");
    	    }
    	nameSurname = name+","+surname;
    	QSqlQuery qUpdate(m_db);
    	QString reqUp = QString("UPDATE %1 SET %2 = '%3' WHERE %4 = '%5'")
    	                .arg("account","PATIENT_NAME",nameSurname,"ACCOUNT_ID",id);
    	if (!qUpdate.exec(reqUp))
    	{
    		  qWarning() << __FILE__ << QString::number(__LINE__) << qUpdate.lastError().text() ;
    		  return false;

    	    }
        }//while

    return success;
}*/
