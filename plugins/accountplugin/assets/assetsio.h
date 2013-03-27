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
 *  Main Developers:  Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#ifndef ASSETSIO_H
#define ASSETSIO_H

#include <accountplugin/account_exporter.h>

#include <QObject>
#include <QVariant>
#include <QHash>
#include <QStandardItemModel>

// #include "../../accountbaseplugin/constants.h"//<accountbaseplugin/constants.h>  A eviter+++

// #include "../../accountbaseplugin/assetmodel.h"
// NON à la place déclare la class comme ça et met le include dans le cpp
// Intérêt == si modification du assetModel header pas de recompilation de cet objet avec ma méthode
// alors qu'il aurait été recompilé avec ton include.

namespace AccountDB {
class AssetModel;
}

class ACCOUNT_EXPORT AssetsIO : public QObject
{
    Q_OBJECT

public:
    AssetsIO(QObject *parent);
    ~AssetsIO();
    AccountDB::AssetModel *getModelAssets();
    QString getUserUid();
    bool insertIntoAssets(const QHash<int,QVariant> &hashValues);
    bool insertIntoMovements(const QHash<int,QVariant> &hashValues);
    bool deleteAsset(int row);

    QStandardItemModel *getListsOfValuesForRefresh(QObject *parent);
    QStandardItemModel *getBankComboBoxModel(QObject *parent);

    int getLastMovementId();
    bool deleteMovement(int idMovement, int idBank);
    int getMovementId(int row);
    int getIdFromBankName(const QString &bankName);
    double getResidualValueWhenRefresh(int row);
    bool deleteOneYearToRun(int row);
    double getRate(const QDate &date, double duration);
    QStandardItemModel * getYearlyValues(const QDate &year, QObject *parent);
    double getValueFromRow(int row);
    int getModeFromRow(int row);
    double getDurationFromRow(int row);
    QDate getDateFromRow(int row);
    QString getLabelFromRow(int row);
private:
    QString getBankNameFromId(int id);
    bool debitOrCreditInBankBalance(const QString &bank, double value);
    bool creditValueDeletedToBankAccount(double value, int idBank);
    AccountDB::AssetModel *m_assetModel;
    QString m_user_uid;
};


#endif
