/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "rcitemmodel.h"
#include <edrcplugin/edrccore.h>
#include <edrcplugin/database/constants_db.h>
#include <edrcplugin/database/edrcbase.h>

using namespace eDRC;
using namespace Internal;

static inline eDRC::EdrcCore &edrcCore() {return eDRC::EdrcCore::instance();}
static inline eDRC::Internal::DrcDatabase &edrcBase() {return eDRC::EdrcCore::instance().edrcBase();}

RcItemModel::RcItemModel(QObject *parent):
    QSqlTableModel(parent, edrcBase().database())
{
    setTable(edrcBase().table(Constants::Table_Ref_RCItem));
    select();
}

RcItemModel::~RcItemModel()
{}

void RcItemModel::clear()
{
    setFilterOnCrId(-1);
}

QVariant RcItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::ToolTipRole) {
        int sqlCol = -1;
        switch (index.column()) {
        case Id: sqlCol = Constants::REF_RCITEM_SEQ; break;
        case Label: sqlCol = Constants::LIB_RCITEM_FR; break;
        } // switch
        if (sqlCol == -1)
            return QVariant();

        return QSqlTableModel::data(this->index(index.row(), sqlCol, index.parent()));
    }
    return QSqlTableModel::data(index, role);
}


Qt::ItemFlags RcItemModel::flags(const QModelIndex &index) const
{
    return QSqlTableModel::flags(index);
}

void RcItemModel::setFilterOnCrId(const int rcId)
{
    QHash<int, QString> whereLink;
    whereLink.insert(Constants::RC_LCRITERES_REF_RCITEM_ID, QString("='%1'").arg(rcId));

    QHash<int, QString> where;
    where.insert(Constants::REF_RCITEM_SEQ, QString("IN (%1)")
                 .arg(edrcBase().select(Constants::Table_RC_Link_RC_Criteres, Constants::RC_LCRITERES_REF_RC_ID, whereLink)));
    setFilter(edrcBase().getWhereClause(Constants::Table_Ref_RCItem, where));

    select();
}
