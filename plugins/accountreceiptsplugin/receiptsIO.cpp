/***************************************************************************
 *   Copyright (C) 2009 by Docteur Pierre-Marie Desombre, GP               *
 *   pm.desombre@medsyn.fr                                                 *
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
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>                     *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "receiptsIO.h"
#include <accountbaseplugin/constants.h>

#include <QSqlDriver>
#include <QMessageBox>
#include <QUuid>

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
    if (m_mpmodel->insertRows(rowBefore,1,QModelIndex()))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "Row inserted !" ;
        }
    bool ret = true;
    QVariant data;
    
    for(int i = 1 ; i < ACCOUNT_MaxParam ; i ++){
         data = hashValues.value(i);
         qDebug() << __FILE__ << QString::number(__LINE__) << " data + i =" << data.toString()+" "+QString::number(i);
         if (!m_mpmodel-> setData(m_mpmodel->index(rowBefore,i), data ,Qt::EditRole))
            {
            	qWarning() << __FILE__ << QString::number(__LINE__) << " model account error = " 
                                                                    << m_mpmodel->lastError().text() ;
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

QHash<QString,QVariant> receiptsEngine::getNamesAndValuesFromMP(){
    QHash<QString,QVariant> hash;
    MedicalProcedureModel model(this);
    int rows = model.rowCount(QModelIndex());
    qDebug() << __FILE__ << QString::number(__LINE__) << " MP row count =" << QString::number(rows) ;
    for (int i = 0; i < rows; i += 1)
    {
    	QString name = model.data(model.index(i,MP_NAME),Qt::DisplayRole).toString();
    	QVariant value = model.data(model.index(i,MP_AMOUNT),Qt::DisplayRole);
    	hash.insert(name,value);
    }
    return hash;
}

bool receiptsEngine::insertInThesaurus(QString & listOfValuesStr,QString & userUuid){
    bool ret = true;
    QUuid uuid;
    QString uuidStr = uuid.createUuid();
    ThesaurusModel model(this);
    int rowBefore = model.ThesaurusModel::rowCount(QModelIndex());
    qDebug() << __FILE__ << QString::number(__LINE__) << " rowCount thesaurus =" << QString::number(rowBefore) ;
    if (model.insertRows(rowBefore,1,QModelIndex()))
        {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "Row inserted !" ;
        }
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

bool receiptsEngine::deleteFromThesaurus(QString & data ){
    bool ret = true;
    ThesaurusModel model(this);
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

