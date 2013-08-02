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
 ***************************************************************************/

/**
  \namespace MedicalUtils
  \brief Some generic funtions and classes for all medical calculations.
*/

#include "global.h"

#include <translationutils/constants.h>
#include <translationutils/trans_datetime.h>

#include <QDateTime>
#include <QDate>
#include <QString>
#include <QStringList>
#include <QVariant>

using namespace MedicalUtils;
using namespace Trans::Constants;
using namespace Trans::ConstantTranslations;

namespace MedicalUtils {

/** \brief Returns a readable age calculated from the date to now */
QString readableAge(const QDate &DOB)
{
    QDate current = QDate::currentDate();
    // Check anniversary
    if (current.month() == DOB.month()
            && current.day() == DOB.day()) {
        int years = current.year() - DOB.year();
        return QString("%1 %2").arg(years).arg(tkTr(YEAR_S, years));
    }
    // Compute average age
    int daysTo = DOB.daysTo(current);
    double age = daysTo / 365.242199;
    QStringList readableAge;
    // years
    int years = (int)age;
    if (years>0) {
        QString tmp = QString::number(years) + " ";
        tmp.append(tkTr(YEAR_S, years));
        readableAge << tmp;
        age -= years;
    }
    // months
    int months = age * 12;
    if (months > 0) {
        QString tmp = QString::number(months) + " ";
        age -= months / 12.0;
        tmp.append(tkTr(MONTH_S, months));
        readableAge << tmp;
    }
    int days = daysTo - (years*365.25) - (months*12);
    if (age > 0) {
        QString tmp = QString::number((int)(age*365.242199)) + " ";
        tmp.append(tkTr(DAYS, days));
        readableAge << tmp;
    }
    return readableAge.join(" ");
}

/** \brief Returns a readable age calculated from the date to now */
int ageYears(const QDate &DOB)
{
    int daysTo = DOB.daysTo(QDate::currentDate());
    double age = daysTo / 365.242199;
    return (int)age;
}

/** \brief Calculates clearance of creatinin using Cockroft and Goult formula */
double clearanceCreatinin(const int ageYears, const int weightKg, const double creatMlMin, const bool isMale)
{
    double cockroft = ((140 - ageYears) * weightKg) / (creatMlMin * 7.2);
    if (!isMale)
        cockroft *= 0.85;
    return cockroft;
}

}  // End MedicalUtils
