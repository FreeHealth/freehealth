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
 *   Main Developper : Eric MAEKER, MD <eric.maeker@gmail.com>             *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef ROUTESMODEL_H
#define ROUTESMODEL_H

#include <QAbstractTableModel>
#include <QMultiHash>
#include <QString>
#include <QList>

namespace DrugsDB {
namespace Internal {

struct Route {
    Route() : id(-1), isHelper(false), checkState(Qt::Unchecked){}

    bool operator<(const Route &second) const;

    int id;
    QMultiHash<QString, QString> trLabels;
    bool isHelper;
    Qt::CheckState checkState;
};

}  // namespace Internal

class RoutesModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(QList<int> checkedRouteIds READ checkedRouteIds WRITE setCheckedRouteIds)

public:
    enum DataRepresentation {
        Id = 0,
        FirstTranslatedName,
        AllTranslatedNames,
        ColumnCount
    };

    RoutesModel(QObject *parent = 0);
    ~RoutesModel();

    static QString routeCsvAbsoluteFile();

    void initialize();
    void clear();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &) const {return ColumnCount;}

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    Qt::ItemFlags flags(const QModelIndex &index) const;

    QList<int> checkedRouteIds() const;
    QList<QVariant> checkedRouteIdsInVariant() const;
    void setCheckedRouteIds(const QList<int> &ids);
    void setCheckedRouteIds(const QList<QVariant> &ids);

private:
    QList<Internal::Route> m_Routes;
    QList<int> m_CheckedIds;
};

}  // End namespace DrugsDB

QDebug operator<<(QDebug debug, const DrugsDB::Internal::Route &route);

#endif // ROUTESMODEL_H
