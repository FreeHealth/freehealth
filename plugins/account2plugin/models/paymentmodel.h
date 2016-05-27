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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developers:                                                       *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef ACCOUNT2_PAYMENTMODEL_H
#define ACCOUNT2_PAYMENTMODEL_H

#include <account2plugin/basicfilter.h>

#include <QStandardItemModel>

/**
 * \file paymentmodel.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 17 Mar 2013
*/

namespace Account2 {
namespace Internal {
class PaymentModelPrivate;

class PaymentModel : public QStandardItemModel
{
  Q_OBJECT
public:
    enum DataRepresentation {
        Id = 0,
        Label,
        Amount,
        Type,
        Date_Creation,
        Date_Execution,
        Comment,
        ColumnCount
    };

    PaymentModel(QObject *parent);
    ~PaymentModel();

    int rowCount(const QModelIndex &index = QModelIndex()) const;
    int columnCount(const QModelIndex & = QModelIndex()) const {return ColumnCount;}

//    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const = 0;
//    QModelIndex parent(const QModelIndex &child) const = 0;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
//    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    bool setFilter(const BasicFilter &filter);

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    bool insertRow(int arow, const QModelIndex &aparent = QModelIndex())        { return insertRows(arow, 1, aparent); }
    bool insertColumn(int acolumn, const QModelIndex &aparent = QModelIndex())  { return insertColumns(acolumn, 1, aparent); }
    bool removeRow(int arow, const QModelIndex &aparent = QModelIndex())        { return removeRows(arow, 1, aparent); }
    bool removeColumn(int acolumn, const QModelIndex &aparent = QModelIndex())  { return removeColumns(acolumn, 1, aparent); }

    bool isDirty() const;

public Q_SLOTS:
    bool submit();
    void revert();

private:
    Internal::PaymentModelPrivate *d;
};

} // namespace Internal
} // namespace Account2


#endif // ACCOUNT2_PAYMENTMODEL_H
