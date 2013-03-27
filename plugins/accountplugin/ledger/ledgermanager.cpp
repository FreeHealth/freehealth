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
 *  Main Developers:  Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#include "ledgermanager.h"
#include <QDebug>

enum { WarnDebugMessage = false };

LedgerManager::LedgerManager(QObject * parent):m_sums(0.00){
    Q_UNUSED(parent);
}

LedgerManager::~LedgerManager(){}

AccountModel * LedgerManager::getModelMonthlyReceiptsAnalysis(QObject * parent,QString & month , QString & year){
    LedgerIO lio(this);
    m_sums = 0.00;
    AccountModel * model = lio.getModelMonthlyReceiptsIO(parent,month,year);
    if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << "model->rowCount()  =" <<  QString::number(model->rowCount());
    for (int i = 0; i < model->rowCount(); i += 1)
    {
    	m_sums += model->data(model->index(i,ACCOUNT_CASHAMOUNT),Qt::DisplayRole).toDouble();
    	m_sums += model->data(model->index(i,ACCOUNT_CHEQUEAMOUNT),Qt::DisplayRole).toDouble();
    	m_sums += model->data(model->index(i,ACCOUNT_VISAAMOUNT),Qt::DisplayRole).toDouble();
    	m_sums += model->data(model->index(i,ACCOUNT_INSURANCEAMOUNT),Qt::DisplayRole).toDouble();
    	m_sums += model->data(model->index(i,ACCOUNT_OTHERAMOUNT),Qt::DisplayRole).toDouble();
    }
    return model;
}

QStandardItemModel * LedgerManager::getModelMonthlyAndTypeReceiptsAnalysis(QObject * parent,
                                                                       QString & month ,
                                                                       QString & year){
    QStandardItemModel * model = new QStandardItemModel(parent);
    m_sums = 0.00;
    LedgerIO lio(this);
    AccountModel * modelAccount = lio.getModelMonthlyReceiptsIO(parent,month,year);
    if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << "modelAccount->rowCount()  =" << QString::number(modelAccount->rowCount()) ;
    for (int i = 0; i < modelAccount->rowCount(); i += 1)
    {
    	m_sums += modelAccount->data(modelAccount->index(i,ACCOUNT_CASHAMOUNT),Qt::DisplayRole).toDouble();
    	m_sums += modelAccount->data(modelAccount->index(i,ACCOUNT_CHEQUEAMOUNT),Qt::DisplayRole).toDouble();
    	m_sums += modelAccount->data(modelAccount->index(i,ACCOUNT_VISAAMOUNT),Qt::DisplayRole).toDouble();
    	m_sums += modelAccount->data(modelAccount->index(i,ACCOUNT_INSURANCEAMOUNT),Qt::DisplayRole).toDouble();
    	m_sums += modelAccount->data(modelAccount->index(i,ACCOUNT_OTHERAMOUNT),Qt::DisplayRole).toDouble();
    }
    QStringList listOfTypesByMonth;
    listOfTypesByMonth = lio.getTypesByMonth(parent,month,year);
    if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " listOfTypesByMonth.size() =" << QString::number(listOfTypesByMonth.size()) ;
    int listOfTypesSize = listOfTypesByMonth.size();
    for (int i = 0; i < listOfTypesSize; i += 1)
    {
    	int nbrOfRowByType = lio.getNbrOfRowsByTypeAndMonth(parent,month,year,listOfTypesByMonth[i]);
    	if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " nbrOfRowByType =" << QString::number(nbrOfRowByType) ;
    	if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << "listOfTypesByMonth[i]  =" << listOfTypesByMonth[i] ;
    	QStandardItem * itemRows = new QStandardItem(QString::number(nbrOfRowByType));
    	QStandardItem * itemTypes = new QStandardItem(listOfTypesByMonth[i]);
        QList<QStandardItem*> list;
        list << itemTypes << itemRows;
        model->appendRow(list);
    }
    delete modelAccount;
    if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " model->rowCount() =" << QString::number(model->rowCount()) ;
    return model;//type,number
}

QStandardItemModel * LedgerManager::getModelYearlyAndTypeReceiptsAnalysis(QObject * parent,QString & year){
    QStandardItemModel * model = new QStandardItemModel(parent);
    m_sums = 0.00;
    LedgerIO lio(this);
    m_sums = lio.getYearlyReceiptsSum(this,year);
    QStringList listOfTypesByYear;
    listOfTypesByYear = lio.listOfTypesByYear(year);
    for (int i = 0; i < listOfTypesByYear.size(); i += 1)
    {
    	int nbrOfRowByType = lio.getNbrOfRowsByTypeAndYear(parent,year,listOfTypesByYear[i]);
    	QStandardItem * itemRows = new QStandardItem(QString::number(nbrOfRowByType));
    	QStandardItem * itemTypes = new QStandardItem(listOfTypesByYear[i]);
        QList<QStandardItem*> list;
        list << itemTypes << itemRows;
        model->appendRow(list);
    }
    return model;//type,number
}

MovementModel * LedgerManager::getModelMonthlyMovementsAnalysis(QObject * parent,QString & month , QString & year){
    m_sums = 0.00;
    LedgerIO lio(this);
    MovementModel * model = lio.getModelMonthlyMovementsIO(parent,month,year);
    m_sums = lio.getMovementSum(parent,month,year);
    if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " m_sums =" << QString::number(m_sums) ;
    return model;
}

QStandardItemModel * LedgerManager::getModelMonthlyAndTypeMovementAnalysis(QObject * parent,
                                                                           QString & month,
                                                                           QString & year){
    Q_UNUSED(parent);
    m_sums = 0.00;
    LedgerIO lio(this);
    QStandardItemModel * model = lio.getModelMonthlyAndTypeMovementsIO(this,month,year);
    int rows = model->rowCount();
    for (int i = 0; i < rows; i += 1)
    {
    	m_sums += model->data(model->index(i,MOVEMENT_VALUE),Qt::DisplayRole).toDouble();
    }
    if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " m_sums =" << QString::number(m_sums) ;
    return  model;
}

QStandardItemModel * LedgerManager::getModelYearlyAndTypeMovementAnalysis(QObject * parent,QString & year){
    Q_UNUSED(parent);
    m_sums = 0.00;
    LedgerIO lio(this);
    QStandardItemModel * model = lio.getModelYearlyAndTypeMovementsIO(this,year);
    int rows = model->rowCount();
    for (int i = 0; i < rows; i += 1)
    {
    	m_sums += model->data(model->index(i,MOVEMENT_VALUE),Qt::DisplayRole).toDouble();
    }
    if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " m_sums =" << QString::number(m_sums) ;
    return model;
}

QStringList LedgerManager::getListOfYears(){
    LedgerIO lio(this);
    return lio.getListOfYears();
}

QStringList LedgerManager::getListOfMonths(){
    QStringList list;
    list << "01" << "02"<< "03"<< "04"<< "05"<< "06"<< "07"<< "08"<< "09"<< "10"<< "11" << "12";
    return list;
}

double LedgerManager::getSums()
{
    return m_sums;
}
