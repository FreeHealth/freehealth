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
 ***************************************************************************/

/**
  \namespace Trans::ConstantTranslations
  \brief This namespace regroups some usefull translations that you can easily use in your app ou plugs
  The tkTr() function helps you to translate its constants.\n
  There are a lot of enums defined here for data comprehension. You should them for your models/database...\n
*/


#include "constanttranslations.h"

#include <QLocale>

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

/** \brief predetermined Periods stringlist */
QStringList periods()
{
    return  QStringList()
            << tkTr(Constants::SECONDS)
            << tkTr(Constants::MINUTES)
            << tkTr(Constants::HOURS)
            << tkTr(Constants::DAYS)
            << tkTr(Constants::WEEKS)
            << tkTr(Constants::MONTHS)
            << tkTr(Constants::QUARTERS)
            << tkTr(Constants::YEARS);
}

/** \brief Returns the period corresponding to the enumerator Constants::Time::Period */
QString period( int id )
{
    switch (id)
    {
        case Constants::Time::Seconds : return tkTr(Constants::SECONDS);
        case Constants::Time::Minutes : return tkTr(Constants::MINUTES);
        case Constants::Time::Hours :   return tkTr(Constants::HOURS);
        case Constants::Time::Days :    return tkTr(Constants::DAYS);
        case Constants::Time::Weeks :   return tkTr(Constants::WEEKS);
        case Constants::Time::Months :  return tkTr(Constants::MONTHS);
        case Constants::Time::Quarter : return tkTr(Constants::QUARTERS);
        case Constants::Time::Year :    return tkTr(Constants::YEARS);
        case Constants::Time::Decade :  return tkTr(Constants::DECADES);
    }
    return QString();
}

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

/** \brief Transforms the dailyScheme QFlag mfDosageModel::DailySchemes to the corresponding QStringList \e list */
QString dailyScheme(const int scheme)
{
    return dailySchemeList().at(scheme);
}

///** \brief Transforms the dailyScheme \e list to the corresponding QFlag mfDosageModel::DailySchemes */
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


QStringList genders()
{
    return QStringList()
            << tkTr(Trans::Constants::MALE)
            << tkTr(Trans::Constants::FEMALE)
            << tkTr(Trans::Constants::HERMAPHRODISM)
            ;
}

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
