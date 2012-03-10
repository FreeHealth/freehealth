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
 ***************************************************************************/
#ifndef ICDSEARCHMODEL_H
#define ICDSEARCHMODEL_H

#include <icdplugin/icd_exporter.h>

#include <QAbstractTableModel>

namespace ICD {
namespace Internal {
class IcdSearchModelPrivate;
}

class ICD_EXPORT IcdSearchModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum DataRepresentation {
        SID_Code = 0,
        ICD_Code,
        ICD_CodeWithDagetAndStar,
        Label,
        Daget,
        Type,
        ColumnCount
    };

    enum SearchModes {
        SearchByLabel,
        SearchByCode
    };

    explicit IcdSearchModel(QObject *parent = 0);
    ~IcdSearchModel();

    void setSearchMethod(SearchModes mode);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    void fetchMore(const QModelIndex &parent);
    bool canFetchMore(const QModelIndex &parent) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const;

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

public Q_SLOTS:
    void setFilter(const QString &searchLabel);

private Q_SLOTS:
    void refreshDatabase();
    void languageChanged();

private:
    void init();

private:
    Internal::IcdSearchModelPrivate *d;

};

}  // End namespace ICD

#endif // ICDSEARCHMODEL_H
