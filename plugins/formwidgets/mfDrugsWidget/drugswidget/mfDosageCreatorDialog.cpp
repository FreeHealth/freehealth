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
#include "mfDosageCreatorDialog.h"

// include drugwidget headers
//#include <drugsmodel/mfDosage.h>
#include <drugsmodel/mfDrugs.h>
#include <drugsmodel/mfDosageModel.h>
#include <drugsmodel/mfDrugsModel.h>
#include <mfDrugsConstants.h>

// include toolkit
#include <tkLog.h>
#include <tkStringListModel.h>
#include <tkTheme.h>

// include Qt headers
#include <QHeaderView>
#include <QRadioButton>
#include <QCheckBox>
#include <QTableWidget>
#include <QSpinBox>
#include <QDataWidgetMapper>
#include <QMessageBox>

using namespace mfDrugsConstants;
using namespace mfDosagesConstants;
using namespace mfInteractionsConstants;

Q_TK_USING_CONSTANTS

/**
  \brief Private part
  \internal
*/
class mfDosageCreatorDialogPrivate
{
public:
    mfDosageCreatorDialogPrivate(mfDosageCreatorDialog *parent) :
            m_Mapper(0), m_DosageModel(0), m_Parent(parent) {}

    
    /** \brief Insert value into index (row, colum) of dosage model if checkbox is check and return true. Otherwise, insert a QVariant and return false. */
    bool checkToDosageModel( const int row, const int column, QCheckBox *b, const QVariant &value )
    {
        QModelIndex idx = m_DosageModel->index( row, column );
        if ( b->isChecked() ) {
            m_DosageModel->setData( idx, value );
            return true;
        }
        else
            m_DosageModel->setData( idx, QVariant() );
        return false;
    }

    /** \brief Check the checkbox if index return a non empty QVariant, set combo current index to the value and return true. Return false otherwise.*/
    bool dosageModelToCheckAndCombo( const int row, const int column, QCheckBox *b, QComboBox *cb )
    {
        QModelIndex idx = m_DosageModel->index( row, column );
        if (!idx.data().isNull() ) {
            b->setChecked(true);
            cb->setCurrentIndex( idx.data().toInt() );
            return true;
        } else {
            b->setChecked(false);
            cb->setCurrentIndex(0);
        }
        return false;
    }

    /** \brief Check the checkbox if index return a non empty QVariant, set combo current index to the value and return true. Return false otherwise.*/
    bool dosageModelToCheckAndCombo( const int row, const int column, QCheckBox *b, QSpinBox *cb )
    {
        QModelIndex idx = m_DosageModel->index( row, column );
        if (!idx.data().isNull() ) {
            b->setChecked(true);
            cb->setValue( idx.data().toInt() );
            return true;
        } else {
            b->setChecked(false);
            cb->setValue(0);
        }
        return false;
    }

    /** \brief Commits dosage model using a transaction to database */
    void commitModelToDatabase()
    {
        m_DosageModel->database().transaction();
        if ( m_DosageModel->submitAll() ) {
            m_DosageModel->database().commit();
        } else {
            m_DosageModel->database().rollback();
            QMessageBox::warning(m_Parent, m_Parent->tr("Drug Dosage Creator"),
                                 m_Parent->tr("ERROR SQL : The database reported an error: %1")
                                 .arg(m_DosageModel->database().lastError().text()));
        }
    }


public:
    QDataWidgetMapper  *m_Mapper;
    mfDosageModel      *m_DosageModel;
    QString             m_ActualDosageUuid;
    int                 m_CIS;
private:
    mfDosageCreatorDialog *m_Parent;
};



