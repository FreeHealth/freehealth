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
#include "assetsmanager.h"
#include "assetsio.h"
#include <accountbaseplugin/constants.h>
#include <accountbaseplugin/assetmodel.h>

#include <QDate>
#include <QDebug>
enum { WarnDebugMessage = false };
using namespace AccountDB;
using namespace Constants;

AssetsManager::AssetsManager()
{}

AssetsManager::~AssetsManager()
{}

QHash<int,QVariant> AssetsManager::getHashOfValues(QString &userUid,
                                                   int & accountId,
                                                   QString &label,
                                                   QString &date,
                                                   double & duration,
                                                   int & mode,
                                                   double & value,
                                                   double & taxActs,
                                                   double & yearlyResult,
                                                   double & residualValue,
                                                   int  yearsToRun,
                                                   double & rate,
                                                   QVariant &movement,
                                                   QString &comments,
                                                   QVariant &trace){
    QHash<int, QVariant> hash;
    hash.insert(ASSETS_USER_UID,userUid);
    hash.insert(ASSETS_ACCOUNT_ID,accountId);
    hash.insert(ASSETS_LABEL,label);
    hash.insert(ASSETS_DATE,date);
    hash.insert(ASSETS_DURATION,duration);
    hash.insert(ASSETS_MODE,mode);
    hash.insert(ASSETS_VALUE,value);
    hash.insert(ASSETS_TAXEACTS,taxActs);
    hash.insert(ASSETS_YEARLY_RESULT,yearlyResult);
    hash.insert(ASSETS_RESIDUAL_VALUE,residualValue);
    hash.insert(ASSETS_YEARS,yearsToRun);
    hash.insert(ASSETS_RATE,rate);
    hash.insert(ASSETS_MOVEMENT,movement);
    hash.insert(ASSETS_COMMENT,comments);
    hash.insert(ASSETS_TRACE,trace);
    return hash;
}

QHash<int,QVariant> AssetsManager::getHashOfValuesMovements(
                                            int acMovId,
                                            QString &userUid,
                                            int bankId,
                                            int type,
                                            QString &label,
                                            QString &date,
                                            QString &dateValue,
                                            double valueCalculated,
                                            QString &comment,
                                            int validity,
                                            QString &trace,
                                            int isValid,
                                            QString &details){
    QHash<int,QVariant> hash;
    hash.insert(MOV_AV_MOVEMENT_ID,acMovId);
    hash.insert(MOV_USER_UID,userUid);
    hash.insert(MOV_ACCOUNT_ID,bankId);
    hash.insert(MOV_TYPE,type);
    hash.insert(MOV_LABEL,label);
    hash.insert(MOV_DATE,date);
    hash.insert(MOV_DATEOFVALUE,dateValue);
    hash.insert(MOV_AMOUNT,valueCalculated);
    hash.insert(MOV_COMMENT,comment);
    hash.insert(MOV_VALIDITY,validity);
    hash.insert(MOV_TRACE,trace);
    hash.insert(MOV_ISVALID,isValid);
    hash.insert(MOV_DETAILS,details);
    return hash;
}

QHash<int,QString> AssetsManager::getHashForModeComboBox(){
    QHash<int,QString> hash;
    hash.insert(1, tr("strait line depreciation"));
    hash.insert(2, tr("declining balance"));
    return hash;
}

double AssetsManager::getYearlyValue(const QString &dateBegin , const QDate &currentDate, double value,int mode,double duration){
    double yearlyValue = 0.00;
     switch (mode) {
        case LINEAR_MODE :
            yearlyValue = linearCalc(value , duration , dateBegin , currentDate);
            break;
        case DECREASING_MODE :
            yearlyValue = decreasingCalc(0,value , duration , dateBegin , currentDate)[YEARLY_DECR_VALUE];
            break;
        default :
            break;
        }

    return yearlyValue;
}

double AssetsManager::getYearlyValue(const QString &year, int row){
    double yValue = 0.00;
    QString dateBegin = year+"-01-01";
    QString dateEnd = year+"-12-31" ;
    QDate currentDate = QDate::fromString(dateEnd,"yyyy-MM-dd");
    AssetsIO assetsIO(this);
    double value = assetsIO.getValueFromRow(row);
    int mode = assetsIO.getModeFromRow(row);
    double duration = assetsIO.getDurationFromRow(row);
    QDate date = assetsIO.getDateFromRow(row);
    QString dateYear = date.toString("yyyy");
    if (mode == DECREASING_MODE && dateYear == year)
    {
        dateBegin = date.toString("yyyy-MM-dd");
    }
    yValue = getYearlyValue(dateBegin ,currentDate ,value ,mode ,duration);
    return yValue;
}

