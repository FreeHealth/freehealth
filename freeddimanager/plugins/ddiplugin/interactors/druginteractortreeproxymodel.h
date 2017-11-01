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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, MD <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DDIMANAGER_DDIPLUGIN_INTERACTORTREEPROXYMODEL_H
#define DDIMANAGER_DDIPLUGIN_INTERACTORTREEPROXYMODEL_H

#include <ddiplugin/ddi_exporter.h>
#include <QStandardItemModel>

/**
 * \file druginteractortreeproxymodel.h
 * \author Eric MAEKER
 * \version 0.10.0
 * \date 12 Oct 2013
*/

namespace DDI {
class DrugInteractorTableModel;
namespace Internal {
class DrugInteractorTreeProxyModelPrivate;
}

class DDI_EXPORT DrugInteractorTreeProxyModel : public QStandardItemModel
{
    Q_OBJECT

public:
    enum DataRepresentation {
        Code = 0,
        LabelEn,
        LabelFr,
        LabelDe,
        NumberOfColumn
    };
    DrugInteractorTreeProxyModel(QObject * parent = 0);
    bool initialize(DrugInteractorTableModel *sourceModel);

    ~DrugInteractorTreeProxyModel();

//    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

    QModelIndex toSourceIndex(const QModelIndex &index) const;

private:
    Internal::DrugInteractorTreeProxyModelPrivate *d;
};

}  //  End namespace DDI

#endif // DDIMANAGER_DDIPLUGIN_INTERACTORTREEPROXYMODEL_H
