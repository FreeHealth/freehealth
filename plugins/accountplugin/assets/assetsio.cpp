/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "assetsio.h"

#include <accountbaseplugin/assetmodel.h>
#include <accountbaseplugin/movementmodel.h>
#include <accountbaseplugin/bankaccountmodel.h>
#include <accountbaseplugin/assetsratesmodel.h>
#include <accountbaseplugin/constants.h>

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/itheme.h>
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

static inline Core::IUser *user() { return  Core::ICore::instance()->user(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }

AssetsIO::AssetsIO(QObject *parent) :
    QObject(parent)
{
    m_assetModel = new AccountDB::AssetModel(parent);
    m_user_uid = user()->uuid();
}

AssetsIO::~AssetsIO()
{
}

AssetModel * AssetsIO::getModelAssets(){
    return m_assetModel;
}

QString AssetsIO::getUserUid(){
    return m_assetModel->m_UserUid;
}

bool AssetsIO::insertIntoAssets(const QHash<int,QVariant> &hashValues)
{
    bool ret = true;
    int rowBefore = m_assetModel->rowCount(QModelIndex());
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " rowBefore = " << QString::number(rowBefore);
    if (m_assetModel->insertRows(rowBefore,1,QModelIndex())) {
        qWarning() << __FILE__ << QString::number(__LINE__) << "Row inserted !" ;
    }
    QVariant data;
    for(int i = 1 ; i < ASSETS_MaxParam ; i ++){
        data = hashValues.value(i);
        //qDebug() << __FILE__ << QString::number(__LINE__) << " data + i =" << data.toString()+" "+QString::number(i);
        if (!m_assetModel-> setData(m_assetModel->index(rowBefore,i), data ,Qt::EditRole)) {
            qWarning() << __FILE__ << QString::number(__LINE__) << " asset model error = " << m_assetModel->lastError().text() ;
        }
    }
    m_assetModel->submit();
    if (m_assetModel->rowCount(QModelIndex()) == rowBefore) {
        Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("AssetsIO error: ") + m_assetModel->lastError().text());
        ret = false;
    }
    return ret;
}

