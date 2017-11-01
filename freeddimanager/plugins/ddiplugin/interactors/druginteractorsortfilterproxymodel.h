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
#ifndef DDIMANAGER_DDIPLUGIN_DRUGINTERACTORSORTFILTERPROXYMODEL_H
#define DDIMANAGER_DDIPLUGIN_DRUGINTERACTORSORTFILTERPROXYMODEL_H

#include <ddiplugin/ddi_exporter.h>
#include <QSortFilterProxyModel>

/**
 * \file druginteractorsortfilterproxymodel.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 15 Oct 2013
*/

namespace DDI {
class DDI_EXPORT DrugInteractorSortFilterProxyModel : public QSortFilterProxyModel
{
public:
    enum ClassMoleculeFilter {
        ClassesOnly,
        MoleculesOnly,
        ClassesAndMolecules
    };
    explicit DrugInteractorSortFilterProxyModel(QObject *parent = 0);
    void setClassMolFilter(ClassMoleculeFilter classMolFilter);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &parent) const;

private:
    ClassMoleculeFilter _classMolFilter;
};
}  // namespace DDI

#endif // DDIMANAGER_DDIPLUGIN_DRUGINTERACTORSORTFILTERPROXYMODEL_H
