/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "movementsIO.h"

#include <accountbaseplugin/constants.h>
#include <accountbaseplugin/availablemovementmodel.h>
#include <accountbaseplugin/movementmodel.h>
#include <accountbaseplugin/bankaccountmodel.h>
#include <accountbaseplugin/accountbasecore.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/iuser.h>
#include <coreplugin/constants_icons.h>

#include <utils/global.h>

#include <QDate>

#include <QDebug>

enum { WarnDebugMessage = false };

using namespace AccountDB;
using namespace Constants;

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::IUser *user() { return  Core::ICore::instance()->user(); }
static inline AccountDB::AccountBaseCore *core() {return AccountDB::AccountBaseCore::instance();}

MovementsIODb::MovementsIODb(QObject *parent) :
    QObject(parent)
{
    m_modelMovements = core()->movementModel();
    m_user_uid = user()->uuid();
}

MovementsIODb::~MovementsIODb()
{
}

//MovementModel *MovementsIODb::getModelMovements(QString &year)
//{
//    if (WarnDebugMessage)
//        qDebug() << __FILE__ << QString::number(__LINE__) << " year =" << year ;
//    QString filter = QString("DATEVALUE between '%1' AND '%2'").arg(year+"-01-01", year+"-12-31");
//    if (WarnDebugMessage)
//        qDebug() << __FILE__ << QString::number(__LINE__) << " filter =" << filter ;
//    QString filterUid = m_modelMovements->filter();
//    QString dateAndUidFilter = filterUid+" AND "+filter;
//    m_modelMovements->setFilter(dateAndUidFilter);
//    if (WarnDebugMessage)
//        qDebug() << __FILE__ << QString::number(__LINE__) << " filter =" << m_modelMovements->filter() ;
//    if (WarnDebugMessage)
//        qDebug() << __FILE__ << QString::number(__LINE__) << " rowCount =" << QString::number(m_modelMovements->rowCount()) ;
//    return m_modelMovements;
//}

QString MovementsIODb::getUserUid()
{
    /** \todo code here */
//    return m_modelMovements->currentUserUid();
    return user()->uuid();
}

QStringList MovementsIODb::listOfParents()
{
    QStringList list;
    AvailableMovementModel availablemodel(this);
    for (int i = 0; i < availablemodel.rowCount(); ++i) {
    	QString parent = availablemodel.data(availablemodel.index(i,AVAILMOV_PARENT),Qt::DisplayRole).toString();
    	list << parent;
        }
    return list;
}

QHash<QString,QString> MovementsIODb::hashChildrenAndParentsAvailableMovements()
{
    QHash<QString,QString> hash;
    AvailableMovementModel availablemodel(this);
    int rows = availablemodel.rowCount();
    for (int i = 0; i < rows; ++i) {
    	QString child =  availablemodel.data(availablemodel.index(i,AVAILMOV_LABEL),Qt::DisplayRole).toString();   	
    	QString parent = availablemodel.data(availablemodel.index(i,AVAILMOV_PARENT),Qt::DisplayRole).toString();
    	hash.insertMulti(child,parent);
        }
    return hash;
}

QStandardItemModel  *MovementsIODb::getMovementsComboBoxModel(QObject *parent)
{
    QStandardItemModel *model = new QStandardItemModel(parent);
    AvailableMovementModel availablemodel(this);
    QStringList listOfAvModelParents;
    listOfAvModelParents = listOfParents();
    int avMovRows = availablemodel.rowCount();
    if (avMovRows < 1) {
        qWarning() << __FILE__ << QString::number(__LINE__) << "no availableMovement available" ;
        const QString information = trUtf8("No available movement available.\n"
                                           "Please create defaults in Configuration > Preferences.");
        QMessageBox::warning(0,trUtf8("Warning"),information,QMessageBox::Ok);
    }
    for (int i = 0; i < avMovRows ; ++i) {
        int type = availablemodel.data(availablemodel.index(i,AVAILMOV_TYPE),Qt::DisplayRole).toInt();
        QIcon icon;
        if (type == 1) {
            icon = QIcon(theme()->icon(Core::Constants::ICONADD));
        } else {
            icon = QIcon(theme()->icon(Core::Constants::ICONREMOVE));
        }
        QString label = availablemodel.data(availablemodel.index(i,AVAILMOV_LABEL),Qt::DisplayRole).toString();
        QStandardItem *item = new QStandardItem(icon,label);
        if (!listOfAvModelParents.contains(label)) {
            model->appendRow(item);// no parents in the list of items
        }

    }
    return model;
}

