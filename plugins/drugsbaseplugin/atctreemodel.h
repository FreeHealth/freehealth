/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef ATCTREEMODEL_H
#define ATCTREEMODEL_H

#include <drugsbaseplugin/drugsbase_exporter.h>

#include <QAbstractItemModel>

/**
 * \file atctreemodel.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.4.2
 * \date 14 July 2010
*/


namespace DrugsDB {
namespace Internal {
class AtcTreeModelPrivate;
}  // end namespace Internal


class DRUGSBASE_EXPORT AtcTreeModel : public QAbstractItemModel
{
    Q_OBJECT
    friend class Internal::AtcTreeModelPrivate;

public:
    enum DataRepresentation {
        ATC_Label = 0,
        ATC_Code
    };

    AtcTreeModel(QObject * parent = 0);
    ~AtcTreeModel();

public Q_SLOTS:
    void init();

public:
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    bool setData(const QModelIndex &, const QVariant &, int) {return false;}
    QVariant data(const QModelIndex & item, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

private Q_SLOTS:
    void onDrugsBaseChanged();

private:
    Internal::AtcTreeModelPrivate *d;
};

}  // end namespace DrugsDB

#endif // ATCTREEMODEL_H
