/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
 *   eric.maeker@gmail.com                                                   *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/

/**
  \class mfDosageModel
  \brief Manages predetermined dosage.
  A \b "dosage" is a set of values that defines what, when and how to prescribe a drug. A dosage can apply :
  \li on a specific drug base on its CIS,
  \li on the INN molecule of the drug which are presented in the same "dosage" (100mg, 1g...).
  For example :
  \li CLAMOXYL 1g can have prescription based on its CIS or on the \e amoxicilline molecule.
  \li CIS based could be : one intake morning and evening during seven days.
  \li INN based could be the same and can be applied to AMOXICILLINE PHARMACEUTICAL_LABO 1g.

  A \b "prescription" is the when and how you prescribe a selected drug.\n
  database( DOSAGES_DATABASE_NAME ) should be defined BEFORE instance()

  \todo Create a specific user's right for dosage creation/edition/modification/deletion +++.

  \ingroup freediams drugswidget
*/


#include "mfDosageModel.h"

// including plugins headers
#include <drugsmodel/mfDrugs.h>
#include <drugsmodel/mfDrugsModel.h>
#include <mfDrugsManager.h>

// including toolkit headers
#include <tkGlobal.h>
#include <tkLog.h>
#include <tkTheme.h>
#include <tkConstantTranslations.h>

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

namespace mfDosageModelConstants {
    const char *const XML_DOSAGE_MAINTAG            = "DOSAGE";
    const char* const DIRTYROW_BACKGROUNDCOLOR      = "yellow";
}

Q_TK_USING_CONSTANTS
Q_TK_USING_TRANSLATIONS
using namespace mfDosageModelConstants;

//--------------------------------------------------------------------------------------------------------
//---------------------------------------------- Static Datas --------------------------------------------
//--------------------------------------------------------------------------------------------------------
// intialize static private members
QStringList mfDosageModel::m_ScoredTabletScheme = QStringList();
QStringList mfDosageModel::m_PreDeterminedForms = QStringList();
QString     mfDosageModel::m_ActualLangage = "";

//--------------------------------------------------------------------------------------------------------
//----------------------------------------- Managing Translations ----------------------------------------
//--------------------------------------------------------------------------------------------------------
/** \brief Used to retranslate static stringlists */
void mfDosageModel::changeEvent( QEvent * event )
{
    // proceed only LangageChange events
    if ( event->type() != QEvent::LanguageChange )
        return;
    retranslate();
}

/** \brief Used to retranslate static stringlists */
void mfDosageModel::retranslate()
{
    // proceed only if needed
    if ( m_ActualLangage == QLocale().name().left(2) )
        return;

    // store the langage and retranslate
    m_ActualLangage = QLocale().name().left(2);

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
            << tr( "shampooing" )
            << tr( "eyewash" )
            << tr( "instillation" )
            << tr( "pulverization" );


}


//--------------------------------------------------------------------------------------------------------
//----------------------------------------- Managing Static datas ----------------------------------------
//--------------------------------------------------------------------------------------------------------

