/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#ifndef RECEIPTSENGINE_H
#define RECEIPTSENGINE_H

#include <accountplugin/account_exporter.h>
#include <accountbaseplugin/accountmodel.h>
#include <accountbaseplugin/medicalproceduremodel.h>
#include <accountbaseplugin/thesaurusmodel.h>
#include <accountbaseplugin/distancerulesmodel.h>
#include <accountbaseplugin/workingplacesmodel.h>
#include <accountbaseplugin/insurancemodel.h>

#include <QHash>
#include <QSqlDatabase>
#include <QSqlTableModel>

using namespace AccountDB;
class ACCOUNT_EXPORT receiptsEngine : public QObject
{
    Q_OBJECT
public:
    receiptsEngine();
    ~receiptsEngine();
    bool insertIntoAccount(const QHash<int,QVariant> &hashValues, const QString &userUuid);
    //QHash<QString,QVariant> getNamesAndValuesFromMP();
    bool insertInThesaurus(const QString &listOfValuesStr, const QString &userUuid);
    bool deleteFromThesaurus(const QString & data , const QString & userUid );
    bool addBoolTrue(QString & data);
    double getMinDistanceValue(const QString & data);
    QHash<int,QVariant> getListOfpreferredValues(QString & userUuid,
                                                QString & patientUid,
                                                QString & patientName,
                                                int choice);
    QHash<QString,double> getFilteredValueFromMedicalProcedure(const QString & string, const QString & field);
    QString getStringFromInsuranceUid(const QVariant & insuranceUid);
    QVariant getSiteUidFromSite(const QString & site);
    QVariant getInsuranceUidFromInsurance(const QString & insurance);
    QString getStringOfpreferredActAndHisValue(const QString & preferredAct);
    QHash<QString,QString> getPercentagesAccordingToUser();
    QString getJustDayBeforeLastRelease();
    //bool hideNamesOfPatients();

private:
    enum typeOfChoice {
            CASH = 0,
            CHECK,
            VISA,
            BANKING,
            OTHER,
            DUE
        };
   QString getDateWhereClause();
   bool datapackIsAvalaible();
   AccountModel *m_mpmodel;
   QSqlDatabase  m_db;
   QSqlDatabase  m_dbMP;
};


#endif
