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
 *   Main Developers :                                                    *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef PACKDEPENDENCYCHECKER_H
#define PACKDEPENDENCYCHECKER_H

#include <datapackutils/pack.h>

#include <QObject>

/**
 * \file packdependencychecker.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.2
 * \date 30 Jan 2012
*/

namespace DataPack {

class PackDependencyChecker : public QObject
{
    Q_OBJECT
public:
    explicit PackDependencyChecker(QObject *parent = 0);

    QList<Pack> packDependencies(const Pack &pack, const PackDependencyData::TypeOfDependence &dependence);

    void testCombination(const QList<Pack> &installPacks, const QList<Pack> &updatePacks, const QList<Pack> &removePacks);
    bool isCombinationCorrect() const;

    QList<Pack> neededToInstall() const;
    QList<Pack> neededToUpdate() const;
    QList<Pack> neededToRemove() const;

private:
    QList<Pack> m_ToInstall, m_ToUpdate, m_ToRemove;
    bool m_IsCorrect;
};

}  // End namespace DataPack

#endif // PACKDEPENDENCYCHECKER_H
