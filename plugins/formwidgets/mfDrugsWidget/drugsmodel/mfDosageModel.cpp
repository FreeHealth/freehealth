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
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "mfDosageModel.h"

// including plugins headers
#include <drugsmodel/mfDrugs.h>
#include <drugsmodel/mfDosage.h>
#include <drugsmodel/mfDrugsModel.h>

// including toolkit headers
#include <tkGlobal.h>
#include <tkLog.h>

// including usertoolkit headers (for FreeMedForms only)
#ifndef DRUGS_INTERACTIONS_STANDALONE
#   include <tkUserModel.h>
#   include <tkUserConstants.h>
    using namespace tkUserConstants;
#endif

// including Qt headers
#include <QSqlRecord>
#include <QSqlError>
#include <QApplication>
#include <QStringList>
#include <QUuid>
#include <QDateTime>
#include <QColor>
#include <QLocale>

//mfDosageModel * mfDosageModel::m_Instance = 0;
//mfDosageModel * mfDosageModel::instance()
//{
//    if ( ! m_Instance )
//        m_Instance = new mfDosageModel( qApp );
//    return m_Instance;
//}


//--------------------------------------------------------------------------------------------------------
//---------------------------------------------- Static Datas --------------------------------------------
//--------------------------------------------------------------------------------------------------------
// intialize static private members
QStringList mfDosageModel::m_Periods = QStringList();
QStringList mfDosageModel::m_DailyScheme = QStringList();
QStringList mfDosageModel::m_MealTime = QStringList();
QStringList mfDosageModel::m_Physiology = QStringList();
QStringList mfDosageModel::m_Pregnancy = QStringList();
QStringList mfDosageModel::m_ScoredTabletScheme = QStringList();
QStringList mfDosageModel::m_PreDeterminedForms = QStringList();
QString     mfDosageModel::m_ActualLangage = "";

//--------------------------------------------------------------------------------------------------------
//----------------------------------------- Managing Translations ----------------------------------------
//--------------------------------------------------------------------------------------------------------
void mfDosageModel::changeEvent( QEvent * event )
{
    // proceed only LangageChange events
    if ( event->type() != QEvent::LanguageChange )
        return;
    retranslate();
}

void mfDosageModel::retranslate()
{
    // proceed only if needed
    if ( m_ActualLangage == QLocale().name().left( 2 ) )
        return;

    // store the langage and retranslate
    m_ActualLangage = QLocale().name().left( 2 );

    // translate strings
    m_Periods =
            QStringList()
            << tr( "seconds" )
            << tr( "minutes" )
            << tr( "hours" )
            << tr( "days" )
            << tr( "weeks" )
            << tr( "months" )
            << tr( "quarters" ) // quarters = trimestres
            << tr( "years" );

    m_DailyScheme =
            QStringList()
            << tr( "morning" )
            << tr( "afternoon" )
            << tr( "tea time" )
            << tr( "evening" )
            << tr( "bed time" );

    m_MealTime =
            QStringList()
            << tr( "no relation with meal" )
            << tr( "not during meal" )
            << tr( "during meal" )
            << tr( "before meal" )
            << tr( "after meal" );

    m_Physiology =
            QStringList()
            << tr( "no physiologic limit" )
            << tr( "infants" ) // nourrissons
            << tr( "children" )
            << tr( "adult only" )
            << tr( "no chronic renal failure" )
            << tr( "no chronic heaptic failure" )
            << tr( "weight limited" )
            << tr( "only for man" )
            << tr( "only for woman" );

    m_ScoredTabletScheme =
            QStringList()
            << tr( "complet tab." )
            << tr( "half tab." )
            << tr( "quater tab." );

    m_PreDeterminedForms =
            QStringList()
            << tr( "dose per kilograms" )
            << tr( "reference spoon" )
            << tr( "2.5 ml spoon" )
            << tr( "5 ml spoon" )
            << tr( "puffs" )
            << tr( "dose" )
            << tr( "mouthwash" )
            << tr( "inhalation" )
            << tr( "application" )
            << tr( "washing" )
            << tr( "eyewash" )
            << tr( "instillation" )
            << tr( "pulverization" );
//            << tr("");

    m_Pregnancy =
            QStringList()
            << tr( "usable during whole pregnancy" )
            << tr( "usable during the first quarter of pregnancy" )
            << tr( "usable during the second quarter of pregnancy" )
            << tr( "usable during the third quarter of pregnancy" )
            << tr( "usable during pregnancy with warnings" )
            << tr( "not usable during pregnancy" );

//    m_BreastFeeding =
//            QStringList() <<
//            << tr( "" )
//            << tr( "" )
//            << tr( "" )
//            << tr( "" )
//            << tr( "" )
//            << tr( "" )
//            << tr( "" );
}


//--------------------------------------------------------------------------------------------------------
//----------------------------------------- Managing Static datas ----------------------------------------
//--------------------------------------------------------------------------------------------------------
QStringList mfDosageModel::periods()
{
    if ( m_Periods.count() == 0 )
        retranslate();
    return m_Periods;
}

