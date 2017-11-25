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
#ifndef FORMALIZEDITEMMODEL_H
#define FORMALIZEDITEMMODEL_H

#include <QAbstractTableModel>
#include <QStringList>

namespace DrugsDB {

class AvailableFormalizedItemNameModel : public QAbstractListModel
{
    Q_OBJECT
public:
    AvailableFormalizedItemNameModel(QObject *parent = 0);
    ~AvailableFormalizedItemNameModel();

    int rowCount(const QModelIndex &) const {return m_ItemNames.count();}

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

private:
    QStringList m_ItemNames;
    QStringList m_ItemNamesTooltip;
};


//class FormalizedItemModel : public QAbstractTableModel
//{
//    Q_OBJECT
//public:
//    enum DataRepresentation {
//        ItemName = 0,
//        ItemValue
//    };

//    FormalizedItemModel(QObject *parent = 0);
//    ~FormalizedItemModel();

//    int rowCount(const QModelIndex &parent = QModelIndex()) const;
//    int columnCount(const QModelIndex &parent = QModelIndex()) const;

//    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
//    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

//    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
//    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

//    Qt::ItemFlags flags(const QModelIndex &index) const;


//};

}  // End namespace DrugsDB

#endif // FORMALIZEDITEMMODEL_H
