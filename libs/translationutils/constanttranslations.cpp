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

/**
  \namespace Trans
  \brief Contains all translated strings, and members to ease the access to some translations.
*/

#include "constanttranslations.h"

#include <QLocale>
#include <QDebug>

using namespace Trans;
using namespace Trans::ConstantTranslations;

namespace Trans {
namespace ConstantTranslations {

/**
  \brief Only work with the declared constants in tkConstants.
  Plural are not implemented
*/
QString tkTr(const char* toTr, const int plurials)
{
    Q_UNUSED(plurials);
//    QString tmp = QCoreApplication::translate(CONSTANTS_TR_CONTEXT, toTr, "", QCoreApplication::UnicodeUTF8, plurials);
//    if (tmp.contains("(s)")) {
//        switch (plurials)
//        {
//            case 0 :
//            {
//                if (QLocale().name().left(2)=="en")
//                    tmp.replace("(s)","s");
//                else if (QLocale().name().left(2)=="fr")
//                    tmp.replace("(s)","");
//                break;
//            }
//            case 1:
//            {
//                tmp.replace("(s)","");
//                break;
//            }
//            default:
//            {
//                tmp.replace("(s)","s");
//                break;
//            }
//        }
//    }
    return QCoreApplication::translate(Constants::CONSTANTS_TR_CONTEXT, toTr);
}

QString checkUpdateLabel(const int index)
{
    switch (index) {
    case Constants::CheckUpdate_AtStartup: return tkTr(Constants::AT_STARTUP);
//    case Constants::CheckUpdate_EachDays: return tkTr(Constants::EACH_DAYS);
    case Constants::CheckUpdate_EachWeeks:return tkTr(Constants::EACH_WEEK);
    case Constants::CheckUpdate_EachMonth:return tkTr(Constants::EACH_MONTH);
    case Constants::CheckUpdate_EachQuarters:return tkTr(Constants::EACH_QUARTER);
    case Constants::CheckUpdate_Never:return tkTr(Constants::NEVER_AUTOMATICALLY);
    }
    return QString();
}

QStringList checkUpdateLabels()
{
    // Always keep this sync with the Trans::Constants::CheckUpdate enum
    return QStringList() << tkTr(Constants::AT_STARTUP)
                         << tkTr(Constants::EACH_WEEK)
                         << tkTr(Constants::EACH_MONTH)
                         << tkTr(Constants::EACH_QUARTER)
                         << tkTr(Constants::NEVER_AUTOMATICALLY);
}

/** \brief predetermined Periods stringlist */
QStringList periods()
{
    return  QStringList()
            << tkTr(Constants::SECOND_S)
            << tkTr(Constants::MINUTE_S)
            << tkTr(Constants::HOUR_S)
            << tkTr(Constants::DAY_S)
            << tkTr(Constants::WEEK_S)
            << tkTr(Constants::MONTH_S)
            << tkTr(Constants::QUARTER_S)
            << tkTr(Constants::YEAR_S);
}

/** \brief Returns the period corresponding to the enumerator Constants::Time::Period */
QString period(int id)
{
    switch (id)
    {
        case Constants::Time::Seconds : return tkTr(Constants::SECOND_S);
        case Constants::Time::Minutes : return tkTr(Constants::MINUTE_S);
        case Constants::Time::Hours :   return tkTr(Constants::HOUR_S);
        case Constants::Time::Days :    return tkTr(Constants::DAY_S);
        case Constants::Time::Weeks :   return tkTr(Constants::WEEK_S);
        case Constants::Time::Months :  return tkTr(Constants::MONTH_S);
        case Constants::Time::Quarter : return tkTr(Constants::QUARTER_S);
        case Constants::Time::Year :    return tkTr(Constants::YEAR_S);
        case Constants::Time::Decade :  return tkTr(Constants::DECADE_S);
    }
    return QString();
}

QString periodPlurialForm(int id, int nb, const QString &defaultValue)
{
    switch (id)
    {
        case Constants::Time::Seconds : if (nb > 1) return tkTr(Constants::SECONDS); else return tkTr(Constants::SECOND);
        case Constants::Time::Minutes : if (nb > 1) return tkTr(Constants::MINUTES); else return tkTr(Constants::MINUTE);
        case Constants::Time::Hours :   if (nb > 1) return tkTr(Constants::HOURS); else return tkTr(Constants::HOUR);
        case Constants::Time::Days :    if (nb > 1) return tkTr(Constants::DAYS); else return tkTr(Constants::DAY);
        case Constants::Time::Weeks :   if (nb > 1) return tkTr(Constants::WEEKS); else return tkTr(Constants::WEEK);
        case Constants::Time::Months :  if (nb > 1) return tkTr(Constants::MONTHS); else return tkTr(Constants::MONTH);
        case Constants::Time::Quarter : if (nb > 1) return tkTr(Constants::QUARTERS); else return tkTr(Constants::QUARTER);
        case Constants::Time::Year :    if (nb > 1) return tkTr(Constants::YEARS); else return tkTr(Constants::YEAR);
        case Constants::Time::Decade :  if (nb > 1) return tkTr(Constants::DECADES); else return tkTr(Constants::DECADE);
    }
    return defaultValue;
}

//int periodId(const QString &period)
//{
//    if (period.compare(tkTr(Constants::SECOND)==0) ||
//        period.compare(tkTr(Constants::SECONDS)==0) ||
//        period.compare(tkTr(Constants::SECOND_S)==0)) {
//        return Constants::Time::Seconds;
//    }

//    if (period.compare(tkTr(Constants::MINUTE)==0) ||
//        period.compare(tkTr(Constants::MINUTES)==0) ||
//        period.compare(tkTr(Constants::MINUTE_S)==0)) {
//        return Constants::Time::Minutes;
//    }

//    if (period.compare(tkTr(Constants::HOUR)==0) ||
//        period.compare(tkTr(Constants::HOURS)==0) ||
//        period.compare(tkTr(Constants::HOUR_S)==0)) {
//        return Constants::Time::Hours;
//    }

//    if (period.compare(tkTr(Constants::DAY)==0) ||
//        period.compare(tkTr(Constants::DAYS)==0) ||
//        period.compare(tkTr(Constants::DAY_S)==0)) {
//        return Constants::Time::Seconds;
//    }
//}

/** \brief Daily scheme predetermined list */
QStringList dailySchemeList()
{
    return QStringList()
            << tkTr(Constants::WAKEUPTIME)
            << tkTr(Constants::BREAKFAST)
            << tkTr(Constants::MORNING)
            << tkTr(Constants::TENOCLOCKBRUNCH)
            << tkTr(Constants::MIDDAY)
            << tkTr(Constants::LUNCH)
            << tkTr(Constants::TEATIME)
            << tkTr(Constants::EVENING)
            << tkTr(Constants::DINER)
            << tkTr(Constants::BEDTIME);
}

QStringList dailySchemeXmlTagList()
{
    static QStringList tags;
    if (tags.count())
        return tags;
    tags
            << QString(Constants::WAKEUPTIME).remove(" ")
            << QString(Constants::BREAKFAST).remove(" ")
            << QString(Constants::MORNING).remove(" ")
            << QString(Constants::TENOCLOCKBRUNCH).remove(" ").remove("'")
            << QString(Constants::MIDDAY).remove(" ")
            << QString(Constants::LUNCH).remove(" ")
            << QString(Constants::TEATIME).remove(" ")
            << QString(Constants::EVENING).remove(" ")
            << QString(Constants::DINER).remove(" ")
            << QString(Constants::BEDTIME).remove(" ")
            ;
    return tags;
}

///** \brief Transforms the dailyScheme enum Trans::Constants::Time::DailyScheme to the corresponding QStringList \e list */
//QStringList dailySchemes(const Constants::Time::DailyScheme scheme)
//{
//    if ( scheme == Constants::Time::Undefined )
//        return QStringList();
//    QStringList list;
//    if ( scheme & Constants::Time::Morning )
//        list << tkTr(Constants::MORNING);
//    if ( scheme & Constants::Time::Afternoon )
//        list << tkTr(Constants::AFTERNOON);
//    if ( scheme & Constants::Time::TeaTime )
//        list << tkTr(Constants::TEATIME);
//    if ( scheme & Constants::Time::Evening )
//        list << tkTr(Constants::EVENING);
//    if ( scheme & Constants::Time::BedTime )
//        list << tkTr(Constants::BEDTIME);
//    return list;
//}

/** \brief Transforms the dailyScheme QFlag DrugsDB::DosageModel::DailySchemes to the corresponding QStringList \e list */
QString dailyScheme(const int scheme)
{
    return dailySchemeList().at(scheme);
}

///** \brief Transforms the dailyScheme \e list to the corresponding QFlag DrugsDB::DosageModel::DailySchemes */
//Constants::Time::DailySchemes toDailyScheme( const QStringList & list )
//{
//    Constants::Time::DailySchemes r = Constants::Time::Undefined;
//    if ( list.indexOf( tkTr(Constants::MORNING) ) != -1 )
//        r |= Constants::Time::Morning;
//    if ( list.indexOf( tkTr(Constants::AFTERNOON) ) != -1 )
//        r |= Constants::Time::Afternoon;
//    if ( list.indexOf( tkTr(Constants::TEATIME) ) != -1 )
//        r |= Constants::Time::TeaTime;
//    if ( list.indexOf( tkTr(Constants::EVENING) ) != -1 )
//        r |= Constants::Time::Evening;
//    if ( list.indexOf( tkTr(Constants::BEDTIME) ) != -1 )
//        r |= Constants::Time::BedTime;
//    return r;
//}

/** \brief Returns pre-determined ages translated stringlist */
QStringList preDeterminedAges()
{
    return QStringList()
            << tkTr(Constants::DAYS)
            << tkTr(Constants::WEEKS)
            << tkTr(Constants::MONTHS)
            << tkTr(Constants::QUARTERS)
            << tkTr(Constants::YEARS)
            ;
}

/**
  \brief Returns the translated meal time stringlist.
  Because models record the index of the meal time and not the string, the order should not change.
  \sa Constants::Time::MealTime
*/
QStringList mealTime()
{
    return QStringList()
           << ""
           << tkTr(Constants::NOMEALRELATION)
           << tkTr(Constants::DURINGMEAL)
           << tkTr(Constants::BEFOREMEAL)
           << tkTr(Constants::AFTERMEAL)
           << tkTr(Constants::NOTDURINGMEAL)
           << tkTr(Constants::ONLYIFMEAL)
           << tkTr(Constants::SEPARATEDFROMFOOD)
           << tkTr(Constants::SUGGESTEDWITHFOOD)
           << tkTr(Constants::WITHORWITHOUTFOOD)
//           << tkTr(Constants::)
//           << tkTr(Constants::)
//           << tkTr(Constants::)
           ;
}

/**
  \brief Returns the translated meal time stringlist.
  Models record the index of the meal time and not the string.
  \sa Constants::Time::MealTime
*/
QString mealTime(const int index)
{
    switch (index) {
    case Constants::Time::Undefined : return QString();
    case Constants::Time::NoRelationWithMeal : return tkTr(Constants::NOMEALRELATION);
    case Constants::Time::DuringMeal: return tkTr(Constants::DURINGMEAL);
    case Constants::Time::BeforeMeal: return tkTr(Constants::BEFOREMEAL);
    case Constants::Time::AfterMeal: return tkTr(Constants::AFTERMEAL);
    case Constants::Time::OutsideMeal: return tkTr(Constants::NOTDURINGMEAL);
    case Constants::Time::OnlyIfMeal: return tkTr(Constants::NOTDURINGMEAL);
    case Constants::Time::SeparatedFromFood: return tkTr(Constants::SEPARATEDFROMFOOD);
    case Constants::Time::SuggestedWithFood: return tkTr(Constants::SUGGESTEDWITHFOOD);
    case Constants::Time::WithOrWithoutFood: return tkTr(Constants::WITHORWITHOUTFOOD);
    }
    return QString();
}

/** Return the list of available translated gender (male, female, other, unknown) */
QStringList genders()
{
    return QStringList()
            << tkTr(Trans::Constants::MALE)
            << tkTr(Trans::Constants::FEMALE)
            << tkTr(Trans::Constants::OTHER)
            << tkTr(Trans::Constants::UNKNOWN)
            ;
}

/** Return the list of available translated titles */
QStringList titles()
{
    return QStringList()
            << ""
            << tkTr(Trans::Constants::MISTER)
            << tkTr(Trans::Constants::MISS)
            << tkTr(Trans::Constants::MADAM)
            << tkTr(Trans::Constants::DOCTOR)
            << tkTr(Trans::Constants::PROFESSOR)
            << tkTr(Trans::Constants::CAPTAIN)
            ;
}

} // End ConstantTranslations
} // End Trans
