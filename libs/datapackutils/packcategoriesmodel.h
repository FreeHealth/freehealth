/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developers:                                                     *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DATAPACK_PACKCATEGORIESMODEL_H
#define DATAPACK_PACKCATEGORIESMODEL_H

#include <datapackutils/pack.h>
#include <QStandardItemModel>

/**
 * \file packcategoriesmodel.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.7.2
 * \date 27 Apr 2012
*/

namespace DataPack {
namespace Internal {
class PackCategoriesModelPrivate;
}

class PackCategoriesModel : public QStandardItemModel
{
    Q_OBJECT
public:
    enum DataRepresentation {
        Label = 0,
        ColumnCount
    };

    explicit PackCategoriesModel(QObject *parent = 0);
    ~PackCategoriesModel();

    int columnCount(const QModelIndex & = QModelIndex()) const {return ColumnCount;}
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QList<Pack::DataType> datatype(const QModelIndex &index) const;
    QString vendor(const QModelIndex &index) const;

public Q_SLOTS:
    void updateModel();

private Q_SLOTS:
    void onServerAdded(const int index);
    void onServerRemoved(const int index);

private:
    Internal::PackCategoriesModelPrivate *d;
};

}  // End namespace DataPack

#endif // DATAPACK_PACKCATEGORIESMODEL_H
