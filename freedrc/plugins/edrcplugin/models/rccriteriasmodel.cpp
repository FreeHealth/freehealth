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
#include "rccriteriasmodel.h"
#include <edrcplugin/constants.h>
#include <edrcplugin/edrccore.h>
#include <edrcplugin/database/edrcbase.h>

// Les libellés peuvent être simplifiés en remplaçant le début
//    retour.replace("++1|", QChar(10112));
//    retour.replace("++2|", QChar(10113));
//    retour.replace("++3|", QChar(10114));
//    retour.replace("++4|", QChar(10115));
//    retour.replace("++++", QChar(9745));
//    retour.replace("+ -" , QChar(8226));
//    retour.replace("++1|", "&#"+QString::number(HTML_Char_1)+";" );
//    retour.replace("++2|", "&#"+QString::number(HTML_Char_2)+";" );
//    retour.replace("++3|", "&#"+QString::number(HTML_Char_3)+";" );
//    retour.replace("++4|", "&#"+QString::number(HTML_Char_4)+";" );
//    retour.replace("++++", "&#"+QString::number(HTML_MustBeSelected)+";" );
//    retour.replace("+ -" , "&#"+QString::number(HTML_Optionnal)+";" );

using namespace eDRC;
using namespace Internal;

static inline eDRC::EdrcCore &edrcCore() {return eDRC::EdrcCore::instance();}
static inline eDRC::Internal::DrcDatabase &edrcBase() {return eDRC::EdrcCore::instance().edrcBase();}

RcCriteriasModel::RcCriteriasModel(QObject *parent):
    QSqlQueryModel(parent)
{
    setFilterOnRcId(-1);
}

RcCriteriasModel::~RcCriteriasModel()
{}

QVariant RcCriteriasModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        int sqlCol = -1;
        switch (index.column()) {
        case Id: sqlCol = Constants::RC_LCRITERES_REF_LRCCRITERES_SEQ; break;
        case Label: sqlCol = Constants::RC_LCRITERES_LIB_CRITERES_FR; break;
        case ItemWeight: sqlCol = Constants::RC_LCRITERES_REF_PONDER_ID; break;
        case Indentation: sqlCol = Constants::RC_LCRITERES_REF_RETRAIT_ID; break;
        } // switch

        if (sqlCol == -1)
            return QVariant();
//        if (index.column() == Label)
//            return QString("%1; (P:%2; I:%3)")
//                    .arg(QSqlQueryModel::data(this->index(index.row(), Constants::RC_LCRITERES_LIB_CRITERES_FR, index.parent())).toString())
//                    .arg(QSqlQueryModel::data(this->index(index.row(), Constants::RC_LCRITERES_REF_PONDER_ID, index.parent())).toString())
//                    .arg(QSqlQueryModel::data(this->index(index.row(), Constants::RC_LCRITERES_REF_RETRAIT_ID, index.parent())).toString())
//                    ;
        return QSqlQueryModel::data(this->index(index.row(), sqlCol, index.parent()));
    } else if (role == Qt::ToolTipRole && index.column() == Label) {
        return QString("%1\n  Pond: %2\n  Indent: %3)")
                .arg(QSqlQueryModel::data(this->index(index.row(), Constants::RC_LCRITERES_LIB_CRITERES_FR, index.parent())).toString())
                .arg(QSqlQueryModel::data(this->index(index.row(), Constants::RC_LCRITERES_REF_PONDER_ID, index.parent())).toString())
                .arg(QSqlQueryModel::data(this->index(index.row(), Constants::RC_LCRITERES_REF_RETRAIT_ID, index.parent())).toString())
                ;
    }
    return QSqlQueryModel::data(index, role);
}


Qt::ItemFlags RcCriteriasModel::flags(const QModelIndex &index) const
{
    return QSqlQueryModel::flags(index);
}

/** Filter CR criterias according to the CR primkey \e crId*/
void RcCriteriasModel::setFilterOnRcId(const int crId)
{
    QHash<int, QString> where;
    where.insert(Constants::RC_LCRITERES_REF_RC_ID, QString("='%1'").arg(crId));
    QString req = edrcBase().select(Constants::Table_RC_Link_RC_Criteres, where);
    req += QString(" ORDER BY %1 ASC;").arg(edrcBase().fieldName(Constants::Table_RC_Link_RC_Criteres, Constants::RC_LCRITERES_AFFICH_ORDRE));
    setQuery(req, edrcBase().database());
}