/** \brief Scored tablet predetermined stringlist */
QStringList mfDosageModel::scoredTabletScheme()
{
    if ( m_ScoredTabletScheme.count() == 0 )
        retranslate();
    return m_ScoredTabletScheme;
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
/** \brief Constructor */
mfDosageModel::mfDosageModel( QObject *parent )
        : QSqlTableModel( parent, QSqlDatabase::database( DOSAGES_DATABASE_NAME ) )
{
    setObjectName( "mfDosageModel" );
    QSqlTableModel::setTable( DOSAGES_TABLE_NAME );
    setEditStrategy( QSqlTableModel::OnManualSubmit );
    m_CIS = -1;
}

/** \brief Defines datas for the dosage. Database is only updated when calling submitAll(). */
bool mfDosageModel::setData( const QModelIndex & index, const QVariant & value, int role )
{
    Q_ASSERT_X( m_CIS != -1, "mfDosageModel::setData", "before using the dosagemodel, you must specify the CIS of the related drug");
    if ( ! index.isValid() )
        return false;
    if ( role == Qt::EditRole ) {
        // verify the value is different as model
        QVariant q = data(index,Qt::DisplayRole);
        if (q==value)
            return false;

        // set only once modification date (infinite loop prevention)
        if (index.column()!=Dosage::ModificationDate)
            setData( this->index( index.row(), Dosage::ModificationDate ), QDateTime::currentDateTime() );

        // mark row as dirty
        if (!m_DirtyRows.contains(index.row())) {
            m_DirtyRows << index.row();
        }

        switch (index.column())
        {
        case Dosage::DailyScheme : // --> receive stringList, transform to QFlag
            {
                if (!QSqlTableModel::setData( index, int(toDailyScheme(value.toStringList())), role))
                    return false;
                break;
            }
        default :
        {
            if (!QSqlTableModel::setData( index, value, role ))
                return false;
            break;
        }
    }
//        qWarning() <<"data set"<<record().fieldName(index.column())<<value;
        QModelIndex label = this->index( index.row(), Dosage::Label );
        emit dataChanged( label, label );
    }
    return false;
}

/** \brief Retreive dosage's datas. */
QVariant mfDosageModel::data( const QModelIndex & item, int role ) const
{
    Q_ASSERT_X( m_CIS != -1, "mfDosageModel::data", "before using the dosagemodel, you must specify the CIS of the related drug");
    if ( ! item.isValid() )
        return QVariant();

    switch (role)
    {
    case Qt::FontRole:
        {
            QFont font;
            if (m_DirtyRows.contains(item.row()))
                font.setBold(true);
            else
                font.setBold(false);
            return font;
        }
    case Qt::BackgroundRole :
        {
            if (m_DirtyRows.contains(item.row()))
                return QColor(DIRTYROW_BACKGROUNDCOLOR);
            else
                return QColor("white");
        }
    case Qt::DisplayRole:
    case Qt::EditRole:
        {
            switch (item.column())
            {
            case Dosage::DailyScheme : // --> is a QFlags to transform to stringList
                {
                    return dailySchemes( tkConstants::Time::DailySchemes( QSqlTableModel::data(item).toInt() ) );
                    break;
                }
            default :
                    //                qWarning() << record().fieldName(item.column())<<QSqlTableModel::data(item, role);
                    return QSqlTableModel::data(item, role);
            }
            break;
        }
    case Qt::DecorationRole :
        {
//            qWarning() << QSqlTableModel::index(item.row(), Dosage::INN_LK).data().toString();
            if (!QSqlTableModel::index(item.row(), Dosage::INN_LK).data().toString().isEmpty())
                return tkTheme::icon(MFDRUGS_ICONSEARCHINN);
            return tkTheme::icon(MFDRUGS_ICONSEARCHCOMMERCIAL);
        }
    }
    return QVariant();
}

/** \brief Create new dosages. Defaults values of the dosages are stted here. */
bool mfDosageModel::insertRows( int row, int count, const QModelIndex & parent )
{
    Q_ASSERT_X( m_CIS != -1, "mfDosageModel::insertRows", "before inserting row, you must specify the CIS of the related drug");
    QString userUuid;
#ifdef DRUGS_INTERACTIONS_STANDALONE
    userUuid = DOSAGES_DEFAULT_USER_UUID;
#else
    userUuid = tkUserModel::instance()->currentUserData(User::Uuid).toString();
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
            setData( index( createdRow, Dosage::IntakesTo ) , 1 );
            setData( index( createdRow, Dosage::IntakesFrom ) , 1 );
            setData( index( createdRow, Dosage::IntakesUsesFromTo ) , false );
            setData( index( createdRow, Dosage::IntakesScheme ) , tkTr(INTAKES, 1) );
            setData( index( createdRow, Dosage::Period ), 1 );
            setData( index( createdRow, Dosage::PeriodScheme ) , tkTr(DAYS) );
            setData( index( createdRow, Dosage::DurationTo ) , 1 );
            setData( index( createdRow, Dosage::DurationFrom ) , 1 );
            setData( index( createdRow, Dosage::DurationUsesFromTo ) , false );
            setData( index( createdRow, Dosage::DurationScheme ) , tkTr(MONTHS) );
            setData( index( createdRow, Dosage::IntakesIntervalOfTime ) , 0 );
            setData( index( createdRow, Dosage::MinAge ) , 0 );
            setData( index( createdRow, Dosage::MaxAge ) , 0 );
            setData( index( createdRow, Dosage::MinWeight ) , 0 );
            setData( index( createdRow, Dosage::MinClearance ) , 0 );
            setData( index( createdRow, Dosage::MaxClearance ) , 0 );
            setData( index( createdRow, Dosage::SexLimitedIndex ) , 0 );
            setData( index( createdRow, Dosage::Note ) , QVariant() );
            setData( index( createdRow, Dosage::CreationDate ) , QDateTime::currentDateTime() );
            setData( index( createdRow, Dosage::Transmitted ) , QVariant() );
        }
    }
    return toReturn;
}

