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
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, MD <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 * \class DDI::DrugInteractorSortFilterProxyModel
 * Sort filter over the DDI::DrugInteractorTableModel. Allow to switch
 * between ClassesOnly, MoleculesOnly, ClassesAndMolecules sorting/filtering.
 */

#include "druginteractorsortfilterproxymodel.h"
#include "druginteractortablemodel.h"

#include <QVariant>
#include <QStringList>

using namespace DDI;

DrugInteractorSortFilterProxyModel::DrugInteractorSortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent),
      _classMolFilter(ClassesAndMolecules)
{}

void DrugInteractorSortFilterProxyModel::setClassMolFilter(ClassMoleculeFilter classMolFilter)
{
    _classMolFilter=classMolFilter;
    //invalidate();
}

bool DrugInteractorSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &parent) const
{
    // First of all, search the string in the class children uids
    QModelIndex isClassIndex = sourceModel()->index(sourceRow, DrugInteractorTableModel::IsInteractingClass);
    bool isClass = isClassIndex.data().toBool();
    if (isClass) {
        QModelIndex children = sourceModel()->index(sourceRow, DrugInteractorTableModel::ChildrenUuid);
        if (children.data().toStringList().filter(filterRegExp()).count() > 0)
            return true;
    }
    // Then filter according to the extra-search filter
    if (_classMolFilter==ClassesOnly) {
        if (!isClass)
            return false;
    } else if (_classMolFilter==MoleculesOnly) {
        if (isClass)
            return false;
    }
    return QSortFilterProxyModel::filterAcceptsRow(sourceRow, parent);
}