QString mfDosageModel::period( int id )
{
    if ( m_Periods.count() == 0 )
        retranslate();

    if ( ( id > 0 ) && ( id < m_Periods.count() ) )
        return m_Periods[id];
    return QString::null;
}

QStringList mfDosageModel::scoredTabletScheme()
{
    if ( m_ScoredTabletScheme.count() == 0 )
        retranslate();
    return m_ScoredTabletScheme;
}

QStringList mfDosageModel::dailyScheme()
{
    if ( m_DailyScheme.count() == 0 )
        retranslate();
    // TODO : undefined should not appear in this list...
    return m_DailyScheme;
}

QStringList mfDosageModel::dailySchemes( const DailySchemes scheme )
{
    if ( scheme == Undefined )
        return QStringList();
    QStringList list;
    if ( scheme & Morning )
        list << m_DailyScheme.at(0);
    if ( scheme & Afternoon )
        list << m_DailyScheme.at(1);
    if ( scheme & TeaTime )
        list << m_DailyScheme.at(2);
    if ( scheme & Evening )
        list << m_DailyScheme.at(3);
    if ( scheme & BedTime )
        list << m_DailyScheme.at(4);

//    qWarning() << "dailySchemes" << scheme << list;

    return list;
}

mfDosageModel::DailySchemes mfDosageModel::toDailyScheme( const QStringList & list )
{
    DailySchemes r = Undefined;
    if ( list.indexOf( m_DailyScheme.at(0) ) != -1 )
        r |= Morning;
    if ( list.indexOf( m_DailyScheme.at(1) ) != -1 )
        r |= Afternoon;
    if ( list.indexOf( m_DailyScheme.at(2) ) != -1 )
        r |= TeaTime;
    if ( list.indexOf( m_DailyScheme.at(3) ) != -1 )
        r |= Evening;
    if ( list.indexOf( m_DailyScheme.at(4) ) != -1 )
        r |= BedTime;
    return r;
}

QStringList mfDosageModel::mealTime()
{
    if ( m_MealTime.count() == 0 )
        retranslate();
    return m_MealTime;
}

QString mfDosageModel::mealTime( int id )
{
    if ( m_MealTime.count() == 0 )
        retranslate();
    if ( ( id > 0 ) && ( id < m_MealTime.count() ) )
        return m_MealTime[id];
    return QString::null;
}

QStringList mfDosageModel::pregnancy()
{
    if ( m_Pregnancy.count() == 0 )
        retranslate();
    return m_Pregnancy;
}

QString mfDosageModel::pregnancy( int id )
{
    if ( m_Pregnancy.count() == 0 )
        retranslate();
    if ( ( id > 0 ) && ( id < m_Pregnancy.count() ) )
        return m_Pregnancy[id];
    return QString::null;
}

QStringList mfDosageModel::predeterminedForms()
{
    if (m_PreDeterminedForms.count()==0)
        retranslate();
    return m_PreDeterminedForms;
}

//--------------------------------------------------------------------------------------------------------
//------------------------------------------ Dosage Model ------------------------------------------------
//--------------------------------------------------------------------------------------------------------
mfDosageModel::mfDosageModel( QObject *parent )
    : QSqlTableModel( parent, QSqlDatabase::database( DOSAGES_DATABASE_NAME ) )
{
    setObjectName( "mfDosageModel" );
    QSqlTableModel::setTable( DOSAGES_TABLE_NAME );
    setEditStrategy( QSqlTableModel::OnManualSubmit );
    m_CIS = -1;
}

bool mfDosageModel::setData( const QModelIndex & index, const QVariant & value, int role )
{
    Q_ASSERT_X( m_CIS != -1, "mfDosageModel::setData", "before using the dosagemodel, you must specify the CIS of the related drug");
    if ( ! index.isValid() )
        return false;
    if ( role == Qt::EditRole ) {

        // set only once modification date (infinite loop prevention)
        if ( index.column() != Dosage::ModificationDate )
            setData( this->index( index.row(), Dosage::ModificationDate ), QDateTime::currentDateTime() );

        // mark row as dirty
        if ( ! m_DirtyRows.contains( index.row() ) ) {
            m_DirtyRows << index.row();
            QModelIndex label = this->index( index.row(), Dosage::Label );
            emit dataChanged( label, label );
        }
        // set data to model
        switch (index.column())
        {
        case Dosage::DailyScheme : // --> receive stringList, transform to QFlag
            {
                return QSqlTableModel::setData( index, int(toDailyScheme( value.toStringList() )) , role);
                break;
            }
        default : return QSqlTableModel::setData( index, value, role );
        }
    }
    return false;
}

QVariant mfDosageModel::data( const QModelIndex & item, int role ) const
{
    Q_ASSERT_X( m_CIS != -1, "mfDosageModel::data", "before using the dosagemodel, you must specify the CIS of the related drug");
    if ( ! item.isValid() )
        return QVariant();

    if ( ( role == Qt::DisplayRole ) || ( role == Qt::EditRole ) ) {
        switch (item.column())
        {
        case Dosage::Label :
            {
                if ( m_DirtyRows.contains(item.row())  )
                    return QString("[*] - %1").arg( QSqlTableModel::data( item, role ).toString() );
                break;
            }
         case Dosage::DailyScheme : // --> is a QFlags to transform to stringList
            {
                return dailySchemes( DailySchemes( QSqlTableModel::data(item).toInt() ) );
                break;
            }
        }
    } else if ( role == Qt::BackgroundRole ) {
        if ( m_DirtyRows.contains(item.row())  )
            return QColor(240,240,240);
    }

    return QSqlTableModel::data( item, role );
}

