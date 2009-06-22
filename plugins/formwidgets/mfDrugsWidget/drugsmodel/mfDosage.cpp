/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
 *   eric.maeker@free.fr                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "mfDosage.h"

// include drugswidget headers
#include <drugsdatabase/mfDrugsBase.h>
#include <drugsmodel/mfDrugs.h>

// include toolkit headers
#include <tkLog.h>

// include Qt headers
#include <QLocale>
#include <QEvent>

//--------------------------------------------------------------------------------------------------------
//---------------------------------------------- Static Datas --------------------------------------------
//--------------------------------------------------------------------------------------------------------
// intialize static private members
QStringList mfDosage::m_Periods = QStringList();
QStringList mfDosage::m_DailyScheme = QStringList();
QStringList mfDosage::m_MealTime = QStringList();
QStringList mfDosage::m_Physiology = QStringList();
QStringList mfDosage::m_Pregnancy = QStringList();
QStringList mfDosage::m_ScoredTabletScheme = QStringList();
QStringList mfDosage::m_PreDeterminedForms = QStringList();
QString     mfDosage::m_ActualLangage = "";

//--------------------------------------------------------------------------------------------------------
//----------------------------------------- Managing Translations ----------------------------------------
//--------------------------------------------------------------------------------------------------------
void mfDosage::changeEvent( QEvent * event )
{
    // proceed only LangageChange events
    if ( event->type() != QEvent::LanguageChange )
        return;
    retranslate();
}

void mfDosage::retranslate()
{
    // proceed only if needed
    if ( m_ActualLangage == QLocale().name().left( 2 ) )
        return;

    // store the langage and retranslate
    m_ActualLangage = QLocale().name().left( 2 );

    // translate strings
    m_Periods =
            QStringList()
            << tr( "Seconds" )
            << tr( "Minutes" )
            << tr( "Hours" )
            << tr( "Days" )
            << tr( "Weeks" )
            << tr( "Months" )
            << tr( "Quarters" ) // quarters = trimestres
            << tr( "Years" );

    m_DailyScheme =
            QStringList()
            << tr( "Morning" )
            << tr( "Afternoon" )
            << tr( "Tea Time" )
            << tr( "Evening" )
            << tr( "Bed Time" );

    m_MealTime =
            QStringList()
            << tr( "No relation with meal" )
            << tr( "Not during meal" )
            << tr( "During meal" )
            << tr( "Before meal" )
            << tr( "After meal" );

    m_Physiology =
            QStringList()
            << tr( "No physiologic limit" )
            << tr( "Infants" ) // nourrissons
            << tr( "Children" )
            << tr( "Adult only" )
            << tr( "No chronic renal failure" )
            << tr( "No chronic heaptic failure" )
            << tr( "Weight limited" )
            << tr( "Only for man" )
            << tr( "Only for woman" );

    m_ScoredTabletScheme =
            QStringList()
            << tr( "Complet Tab." )
            << tr( "Half Tab." )
            << tr( "Quater Tab." );

    m_PreDeterminedForms =
            QStringList()
            << tr( "Dose per kilograms" )
            << tr( "Reference spoon" )
            << tr( "2.5 ml spoon" )
            << tr( "5 ml spoon" )
            << tr( "" )
            << tr( "" )
            << tr( "" );

    m_Pregnancy =
            QStringList()
            << tr( "Usable during whole pregnancy" )
            << tr( "Usable during the first quarter of pregnancy" )
            << tr( "Usable during the second quarter of pregnancy" )
            << tr( "Usable during the third quarter of pregnancy" )
            << tr( "Usable during pregnancy with warnings" )
            << tr( "Not usable during pregnancy" );

    /*
    m_BreastFeeding =
            QStringList() <<
            << tr( "" )
            << tr( "" )
            << tr( "" )
            << tr( "" )
            << tr( "" )
            << tr( "" )
            << tr( "" );
    */
}


//--------------------------------------------------------------------------------------------------------
//----------------------------------------- Managing Static datas ----------------------------------------
//--------------------------------------------------------------------------------------------------------
QStringList mfDosage::periods()
{
    if ( m_Periods.count() == 0 )
        retranslate();
    return m_Periods;
}


QString mfDosage::period( int id )
{
    if ( m_Periods.count() == 0 )
        retranslate();

    if ( ( id > 0 ) && ( id < m_Periods.count() ) )
        return m_Periods[id];
    return QString::null;
}

QStringList mfDosage::scoredTabletScheme()
{
    if ( m_ScoredTabletScheme.count() == 0 )
        retranslate();
    return m_ScoredTabletScheme;
}

QStringList mfDosage::dailyScheme()
{
    if ( m_DailyScheme.count() == 0 )
        retranslate();
    // TODO : undefined should not appear in this list...
    return m_DailyScheme;
}


QStringList mfDosage::mealTime()
{
    if ( m_MealTime.count() == 0 )
        retranslate();
    return m_MealTime;
}

QString mfDosage::mealTime( int id )
{
    if ( m_MealTime.count() == 0 )
        retranslate();
    if ( ( id > 0 ) && ( id < m_MealTime.count() ) )
        return m_MealTime[id];
    return QString::null;
}

QStringList mfDosage::pregnancy()
{
    if ( m_Pregnancy.count() == 0 )
        retranslate();
    return m_Pregnancy;
}

QString mfDosage::pregnancy( int id )
{
    if ( m_Pregnancy.count() == 0 )
        retranslate();
    if ( ( id > 0 ) && ( id < m_Pregnancy.count() ) )
        return m_Pregnancy[id];
    return QString::null;
}
