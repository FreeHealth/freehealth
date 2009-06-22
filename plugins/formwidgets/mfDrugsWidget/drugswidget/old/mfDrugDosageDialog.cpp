/***************************************************************************
 *   FreeMedForms Project                                                  *
 *   Copyright (C) 2008-2009 by Eric MAEKER, MD                            *
 *   eric.maeker@free.fr                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "mfDrugDosageDialog.h"

// include drugwidget headers
#include "drugsmodel/mfDrugDosage.h"
#include "drugsmodel/mfDrugs.h"

// include toolkit
#include <tkLog.h>

// include Qt headers
#include <QHeaderView>
#include <QRadioButton>
#include <QCheckBox>

mfDrugDosageDialog::mfDrugDosageDialog( QWidget * parent, int CIS_id, mfDrugDosage* d ) :
        QDialog( parent )
{
    setObjectName( "mfDrugDosageDialog" );

    // test existing drug dosage --> create an empty one if necessary
    if ( ! d )
    {
        m_Dosage = new mfDrugDosage();
        m_Dosage->setDrugCIS( CIS_id );
    }
    else
    {
        m_Dosage = d;
        setDosage( m_Dosage );
    }

    setupUi( this );
    setWindowTitle( tr( "Drug Dosage Creator" ) + " - " + qApp->applicationName() );
    drugNameLabel->setText( m_Dosage->drug()->denomination() );
    hourlyTableWidget->verticalHeader()->hide();
    hourlyTableWidget->horizontalHeader()->hide();

    // populate Ui with static datas of mfDrugDosage
    // populate combos with drugdosage stringlist
    intakeTimeSchemeCombo->clear();
    intervalTimeSchemeCombo->clear();
    durationTimeSchemeCombo->clear();
    mealTimeCombo->clear();
    scoredTabletScheme->clear();
    intakeTimeSchemeCombo->addItems( mfDrugDosage::periods() );
    intervalTimeSchemeCombo->addItems( mfDrugDosage::periods() );
    durationTimeSchemeCombo->addItems( mfDrugDosage::periods() );
    mealTimeCombo->addItems( mfDrugDosage::mealTime() );
    scoredTabletScheme->addItems( mfDrugDosage::scoredTabletScheme() );

    // create dailySchemes chkButtons
    int i = 0;
    foreach( QString s, mfDrugDosage::dailyScheme() )
    {
        QCheckBox * r = new QCheckBox( s, intakeSchemeGroup );
        intakeSchemeHorizLayout->addWidget( r );
        i++;
    }

    // resize table widget
    show();
    resizeTableWidget();

    // TODO : create pregnancy and breast feeding radios

    tkLog::addMessage( this, tr( "Showing dosage dialog for drug : %1" ).arg( "???" ) );
}


void mfDrugDosageDialog::setDosage( mfDrugDosage* d )
{
    //    if ( !d )
    //         return;
    m_Dosage = d;

    // label
    labelLineEdit->setText( d->label() );

    // intakes
    intakeTimeSchemeCombo->setCurrentIndex( d->intakesTimeScheme() );
    minIntakesSlider->setValue( d->minIntakes() );
    maxIntakesSlider->setValue( d->maxIntakes() );
    minIntervalIntakesSlider->setValue( d->intakesInterval() );
    intervalTimeSchemeCombo->setCurrentIndex( d->intakesIntervalScheme() );

    // duration
    durationTimeSchemeCombo->setCurrentIndex( 0 );
    minDurationSlider->setValue( 1 );
    maxDurationSlider->setValue( 10 );
    recommendedDurationSlider->setValue( 1 );
    durationTimeModulationCombo->setCurrentIndex( 1 );

    // extras
    noteTextEdit->setText( "a try " );
}

mfDrugDosage * mfDrugDosageDialog::getDosage()
{
    mfDrugDosage * d = new mfDrugDosage( qApp );
    d->setValue( mfDrugsTables::DOSE_CIS , 324432 );

    if ( labelLineEdit->text().isEmpty() )
        d->setLabel( tr( "User defined dosage" ) );
    else
        d->setLabel( labelLineEdit->text() );

//    d->setIntakesPeriodRange( minIntakesSlider->value(), maxIntakesSlider->value(), intakeTimeSchemeCombo->currentIndex() );
    d->setNumberOfIntakesRange( minIntakesSlider->value(), maxIntakesSlider->value() );
    d->setHourlyIntakesScheme( "hours" );
    d->setDailyIntakesScheme( 1 );
    /*
    d->setMealIntakesScheme(  );
    d->setIntakesIntervalBetweenTwo( minIntervalIntakesSlider, intervalTimeSchemeCombo );
    d->setDurationRange( int min, int max, int recommended, Period scheme );
    d->setTimeDurationModulator( const QString & e );
    d->setNote( const QString & n );

*/
    return d;
}

void mfDrugDosageDialog::resizeTableWidget()
{
    int i = 0;
    int size = ( ( hourlyTableWidget->size().width() - style()->pixelMetric( QStyle::PM_DefaultFrameWidth ) ) / 8 );
    for( i = 0; i < 8; i++ )
        hourlyTableWidget->setColumnWidth( i, size );
}

void mfDrugDosageDialog::resizeEvent ( QResizeEvent * event )
{
    resizeTableWidget();
    QWidget::resizeEvent( event );
}

void mfDrugDosageDialog::on_tabWidget_currentChanged( int index )
{
    // this function is used to resize the hourly table widget
    if ( index == 1 )
        resizeTableWidget();
}


// Manage mins and max of the sliders
void mfDrugDosageDialog::on_minIntakesSlider_valueChanged ( int value )
{
    // min should always be <= to max
    if ( value > maxIntakesSlider->value() )
        minIntakesSlider->setValue( maxIntakesSlider->value() );
}

void mfDrugDosageDialog::on_maxIntakesSlider_valueChanged ( int value )
{
    // max should always be >= to min
    if ( value < minIntakesSlider->value() )
        maxIntakesSlider->setValue( minIntakesSlider->value() );
}

void mfDrugDosageDialog::on_minDurationSlider_valueChanged( int value )
{
    // min should always be <= to max
    if ( value > maxDurationSlider->value() )
        minDurationSlider->setValue( maxDurationSlider->value() );
}

void mfDrugDosageDialog::on_maxDurationSlider_valueChanged( int value )
{
    // max should always be >= to min
    if ( value < minDurationSlider->value() )
        maxDurationSlider->setValue( minDurationSlider->value() );
}

