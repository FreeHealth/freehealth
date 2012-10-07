/***************************************************************************
 *   Copyright (C) 2008 by Dr Pierre-Marie DESOMBRE                        *
 *   pm.desombre@medsyn.fr                                                 *
 *                  on a idea from Eric Maeker                             *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "connexion.h"
#include "preventIO.h"
#include "IPrevention.h"

#include <QMessageBox>
#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QApplication>
#include <QtSql>
#include <QDebug>

static const QString PREVENT = QString("prevention");
static inline QString pixmaps() 
{ 
    return QString(qApp->applicationDirPath()+"/../../global_resources/pixmap");
};

bool WarnDebugMessage = false;
using namespace Common;

Connexion::Connexion(QObject *parent) : QObject(parent),
        m_IsDBConfigured(false)
{
    
}

Connexion::~Connexion(){}

bool Connexion::connect(){
    int driverType = 0;
    QString log    = "root";
    QString pass   = "";
    QString host   = "localhost";
    //QString port   = ;
    QString driver = "QSQLITE";
    //QString driver = "QMYSQL";
    //--------test driver--------------------------------
    qDebug() << __FILE__ << QString::number(__LINE__)  << "Driver chosen is "+driver;
    if(driver == "QMYSQL"){
        driverType = Driver_MySQL;
    }
    else if(driver == "QSQLITE"){
        driverType = Driver_SQLite;
    }
    else if(driver == "QPOSTGRESQL"){
        driverType = Driver_PostGreSQL;
    }
    else{
        QMessageBox::critical(0,trUtf8("Critical"),
                    trUtf8("Le driver de la base de données ne peut pas être défini.\n"
                    "Vous devez écrire dans config.ini après Driver :\n"
                    "pour MySql : MYSQL\npour Sqlite : SQLITE\npour PostGresql : POSTGRESQL."),
                    QMessageBox::Ok);
        return false;
    }
    //--------------------------------------
    
    bool dbTest = 0;
    switch (driverType) {
        case Driver_MySQL :
        {
            m_db = QSqlDatabase::addDatabase("QMYSQL",PREVENT);
            m_db.setHostName(host);
            m_db.setDatabaseName("prevention");
            m_db.setUserName(log);
            m_db.setPassword(pass);
              if (WarnDebugMessage){
                qWarning() << "Connecting  database" << host <<  log << pass ;
              }
            dbTest = (!m_db.isOpen()) && (!m_db.open());
            break;
        }
        case Driver_SQLite :
        {
            m_db = QSqlDatabase::addDatabase("QSQLITE",PREVENT);
            m_db.setDatabaseName("prevention.db");
            m_db.setHostName(host);
            m_db.setUserName(log);
            m_db.setPassword(pass);            
            
            if(!m_db.open()){
                qWarning() << "database cannot open";
            }
            if (WarnDebugMessage){
                qWarning() << "Connecting  database SQLite " + m_db.databaseName() +" "
                + QString::number(__LINE__);
              }
            QStringList tables = m_db.tables();
            qDebug() << "tables "+QString::number(tables.size());
            dbTest = (tables.size() < 2);
            break;
        }
        case Driver_PostGreSQL :
            break;
    }

    if (dbTest) {
        if (WarnDebugMessage){
                qWarning() << "Connected to  database"
                           << m_db.databaseName()
                           << __FILE__
                           << QString::number(__LINE__);
            }
        if (isDatabaseCorrupted(driverType,m_db))
             {
             	  if (!createDatabase(driverType))
             	  {
             	  	  qWarning() << __FILE__ << QString::number(__LINE__) 
             	  	  << "database cannot be created" ;
             	  	  return false;
             	      }
                 }
             
    }
    return true;
}


bool Connexion::isDatabaseCorrupted(const int driverType,QSqlDatabase &m_db)
{

    QString driverName = "";
    if(driverType == Driver_MySQL){
        driverName = "QMYSQL";
        }
    if(driverType == Driver_SQLite){
        driverName = "QSQLITE";
        }
    if(driverType == Driver_PostGreSQL){
        driverName = "QPOSTGRESQL";
        }
    if(!m_db.isOpen()){
       qWarning() << "m_db is not opened " << __FILE__ << __LINE__;
    }
    qDebug() << __FILE__ << " driverName ="+driverName;
    qDebug() << __FILE__ << " database ="+m_db.connectionName();

    int tablesCount = 2;
    int count = m_db.tables().size();
    qDebug() << __FILE__ << QString::number(__LINE__) 
    << " count =" << QString::number(count);
    qDebug() << __FILE__ << QString::number(__LINE__) 
    << " tableCount =" << QString::number(tablesCount);
    return count !=tablesCount;
}



bool Connexion::createFirstSqlConnection() const
{

     QSqlDatabase db = QSqlDatabase :: addDatabase("QMYSQL","mysql");
     db.setHostName("localhost");
     db.setDatabaseName("mysql");
     db.setUserName("root");
     db.setPassword("");
     if(!db.open()) {
         qWarning() << "la base firstConnection ne peut être ouverte.";
     return false;
     }

  return true;
}

bool Connexion::createDatabase(int driver) 
{
    qWarning()<< "in createDatabase" << __FILE__;
    if (driver==Driver_SQLite){
        if (!createDatabaseSchema(Driver_SQLite))
        {
        	  qWarning() << __FILE__ << QString::number(__LINE__) 
        	  << "unable to create prevention database" ;
        	  return false;
            }
        return true;
        }
        

    if (!createFirstSqlConnection()) {//MYSQL
        QMessageBox::warning(qApp->activeWindow(),
            tr("Fatal error"),
            tr("An error occured during prevention database creation.\nDo you have ")+"MYSQL"+""
            ""+tr(" installed ?"),
            QMessageBox::Abort);
    }
    QSqlDatabase dbFirst = QSqlDatabase::database("mysql");
    QString reqForA = "CREATE DATABASE prevention";
    QSqlQuery q(dbFirst);
    q.exec(reqForA);
    if (q.lastError().isValid() == true) {
        QMessageBox::warning(qApp->activeWindow(),
                             tr("Fatal error"),
                             tr("An error occured during prevention database creation.\n")
                             +__FILE__+" "+QString::number(__LINE__),
                             QMessageBox::Abort);
        if (WarnDebugMessage)
            qWarning() << "SQL Error" << q.lastError().text() ;
        return false;
    }
    if (!createDatabaseSchema(Driver_MySQL))
        {
        	  qWarning() << __FILE__ << QString::number(__LINE__) 
        	  << "unable to create prevention database" ;
        	  return false;
            }
    return true;
}



bool Connexion::createDatabaseSchema(int driver)
{
    bool result = true;
    if ((driver!=Driver_SQLite) && (!createDatabase(driver))) {
        QMessageBox::critical(0, tr("Critical"), tr("Unable to create database."), QMessageBox::Ok);
        result = false;
    }
    QHash<QString, QString> name_sql;
    //ITEM TYPE_OF_ITEM PARENT_ITEM PARENT_OR_CHILD ICON DATE_DONE DATE_NEXT ABSTRACT RESULT
    if (driver==Driver_MySQL) {
    name_sql.insert("Table prevention",
                    "CREATE TABLE IF NOT EXISTS prevention ("
                    "ID_Primkey bigint(20) UNSIGNED  NOT NULL  auto_increment ,"
                    "ITEM longtext COLLATE utf8_unicode_ci NULL,"
                    "TYPE tinyint(1) NOT NULL,"
                    "PARENT longtext COLLATE utf8_unicode_ci NOT NULL,"
                    "PARENT_OR_CHILD tinyint(1) NOT NULL,"
                    "ICON BLOB  NULL,"
                    "DATE_DONE	datetime   NULL,"
                    "DATE_NEXT	datetime   NULL,"
                    "ABSTRACT	longtext  NULL,"
                    "ID_ITEM    bigint NOT NULL,"
                    "PATIENT_UID varchar(32) COLLATE utf8_unicode_ci NULL ,"
                    "USER_UID int(11) NULL,"
                    "RESULT	BLOB  NULL,"
                    "PRIMARY KEY(ID_Primkey));");
    
    name_sql.insert("Table nexdate",
                    "CREATE TABLE IF NOT EXISTS nextdate ("
                    "ID_Primkey bigint(20) UNSIGNED NOT NULL auto_increment,"
                    "ITEM longtext COLLATE utf8_unicode_ci NULL,"
                    "YEAR int(4) NULL,"
                    "MONTH int(2) NULL,"
                    "DAY int(2) NULL,"
                    "HOURS int(2) NULL,"
                    "MIN int(2) NULL,"
                    "USER_UID int(11) NULL,"
                    "PRIMARY KEY(ID_Primkey));");

    }
    else if(driver==Driver_SQLite) {
    name_sql.insert("Table prevention",
                    "CREATE TABLE IF NOT EXISTS	prevention  ("
                    "ID_Primkey  INTEGER PRIMARY KEY,"
                    "ITEM longtext COLLATE utf8_unicode_ci NULL,"
                    "TYPE tinyint(1) NOT NULL,"
                    "PARENT longtext COLLATE utf8_unicode_ci NOT NULL,"
                    "PARENT_OR_CHILD tinyint(1) NOT NULL,"
                    "ICON BLOB  NULL,"
                    "DATE_DONE	datetime   NULL,"
                    "DATE_NEXT	datetime   NULL,"
                    "ABSTRACT	longtext  NULL,"
                    "ID_ITEM    bigint NOT NULL,"
                    "PATIENT_UID varchar(32) COLLATE utf8_unicode_ci NULL ,"
                    "USER_UID int(11) NULL,"
                    "RESULT	BLOB  NULL);");
                    
    name_sql.insert("Table nextdate",
                    "CREATE TABLE IF NOT EXISTS	nextdate  ("
                    "ID_Primkey  INTEGER PRIMARY KEY,"
                    "ITEM longtext COLLATE utf8_unicode_ci NULL,"
                    "YEAR int(4) NULL,"
                    "MONTH int(2) NULL,"
                    "DAY int(2) NULL,"
                    "HOURS int(2) NULL,"
                    "MIN int(2) NULL,"
                    "USER_UID int(11) NULL);");                  
    }

    // Mass execute SQL queries
    
    if ((!m_db.isOpen()) && (!m_db.open())) {  
        if (WarnDebugMessage)
            qWarning() << __FILE__ << QString::number(__LINE__) << "Unable to create database, "
                                                                   "database can not be open.";
        result = false;
    }
    qDebug() << __FILE__ << QString::number(__LINE__) <<  "Hash Size "+QString::number(name_sql.size());
    
    QHashIterator<QString, QString> it(name_sql);
     while (it.hasNext()) {
         it.next();
         QSqlQuery q(m_db);
         QString req = it.value();
         if (driver==Driver_SQLite) {
             req = req.remove("COLLATE utf8_unicode_ci", 
                             Qt::CaseInsensitive).remove("UNSIGNED", Qt::CaseInsensitive);

         }
         q.exec(req);
         // Is there an error with this req ?
         if (q.lastError().isValid()) {
             QMessageBox::warning(qApp->activeWindow(),
                                  tr("Fatal error"),
                                  tr("An error occured during  database creation.\n"
                                     "Reference : %1\n"
                                     "Error : %2\n\n"
                                     "Application will not work.")
                                  .arg(it.key()).arg(q.lastError().text())+QString::number(__LINE__),
                                  QMessageBox::Abort);
             if (WarnDebugMessage)
                 qWarning() << __FILE__ << QString::number(__LINE__) << "SQL Error" 
                            << it.key() << q.lastError().text();
             result = false;
         }
     }
     //-------table paiements et remplissage des bases d'initialisation ----------------------------------
   int numberOfTables = m_db.tables().size();
   QMessageBox mess;
               mess.setText(trUtf8("La base  a été installée "
                 "avec ")+QString::number(numberOfTables)
                 +trUtf8(" tables."));
               mess.setInformativeText(trUtf8("Do you want to install default values ?"));
               mess.setStandardButtons(QMessageBox::Ok | QMessageBox::Close);
               mess.setDefaultButton(QMessageBox::Ok);
               int r = mess.exec();
               switch(r){
                   case QMessageBox::Ok :
                       if (!setFirstDefaultValues())
                       {
                       	  QMessageBox::warning(0,trUtf8("Warning"),
                       	  trUtf8("Unable to install default values"),QMessageBox::Ok);
                           }
                       break;
                   default :
                       break;    
                   }
   
    return result;
}

bool Connexion::setFirstDefaultValues(){
    bool success = true;
    QSqlTableModel model(this,m_db);
    model.setTable(PREVENT);
    model.select();
    //ITEM TYPE_OF_ITEM PARENT_ITEM PARENT_OR_CHILD ICON DATE_DONE DATE_NEXT ABSTRACT ID_ITEM  RESULT  PATIENT_UID USER_UID
    const QString textOfDatas =
    "vaccinations,PRIMARY_PREVENTION_ITEM,ROOT,PARENT,NULL,NULL,NULL,NULL,1,NULL,NULL,NULL\n"
    "coloscopie,SECONDARY_PREVENTION_ITEM,ROOT,PARENT,NULL,NULL,NULL,NULL,2,NULL,NULL,NULL\n"
    "mammographie,SECONDARY_PREVENTION_ITEM,ROOT,PARENT,NULL,NULL,NULL,NULL,3,NULL,NULL,NULL";
    QStringList linesList;
    linesList = textOfDatas.split("\n")  ;
    for(int row = 0; row < linesList.size(); row += 1){
        QStringList listOfDatas;
        listOfDatas = linesList[row].split(",");
        listOfDatas.prepend("NULL");
        if (WarnDebugMessage)
           qDebug() << __FILE__ << QString::number(__LINE__) << " rows =" << QString::number(row) ;
        success = model.insertRows(row,1,QModelIndex());
        if (WarnDebugMessage)
           qDebug() << __FILE__ << QString::number(__LINE__) << " rowCount =" << QString::number(model.rowCount()) ;
	for (int it = 0; it < listOfDatas.size(); it += 1)
	{
		  QVariant data;
		  switch(it){
		      case PreventionEngine::IPreventionEngine::ID_PREVENTION :
		          data = row;
		          success = model.setData(model.index(row,it),data,Qt::EditRole);
		          break;		          
		      case PreventionEngine::IPreventionEngine::ITEM :
		          data = listOfDatas[it];
		          
		          success = model.setData(model.index(row,it),data,Qt::EditRole);
		          break;
		      case PreventionEngine::IPreventionEngine::TYPE_OF_ITEM :
		          data = listOfDatas[it];
		          if (data == QVariant("PRIMARY_PREVENTION_ITEM"))
		          {
		          	  data = QVariant(PreventionEngine::IPreventionEngine::PRIMARY_PREVENTION_ITEM);
		              }
		          else{
		                  data = QVariant(PreventionEngine::IPreventionEngine::SECONDARY_PREVENTION_ITEM);
		          }
		          success = model.setData(model.index(row,it),data,Qt::EditRole);
		          break;
		      case PreventionEngine::IPreventionEngine::PARENT_ITEM :
		          data = listOfDatas[it];
		          success = model.setData(model.index(row,it),data,Qt::EditRole);
		          break;
		      case PreventionEngine::IPreventionEngine::PARENT_OR_CHILD :
		          data = listOfDatas[it];
		          if (data == QVariant("PARENT"))
		          {
		          	  data = QVariant(PreventionEngine::IPreventionEngine::PARENT);
		              }
		          else
		          {
		          	data = QVariant(PreventionEngine::IPreventionEngine::CHILD);
		              }
		          success = model.setData(model.index(row,it),data,Qt::EditRole);
		          break;
		      case PreventionEngine::IPreventionEngine::ICON  :
		          data = QVariant();
		          success = model.setData(model.index(row,it),data,Qt::EditRole);
		          break;
		      case PreventionEngine::IPreventionEngine::DATE_DONE :
		          data = QVariant("2000-01-01");
		          success = model.setData(model.index(row,it),data,Qt::EditRole);
		          break;
		      case PreventionEngine::IPreventionEngine::DATE_NEXT :
		          data = QVariant("2000-01-01");
		          success = model.setData(model.index(row,it),data,Qt::EditRole);
		          break;
		      case PreventionEngine::IPreventionEngine::ABSTRACT :
		          data = QVariant("");
		          success = model.setData(model.index(row,it),data,Qt::EditRole);
		          break;
		      case PreventionEngine::IPreventionEngine::RESULT :
		          data = QVariant("");
		          success = model.setData(model.index(row,it),data,Qt::EditRole);
		          break;
		      case PreventionEngine::IPreventionEngine::ID_ITEM :
		          data = listOfDatas[it];
		          success = model.setData(model.index(row,it),data,Qt::EditRole);
		          break;
    	 	      case PreventionEngine::IPreventionEngine::PATIENT_UID :
    	 	         success = model.setData(model.index(row,it),"111111111111111111111111111111",Qt::EditRole);
    	 	         break;
    	 	      case PreventionEngine::IPreventionEngine::USER_UID :
    	 	         success = model.setData(model.index(row,it),QVariant(1),Qt::EditRole);
    	 	         break;		         
		      default :
		          break;    
		      }
		  
		      
	    }
            success = model.submitAll();
        }
        
        if (success)
        {
               QMessageBox mess;
               mess.setText(trUtf8("Default categories have been installed"));
               mess.setInformativeText(trUtf8("Do you want to install default values for testing ?"));
               mess.setStandardButtons(QMessageBox::Ok | QMessageBox::Close);
               mess.setDefaultButton(QMessageBox::Ok);
               int r = mess.exec();
               switch(r){
                   case QMessageBox::Ok :
                       if (!fillWithValuesForTesting())
                       {
                       	  QMessageBox::warning(0,trUtf8("Warning"),
                       	  trUtf8("Unable to install default values"),QMessageBox::Ok);
                           }
                       break;
                   default :
                       break;    
                   }  
            }     
    return success;
}

bool Connexion::fillWithValuesForTesting(){
    bool success = true;
    QSqlTableModel model(this,m_db);
    model.setTable(PREVENT);
    model.select();
    //ITEM TYPE_OF_ITEM PARENT_ITEM PARENT_OR_CHILD ICON DATE_DONE DATE_NEXT ABSTRACT RESULT PATIENT_UID USER_UID
    const QString textOfDatas =
    "coloscopie,SECONDARY_PREVENTION_ITEM,coloscopie,CHILD,GREEN,1988-12-O1,2030-12-01,normale,4,NULL,NULL,NULL\n"
    "coloscopie,SECONDARY_PREVENTION_ITEM,coloscopie, CHILD,RED,2000-01-01,2003-O1-01,polypes adénomateux,5,NULL,NULL,NULL\n"
    "mammographie,SECONDARY_PREVENTION_ITEM,mammographie, CHILD,GREEN,2001-03-06,2003-06-02,ACR2,6,NULL,NULL,NULL\n"
    "dtp,SECONDARY_PREVENTION_ITEM,vaccinations,CHILD,GREEN,1988-12-O1,2030-12-01,RAS,7,NULL,NULL,NULL";
    QStringList linesList;
    linesList = textOfDatas.split("\n")  ;
    int lastRow = model.rowCount();
    for(int row = lastRow; row < linesList.size()+lastRow; row += 1){
        QStringList listOfDatas;
        listOfDatas = linesList[row-lastRow].split(",");
        listOfDatas.prepend("NULL");
        if (WarnDebugMessage)
           qDebug() << __FILE__ << QString::number(__LINE__) << " rows =" << QString::number(row) ;
        success = model.insertRows(row,1,QModelIndex());
        if (WarnDebugMessage)
           qDebug() << __FILE__ << QString::number(__LINE__) << " rowCount =" << QString::number(model.rowCount()) ;
	for (int it = 0; it < listOfDatas.size(); it += 1)
	{
		  QVariant data;
		  switch(it){
		      case PreventionEngine::IPreventionEngine::ID_PREVENTION :
		          data = row;
		          success = model.setData(model.index(row,it),data,Qt::EditRole);
		          break;		          
		      case PreventionEngine::IPreventionEngine::ITEM :
		          data = listOfDatas[it];
		          
		          success = model.setData(model.index(row,it),data,Qt::EditRole);
		          break;
		      case PreventionEngine::IPreventionEngine::TYPE_OF_ITEM :
		          data = listOfDatas[it];
		          if (data == QVariant("PRIMARY_PREVENTION_ITEM"))
		          {
		          	  data = QVariant(PreventionEngine::IPreventionEngine::PRIMARY_PREVENTION_ITEM);
		              }
		          else{
		                  data = QVariant(PreventionEngine::IPreventionEngine::SECONDARY_PREVENTION_ITEM);
		          }
		          success = model.setData(model.index(row,it),data,Qt::EditRole);
		          break;
		      case PreventionEngine::IPreventionEngine::PARENT_ITEM :
		          data = listOfDatas[it];
		          success = model.setData(model.index(row,it),data,Qt::EditRole);
		          break;
		      case PreventionEngine::IPreventionEngine::PARENT_OR_CHILD :
		          data = listOfDatas[it];
		          if (data == QVariant("PARENT"))
		          {
		          	  data = QVariant(PreventionEngine::IPreventionEngine::PARENT);
		              }
		          else
		          {
		          	data = QVariant(PreventionEngine::IPreventionEngine::CHILD);
		              }
		          success = model.setData(model.index(row,it),data,Qt::EditRole);
		          break;
		      case PreventionEngine::IPreventionEngine::ICON  :
		          data = listOfDatas[it];
		          if (data.toString().contains("GREEN"))
		          {
		          	  data = pixmaps()+"/preventOk.png";
		              }
		          else
		          {
		          	data = pixmaps()+"/preventWarning.png";
		              }
		          success = model.setData(model.index(row,it),data,Qt::EditRole);
		          break;
		      case PreventionEngine::IPreventionEngine::DATE_DONE :
		          data = QVariant("2000-01-01");
		          success = model.setData(model.index(row,it),data,Qt::EditRole);
		          break;
		      case PreventionEngine::IPreventionEngine::DATE_NEXT :
		          data = QVariant("2000-01-01");
		          success = model.setData(model.index(row,it),data,Qt::EditRole);
		          break;
		      case PreventionEngine::IPreventionEngine::ABSTRACT :
		          data = QVariant(listOfDatas[it]);
		          success = model.setData(model.index(row,it),data,Qt::EditRole);
		          break;
		      case PreventionEngine::IPreventionEngine::RESULT :
		          data = QVariant("");
		          success = model.setData(model.index(row,it),data,Qt::EditRole);
		          break;
		      case PreventionEngine::IPreventionEngine::ID_ITEM :
		          data = listOfDatas[it];
		          success = model.setData(model.index(row,it),data,Qt::EditRole);
		          break;
    	 	      case PreventionEngine::IPreventionEngine::PATIENT_UID :
    	 	         success = model.setData(model.index(row,it),"111111111111111111111111111111",Qt::EditRole);
    	 	         break;
    	 	      case PreventionEngine::IPreventionEngine::USER_UID :
    	 	         success = model.setData(model.index(row,it),QVariant(1),Qt::EditRole);
    	 	         break;	
		      default :
		          break;    
		      }
		  
		      
	    }
            success = model.submitAll();
        }    
        

    return success;

}



