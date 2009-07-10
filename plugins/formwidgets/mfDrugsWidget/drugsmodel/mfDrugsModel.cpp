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
/**
  \class mfDrugsModel
  \brief Model asks mfDrugsBase to check interaction only while passing new *mfDrugs via addDrugs()
  or while passing QDrugsList via setDrugsList().
  \todo write code documentation
  \ingroup drugsinteractions drugswidget
*/

#include "mfDrugsModel.h"

// include drugwidgets headers
#include <drugsdatabase/mfDrugsBase.h>
#include <drugsmodel/mfDrugInteraction.h>
#include <drugsmodel/mfDrugsIO.h>
#include <mfDrugsConstants.h>

// include toolkit headers
#include <tkGlobal.h>
#include <tkLog.h>
#include <tkSettings.h>
#include <tkSerializer.h>

// include Qt headers
#include <QApplication>
#include <QIcon>
#include <QFont>
#include <QHash>
#include <QPointer>
#include <QStringList>


namespace mfDrugsModelConstants {
    const char * const ALD_BACKGROUND_COLOR               = "khaki";
}

using namespace mfDrugsModelConstants;

/**
  \brief Private part of mfDrugsModel
  \internal
*/
class mfDrugsModelPrivate
{
public:
    mfDrugsModelPrivate() : m_LastDrugRequiered(0)
    {
    }

    ~mfDrugsModelPrivate()
    {
        qDeleteAll( m_DosageModelList );
        m_DosageModelList.clear();
        qDeleteAll( m_DrugsList );
        m_DrugsList.clear();
    }

    /** \brief Return the pointer to the drug if it is already in the drugs list, otherwise return 0 */
    mfDrugs *getDrug(const int CIS)
    {
        if (m_LastDrugRequiered) {
            if (m_LastDrugRequiered->CIS() == CIS) {
                return m_LastDrugRequiered;
            }
        }
        m_LastDrugRequiered = 0;
        foreach(mfDrugs *drug, m_DrugsList) {
            if (drug->CIS()==CIS)
                m_LastDrugRequiered = drug;
        }
        return m_LastDrugRequiered;
    }

    /**
       \brief Set drugs' data directly into the private drugsList
       \sa mfDrugsModel::setData()
    */
    bool setDrugData( mfDrugs *drug, const int column, const QVariant & value )
    {
        Q_ASSERT(drug);
        if ((column < Prescription::Id) || (column > Prescription::MaxParam))
            return false;
        drug->setPrescriptionValue( column, value );
//        qWarning() << "setdrugdata" << xmlTagForPrescriptionRow(column) << value;
        return true;
    }

