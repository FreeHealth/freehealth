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
 *  This program is distributed in the hope that it will be useful, *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef EDRC_INTERNAL_CRCRITERIASMODEL_H
#define EDRC_INTERNAL_CRCRITERIASMODEL_H

#include <QAbstractTableModel>

/**
 * \file crcriteriasmodel.h
 * \author Eric Maeker
 * \version 0.9.2
 * \date 05 Oct 2013
*/

namespace eDRC {
namespace Internal {
class CrCriteriasModelPrivate;

class CrCriteriasModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Datarepresentation {
        Id = 0,
        Label,
        ItemWeight, // Pondération
        Indentation,
        ColumnCount
    };

    enum CodingStatus {
        NoCodingStarted = 0,
        IncompleteCoding,
        ValidCoding
    };

    CrCriteriasModel(QObject *parent = 0);
    ~CrCriteriasModel();
    void clear();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    Qt::ItemFlags flags(const QModelIndex &index) const;

    void setFilterOnCrId(const int crId);
    void setSelectedCriteriaIds(const QList<int> &ids);
    int currentConsulResultId() const;
    CodingStatus currentCodingStatus() const;
    QList<int> currentSelectedCriteriaIds() const;

private:
    CrCriteriasModelPrivate *d;
};

} // namespace eDRC
} // namespace Internal

#endif  // EDRC_INTERNAL_CRCRITERIASMODEL_H
