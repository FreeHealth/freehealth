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
#ifndef MOVEMENTSIO_H
#define MOVEMENTSIO_H

#include <accountplugin/account_exporter.h>

#include <QStandardItemModel>
#include <QHash>

namespace AccountDB {
    class MovementModel;
}

class ACCOUNT_EXPORT MovementsIODb : public QObject
{
    Q_OBJECT
enum Icons
{
    ICON_NOT_PREF = 0,
    ICON_PREF
    };
public:
    MovementsIODb(QObject *parent);
    ~MovementsIODb();

    AccountDB::MovementModel *getModelMovements(QString &year);
    QStandardItemModel *getMovementsComboBoxModel(QObject *parent);

    QStringList getYearComboBoxModel();
    QStandardItemModel *getBankComboBoxModel(QObject * parent);

    bool insertIntoMovements(QHash<int,QVariant> &hashValues);
    bool deleteMovement(int row, const QString & year);
    bool validMovement(int row);
    int getAvailableMovementId(QString & movementsComboBoxText);
    int getTypeOfMovement(QString & movementsComboBoxText);
    int getBankId(QString & bankComboBoxText);
    QString getBankNameFromId(int id);
    QString getUserUid();
    QHash<QString,QString> hashChildrenAndParentsAvailableMovements();
    bool containsFixAsset(int & row);

private:
    QStringList listOfParents();
    bool debitOrCreditInBankBalance(const QString & bank, double & value);
    AccountDB::MovementModel *m_modelMovements;
    QString m_user_uid;
};

#endif

