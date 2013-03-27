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
#ifndef DATAPACK_PACKMODEL_H
#define DATAPACK_PACKMODEL_H

#include <datapackutils/pack.h>
#include <QAbstractTableModel>

/**
 * \file packmodel.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.2
 * \date 27 Jan 2012
*/

namespace DataPack {
class Pack;
namespace Internal {
class PackModelPrivate;
}

class PackModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum DataRepresentation {
        Label = 0,
        IsInstalled,
        IsAnUpdate,
        ColumnCount
    };

    explicit PackModel(QObject *parent = 0);
    ~PackModel();

    void setInstallChecker(const bool onOff);
    void setPackCheckable(const bool checkable);
    bool isDirty() const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex & = QModelIndex()) const {return ColumnCount;}

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    Qt::ItemFlags flags(const QModelIndex &index) const;

    const Pack &packageAt(const int index) const;

    QList<Pack> packageToInstall() const;
    QList<Pack> packageToUpdate() const;
    QList<Pack> packageToRemove() const;

public Q_SLOTS:
    void updateModel();
    void filter(const QString &vendor, const QList<Pack::DataType> &type);

private Q_SLOTS:
    void onServerAdded(const int index);
    void onServerRemoved(const int index);
//    void onServerDescriptionAvailable();
    void onPackInstalled(const DataPack::Pack &pack);
    void onPackRemoved(const DataPack::Pack &pack);

private:
    Internal::PackModelPrivate *d;
};

}  // End namespace DataPack

#endif // DATAPACK_PACKMODEL_H
