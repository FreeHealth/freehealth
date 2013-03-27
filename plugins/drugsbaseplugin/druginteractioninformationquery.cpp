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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "druginteractioninformationquery.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <drugsbaseplugin/constants.h>

#include <QString>

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}

using namespace DrugsDB;

DrugInteractionInformationQuery::DrugInteractionInformationQuery(const DrugInteractionInformationQuery &query) :
        messageType(query.messageType),
        processTime(query.processTime),
        iconSize(query.iconSize),
        levelOfWarningStaticAlert(query.levelOfWarningStaticAlert),
        levelOfWarningDynamicAlert(query.levelOfWarningDynamicAlert),
        engineUid(query.engineUid),
        result(query.result),
        relatedDrug(query.relatedDrug)
{
}

DrugInteractionInformationQuery::DrugInteractionInformationQuery() :
        messageType(-1), processTime(-1), iconSize(-1), result(0), relatedDrug(0)
{
    levelOfWarningStaticAlert = settings()->value(Constants::S_LEVELOFWARNING_STATICALERT).toInt();
    levelOfWarningDynamicAlert = settings()->value(Constants::S_LEVELOFWARNING_DYNAMICALERT).toInt();
}

DrugInteractionInformationQuery::~DrugInteractionInformationQuery()
{}
