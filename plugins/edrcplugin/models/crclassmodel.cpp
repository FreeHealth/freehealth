/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "crclassmodel.h"
#include <edrcplugin/edrccore.h>
#include <edrcplugin/database/constants_db.h>
#include <edrcplugin/database/edrcbase.h>

using namespace eDRC;
using namespace Internal;

static inline eDRC::EdrcCore &edrcCore() {return eDRC::EdrcCore::instance();}
static inline eDRC::Internal::DrcDatabase &edrcBase() {return eDRC::EdrcCore::instance().edrcBase();}

CrClassModel::CrClassModel(QObject *parent):
    QSqlTableModel(parent, edrcBase().database())
{
    setTable(edrcBase().table(Constants::Table_Ref_ClassRC));
    select();
}

CrClassModel::~CrClassModel()
{}

QVariant CrClassModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int sqlIndex = -1;
    switch (index.column()) {
    case Id: sqlIndex = Constants::REF_CLASSRC_SEQ; break;
    case Label: sqlIndex = Constants::REF_CLASSRC_LIB; break;
    default: break;
    }
    if (sqlIndex==-1)
        return QVariant();
    QModelIndex sql = this->index(index.row(), sqlIndex);
    return QSqlTableModel::data(sql, role);
}

QVariant CrClassModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        return QString("Column %1").arg(section);
    else
        return QString("Row %1").arg(section);
}

Qt::ItemFlags CrClassModel::flags(const QModelIndex &index) const
{
    return QSqlTableModel::flags(index);
}

void CrClassModel::setFilterOnCrId(const QString &rcId)
{
    Q_UNUSED(rcId);
}