    /**
       \brief Returns the datas of the drugs by picking it into the private drugslist of the model
       \sa mfDrugsModel::data()
    */
    QVariant getDrugValue( const mfDrugs *drug, const int column ) const
    {
        switch (column)
        {
             case Drug::Denomination : return drug->denomination();
             case Drug::CIS : return drug->CIS();
             case Drug::CIPs : return drug->CIPs();
             case Drug::Form :  return drug->form();
             case Drug::AvailableForms :
                 {
                     QStringList toReturn;
                     toReturn << QApplication::translate("mfDrugsModel", "intakes" );
                     toReturn << drug->form();
                     if ( drug->numberOfInn() == 1 ) {
                         toReturn << QApplication::translate("mfDrugsModel", "x %1 of %2" )
                                 .arg( drug->dosageOfMolecules().at( 0 ) , drug->listOfInn().at( 0 ) );
                     }
                     if (!tkSettings::instance()->value(MFDRUGS_SETTING_USERRECORDEDFORMS).isNull()) {
                         foreach( const QString &s, tkSettings::instance()->value(MFDRUGS_SETTING_USERRECORDEDFORMS).toStringList())
                             if (!s.isEmpty())
                                 toReturn << s;
                     }
                     toReturn << mfDosageModel::predeterminedForms();
                     return toReturn;
                     break;
                 }
             case Drug::AvailableDosages :
                 {
                     QStringList list;
                     list << QApplication::translate("mfDrugsModel","Available Dosages" );
                     // TODO
                     return list.join("<br />");
                     break;
                 }
             case Drug::IsScoredTablet :     return drug->isScoredTablet();
             case Drug::Molecules :          return drug->listOfMolecules();
             case Drug::AllInnsKnown :       return mfDrugsBase::instance()->drugsINNIsKnown(drug);
             case Drug::Inns :               return drug->listOfInn();
             case Drug::MainInnCode :        return drug->mainInnCode();
             case Drug::MainInnDosage :      return drug->mainInnDosage();
             case Drug::MainInnName :        return drug->mainInnName();
             case Drug::InnClasses :         return drug->listOfInnClasses();
             case Drug::Administration :     return QVariant();
             case Drug::Interacts :          return mfDrugsBase::instance()->drugHaveInteraction( drug );
             case Drug::MaximumLevelOfInteraction : return int(mfDrugsBase::instance()->getMaximumTypeOfIAM( drug ));
             case Drug::CompositionString :  return drug->toHtml();
             case Drug::InnCompositionString :  return drug->innComposition();
             case Drug::CodeMoleculesList :  return drug->listOfCodeMolecules();
             case Drug::HasPrescription :    return drug->hasPrescription();
             case Drug::FullPrescription :
                 {
                     QString tmp;
                     if (drug->prescriptionValue(Prescription::IsINNPrescription).toBool())
                         tmp = drug->innComposition();
                     else tmp = drug->denomination();
                     tmp += "\n";
                     tmp += drug->prescriptionToPlainText();
                     return tmp;
                 }

             case Prescription::UsedDosage :            return drug->prescriptionValue( Prescription::UsedDosage );
             case Prescription::IntakesFrom :           return drug->prescriptionValue( Prescription::IntakesFrom );
             case Prescription::IntakesTo :             return drug->prescriptionValue( Prescription::IntakesTo );
             case Prescription::IntakesScheme :         return drug->prescriptionValue( Prescription::IntakesScheme );
             case Prescription::IntakesUsesFromTo :     return drug->prescriptionValue( Prescription::IntakesUsesFromTo );
             case Prescription::IntakesIntervalOfTime : return drug->prescriptionValue( Prescription::IntakesIntervalOfTime );
             case Prescription::IntakesIntervalScheme : return drug->prescriptionValue( Prescription::IntakesIntervalScheme );
             case Prescription::DurationFrom :          return drug->prescriptionValue( Prescription::DurationFrom );
             case Prescription::DurationTo :            return drug->prescriptionValue( Prescription::DurationTo );
             case Prescription::DurationScheme :        return drug->prescriptionValue( Prescription::DurationScheme );
             case Prescription::DurationUsesFromTo :    return drug->prescriptionValue( Prescription::DurationUsesFromTo );
             case Prescription::MealTimeSchemeIndex :   return drug->prescriptionValue( Prescription::MealTimeSchemeIndex );
             case Prescription::Period :                return drug->prescriptionValue( Prescription::Period );
             case Prescription::PeriodScheme :          return drug->prescriptionValue( Prescription::PeriodScheme );
             case Prescription::DailyScheme :           return drug->prescriptionValue( Prescription::DailyScheme );
             case Prescription::Note :                  return drug->prescriptionValue( Prescription::Note );
             case Prescription::IsINNPrescription :     return drug->prescriptionValue( Prescription::IsINNPrescription );
             case Prescription::SpecifyForm :           return drug->prescriptionValue( Prescription::SpecifyForm );
             case Prescription::SpecifyPresentation :   return drug->prescriptionValue( Prescription::SpecifyPresentation );
             case Prescription::IsALD :                 return drug->prescriptionValue( Prescription::IsALD );
             case Prescription::ToHtml :                return drug->prescriptionToHtml();

             case Interaction::Id :     return QVariant();
             case Interaction::Icon :   return mfDrugInteraction::iamIcon( drug, m_levelOfWarning );
             case Interaction::Pixmap : return mfDrugInteraction::iamIcon( drug, m_levelOfWarning ).pixmap(16,16);
             case Interaction::ToolTip :
                 {
                     mfDrugsBase *b = mfDrugsBase::instance();
                     QString display;
                     if ( b->drugHaveInteraction( drug ) ) {
                         const QList<mfDrugInteraction *> & list = b->getInteractions( drug );
                         display.append( mfDrugInteraction::listToHtml( list, false ) );
                     } else if ( b->drugsINNIsKnown( drug ) ) {
                         display = drug->listOfInn().join( "<br />") + "<br />" + drug->listOfInnClasses().join( "<br />" );
                     } else
                         display = QApplication::translate("mfDrugsModel","No Inn founded" );
                     return display;
                     break;
                 }
             case Interaction::FullSynthesis :
                 {
                     QString display;
                     const QList<mfDrugInteraction *> & list = mfDrugsBase::instance()->getAllInteractionsFound();
                     int i = 0;
                     display.append("<p>");
                     foreach( mfDrugs *drg, m_DrugsList ) {
                         ++i;
                         display.append( QString("%1&nbsp;.&nbsp;%2<br />")
                                         .arg(i)
                                         .arg(drg->denomination() ) );
                     }
                     display.append("</p><p>");
                     if ( list.count() > 0 ) {
                         display.append( mfDrugInteraction::synthesisToHtml( list, true ) );
                     } else
                         display = QApplication::translate("mfDrugsModel","No interactions founded" );
                     display.append("</p>");
                     return display;
                     break;
                 }
             case Interaction::MaxParam : return QVariant();
             }
        return QVariant();
    }

public:
    QDrugsList  m_DrugsList;       /*!< \brief Actual prescription drugs list */
    int m_levelOfWarning;          /*!< \brief Level of warning to use (retrieve from settings). */
    mutable QHash<int, QPointer<mfDosageModel> > m_DosageModelList;  /** \brief associated CIS / dosageModel */
    mfDrugs *m_LastDrugRequiered; /*!< \brief Stores the last requiered drug by drugData() for speed improvments */

};

