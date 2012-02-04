/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developpers : Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors :                                                         *
 *      NAME <MAIL@ADRESS>                                                 *
 ***************************************************************************/
#include "xmlcategoriesparser.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <QMessageBox>
#include <QFile>
#include <QtXml>
#include <QLocale>
enum { WarnDebugMessage = true };
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }


xmlCategoriesParser::xmlCategoriesParser(){}

xmlCategoriesParser::~xmlCategoriesParser(){}


QList<QHash<QString,QString> > xmlCategoriesParser::readXmlFile()
{
    QList<QHash<QString,QString> > xmlHashList;
    QDomDocument doc;
    QString xmlFilePath = settings()->path(Core::ISettings::BundleResourcesPath) + "/sql/account/categories_"+QLocale().name()+".xml";

    QFile xmlFile(xmlFilePath);
    if(xmlFile.exists() == false) {
        QMessageBox::warning(0,trUtf8("Error"),xmlFilePath+trUtf8(" does not exist."),QMessageBox::Ok);
    } else {
        if(!xmlFile.open(QIODevice::ReadOnly)) {
            QMessageBox::warning(0,trUtf8("Error"),trUtf8("xmlEchangeFile.xml not found."),QMessageBox::Ok);
        }
        if(!doc.setContent(&xmlFile)) {
            xmlFile.close();
            QMessageBox::warning(0,trUtf8("Error"),trUtf8("xmlEchangeFile.xml cannot be parsed."),QMessageBox::Ok);
            QHash<QString,QString> xmlHashZero;
            xmlHashZero.insert("error","error");
            xmlHashList << xmlHashZero;
        }
        xmlFile.close();
        QDomElement rootElement = doc.documentElement();
        for(QDomNode n = rootElement.firstChild() ; !n.isNull() ;  n = n.nextSibling()) {
            QHash<QString,QString> xmlHash;
            QDomElement childElement = n.toElement();
//            if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " tag = "+childElement.tagName();
            if(n.isElement()){
                for(QDomNode nc = childElement.firstChild() ; !nc.isNull() ;  nc = nc.nextSibling()){
                    QString str;
                    QString tagName;
                    if(nc.isElement()){
                        QDomElement e = nc.toElement();
                        str = e.text().replace("\n","").replace(" ","");
                        tagName = e.tagName();
//                        if (WarnDebugMessage)
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
