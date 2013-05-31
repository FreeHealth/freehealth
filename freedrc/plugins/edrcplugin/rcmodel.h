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
#ifndef EDRC_INTERNAL_RC_MODEL_H
#define EDRC_INTERNAL_RC_MODEL_H

#include <QList>
#include <QSqlTableModel>

namespace eDRC {
namespace Internal {

class RCModel : public QSqlTableModel
{
    Q_OBJECT

public:
    enum DataRepresentation {
        Id = 0,
        Label,
        Validity,
        IsDiagPosA,
        IsDiagPosB,
        IsDiagPosC,
        IsDiagPosD,
        IsDiagPosZ,
        IsRiskLevel1,
        IsRiskLevel2,
        IsRiskLevel3,
        IsRiskLevel4,
        IsRiskLevel5,
        DateValidityStart,
        DateValidityEnd,
        Arguments,
        Nature
//        REG_RC_LIB,         // Libellé du RC de regroupement (quand item passe de valide à invalide)
//        REG_RC_ID,          // Id du RC de regroupement
//        SUB_RC_ID,          // RC de substitution (quand item passe de valide à invalide)
    };

    RCModel(QObject *parent = 0);
    ~RCModel();

//    int rowCount(const QModelIndex &parent = QModelIndex()) const;
//    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

//    bool insertRows(int position, int rows, const QModelIndex &parent = QModelIndex() );
//    bool removeRow ( int row, const QModelIndex & parent = QModelIndex() );

//    RC_Elements getRC(const QModelIndex& index);

// signals:
//     void dataChanged(const QModelIndex &index1 , const QModelIndex & index2);


private:
//    QList<RC_Elements>    *m_ListRC;
};

} // namespace Internal
} // namespace eDRC

#endif  // EDRC_INTERNAL_RC_MODEL_H
