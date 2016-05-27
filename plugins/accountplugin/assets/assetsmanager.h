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

