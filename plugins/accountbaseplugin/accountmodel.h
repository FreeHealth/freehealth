/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  The FreeAccount plugins are free, open source FreeMedForms' plugins.   *
 *  (C) 2010-2011 by Pierre-Marie Desombre, MD <pm.desombre@medsyn.fr>     *
 *  and Eric Maeker, MD <eric.maeker@gmail.com>                            *
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
 *  Main Developers:  Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#ifndef ACCOUNTMODEL_H
#define ACCOUNTMODEL_H

#include <accountbaseplugin/accountbase_exporter.h>

#include <QAbstractTableModel>
#include <QSqlRecord>
#include <QSqlError>
#include <QColor>

namespace AccountDB {
namespace Internal {
class AccountModelPrivate;
}

class ACCOUNTBASE_EXPORT AccountModel : public QAbstractTableModel
{
    Q_OBJECT
    friend class Internal::AccountModelPrivate;

public:
    // Data representation is the same as the SQL table ACCOUNT

    AccountModel(QObject *parent);
    ~AccountModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant & value, int role = Qt::EditRole);

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    bool insertRow(int arow, const QModelIndex &aparent = QModelIndex())        { return insertRows(arow, 1, aparent); }
    bool insertColumn(int acolumn, const QModelIndex &aparent = QModelIndex())  { return insertColumns(acolumn, 1, aparent); }
    bool removeRow(int arow, const QModelIndex &aparent = QModelIndex())        { return removeRows(arow, 1, aparent); }
    bool removeColumn(int acolumn, const QModelIndex &aparent = QModelIndex())  { return removeColumns(acolumn, 1, aparent); }

    bool isDirty() const;

    QSqlError lastError();
    bool canFetchMore(const QModelIndex &parent = QModelIndex()) const;
    void fetchMore(const QModelIndex &parent = QModelIndex());
    void setFilter(const QString & filter);
    QString filter();
    void select();
    QSqlRecord record(int row);

    // Filter fucntions
    void setStartDate(const QDate &date);
    void setEndDate(const QDate &date);
    QString getUserUuid();

    // Sums
    double sum(const int &fieldRef);

    //list of doubles rows
    QList<int> getDoublesRows;

public Q_SLOTS:
    bool submit();
    void revert();

private Q_SLOTS:
    void userChanged();

private:
    Internal::AccountModelPrivate *d;
};

}  // End namespace AccountDB


#endif // ACCOUNTMODEL_H
