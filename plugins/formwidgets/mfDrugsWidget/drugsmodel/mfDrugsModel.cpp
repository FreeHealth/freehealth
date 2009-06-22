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
#include "mfDrugsModel.h"

// include drugwidgets headers
#include "drugsdatabase/mfDrugsBase.h"
#include "drugsmodel/mfDrugInteraction.h"
#include "mfDrugsConstants.h"

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

class mfDrugsModelPrivate
{
public:
    QVariant getDrugValue( const mfDrugs *drug, const int column ) const
    {
        switch ( column )
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
             case Drug::InnClasses :         return drug->listOfInnClasses();
             case Drug::InnEquivalentsNames: return mfDrugsBase::instance()->findInnEquivalentsNames( drug );
             case Drug::Administration :     return QVariant();
             case Drug::Interacts :          return mfDrugsBase::instance()->drugHaveInteraction( drug );
             case Drug::MaximumLevelOfInteraction : return int(mfDrugsBase::instance()->getMaximumTypeOfIAM( drug ));
             case Drug::CompositionString :  return drug->toHtml();
             case Drug::CodeMoleculesList :  return drug->listOfCodeMolecules();
             case Drug::HasPrescription :    return drug->hasPrescription();
             case Drug::FullPrescription :
                 {
                     QString tmp = drug->denomination();
                     tmp += "\n";
                     tmp += drug->prescriptionToPlainText();
                     return tmp;
                 }

             case Prescription::UsedDosage :            return drug->prescriptionValue( Prescription::UsedDosage );
             case Prescription::IntakesFrom :           return drug->prescriptionValue( Prescription::IntakesFrom );
             case Prescription::IntakesTo :             return drug->prescriptionValue( Prescription::IntakesTo );
             case Prescription::IntakesScheme :         return drug->prescriptionValue( Prescription::IntakesScheme );
             case Prescription::IntakesUsesFromAndTo :  return drug->prescriptionValue( Prescription::IntakesUsesFromAndTo );
             case Prescription::DurationFrom :          return drug->prescriptionValue( Prescription::DurationFrom );
             case Prescription::DurationTo :            return drug->prescriptionValue( Prescription::DurationTo );
             case Prescription::DurationScheme :        return drug->prescriptionValue( Prescription::DurationScheme );
             case Prescription::DurationUsesFromAndTo : return drug->prescriptionValue( Prescription::DurationUsesFromAndTo );
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
    QDrugsList  m_DrugsList;
    int m_levelOfWarning;
    mutable QHash<int, QPointer<mfDosageModel> > m_DosageModelList;  /** \brief associated CIS / dosageModel */
    QHash<int, int> m_RowSize;
};

const char * const ALD_BACKGROUND_COLOR = "khaki";

using namespace mfDrugsConstants;
using namespace mfDosagesConstants;
using namespace mfInteractionsConstants;

mfDrugsModel *mfDrugsModel::m_Instance = 0;

mfDrugsModel *mfDrugsModel::instance()
{
    if ( ! m_Instance )
        m_Instance = new mfDrugsModel( qApp );
    return m_Instance;
}

mfDrugsModel::mfDrugsModel( QObject * parent )
               : QAbstractTableModel( parent ),
               d(new mfDrugsModelPrivate())
{
    if ( !mfDrugsBase::isInitialized() )
        return;
//    m_DosageModel = mfDosageModel::instance();
    d->m_DrugsList.clear();
    d->m_DosageModelList.clear();
}

mfDrugsModel::~mfDrugsModel()
{
    qDeleteAll( d->m_DosageModelList );
    d->m_DosageModelList.clear();
    qDeleteAll( d->m_DrugsList );
    d->m_DrugsList.clear();
    if (d) delete d;
    d=0;
}

int mfDrugsModel::rowCount( const QModelIndex & parent ) const
{
    Q_UNUSED(parent);
    return d->m_DrugsList.count();
}

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

bool mfDrugsModel::setData( const QModelIndex & index, const QVariant & value, int role )
{
    Q_UNUSED(role);
    // The drugs model is a read only model for all Drug:: values.
    // Dosages values are writable using the dosage model
    // Prescritions values are writables
   if (!index.isValid())
       return false;
   int column = index.column();
   int row = index.row();
   // Writables datas are ONLY Prescription:: values
   if ((column < Prescription::Id) || (column > Prescription::MaxParam))
       return false;
   if ((row >= d->m_DrugsList.count()) || (row < 0))
       return false;

   mfDrugs * drug = d->m_DrugsList.at( row );
   drug->setPrescriptionValue( column, value );
//   switch (column)
//   {
//       case Prescription::UsedDosage : drug->setPrescriptionValue( Prescription::UsedDosage, value ); break;
//       case Prescription::IntakesFrom : drug->setPrescriptionValue( Prescription::IntakesFrom, value ); break;
//       case Prescription::IntakesTo : drug->setPrescriptionValue( Prescription::IntakesTo, value ); break;
//       case Prescription::IntakesScheme : drug->setPrescriptionValue( Prescription::IntakesScheme, value ); break;
//       case Prescription::IntakesUsesFromAndTo : drug->setPrescriptionValue( Prescription::IntakesUsesFromAndTo, value ); break;;
//       case Prescription::DurationFrom : drug->setPrescriptionValue( Prescription::DurationFrom, value ); break;
//       case Prescription::DurationTo : drug->setPrescriptionValue( Prescription::DurationTo, value ); break;
//       case Prescription::DurationScheme : drug->setPrescriptionValue( Prescription::DurationScheme, value ); break;
//       case Prescription::DurationUsesFromAndTo : drug->setPrescriptionValue( Prescription::DurationUsesFromAndTo, value ); break;;
//       case Prescription::Period : drug->setPrescriptionValue( Prescription::Period, value ); break;
//       case Prescription::PeriodScheme : drug->setPrescriptionValue( Prescription::PeriodScheme, value ); break;
//       case Prescription::MealTimeSchemeIndex : drug->setPrescriptionValue( Prescription::MealTimeSchemeIndex, value ); break;
//       case Prescription::Note : drug->setPrescriptionValue( Prescription::Note, value ); break;
//        IsINNPrescription,
//        SpecifyForm,
//        SpecifyPresentation,
//        IsALD,
//             case Prescription::DailyScheme : return drug->prescriptionValue( Prescription::DailyScheme );

//       default : break;
//   }
   emit dataChanged(index, index);
   QModelIndex fullPrescr = this->index(index.row(),Drug::FullPrescription);
   emit dataChanged( fullPrescr, fullPrescr );
   emit prescriptionResultChanged( drug->prescriptionToPlainText() );

   return true;
}

/**
  \brief Returns the values of drugs and prescription, you can not access to the dosage model this way.
  mfDosageModel regarding a specific drug is accessible using dosageModel().
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

          QDrugsList withoutThis = d->m_DrugsList;
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
*/
QVariant mfDrugsModel::drugData( const int CIS, const int column )
{
    static mfDrugs *lastDrugRequiered = 0;
    if (lastDrugRequiered) {
        if (lastDrugRequiered->CIS() != CIS) {
            foreach( mfDrugs *dg, m_Instance->d->m_DrugsList) {
                if (dg->CIS() == CIS) {
                    lastDrugRequiered = dg;
                    break;
                }
            }
            if (!lastDrugRequiered)
                return QVariant();
        }
    } else {
            foreach( mfDrugs *dg, m_Instance->d->m_DrugsList) {
                if (dg->CIS() == CIS) {
                    lastDrugRequiered = dg;
                    break;
                }
            }
            if (!lastDrugRequiered)
                return QVariant();
    }
    return m_Instance->d->getDrugValue(lastDrugRequiered, column);
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
    beginRemoveRows( parent, row, row+count );
    if ( row >= d->m_DrugsList.count() )
        return false;
    if ( ( row + count ) > d->m_DrugsList.count() )
        return false;
    int i;
    for( i = 0; i < count; ++i )
        d->m_DrugsList.removeAt( row );
    checkInteractions();
    endRemoveRows();
    reset();
    emit numberOfRowsChanged();
    return true;
}

/** \brief Add a drug to the prescription. \e automaticInteractionChecking can be setted of if you want to add
     multiple drugs. You should call checkInteractions() after all in this case.
*/
int mfDrugsModel::addDrug( mfDrugs* drug, bool automaticInteractionChecking )
{
     if ( ! d->m_DrugsList.isEmpty() ) {
          if ( !d->m_DrugsList.contains( drug ) )
               d->m_DrugsList.append( drug );
     }
     else
          d->m_DrugsList <<  drug;
     // check drugs interactions from mfDrugsBase
     if (automaticInteractionChecking) {
         checkInteractions();
         d->m_levelOfWarning = tkSettings::instance()->value( MFDRUGS_SETTING_LEVELOFWARNING ).toInt();
     }
     reset();
     emit numberOfRowsChanged();
     return d->m_DrugsList.indexOf( drug );
}

/** \brief Add a drug to the prescription. \e automaticInteractionChecking can be setted of if you want to add
     multiple drugs. You should call checkInteractions() after all in this case.
*/
int mfDrugsModel::addDrug( const int _CIS, bool automaticInteractionChecking )
{
    return addDrug( mfDrugsBase::instance()->getDrugByCIS( _CIS ), automaticInteractionChecking );
}

/** \brief Clear the prescription. Clear all interactions too. */
void mfDrugsModel::clearDrugsList()
{
     qDeleteAll(d->m_DrugsList);
     d->m_DrugsList.clear();
     d->m_levelOfWarning = tkSettings::instance()->value( MFDRUGS_SETTING_LEVELOFWARNING ).toInt();
     reset();
     emit numberOfRowsChanged();
}

/** \brief Insert a list of drugs and check interactions. */
void mfDrugsModel::setDrugsList( QDrugsList & list )
{
     d->m_DrugsList.clear();
     d->m_DrugsList << list;
     checkInteractions();
     d->m_levelOfWarning = tkSettings::instance()->value( MFDRUGS_SETTING_LEVELOFWARNING ).toInt();
     reset();
     emit numberOfRowsChanged();
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

mfDosageModel * mfDrugsModel::dosageModel( const QModelIndex & drugIndex )
{
    if ( ! drugIndex.isValid() )
        return 0;
    if ( drugIndex.column() != Drug::CIS )
        return 0;
    return dosageModel( drugIndex.data().toInt() );
}

int mfDrugsModel::removeDrug( const int _CIS )
{
    // Take care that this function removes all occurence of the referenced drug
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
    emit numberOfRowsChanged();
    return i;
}

/**  */
int mfDrugsModel::removeLastInsertedDrug()
{
    // TODO Take care if user inserted x times the same drug
    if (d->m_DrugsList.count() == 0)
        return 0;
    delete d->m_DrugsList.last();
    d->m_DrugsList.removeLast();
    checkInteractions();
    reset();
    emit numberOfRowsChanged();
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

QString mfDrugsModel::prescriptionToHtml()
{
    if (rowCount() <= 0)
        return QString();
    sort(0);

    // TODO manage DCI, no Forms, FontSize from settings()
    tkSettings *s = tkSettings::instance();

    // Prepare font format
    QFont drugsFont;
    drugsFont.fromString(s->value( MFDRUGS_SETTING_DRUGFONT ).toString());
    QFont prescrFont;
    prescrFont.fromString(s->value( MFDRUGS_SETTING_PRESCRIPTIONFONT ).toString());

    QString ALD, nonALD;
    QString tmp;
    // Add drugs
    int i;
    QString drugStyle, prescrStyle;
    drugStyle = tkGlobal::fontToHtml(drugsFont, "black");
    prescrStyle = tkGlobal::fontToHtml(prescrFont, "black");// + "margin-left:20px;";
    for(i=0; i < rowCount(); ++i) {
        tmp = QString(ENCODEDHTML_DRUG);
        tmp.replace( "{NUMBER}", QString::number(i+1));
        tmp.replace( "{DRUGSTYLE}", drugStyle);
//        if (index(i, Prescription::IsINNPrescription).data().toBool()) {
//            mfDrugs *drug = d->m_DrugsList.at(i);
//            QString name = drug->listOfInnWithPosology().join(" ; ");
//            tmp.replace( "{DRUG}", name );
//        } else {
            tmp.replace( "{DRUG}", index( i, Drug::Denomination ).data().toString());
//        }
        tmp.replace( "{PRESCRIPTIONSTYLE}", prescrStyle );
        tmp.replace( "{PRESCRIPTION}", index( i, Prescription::ToHtml ).data().toString());
//        tmp.replace( "{NOTE}", index( i, Prescription::Note).data().toString());

        if (index( i, Prescription::IsALD ).data().toBool()) {
            ALD += tmp;
        } else {
            nonALD += tmp;
        }
        tmp.clear();
    }
    if (!ALD.isEmpty()) {
        tmp = s->value( MFDRUGS_SETTING_ALD_PRE_HTML ).toString();
        tmp += QString(ENCODEDHTML_FULLPRESCRIPTION).replace( "{FULLPRESCRIPTION}", ALD );
        tmp += s->value( MFDRUGS_SETTING_ALD_POST_HTML ).toString();
    }
    if (!nonALD.isEmpty()) {
        tmp += QString(ENCODEDHTML_FULLPRESCRIPTION).replace( "{FULLPRESCRIPTION}", nonALD );
    }

    QString toReturn;
    toReturn = QString(ENCODEDHTML_FULLDOC);
    toReturn.replace("{GENERATOR}", qApp->applicationName());
    toReturn.replace("{PRESCRIPTION}", tmp );
    toReturn.replace("{ENCODEDPRESCRIPTION}", QString("%1%2")
                     .arg(ENCODEDHTML_DRUGSINTERACTIONSTAG)
                     .arg( QString(serializePrescription().toAscii().toBase64())) );
    return toReturn;
}

QString mfDrugsModel::serializePrescription()
{
    QString tmp;
    QList<int> keysToSave;
    keysToSave << Prescription::IntakesFrom << Prescription::IntakesTo
            << Prescription::IntakesScheme << Prescription::IntakesUsesFromAndTo << Prescription::DurationFrom
            << Prescription::DurationTo << Prescription::DurationScheme << Prescription::DurationUsesFromAndTo
            << Prescription::Period << Prescription::PeriodScheme <<  Prescription::MealTimeSchemeIndex
            << Prescription::Note << Prescription::IsINNPrescription << Prescription::SpecifyForm
            << Prescription::SpecifyPresentation << Prescription::IsALD;
    QHash<QString, QString> serializeIt;
    int i;
    for(i=0; i<rowCount() ; ++i) {
        serializeIt.insert( "CIS", index(i, Drug::CIS).data().toString() );
        foreach(int k, keysToSave) {
            serializeIt.insert( Prescription::PRESCRIPTION_CODES[k-Prescription::Id], index(i, k).data().toString() );
        }
        tmp += "[Drug]" + tkSerializer::threeCharKeyHashToString(serializeIt);
    }
    return tmp;
}

void mfDrugsModel::deSerializePrescription( const QString &serialized, PrescriptionDeserializer z )
{
    int row = 0;
    QStringList drugs = serialized.split("[Drug]", QString::SkipEmptyParts );
    // clear model
    if (z==ReplacePrescription)
        clearDrugsList();
    // rebuild model with serialized prescription
    foreach( const QString &s, drugs) {
        QHash<QString, QString> hash = tkSerializer::threeCharKeyHashToHash(s);
        if (hash.isEmpty())
            continue;
        this->addDrug( hash.value( "CIS" ).toInt(), false );
        hash.remove("CIS");
        foreach(const QString &i, hash.keys()) {
            setData( index(row, Prescription::PRESCRIPTION_CODES.indexOf(i) + Prescription::Id ), hash.value(i) );
        }
        ++row;
    }
    checkInteractions();
    reset();
}

void mfDrugsModel::checkInteractions() const
{
    mfDrugsBase::instance()->interactions( d->m_DrugsList );
}
