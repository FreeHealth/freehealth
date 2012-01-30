/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developpers :                                                    *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "packdependencychecker.h"

#include <datapackutils/datapackcore.h>
#include <datapackutils/iservermanager.h>

using namespace DataPack;

static inline DataPack::DataPackCore &core() { return DataPack::DataPackCore::instance(); }
static inline DataPack::IServerManager *serverManager() { return DataPack::DataPackCore::instance().serverManager(); }

PackDependencyChecker::PackDependencyChecker(QObject *parent) :
    QObject(parent),
    m_IsCorrect(false)
{
}

void PackDependencyChecker::testCombination(const QList<Pack> &installPacks, const QList<Pack> &updatePacks, const QList<Pack> &removePacks)
{
}

bool PackDependencyChecker::isCombinationCorrect() const
{
    return true;
}

QList<Pack> PackDependencyChecker::neededToInstall() const
{}

QList<Pack> PackDependencyChecker::neededToUpdate() const
{}

QList<Pack> PackDependencyChecker::neededToRemove() const
{}
