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
#ifndef CONSTANTTRANSLATIONS_DATETIME_H
#define CONSTANTTRANSLATIONS_DATETIME_H

#include <qglobal.h>

namespace Trans {
namespace Constants {

namespace Time {

enum Period
{
    Seconds = 0,
    Minutes,
    Hours,
    Days,
    Weeks,
    Months,
    Quarter,
    Year,
    Decade
};

enum MealTime
{
    Undefined = 0,
    NoRelationWithMeal,
    DuringMeal,
    BeforeMeal,
    AfterMeal,
    OutsideMeal,
    OnlyIfMeal,
    SeparatedFromFood,
    SuggestedWithFood,
    WithOrWithoutFood
};


enum DailyScheme
{
    DS_WakeUpTime = 0,
    DS_Breakfast,
    DS_Morning,
    DS_10OClockBrunch,
    DS_MidDay,
    DS_Lunch,
    DS_TeaTime,
    DS_Evening,
    DS_Diner,
    DS_BedTime,
    DS_MaxParam
};
//    Q_DECLARE_FLAGS( DailySchemes, DailyScheme );
}  // End Time
//Q_DECLARE_OPERATORS_FOR_FLAGS(Time::DailySchemes)

// Time and periods
const char* const DURATION   = QT_TRANSLATE_NOOP("tkConstants", "Duration");
const char* const SECOND_S    = QT_TRANSLATE_NOOP("tkConstants", "second(s)");
const char* const MINUTE_S    = QT_TRANSLATE_NOOP("tkConstants", "minute(s)");
const char* const HOUR_S      = QT_TRANSLATE_NOOP("tkConstants", "hour(s)");
const char* const DAY_S       = QT_TRANSLATE_NOOP("tkConstants", "day(s)");
const char* const WEEK_S      = QT_TRANSLATE_NOOP("tkConstants", "week(s)");
const char* const MONTH_S     = QT_TRANSLATE_NOOP("tkConstants", "month(s)");
const char* const QUARTER_S   = QT_TRANSLATE_NOOP("tkConstants", "quarter(s)");
const char* const YEAR_S      = QT_TRANSLATE_NOOP("tkConstants", "year(s)");
const char* const DECADE_S    = QT_TRANSLATE_NOOP("tkConstants", "decade(s)");

const char* const SECOND    = QT_TRANSLATE_NOOP("tkConstants", "second");
const char* const MINUTE    = QT_TRANSLATE_NOOP("tkConstants", "minute");
const char* const HOUR      = QT_TRANSLATE_NOOP("tkConstants", "hour");
const char* const DAY       = QT_TRANSLATE_NOOP("tkConstants", "day");
const char* const WEEK      = QT_TRANSLATE_NOOP("tkConstants", "week");
const char* const MONTH     = QT_TRANSLATE_NOOP("tkConstants", "month");
const char* const QUARTER   = QT_TRANSLATE_NOOP("tkConstants", "quarter");
const char* const YEAR      = QT_TRANSLATE_NOOP("tkConstants", "year");
const char* const DECADE    = QT_TRANSLATE_NOOP("tkConstants", "decade");

const char* const SECONDS    = QT_TRANSLATE_NOOP("tkConstants", "seconds");
const char* const MINUTES    = QT_TRANSLATE_NOOP("tkConstants", "minutes");
const char* const HOURS      = QT_TRANSLATE_NOOP("tkConstants", "hours");
const char* const DAYS       = QT_TRANSLATE_NOOP("tkConstants", "days");
const char* const WEEKS      = QT_TRANSLATE_NOOP("tkConstants", "weeks");
const char* const MONTHS     = QT_TRANSLATE_NOOP("tkConstants", "months");
const char* const QUARTERS   = QT_TRANSLATE_NOOP("tkConstants", "quarters");
const char* const YEARS      = QT_TRANSLATE_NOOP("tkConstants", "years");
const char* const DECADES    = QT_TRANSLATE_NOOP("tkConstants", "decades");

// Day
const char* const WAKEUPTIME           = QT_TRANSLATE_NOOP("tkConstants", "wake up");
const char* const BREAKFAST            = QT_TRANSLATE_NOOP("tkConstants", "breakfast");
const char* const TENOCLOCKBRUNCH      = QT_TRANSLATE_NOOP("tkConstants", "mid-morning");
const char* const MORNING              = QT_TRANSLATE_NOOP("tkConstants", "morning");
const char* const MIDDAY               = QT_TRANSLATE_NOOP("tkConstants", "midday");
const char* const LUNCH                = QT_TRANSLATE_NOOP("tkConstants", "lunch");
const char* const AFTERNOON            = QT_TRANSLATE_NOOP("tkConstants", "afternoon");
const char* const TEATIME              = QT_TRANSLATE_NOOP("tkConstants", "mid-afternoon");
const char* const DINER                = QT_TRANSLATE_NOOP("tkConstants", "dinner");
const char* const EVENING              = QT_TRANSLATE_NOOP("tkConstants", "evening");
const char* const BEDTIME              = QT_TRANSLATE_NOOP("tkConstants", "bedtime");

// Meal time relations
const char* const NOMEALRELATION      = QT_TRANSLATE_NOOP("tkConstants", "no meal relation");
const char* const DURINGMEAL          = QT_TRANSLATE_NOOP("tkConstants", "during meal");
const char* const AFTERMEAL           = QT_TRANSLATE_NOOP("tkConstants", "after meal");
const char* const BEFOREMEAL          = QT_TRANSLATE_NOOP("tkConstants", "before meal");
const char* const NOTDURINGMEAL       = QT_TRANSLATE_NOOP("tkConstants", "not during meal");
const char* const ONLYIFMEAL          = QT_TRANSLATE_NOOP("tkConstants", "only if meal");
const char* const SEPARATEDFROMFOOD   = QT_TRANSLATE_NOOP("tkConstants", "separated from food (1-2 hours)");
const char* const SUGGESTEDWITHFOOD   = QT_TRANSLATE_NOOP("tkConstants", "suggested with food");
const char* const WITHORWITHOUTFOOD   = QT_TRANSLATE_NOOP("tkConstants", "with or without food");

const char* const SHOW_LONG_FORMAT      = QT_TRANSLATE_NOOP("tkConstants", "Show long date format");
const char* const SHOW_SHORT_FORMAT     = QT_TRANSLATE_NOOP("tkConstants", "Show short date format");
const char* const SHOW_NUMERIC_FORMAT   = QT_TRANSLATE_NOOP("tkConstants", "Show numeric date format");
const char* const ENTER_DATE            = QT_TRANSLATE_NOOP("tkConstants", "Enter a date");
const char* const ENTER_DATE_FORMAT_1   = QT_TRANSLATE_NOOP("tkConstants", "Enter a date using the formats:<br/>%1");
const char* const NO_DATE               = QT_TRANSLATE_NOOP("tkConstants", "No date");

const char* const DATE_OF_RELEASE    = QT_TRANSLATE_NOOP("tkConstants", "Date of release");
const char* const DATE_OF_CREATION   = QT_TRANSLATE_NOOP("tkConstants", "Date of creation");
const char* const DATE_LAST_UPDATE   = QT_TRANSLATE_NOOP("tkConstants", "Last update");

} // end Constants
} // end TranslationUtils

#endif // CONSTANTTRANSLATIONS_DATETIME_H
