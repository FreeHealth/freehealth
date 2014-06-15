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
/**
 * \class eDRC::Internal::CrCriteriasModel
 * Manage the CR criterias and owns a realtime coding validation. \n
 * The coding system uses the Qt::CheckStateRole without the values. When setData() is called
 * with this role, the item checkstate is just toggled (check <-> uncheck). \n
 * You can get the coding validation status with currentCodingStatus().
 */

#include "crcriteriasmodel.h"
#include <edrcplugin/constants.h>
#include <edrcplugin/edrccore.h>
#include <edrcplugin/consultresult.h>
#include <edrcplugin/consultresultvalidator.h>
#include <edrcplugin/database/constants_db.h>
#include <edrcplugin/database/edrcbase.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <utils/global.h>

#include <QFont>

// Les libellés peuvent être simplifiés en remplaçant le début
// Constants::S_CR_USE_MODERNLABEL
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

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline eDRC::EdrcCore &edrcCore() {return eDRC::EdrcCore::instance();}
static inline eDRC::Internal::DrcDatabase &edrcBase() {return eDRC::EdrcCore::instance().edrcBase();}

namespace eDRC {
namespace Internal {
class CrCriteriasModelPrivate
{
public:
    CrCriteriasModelPrivate(CrCriteriasModel *parent) :
        _crId(-1),
        q(parent)
    {
        Q_UNUSED(q);
    }

    ~CrCriteriasModelPrivate()
    {}

    void getCriteriasFromDatabase(int crId)
    {
        _crId = crId;
        _criterias.clear();
        _criterias = edrcBase().getOrderedCriteriasForCr(crId);
        _validator.setCrId(_crId);
        _validator.setAvailableCriterias(_criterias);
        _validator.setSelectedCriterias(_checkedIds);
        _validator.check();
    }

public:
    int _crId;
    QList<ConsultResultCriteria> _criterias;
    QList<int> _checkedRows, _checkedIds;
    ConsultResultValidator _validator;

private:
    CrCriteriasModel *q;
};
}
}

CrCriteriasModel::CrCriteriasModel(QObject *parent) :
    QAbstractTableModel(parent),
    d(new CrCriteriasModelPrivate(this))
{
    setFilterOnCrId(-1);
}

CrCriteriasModel::~CrCriteriasModel()
{
    if (d)
        delete d;
    d = 0;
}

void CrCriteriasModel::clear()
{
    setFilterOnCrId(-1);
}

int CrCriteriasModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return d->_criterias.count();
}

int CrCriteriasModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return ColumnCount;
}

QVariant CrCriteriasModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (!IN_RANGE_STRICT_MAX(index.row(), 0, d->_criterias.count()))
        return QVariant();

    const ConsultResultCriteria &crit = d->_criterias.at(index.row());

    switch (role) {
    case Qt::DisplayRole:
    {
        switch (index.column()) {
        case Id: return crit.id();
        case Label: return crit.label(settings()->value(Constants::S_CR_USE_MODERNLABEL, true).toBool());
        case ItemWeight: return crit.weight();
        case Indentation: return crit.indentation();
        } // switch
        break;
    }
    case Qt::ToolTipRole:
    {
        if (index.column() == Label) {
            if (crit.isLineBreak())
                return QVariant();
            return QString("%1")
                    .arg(crit.label(settings()->value(Constants::S_CR_USE_MODERNLABEL, true).toBool()).trimmed())
                    //                .arg(crit.comment())
                    //                .arg(crit.weight())
                    //                .arg(crit.indentation())
                    ;
        }
        break;
    }
    case Qt::FontRole:
    {
        if (index.column() == Label) {
            if (crit.weight() == 1) {
                if (settings()->value(Constants::S_CR_MANDATORYLABEL_IN_BOLD, true).toBool()) {
                    // Root items
                    if (crit.indentation() == 1) {
                        QFont font;
                        font.setBold(true);
                        return font;
                    } else {
                        // Return bold only if parent is checked
                        int indent = crit.indentation();
                        int row = index.row() - 1;

                        while (indent > 1 && row >= 0) {
                            const ConsultResultCriteria &previous = d->_criterias.at(row);

                            // If previous line is not a label -> break
                            if (previous.isLineBreak())
                                break;

                            // Check previous line indentation
                            if (previous.indentation() < indent) {
                                if (d->_checkedRows.contains(row)) {
                                    QFont font;
                                    font.setBold(true);
                                    return font;
                                } else {
                                    return QVariant();
                                }
                            }
                            --row;
                        }
                    }
                }
            }
        }
        break;
    }
    case Qt::BackgroundRole:
    {
        // Selected items
        if (d->_checkedRows.contains(index.row())) {
            QColor c(QColor(settings()->value(Constants::S_CRVALIDATOR_COLORS_SELECTED_BACKGROUND).toString()));
            c.setAlpha(125);
            return c;
        }
        // Wrong coding
        if (settings()->value(Constants::S_REALTIME_CR_CODING_CHECKING, true).toBool()) {
            if (d->_validator.wrongCriteriaIds().contains(crit.id())) {
                QColor c(QColor(settings()->value(Constants::S_CRVALIDATOR_COLORS_ERROR_BACKGROUND).toString()));
                c.setAlpha(125);
                return c;
            }
        }
        break;
    }
    case Qt::ForegroundRole:
    {
        // Selected items
        if (d->_checkedRows.contains(index.row())) {
            QColor c(QColor(settings()->value(Constants::S_CRVALIDATOR_COLORS_SELECTED_FOREGROUND).toString()));
            return c;
        }
        // Wrong coding
        if (settings()->value(Constants::S_REALTIME_CR_CODING_CHECKING, true).toBool()) {
            if (d->_validator.wrongCriteriaIds().contains(crit.id())) {
                QColor c(QColor(settings()->value(Constants::S_CRVALIDATOR_COLORS_ERROR_FOREGROUND).toString()));
                return c;
            }
        }
        break;
    }
    default: break;
    } // switch

    return QVariant();
}

