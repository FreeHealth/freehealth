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
 *  Main Developers : Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors :                                                         *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#include "ledgerio.h"
#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <QDate>
#include <QDebug>

enum { WarnDebugMessage = false };

static inline Core::IUser *user() { return Core::ICore::instance()->user(); }

LedgerIO::LedgerIO(QObject * parent){
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " IO constructeur 1" ;
    if ((m_accountModel = new AccountModel(parent)) == NULL)
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << "AccountModel is null" ;
        }
    m_userUuid = user()->uuid(); //m_accountModel->getUserUuid();
    if (WarnDebugMessage)
    qDebug() << __FILE__ << QString::number(__LINE__) << " m_userUuid =" << m_userUuid ;
    if ((m_movementModel = new MovementModel(parent))== NULL)
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << "MovementModel is null" ;
        }
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " IO constructeur 2" ;
}

LedgerIO::~LedgerIO(){}

QStringList LedgerIO::getListOfYears(){
    QStringList list;
    QString filter = QString("%1='%2'").arg("USER_UID",m_userUuid);
    m_accountModel->setFilter(filter);
    while (m_accountModel->canFetchMore())
    {
          m_accountModel->fetchMore();
        }
    int rows = m_accountModel->rowCount();
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " rows =" << QString::number(rows) ;
    for (int i = 0; i < rows; ++i)
    {
        QString dateS = m_accountModel->data(m_accountModel->index(i,ACCOUNT_DATE),Qt::DisplayRole).toString();
        /*if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " dateS =" << dateS ;*/
        QString yearOfDate = dateS.split(" ").last();
        list << yearOfDate;
        /*if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " yearOfDate = " << yearOfDate ;*/
    }
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " listOfYears =" << QString::number(list.size()) ;
    list.removeDuplicates();

    return list;
}


AccountModel * LedgerIO::getModelMonthlyReceiptsIO(QObject * parent,QString & month , QString & year){
    QString dateBeginStr = year+"-"+month+"-01";
    QDate dateBegin = QDate::fromString(dateBeginStr,"yyyy-MM-dd");
    QDate dateMonthAfter;
    int numberOfDays = 0;
    if (month.toInt() < 12)
    {
           dateMonthAfter = dateBegin.addMonths(1);
           numberOfDays = dateBegin.daysTo(dateMonthAfter);
        }
    else{
        numberOfDays = dateBegin.daysTo(QDate::fromString(year+"-12-31","yyyy-MM-dd"));
        }
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " numberOfDays =" << QString::number(numberOfDays);
    AccountModel * accountModel = new AccountModel(parent);
    QString dateEndStr = year+"-"+month+"-"+QString::number(numberOfDays+1);
    QString filter = QString("%1='%2'").arg("USER_UID",m_userUuid);
    filter += " AND ";
    filter += QString("DATE BETWEEN '%1' AND '%2'").arg(dateBeginStr,dateEndStr);
    accountModel->setFilter(filter);
    while (accountModel->canFetchMore())
    {
          accountModel->fetchMore();
        }
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " filter  =" << accountModel->filter() ;
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " rows =" << accountModel->rowCount() ;
    return accountModel;
}



QStringList LedgerIO::getTypesByMonth(QObject * parent,QString & month,QString & year){
    QStringList list;
    QString dateBeginStr = year+"-"+month+"-01";
    QDate dateBegin = QDate::fromString(dateBeginStr,"yyyy-MM-dd");
    int lastDayInMonth = dateBegin.daysInMonth();
    QString dateEndStr = year+"-"+month+"-"+QString::number(lastDayInMonth);
    AccountModel * accountModel = new AccountModel(parent);
    QString filter = QString("%1='%2'").arg("USER_UID",m_userUuid);
    filter += " AND ";
    filter += QString("DATE BETWEEN '%1' AND '%2'").arg(dateBeginStr,dateEndStr);
    accountModel->setFilter(filter);
    while (accountModel->canFetchMore())
    {
          accountModel->fetchMore();
        }
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " filter  =" << accountModel->filter() ;
    int rowsNumber = accountModel->rowCount();
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " rowsNumber =" << QString::number(rowsNumber) ;
    for (int i = 0; i < rowsNumber; ++i)
    {
        QString dataType = accountModel->data(accountModel->index(i,ACCOUNT_MEDICALPROCEDURE_TEXT),Qt::DisplayRole)
                           .toString();
        if (dataType.contains("+"))
        {
              list << dataType.split("+");
            }
        else{
                  list << dataType;
        }
    }
    list.removeDuplicates();
    return list;
}

