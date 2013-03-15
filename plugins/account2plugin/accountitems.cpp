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
 *   Main Developpers:                                                     *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "accountitems.h"

namespace Account2 {

QDateTime VariableDatesItem::date(DateType type) const
{
    return _dates.value(type, QDateTime());
}

void VariableDatesItem::setDate(int type, const QDateTime &datetime)
{
    _dates.insert(type, datetime);
}

void VariableDatesItem::setDate(int type, const QDate &date)
{
    _dates.insert(type, QDateTime(date, QTime(0,0,0)));
}

QString VariableDatesItem::dateTypeUid(DateType type)
{
    switch (type) {
    case Date_MedicalRealisation: return "med_real";
    case Date_Invocing: return "inv";
    case Date_Payment: return "pay";
    case Date_Banking: return "bkg";
    case Date_Accountancy: return "acc";
    case Date_Creation: return "crea";
    case Date_Update: return "upd";
    case Date_Validation: return "val";
    case Date_Annulation: return "ann";
    case Date_ValidityPeriodStart: return "validitystart";
    case Date_ValidityPeriodEnd: return "validityend";
    }
    return QString::null;
}

} // namespace Account2
