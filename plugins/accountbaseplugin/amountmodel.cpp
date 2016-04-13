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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developers:  Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#include "amountmodel.h"
#include "accountdata.h"

#include <QAbstractTableModel>
#include <QList>

using namespace AccountDB;


AmountModel::AmountModel(AccountData *data, QObject *parent) :
        QAbstractTableModel(parent), m_AccountData(data)
{
}

QVariant AmountModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role==Qt::EditRole || role==Qt::DisplayRole) {
        switch (index.column()) {
        case Col_Value:
            switch (index.row()) {
            case Row_Cash: return m_AccountData->value(AccountData::Cash);
            case Row_Visa: return m_AccountData->value(AccountData::Visa);
            case Row_Cheque: return m_AccountData->value(AccountData::Cheque);
//            case Row_Banking: return m_AccountData->value(AccountData::);
            case Row_Insurance: return m_AccountData->value(AccountData::Insurance);
            case Row_Other: return m_AccountData->value(AccountData::Other);
            case Row_Due: return m_AccountData->value(AccountData::DueAmount);
            }
        }
    }
    return QVariant();
}

bool AmountModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    if (role==Qt::EditRole) {
        switch (index.column()) {
        case Col_Value:
            {
                switch (index.row()) {
                case Row_Cash:  m_AccountData->setValue(AccountData::Cash, value); break;
                case Row_Visa:  m_AccountData->setValue(AccountData::Visa, value); break;
                case Row_Cheque:  m_AccountData->setValue(AccountData::Cheque, value); break;
//            case Row_Banking:  m_AccountData->setValue(AccountData::); break;
                case Row_Insurance:  m_AccountData->setValue(AccountData::Insurance, value); break;
                case Row_Other:  m_AccountData->setValue(AccountData::Other, value); break;
                case Row_Due:  m_AccountData->setValue(AccountData::DueAmount, value); break;
                }
                Q_EMIT dataChanged(index, index);
                return true;
            }
        }
    }
    return false;
}

QVariant AmountModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role==Qt::DisplayRole) {
        if (orientation==Qt::Vertical) {
            switch (section) {
            case Row_Cash: return tr("Cash");
            case Row_Visa: return tr("Visa");
            case Row_Cheque: return tr("Cheque");
            case Row_Banking: return tr("Banking");
            case Row_Other: return tr("Other");
            case Row_Due: return tr("Due");
            }
        } else if (orientation==Qt::Horizontal) {
            switch (section) {
            case Col_Value: return tr("Value");
            case Col_Currency : return tr("Currency");
            }
        }
    }
    return QVariant();
}

Qt::ItemFlags AmountModel::flags(const QModelIndex &index) const
{
    if (index.column()==Col_Value) {
        return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
    } else {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
}
