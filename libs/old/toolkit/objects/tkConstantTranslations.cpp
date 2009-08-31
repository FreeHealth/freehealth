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

/** \enum Physiology */
/** \enum PregnancyLimit */
/** \enum BreastFeeding */

/**
  \namespace tkConstantTranslations
  \brief This namespace regroups some usefull translations that you can easily use in your app ou plugs
  The tkTr() function helps you to translate its constants.\n
  There are a lot of enums defined here for data comprehension. You should them for your models/database...\n
*/


#include "tkConstantTranslations.h"

#include <QCoreApplication>
#include <QLocale>


Q_TK_BEGIN_TRANSLATIONS

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
    return QCoreApplication::translate(CONSTANTS_TR_CONTEXT, toTr);
}

/** \brief predetermined Periods stringlist */
QStringList periods()
{
    return  QStringList()
            << tkTr(SECONDS)
            << tkTr(MINUTES)
            << tkTr(HOURS)
            << tkTr(DAYS)
            << tkTr(WEEKS)
            << tkTr(MONTHS)
            << tkTr(QUARTERS)
            << tkTr(YEARS);
}

/** \brief Returns the period corresponding to the enumerator tkConstants::Time::Period */
QString period( int id )
{
    switch (id)
    {
        case Time::Seconds : return tkTr(SECONDS);
        case Time::Minutes : return tkTr(MINUTES);
        case Time::Hours :   return tkTr(HOURS);
        case Time::Days :    return tkTr(DAYS);
        case Time::Weeks :   return tkTr(WEEKS);
        case Time::Months :  return tkTr(MONTHS);
        case Time::Quarter : return tkTr(QUARTERS);
        case Time::Year :    return tkTr(YEARS);
        case Time::Decade :  return tkTr(DECADES);
    }
    return QString();
}

/** \brief Daily scheme predetermined list */
QStringList dailySchemeList()
{
    return QStringList()
            << tkTr(MORNING)
            << tkTr(AFTERNOON)
            << tkTr(TEATIME)
            << tkTr(EVENING)
            << tkTr(BEDTIME);
}

/** \brief Transforms the dailyScheme QFlag mfDosageModel::DailySchemes to the corresponding QStringList \e list */
QStringList dailySchemes( const tkConstants::Time::DailySchemes scheme )
{
    if ( scheme == tkConstants::Time::Undefined )
        return QStringList();
    QStringList list;
    if ( scheme & tkConstants::Time::Morning )
        list << tkTr(MORNING);
    if ( scheme & tkConstants::Time::Afternoon )
        list << tkTr(AFTERNOON);
    if ( scheme & tkConstants::Time::TeaTime )
        list << tkTr(TEATIME);
    if ( scheme & tkConstants::Time::Evening )
        list << tkTr(EVENING);
    if ( scheme & tkConstants::Time::BedTime )
        list << tkTr(BEDTIME);
    return list;
}

/** \brief Transforms the dailyScheme \e list to the corresponding QFlag mfDosageModel::DailySchemes */
tkConstants::Time::DailySchemes toDailyScheme( const QStringList & list )
{
    tkConstants::Time::DailySchemes r = tkConstants::Time::Undefined;
    if ( list.indexOf( tkTr(MORNING) ) != -1 )
        r |= tkConstants::Time::Morning;
    if ( list.indexOf( tkTr(AFTERNOON) ) != -1 )
        r |= tkConstants::Time::Afternoon;
    if ( list.indexOf( tkTr(TEATIME) ) != -1 )
        r |= tkConstants::Time::TeaTime;
    if ( list.indexOf( tkTr(EVENING) ) != -1 )
        r |= tkConstants::Time::Evening;
    if ( list.indexOf( tkTr(BEDTIME) ) != -1 )
        r |= tkConstants::Time::BedTime;
    return r;
}

/** \brief Returns pre-determined ages translated stringlist */
QStringList preDeterminedAges()
{
    return QStringList()
            << tkTr(DAYS)
            << tkTr(WEEKS)
            << tkTr(MONTHS)
            << tkTr(QUARTERS)
            << tkTr(YEARS)
            ;
}

/**
  \brief Returns the translated meal time stringlist.
  Because models record the index of the meal time and not the string, the order should not change.
  \sa tkConstants::Time::MealTime
*/
QStringList mealTime()
{
    return QStringList()
           << tkTr(NOMEALRELATION)
           << tkTr(DURINGMEAL)
           << tkTr(BEFOREMEAL)
           << tkTr(AFTERMEAL)
           << tkTr(NOTDURINGMEAL);
}

/**
  \brief Returns the translated meal time stringlist.
  Models record the index of the meal time and not the string.
  \sa tkConstants::Time::MealTime
*/
QString mealTime(const int index)
{
    switch (index) {
        case Time::NoRelationWiyhMeal : return tkTr(NOMEALRELATION);
         case Time::DuringMeal: return tkTr(DURINGMEAL);
         case Time::BeforeMeal: return tkTr(BEFOREMEAL);
         case Time::AfterMeal: return tkTr(AFTERMEAL);
         case Time::OutsideMeal: return tkTr(NOTDURINGMEAL);
    }
    return QString();
}


//    m_Pregnancy =
//            QStringList()
//            << tr( "usable during whole pregnancy" )
//            << tr( "usable during the first quarter of pregnancy" )
//            << tr( "usable during the second quarter of pregnancy" )
//            << tr( "usable during the third quarter of pregnancy" )
//            << tr( "usable during pregnancy with warnings" )
//            << tr( "not usable during pregnancy" );

Q_TK_END_TRANSLATIONS