double AssetsManager::linearCalc(double value , double  duration , const QString &beginDate, const QDate &refDate){
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " in linear calc " ;
    AssetsIO assetIO(this);
    QDate currentDate = refDate;
    int currDateYear = currentDate.year();
    QDate dateduration = QDate::fromString(beginDate,"yyyy-MM-dd");
    int  durationInt = dateduration.year();
    double yearlyValue = value/duration;
    int diffDates = currDateYear - durationInt ;
    if(diffDates < 0 || diffDates > duration){
       yearlyValue = 0 ;
       }
    return yearlyValue;
}

QList<double> AssetsManager::decreasingCalc(int row, double value , double duration , const QString &dateBegin, const QDate &refDate){
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " in decreasing calc " ;
    QList<double> list;
    AssetsIO assetIO(this);
    QDate       beginDate       = QDate::fromString(dateBegin,"yyyy-MM-dd");
    QDate       dateThisDay    = refDate;//QDate::currentDate();
    double     yearlyValue       = 0.00 ;
    double    residualValue         = 0.00;
    /*double*/ m_rate  = getRateFromAssetsRates(beginDate,duration);
    double rate = m_rate;
    int       month              = beginDate.month();
    int       beginDateYear       = beginDate.year();
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " beginDateYear =" << QString::number(beginDateYear) ;
    int       currentDateYear        = dateThisDay.year();
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " currDateYear =" << QString::number(currentDateYear) ;
    //----------------first year---------------------------------------

    double    numberOfMonths          = double(12 - month +1);//from the beginning of month
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " numberOfMonths =" << QString::number(numberOfMonths) ;
    double    decreasingRatePercent      = 100/duration * rate;
    double    decreasingRate          = (decreasingRatePercent)/100;

    int diffOfYears = currentDateYear - beginDateYear;
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " diffOfYears =" << QString::number(diffOfYears) ;
    if (diffOfYears == 0)
    {
        yearlyValue         = (value * decreasingRate) * (numberOfMonths/12) ;
        if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " yearlyValue =" <<  QString::number(yearlyValue);
        residualValue          = value - yearlyValue;
        }
    //------------------other years--------------------------------------
    else
    {
        double residualValueRefreshed = assetIO.getResidualValueWhenRefresh(row);
        if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << "old residualValue =" << QString::number(residualValueRefreshed) ;
        if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " decreasingRatePercent =" << QString::number(decreasingRatePercent) ;
        if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << "100/duration  =" << QString::number(100/duration) ;
        double newDuration = duration - diffOfYears;
        if(decreasingRatePercent < 100/newDuration){
            qWarning() << __FILE__ << QString::number(__LINE__) << "in lin calc" ;
            decreasingRate             = 1/newDuration;//we apply linear calc.
        }
        double newYearlyValue      = residualValueRefreshed * decreasingRate;
        double residualValueNew = residualValueRefreshed;
        if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " newYearlyValue =" << QString::number(newYearlyValue) ;
        residualValueNew  -= newYearlyValue;
        if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " residualValue =" << QString::number(residualValueNew) ;
           yearlyValue            = newYearlyValue ;
           residualValue = residualValueNew;
        }
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " yearlyValue =" << QString::number(yearlyValue) ;
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " residualValue =" << QString::number(residualValue) ;
    list << yearlyValue << residualValue;
    return list;
}

double AssetsManager::getCalculatedResidualValue(double value, double yearlyValue){
    double result = 0.00;
    result = value - yearlyValue;
    return result;
}

