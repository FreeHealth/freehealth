/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  The FreeAccount plugins are free, open source FreeMedForms' plugins.   *
 *  (C) 2010-2011 by Pierre-Marie Desombre, MD <pm.desombre@medsyn.fr>     *
 *  and Eric Maeker, MD <eric.maeker@gmail.com>                            *
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
 *  Main Developpers : Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors :                                                         *
 *      NAME <MAIL@ADRESS>                                                 *
 ***************************************************************************/
#include "movementsmanager.h"

#include <accountbaseplugin/movementmodel.h>

using namespace AccountDB;

MovementsManager::MovementsManager(){}

MovementsManager::~MovementsManager(){}

QHash<int,QVariant> MovementsManager::getHashOfValues(int avMovId,
                                            QString &userUid,
                                            int bankId,
                                            int type,
                                            QString &label,
                                            QString &date,
                                            QString &dateValue,
                                            double valueCalculated,
                                            QString &comment,
                                            int validity,
                                            QString &trace,
                                            int isValid,
                                            QString &details){
    QHash<int,QVariant> hash;
    hash.insert(MovementModel::AvailableMovementId,avMovId);
    hash.insert(MovementModel::UserUid,userUid);
    hash.insert(MovementModel::AccountId,bankId);
    hash.insert(MovementModel::Type,type);
    hash.insert(MovementModel::Label,label);
    hash.insert(MovementModel::Date,date);
    hash.insert(MovementModel::DateOfValue,dateValue);
    hash.insert(MovementModel::Amount,valueCalculated);
    hash.insert(MovementModel::Comment,comment);
    hash.insert(MovementModel::Validity,validity);
    hash.insert(MovementModel::Trace,trace);
    hash.insert(MovementModel::IsValid,isValid);
    hash.insert(MovementModel::Details,details);
    return hash;
}

double MovementsManager::getCalculatedValue(double value, double percentage)
{
    double calculation = 0.00;
    calculation = value * percentage/100.00;
    return calculation;
}