using namespace mfDrugsConstants;
using namespace mfDosagesConstants;
using namespace mfInteractionsConstants;

mfDrugsModel *mfDrugsModel::m_Instance = 0;

/** \brief Unique instance of the drugs model */
mfDrugsModel *mfDrugsModel::instance()
{
    if ( ! m_Instance )
        m_Instance = new mfDrugsModel( qApp );
    return m_Instance;
}

/** \brief Constructor */
mfDrugsModel::mfDrugsModel( QObject * parent )
        : QAbstractTableModel( parent ),
        d(new mfDrugsModelPrivate())
{
    if ( !mfDrugsBase::isInitialized() )
        return;
    d->m_DrugsList.clear();
    d->m_DosageModelList.clear();
}

/** \brief Destructor */
mfDrugsModel::~mfDrugsModel()
{
    if (d) delete d;
    d=0;
}

/** \brief count the number of selected drugs */
int mfDrugsModel::rowCount( const QModelIndex & parent ) const
{
    Q_UNUSED(parent);
    return d->m_DrugsList.count();
}

/** \brief Returns the actual selected drugs list in the model */
const QDrugsList & mfDrugsModel::drugsList() const
{
    return d->m_DrugsList;
}

QModelIndex mfDrugsModel::index( int row, int column, const QModelIndex & parent ) const
{
    Q_UNUSED(parent);
    if ( row < d->m_DrugsList.count() )
        return createIndex( row, column );
    return QModelIndex();
}

QModelIndex mfDrugsModel::createIndex( int row, int column, void * ) const
{
    // TODO create index with mfDosageModel if necessary
    return QAbstractItemModel::createIndex( row, column, 0 );
}

QModelIndex mfDrugsModel::createIndex( int /*row*/, int /*column*/, quint32 /*id*/ ) const
{
    Q_ASSERT_X( false, "mfDrugsModel::createIndex( int, int , quint32  ) const", "You should not use this function" );
    return QModelIndex();
}

/**
  \brief Defines the data of drugs.
  The drugs model is a read only model for all the namespace mfDrugsConstants::Drug enum values.\n
  Dosages values are not accessible from here. Use the mfDosageModel.\n
  Prescritions values are writables. Informations are transmitted using mfDrugs::setPrescriptionValue().
*/
bool mfDrugsModel::setData( const QModelIndex & index, const QVariant & value, int role )
{
    Q_UNUSED(role);
    if (!index.isValid())
        return false;
    int row = index.row();
    if ((row >= d->m_DrugsList.count()) || (row < 0))
        return false;
    mfDrugs * drug = d->m_DrugsList.at(row);
    if (d->setDrugData(drug, index.column(), value)) {
        // inform of the modification
        Q_EMIT dataChanged(index, index);
        QModelIndex fullPrescr = this->index(index.row(),Drug::FullPrescription);
        Q_EMIT dataChanged( fullPrescr, fullPrescr );
        Q_EMIT prescriptionResultChanged( drug->prescriptionToPlainText() );
    }
    return true;
}

/**
  \brief Set data for the specified drug with the corresponding \e CIS.
  Drug must be setted into the model otherwise, this function returns false.\n
  If you want the model to be refreshed call resetModel() after all datas were setted.
*/
bool mfDrugsModel::setDrugData( const int CIS, const int column, const QVariant &value )
{
    mfDrugs *drug = d->getDrug(CIS);
    if (!drug)
        return false;
    if (d->setDrugData(drug, column, value)) {
        Q_EMIT prescriptionResultChanged( drug->prescriptionToPlainText() );
        return true;
    }
    return false;
}

/** \brief Reset the model */
void mfDrugsModel::resetModel()
{
    reset();
}

