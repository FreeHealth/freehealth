/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef EDRC_INTERNAL_RCE_MODEL_H
#define EDRC_INTERNAL_RCE_MODEL_H

#include <QList>
#include <QSqlTableModel>

/**
 * \file preventablecriticalriskmodel.h
 * \author Eric Maeker
 * \version 0.9.0
 * \date 18 June 2013
*/

namespace eDRC {
namespace Internal {

class PreventableCriticalRiskModel : public QSqlTableModel
{
    Q_OBJECT

public:
    enum DataRepresentation {
        Id = 0,
        Label
    };

    PreventableCriticalRiskModel(QObject *parent = 0);
    ~PreventableCriticalRiskModel();

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;

    void setFilterOnCrId(const int rcId);

};

} // namespace Internal
} // namespace eDRC

#endif  // EDRC_INTERNAL_RCE_MODEL_H