/**
  \brief Removes somes rows.
  \todo  when QSqlTableModel removes rows it may call select() and reset(),  this causes non submitted modifications deletion on the model THIS MUST BE IMPROVED
*/
bool mfDosageModel::removeRows( int row, int count, const QModelIndex & parent )
{
    Q_ASSERT_X( m_CIS != -1, "mfDosageModel::removeRows", "before using the dosagemodel, you must specify the CIS of the related drug");
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

/** \brief Revert a row */
void mfDosageModel::revertRow( int row )
{
    Q_ASSERT_X( m_CIS != -1, "mfDosageModel::revertRow", "before using the dosagemodel, you must specify the CIS of the related drug");
    QSqlTableModel::revertRow( row );
    if (m_DirtyRows.contains(row))
        m_DirtyRows.remove( row );
}

/** \brief Submit model changes to database */
bool mfDosageModel::submitAll()
{
    Q_ASSERT_X( m_CIS != -1, "mfDosageModel::submitAll", "before using the dosagemodel, you must specify the CIS of the related drug");
//    warn();
    QSet<int> safe = m_DirtyRows;
    m_DirtyRows.clear();
    if ( QSqlTableModel::submitAll() ) {
        return true;
    } else
        m_DirtyRows = safe;
    //    reset();
    return false;
}

/**
  \brief Filter the model from the drug CIS and if possible for the inn prescriptions.
*/
bool mfDosageModel::setDrugCIS( const int _CIS )
{
    if ( _CIS == m_CIS )
        return true;
    m_CIS = _CIS;
    QString filter = QString("%1=%2").arg(record().fieldName(Dosage::CIS_LK)).arg(m_CIS);
    int inn = DRUGMODEL->drugData(_CIS,Drug::MainInnCode).toInt();
    if (inn!=-1) {
        // add INN_LK
        QString innFilter = QString::number(inn);
        innFilter = QString("%1=%2").arg(record().fieldName(Dosage::INN_LK)).arg(innFilter);
        // add INN_DOSAGE
        innFilter = QString("(%1) AND (%2='%3')")
                    .arg(innFilter)
                    .arg(record().fieldName(Dosage::InnLinkedDosage))
                    .arg(DRUGMODEL->drugData(_CIS,Drug::MainInnDosage).toString());
        filter = QString("((%1) OR (%2))").arg(filter).arg(innFilter);
    }        
//    qWarning() << "filter" << filter;
    setFilter(filter);
    select();
    return true;
}

/** \brief Return the actual drug's CIS */
int mfDosageModel::drugCIS()
{
    return m_CIS;
}

/** \brief Test all the column of the selected row, if one value is dirty return true. */
bool mfDosageModel::isDirty(const int row) const
{
    int i;
    for(i=0;i<columnCount();++i) {
        if (QSqlTableModel::isDirty(index(row, i)))
            return true;
    }
    return false;
}

// TODO: write this */
QStringList mfDosageModel::isDosageValid( const int row )
{
    Q_ASSERT_X( m_CIS != -1, "mfDosageModel::isDosageValid", "before using the dosagemodel, you must specify the CIS of the related drug");
    QStringList errors;
    // Label
    if (index(row, Dosage::Label).data().toString().isEmpty())
        errors << tr("The dosage must be labelled.");
    // Intakes
    if (index(row, Dosage::IntakesScheme).data().toString().isEmpty())
        errors << tr("The intakes' scheme must be defined.");
    if (index(row, Dosage::PeriodScheme).data().toString().isEmpty())
        errors << tr("The period's scheme must be defined.");
    // Duration
    if (index(row, Dosage::DurationScheme).data().toString().isEmpty())
        errors << tr("The duration's scheme must be defined.");

    //TODO
    return errors;
}

#ifndef DRUGS_INTERACTIONS_STANDALONE
/** \brief Test user's permissions for the drugs read access */
bool mfDosageModel::userCanRead()
{
    //TODO --> test
    User::UserRights r = User::UserRights(tkUserModel::instance()->currentUserData( User::DrugsRights ).toInt());
    return (r & User::ReadOwn) || (r & User::ReadAll);
}

/** \brief Test user's permissions for the drugs write access */
bool mfDosageModel::userCanWrite()
{
    //TODO  --> test
    User::UserRights r = User::UserRights(tkUserModel::instance()->currentUserData( User::DrugsRights ).toInt());
    return (r & User::WriteOwn) || (r & User::WriteAll);
}
#endif

/**
  \brief Transforms a dosage to a drug's prescription.
  \sa mfDrugsModel
*/
void mfDosageModel::toPrescription(const int row)
{
    Q_ASSERT(DRUGMODEL->containsDrug(m_CIS));
    mfDrugsModel *M = DRUGMODEL;
    QHash<int,int> prescr_dosage;
    prescr_dosage.insert( Prescription::UsedDosage ,           Dosage::Uuid );
    prescr_dosage.insert( Prescription::IntakesFrom ,          Dosage::IntakesFrom );
    prescr_dosage.insert( Prescription::IntakesTo ,            Dosage::IntakesTo );
    prescr_dosage.insert( Prescription::IntakesUsesFromTo ,    Dosage::IntakesUsesFromTo );
    prescr_dosage.insert( Prescription::IntakesScheme ,        Dosage::IntakesScheme );
    prescr_dosage.insert( Prescription::Period ,               Dosage::Period );
    prescr_dosage.insert( Prescription::PeriodScheme ,         Dosage::PeriodScheme );
    prescr_dosage.insert( Prescription::DurationFrom ,         Dosage::DurationFrom );
    prescr_dosage.insert( Prescription::DurationTo ,           Dosage::DurationTo );
    prescr_dosage.insert( Prescription::DurationUsesFromTo,    Dosage::DurationUsesFromTo );
    prescr_dosage.insert( Prescription::DurationScheme ,       Dosage::DurationScheme );
    prescr_dosage.insert( Prescription::IntakesIntervalOfTime ,Dosage::IntakesIntervalOfTime );
    prescr_dosage.insert( Prescription::IntakesIntervalScheme ,Dosage::IntakesIntervalScheme );
    prescr_dosage.insert( Prescription::Note ,                 Dosage::Note );
    prescr_dosage.insert( Prescription::DailyScheme ,          Dosage::DailyScheme );
    prescr_dosage.insert( Prescription::MealTimeSchemeIndex ,  Dosage::MealScheme );
    prescr_dosage.insert( Prescription::IsALD ,                Dosage::IsALD );
    foreach( const int i, prescr_dosage.keys()) {
        M->setDrugData(m_CIS, i, data(index(row, prescr_dosage.value(i))) );
    }
    if (index(row,Dosage::INN_LK).data().toInt() > 999) // this is an INN prescription
        M->setDrugData(m_CIS, Prescription::IsINNPrescription, true);
    else
        M->setDrugData(m_CIS, Prescription::IsINNPrescription, false);
    M->resetModel();
}

/** \brief Transforms a model's row into XML encoded QString. This part is using the database fieldnames.*/
QString mfDosageModel::toXml( const int row )
{
    int j;
    QHash<QString,QString> datas;
    for( j=0; j < columnCount(); ++j ) {
        datas.insert( record().fieldName(j).toLower(), index( row, j ).data().toString() );
    }
    return tkGlobal::createXml(XML_DOSAGE_MAINTAG,datas,4,false);
}

/**
  \brief Adds a XML file to the model. A verification of the drugs' CIS is done.
  \todo write this
*/
bool mfDosageModel::addFromXml(const QString &xml)
{
//    QHash<QString, QString> datas;
//    int n = xml.count("<"+XML_DOSAGE_MAINTAG+">");
//    if (n<=0)
//        return false;
//    foreach( const QString &s, xml.split("</"+XML_DOSAGE_MAINTAG+">")) {
//        if (!tkGlobal::readXml(xml,XML_DOSAGE_MAINTAG,datas,false))
//            return false;
//        // insert row, get the index of it
//        foreach(const QString &k, datas.keys()) {
//            int id = record().indexOf(k);
//            if (id==-1)
//                continue;
//            //        setData( index(
//        }
//
//    }
    return true;
}

/** \brief For debugging purpose only */
void mfDosageModel::warn( const int row )
{
    if (!tkGlobal::isDebugCompilation())
        return;
//    qWarning() << "database connection" << database().connectionName();

    // row == -1 -> warn all rows
    int i;
    int j;
    if ( row == -1 ) {
        for (i=0; i < rowCount(); ++i )
            qWarning() << toXml(i);
//            for( j=0; j < columnCount(); ++j ) {
//            qWarning() << record().fieldName(j) << index( i, j ).data() ;
//        }
    } else {
        for( j=0; j < columnCount(); ++j ) {
            qWarning() << record().fieldName(j) << index( row, j ).data() ;
        }
    }
}
