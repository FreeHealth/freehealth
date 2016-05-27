/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developers:  Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#include "movementsmanager.h"
#include <accountbaseplugin/constants.h>

#ifndef FREEMEDFORMS
#include <coreplugin/icore.h>
#include <coreplugin/icommandline.h>
#include <coreplugin/freeaccount_constants.h>
static inline Core::ICommandLine *commandLine() {return Core::ICore::instance()->commandLine();}
#endif

using namespace AccountDB;
using namespace Constants;

MovementsManager::MovementsManager(){}

MovementsManager::~MovementsManager(){}

QHash<int,QVariant> MovementsManager::getHashOfValues(int acMovId,
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
    hash.insert(MOV_AV_MOVEMENT_ID,acMovId);
    hash.insert(MOV_USER_UID,userUid);
    hash.insert(MOV_ACCOUNT_ID,bankId);
    hash.insert(MOV_TYPE,type);
    hash.insert(MOV_LABEL,label);
    hash.insert(MOV_DATE,date);
    hash.insert(MOV_DATEOFVALUE,dateValue);
    hash.insert(MOV_AMOUNT,valueCalculated);
    hash.insert(MOV_COMMENT,comment);
    hash.insert(MOV_VALIDITY,validity);
    hash.insert(MOV_TRACE,trace);
    hash.insert(MOV_ISVALID,isValid);
    hash.insert(MOV_DETAILS,details);
    return hash;
}

double MovementsManager::getCalculatedValue(double value, double percentage)
{
    double calculation = 0.00;
    calculation = value * percentage/100.00;
    return calculation;
}

bool MovementsManager::isMedintuxArg()
{
#ifdef FREEMEDFORMS
    return false;
#else
    return commandLine()->value(Core::Constants::CL_MedinTux).toBool();
#endif
}

bool MovementsManager::isFreaccountIndependant()
{
#ifdef FREEMEDFORMS
    return false;
#else
    return commandLine()->value(Core::Constants::CL_Independant).toBool();
#endif
}
