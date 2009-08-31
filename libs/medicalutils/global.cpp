/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/

/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "global.h"

#include <translationutils/constanttranslations.h>

#include <QDateTime>
#include <QString>
#include <QStringList>
#include <QVariant>

using namespace MedicalUtils;
using namespace MedicalUtils::Globals;
using namespace Trans::Constants;
using namespace Trans::ConstantTranslations;

/** \brief Returns a readable age calculated from the date to now */
QString readableAge(const QDateTime &DOB)
{
    int daysTo = DOB.daysTo(QDateTime::currentDateTime());
    double age = daysTo / 365.242199;
    QStringList readableAge;
    // years
    int years = (int)age;
    if (years>0) {
        QString tmp = QString::number(years) + " ";
        tmp.append(tkTr(YEARS, years));
        readableAge << tmp;
        age -= years;
    }
    // months
    int months = age * 12;
    if (months > 0) {
        QString tmp = QString::number(months) + " ";
        age -= months / 12.0;
        tmp.append(tkTr(MONTHS, months));
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


/** \brief Calculates clearance of creatinin using Cockroft and Goult formula */
double clearanceCreatinin(const int ageYears, const int weightKg, const double creatMlMin, const bool isMale)
{
    double cockroft = ((140 - ageYears) * weightKg) / (creatMlMin * 7.2);
    if (!isMale)
        cockroft *= 0.85;
    return cockroft;
}
