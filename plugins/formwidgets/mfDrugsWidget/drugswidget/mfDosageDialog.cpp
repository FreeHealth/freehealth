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
#include "mfDosageDialog.h"

// include drugwidget headers
#include <mfDrugsConstants.h>
#include <drugsmodel/mfDosageModel.h>
#include <drugsmodel/mfDrugsModel.h>
#include <drugswidget/mfDrugInfo.h>

// include toolkit
#include <tkLog.h>
#include <tkStringListModel.h>
#include <tkSettings.h>
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
 \todo when showing dosage, make verification of limits +++  ==> for FMF only
 \todo use a QPersistentModelIndex instead of drugRow, dosageRow
*/
// **************************************************************************************************************
// TODO : with FreeMedForms check compatibility between patient parameters and dosage (weight, age, clearance...)
// **************************************************************************************************************

mfDosageDialog::mfDosageDialog( QWidget *parent, const int drugRow, const int dosageRow )
    : QDialog( parent ),
    m_Mapper(0),
    m_DosageModel(0),
    m_DrugRow(drugRow),
    m_DosageRow(dosageRow),
    m_UserFormButtonPopup(0)
{
    setObjectName( "mfDosageDialog" );
    setupUi(this);
    setWindowTitle( tr( "Drug Dosage" ) + " - " + qApp->applicationName() );

    // hide what should be hidden , set some icons
    dosageSelectorLabel->hide();
    dosageSelectorCombo->hide();
    but_ModifyDosage->hide();
    but_CreateDosage->hide();
    userformsButton->setIcon( tkTheme::icon(ICONEDIT) );

    // Prepare some combos
    durationCombo->clear();
    durationCombo->addItems( mfDosageModel::periods() );
    durationCombo->setCurrentIndex( mfDosageModel::periodDefault()+2 );
    periodSchemeCombo->clear();
    periodSchemeCombo->addItems( mfDosageModel::periods() );
    periodSchemeCombo->setCurrentIndex( mfDosageModel::periodDefault() );
    mealTimeCombo->clear();
    mealTimeCombo->addItems( mfDosageModel::mealTime() );

    // Prepare dailyScheme
    tkStringListModel * stringModel = new tkStringListModel( this );
    stringModel->setStringList( mfDosageModel::dailyScheme() );
    stringModel->setCheckable(true);
    dailySchemeListView->setModel( stringModel );
    dailySchemeListView->hideButtons();
    dailySchemeListView->useContextMenu(false);

    // Prepare mapper
    m_Mapper = new QDataWidgetMapper(this);
    m_Mapper->setModel( mfDrugsModel::instance() );
    m_Mapper->addMapping( intakesFromSpin, Prescription::IntakesFrom, "value" );
    m_Mapper->addMapping( intakesToSpin, Prescription::IntakesTo, "value" );
    m_Mapper->addMapping( durationFromSpin, Prescription::DurationFrom, "value" );
    m_Mapper->addMapping( durationToSpin, Prescription::DurationTo, "value" );
    m_Mapper->addMapping( intakesCombo, Prescription::IntakesScheme, "currentText" );
    m_Mapper->addMapping( durationCombo, Prescription::DurationScheme, "currentText" );
    m_Mapper->addMapping( periodSpin, Prescription::Period, "value" );
    m_Mapper->addMapping( periodSchemeCombo, Prescription::PeriodScheme, "currentText" );
    m_Mapper->addMapping( mealTimeCombo, Prescription::MealTimeSchemeIndex, "currentIndex" );
    m_Mapper->addMapping( noteTxtEdit, Prescription::Note, "plainText" );
    m_Mapper->addMapping( dailySchemeListView, Prescription::DailyScheme, "checkedStringList" );
    m_Mapper->setSubmitPolicy( QDataWidgetMapper::AutoSubmit );

    // make connections
    connect( mfDrugsModel::instance(), SIGNAL(prescriptionResultChanged(const QString &)),
             resultTextBrowser, SLOT(setPlainText(const QString &)));
    changeRow( drugRow, dosageRow );
}

mfDosageDialog::~mfDosageDialog()
{
    m_Mapper->clearMapping();
}

