/***************************************************************************
 *   Copyright (C) 2009 by Docteur Pierre-Marie Desombre.   *
 *   pm.desombre@medsyn.fr  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "parseCCAM.h"
#include <QRegExp>
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QUuid>
#include <QDate>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlError>

ParseCcam::ParseCcam()
{
    setupUi(this);
    ccamButton->setEnabled(false);
    ngapButton->setEnabled(false);
    databaseButton->setEnabled(false);
    pathLabel->setText("");
    activityLabel->setText("");
    m_model = new QStandardItemModel(this);

    connect(searchButton,SIGNAL(pressed()),this,SLOT(searchCsvFile()));
    connect(quitButton,SIGNAL(pressed()),this,SLOT(close()));
    connect(ccamButton,SIGNAL(pressed()),this,SLOT(writeCsv()));
    connect(ngapButton,SIGNAL(pressed()),this,SLOT(writeMedicalProcedureCsv()));
    connect(databaseButton,SIGNAL(pressed()),this,SLOT(createMPDatapackDatabase()));
        
}

ParseCcam::~ParseCcam(){}

void ParseCcam::searchCsvFile()
{
    m_f = new FindFile(this);
    if (!parseAndGetCsv())
    {
    	  QMessageBox::warning(0,trUtf8("Warning"),trUtf8("Unable to get ccam file"),
    	  QMessageBox::Ok);
        }
    ccamButton->setEnabled(true);
    searchButton->setEnabled(false);
}

bool ParseCcam::parseAndGetCsv()
{
    bool ret = true;
    QString fileFound;
    QString pathFound;
    if (m_f->dialog()->exec()==QDialog::Accepted)
    {
    	  fileFound = m_f->fileRecord();
    	  pathFound = m_f->findFilePath();
        }
     
    qDebug() << __FILE__ << QString::number(__LINE__) << " found =" << fileFound ;
    qDebug() << __FILE__ << QString::number(__LINE__) << " pathFound =" << pathFound ;
    QString csvFile = pathFound;
    QFile file(csvFile);
    if (!file.open(QIODevice::ReadOnly))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << csvFile+" cannot open !" ;
    	  ret = false;
        }
    else
    {
    	qWarning() << __FILE__ << QString::number(__LINE__) << "OPENING "+csvFile ;
        }
    QTextStream stream(&file);
    stream.setCodec("UTF8");
    QString ccamExpr ="[A-Z][A-Z][A-Z][A-Z][0-9][0-9][0-9]";
    QString excludeComma = "^;";
    QString excludeHook = "[[A-Z],";
    QString tinintString = "[1-9]{1}";
    QRegExp codeCcam(ccamExpr,Qt::CaseSensitive,QRegExp::RegExp);
    //QRegExp excludeCommaExp(excludeComma,Qt::CaseSensitive,QRegExp::RegExp);
    //QRegExp excludeHookExp(excludeHook,Qt::CaseSensitive,QRegExp::RegExp);
    QRegExp regExpInt(tinintString,Qt::CaseSensitive,QRegExp::RegExp);
    QString keepCCAMString;
    while (!stream.atEnd())
    {
    	QString line = stream.readLine();
    	QStringList listOfItems = line.split(";");
    	if (listOfItems.size()>(ITEMS_ENUM_MaxParam-1) )
    	{
    	        QString code = listOfItems[CODE].remove("\"");
    	        if (code.contains(codeCcam))
    	        {
    	        	  keepCCAMString = code;
    	            }
    	        qDebug() << __FILE__ << QString::number(__LINE__) << " code  =" << keepCCAMString ;
    	        QString cCode = listOfItems[C_CODE].remove("\"");
    	        QString text = listOfItems[TEXT].remove("\"");
    	        QString activity = listOfItems[ACTIVITY].remove("\"");
    	        //qDebug() << __FILE__ << QString::number(__LINE__) << " activity =" << activity ;
    	        QString phase = listOfItems[PHASE].remove("\"");
    	        QString price = listOfItems[PRICE].remove("\"");
    	        QString reimbursment = listOfItems[REIMBURSMENT].remove("\"");
    	        QString agreement = listOfItems[AGREEMENT].remove("\"");
    	        QString exemption = listOfItems[EXEMPTION].remove("\"");
    	        QString regroupment = listOfItems[REGROUPMENT].remove("\"");
    	        QString blobXml = getXmlForBlobItem(activity,phase,reimbursment,agreement,exemption,regroupment);
    	        QStandardItem * itemCode = new QStandardItem(keepCCAMString);
    	        QStandardItem * itemText = new QStandardItem(text);
    	        QStandardItem * itemPrice = new QStandardItem(price);
                QStandardItem * itemBlob = new QStandardItem(blobXml);
    	        QList<QStandardItem*> listOfStandardItems;
    	        listOfStandardItems << itemCode << itemText << itemPrice << itemBlob;
    	        if (activity.contains(regExpInt) && activity.count() < 2)
    	        {
    	        	  m_model->appendRow(listOfStandardItems);
    	            }
    	          
    	        }//if

   	        }//while

    if (m_model->rowCount()<1)
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "m_model size = 0" ;
    	  ret = false;
        }
    return ret;
}

/*bool ParseCcam::parseAndGetCsv()
{
    bool ret = true;
    QString fileFound;
    QString pathFound;
    if (m_f->dialog()->exec()==QDialog::Accepted)
    {
    	  fileFound = m_f->fileRecord();
    	  pathFound = m_f->findFilePath();
        }
     
    qDebug() << __FILE__ << QString::number(__LINE__) << " found =" << fileFound ;
    qDebug() << __FILE__ << QString::number(__LINE__) << " pathFound =" << pathFound ;
    QString csvFile = pathFound;
    QFile file(csvFile);
    if (!file.open(QIODevice::ReadOnly))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << csvFile+" cannot open !" ;
    	  ret = false;
        }
    else
    {
    	qWarning() << __FILE__ << QString::number(__LINE__) << "OPENING "+csvFile ;
        }
    QTextStream stream(&file);
    stream.setCodec("UTF8");
    QString ccamExpr ="[A-Z][A-Z][A-Z][A-Z][0-9][0-9][0-9]";
    QString excludeComma = "^;";
    QString excludeHook = "[[A-Z],";
    QRegExp codeCcam(ccamExpr,Qt::CaseSensitive,QRegExp::RegExp);
    QRegExp excludeCommaExp(excludeComma,Qt::CaseSensitive,QRegExp::RegExp);
    QRegExp excludeHookExp(excludeHook,Qt::CaseSensitive,QRegExp::RegExp);
    
    while (!stream.atEnd())
    {
    	QString line = stream.readLine();
    	if(line.contains(codeCcam)&& !line.contains(excludeHookExp)&& !line.contains("(")&& !line.contains(excludeCommaExp)){
    	    //qDebug() << __FILE__ << QString::number(__LINE__) << " line =" << line ;
    	    QStringList listOfItems = line.split(";");
    	    qDebug() << __FILE__ << QString::number(__LINE__) << "listOfItems.size()  =" << QString::number(listOfItems.size()) ;
    	    if(listOfItems.size()==10){
    	        QString code = listOfItems[CODE].remove("\"");
    	        qDebug() << __FILE__ << QString::number(__LINE__) << " code  =" << code ;
    	        QString cCode = listOfItems[C_CODE].remove("\"");
    	        QString text = listOfItems[TEXT].remove("\"");
    	        QString activity = listOfItems[ACTIVITY].remove("\"");
    	        QString phase = listOfItems[PHASE].remove("\"");
    	        QString price = listOfItems[PRICE].remove("\"");
    	        QString reimbursment = listOfItems[REIMBURSMENT].remove("\"");
    	        QString agreement = listOfItems[AGREEMENT].remove("\"");
    	        QString exemption = listOfItems[EXEMPTION].remove("\"");
    	        QString regroupment = listOfItems[REGROUPMENT].remove("\"");

    	        QStandardItem * itemCode = new QStandardItem(code);
    	        QStandardItem * itemText = new QStandardItem(text);
    	        QStandardItem * itemPrice = new QStandardItem(price);
                QStandardItem * itemBlob = new QStandardItem(blobXml);
    	        QList<QStandardItem*> listOfStandardItems;
    	        listOfStandardItems << itemCode << itemText << itemPrice;
    	        m_model->appendRow(listOfStandardItems);
   	        }
    	    }
        }
    if (m_model->rowCount()<1)
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "m_model size = 0" ;
    	  ret = false;
        }
    return ret;
}*/

