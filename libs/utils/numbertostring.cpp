/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "numbertostring.h"
#include "math.h"

#include <translationutils/constants.h>
#include <translationutils/trans_numbers.h>
#include <translationutils/trans_current.h>

#include <QStringList>

#include <QDebug>

using namespace Trans::ConstantTranslations;

namespace Utils
{

// Return readable string for number from 1 to 9
QString digits(int digit)
{
    switch (digit) {
    case 0:
      return QString::null;
    case 1:
      return tkTr(Trans::Constants::ONE);
    case 2:
      return tkTr(Trans::Constants::TWO);
    case 3:
      return tkTr(Trans::Constants::THREE);
    case 4:
      return tkTr(Trans::Constants::FOUR);
    case 5:
      return tkTr(Trans::Constants::FIVE);
    case 6:
      return tkTr(Trans::Constants::SIX);
    case 7:
      return tkTr(Trans::Constants::SEVEN);
    case 8:
      return tkTr(Trans::Constants::EIGHT);
    case 9:
      return tkTr(Trans::Constants::NINE);
    }
    return QString::null;
}

// Return readable string for number from 10 to 99
QString teens(int number)
{
    int quotient = number % 100;
    int digit = quotient % 10;
    quotient = quotient / 10;

    switch (quotient) {
    case 0: return digits(digit);
    case 1:
    {
        switch (digit) {
        case 0 : return tkTr(Trans::Constants::TEN);
        case 1 : return tkTr(Trans::Constants::ELEVEN);
        case 2 : return tkTr(Trans::Constants::TWELVE);
        case 3 : return tkTr(Trans::Constants::THIRTEEN);
        case 4 : return tkTr(Trans::Constants::FOURTEEN);
        case 5 : return tkTr(Trans::Constants::FIFTEEN);
        case 6 : return tkTr(Trans::Constants::SIXTEEN);
        case 7 : return tkTr(Trans::Constants::SEVENTEEN);
        case 8 : return tkTr(Trans::Constants::EIGHTEEN);
        case 9 : return tkTr(Trans::Constants::NINETEEN);
        }
    }
    case 2:
        return QString("%1 %2").arg(tkTr(Trans::Constants::TWENTY)).arg(digits(digit));
    case 3:
        return QString("%1 %2").arg(tkTr(Trans::Constants::THIRTY)).arg(digits(digit));
    case 4:
        return QString("%1 %2").arg(tkTr(Trans::Constants::FOURTY)).arg(digits(digit));
    case 5:
        return QString("%1 %2").arg(tkTr(Trans::Constants::FIFTY)).arg(digits(digit));
    case 6:
        return QString("%1 %2").arg(tkTr(Trans::Constants::SIXTY)).arg(digits(digit));
    case 7:
        return QString("%1 %2").arg(tkTr(Trans::Constants::SEVENTY)).arg(digits(digit));
    case 8:
        return QString("%1 %2").arg(tkTr(Trans::Constants::EIGHTY)).arg(digits(digit));
    case 9:
        return QString("%1 %2").arg(tkTr(Trans::Constants::NINETY)).arg(digits(digit));
    }
    return QString::null;
}

// Return thousand, million, billion, trillion
QString millions(int index)
{
    switch (index) {
    case 0: return QString::null;
    case 1: return tkTr(Trans::Constants::THOUSAND);
    case 2: return tkTr(Trans::Constants::MILLION);
    case 3: return tkTr(Trans::Constants::BILLION);
    case 4: return tkTr(Trans::Constants::TRILLION);
    }
    return QString::null;
}

// Return a string segment (from 0 to 999)
QString stringSegment(int number)
{
    QStringList seg;

    if (number >= 100)
        seg << QString("%1 %2").arg(digits(number / 100)).arg(tkTr(Trans::Constants::HUNDRED));

    if ((number % 100) < 10)
        seg << digits(number % 100);
    else
        seg << QString("%1").arg(teens(number % 100));

    return seg.join(" ");
}

/**
 * Returns the human readable string corresponding to the number (integer and decimal) into
 * a stringlist: [0] is the integer, [1] is the decimal (if decimal > 0, otherwise there are no [1]).
 * Eg: 1222 -> one thousand two hundred twenty-two
 * TODO: (if those functions are actually used in the future) use https://github.com/trodevel/HumanizerCpp
 */
QString integerToHumanReadableString(int n)
{
    if (n == 0.0)
        return "zero";

    QString work;

    QStringList parts;


    for (int i = 0; i < 4; i++) {
        parts << stringSegment((int)n % (int)(pow(1000, i+1)) / pow(1000, i));
    }

    for (int i = 0; i < parts.length(); i++) {
        if (parts[i].isEmpty())
            continue;
        work = parts[i] + " " + millions(i) + " " + work;
    }

    work = work.simplified();

    // French specifics:
    // un mille, un cent
    if (QLocale().language() == QLocale::French) {
        work = work.replace(QString("%1 %2")
                            .arg(tkTr(Trans::Constants::ONE))
                            .arg(tkTr(Trans::Constants::HUNDRED)), tkTr(Trans::Constants::HUNDRED));
        work = work.replace(QString("%1 %2")
                            .arg(tkTr(Trans::Constants::ONE))
                            .arg(tkTr(Trans::Constants::THOUSAND)), tkTr(Trans::Constants::THOUSAND));

        // 21, 31, 41... 81
        QStringList etUn;
        etUn << tkTr(Trans::Constants::TWENTY)
             << tkTr(Trans::Constants::THIRTY)
             << tkTr(Trans::Constants::FOURTY)
             << tkTr(Trans::Constants::FIFTY)
             << tkTr(Trans::Constants::SIXTY)
             << tkTr(Trans::Constants::SEVENTY)
             << tkTr(Trans::Constants::EIGHTY);
        foreach(const QString &un, etUn) {
            work = work.replace(QString("%1 %2")
                                .arg(un)
                                .arg(tkTr(Trans::Constants::ONE))
                                , QString("%1 %2 %3")
                                .arg(un)
                                .arg(tkTr(Trans::Constants::AND))
                                .arg(tkTr(Trans::Constants::ONE))
                                );
        }

        // 91 -> 99
        QHash<QString, QString> s90;
        s90.insert(tkTr(Trans::Constants::ONE), "quatre-vingt-onze");
        s90.insert(tkTr(Trans::Constants::TWO), "quatre-vingt-douze");
        s90.insert(tkTr(Trans::Constants::THREE), "quatre-vingt-treize");
        s90.insert(tkTr(Trans::Constants::FOUR), "quatre-vingt-quatorze");
        s90.insert(tkTr(Trans::Constants::FIVE), "quatre-vingt-quinze");
        s90.insert(tkTr(Trans::Constants::SIX), "quatre-vingt-seize");
        s90.insert(tkTr(Trans::Constants::SEVEN), "quatre-vingt-dix-sept");
        s90.insert(tkTr(Trans::Constants::EIGHT), "quatre-vingt-dix-huit");
        s90.insert(tkTr(Trans::Constants::NINE), "quatre-vingt-dix-neuf");
        foreach(const QString &s, s90.keys()) {
            work = work.replace(QString("%1 %2")
                                .arg(tkTr(Trans::Constants::NINETY))
                                .arg(s),
                                s90.value(s)
                                );
        }
    }
    return work;
}

QStringList doubleToHumanReadableString(int integer, int decimal)
{
    QStringList toReturn;
    toReturn << integerToHumanReadableString(integer);
    if (decimal > 0)
        toReturn << QString("%1 %2").arg(integerToHumanReadableString(decimal)).arg(Trans::Constants::CENT);
    return toReturn;
}

/**
 * Returns the human readable string corresponding to the number (double) into
 * a stringlist: [0] is the integer part, [1] is the decimal part. \n
 * The decimal part is limited at 5 numbers
 */
QStringList doubleToHumanReadableString(double doubleNumber)
{
    QString n = QString::number(doubleNumber, 'f', 5);
    QStringList v = n.split('.');
    int integer = v.at(0).toInt();
    int decimal = v.at(1).toInt();
    while ((decimal > 0) && (decimal % 10 == 0)) decimal = decimal / 10;
    return doubleToHumanReadableString(integer, decimal);
}

}