int LedgerIO::getNbrOfRowsByTypeAndMonth(QObject * parent,QString & month,QString & year,QString & type){
    int rows = 0;
    QString dateBeginStr = year+"-"+month+"-01";
    QDate dateBegin = QDate::fromString(dateBeginStr,"yyyy-MM-dd");
    int lastDayInMonth = dateBegin.daysInMonth();
    QString dateEndStr = year+"-"+month+"-"+QString::number(lastDayInMonth);
    QString filter = QString("%1='%2'").arg("USER_UID",m_userUuid);
    filter += " AND ";
    filter += QString("DATE BETWEEN '%1' AND '%2'").arg(dateBeginStr,dateEndStr);
    AccountModel * accountModel = new AccountModel(parent);
    accountModel->setFilter(filter);
    while (accountModel->canFetchMore())
    {
          accountModel->fetchMore();
        }
    int rowsNumber = accountModel->rowCount();
    for (int i = 0; i < rowsNumber; ++i)
    {
        QString dataType = accountModel->data(accountModel->index(i,ACCOUNT_MEDICALPROCEDURE_TEXT),Qt::DisplayRole)
                          .toString();
        if (dataType.contains("+"))
        {
              QStringList list = dataType.split("+");
              QString str;
              foreach(str,list){
                  if (str == type)
                  {
                      rows++;
                      }
                  }
            }
        else{
            if (dataType == type)
            {
                  rows++;
                }
        }
    }
    return rows;
}

QStringList LedgerIO::listOfTypesByYear(QString & year){
    QStringList list;
    QString dateBeginStr = year+"-01-01";
    QString dateEndStr = year+"-12-31";
    QString filter = QString("%1='%2'").arg("USER_UID",m_userUuid);
    filter += " AND ";
    filter += QString("DATE BETWEEN '%1' AND '%2'").arg(dateBeginStr,dateEndStr);
    m_accountModel->setFilter(filter);
    while (m_accountModel->canFetchMore())
    {
          m_accountModel->fetchMore();
        }
    int rowsNumber = m_accountModel->rowCount();
    for (int i = 0; i < rowsNumber; ++i)
    {
        QString dataType = m_accountModel->data(m_accountModel->index(i,ACCOUNT_MEDICALPROCEDURE_TEXT),Qt::DisplayRole)
                           .toString();
        if (dataType.contains("+"))
        {
              list << dataType.split("+");
            }
        else{
                  list << dataType;
        }
    }
    list.removeDuplicates();
    return list;
}
int LedgerIO::getNbrOfRowsByTypeAndYear(QObject * parent,QString & year,QString & type){
    Q_UNUSED(parent);
    QStringList list;
    int rows = 0;
    QString dateBeginStr = year+"-01-01";
    QString dateEndStr = year+"-12-31";
    QString filter = QString("%1='%2'").arg("USER_UID",m_userUuid);
    filter += " AND ";
    filter += QString("DATE BETWEEN '%1' AND '%2'").arg(dateBeginStr,dateEndStr);
    m_accountModel->setFilter(filter);
    while (m_accountModel->canFetchMore())
    {
          m_accountModel->fetchMore();
        }
    int rowsNumber = m_accountModel->rowCount();
    for (int i = 0; i < rowsNumber; ++i)
    {
        QString dataType = m_accountModel->data(m_accountModel->index(i,ACCOUNT_MEDICALPROCEDURE_TEXT),Qt::DisplayRole)
                           .toString();
        if (dataType.contains("+"))
        {
              QStringList list = dataType.split("+");
              QString str;
              foreach(str,list){
                  if (str == type)
                  {
                      rows++;
                      }
                  }
            }
        else{
            if (dataType == type)
            {
                  rows++;
                }
        }
    }
    return rows;

}

