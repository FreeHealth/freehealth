#ifndef ASSETSMANAGER_H
#define ASSETSMANAGER_H

#include <accountplugin/account_exporter.h>

#include <QHash>
#include <QVariant>
#include <QStandardItemModel>

class ACCOUNT_EXPORT AssetsManager : public QObject
{
    Q_OBJECT
public:
    AssetsManager();
    ~AssetsManager();
    enum itemEnum {
        ITEM_ENUM_DATE = 0,
        ITEM_ENUM_MODE,
        ITEM_ENUM_VALUE,
        ITEM_ENUM_DURATION,
        ITEM_ENUM_YEARS
        };
    enum itemValues {
        LABEL = 0,
        VALUE,
        MODE,
        DURATION,
        DATE
        };
    enum decreasingCalcValues {
        YEARLY_DECR_VALUE = 0,
        RESIDUAL_DECR_VALUE
        };
    QHash<int,QVariant> getHashOfValues(QString &userUid,
                                        int & accountId,
                                        QString &label,
                                        QString & date,
                                        double & duration,
                                        int & mode,
                                        double & value,
                                        double & taxActs,
                                        double & yearlyResult,
                                        double & residualValue,
                                        int  yearsToRun,
                                        double & rate,
                                        QVariant & movement,
                                        QString &comments,
                                        QVariant &trace);
    QHash<int,QVariant> getHashOfValuesMovements(
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
                                            QString &details);
    QHash<int,QString> getHashForModeComboBox();
    double getYearlyValue(const QString &dateBegin, const QDate &currentDate, double value,int mode, double duration);
    double getCalculatedResidualValue(double value, double yearlyValue);
    bool setRefreshedResidualValue();
    double getRate(const QDate &date,double duration,int mode);
    double getRateFromAssetsRates(const QDate &date,double duration);
    QStandardItemModel *getYearlyValues(const QDate &year);
    double getYearlyValue(const QString &year, int row);
    QString getLabelFromRow(int & row);
private:
    double linearCalc(double value, double  duration, const QString &beginDate, const QDate &refDate);
    QList<double> decreasingCalc(int row, double val, double duration, const QString &dateBegin, const QDate &refDate);//yearlyValue,residualValue
    double m_rate;
};

#endif