void mfDosageDialog::changeRow( const int drugRow, const int dosageRow )
{
    m_DrugRow = drugRow;
    m_DosageRow = dosageRow;
    mfDrugsModel *drugM = mfDrugsModel::instance();
    //    m_DosageModel = drugM->dosageModel( mfDrugsModel::instance()->index(drugRow, Drug::CIS).data().toInt() );

    // retreive drug informations before drugmodel changes
    QString name = drugM->index( drugRow, Drug::Denomination).data().toString();
    drugNameButton->setText( name.left( name.lastIndexOf(",") ) );
    QString toolTip = drugM->index( drugRow, Interaction::ToolTip ).data().toString();
    iconInteractionLabel->setToolTip( toolTip );
    iconInteractionLabel->setPixmap( drugM->index( drugRow, Interaction::Icon ).data().value<QIcon>().pixmap(16,16) );
    toolTip = drugM->index( drugRow , Drug::CompositionString ).data().toString();
    drugNameButton->setToolTip( toolTip );
    toogleInnButton->setEnabled( drugM->index( drugRow , Drug::AllInnsKnown ).data().toBool() );

    tabWidget->setCurrentWidget(tabIntakes);
    intakesCombo->clear();
    intakesCombo->addItems( drugM->index( m_DrugRow, Drug::AvailableForms ).data().toStringList() );
    periodSpin->setValue( 1 );
    noteTxtEdit->clear();

    if (!drugM->index( m_DrugRow, Drug::HasPrescription ).data().toBool())
        createDefaultDosage();
    m_Mapper->setCurrentIndex( m_DrugRow );
    durationCombo->setCurrentIndex( mfDosageModel::periodDefault()+2 );
    periodSchemeCombo->setCurrentIndex( mfDosageModel::periodDefault() );
    modelToCheckBoxes();
}

void mfDosageDialog::modelToCheckBoxes()
{
    mfDrugsModel *drugM = mfDrugsModel::instance();
    if (drugM->index( m_DrugRow, Prescription::IntakesUsesFromAndTo).data().toBool()) {
        fromToIntakesCheck->setCheckState( Qt::Checked );
        intakesFromLabel->show();
        intakesToLabel->show();
        intakesToSpin->show();
    } else {
        fromToIntakesCheck->setCheckState( Qt::Unchecked );
        intakesFromLabel->hide();
        intakesToLabel->hide();
        intakesToSpin->hide();
    }
    if (drugM->index( m_DrugRow, Prescription::DurationUsesFromAndTo).data().toBool()) {
        fromToDurationCheck->setCheckState( Qt::Checked );
        durationFromLabel->show();
        durationToLabel->show();
        durationToSpin->show();
    } else {
        fromToDurationCheck->setCheckState( Qt::Unchecked );
        durationFromLabel->hide();
        durationToLabel->hide();
        durationToSpin->hide();        
    }

    if (drugM->index( m_DrugRow, Prescription::IsINNPrescription).data().toBool())
        INNCheck->setCheckState( Qt::Checked );
    else
        INNCheck->setCheckState( Qt::Unchecked );
    if (drugM->index( m_DrugRow, Prescription::IsALD).data().toBool())
        ALDCheck->setCheckState( Qt::Checked );
    else
        ALDCheck->setCheckState( Qt::Unchecked );
    if (!drugM->index( m_DrugRow, Prescription::SpecifyForm).data().toBool())
        noFormCheck->setCheckState( Qt::Checked );
    else
        noFormCheck->setCheckState( Qt::Unchecked );
    if (!drugM->index( m_DrugRow, Prescription::SpecifyPresentation).data().toBool())
        noPresentationCheck->setCheckState( Qt::Checked );
    else
        noPresentationCheck->setCheckState( Qt::Unchecked );
}

void mfDosageDialog::createDefaultDosage()
{
    mfDrugsModel *drugM = mfDrugsModel::instance();
    drugM->setData( drugM->index( m_DrugRow, Prescription::IntakesFrom), 1 );
    drugM->setData( drugM->index( m_DrugRow, Prescription::IntakesTo), 1 );
    drugM->setData( drugM->index( m_DrugRow, Prescription::IntakesUsesFromAndTo), false );
    drugM->setData( drugM->index( m_DrugRow, Prescription::DurationFrom), 1 );
    drugM->setData( drugM->index( m_DrugRow, Prescription::DurationTo), 1 );
    drugM->setData( drugM->index( m_DrugRow, Prescription::DurationUsesFromAndTo), false );
    drugM->setData( drugM->index( m_DrugRow, Prescription::IntakesScheme), intakesCombo->itemText(0) );
    drugM->setData( drugM->index( m_DrugRow, Prescription::DurationScheme), mfDosageModel::period(mfDosageModel::periodDefault()+2) );
    drugM->setData( drugM->index( m_DrugRow, Prescription::Period), 1 );
    drugM->setData( drugM->index( m_DrugRow, Prescription::PeriodScheme), mfDosageModel::period(mfDosageModel::periodDefault()) );
    drugM->setData( drugM->index( m_DrugRow, Prescription::MealTimeSchemeIndex), QVariant() );
    drugM->setData( drugM->index( m_DrugRow, Prescription::Note), QVariant() );
    drugM->setData( drugM->index( m_DrugRow, Prescription::IsINNPrescription), false );
    drugM->setData( drugM->index( m_DrugRow, Prescription::SpecifyForm), true );
    drugM->setData( drugM->index( m_DrugRow, Prescription::SpecifyPresentation), true );
    drugM->setData( drugM->index( m_DrugRow, Prescription::IsALD), false );
}