bool AssetsIO::insertIntoMovements(const QHash<int,QVariant> &hashValues)
{
    bool ret = true;
    MovementModel modelMovements(this);
    double value = 0.00;
    int type = 2;
    QString bank;
    int rowBefore = modelMovements.rowCount(QModelIndex());
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " rowBefore = " << QString::number(rowBefore);
    if (modelMovements.insertRows(rowBefore,1,QModelIndex())) {
        qWarning() << __FILE__ << QString::number(__LINE__) << "Row inserted !" ;
    }
    QVariant data;
    for(int i = 1 ; i < MOV_MaxParam ; i ++) {
        data = hashValues.value(i);
        if (i == MOV_AMOUNT)
        {
            value = data.toDouble();
        }
        if (i == MOV_TYPE)
        {
            type = data.toInt();
        }
        if (i == MOV_ACCOUNT_ID)
        {
            int bankId = data.toInt();
            bank = getBankNameFromId(bankId);
            if (WarnDebugMessage)
                qDebug() << __FILE__ << QString::number(__LINE__) << " bank =" << bank ;
        }
        //qDebug() << __FILE__ << QString::number(__LINE__) << " data + i =" << data.toString()+" "+QString::number(i);
        if (!modelMovements. setData(modelMovements.index(rowBefore,i), data ,Qt::EditRole)) {
            qWarning() << __FILE__ << QString::number(__LINE__) << " model account error = "
                       << modelMovements.lastError().text() ;
        }
    }
    modelMovements.submit();
    if (modelMovements.rowCount(QModelIndex()) == rowBefore) {
        Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("AssetsIO error: ") + modelMovements.lastError().text());
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

bool AssetsIO::debitOrCreditInBankBalance(const QString &bank, double value){
    bool ret = true;
    BankAccountModel model(this);
    int row = 0;
    QList<int> rowsTestList;
    for (int i = 0; i < model.rowCount(); i += 1) {
        QString bankLabel = model.data(model.index(i,BANKDETAILS_LABEL),Qt::DisplayRole).toString();
        if (bankLabel == bank) {
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
    if (!model.setData(model.index(row,BANKDETAILS_BALANCE),newBalance,Qt::EditRole))  {
        qWarning() << __FILE__ << QString::number(__LINE__) << "Unable to insert balance data !" ;
    }
    if (!model.setData(model.index(row,BANKDETAILS_BALANCEDATE),date,Qt::EditRole)) {
        qWarning() << __FILE__ << QString::number(__LINE__) << "Unable to insert balance new date !" ;
    }
    if (!model.submit()) {
        ret = false;
    }
    return ret;
}

QString AssetsIO::getBankNameFromId(int id){
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

int AssetsIO::getIdFromBankName(const QString & bankName){
    int id = 0;
    QString bank = bankName;
    if (bankName.contains("'")) {
        bank.replace("'","''");
    }

    BankAccountModel bankmodel(this);
    QString filter = QString("%1 = '%2'").arg("BD_LABEL",bank);
    bankmodel.setFilter(filter);
    id = bankmodel.data(bankmodel.index(0,BANKDETAILS_ID),Qt::DisplayRole).toInt();
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " filter =" << bankmodel.filter() ;
    return id;
}

QStandardItemModel * AssetsIO::getBankComboBoxModel(QObject * parent){
    QStandardItemModel *model = new QStandardItemModel(parent);
    BankAccountModel bankmodel(this);
    QString filterUserAndpreferred = QString("BD_USER_UID = '%1' AND BD_ISDEFAULT = '%2'").arg(m_user_uid,1);
    QString filterUser = QString("BD_USER_UID = '%1'").arg(m_user_uid);
    int rows = bankmodel.rowCount();
    for (int i = 0; i < rows; i += 1)
    {
        QString bankLabel = bankmodel.data(bankmodel.index(i,BANKDETAILS_LABEL),Qt::DisplayRole).toString();
        QString bankDefault = bankmodel.data(bankmodel.index(i,BANKDETAILS_DEFAULT),Qt::DisplayRole).toString();
        QStandardItem *item = new QStandardItem(bankLabel);
        QIcon icon;
        if (bankDefault == "1") {
            icon = QIcon(theme()->icon(Core::Constants::ICONOK));
            item->setIcon(icon);
            if (WarnDebugMessage)
                qDebug() << __FILE__ << QString::number(__LINE__) << " item def =" << item->text() ;
            }
            model->appendRow(item);
    }
    /*for (int i = 0; i < rows; i += 1)
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
    }*/
    return model;
}

bool AssetsIO::deleteAsset(int row)
{
    bool b = true;
    if (!m_assetModel->removeRow(row,QModelIndex())) {
        b = false;
    }
    return b;
}

QStandardItemModel * AssetsIO::getListsOfValuesForRefresh(QObject * parent){
    QStandardItemModel * model = new QStandardItemModel(parent);
    for (int i = 0; i < m_assetModel->rowCount(); i += 1)
    {
        QString dateBeginStr = m_assetModel->data(m_assetModel->index(i,ASSETS_DATE),Qt::DisplayRole).toString();
        QString mode = m_assetModel->data(m_assetModel->index(i,ASSETS_MODE),Qt::DisplayRole).toString();
        QString beginValue = m_assetModel->data(m_assetModel->index(i,ASSETS_VALUE),Qt::DisplayRole).toString();
        QString duration = m_assetModel->data(m_assetModel->index(i,ASSETS_DURATION),Qt::DisplayRole).toString();
        QString yearsToRun = m_assetModel->data(m_assetModel->index(i,ASSETS_YEARS),Qt::DisplayRole).toString();
        QStandardItem * itemDate = new QStandardItem(dateBeginStr);
        QStandardItem * itemMode = new QStandardItem(mode);
        QStandardItem * itemValue = new QStandardItem(beginValue);
        QStandardItem * itemDuration = new QStandardItem(duration);
        QStandardItem * itemYearsToRun = new QStandardItem(yearsToRun);
        QList<QStandardItem*> list;
        list << itemDate << itemMode << itemValue << itemDuration << itemYearsToRun ;
        model->appendRow(list);
    }
    return model;
}

int AssetsIO::getLastMovementId(){
    int lastId = -1;
    MovementModel mov(this);
    lastId = mov.data(mov.index(mov.rowCount()-1,MOV_ID),Qt::DisplayRole).toInt();
    return lastId;
}

bool AssetsIO::deleteMovement(int idMovement,int idBank){
    bool ret = true;
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " idMovement =" << QString::number(idMovement) ;
    MovementModel movModel(this);
    QString filter = QString("%1 = '%2'").arg("MOV_ID",QString::number(idMovement));
    movModel.setFilter(filter);
    double value = movModel.data(movModel.index(0,MOV_AMOUNT),Qt::DisplayRole).toDouble();
    if (creditValueDeletedToBankAccount(value,idBank)) {
        Utils::informativeMessageBox(tkTr(Trans::Constants::INFORMATION), tr("Credited value = ") + QString::number(value));
    }
    if (!movModel.removeRows(0,1,QModelIndex())) {
        Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("Unable to delete movement of this asset."));
        ret = false;
    }
    // TODO: add value deleted to bank and delete asset without delete movement?
    return ret;
}

int AssetsIO::getMovementId(int row){
    return m_assetModel->data(m_assetModel->index(row,ASSETS_MOVEMENT),Qt::DisplayRole).toInt();
}

bool AssetsIO::creditValueDeletedToBankAccount(double value, int idBank){
    bool ret = true;
    BankAccountModel bankmodel(this);
    QString filter = QString("%1 = '%2'").arg("BD_ID",QString::number(idBank));
    bankmodel.setFilter(filter);
    double newvalue = bankmodel.data(bankmodel.index(0,BANKDETAILS_BALANCE),Qt::DisplayRole).toDouble() + value;
    if (!bankmodel.setData(bankmodel.index(0,BANKDETAILS_BALANCE),newvalue,Qt::EditRole)) {
        Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("Unable to credit the value in bank balance."));
    }
    return ret;
}

double AssetsIO::getResidualValueWhenRefresh(int row){
    double residualValue = 0.00;
    AssetModel model(this);
    residualValue = model.data(model.index(row,ASSETS_RESIDUAL_VALUE),Qt::DisplayRole).toDouble();
    return residualValue;
}

bool AssetsIO::deleteOneYearToRun(int row){
    bool ret = true;
    AssetModel model(this);
    int yearsToRun = model.data(model.index(row,ASSETS_YEARS),Qt::DisplayRole).toInt();
    yearsToRun--;
    if (!model.setData(model.index(row,ASSETS_YEARS),yearsToRun,Qt::EditRole))
    {
        qWarning() << __FILE__ << QString::number(__LINE__) << "Error = "+model.lastError().text() ;
    }
    if (!model.submit())
    {
        ret = false;
    }
    return ret;
}

double AssetsIO::getRate(const QDate &date, double duration) {
    Q_UNUSED(date);
    double rate = 0.00;
    QHash<QString,QDate> hashRatesDates;
    QStringList listChosenOfRanges;
    AssetsRatesModel model(this);
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " model.rowCount() =" << QString::number(model.rowCount()) ;
    for (int i = 0; i < model.rowCount(); i += 1) {
        QDate dateRequest = model.data(model.index(i,ASSETSRATES_DATE),Qt::DisplayRole).toDate();
        QString rangeReq = model.data(model.index(i,ASSETSRATES_YEARS),Qt::DisplayRole).toString();
        QString rate = model.data(model.index(i,ASSETSRATES_RATES),Qt::DisplayRole).toString();
        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << " rangeReq and rate =" << rangeReq+" "+rate ;
        QStringList listOfRanges = rangeReq.split("_");
        if (int(duration) >= listOfRanges[0].toInt() && int(duration) <= listOfRanges[1].toInt()) {
            hashRatesDates.insertMulti(rate,dateRequest) ;
        }
    }
    QList<QDate> valuesOfDates = hashRatesDates.values();
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " valuesOfDates size =" << QString::number(valuesOfDates.size()) ;
    if (hashRatesDates.size() < 1) {
        Utils::warningMessageBox(tkTr(Trans::Constants::ERROR),
                                 tr("You have to fill defaults for assets rates.\nGo "
                                    "in Configuration>Preferences to do so.\n"
                                    "Otherwise result will be wrong !"));
        return 1.00;
    }
    qSort(valuesOfDates.begin(),valuesOfDates.end());

    QDate nearestDate = valuesOfDates.last();
    QString nearestDateStr = nearestDate.toString("yyyy-MM-dd");
    QString rateStr = hashRatesDates.key(nearestDate);
    rate = rateStr.toDouble();
    qWarning() << __FILE__ << QString::number(__LINE__) << "rate = " << QString::number(rate) ;
    return rate;
}

QStandardItemModel * AssetsIO::getYearlyValues(const QDate & year, QObject * parent){
    QStandardItemModel *model = new QStandardItemModel(parent);
    QString yearStr = year.toString("yyyy");
    QString dateBegin = yearStr+"-01-01";
    QString dateEnd = yearStr+"-12-31" ;
    QString filter = QString("%1 = '%2' AND %3 BETWEEN '%4' AND '%5'").arg("USER_UID",
                                                                           m_user_uid,
                                                                           "DATE",
                                                                           dateBegin,
                                                                           dateEnd);
    AssetModel assetModel(this);
    assetModel.setFilter(filter);
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " assetModel filter =" << assetModel.filter() ;
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << "model row = " << assetModel.rowCount();
    for (int i = 0; i < assetModel.rowCount(); i += 1)
    {
        QString label = assetModel.data(assetModel.index(i,ASSETS_LABEL),Qt::DisplayRole).toString();
        QString value = assetModel.data(assetModel.index(i,ASSETS_VALUE),Qt::DisplayRole).toString();
        QString mode = assetModel.data(assetModel.index(i,ASSETS_MODE),Qt::DisplayRole).toString();
        QString duration = assetModel.data(assetModel.index(i,ASSETS_DURATION),Qt::DisplayRole).toString();
        QString date = assetModel.data(assetModel.index(i,ASSETS_DATE),Qt::DisplayRole).toString();
        QStandardItem * itemLabel = new QStandardItem(label);
        QStandardItem * itemValue = new QStandardItem(value);
        QStandardItem * itemMode = new QStandardItem(mode);
        QStandardItem * itemDuration = new QStandardItem(duration);
        QStandardItem * itemDate = new QStandardItem(date);
        QList<QStandardItem*> listOfItems;
        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__);

        listOfItems << itemLabel << itemValue << itemMode << itemDuration << itemDate;
        model->appendRow(listOfItems);
    }
    return model;
}

double AssetsIO::getValueFromRow(int row){
    double value = 0.00;
    AssetModel model(this);
    value = model.data(model.index(row,ASSETS_VALUE),Qt::DisplayRole).toDouble();
    return value;
}

int AssetsIO::getModeFromRow(int row){
    int mode = 0;
    AssetModel model(this);
    mode = model.data(model.index(row,ASSETS_MODE),Qt::DisplayRole).toInt();
    return mode;
}

double AssetsIO::getDurationFromRow(int row){
    double duration = 0.00;
    AssetModel model(this);
    duration = model.data(model.index(row,ASSETS_DURATION),Qt::DisplayRole).toDouble();
    return duration;

}

QDate AssetsIO::getDateFromRow(int row){
    QDate date;
    AssetModel model(this);
    date = model.data(model.index(row,ASSETS_DATE),Qt::DisplayRole).toDate();
    return date;
}

QString AssetsIO::getLabelFromRow(int row){
    QString label;
    AssetModel model(this);
    label = model.data(model.index(row,ASSETS_LABEL),Qt::DisplayRole).toString();
    return label;
}
