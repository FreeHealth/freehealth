/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "constants.h"

#include <QStringList>
#include <QCoreApplication>

namespace PMH {
namespace Constants {

QString typeToString(int type)
{
    if (type == NoTypeDefined)
        return QCoreApplication::translate(MH_CONSTANTS_TR_CONTEXT, "Not defined");
    if (type == ChronicDisease)
        return QCoreApplication::translate(MH_CONSTANTS_TR_CONTEXT, "Chronic disease");
    if (type == ChronicDiseaseWithoutAcuteEpisodes)
        return QCoreApplication::translate(MH_CONSTANTS_TR_CONTEXT, "Chronic disease without acute episode");
    if (type == AcuteDisease)
        return QCoreApplication::translate(MH_CONSTANTS_TR_CONTEXT, "Acute disease");
    return QString();
}

QStringList availableTypes()
{
    return QStringList()
            << QCoreApplication::translate(MH_CONSTANTS_TR_CONTEXT, "Not defined")
            << QCoreApplication::translate(MH_CONSTANTS_TR_CONTEXT, "Chronic disease")
            << QCoreApplication::translate(MH_CONSTANTS_TR_CONTEXT, "Chronic disease without acute episode")
            << QCoreApplication::translate(MH_CONSTANTS_TR_CONTEXT, "Acute disease")
            << QCoreApplication::translate(MH_CONSTANTS_TR_CONTEXT, "Risk factor");
}

int stringToType(const QString &type)
{
    if (type.compare(QCoreApplication::translate(MH_CONSTANTS_TR_CONTEXT, "Not defined"), Qt::CaseInsensitive)==0)
        return NoTypeDefined;
    if (type.compare(QCoreApplication::translate(MH_CONSTANTS_TR_CONTEXT, "Chronic disease"), Qt::CaseInsensitive)==0)
        return ChronicDisease;
    if (type.compare(QCoreApplication::translate(MH_CONSTANTS_TR_CONTEXT, "Chronic disease without acute episode"), Qt::CaseInsensitive)==0)
        return ChronicDiseaseWithoutAcuteEpisodes;
    if (type.compare(QCoreApplication::translate(MH_CONSTANTS_TR_CONTEXT, "Acute disease"), Qt::CaseInsensitive)==0)
        return AcuteDisease;
    if (type.compare(QCoreApplication::translate(MH_CONSTANTS_TR_CONTEXT, "Risk factor"), Qt::CaseInsensitive)==0)
        return RiskFactor;
    return NoTypeDefined;
}

QString statusToString(int state)
{
    if (state == NoStatusDefined)
        return QCoreApplication::translate(MH_CONSTANTS_TR_CONTEXT, "Not defined");
    if (state == IsActive)
        return QCoreApplication::translate(MH_CONSTANTS_TR_CONTEXT, "Active");
    if (state == IsInRemission)
        return QCoreApplication::translate(MH_CONSTANTS_TR_CONTEXT, "In remission");
    if (state == IsQuiescent)
        return QCoreApplication::translate(MH_CONSTANTS_TR_CONTEXT, "Quiescent");
    if (state == IsCured)
        return QCoreApplication::translate(MH_CONSTANTS_TR_CONTEXT, "Cured");
    return QString();
}

QStringList availableStatus()
{
    return QStringList()
            << QCoreApplication::translate(MH_CONSTANTS_TR_CONTEXT, "Not defined")
            << QCoreApplication::translate(MH_CONSTANTS_TR_CONTEXT, "Active")
            << QCoreApplication::translate(MH_CONSTANTS_TR_CONTEXT, "In remission")
            << QCoreApplication::translate(MH_CONSTANTS_TR_CONTEXT, "Quiescent")
            << QCoreApplication::translate(MH_CONSTANTS_TR_CONTEXT, "Cured");
}

int stringToStatus(const QString &status)
{
    if (status.compare(QCoreApplication::translate(MH_CONSTANTS_TR_CONTEXT, "Not defined"), Qt::CaseInsensitive)==0)
        return NoStatusDefined;
    if (status.compare(QCoreApplication::translate(MH_CONSTANTS_TR_CONTEXT, "Active"), Qt::CaseInsensitive)==0)
        return IsActive;
    if (status.compare(QCoreApplication::translate(MH_CONSTANTS_TR_CONTEXT, "In remission"), Qt::CaseInsensitive)==0)
        return IsInRemission;
    if (status.compare(QCoreApplication::translate(MH_CONSTANTS_TR_CONTEXT, "Quiescent"), Qt::CaseInsensitive)==0)
        return IsQuiescent;
    if (status.compare(QCoreApplication::translate(MH_CONSTANTS_TR_CONTEXT, "Cured"), Qt::CaseInsensitive)==0)
        return IsCured;
    return NoStatusDefined;
}


}
}