bool CrCriteriasModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(value);
    if (!index.isValid())
        return false;

    if (role == Qt::CheckStateRole) {

        if (!IN_RANGE_STRICT_MAX(index.row(), 0, d->_criterias.count()))
            return false;

        const ConsultResultCriteria &crit = d->_criterias.at(index.row());
        if (crit.isLineBreak())
            return true;

        if (!d->_checkedRows.contains(index.row())) {
            // If the item is now selected && the item has parent, all parent must be selected
            int indent = crit.indentation();
            int row = index.row() - 1;
            while (indent > 1 && row >= 0) {
                const ConsultResultCriteria &previous = d->_criterias.at(row);

                // If previous line is not a label -> break
                if (previous.isLineBreak())
                    break;

                // Check previous line indentation
                if (previous.indentation() < indent) {
                    indent = previous.indentation();
                    if (!d->_checkedRows.contains(row)) {
                        d->_checkedRows << row;
                        d->_checkedIds << previous.id();
                    }
                    Q_EMIT dataChanged(this->index(row, Label), this->index(row, Label));
                }
                --row;
            }
            d->_checkedRows << index.row();
            d->_checkedIds << crit.id();
        } else {
            // If the item is now deselected && the item has children, all children must be deselected
            int indent = crit.indentation();
            int row = index.row() + 1;
            while (row < d->_criterias.count()) {
                const ConsultResultCriteria &next = d->_criterias.at(row);

                // If next line is not a label -> break
                if (next.isLineBreak())
                    break;

                // Check next line indentation
                if (next.indentation() > indent) {
                    d->_checkedRows.removeAll(row);
                    d->_checkedIds.removeAll(next.id());
                    Q_EMIT dataChanged(this->index(row, Label), this->index(row, Label));
                }
                ++row;
            }
            d->_checkedRows.removeAll(index.row());
            d->_checkedIds.removeAll(crit.id());
        }
        Q_EMIT dataChanged(index, index);

        // Update validator
        d->_validator.setSelectedCriterias(d->_checkedIds);
        d->_validator.check();

//        qWarning() << d->_validator.wrongCriteriaIds();
    }
    return true;
}

Qt::ItemFlags CrCriteriasModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;// | Qt::ItemIsUserCheckable;
}

/** Filter CR criterias according to the CR primkey \e crId*/
void CrCriteriasModel::setFilterOnCrId(const int crId)
{
    beginResetModel();
    d->_checkedRows.clear();
    d->_checkedIds.clear();
    d->getCriteriasFromDatabase(crId);
    endResetModel();
}

/** Define the selected criterias using their database unique identifiers */
void CrCriteriasModel::setSelectedCriteriaIds(const QList<int> &ids)
{
    beginResetModel();
    // Update the selected ids/rows
    d->_checkedIds = ids;
    for(int i = 0; i < d->_criterias.count(); ++i) {
        const ConsultResultCriteria &crit = d->_criterias.at(i);
        if (ids.contains(crit.id()))
            d->_checkedRows << i;
    }
    // Update validator
    d->_validator.setSelectedCriterias(ids);
    d->_validator.check();
    endResetModel();
}

/** Returns the current filtered consultation result database unique identifiant */
int CrCriteriasModel::currentConsulResultId() const
{
    return d->_crId;
}

/** Returns the ConsultResult coding status. This status is available at any time (realtime computation) */
CrCriteriasModel::CodingStatus CrCriteriasModel::currentCodingStatus() const
{
    if (d->_checkedRows.isEmpty())
        return NoCodingStarted;

    if (d->_validator.wrongCriteriaIds().isEmpty())
        return ValidCoding;

    return IncompleteCoding;
}

/** Returns the currently selected (checked) criteria ids in a list */
QList<int> CrCriteriasModel::currentSelectedCriteriaIds() const
{
    return d->_checkedIds;
}
