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
#ifndef PMHCATEGORYMODEL_H
#define PMHCATEGORYMODEL_H

#include <QAbstractItemModel>
#include <QAbstractProxyModel>

namespace PMH {
namespace Internal {
class PmhCategoryModelPrivate;
class PmhData;
class PmhCategory;
}  // End namespace Internal

class PmhCategoryModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum DataRepresentation {
        Label = 0,
        Type,
        Id,
        EmptyColumn,
        ColumnCount
    };

    // Model
    explicit PmhCategoryModel(QObject *parent = 0);
    ~PmhCategoryModel();

    QAbstractProxyModel *categoryOnlyModel();
    bool isCategory(const QModelIndex &index) const;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    void fetchMore(const QModelIndex &parent);
    bool canFetchMore(const QModelIndex &parent) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    Qt::ItemFlags flags(const QModelIndex &index) const;

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

//    QVariant headerData(int section, Qt::Orientation orientation, int role) const {return section;}

    // Data
    bool addPmhData(Internal::PmhData *pmh);
    bool addPmhCategoryData(Internal::PmhCategory *cat);
    Internal::PmhData *pmhDataforIndex(const QModelIndex &item) const;
    Internal::PmhCategory *pmhCategoryforIndex(const QModelIndex &item) const;

    // Helpers
    QModelIndex indexForCategoryId(const int id) const;
    void updateFontAndColors(const QModelIndex &parent = QModelIndex());

private:
    Internal::PmhCategoryModelPrivate *d;
};


}  // End namespace PMH


#endif // PMHCATEGORYMODEL_H