double LedgerIO::getYearlyReceiptsSum(QObject * parent,QString & year){
    Q_UNUSED(parent);
    double totalValue = 0.00;
    QString dateBeginStr = year+"-01-01";
    QString dateEndStr = year+"-12-31";
    QString filter = QString("%1='%2'").arg("USER_UID",m_userUuid);
    filter += " AND ";
    filter += QString("DATE BETWEEN '%1' AND '%2'").arg(dateBeginStr,dateEndStr);
    m_accountModel->setFilter(filter);
    while (m_accountModel->canFetchMore())
    {
          m_accountModel->fetchMore();
        }
    int rows = 0;
    rows = m_accountModel->rowCount();
    for (int i = 0; i < rows; ++i)
    {
        totalValue += m_accountModel->data(m_accountModel->index(i,ACCOUNT_CASHAMOUNT),Qt::DisplayRole).toDouble();
        totalValue += m_accountModel->data(m_accountModel->index(i,ACCOUNT_CHEQUEAMOUNT),Qt::DisplayRole).toDouble();
        totalValue += m_accountModel->data(m_accountModel->index(i,ACCOUNT_VISAAMOUNT),Qt::DisplayRole).toDouble();
        totalValue += m_accountModel->data(m_accountModel->index(i,ACCOUNT_INSURANCEAMOUNT),Qt::DisplayRole).toDouble();
        totalValue += m_accountModel->data(m_accountModel->index(i,ACCOUNT_OTHERAMOUNT),Qt::DisplayRole).toDouble();
    }
    return totalValue;
}

MovementModel *LedgerIO::getModelMonthlyMovementsIO(QObject * parent, const QString &month, const QString &year)
{
    Q_UNUSED(parent);
    QDateTime start(QDate(year.toInt(), month.toInt(), 1), QTime(0,0));
    QDateTime end(QDate(year.toInt(), month.toInt(), start.date().daysInMonth()), QTime(23,59,59));
    //m_movementModel->clearAllFilters();
    //m_movementModel->setDateFilter(start, end);
    m_movementModel->setDatesBeginEndAndUserFilter(start,end,m_userUuid);
    while (m_movementModel->canFetchMore())
    {
          m_movementModel->fetchMore();
        }
    return m_movementModel;
}

QStandardItemModel *LedgerIO::getModelMonthlyAndTypeMovementsIO(QObject *parent, const QString &month, const QString &year)
{
    QStandardItemModel * model = new QStandardItemModel(parent);
    QHash<QString,double> hash;
    int rows = 0;
    QDateTime start(QDate(year.toInt(), month.toInt(), 1), QTime(0,0));
    QDateTime end(QDate(year.toInt(), month.toInt(), start.date().daysInMonth()), QTime(23,59,59));
    m_movementModel->setDatesBeginEndAndUserFilter(start,end,m_userUuid);
    while (m_movementModel->canFetchMore())
    {
          m_movementModel->fetchMore();
        }
    if (WarnDebugMessage)
    qDebug() << __FILE__ << QString::number(__LINE__) << " movementModel filter =" << m_movementModel->filter() ;
    rows = m_movementModel->rowCount();
    if (WarnDebugMessage)
    qDebug() << __FILE__ << QString::number(__LINE__) << " rows =" << QString::number(rows) ;
    for (int i = 0; i < rows; ++i) {
        QString labelOfMovement = m_movementModel->data(m_movementModel->index(i,MOV_LABEL/*MovementModel::Label*/),Qt::DisplayRole).toString();
        if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " labelOfMovement =" << labelOfMovement ;
        double value = m_movementModel->data(m_movementModel->index(i,MOV_AMOUNT/*MovementModel::Amount*/),Qt::DisplayRole).toDouble();
        if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " value =" << QString::number(value) ;
        hash.insertMulti(labelOfMovement,value);
    }
    QStringList keysList = hash.uniqueKeys();
    QString keyStr;
    foreach(keyStr,keysList){
        double valueAssocWithKey = 0.00;
        QList<double> valuesList = hash.values(keyStr);
        for (int i = 0; i < valuesList.size(); ++i) {
            valueAssocWithKey += valuesList[i];
        }
        QStandardItem * itemLabel = new QStandardItem(keyStr);
        QStandardItem * itemAmount = new QStandardItem(QString::number(valueAssocWithKey));
        QList<QStandardItem *> items;
        items << itemLabel << itemAmount;
        model->appendRow(items);
    }
    return model;
}