void mfDosageDialog::done( int r )
{
    if ( r == QDialog::Accepted ) {
        // test intakes forms
        const QStringList &pre = mfDosageModel::predeterminedForms();
        const QStringList &av  = mfDrugsModel::instance()->index(m_DrugRow,Drug::AvailableForms).data().toStringList();
        if (( pre.indexOf(intakesCombo->currentText()) == -1 ) &&
            ( av.indexOf(intakesCombo->currentText()) == -1 )) {
            tkSettings::instance()->appendToValue( MFDRUGS_SETTING_USERRECORDEDFORMS, intakesCombo->currentText() );
        }
    }
    QDialog::done(r);
}

void mfDosageDialog::availableDosages_activated( const QModelIndex & item )
{
    // change current index of mapper
    int row = item.row();
    if ( m_ActualDosageUuid == m_DosageModel->index( row, Dosage::Uuid ).data().toString() ) {
        return;
    } else {
        m_ActualDosageUuid = m_DosageModel->index( row, Dosage::Uuid ).data().toString();
        m_DosageRow = row;
    }
    // prepare ui before changing the view
    //    if (stackedWidget->currentWidget() == dosageCreatorWidget ) {
    //        prepareMapperDosageCreator( row );
    //    } else {
    //        preparePrescriptionWidget();
    //    }
}

void mfDosageDialog::setPrescriptionState( const int index, const int qtCheckState )
{
    mfDrugsModel *drugM = mfDrugsModel::instance();
    if (qtCheckState==Qt::Checked)
        drugM->setData( drugM->index( m_DrugRow, index), true );
    else
        drugM->setData( drugM->index( m_DrugRow, index), false );
}

void mfDosageDialog::on_drugNameButton_clicked()
{
    mfDrugInfo dialog( m_DrugRow, this );
    dialog.exec();
}

void mfDosageDialog::on_fromToIntakesCheck_stateChanged( int state )
{
    setPrescriptionState( Prescription::IntakesUsesFromAndTo, state );
}

void mfDosageDialog::on_fromToDurationCheck_stateChanged( int state )
{
    setPrescriptionState( Prescription::DurationUsesFromAndTo, state );
}

void mfDosageDialog::on_ALDCheck_stateChanged( int state )
{
    setPrescriptionState( Prescription::IsALD, state );
}

void mfDosageDialog::on_INNCheck_stateChanged( int state )
{
    setPrescriptionState( Prescription::IsINNPrescription, state );
}

void mfDosageDialog::on_noFormCheck_stateChanged( int state )
{
    if (state==Qt::Checked)
        setPrescriptionState( Prescription::SpecifyForm, Qt::Unchecked );
    else
        setPrescriptionState( Prescription::SpecifyForm, Qt::Checked );
}

void mfDosageDialog::on_noPresentationCheck_stateChanged( int state )
{
    if (state==Qt::Checked)
        setPrescriptionState( Prescription::SpecifyPresentation, Qt::Unchecked );
    else
        setPrescriptionState( Prescription::SpecifyPresentation, Qt::Checked );
}

void mfDosageDialog::on_intakesFromSpin_valueChanged( double d  )
{
    if (intakesToSpin->value() < d) {
        intakesToSpin->setValue( d );
    }
    intakesToSpin->setMinimum(d);
}

void mfDosageDialog::on_durationFromSpin_valueChanged( double d  )
{
    if (durationToSpin->value() < d) {
        durationToSpin->setValue( d );
    }
    durationToSpin->setMinimum(d);
}

void mfDosageDialog::on_userformsButton_clicked()
{
    if (tkSettings::instance()->value(MFDRUGS_SETTING_USERRECORDEDFORMS).isNull())
        return;

    const QStringList &ulist = tkSettings::instance()->value(MFDRUGS_SETTING_USERRECORDEDFORMS).toStringList();
    QList<QAction*> list;
    foreach( const QString &form, ulist ) {
        if (!form.isEmpty())
            list << new QAction(form, this);
    }
    QAction *aclear = new QAction(tr("Clear this list", "Clear the user's intakes recorded forms"), this);
    list << aclear;

    QAction *a = QMenu::exec(list, userformsButton->mapToGlobal(QPoint(0,20)) );
    if (!a)
        return;
    if (a == aclear) {
        tkSettings::instance()->setValue(MFDRUGS_SETTING_USERRECORDEDFORMS, QString() );
    } else {
        intakesCombo->setEditText( a->text() );
    }
}

void mfDosageDialog::on_toogleInnButton_clicked()
{
    // get from model the list of substituable drugs
    // show inside a QMenu
}
