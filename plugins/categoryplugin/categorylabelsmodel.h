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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef CATEGORYLABELSMODEL_H
#define CATEGORYLABELSMODEL_H

#include <categoryplugin/category_exporter.h>
#include <QAbstractTableModel>


namespace Category {
class CategoryItem;
namespace Internal {
class CategoryLabelsModelPrivate;
}  // End namespace Internal


class CATEGORY_EXPORT CategoryLabelsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum DataRepresentation {
        Lang = 0,
        Label,
        ColumnCount
    };

    explicit CategoryLabelsModel(QObject *parent = 0);
    ~CategoryLabelsModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex & = QModelIndex()) const {return ColumnCount;}

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    Qt::ItemFlags flags(const QModelIndex &index) const;

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());

    // Data
    bool setCategoryItem(CategoryItem *cat);
    CategoryItem *categoryItem() const;

public Q_SLOTS:
    bool submit();

Q_SIGNALS:
    void labelChanged(const Category::CategoryItem *item);

private:
    Internal::CategoryLabelsModelPrivate *d;
};

}  // End namespace Category


#endif // CATEGORYLABELSMODEL_H
