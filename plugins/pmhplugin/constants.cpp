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
#include "constants.h"

#include <QStringList>
#include <QCoreApplication>

namespace PMH {
namespace Constants {

QString typeToString(int type)
{
    if (type == NoTypeDefined)
        return QCoreApplication::translate(PMH_CONSTANTS_TR_CONTEXT, NOTDEFINED);
    if (type == ChronicDisease)
        return QCoreApplication::translate(PMH_CONSTANTS_TR_CONTEXT, TYPE_CHRONICDISEASE);
    if (type == ChronicDiseaseWithoutAcuteEpisodes)
        return QCoreApplication::translate(PMH_CONSTANTS_TR_CONTEXT, TYPE_CHRONICDISEASE_WO_ACUTE);
    if (type == AcuteDisease)
        return QCoreApplication::translate(PMH_CONSTANTS_TR_CONTEXT, TYPE_ACUTE);
    if (type == RiskFactor)
        return QCoreApplication::translate(PMH_CONSTANTS_TR_CONTEXT, RISK_FACTOR);
    return QString();
}

QStringList availableTypes()
{
    return QStringList()
            << QCoreApplication::translate(PMH_CONSTANTS_TR_CONTEXT, NOTDEFINED)
            << QCoreApplication::translate(PMH_CONSTANTS_TR_CONTEXT, TYPE_CHRONICDISEASE)
            << QCoreApplication::translate(PMH_CONSTANTS_TR_CONTEXT, TYPE_CHRONICDISEASE_WO_ACUTE)
            << QCoreApplication::translate(PMH_CONSTANTS_TR_CONTEXT, TYPE_ACUTE)
            << QCoreApplication::translate(PMH_CONSTANTS_TR_CONTEXT, RISK_FACTOR);
}

int stringToType(const QString &type)
{
    if (type.compare(QCoreApplication::translate(PMH_CONSTANTS_TR_CONTEXT, NOTDEFINED), Qt::CaseInsensitive)==0)
        return NoTypeDefined;
    if (type.compare(QCoreApplication::translate(PMH_CONSTANTS_TR_CONTEXT, TYPE_CHRONICDISEASE), Qt::CaseInsensitive)==0)
        return ChronicDisease;
    if (type.compare(QCoreApplication::translate(PMH_CONSTANTS_TR_CONTEXT, TYPE_CHRONICDISEASE_WO_ACUTE), Qt::CaseInsensitive)==0)
        return ChronicDiseaseWithoutAcuteEpisodes;
    if (type.compare(QCoreApplication::translate(PMH_CONSTANTS_TR_CONTEXT, TYPE_ACUTE), Qt::CaseInsensitive)==0)
        return AcuteDisease;
    if (type.compare(QCoreApplication::translate(PMH_CONSTANTS_TR_CONTEXT, RISK_FACTOR), Qt::CaseInsensitive)==0)
        return RiskFactor;
    return NoTypeDefined;
}

QString statusToString(int state)
{
    if (state == NoStatusDefined)
        return QCoreApplication::translate(PMH_CONSTANTS_TR_CONTEXT, NOTDEFINED);
    if (state == IsActive)
        return QCoreApplication::translate(PMH_CONSTANTS_TR_CONTEXT, STATUS_ACTIVE);
    if (state == IsInRemission)
        return QCoreApplication::translate(PMH_CONSTANTS_TR_CONTEXT, STATUS_INREMISSION);
    if (state == IsQuiescent)
        return QCoreApplication::translate(PMH_CONSTANTS_TR_CONTEXT, STATUS_QUIESCENT);
    if (state == IsCured)
        return QCoreApplication::translate(PMH_CONSTANTS_TR_CONTEXT, STATUS_CURED);
    return QString();
}

QStringList availableStatus()
{
    return QStringList()
            << QCoreApplication::translate(PMH_CONSTANTS_TR_CONTEXT, NOTDEFINED)
            << QCoreApplication::translate(PMH_CONSTANTS_TR_CONTEXT, STATUS_ACTIVE)
            << QCoreApplication::translate(PMH_CONSTANTS_TR_CONTEXT, STATUS_INREMISSION)
            << QCoreApplication::translate(PMH_CONSTANTS_TR_CONTEXT, STATUS_QUIESCENT)
            << QCoreApplication::translate(PMH_CONSTANTS_TR_CONTEXT, STATUS_CURED);
}

int stringToStatus(const QString &status)
{
    if (status.compare(QCoreApplication::translate(PMH_CONSTANTS_TR_CONTEXT, NOTDEFINED), Qt::CaseInsensitive)==0)
        return NoStatusDefined;
    if (status.compare(QCoreApplication::translate(PMH_CONSTANTS_TR_CONTEXT, STATUS_ACTIVE), Qt::CaseInsensitive)==0)
        return IsActive;
    if (status.compare(QCoreApplication::translate(PMH_CONSTANTS_TR_CONTEXT, STATUS_INREMISSION), Qt::CaseInsensitive)==0)
        return IsInRemission;
    if (status.compare(QCoreApplication::translate(PMH_CONSTANTS_TR_CONTEXT, STATUS_QUIESCENT), Qt::CaseInsensitive)==0)
        return IsQuiescent;
    if (status.compare(QCoreApplication::translate(PMH_CONSTANTS_TR_CONTEXT, STATUS_CURED), Qt::CaseInsensitive)==0)
        return IsCured;
    return NoStatusDefined;
}


}
}