bool mfDosageModel::insertRows( int row, int count, const QModelIndex & parent )
{
    Q_ASSERT_X( m_CIS != -1, "mfDosageModel::insertRows", "before inserting row, you must specify the CIS of the related drug");
    QString userUuid;
#ifdef DRUGS_INTERACTIONS_STANDALONE
    userUuid = DOSAGES_DEFAULT_USER_UUID;
#else
    userUuid = tkUserModel::instance()->currentUserData(User::UserUuid).toString();
#endif
    int i;
    int createdRow;
    bool toReturn = true;
    for (i=0; i < count; ++i) {
        createdRow = row+i;
        if ( !QSqlTableModel::insertRows( createdRow, 1, parent ) ) {
            tkLog::addError( this, tr("Model Error : unable to insert a row") );
            toReturn = false;
        } else {
            setData( index( createdRow, Dosage::Uuid ) , QUuid::createUuid().toString() );
            setData( index( createdRow, Dosage::CIS_LK ) , m_CIS );
            setData( index( createdRow, Dosage::CreationDate ) , QDateTime::currentDateTime() );
            setData( index( createdRow, Dosage::UserUuid ) , userUuid );
        }
    }
    return toReturn;
}

bool mfDosageModel::removeRows( int row, int count, const QModelIndex & parent )
{
    Q_ASSERT_X( m_CIS != -1, "mfDosageModel::removeRows", "before using the dosagemodel, you must specify the CIS of the related drug");
    // TODO when QSqlTableModel removes rows it may call select() and reset(),
    //      this causes non submitted modifications deletion on the model
    //      THIS MUST BE IMPROVED
    setEditStrategy( QSqlTableModel::OnRowChange );
    bool toReturn = false;
    if ( QSqlTableModel::removeRows( row, count, parent ) ) {
        int i;
        for(i=0; i < count; ++i) {
            if (m_DirtyRows.contains(row+i))
                m_DirtyRows.remove(row+i);
        }
        toReturn = true;
    }
    setEditStrategy( QSqlTableModel::OnManualSubmit );
    return toReturn;
}

void mfDosageModel::revertRow( int row )
{
   Q_ASSERT_X( m_CIS != -1, "mfDosageModel::revertRow", "before using the dosagemodel, you must specify the CIS of the related drug");
   QSqlTableModel::revertRow( row );
    if (m_DirtyRows.contains(row))
        m_DirtyRows.remove( row );
}

bool mfDosageModel::submitAll()
{
    Q_ASSERT_X( m_CIS != -1, "mfDosageModel::submitAll", "before using the dosagemodel, you must specify the CIS of the related drug");
    QSet<int> safe = m_DirtyRows;
    m_DirtyRows.clear();
    if ( QSqlTableModel::submitAll() ) {
        return true;
    } else
        m_DirtyRows = safe;
//    reset();
    return false;
}

bool mfDosageModel::setDrugCIS( const int _CIS )
{
    if ( _CIS == m_CIS )
        return true;
    m_CIS = _CIS;
    setFilter( QString( "%1=%2" ).arg( record().fieldName( Dosage::CIS_LK ) ).arg( m_CIS ) );
    select();
    return true;
}

bool mfDosageModel::isDosageValid( const int row )
{
    Q_ASSERT_X( m_CIS != -1, "mfDosageModel::isDosageValid", "before using the dosagemodel, you must specify the CIS of the related drug");
    //TODO
    return true;
}


#ifndef DRUGS_INTERACTIONS_STANDALONE
bool mfDosageModel::userCanRead()
{
    //TODO --> test
    User::UserRights r = User::UserRights(tkUserModel::instance()->currentUserData( User::UserDrugsRights ).toInt());
    return (r & User::ReadOwn) || (r & User::ReadAll);
}

bool mfDosageModel::userCanWrite()
{
    //TODO  --> test
    User::UserRights r = User::UserRights(tkUserModel::instance()->currentUserData( User::UserDrugsRights ).toInt());
    return (r & User::WriteOwn) || (r & User::WriteAll);
}
#endif

void mfDosageModel::warn( const int row )
{
    if (!tkGlobal::isDebugCompilation())
        return;
    // row == -1 -> warn all rows
    int i;
    int j;
    if ( row == -1 ) {
        for (i=0; i < rowCount(); ++i )
        for( j=0; j < columnCount(); ++j ) {
            qWarning() << record().fieldName(j) << index( i, j ).data() ;
        }
    } else {
        for( j=0; j < columnCount(); ++j ) {
            qWarning() << record().fieldName(j) << index( row, j ).data() ;
        }
    }
}