QStringList MovementsIODb::getYearComboBoxModel()
{
    QStringList listOfYears;
    for (int i = 0; i < m_modelMovements->rowCount(); ++i) {
    	QString dateStr = m_modelMovements->data(m_modelMovements->index(i,MOV_DATE),Qt::DisplayRole).toString();
    	QString dateOfValueStr = m_modelMovements->data(m_modelMovements->index(i,MOV_DATE),Qt::DisplayRole).toString();
    	QString yearDate = QString::number(QDate::fromString(dateStr,"yyyy-MM-dd").year());
    	QString yearDateOfValue = QString::number(QDate::fromString(dateOfValueStr,"yyyy-MM-dd").year());
    	listOfYears << yearDate << yearDateOfValue;
    }
    listOfYears.removeDuplicates();
    return listOfYears;
}

QStandardItemModel * MovementsIODb::getBankComboBoxModel(QObject * parent){
    QStandardItemModel *model = new QStandardItemModel(parent);
    BankAccountModel bankmodel(this);
    QString filterUserAndPrefered = QString("BD_USER_UID = '%1' AND BD_ISDEFAULT = '%2'").arg(m_user_uid,1);
    QString filterUser = QString("BD_USER_UID = '%1'").arg(m_user_uid);
    int rows = bankmodel.rowCount();
    for (int i = 0; i < rows; ++i)
    {
    	QString bankLabel = bankmodel.data(bankmodel.index(i,BANKDETAILS_LABEL),Qt::DisplayRole).toString();
    	QString bankDefault = bankmodel.data(bankmodel.index(i,BANKDETAILS_DEFAULT),Qt::DisplayRole).toString();
    	QStandardItem *item = new QStandardItem(bankLabel);
    	QIcon icon;
        if (bankDefault == "1") {
            icon = QIcon(theme()->icon(Core::Constants::ICONADD));
            item->setIcon(icon);
            if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " item def =" << item->text() ;
            model->appendRow(item);
            } 
        }
    for (int i = 0; i < rows; ++i)
    {
    	QString bankLabel = bankmodel.data(bankmodel.index(i,BANKDETAILS_LABEL),Qt::DisplayRole).toString();
    	QString bankDefault = bankmodel.data(bankmodel.index(i,BANKDETAILS_DEFAULT),Qt::DisplayRole).toString();
    	QStandardItem *item = new QStandardItem(bankLabel);
    	QIcon icon;   
    	if (bankDefault != "1")
    	{
    	    icon = QIcon(theme()->icon(Core::Constants::ICONREMOVE));
            item->setIcon(icon);
            if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " item def =" << item->text() ;
            model->appendRow(item);
    	    } 
        }
    return model;
}

bool MovementsIODb::insertIntoMovements(QHash<int,QVariant> &hashValues)
{
    bool ret = true;
    double value = 0.00;
    int type = 2;
    QString bank;
    int rowBefore = m_modelMovements->rowCount();
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " rowBefore = " << QString::number(rowBefore);
    if (m_modelMovements->insertRow(rowBefore)) {
        qWarning() << __FILE__ << QString::number(__LINE__) << "Row inserted at" << rowBefore;
    }
    QVariant data;
    for(int i = 0; i < m_modelMovements->columnCount(); ++i) {
        data = hashValues.value(i);
        if (i == MovementModel::Amount) {
            value = data.toDouble();
        }
        if (i == MovementModel::Type) {
            type = data.toInt();
        }
        if (i == MovementModel::AccountId) {
            int bankId = data.toInt();
            bank = getBankNameFromId(bankId);
            if (WarnDebugMessage)
                qDebug() << __FILE__ << QString::number(__LINE__) << " bank =" << bank ;
        }
        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << " data + i =" << data.toString()+" "+QString::number(i);
        if (!m_modelMovements-> setData(m_modelMovements->index(rowBefore,i), data)) {
            qWarning() << __FILE__ << QString::number(__LINE__) << "Unable to setData. Model account error = " << m_modelMovements->lastError().text() ;
        }
    }
    m_modelMovements->submit();
    if (m_modelMovements->rowCount() == rowBefore) {
        Utils::warningMessageBox(tr("An error occured"),
                                 m_modelMovements->lastError().text(),
                                 QString("File: %1, Line: %2").arg(__FILE__).arg(__LINE__));
        ret = false;
    }
    if (type < 1) {
        value = 0.00 - value;
        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << " value neg =" << QString::number(value) ;

    }
    if (!debitOrCreditInBankBalance(bank,value)) {
        qWarning() << __FILE__ << QString::number(__LINE__) << "Unable to debit or credit balance !" ;
    }
    return ret;
}

