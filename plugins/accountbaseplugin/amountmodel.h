/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developers:  Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#ifndef AMOUNTMODEL_H
#define AMOUNTMODEL_H

#include <accountbaseplugin/accountbase_exporter.h>
#include <QAbstractTableModel>

namespace AccountDB {
class AccountData;

class ACCOUNTBASE_EXPORT AmountModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum ColumnRepresentation {
        Col_Value = 0,
        Col_Currency,  // Devise monétaire
        ColCount
    };

    enum RowRepresentation {
        Row_Cash = 0,
        Row_Visa,
        Row_Cheque,
        Row_Banking,  // Virement banquaire
        Row_Insurance,
        Row_Other,
        Row_Due,
        RowCount
    };

    AmountModel(AccountData *data, QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const {Q_UNUSED(parent); return RowCount;}
    int columnCount(const QModelIndex &parent = QModelIndex()) const {Q_UNUSED(parent); return ColCount;}

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;

private:
    QList<float> m_Values;
    AccountData *m_AccountData;
};

}  //  End namespace AccountDB


#endif // AMOUNTMODEL_H
