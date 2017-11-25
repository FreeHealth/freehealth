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
/**
 * \class eDRC::CrListModel
 * Provides a basic CR list read-only table model.
 */

#include "crlistmodel.h"
#include <edrcplugin/edrccore.h>
#include <edrcplugin/consultresult.h>
#include <edrcplugin/database/constants_db.h>
#include <edrcplugin/database/edrcbase.h>

#include <utils/global.h>

using namespace eDRC;
using namespace Internal;

static inline eDRC::EdrcCore &edrcCore() {return eDRC::EdrcCore::instance();}
static inline eDRC::Internal::DrcDatabase &edrcBase() {return eDRC::EdrcCore::instance().edrcBase();}

CrListModel::CrListModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

CrListModel::~CrListModel()
{}

int CrListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return _list.count();
}

int CrListModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return ColumnCount;
}

QVariant CrListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (!IN_RANGE_STRICT_MAX(index.row(), 0, _list.count()))
        return QVariant();

    if (role == Qt::DisplayRole) {
        const ConsultResult &cr = _list.at(index.row());
        switch (index.column()) {
        case Id: return cr.consultResultId();
        case Label: return edrcCore().edrcBase().getCrLabel(cr.consultResultId());
        case DateOfExamination: return cr.dateOfExamination();
        case Validity: return cr.isValid();
        case DiagnosisPosition: return cr.diagnosisPosition();
        //case RiskLevel: return cr.isValid();
        case MedicalFollowUp: return cr.medicalFollowUp();
        case Html: return edrcCore().toHtml(cr);
        } // switch
    }
    return QVariant();
}

Qt::ItemFlags CrListModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

/** Populate the model with the selected \e list of CR */
void CrListModel::setCrList(const QList<ConsultResult> &list)
{
    beginResetModel();
    _list = list;
    endResetModel();
}
