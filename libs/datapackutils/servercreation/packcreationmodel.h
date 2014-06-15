/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main developers : Eric Maeker                                          *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DATAPACK_PACKCREATIONMODEL_H
#define DATAPACK_PACKCREATIONMODEL_H

#include <datapackutils/datapack_exporter.h>
#include <QStandardItemModel>

/**
 * \file packcreationmodel.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 15 Jun 2014
*/

namespace DataPack {
class PackCreationQueue;

namespace Internal {
class PackCreationModelPrivate;
} // namespace Internal

class DATAPACK_EXPORT PackCreationModel : public QStandardItemModel
{
public:
    enum Format {
        ShowByQueue = 0,
        ShowByServer
    };

    explicit PackCreationModel(QObject *parent = 0);
    ~PackCreationModel();

    void setFormat(Format format);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

//    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

public Q_SLOTS:
    bool addPackCreationQueue(const PackCreationQueue &queue);
    bool addScreeningPath(const QString &screeningAbsPath);

private:
    bool setCheckStateRoleToItemAndChildren(const QModelIndex &parent, const QVariant &value);
    bool setCheckedToAllParents(const QModelIndex &index);

private:
    Internal::PackCreationModelPrivate *d;
};

} // namespace DataPack

#endif // DATAPACK_PACKCREATIONMODEL_H