/**
 \todo when showing dosage, make verification of limits +++  ==> for FMF only
 \todo use a QPersistentModelIndex instead of drugRow, dosageRow
*/
mfDosageCreatorDialog::mfDosageCreatorDialog( QWidget *parent, const int CIS, mfDosageModel *dosageModel )
    : QDialog( parent ),
    d(0)
{
    // some initializations
    mfDrugsModel *drugM = mfDrugsModel::instance();
    setObjectName( "mfDosageCreatorDialog" );
    d = new mfDosageCreatorDialogPrivate(this);
    d->m_DosageModel = dosageModel;
    d->m_CIS = CIS;

    // Ui initialization
    setupUi(this);
    setWindowTitle( tr( "Drug Dosage Creator" ) + " - " + qApp->applicationName() );
    userformsButton->setIcon( tkTheme::icon(ICONEDIT) );
    intakesToLabel->hide();
    intakesToSpin->hide();
    durationToLabel->hide();
    durationToSpin->hide();

    // this must be done here
    tkStringListModel * stringModel = new tkStringListModel( this );
    stringModel->setStringList( d->m_DosageModel->dailyScheme() );
    stringModel->setCheckable(true);
    dailySchemeListView->setModel( stringModel );
    dailySchemeListView->hideButtons();

    // retreive drug informations before drugmodel changes
    drugNameLabel->setText( drugM->drugData( d->m_CIS, Drug::Denomination).toString() );
    labelOfDosageLabel->setToolTip( drugM->drugData( d->m_CIS, Drug::AvailableDosages).toString() );
    QString toolTip = drugM->drugData( d->m_CIS, Interaction::ToolTip ).toString();
//    iconInteractionLabel->setToolTip( toolTip );
//    iconInteractionLabel->setPixmap( drugM->index( drugRow, Interaction::Icon ).data().value<QIcon>().pixmap(16,16) );
    toolTip = drugM->drugData( d->m_CIS, Drug::CompositionString ).toString();
    drugNameLabel->setToolTip( toolTip );

    resetUiToDefaults();
}

void mfDosageCreatorDialog::prepareMapper( const int dosageRow )
{
    // mapper preparation
    if ( ! d->m_Mapper ) {
        d->m_Mapper = new QDataWidgetMapper( this );
        d->m_Mapper->setModel( d->m_DosageModel );
        d->m_Mapper->setSubmitPolicy( QDataWidgetMapper::ManualSubmit );
        d->m_Mapper->addMapping( labelLineEdit, Dosage::Label, "text" );
        d->m_Mapper->addMapping( intakesCombo, Dosage::SelectedForm, "currentText" );

        d->m_Mapper->addMapping( intakesFromSpin, Dosage::IntakeFrom, "value" );
        d->m_Mapper->addMapping( intakesToSpin, Dosage::IntakeTo, "value" );
        d->m_Mapper->addMapping( intakesCombo, Dosage::SelectedForm, "currentText" );
//        d->m_Mapper->addMapping( minNumberIntakesByUnit, Dosage::MinIntakePerUnit, "value" );
//        d->m_Mapper->addMapping( maxNumberIntakesByUnit, Dosage::MaxIntakePerUnit, "value" );
//        d->m_Mapper->addMapping( unitCombo, Dosage::SelectedUnit, "currentText" );
//        d->m_Mapper->addMapping( unitPerCombo, Dosage::UnitDivisor, "currentText" );
        d->m_Mapper->addMapping( periodSchemeCombo, Dosage::IntakeScheme, "currentIndex" );

        d->m_Mapper->addMapping( durationFromSpin, Dosage::DurationFrom );
        d->m_Mapper->addMapping( durationToSpin, Dosage::DurationTo );
        d->m_Mapper->addMapping( durationCombo, Dosage::DurationScheme, "currentIndex"  );
//        d->m_Mapper->addMapping( minIntervalIntakesSpin, Dosage::IntakesIntervalOfTime );
//        d->m_Mapper->addMapping( intervalTimeSchemeCombo, Dosage::IntakesIntervalScheme, "currentIndex"  );
        d->m_Mapper->addMapping( mealTimeCombo, Dosage::MealScheme, "currentIndex" );
        d->m_Mapper->addMapping( noteTextEdit, Dosage::Note, "plainText" );
        d->m_Mapper->addMapping( dailySchemeListView, Dosage::DailyScheme, "checkedStringList" );
    }

    // manage non mapped datas
    QModelIndex idx;
    d->dosageModelToCheckAndCombo( dosageRow, Dosage::MinClearance, minClearanceCheck, minClearanceSpin );
    d->dosageModelToCheckAndCombo( dosageRow, Dosage::MaxClearance, maxClearanceCheck, maxClearanceSpin );
    d->dosageModelToCheckAndCombo( dosageRow, Dosage::SexLimitedIndex, sexLimitCheck, sexLimitCombo );
    if (d->dosageModelToCheckAndCombo( dosageRow, Dosage::MinAge, minAgeCheck, minAgeSpin )) {
        idx = d->m_DosageModel->index( dosageRow, Dosage::MinAgeReferenceIndex );
        minAgeCombo->setCurrentIndex( idx.data().toInt() );
    }
    if (d->dosageModelToCheckAndCombo( dosageRow, Dosage::MaxAge, maxAgeCheck, maxAgeSpin )) {
        idx = d->m_DosageModel->index( dosageRow, Dosage::MaxAgeReferenceIndex );
        maxAgeCombo->setCurrentIndex( idx.data().toInt() );
    }
    d->m_Mapper->setCurrentIndex( dosageRow );
}

