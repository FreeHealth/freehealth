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
#include "rcmodel.h"
#include <edrcplugin/edrccore.h>
#include <edrcplugin/database/constants_db.h>
#include <edrcplugin/database/edrcbase.h>

using namespace eDRC;
using namespace Internal;

static inline eDRC::EdrcCore &edrcCore() {return eDRC::EdrcCore::instance();}
static inline eDRC::Internal::DrcDatabase &edrcBase() {return eDRC::EdrcCore::instance().edrcBase();}

RcModel::RcModel(QObject *parent) :
    QSqlTableModel(parent, edrcBase().database())
{
    setTable(edrcBase().table(Constants::Table_REF_RC));
    QHash<int, QString> where;
    where.insert(Constants::VALIDITE, QString("='1'"));
    setFilter(edrcBase().getWhereClause(Constants::Table_REF_RC, where));
    select();
}

RcModel::~RcModel()
{}

QVariant RcModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        int sqlCol = -1;
        switch (index.column()) {
        case Id: sqlCol = Constants::REF_RC_SEQ; break;
        case Label: sqlCol = Constants::LIB_RC_FR; break;
        case Validity: sqlCol = Constants::VALIDITE; break;
        case IsDiagPosA: sqlCol = Constants::PA; break;
        case IsDiagPosB: sqlCol = Constants::PB; break;
        case IsDiagPosC: sqlCol = Constants::PC; break;
        case IsDiagPosD: sqlCol = Constants::PD; break;
        case IsDiagPosZ: sqlCol = Constants::PZ; break;
        case IsRiskLevel1: sqlCol = Constants::P1; break;
        case IsRiskLevel2: sqlCol = Constants::P2; break;
        case IsRiskLevel3: sqlCol = Constants::P3; break;
        case IsRiskLevel4: sqlCol = Constants::P4; break;
        case IsRiskLevel5: sqlCol = Constants::P5; break;
        case DateValidityStart: sqlCol = Constants::VALID_DEBUT; break;
        case DateValidityEnd: sqlCol = Constants::VALID_FIN; break;
        case Arguments: sqlCol = Constants::ARGUMENTAIRE; break;
        case Nature: sqlCol = Constants::NATURE; break;
        } // switch
        if (sqlCol == -1)
            return QVariant();

        return QSqlTableModel::data(this->index(index.row(), sqlCol, index.parent()));
    }


    //    if (role == Qt::DecorationRole)
//    {   // Affiche l'icone d'état de complétion du RC
//        if (idx.column() == RC_Elements::Col_RC)
//        {  RC_Elements el = m_ListRC->at(idx.row());
//           QString tmp;
//           if (!el.isCompleted()) return QPixmap(":/icone_incomplet.png");
//           else return QPixmap(":/icone_ok.png");
//        }
//    }
///*    else
//    if ((index.column() == RC_Elements::Col_RC) && (role == Qt::FontRole))
//    {   RC_Elements el = m_ListRC->at(index.row());
//        QString msg;
//        if (el.isCompleted())
//        {   QFont myFont;
//            myFont.setBold(true);
//            QVariant myData = QVariant(myFont);
//            return myData;
//        }
//        else
//        {   QFont myFont;
//            myFont.setItalic(true);
//            QVariant myData = QVariant(myFont);
//            return myData;
//        }

//    }
//*/    else
//    if (role == Qt::UserRole)   // Retourne les listes d'autorisation pour les pos diag et les suivis
//    {   RC_Elements el = m_ListRC->at(idx.row());
//        QVariant qvar = QVariant(el.getAuthorizedPosDiagAndSuivi());
//        return qvar;
//    }
    return QVariant();
}

Qt::ItemFlags RcModel::flags(const QModelIndex &index) const
{
//    if (!index.isValid())
//        return Qt::ItemIsEnabled;
//    RC_Elements el = m_ListRC->at( index.row() );
//    if (el.getLibelle().length() )
//    {   return QAbstractItemModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsUserCheckable; }
//    else
//    {   return Qt::ItemIsEditable; }
    return QSqlTableModel::flags(index);
}

bool RcModel::setData(const QModelIndex &index,
                             const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

//    if (role == Qt::EditRole)
//    {   // si suivi == N et que PosDiag != Z ==> Sympto = true
//        RC_Elements el = m_ListRC->at( index.row() );
//        if ((index.column() == RC_Elements::Col_Pos_Suivi) && (value.toString() == "N"))
//        {  QModelIndex diagIndex = this->index( index.row(), RC_Elements::Col_Pos_Diag );
//           QString diag =  el.getSelectedPosDiag();
//	if ((diag != "Z") && ((diag == "A") || (diag == "B") || (diag == "C") || (diag == "D")) )
//           {   QModelIndex symptoIndex = this->index( index.row(), RC_Elements::Col_Sympto);
//               setData( symptoIndex, "O", Qt::EditRole);
//               el = m_ListRC->at( index.row() );
//           }
//        }
//        // Ajouter la value aux données sauf pour les critères
//        if (index.column() != RC_Elements::Col_Criteres) el.setQVariant( value, index.column() );

//        // Vérifier état du RC (complet ou non ?)
//        const QModelIndex i = this->index( index.row(), RC_Elements::Col_RC);
//        emit dataChanged(i,i);

//        m_ListRC->replace( index.row(), el );
//        emit dataChanged(index, index);
//        return true;
//    }
//    else
//    if (role == Qt::UserRole)
//    { RC_Elements el = m_ListRC->at( index.row() );
//      el.setAuthorizedPosDiagAndSuivi( value );
//      m_ListRC->replace( index.row(), el );
//      emit dataChanged(index, index);
//      return true;
//    }

    return false;
}