void ParseCcam::writeCsv()
{
    QString pathToCcamCsv = qApp->applicationDirPath()+"/../texts/ccam.csv";
    if (QFile::exists(pathToCcamCsv))
    {
    	  if (!QFile::remove(pathToCcamCsv))
    	  {
    	      qWarning() << __FILE__ << QString::number(__LINE__) << "UNABLE TO REMOVE ccam.csv" ;
    	      return;
    	      }
    	  }
    QFile newCsvFile(pathToCcamCsv);
    pathLabel->setText(pathToCcamCsv);
    if (!newCsvFile.open(QIODevice::ReadWrite|QIODevice::Text))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "unable to create and fill ccam.csv" ;
    	  QMessageBox::warning(0,trUtf8("Warning"),
    	  trUtf8("unable to create and fill ccam.csv"),QMessageBox::Ok);
    	  return;
        }
    QTextStream stream(&newCsvFile);
    stream.setCodec("UTF8");
    for (int i = 0; i < m_model->rowCount(); i += 1)
    {
    	QString code = m_model->data(m_model->index(i,MODEL_CODE),Qt::DisplayRole).toString();
    	//qDebug() << __FILE__ << QString::number(__LINE__) << " codeCcam =" << code ;
    	QString text = m_model->data(m_model->index(i,MODEL_TEXT),Qt::DisplayRole).toString();
    	QString price = m_model->data(m_model->index(i,MODEL_PRICE),Qt::DisplayRole).toString();
    	QString blob = m_model->data(m_model->index(i,MODEL_BLOB),Qt::DisplayRole).toString();
    	QString line = "\""+code+"\";\""+text+"\";\""+price+"\";\""+blob+"\"\n";
    	stream << line;
    	activityLabel->setText(text);
        }
    ccamButton->setEnabled(false);
    ngapButton->setEnabled(true);
}

