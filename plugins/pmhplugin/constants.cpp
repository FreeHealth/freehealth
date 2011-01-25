/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
    if (type == ChronicDisease)
        return QCoreApplication::translate(MH_CONSTANTS_TR_CONTEXT, "Chronic disease");
    if (type == ChronicDiseaseWithoutAcuteEpisodes)
        return QCoreApplication::translate(MH_CONSTANTS_TR_CONTEXT, "Chronic disease without acute episode");
    if (type == AcuteDisease)
        return QCoreApplication::translate(MH_CONSTANTS_TR_CONTEXT, "Acute disease");
    return QString();
}

QString statusToString(int state)
{
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


}
}
