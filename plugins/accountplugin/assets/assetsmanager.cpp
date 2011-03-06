#include "assetsmanager.h"

#include <accountbaseplugin/constants.h>

#include <QDate>

#include <QDebug>


using namespace AccountDB;
using namespace Constants;

AssetsManager::AssetsManager()
{}

AssetsManager::~AssetsManager()
{}

QHash<int,QVariant> AssetsManager::getHashOfValues(QString &userUid,
                                                   int accountId,
                                                   QString &label,
                                                   QString &date,
                                                   double duration,
                                                   int mode,
                                                   double value,
                                                   double taxActs,
                                                   double residualValue,
                                                   QVariant &result,
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
    hash.insert(ASSETS_RESIDUAL_VALUE,residualValue);
    hash.insert(ASSETS_RESULT,result);
    hash.insert(ASSETS_MOVEMENT,movement);
    hash.insert(ASSETS_COMMENT,comments);
    hash.insert(ASSETS_TRACE,trace);
    return hash;
}

QHash<int,QString> AssetsManager::getHashForModeComboBox(){
    QHash<int,QString> hash;
    hash.insert(1,trUtf8("strait line depreciation"));
    hash.insert(2,trUtf8("declining balance"));
    return hash;
}

double AssetsManager::getResidualValue(QString &dateBegin , double value,int mode,double duration){
    double residualValue = 0.00;
    switch (mode) {
        case LINEAR_MODE :
            residualValue = linearCalc(value , duration , dateBegin);
            break;
        case DECREASING_MODE :
            residualValue = decreasingCalc(value , duration , dateBegin);
            break;
        default :
            break;
        }
        
    return residualValue;
}

double AssetsManager::linearCalc(double value , double  duration , QString &date){
  QDate currentDate = QDate::currentDate();
  int currDateYear = currentDate.year();
  QDate dateduration = QDate::fromString(date,"yyyy-MM-dd");
  int  durationInt = dateduration.year();
  double yearlyValue = value/duration;
  int     differenceDates       = currDateYear - durationInt ;
  if(differenceDates < 0 || differenceDates > duration){
     yearlyValue = 0 ;
  }
  return yearlyValue;
}

double AssetsManager::decreasingCalc(double  val , double  duration , QString &date){
    double     value          = val;
    double      ans             = duration;
    QDate       dateDebut       = QDate::fromString(date,"yyyy-MM-dd");
    QDate       dateCourante    = QDate::currentDate();
    double     valueAnn       = 0.00 ;
    
    //-----------------premiere annee----------------------------------
    double      taux            = 1.25;
    if(ans > 4 &&ans < 7){
        taux                    = 1.75;
    }
    if(ans > 6){
        taux                    = 2.25;
    }
    int       mois              = dateDebut.month();
    int       dateDebutAn       = dateDebut.year();
    int       dateCourAn        = dateCourante.year();
    qDebug() << "mois decreasingCalc ="+QString::number(mois);
    qDebug() << "dateDebutAn ="+QString::number(dateDebutAn);
    qDebug() << "dateCourAn ="+QString::number(dateCourAn);
    //----------------premiere annee---------------------------------------
    
    double    nbreMois          = double(12 - mois);
    double    tauxDegrCent      = 100/ans * taux;
    qDebug() << "tauxDegrCent ="+QString::number(tauxDegrCent);
    double    tauxDegr          = (tauxDegrCent)/100;
    qDebug() << "tauxDegr ="+QString::number(tauxDegr);
    double  valueAnn1         = (value * tauxDegr) * (nbreMois/12) ;
              qDebug() << "valueAnn1 ="+QString::number(valueAnn1);
    double    valResid        = value -valueAnn1;
              
    //------------------autres annees--------------------------------------
    for(int i = 1 ; i < int(ans) ; i++){
        ans --;
        if(tauxDegrCent < 100/ans){
            tauxDegr             = 1/ans;
        }
        double valueAnnFor      = valResid * tauxDegr;
        qDebug() << "valueAnn ="+QString::number(valueAnnFor);
        valResid                -=valueAnnFor;
        qDebug() << "valueResid ="+QString::number(valResid);
        if(dateCourAn - dateDebutAn == i){
           valueAnn            =valueAnnFor ;
        }
        
    }
    if(dateCourAn - dateDebutAn < 1){
       valueAnn = valueAnn1 ;
    }

    return valueAnn;
}