void mfDosageCreatorDialog::resetUiToDefaults()
{
    mfDrugsModel *drugM = mfDrugsModel::instance();
    // some ui preparations
    periodSchemeCombo->clear();
//    intervalTimeSchemeCombo->clear();
    durationCombo->clear();
    mealTimeCombo->clear();
//    scoredTabletScheme->clear();
    hourlyTableWidget->verticalHeader()->hide();
    hourlyTableWidget->horizontalHeader()->hide();
    hourlyTableWidget->resizeColumnsToContents();
    // populate combos
//    formSchemeCombo->clear();
//    formSchemeCombo->addItems( drugM->index( m_DrugRow, Drug::AvailableForms ).data().toStringList() );
    bool isScored = drugM->drugData( d->m_CIS, Drug::IsScoredTablet ).toBool();
    if ( isScored ) {
//        scoredTabletScheme->setEnabled( true );
        intakesToSpin->setDecimals( 2 );
        intakesFromSpin->setDecimals( 2 );
        intakesToSpin->setSingleStep( 0.25 );
        intakesFromSpin->setSingleStep( 0.25 );
    } else {
//        scoredTabletScheme->setEnabled( false );
        durationToSpin->setDecimals( 0 );
        durationFromSpin->setDecimals( 0 );
        durationToSpin->setSingleStep( 1 );
        durationFromSpin->setSingleStep( 1 );
    }
    // set periods
    periodSchemeCombo->addItems( mfDosageModel::periods() );
    intervalTimeSchemeCombo->addItems( mfDosageModel::periods() );
    durationCombo->addItems( mfDosageModel::periods() );
    periodSchemeCombo->setCurrentIndex( mfDosageModel::periodDefault() );
    intervalTimeSchemeCombo->setCurrentIndex( mfDosageModel::periodDefault() );
    durationCombo->setCurrentIndex( mfDosageModel::periodDefault() );
    mealTimeCombo->addItems( mfDosageModel::mealTime() );

    resizeTableWidget();
}

void mfDosageCreatorDialog::done( int r )
{
    int row;
    if (d->m_Mapper)
        row =  d->m_Mapper->currentIndex();
    else
        QDialog::done(r);

    if ( r == QDialog::Rejected ) {
        d->m_DosageModel->revertRow( row );
    }  else if ( d->m_Mapper ) {

        // ******************************************************
        // * TODO check validity of the dosage before submition *
        // ******************************************************
        //        if ( m_DosageModel->isDosageValid( row ) ) {
        // submit mapper to model
        d->m_Mapper->submit();

        // manage non mapped datas (sexlimit, weightlimit...)
        QModelIndex idx;

        d->checkToDosageModel( row, Dosage::SexLimitedIndex, sexLimitCheck , sexLimitCombo->currentIndex() );
        d->checkToDosageModel( row, Dosage::MinWeight, minWeightCheck , minWeightSpin->value() );
        d->checkToDosageModel( row, Dosage::MinClearance, minClearanceCheck , minClearanceSpin->value() );
        d->checkToDosageModel( row, Dosage::MaxClearance, maxClearanceCheck , maxClearanceSpin->value() );
        if (d->checkToDosageModel( row, Dosage::MinAge, minWeightCheck , minAgeSpin->value() )) {
            idx = d->m_DosageModel->index( row, Dosage::MinAgeReferenceIndex );
            d->m_DosageModel->setData( idx, minAgeCombo->currentIndex() );
        }
        if (d->checkToDosageModel( row, Dosage::MaxAge, maxAgeCheck , maxAgeSpin->value() )) {
            idx = d->m_DosageModel->index( row, Dosage::MaxAgeReferenceIndex );
            d->m_DosageModel->setData( idx, maxAgeCombo->currentIndex() );
        }

        // commit model changes to database
        d->commitModelToDatabase();
    } else
        QDialog::done(QDialog::Rejected);
    QDialog::done(r);
}

/** \brief Resize hourly table widget on view */
void mfDosageCreatorDialog::resizeTableWidget()
{
    int i = 0;
    int size = ( ( hourlyTableWidget->size().width() - style()->pixelMetric( QStyle::PM_DefaultFrameWidth ) ) / 8 );
    for( i = 0; i < 8; i++ )
        hourlyTableWidget->setColumnWidth( i, size );
}

/** \brief Used for hourly table widget resizing */
void mfDosageCreatorDialog::resizeEvent( QResizeEvent * event )
{
    resizeTableWidget();
    QWidget::resizeEvent( event );
}