bool AssetsManager::setRefreshedResidualValue(){
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " in  setRefreshedResidualValue()"   ;
    bool ret = true;
    AssetsIO assetIO(this);
    AssetModel assetModel(this);
    QStandardItemModel * model = assetIO.getListsOfValuesForRefresh(this);//QString beginDate,int mode, double value, double duration,int yearsToRun
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " model rowCount =" << QString::number(model->rowCount()) ;
    for (int i = 0; i < model->rowCount(); i += 1)
    {
        double residualValue = 0.00;
        double yearlyValue = 0.00;
        QString beginDateStr = model->data(model->index(i,ITEM_ENUM_DATE),Qt::DisplayRole).toString();
        QDate beginDate = QDate::fromString(beginDateStr,"yyyy-MM-dd");
        QDate currentDate = QDate::currentDate();
        int daysFromBeginDateToCurrentDate = beginDate.daysTo(currentDate);
        int beginYear = beginDate.year();
        int currentYear = currentDate.year();
        double yearFromBeginDateToCurrentDate = daysFromBeginDateToCurrentDate/currentDate.daysInYear();
        int yearsFactor = (int)yearFromBeginDateToCurrentDate +1;
        int mode = model->data(model->index(i,ITEM_ENUM_MODE),Qt::DisplayRole).toInt();
        double value = model->data(model->index(i,ITEM_ENUM_VALUE),Qt::DisplayRole).toDouble();
        double duration = model->data(model->index(i,ITEM_ENUM_DURATION),Qt::DisplayRole).toDouble();
        int yearsToRun = model->data(model->index(i,ITEM_ENUM_YEARS),Qt::DisplayRole).toInt();
        int diffOfYears = currentYear - beginYear;
        int lessYearsToRun = duration - yearsToRun;
        if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__);
        if(lessYearsToRun != diffOfYears && yearsToRun > 0){
        QList<double> listFromDecreasingCalc;
        if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " mode =" << QString::number(mode) ;
        if (!assetIO.deleteOneYearToRun(i))
        {
              qWarning() << __FILE__ << QString::number(__LINE__) << "Unable to delete one year !" ;
            }
        switch(mode){
            case LINEAR_MODE :
                yearlyValue = linearCalc(value ,duration, beginDateStr , currentDate);
                residualValue = value - (yearlyValue*yearsFactor);
                if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " residualValue = " << QString::number(residualValue) ;
                break;
            case DECREASING_MODE :
                listFromDecreasingCalc = decreasingCalc(i,value ,duration, beginDateStr , currentDate);
                yearlyValue = listFromDecreasingCalc[YEARLY_DECR_VALUE];
                if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " yearlyValue = " << QString::number(yearlyValue) ;
                residualValue = listFromDecreasingCalc[1];
                break;
            default :
                break;
            }
        assetModel.setData(assetModel.index(i,ASSETS_YEARLY_RESULT),yearlyValue,Qt::EditRole);
        assetModel.setData(assetModel.index(i,ASSETS_RESIDUAL_VALUE),residualValue,Qt::EditRole);
        if (!assetModel.submit())
        {
              qWarning() << __FILE__ << QString::number(__LINE__) << "Residual value not replaced !" ;
              ret = false;
            }
        }
        }
        if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__);
    return ret;
}

double AssetsManager::getRate(const QDate &date,double duration,int mode){
    double rate = 0.00;
    switch(mode){
            case LINEAR_MODE :
                rate = 1/duration;
                break;
            case DECREASING_MODE :
                rate = getRateFromAssetsRates(date,duration);
                break;
            default :
                break;
            }
    return rate;
}

double AssetsManager::getRateFromAssetsRates(const QDate &date,double duration){
    double rate = 0.00;
    AssetsIO assetsIO(this);
    rate = assetsIO.getRate(date,duration);
    return rate;
}

QStandardItemModel * AssetsManager::getYearlyValues(const QDate &year){
    QStandardItemModel *model = new QStandardItemModel(this);
    QStandardItemModel *newModel = new QStandardItemModel(this);
    QString yearStr = year.toString("yyyy");
    QString dateBegin = yearStr+"-01-01";
    QString dateEnd = yearStr+"-12-31" ;
    QDate refDate = QDate::fromString(dateEnd,"yyyy-MM-dd");
    AssetsIO assetIO(this);
    model = assetIO.getYearlyValues(year,this);
    for (int i = 0; i < model->rowCount(); i += 1)
    {
        QString label = model->data(model->index(i,LABEL),Qt::DisplayRole).toString();
        double value = model->data(model->index(i,VALUE),Qt::DisplayRole).toDouble();
        int mode = model->data(model->index(i,MODE),Qt::DisplayRole).toInt();
        double duration = model->data(model->index(i,DURATION),Qt::DisplayRole).toDouble();
        QDate date = model->data(model->index(i,DATE),Qt::DisplayRole).toDate();
        QString dateYear = date.toString("yyyy");
        QString yearlyValue ;
        QStandardItem * itemLabel = new QStandardItem();
        QStandardItem * itemYearlyValue = new QStandardItem();
        QList<QStandardItem*> listOfItems;
        if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " dateYear =" << dateYear << "yearStr = "<< yearStr ;
        switch(mode){
            case LINEAR_MODE :
                yearlyValue = QString::number(linearCalc(value ,duration , dateBegin, refDate));
                break;
            case DECREASING_MODE :
                if (dateYear == yearStr)
                {
                    dateBegin = date.toString("yyyy-MM-dd");
                    }
                yearlyValue = QString::number(decreasingCalc(i,value ,duration , dateBegin, refDate)[YEARLY_DECR_VALUE]);
                break;
            default :
                break;
            }
        itemLabel->setText(label);
        itemYearlyValue->setText(yearlyValue);
        listOfItems << itemLabel << itemYearlyValue;
        newModel->appendRow(listOfItems);
        if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " yearlyValue in manager  =" << yearlyValue ;
        }

    return newModel;
}

QString AssetsManager::getLabelFromRow(int & row){
    QString label;
    AssetsIO assetsIO(this);
    label = assetsIO.getLabelFromRow(row);
    return label;
}