/**
  \brief Returns the values of drugs and prescription, you can not access to the dosage model this way.
  mfDosageModel regarding a specific drug is accessible using dosageModel().\n
  Available datas index :
  \li Drugs specific datas : see mfDrugsConstants::Drug enumerator
  \li Interactions specific datas : see mfInteractionsConstants::Interaction enumerator
  \li Prescription datas : see mfDrugsConstants::Prescription enumerator
*/
QVariant mfDrugsModel::data( const QModelIndex &index, int role ) const
{
    if ( !index.isValid() )
        return QVariant();

    if (( index.row() > d->m_DrugsList.count() ) || ( index.row() < 0 ) )
        return QVariant();

    mfDrugsBase *b = mfDrugsBase::instance();
    const mfDrugs * drug = d->m_DrugsList.at( index.row() );

    if ( ( role == Qt::DisplayRole ) || ( role == Qt::EditRole ) ) {
        int col = index.column();

        // manage indexes for dosageModel
        if ( (col >= Dosage::Id) && (col < Dosage::MaxParam) ) {
            return QVariant();
        } else {
            // deep in the abyss
            return d->getDrugValue(drug, index.column());
        }
    }
    else if ( role == Qt::DecorationRole ) {
        return mfDrugInteraction::iamIcon( drug, d->m_levelOfWarning );
    }
    else if ( role == Qt::ToolTipRole ) {
        QString display;
        display = drug->toHtml();

        if ( b->drugHaveInteraction( drug ) ) {
            const QList<mfDrugInteraction *> & list = b->getInteractions( drug );
            display.append( "<br>\n" );
            display.append( mfDrugInteraction::listToHtml( list, false ) );
        }
        return display;
    }
    else if ( role == Qt::BackgroundRole ) {
        if (drug->prescriptionValue( Prescription::IsALD ).toBool()) {
            return QColor(ALD_BACKGROUND_COLOR);
        }
    }
    //     else if ( role == Qt::SizeHintRole ) {
    //         return QSize(d->m_RowSize[index.row()],d->m_RowSize[index.row()]);
    //     }
    return QVariant();
}

/**
  \brief At anytime, you can get all values of drugs inside the prescription model using the CIS as row index.
  \sa data()
*/
QVariant mfDrugsModel::drugData( const int CIS, const int column )
{
    mfDrugs *drug = m_Instance->d->getDrug(CIS);
    if (!drug)
        return QVariant();
    return m_Instance->d->getDrugValue(drug, column);
}

/** \brief Should not be used. \internal */
Qt::ItemFlags mfDrugsModel::flags( const QModelIndex &index ) const
{
    if ( !index.isValid() )
        return Qt::ItemIsEnabled;

    return QAbstractItemModel::flags( index );
}

/** \brief Removes \e count drugs from the \e row. */
bool mfDrugsModel::removeRows( int row, int count, const QModelIndex & parent )
{
    d->m_LastDrugRequiered = 0;
    beginRemoveRows( parent, row, row+count );
    if (row >= d->m_DrugsList.count())
        return false;
    if ( (row + count) > d->m_DrugsList.count() )
        return false;
    int i;
    for( i = 0; i < count; ++i ) {
        delete d->m_DrugsList.at(row+i);
        d->m_DrugsList.removeAt(row+i);
    }
    checkInteractions();
    endRemoveRows();
    reset();
    Q_EMIT numberOfRowsChanged();
    return true;
}

/**
 \brief Add a drug to the prescription.
 \sa addDrug()
*/
int mfDrugsModel::addDrug( mfDrugs* drug, bool automaticInteractionChecking )
{
    if (!drug)
        return -1;
    // insert only once the same drug
    if (containsDrug(drug->CIS()))
        return d->m_DrugsList.indexOf(drug);
    d->m_DrugsList << drug;
    // check drugs interactions from mfDrugsBase
    if (automaticInteractionChecking) {
        checkInteractions();
        d->m_levelOfWarning = tkSettings::instance()->value( MFDRUGS_SETTING_LEVELOFWARNING ).toInt();
    }
    reset();
    Q_EMIT numberOfRowsChanged();
    return d->m_DrugsList.indexOf(drug);
}

/**
  \brief Add a drug to the prescription.
  \e automaticInteractionChecking can be setted of if you want to add
     multiple drugs. You should call checkInteractions() after all in this case. \n
   Please take care, that the same drug can not be inserted more than once ! \n
   Return the index of the inserted drug into the list or -1 if no drug was inserted.
   \sa addDrug()
*/
int mfDrugsModel::addDrug( const int _CIS, bool automaticInteractionChecking )
{
    return addDrug( mfDrugsBase::instance()->getDrugByCIS( _CIS ), automaticInteractionChecking );
}

