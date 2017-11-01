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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef EDRC_INTERNAL_CRLISTMODEL_H
#define EDRC_INTERNAL_CRLISTMODEL_H

#include <QList>
#include <QAbstractTableModel>

/**
 * \file crlistmodel.h
 * \author Eric Maeker
 * \version 0.9.2
 * \date 23 Sept 2013
*/

namespace eDRC {
namespace Internal {
class ConsultResult;

class CrListModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum DataRepresentation {
        Id = 0,
        Label,
        DateOfExamination,
        Validity,
        DiagnosisPosition,
        //RiskLevel,
        MedicalFollowUp,
        Html,
        ColumnCount
    };

    CrListModel(QObject *parent = 0);
    ~CrListModel();

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;

    void setCrList(const QList<ConsultResult> &cr);

private:
    QList<ConsultResult> _list;
};

} // namespace Internal
} // namespace eDRC

#endif  // EDRC_INTERNAL_CRLISTMODEL_H
