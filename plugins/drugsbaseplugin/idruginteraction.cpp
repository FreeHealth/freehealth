/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "idruginteraction.h"
#include <drugsbaseplugin/idrugengine.h>

#include <QStringList>
#include <QDebug>

QDebug operator<<(QDebug dbg, const DrugsDB::IDrugInteraction &c)
{
    QStringList drugs;
    for(int i=0; i<c.drugs().count(); ++i) {
        drugs << c.drugs().at(i)->brandName();
    }
    QString type;
    if (c.isDrugDrugInteraction())
        type = "DDI";
    else if (c.isPotentiallyInappropriate())
        type = "PIM";
    dbg.nospace() << "IDrugInteraction("
                  << type << "; "
                  << c.type() << "\n"
                  << drugs.join(",") << "; "
                  << "engine: " << c.engine()->uid()<< "; "
                  << ")";
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const DrugsDB::IDrugInteraction *c)
{
    if (c)
        dbg = operator<<(dbg, *c);
    else
        dbg.nospace() << "IDrugInteraction(0x0)";
    return dbg.space();
}