bool MovementsIODb::deleteMovement(int row)
{
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " row =" << QString::number(row) ;
    bool b = false;
    double value = m_modelMovements->data(m_modelMovements->index(row,MovementModel::Amount),Qt::DisplayRole).toDouble();
    int bankId = m_modelMovements->data(m_modelMovements->index(row,MovementModel::AccountId),Qt::DisplayRole).toInt();
    value = -value;
    QString bank;
    bank = getBankNameFromId(bankId);
    b = m_modelMovements->removeRows(row,1,QModelIndex());
    if (!debitOrCreditInBankBalance(bank,value)){
        qWarning() << __FILE__ << QString::number(__LINE__) << "Unable to debit or credit balance !" ;
    }
    return b;
}

int MovementsIODb::getAvailableMovementId(QString &movementsComboBoxText)
{
    int availableMovementId = 0;
    AvailableMovementModel  availablemodel(this);
    QString field = availablemodel.headerData(AVAILMOV_LABEL,Qt::Horizontal,Qt::DisplayRole).toString() ;
    QString filter = field +QString(" = '%1'").arg(movementsComboBoxText);
    if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " filter =" << filter ;
    availablemodel.setFilter(filter);
    availableMovementId = availablemodel.data(availablemodel.index(0,AVAILMOV_ID),Qt::DisplayRole).toInt();
    return availableMovementId;
}

int MovementsIODb::getBankId(QString & bankComboBoxText){
    int bankId = 0;
    BankAccountModel model(this);
    QString field = model.headerData(BANKDETAILS_LABEL,Qt::Horizontal,Qt::DisplayRole).toString();
    QString filter = field +QString(" = '%1'").arg(bankComboBoxText);
    model.setFilter(filter);
    bankId = model.data(model.index(0,BANKDETAILS_ID),Qt::DisplayRole).toInt();
    return bankId;
}

QString MovementsIODb::getBankNameFromId(int id){
    QString bank;
    BankAccountModel model(this);
    QString field = model.headerData(BANKDETAILS_ID,Qt::Horizontal,Qt::DisplayRole).toString();
    QString filter = field +QString(" = '%1'").arg(id);
    model.setFilter(filter);
    if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " model filter =" << model.filter() ;
    bank = model.data(model.index(0,BANKDETAILS_LABEL),Qt::DisplayRole).toString();
    return bank;
}

int MovementsIODb::getTypeOfMovement(QString & movementsComboBoxText){
    int type = 0;
    AvailableMovementModel  availablemodel(this);
    QString field = availablemodel.headerData(AVAILMOV_LABEL,Qt::Horizontal,Qt::DisplayRole).toString() ;
    QString filter = field +QString(" = '%1'").arg(movementsComboBoxText);
    if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " filter =" << filter ;
    availablemodel.setFilter(filter);
    type = availablemodel.data(availablemodel.index(0,AVAILMOV_TYPE),Qt::DisplayRole).toInt();
    return type;
}

bool MovementsIODb::debitOrCreditInBankBalance(const QString & bank, double & value){
    bool ret = true;
    BankAccountModel model(this);
    int row = 0;
    QList<int> rowsTestList;
    for (int i = 0; i < model.rowCount(); ++i)
    {
    	QString bankLabel = model.data(model.index(i,BANKDETAILS_LABEL),Qt::DisplayRole).toString();
    	if (bankLabel == bank)
    	{
    		  row = i;
    		  rowsTestList << i;
    	    }
        }
    if (rowsTestList.size()>1)
    {
    	  QMessageBox::warning(0,trUtf8("Warning"),
    	             trUtf8("You have two or more records with the same bank name ! Risk of errors!"),QMessageBox::Ok);
        }
    double balance = model.data(model.index(row,BANKDETAILS_BALANCE),Qt::DisplayRole).toDouble();
    double newBalance = balance + value;
    QDate date = QDate::currentDate();
    if (!model.setData(model.index(row,BANKDETAILS_BALANCE),newBalance,Qt::EditRole))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "Unable to insert balance data !" ;
        }
    if (!model.setData(model.index(row,BANKDETAILS_BALANCEDATE),date,Qt::EditRole))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "Unable to insert balance new date !" ;
        }
    if (!model.submit())
    {
    	  ret = false;
        }
    return ret;
}

bool MovementsIODb::containsFixAsset(int & row){
    bool ret = false;
    QString assetStr = m_modelMovements->data(m_modelMovements->index(row,MOV_LABEL),Qt::DisplayRole).toString();
    if (assetStr == trUtf8("Fixed Asset")) {
        qWarning() << __FILE__ << QString::number(__LINE__) << "You try to delete an asset in movements !" ;
        ret = true;
    }
    return ret;
}