QStandardItemModel * LedgerIO::getModelYearlyAndTypeMovementsIO(QObject * parent, const QString &year){
    QStandardItemModel * model = new QStandardItemModel(parent);
    QHash<QString,double> hash;
    int rows = 0;
    QDate first(year.toInt(),01,01);
    QDate last(year.toInt(),12,31);
    QDateTime start(first);
    QDateTime end(last);
    m_movementModel->setDatesBeginEndAndUserFilter(start,end,m_userUuid);
    while (m_movementModel->canFetchMore())
    {
          m_movementModel->fetchMore();
        }
    rows = m_movementModel->rowCount();
    for (int i = 0; i < rows; ++i) {
        QString labelOfMovement = m_movementModel->data(m_movementModel->index(i,MOV_LABEL),Qt::DisplayRole).toString();
        double value = m_movementModel->data(m_movementModel->index(i,MOV_AMOUNT),Qt::DisplayRole).toDouble();
        hash.insertMulti(labelOfMovement,value);
    }
    QStringList keysList = hash.uniqueKeys();
    QString keyStr;
    foreach(keyStr,keysList){
        double valueAssocWithKey = 0.00;
        QList<double> valuesList = hash.values(keyStr);
        for (int i = 0; i < valuesList.size(); ++i) {
            valueAssocWithKey += valuesList[i];
        }
        QStandardItem * itemLabel = new QStandardItem(keyStr);
        QStandardItem * itemAmount = new QStandardItem(QString::number(valueAssocWithKey));
        QList<QStandardItem*> items;
        items << itemLabel << itemAmount;
        model->appendRow(items);
    }
    return model;
}
//for ledger
QStringList LedgerIO::getListOfSumsMonthlyReceiptsIO(QObject * parent,QString & dateBegin , QString & dateEnd)
{
    QStringList list;
    QString dateBeginStr = dateBegin;
    QString dateEndStr = dateEnd;
    QString filter = QString("%1 = '%2'").arg("USER_UID",user()->uuid());
    filter += " AND ";
    filter += QString("DATE BETWEEN '%1' AND '%2'").arg(dateBeginStr,dateEndStr);
    AccountModel accountModel(parent);
    accountModel.setFilter(filter);
    while (accountModel.canFetchMore())
    {
          accountModel.fetchMore();
        }
    double sum = 0.00;
    double cash = 0.00;
    double checks = 0.00;
    double creditCard = 0.00;
    double banking = 0.00;
    int rowsNumber = accountModel.rowCount();
    for (int i = 0; i < rowsNumber; ++i) {
        double cashPartial = accountModel.data(accountModel.index(i,ACCOUNT_CASHAMOUNT),Qt::DisplayRole).toDouble();
        cash        += cashPartial;
        /*if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << "cash  =" << QString::number(cash) ;*/
        double checksPartial = accountModel.data(accountModel.index(i,ACCOUNT_CHEQUEAMOUNT),Qt::DisplayRole).toDouble();
        checks      += checksPartial;
        /*if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << "checks  =" << QString::number(checks) ;*/
        double creditCardPartial = accountModel.data(accountModel.index(i,ACCOUNT_VISAAMOUNT),Qt::DisplayRole).toDouble();
        creditCard  += creditCardPartial;
        /*if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << "creditCard  =" << QString::number(creditCard) ;*/
        double bankingPartial = accountModel.data(accountModel.index(i,ACCOUNT_INSURANCEAMOUNT),Qt::DisplayRole).toDouble();
        banking     += bankingPartial;
        /*if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << "banking  =" << QString::number(banking)  ;*/
        /*if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " sum =" << QString::number(sum) ;*/
        sum += cashPartial+checksPartial+creditCardPartial+bankingPartial;
        /*if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " sum =" << QString::number(sum) ;*/
    }
    /*if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " sumsReceipts =" << QString::number(sum) ;*/
    list << QString::number(sum)
         << QString::number(cash)
         << QString::number(checks)
         << QString::number(creditCard)
         << QString::number(banking);
    return list;
}

