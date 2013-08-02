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
#ifndef LEDGERIO_H
#define LEDGERIO_H
#include "../../accountbaseplugin/constants.h"//<accountbaseplugin/constants.h>
#include "../../accountbaseplugin/accountmodel.h"//<accountbaseplugin/availablemovementmodel>
#include "../../accountbaseplugin/movementmodel.h"//<accountbaseplugin/movementmodel>
#include <QObject>
#include <QAbstractTableModel>
#include <QStringList>
#include <QStandardItemModel>
#include <QVector>

using namespace AccountDB;
using namespace Constants;
class LedgerIO : public QObject {
    Q_OBJECT
    public :
        LedgerIO(QObject * parent = 0);
        ~LedgerIO();
        QStringList getListOfYears();
        AccountModel * getModelMonthlyReceiptsIO(QObject * parent,QString & month , QString & year);
        QStringList getTypesByMonth(QObject * parent,QString & month,QString & year);
        int getNbrOfRowsByTypeAndMonth(QObject * parent,QString & month,QString & year,QString & type);
        QStringList listOfTypesByYear(QString & year);
        int getNbrOfRowsByTypeAndYear(QObject * parent,QString & year,QString & type);
        double getYearlyReceiptsSum(QObject * parent,QString & year);

        MovementModel * getModelMonthlyMovementsIO(QObject * parent,const QString & month,const QString & year);
        QStandardItemModel * getModelMonthlyAndTypeMovementsIO(QObject * parent,const QString & month,const QString & year);
        QStandardItemModel * getModelYearlyAndTypeMovementsIO(QObject * parent,const QString & year);

        double getMovementSum(QObject * parent,QString & month, QString & year);
        //ledger
        QStringList getListOfSumsMonthlyReceiptsIO(QObject * parent,QString & dateBegin , QString & dateEnd);
        QStringList getListOfSumsMonthlyMovementsIO(QObject * parent,const QString & dateBegin ,const QString & dateEnd);
        QStringList listOfReceiptsTypes();
        QStringList listOfMovementsTypes();
        QList<QVector<QString> > getDataReceiptsInVector(const QString & dateBegin,const QString & dateEnd, bool noName);
        QList<QVector<QString> > getDataMovementsInVector(const QString & dateBegin,const QString & dateEnd);
    private :
        QString getNoName(const QString name);
        AccountModel * m_accountModel;
        MovementModel * m_movementModel;
        QString m_userUuid;
};

#endif