void ParseCcam::writeMedicalProcedureCsv()
{
    QFile fileFreeAccountMPcsv(qApp->applicationDirPath()+"/../texts/medical_procedure_fr_FR_only.csv");
    if (!fileFreeAccountMPcsv.open(QIODevice::ReadOnly))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "medical_procedure_fr_FR_only.csv cannot open" ;
    	  QMessageBox::warning(0,trUtf8("Warning"),
    	  trUtf8("medical_procedure_fr_FR_only.csv cannot open"),QMessageBox::Ok);
    	  return;
        }
    int id = 0;
    QStringList listOfMedicalProcedureSineCcam;
    QTextStream stream(&fileFreeAccountMPcsv);
    stream.setCodec("UTF8");
    while (!stream.atEnd())
    {
    	QString lineNGAP = stream.readLine();
    	lineNGAP = lineNGAP.remove("\"");
    	QStringList listOfLine = lineNGAP.split(";");
    	QString mpUuid = QUuid::createUuid().toString();
    	QString mpUserUuid = "{00000000-0000-0000-0000-000000000000}";
    	QString mpInsuranceUuid = "2";
    	QString mpType = listOfLine[NGAP_TYPE];
    	QString reimbursment = "70.00";
    	QString mpDate = QDate::currentDate().toString("yyyy-MM-dd");
    	QString code = listOfLine[NGAP_CODE];
    	QString text = listOfLine[NGAP_TEXT];
    	QString price = listOfLine[NGAP_AMOUNT];
    	QString blob = listOfLine[NGAP_OTHERS];
    	const QString country = "FR";
    	if (price == "Non pris en charge")
    	{
    		  price = "0.00";
    	    }
    	QString line = "\""+QString::number(id)+"\";"+"\""+mpUuid+"\";"+"\""+mpUserUuid+"\";"+"\""+mpInsuranceUuid+"\";"+"\""+code+"\";"+"\""+text+"\";"+"\""+mpType+"\";"+"\""+price+"\";"+"\""+reimbursment+"\";"+"\""+mpDate+"\";"+"\""+blob+"\";"+"\""+country+"";
    	listOfMedicalProcedureSineCcam << line;    	
        ++id;
        }
    //"31","{91d2a429-dd3d-44cc-8782-1115a80adb71}","{00000000-0000-0000-0000-000000000000}","CST","majoration du forfait de cure thermal","Forfaits","10.0","70.0","2011-04-12",blob,FR
    //insert ccam items into MPcsv after the last procedure
    
    if (QFile::exists(qApp->applicationDirPath()+"/../texts/medical_procedure_fr_FR.csv"))
    {
    	  if (!QFile::remove(qApp->applicationDirPath()+"/../texts/medical_procedure_fr_FR.csv"))
    	  {
    	  	  qWarning() << __FILE__ << QString::number(__LINE__) << "UNABLE TO REMOVE OLD medical_procedure_fr_FR.csv " ;
    	  	  QMessageBox::warning(0,trUtf8("Warning"),trUtf8("UNABLE TO REMOVE OLD medical_procedure_fr_FR.csv "),QMessageBox::Ok);
    	  	  return;
    	      }
        }
    QFile newCsvFile(qApp->applicationDirPath()+"/../texts/medical_procedure_fr_FR.csv");
    if (!newCsvFile.open(QIODevice::ReadWrite|QIODevice::Append))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) 
    	  << "unable to create and fill medical_procedure_fr_FR.csv" ;
    	  QMessageBox::warning(0,trUtf8("Warning"),
    	  trUtf8("unable to create and fill medical_procedure_fr_FR.csv"),QMessageBox::Ok);
    	  return;
        }
    QTextStream streamNew(&newCsvFile);
    streamNew.setCodec("UTF8");
    QString strOfListMP;
    foreach(strOfListMP,listOfMedicalProcedureSineCcam){
        qDebug() << __FILE__ << QString::number(__LINE__) << " strOfListMP =" << strOfListMP ;
        streamNew << strOfListMP+"\n";
        }
    //qDebug() << __FILE__ << QString::number(__LINE__) << " id =" << QString::number(id) ;
    for (int i = 0; i < m_model->rowCount(); i += 1)
    {
    	int MPId = id;
    	QString mpUuid = QUuid::createUuid().toString();
    	QString mpUserUuid = "{00000000-0000-0000-0000-000000000000}";
    	QString mpInsuranceUuid = "2";
    	QString mpType = "CCAM";
    	
    	QString mpDate = QDate::currentDate().toString("yyyy-MM-dd");
    	QString code = m_model->data(m_model->index(i,MODEL_CODE),Qt::DisplayRole).toString();
    	QString text = m_model->data(m_model->index(i,MODEL_TEXT),Qt::DisplayRole).toString();
    	QString price = m_model->data(m_model->index(i,MODEL_PRICE),Qt::DisplayRole).toString();
    	QString reimbursment = "70.00";
    	QString blob = m_model->data(m_model->index(i,MODEL_BLOB),Qt::DisplayRole).toString();
    	const QString country = "FR";
    	if (price == "Non pris en charge")
    	{
    		  price = "0.00";
    	    }
    	QString line = "\""+QString::number(MPId)+"\";"+"\""+mpUuid+"\";"+"\""+mpUserUuid+"\";"+"\""+mpInsuranceUuid+"\";"+"\""+code+"\";"+"\""+text+"\";"+"\""+mpType+"\";"+"\""+price+"\";"+"\""+reimbursment+"\";"+"\""+mpDate+"\";"+"\""+blob+"\";"+"\""+country+"\"\n";
    	streamNew << line;
    	++id;
        }
    qWarning() << __FILE__ << QString::number(__LINE__) << "Creation of medical_procedure_fr_FR.csv finished !" ;
    ngapButton->setEnabled(false);
    databaseButton->setEnabled(true);
    
}

