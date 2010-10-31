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
#include "xmlcategoriesparser.h"
#include <QMessageBox>
#include <QtCore>
#include <QtXml>


static inline QString absoluteXmlPath() {return qApp->applicationDirPath();}


xmlCategoriesParser::xmlCategoriesParser(){}

xmlCategoriesParser::~xmlCategoriesParser(){}


QList<QHash<QString,QString> > xmlCategoriesParser::readXmlFile(){
  QList<QHash<QString,QString> > xmlHashList;
  QDomDocument doc;
  QString xmlFilePath = absoluteXmlPath()+"/xmlFiles/frenchCategories.xml";

  QFile xmlFile(xmlFilePath);
  if(xmlFile.exists() == false){
      QMessageBox::warning(0,trUtf8("Error"),xmlFilePath+trUtf8(" does not exist."),QMessageBox::Ok);
      }
  else{  
     if(!xmlFile.open(QIODevice::ReadOnly)){
         QMessageBox::warning(0,trUtf8("Error"),trUtf8("xmlEchangeFile.xml not found."),QMessageBox::Ok);
         }
     if(!doc.setContent(&xmlFile)){
        xmlFile.close();
        QMessageBox::warning(0,trUtf8("Error"),trUtf8("xmlEchangeFile.xml cannot be parsed."),QMessageBox::Ok);
        QHash<QString,QString> xmlHashZero;
        xmlHashZero.insert("error","error");
        xmlHashList << xmlHashZero;
        }
     xmlFile.close();
     QDomElement rootElement = doc.documentElement();
     for(QDomNode n = rootElement.firstChild() ; !n.isNull() ;  n = n.nextSibling()){
        QHash<QString,QString> xmlHash;
        QDomElement childElement = n.toElement();
        qDebug() << __FILE__ << QString::number(__LINE__) << " tag = "+childElement.tagName();
        if(n.isElement()){
        for(QDomNode nc = childElement.firstChild() ; !nc.isNull() ;  nc = nc.nextSibling()){
            QString str;
            QString tagName;
            if(nc.isElement()){
               QDomElement e = nc.toElement();
               str = e.text().replace("\n","").replace(" ","");
               tagName = e.tagName();
               qDebug() << __FILE__ << QString::number(__LINE__) << " tag = "+tagName+" str ="+str;
               xmlHash.insert(tagName,str);
               
               }
            }
            xmlHashList << xmlHash;
        }
      }
  }
  return xmlHashList;
  
}