QStringList LedgerIO::getListOfSumsMonthlyMovementsIO(QObject * parent, const QString &dateBegin , const QString &dateEnd)
{
    Q_UNUSED(parent);
    QStringList list;
    QHash<QString,double> hash;
    int rows = 0;
    double totalMovementValue = 0.00;
    QDateTime start = QDateTime::fromString(dateBegin, Qt::ISODate);
    QDateTime end = QDateTime::fromString(dateEnd, Qt::ISODate);
    m_movementModel->setDatesBeginEndAndUserFilter(start,end,m_userUuid);

    rows = m_movementModel->rowCount();
    if (WarnDebugMessage)
    qDebug() << __FILE__ << QString::number(__LINE__) << " rows =" << QString::number(rows) ;
    for (int i = 0; i < rows; ++i) {
        QString labelOfMovement = m_movementModel->data(m_movementModel->index(i,MOV_LABEL),Qt::DisplayRole).toString();
        if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " labelOfMovement =" << labelOfMovement ;
        if (labelOfMovement.isEmpty())
        {
              labelOfMovement = "NULL";
            }
        double value = m_movementModel->data(m_movementModel->index(i,MOV_AMOUNT),Qt::DisplayRole).toDouble();
        if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " value =" << QString::number(value) ;
        hash.insertMulti(labelOfMovement,value);
    }
    QStringList keysList = hash.uniqueKeys();
    QString keyStr;
    foreach(keyStr,keysList){
        double valueAssocWithKey = 0.00;
        QList<double> valuesList = hash.values(keyStr);
        for (int i = 0; i < valuesList.size(); ++i) {
            valueAssocWithKey += valuesList[i];
        }
        totalMovementValue += valueAssocWithKey;
        QString strPair = keyStr+"="+QString::number(valueAssocWithKey);
        if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " strPair =" << strPair ;
        list << strPair;

    }
    QString totalString = tr("Total");
    QString totalMovementValueStr = QString::number(totalMovementValue);
    list.append(totalString+"="+totalMovementValueStr);
    return list;
}

QStringList LedgerIO::listOfReceiptsTypes(){
    QStringList list;
    AccountModel accountModel(this);
    //accountModel.setFilterUserUuid();
    accountModel.setFilter("");
    while (accountModel.canFetchMore())
    {
          accountModel.fetchMore();
        }
    int rowsNumber = accountModel.rowCount();
    for (int i = 0; i < rowsNumber; ++i) {
        QString type = accountModel.data(accountModel.index(i,ACCOUNT_MEDICALPROCEDURE_TEXT),Qt::DisplayRole)
                       .toString();
        if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " type =" << type ;
        list << type;
    }
    return list;
}

QStringList LedgerIO::listOfMovementsTypes(){
    QStringList list;
    int rowsNumber = m_movementModel->rowCount();
    for (int i = 0; i < rowsNumber; ++i) {
        QString type = m_movementModel->data(m_movementModel->index(i,MOV_LABEL),Qt::DisplayRole)
                       .toString();
        list << type;
    }
    return list;
}

QList<QVector<QString> > LedgerIO::getDataReceiptsInVector(const QString & dateBegin,const QString & dateEnd){
    QList<QVector<QString> > tableLedgerMonth;
    QString filter = QString("%1 = '%2'").arg("USER_UID",user()->uuid());
    filter += " AND ";
    filter += QString("DATE BETWEEN '%1' AND '%2'").arg(dateBegin,dateEnd);
    //QString filter = "DATE BETWEEN '2011-01-01' AND '2011-01-31'";
    AccountModel accountModel(this);
    accountModel.setFilter(filter);
    while (accountModel.canFetchMore())
    {
          accountModel.fetchMore();
        }
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " filter =" << accountModel.filter() ;
    int rows = accountModel.rowCount();
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " rowCount =" << QString::number(rows) ;
    for (int i = 0; i < rows; ++i)
    {
        QString vectorDate = accountModel.data(accountModel.index(i,ACCOUNT_DATE),Qt::DisplayRole).toString();
        QString vectorPatient = accountModel.data(accountModel.index(i,ACCOUNT_PATIENT_NAME),Qt::DisplayRole)
                                                                                                     .toString();
        QString vectorCash = accountModel.data(accountModel.index(i,ACCOUNT_CASHAMOUNT),Qt::DisplayRole)
                                                                                                .toString();
        QString vectorChecks = accountModel.data(accountModel.index(i,ACCOUNT_CHEQUEAMOUNT),Qt::DisplayRole)
                                                                                                   .toString();
        QString vectorVisa = accountModel.data(accountModel.index(i,ACCOUNT_VISAAMOUNT),Qt::DisplayRole).toString();
        QString vectorBanking = accountModel.data(accountModel.index(i,ACCOUNT_INSURANCEAMOUNT),Qt::DisplayRole)
                                                                                                         .toString();
        QString vectorActs = accountModel.data(accountModel.index(i,ACCOUNT_MEDICALPROCEDURE_TEXT),Qt::DisplayRole)
                                                                                                 .toString();//acts
        QVector<QString> vector;
        vector << vectorDate
               << vectorPatient
               << vectorCash
               << vectorChecks
               << vectorVisa
               << vectorBanking
               << vectorActs;
        tableLedgerMonth << vector;
    }
//on a le tableau date,patient,esp,chq,cb,virement,acte du mois
    return tableLedgerMonth;
}