QString ParseCcam::getXmlForBlobItem(QString& activity,
                                     QString& phase,
                                     QString& reimbursment,
                                     QString& agreement,
                                     QString& exemption,
                                     QString& regroupment)
{
    QString xmlString;
    xmlString += "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>";
    xmlString += "<activity>";
    xmlString += activity;
    xmlString += "</activity>";
    xmlString += "<phase>";
    xmlString += phase;
    xmlString += "</phase>";
    xmlString += "<reimbursment>";
    xmlString += reimbursment;
    xmlString += "</reimbursment>";
    xmlString += "<agreement>";
    xmlString += agreement;
    xmlString += "</agreement>";
    xmlString += "<exemption>";
    xmlString += exemption;
    xmlString += "</exemption>";
    xmlString += "<regroupment>";
    xmlString += regroupment;
    xmlString += "</regroupment>";
    return xmlString;
}

void ParseCcam::createMPDatapackDatabase()
{
    databaseButton->setEnabled(false);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    bool ret = true;
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","MPDatabase");
    db.setDatabaseName("mp_datapack.db");
    if (!db.open())
    {
          QMessageBox::critical(0,trUtf8("Critical"),trUtf8("mp_datapack : ")+db.lastError().text(),QMessageBox::Ok);
          qWarning() << __FILE__ << QString::number(__LINE__) << trUtf8("mp_datapack : ")+db.lastError().text() ;
        }
    QSqlTableModel model(this,db);
    if (model.rowCount()<1)
    {
    	  QSqlQuery q(db);
    	  const QString req = "CREATE table if not exists medical_procedure ("
    	                      "MP_ID integer PRIMARY KEY,"
    	                      "MP_UUID varchar(32) NULL,"
    	                      "MP_USER_UID varchar(32) NULL,"
    	                      "MP_INSURANCE_UID varchar(32) NULL,"
    	                      "NAME varchar(200) NULL,"
    	                      "ABSTRACT varchar(2000) NULL,"
    	                      "TYPE varchar(200) NULL,"
    	                      "AMOUNT double NULL,"
    	                      "REIMBOURSEMENT double NULL,"   	                      
    	                      "DATE date NULL,"
    	                      "OTHERS blob NULL,"
    	                      "COUNTRY blob NULL);";
    	  if (!q.exec(req))
    	  {
    	  	  qWarning() << __FILE__ << QString::number(__LINE__) << q.lastError().text() ;
    	  	  QMessageBox::critical(0,trUtf8("Critical"),trUtf8("Unable to create table MP")+q.lastError().text(),QMessageBox::Ok);
    	  	  ret = false;
    	      }
        }

     QFile file(qApp->applicationDirPath()+"/../texts/medical_procedure_fr_FR.csv");
     if (!file.open(QIODevice::ReadOnly))
     {
     	  qWarning() << __FILE__ << QString::number(__LINE__) << "unable to open file" ;
     	  QMessageBox::critical(0,trUtf8("Critical"),trUtf8("Unable to open medical_procedure_fr_FR.csv"),QMessageBox::Ok);
         }
     QTextStream stream(&file);
     int row = 0;
     //error
     QFile log("logError.txt");
     if (!log.open(QIODevice::WriteOnly))
     {
     	  qWarning() << __FILE__ << QString::number(__LINE__) << "unable to open logError" ;
         }
     QTextStream streamLogError(&log);
     while (!stream.atEnd())
     {     	
     	QString line = stream.readLine();
     	if (line.contains("NAME"))
     	{
     		  continue;
     	    }
     	++row;
     	qDebug() << __FILE__ << QString::number(__LINE__) << " row =" << QString::number(row) ;
     	line = line.remove("\"");
     	QStringList listOfLineDatas = line.split(";");
     	//qDebug() << __FILE__ << QString::number(__LINE__) << " line =" << line ;
     	QStringList listOfStrings;
     	foreach(QString str,listOfLineDatas){
     	    if (str.contains("'"))
     	    {
     	    	  str.replace("'","''");
     	        }
     	    listOfStrings << "'"+str+"'";
     	    }
        QSqlQuery qy(db);
     	QString lineOfDatas = listOfStrings.join(",");
        QString req = QString("INSERT INTO %1 (%2) VALUES(%3)")
                     .arg("medical_procedure",
                          "MP_ID,MP_UUID,MP_USER_UID,MP_INSURANCE_UID,NAME,ABSTRACT,TYPE,AMOUNT,REIMBOURSEMENT, DATE,OTHERS,COUNTRY",
                          lineOfDatas);
        if (!qy.exec(req))
        {
        	  qWarning() << __FILE__ << QString::number(__LINE__) << qy.lastError().text() ;
        	  qWarning() << __FILE__ << QString::number(__LINE__) << lineOfDatas ;
        	  QApplication::restoreOverrideCursor();
        	  return;        	  
            }
         }
         if (ret == true)
         {
         	 /* if (!model.submitAll())
         	  {
         	  	  QMessageBox::critical(0,trUtf8("Critical"),model.query().lastError().text(),QMessageBox::Ok);
         	      }
         	  else{*/
                  qWarning() << __FILE__ << QString::number(__LINE__) << "mp_datapack FINISHED !" ;
         	      QApplication::restoreOverrideCursor();
         	      log.close();
         	  //}
             } 
          else
          {
          	QApplication::restoreOverrideCursor();
          	qWarning() << __FILE__ << QString::number(__LINE__) << "Unable to create mp_datapack" ;
          	log.close();
              }       
             
}
