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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developers:  Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#include "movementsio.h"
#include <accountbaseplugin/constants.h>
#include <accountbaseplugin/availablemovementmodel.h>
#include <accountbaseplugin/movementmodel.h>
#include <accountbaseplugin/bankaccountmodel.h>

//#include <accountbaseplugin/accountbasecore.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/iuser.h>
#include <coreplugin/constants_icons.h>

#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_msgerror.h>

#include <QDate>

#include <QDebug>

enum { WarnDebugMessage = false };

using namespace AccountDB;
using namespace Constants;
using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::IUser *user() { return  Core::ICore::instance()->user(); }

//static inline AccountDB::AccountBaseCore *core() {return AccountDB::AccountBaseCore::instance();}

MovementsIODb::MovementsIODb(QObject *parent) : QObject(parent){
    m_modelMovements = new MovementModel(parent);
    m_user_uid = user()->uuid();
}

MovementsIODb::~MovementsIODb()
{
}

MovementModel *MovementsIODb::getModelMovements(QString &year)
{
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " year =" << year ;
    QString filter = QString("DATEVALUE between '%1' AND '%2'").arg(year+"-01-01", year+"-12-31");
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " filter =" << filter ;
    QString filterUid = m_modelMovements->filter();
    QString dateAndUidFilter = filterUid+" AND "+filter;
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << "dateAndUidFilter  =" << dateAndUidFilter ;
    m_modelMovements->setFilter(dateAndUidFilter);
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " filter =" << m_modelMovements->filter() ;
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " rowCount =" << QString::number(m_modelMovements->rowCount()) ;
    return m_modelMovements;
}

/*MovementModel *MovementsIODb::getModelMovements(QString &year)
{if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " year =" << year ;
    QString filter = QString("DATEVALUE between '%1' AND '%2'").arg(year+"-01-01",year+"-12-31");
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " filter =" << filter ;
    QString filterUid = m_modelMovements->filter();
    QString dateAndUidFilter = filterUid+" AND "+filter;
    m_modelMovements->setFilter(dateAndUidFilter);
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " filter =" << m_modelMovements->filter() ;
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " rowCount =" << QString::number(m_modelMovements->rowCount()) ;
    return m_modelMovements;
}*/


QString MovementsIODb::getUserUid(){
    return m_modelMovements->m_UserUid;
}

QStringList MovementsIODb::listOfParents(){
    QStringList list;
    AvailableMovementModel availablemodel(this);
    for (int i = 0; i < availablemodel.rowCount(); i += 1)
    {
        QString parent = availablemodel.data(availablemodel.index(i,AVAILMOV_PARENT),Qt::DisplayRole).toString();
        list << parent;
    }
    return list;
}

QHash<QString,QString> MovementsIODb::hashChildrenAndParentsAvailableMovements(){
    QHash<QString,QString> hash;
    AvailableMovementModel availablemodel(this);
    int rows = availablemodel.rowCount();
    for (int i = 0; i < rows; i += 1)
    {
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
        const QString &information = tr("No available movement available.\n"
                                        "Please create defaults in Configuration > Preferences.");
        Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), information);
    }
    for (int i = 0; i < avMovRows ; i += 1) {
        int type = availablemodel.data(availablemodel.index(i,AVAILMOV_TYPE),Qt::DisplayRole).toInt();
        QIcon icon;
        if (type == 1) {
            icon = QIcon(theme()->icon(Core::Constants::ICONADD));
        } else {
            icon = QIcon(theme()->icon(Core::Constants::ICONREMOVE));
        }
        QString label = availablemodel.data(availablemodel.index(i,AVAILMOV_LABEL),Qt::DisplayRole).toString();
        QStandardItem *item = new QStandardItem(icon,label);
        if (!listOfAvModelParents.contains(label))
        {
            model->appendRow(item);// no parents in the list of items
        }

    }
    return model;
}

QStringList MovementsIODb::getYearComboBoxModel()
{
    QStringList listOfYears;
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " m_modelMovements->rowCount() =" << QString::number(m_modelMovements->rowCount()) ;
    for (int i = 0; i < m_modelMovements->rowCount(); ++i) {
        QString dateStr = m_modelMovements->data(m_modelMovements->index(i,MOV_DATE),Qt::DisplayRole).toString();
        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << " dateStr =" << dateStr ;
        QString dateOfValueStr = m_modelMovements->data(m_modelMovements->index(i,MOV_DATE),Qt::DisplayRole).toString();
        QString yearDate = QString::number(QDate::fromString(dateStr,"yyyy-MM-dd").year());
        QString yearDateOfValue = QString::number(QDate::fromString(dateOfValueStr,"yyyy-MM-dd").year());
        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << "yearDate  =" << yearDate ;
        listOfYears << yearDate << yearDateOfValue;
    }
    listOfYears.removeDuplicates();
    return listOfYears;
}

