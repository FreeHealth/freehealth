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
#ifndef MFDOSAGE_H
#define MFDOSAGE_H

// include drugswidget headers
class mfDrugs;

// include Qt headers
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QDateTime>

/**
 * \file mfDosage.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.6
 * \date 13 March 2009
*/

/**
  \brief This class holds statics informations of the drugs dosages.
  Static datas are initialized with initStaticDatas() which is called by mfDrugsBase during mfDrugsBase::init().
  \todo put this into mfDosagesConstants ?
*/

class mfDosage : public QObject
{
    Q_OBJECT
public:

    //--------------------------------------------------------------------------------------------------------
    //------------------------------------------ Understanding datas -----------------------------------------
    //--------------------------------------------------------------------------------------------------------

    enum Physio    // QFlag it
    {
        EveryOne     = 0,
        Nourrissons  = 0x00000001,
        Children     = 0x00000010,
        AdultOnly    = 0x00000100,
        InsufRenal   = 0x00001000,
        InsufHepat   = 0x00010000,
        WeigtLimited = 0x00100000,
        ManOnly      = 0x01000000,
        WomanOnly    = 0x10000000
    };

    enum Pregnancy  // QFlag it
    {
        PregnancyOk         = 0x00000000,
        FirstQuaOk          = 0x00000010,
        SecondQuaOk         = 0x00000100,
        ThirdQuaOk          = 0x00001000,
        UsableWithWarn      = 0x00010000,
        NeverDuringPregnacy = 0x00100000
    };

    enum Period
    {
        Seconds      = 0,
        Minutes,
        Hours,
        Days,
        Weeks,
        Months,
        Quarter,
        Year
    };

    enum DailyScheme  // QFlag it
    {
        Morning      = 0x00000001,
        Afternoon    = 0x00000010,
        TeaTime      = 0x00000100,
        Evening      = 0x00001000,
        BedTime      = 0x00010000
    };

    enum MealTime
    {
        NoRelationWiyhMeal = 0,
        DuringMeal,
        BeforeMeal,
        AfterMeal,
        OutsideMeal
    };

    enum ScoredTablet
    {
        CompletTablet = 0,
        HalfTablet,
        QuaterTablet
    };

    enum PreDeterminedForms
    {
        DosePerKilo=0,
        SpoonReference,     // cuillère-mesure
        Spoon2_5ml,
        Spoon5ml,
        Puffs,              // bouffées
        Dose,
        Mouthwash,          // bain de bouche
        Inhalation,
        Application,
        Washing,            // lavement
        EyeWash,            // lavage oculaire
        Instillation
    };

    //--------------------------------------------------------------------------------------------------------
    //---------------------------------------- STATIC MEMBERS ------------------------------------------------
    //--------------------------------------------------------------------------------------------------------
    // static viewers to use for ui generation
    static void        initStaticDatas()      { retranslate() ; }
    static QStringList periods();
    static QString     period( int id );
    static int         periodDefault()        { return Days; }
    static QStringList scoredTabletScheme();
    static QStringList dailyScheme();
    static QStringList mealTime();
    static QString     mealTime( int id );
    static QStringList pregnancy();
    static QString     pregnancy( int id );
    // non static viewers for ui generation (need to be instanciate first)
    QStringList        forms();

private slots:
    void changeEvent( QEvent * event );
    static void retranslate();

    //--------------------------------------------------------------------------------------------------------
    //----------------------------------------- PRIVATE DATAS ------------------------------------------------
    //--------------------------------------------------------------------------------------------------------
private:
    static QStringList    m_Periods;
    static QStringList    m_DailyScheme;
    static QStringList    m_MealTime;
    static QStringList    m_Physiology;
    static QStringList    m_Pregnancy;
    static QStringList    m_BreastFeeding;
    static QStringList    m_ScoredTabletScheme;
    static QStringList    m_PreDeterminedForms;
    static QString        m_ActualLangage;
};

#endif // MFDOSAGE_H