QList<QVector<QString> > LedgerIO::getDataMovementsInVector(const QString &dateBegin, const QString &dateEnd)
{
    QList<QVector<QString> > tableLedgerMonth;
    QString totalAmount;
    double total = 0.00;

    QDateTime start = QDateTime::fromString(dateBegin, Qt::ISODate);
    QDateTime end = QDateTime::fromString(dateEnd, Qt::ISODate);
    m_movementModel->setDatesBeginEndAndUserFilter(start,end,m_userUuid);
    while (m_movementModel->canFetchMore())
    {
          m_movementModel->fetchMore();
        }
    int rows = m_movementModel->rowCount();
    for (int i = 0; i < rows; ++i) {
        QString vectorDate = m_movementModel->data(m_movementModel->index(i,MOV_DATE),Qt::DisplayRole).toString();
        QString vectorLabel = m_movementModel->data(m_movementModel->index(i,MOV_LABEL),Qt::DisplayRole).toString();
        QString vectorAmount = m_movementModel->data(m_movementModel->index(i,MOV_AMOUNT),Qt::DisplayRole).toString();
        QString vectorComment = m_movementModel->data(m_movementModel->index(i,MOV_COMMENT),Qt::DisplayRole).toString();
        QString vectorDetails = m_movementModel->data(m_movementModel->index(i,MOV_DETAILS),Qt::DisplayRole).toString();
        total += vectorAmount.toDouble();
        QVector<QString> vector;
        vector << vectorDate << vectorLabel << vectorAmount << vectorComment << vectorDetails;
        tableLedgerMonth << vector;
    }
    totalAmount = QString::number(total);
    QVector<QString> totalVector;
    totalVector << "1961-02-06" << "Total" << totalAmount << "no comment" << "no details";
    tableLedgerMonth.append(totalVector);
    return tableLedgerMonth;
}

double LedgerIO::getMovementSum(QObject * parent,QString & month, QString & year){
    Q_UNUSED(parent);
    double totalValue = 0.00;
    QString dateBeginStr ;
    QDate dateBegin ;
    int lastDayInMonth = 0;
    QString dateEndStr = year+"-"+month+"-"+QString::number(lastDayInMonth);
    if (month == "0")
    {
         dateBeginStr = year+"-01-01";
         dateEndStr = year+"-12-31";
        }
    else{
        dateBeginStr = year+"-"+month+"-01";
        dateBegin = QDate::fromString(dateBeginStr,"yyyy-MM-dd");
        lastDayInMonth = dateBegin.daysInMonth();
        dateEndStr = year+"-"+month+"-"+QString::number(lastDayInMonth);
        }
    QString filter = QString("%1='%2'").arg("USER_UID",m_userUuid);
    filter += " AND ";
    filter += QString("DATEVALUE BETWEEN '%1' AND '%2'").arg(dateBeginStr,dateEndStr);
    m_movementModel->setFilter(filter);
    while (m_movementModel->canFetchMore())
    {
          m_movementModel->fetchMore();
        }
    int rows = 0;
    rows = m_movementModel->rowCount();
    for (int i = 0; i < rows; i += 1)
    {
        totalValue += m_movementModel->data(m_movementModel->index(i,MOV_AMOUNT),Qt::DisplayRole).toDouble();
    }
    return totalValue;
}

