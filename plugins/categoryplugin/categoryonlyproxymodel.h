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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef CATEGORYONLYPROXYMODEL_H
#define CATEGORYONLYPROXYMODEL_H

#include <categoryplugin/category_exporter.h>
#include <QAbstractProxyModel>

namespace Category {
class ICategoryModelHelper;

namespace Internal {
class CategoryOnlyProxyModelPrivate;
}  // End namespace Internal


class CATEGORY_EXPORT CategoryOnlyProxyModel : public QAbstractProxyModel
{
    Q_OBJECT
public:
    explicit CategoryOnlyProxyModel(ICategoryModelHelper *parent);
    ~CategoryOnlyProxyModel();

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;

//    QItemSelection mapSelectionFromSource(const QItemSelection &sourceSelection) const;
//    QItemSelection mapSelectionToSource(const QItemSelection &proxySelection) const;

//    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());

//    Qt::ItemFlags flags(const QModelIndex &index) const;
//    void hidePmh(bool hide);

private:
    void updateBranch(QModelIndex &rootIndex);

private Q_SLOTS:
    void updateModel();
    void emitDataChanged(const QModelIndex &left, const QModelIndex &right);

private:
    Internal::CategoryOnlyProxyModelPrivate *d;
};

}  // End namespace PMH


#endif // PMHCATEGORYONLYPROXYMODEL_H