QStandardItemModel * MovementsIODb::getBankComboBoxModel(QObject * parent){
    QStandardItemModel *model = new QStandardItemModel(parent);
    BankAccountModel bankmodel(this);
    QString filterUserAndpreferred = QString("BD_USER_UID = '%1' AND BD_ISDEFAULT = '%2'").arg(m_user_uid,1);
    QString filterUser = QString("BD_USER_UID = '%1'").arg(m_user_uid);
    int rows = bankmodel.rowCount();
    for (int i = 0; i < rows; i += 1)
    {
        QString bankLabel = bankmodel.data(bankmodel.index(i,BANKDETAILS_LABEL),Qt::DisplayRole).toString();
        int bankDefault = bankmodel.data(bankmodel.index(i,BANKDETAILS_DEFAULT),Qt::DisplayRole).toInt();
        QStandardItem *item = new QStandardItem(bankLabel);
        QIcon icon;
        if (bankDefault == ICON_PREF) {
            icon = QIcon(theme()->icon(Core::Constants::ICONOK));
            item->setIcon(icon);
            if (WarnDebugMessage)
                qDebug() << __FILE__ << QString::number(__LINE__) << " item def =" << item->text() ;
            model->appendRow(item);
        }
    }
    for (int i = 0; i < rows; i += 1)
    {
        QString bankLabel = bankmodel.data(bankmodel.index(i,BANKDETAILS_LABEL),Qt::DisplayRole).toString();
        int bankDefault = bankmodel.data(bankmodel.index(i,BANKDETAILS_DEFAULT),Qt::DisplayRole).toInt();
        QStandardItem *item = new QStandardItem(bankLabel);
        QIcon icon;
        if (bankDefault < ICON_PREF)
        {
            icon = QIcon(theme()->icon(QString()/*Core::Constants::ICONREMOVE*/));
            item->setIcon(icon);
            if (WarnDebugMessage)
                qDebug() << __FILE__ << QString::number(__LINE__) << " item def =" << item->text() ;
            model->appendRow(item);
        }
    }
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << "getBankComboBoxModel  "  ;
    return model;
}

bool MovementsIODb::insertIntoMovements(QHash<int,QVariant> &hashValues)
{
    bool ret = true;
    double value = 0.00;
    int type = 2;
    QString bank;
    int rowBefore = m_modelMovements->rowCount(QModelIndex());
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " rowBefore = " << QString::number(rowBefore);
    if (m_modelMovements->insertRows(rowBefore,1,QModelIndex())) {
        qWarning() << __FILE__ << QString::number(__LINE__) << "Row inserted !" ;
    }
    QVariant data;
    for(int i = 1 ; i < MOV_MaxParam ; i ++) {
        data = hashValues.value(i);
        if (i == MOV_AMOUNT) {
            value = data.toDouble();
        }
        if (i == MOV_TYPE) {
            type = data.toInt();
        }
        if (i == MOV_ACCOUNT_ID) {
            int bankId = data.toInt();
            bank = getBankNameFromId(bankId);
            if (WarnDebugMessage)
                qDebug() << __FILE__ << QString::number(__LINE__) << " bank =" << bank ;
        }
        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << " data + i =" << data.toString()+" "+QString::number(i);
        if (!m_modelMovements-> setData(m_modelMovements->index(rowBefore,i), data ,Qt::EditRole)) {
            qWarning() << __FILE__ << QString::number(__LINE__) << " model account error = "
                       << m_modelMovements->lastError().text() ;
        }
    }
    m_modelMovements->submit();
    if (m_modelMovements->rowCount(QModelIndex()) == rowBefore) {
        Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), m_modelMovements->lastError().text());
        ret = false;
    }
    if (type < 1)
    {
        value = 0.00 - value;
        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << " value neg =" << QString::number(value) ;

    }
    if (!debitOrCreditInBankBalance(bank,value)){
        qWarning() << __FILE__ << QString::number(__LINE__) << "Unable to debit or credit balance !" ;
    }
    return ret;
}

bool MovementsIODb::deleteMovement(int row, const QString & year)
{
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " row =" << QString::number(row) ;
    bool b = false;
    m_modelMovements->clearFilters();
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " filter =" << m_modelMovements->filter() ;
    QDateTime start(QDate(year.toInt(), 1, 1), QTime(0,0));
    QDateTime end(QDate(year.toInt(), 12, 31), QTime(23,59,59));
    m_modelMovements->setDatesBeginEndAndUserFilter(start,end,m_user_uid);
    double value = m_modelMovements->data(m_modelMovements->index(row,MOV_AMOUNT),Qt::DisplayRole).toDouble();
    int bankId = m_modelMovements->data(m_modelMovements->index(row,MOV_ACCOUNT_ID),Qt::DisplayRole).toInt();
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

bool MovementsIODb::validMovement(int row)
{
    return m_modelMovements->setData(m_modelMovements->index(row, MOV_ISVALID), 1, Qt::EditRole);
}

bool MovementsIODb::debitOrCreditInBankBalance(const QString & bank, double & value){
    bool ret = true;
    BankAccountModel model(this);
    int row = 0;
    QList<int> rowsTestList;
    for (int i = 0; i < model.rowCount(); i += 1)
    {
        QString bankLabel = model.data(model.index(i,BANKDETAILS_LABEL),Qt::DisplayRole).toString();
        if (bankLabel == bank)
        {
            row = i;
            rowsTestList << i;
        }
    }
    if (rowsTestList.size()>1) {
        Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("Two or more records with the same bank name."));
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
    if (assetStr == tr("Fixed Asset"))
    {
        qWarning() << __FILE__ << QString::number(__LINE__) << "You try to delete an asset in movements !" ;
        ret = true;
    }
    return ret;
}
