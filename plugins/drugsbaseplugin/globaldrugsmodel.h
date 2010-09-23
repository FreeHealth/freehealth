/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef GLOBALDRUGSMODEL_H
#define GLOBALDRUGSMODEL_H

#include <drugsbaseplugin/drugsbase_exporter.h>

#include <QSqlTableModel>
#include <QObject>
class QStandardItemModel;

/**
 * \file globaldrugsmodel.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.5.0
 * \date 24 Aug 2010
*/

namespace DrugsDB {
namespace Internal {
class GlobalDrugsModelPrivate;
class DrugsData;
}  // end namespace Internal


class DRUGSBASE_EXPORT GlobalDrugsModel : public QSqlTableModel
{
    Q_OBJECT
    friend class Internal::GlobalDrugsModelPrivate;

public:
    GlobalDrugsModel(QObject * parent = 0);
    ~GlobalDrugsModel();

    static void updateAvailableDosages();

    static bool hasAllergy(const DrugsDB::Internal::DrugsData *drug);
    static bool hasIntolerance(const DrugsDB::Internal::DrugsData *drug);

    static QStandardItemModel *drugsPrecautionsModel();

    bool setData(const QModelIndex &, const QVariant &, int = Qt::EditRole) { return false; }
    QVariant data(const QModelIndex & item, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

public Q_SLOTS:
    void updateCachedAvailableDosage();
    void onDrugsDatabaseChanged();

private Q_SLOTS:
    void refreshDrugsPrecautions(const QModelIndex &topleft, const QModelIndex &bottomright);
//    void physiologyProcessed();
//    void processPhysiology();

private:
    Internal::GlobalDrugsModelPrivate *d;
};


}  // end namespace DrugsDB


#endif // GLOBALDRUGSMODEL_H
