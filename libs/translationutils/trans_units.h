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
#ifndef CONSTANTTRANSLATIONS_UNITS_H
#define CONSTANTTRANSLATIONS_UNITS_H

namespace Trans {
namespace Constants {

enum Length {
    Millimeter = 0,
    Centimeter,
    Decimeter,
    Meter,
    Kilometer,
    Inch,
    Foot,
    Yard,
    Mile
};

const char * const MILLIMETER_S = QT_TRANSLATE_NOOP("tkConstants", "Millimeter(s)");
const char * const CENTIMETER_S = QT_TRANSLATE_NOOP("tkConstants", "Centimeter(s)");
const char * const DECIMETER_S = QT_TRANSLATE_NOOP("tkConstants", "Decimeter(s)");
const char * const METER_S = QT_TRANSLATE_NOOP("tkConstants", "Meter(s)");
const char * const KILOMETER_S = QT_TRANSLATE_NOOP("tkConstants", "Kilometer(s)");

const char * const INCH_S = QT_TRANSLATE_NOOP("tkConstants", "Inch(es)");
const char * const FOOT_S = QT_TRANSLATE_NOOP("tkConstants", "Foot(s)");
const char * const YARD_S = QT_TRANSLATE_NOOP("tkConstants", "Yard(s)");
const char * const MILE_S = QT_TRANSLATE_NOOP("tkConstants", "Mile(s)");

enum Weight {
    Milligram = 0,
    Centigram,
    Decigram,
    Kilogram,
    Ounce,
    Pound
};

const char * const MILLIGRAM_S = QT_TRANSLATE_NOOP("tkConstants", "Milligram(s)");
const char * const CENTIGRAM_S = QT_TRANSLATE_NOOP("tkConstants", "Centigram(s)");
const char * const DECIGRAM_S = QT_TRANSLATE_NOOP("tkConstants", "Decigram(s)");
const char * const GRAM_S = QT_TRANSLATE_NOOP("tkConstants", "Gram(s)");
const char * const KILOGRAM_S = QT_TRANSLATE_NOOP("tkConstants", "Kilogram(s)");

const char * const OUNCE_S = QT_TRANSLATE_NOOP("tkConstants", "Ounce(s)");
const char * const POUND_S = QT_TRANSLATE_NOOP("tkConstants", "Pound(s)");

} // namespace Constants
} // namespace TranslationUtils

#endif // CONSTANTTRANSLATIONS_UNITS_H