/** \brief Clear the prescription. Clear all interactions too. */
void mfDrugsModel::clearDrugsList()
{
    d->m_LastDrugRequiered = 0;
    qDeleteAll(d->m_DrugsList);
    d->m_DrugsList.clear();
    d->m_levelOfWarning = tkSettings::instance()->value( MFDRUGS_SETTING_LEVELOFWARNING ).toInt();
    reset();
    Q_EMIT numberOfRowsChanged();
}

/** \brief Insert a list of drugs and check interactions. */
void mfDrugsModel::setDrugsList( QDrugsList & list )
{
    d->m_DrugsList.clear();
    d->m_LastDrugRequiered = 0;
    d->m_DrugsList << list;
    checkInteractions();
    d->m_levelOfWarning = tkSettings::instance()->value( MFDRUGS_SETTING_LEVELOFWARNING ).toInt();
    reset();
    Q_EMIT numberOfRowsChanged();
}

/** \brief Returns true if the drug is already in the prescription */
bool mfDrugsModel::containsDrug(const int CIS) const
{
    if (d->getDrug(CIS))
        return true;
    return false;
}

/** \brief Sort the drugs inside prescription. \sa mfDrugs::lessThan(). */
void mfDrugsModel::sort( int, Qt::SortOrder )
{
    qSort(d->m_DrugsList.begin(), d->m_DrugsList.end(), mfDrugs::lessThan );
    reset();
}

/** \brief Moves a drug up. */
bool mfDrugsModel::moveUp( const QModelIndex & item )
{
    if ( !item.isValid() )
        return false;

    if (item.row() >= 1) {
        d->m_DrugsList.move( item.row(), item.row()-1);
        reset();
        return true;
    }
    return false;
}

/** \brief Moves a drug down. */
bool mfDrugsModel::moveDown( const QModelIndex & item )
{
    if ( !item.isValid() )
        return false;

    if (item.row() < (rowCount()-1)) {
        d->m_DrugsList.move( item.row(), item.row()+1);
        reset();
        return true;
    }
    return false;
}

/** \brief Returns the dosage model for the selected drug */
mfDosageModel * mfDrugsModel::dosageModel( const int _CIS )
{
    if ( ! d->m_DosageModelList.keys().contains( _CIS ) ) {
        d->m_DosageModelList.insert( _CIS, new mfDosageModel(this) );
        d->m_DosageModelList[_CIS]->setDrugCIS(_CIS);
    } else if ( ! d->m_DosageModelList.value(_CIS) ) {
        d->m_DosageModelList.insert( _CIS, new mfDosageModel(this) );
        d->m_DosageModelList[_CIS]->setDrugCIS(_CIS);
    }
    return d->m_DosageModelList.value(_CIS);
}

/** \brief Returns the dosage model for the selected drug */
mfDosageModel *mfDrugsModel::dosageModel( const QModelIndex &drugIndex )
{
    if ( ! drugIndex.isValid() )
        return 0;
    if ( drugIndex.column() != Drug::CIS )
        return 0;
    return dosageModel( drugIndex.data().toInt() );
}

/** \brief Removes a drug from the prescription */
int mfDrugsModel::removeDrug( const int _CIS )
{
    // Take care that this function removes all occurence of the referenced drug
    d->m_LastDrugRequiered = 0;
    int i = 0;
    foreach( mfDrugs * drug, d->m_DrugsList ) {
        if ( drug->CIS() == _CIS ) {
            d->m_DrugsList.removeAt( d->m_DrugsList.indexOf(drug) );
            delete drug;
            ++i;
        }
    }
    checkInteractions();
    reset();
    Q_EMIT numberOfRowsChanged();
    return i;
}

/** \brief Removes last inserted drug from the prescription */
int mfDrugsModel::removeLastInsertedDrug()
{
    // TODO Take care if user inserted x times the same drug
    d->m_LastDrugRequiered = 0;
    if (d->m_DrugsList.count() == 0)
        return 0;
    delete d->m_DrugsList.last();
    d->m_DrugsList.removeLast();
    checkInteractions();
    reset();
    Q_EMIT numberOfRowsChanged();
    return 1;
}

/**  \brief Only for debugging purpose. */
void mfDrugsModel::warn()
{
    if (!tkGlobal::isDebugCompilation())
        return;
    qWarning() << "drugs in memory" << d->m_DrugsList.count();
    qWarning() << "dosagemodels in memory" << d->m_DosageModelList.count();
}

/** \brief Starts the interactions checking */
void mfDrugsModel::checkInteractions() const
{
    mfDrugsBase::instance()->interactions( d->m_DrugsList );
}
