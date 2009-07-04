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
#include <tkConstantTranslations.h>

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
Q_TK_USING_TRANSLATIONS

class mfDosageDialogPrivate
{
public:
    mfDosageDialogPrivate() : m_DosageModel(0), m_CIS(-1), m_UserFormButtonPopup(0) {}

    mfDosageModel      *m_DosageModel;
    QString             m_ActualDosageUuid;
    int m_CIS, m_DosageRow;
    QMenu *m_UserFormButtonPopup;
};


/**
 \todo when showing dosage, make verification of limits +++  ==> for FMF only
*/
// **************************************************************************************************************
// TODO : with FreeMedForms check compatibility between patient parameters and dosage (weight, age, clearance...)
// **************************************************************************************************************

/**
  \brief Constructor
  \param parent : parent QWidget
  \param drugRow : row of the drug inside the actual model \todo Change to QPersistentModelIndex
  \param dosageRow : row of the dosage to use in the actual model \todo Change to QPersistentModelIndex
*/
mfDosageDialog::mfDosageDialog( QWidget *parent )
    : QDialog( parent ),
    d(0)
{
    setObjectName( "mfDosageDialog" );
    d = new mfDosageDialogPrivate();
    setupUi(this);
    innButton->setIcon( tkTheme::icon(MFDRUGS_ICONSEARCHDCI));
    setWindowTitle( tr( "Drug Dosage" ) + " - " + qApp->applicationName() );

    // make connections
    connect( mfDrugsModel::instance(), SIGNAL(prescriptionResultChanged(const QString &)),
             resultTextBrowser, SLOT(setPlainText(const QString &)));
}

/** \brief Destructor, frees mapper */
mfDosageDialog::~mfDosageDialog()
{
    if (d) delete d; d=0;
}

/**
  \brief Change the current row of the drug model
  \todo Manage dosagemodel
*/
void mfDosageDialog::changeRow( const int CIS, const int dosageRow )
{
    Q_ASSERT(mfDrugsModel::instance()->containsDrug(CIS));
    d->m_CIS = CIS;
    d->m_DosageRow = dosageRow;
    mfDrugsModel *m = mfDrugsModel::instance();
    dosageViewer->useDrugsModel(CIS, dosageRow);
    innButton->setChecked(m->drugData( d->m_CIS, Prescription::IsINNPrescription).toBool() );

    // retreive drug informations before drugmodel changes
    QString name = m->drugData(CIS, Drug::Denomination).toString();
    drugNameButton->setText(name.left( name.lastIndexOf(",")));
    QString toolTip = m->drugData(CIS, Interaction::ToolTip ).toString();
    iconInteractionLabel->setToolTip( toolTip );
    iconInteractionLabel->setPixmap( m->drugData(CIS, Interaction::Icon ).value<QIcon>().pixmap(16,16) );
    toolTip = m->drugData(CIS, Drug::CompositionString ).toString();
    drugNameButton->setToolTip( toolTip );
    innButton->setEnabled( m->drugData(CIS, Drug::AllInnsKnown ).toBool() );
}

///** \brief Creates an empty default dosage inside the model. The Ui should be updated with the mapper. */
//void mfDosageDialog::createDefaultDosage()
//{
//    mfDrugsModel *drugM = mfDrugsModel::instance();
//    drugM->setData( drugM->index( m_DrugRow, Prescription::IntakesFrom), 1 );
//    drugM->setData( drugM->index( m_DrugRow, Prescription::IntakesTo), 1 );
//    drugM->setData( drugM->index( m_DrugRow, Prescription::IntakesUsesFromTo), false );
//    drugM->setData( drugM->index( m_DrugRow, Prescription::DurationFrom), 1 );
//    drugM->setData( drugM->index( m_DrugRow, Prescription::DurationTo), 1 );
//    drugM->setData( drugM->index( m_DrugRow, Prescription::DurationUsesFromTo), false );
//    drugM->setData( drugM->index( m_DrugRow, Prescription::IntakesScheme), intakesCombo->itemText(0) );
//    drugM->setData( drugM->index( m_DrugRow, Prescription::DurationScheme), period(Time::Months));
//    drugM->setData( drugM->index( m_DrugRow, Prescription::Period), 1 );
//    drugM->setData( drugM->index( m_DrugRow, Prescription::PeriodScheme), period(Time::Days) );
//    drugM->setData( drugM->index( m_DrugRow, Prescription::MealTimeSchemeIndex), QVariant() );
//    drugM->setData( drugM->index( m_DrugRow, Prescription::Note), QVariant() );
//    drugM->setData( drugM->index( m_DrugRow, Prescription::IsINNPrescription), false );
//    drugM->setData( drugM->index( m_DrugRow, Prescription::SpecifyForm), true );
//    drugM->setData( drugM->index( m_DrugRow, Prescription::SpecifyPresentation), true );
//    drugM->setData( drugM->index( m_DrugRow, Prescription::IsALD), false );
//}

/**
  \brief Closes the dialog.
  \li If the dialog is accepted, retreive the prescribed form and store it into the settings is needed.
  \todo If the dialog is accepted and no dosage exists in the dosage model --> create a dosage in the dosage model
*/
void mfDosageDialog::done( int r )
{
//    if ( r == QDialog::Accepted ) {
//        // test intakes forms
//        const QStringList &pre = mfDosageModel::predeterminedForms();
//        const QStringList &av  = mfDrugsModel::instance()->drugData(CIS,Drug::AvailableForms).toStringList();
//        if (( pre.indexOf(intakesCombo->currentText()) == -1 ) &&
//            ( av.indexOf(intakesCombo->currentText()) == -1 )) {
//            tkSettings::instance()->appendToValue( MFDRUGS_SETTING_USERRECORDEDFORMS, intakesCombo->currentText() );
//        }
//    }
    QDialog::done(r);
}

//void mfDosageDialog::availableDosages_activated( const QModelIndex & item )
//{
//    // change current index of mapper
//    int row = item.row();
//    if ( m_ActualDosageUuid == m_DosageModel->index( row, Dosage::Uuid ).data().toString() ) {
//        return;
//    } else {
//        m_ActualDosageUuid = m_DosageModel->index( row, Dosage::Uuid ).data().toString();
//        m_DosageRow = row;
//    }
//    // prepare ui before changing the view
//    //    if (stackedWidget->currentWidget() == dosageCreatorWidget ) {
//    //        prepareMapperDosageCreator( row );
//    //    } else {
//    //        preparePrescriptionWidget();
//    //    }
//}
//
//void mfDosageDialog::setPrescriptionState( const int index, const int qtCheckState )
//{
//    mfDrugsModel *drugM = mfDrugsModel::instance();
//    if (qtCheckState==Qt::Checked)
//        drugM->setData( drugM->index( m_DrugRow, index), true );
//    else
//        drugM->setData( drugM->index( m_DrugRow, index), false );
//}

/** \brief Show the information dialog for the drug */
void mfDosageDialog::on_drugNameButton_clicked()
{
    mfDrugInfo dialog(d->m_CIS, this );
    dialog.exec();
}

//void mfDosageDialog::on_fromToIntakesCheck_stateChanged( int state )
//{
//    setPrescriptionState( Prescription::IntakesUsesFromTo, state );
//}
//
//void mfDosageDialog::on_fromToDurationCheck_stateChanged( int state )
//{
//    setPrescriptionState( Prescription::DurationUsesFromTo, state );
//}
//
//void mfDosageDialog::on_ALDCheck_stateChanged( int state )
//{
//    setPrescriptionState( Prescription::IsALD, state );
//}
//
//void mfDosageDialog::on_INNCheck_stateChanged( int state )
//{
//    setPrescriptionState( Prescription::IsINNPrescription, state );
//}
//
//void mfDosageDialog::on_noFormCheck_stateChanged( int state )
//{
//    if (state==Qt::Checked)
//        setPrescriptionState( Prescription::SpecifyForm, Qt::Unchecked );
//    else
//        setPrescriptionState( Prescription::SpecifyForm, Qt::Checked );
//}
//
//void mfDosageDialog::on_noPresentationCheck_stateChanged( int state )
//{
//    if (state==Qt::Checked)
//        setPrescriptionState( Prescription::SpecifyPresentation, Qt::Unchecked );
//    else
//        setPrescriptionState( Prescription::SpecifyPresentation, Qt::Checked );
//}
//
//void mfDosageDialog::on_intakesFromSpin_valueChanged( double d  )
//{
//    if (intakesToSpin->value() < d) {
//        intakesToSpin->setValue( d );
//    }
//    intakesToSpin->setMinimum(d);
//}
//
//void mfDosageDialog::on_durationFromSpin_valueChanged( double d  )
//{
//    if (durationToSpin->value() < d) {
//        durationToSpin->setValue( d );
//    }
//    durationToSpin->setMinimum(d);
//}
//
///** \brief Show a menu with the user recorded forms */
//void mfDosageDialog::on_userformsButton_clicked()
//{
//    if (tkSettings::instance()->value(MFDRUGS_SETTING_USERRECORDEDFORMS).isNull())
//        return;
//
//    const QStringList &ulist = tkSettings::instance()->value(MFDRUGS_SETTING_USERRECORDEDFORMS).toStringList();
//    QList<QAction*> list;
//    foreach( const QString &form, ulist ) {
//        if (!form.isEmpty())
//            list << new QAction(form, this);
//    }
//    QAction *aclear = new QAction(tr("Clear this list", "Clear the user's intakes recorded forms"), this);
//    list << aclear;
//
//    QAction *a = QMenu::exec(list, userformsButton->mapToGlobal(QPoint(0,20)) );
//    if (!a)
//        return;
//    if (a == aclear) {
//        tkSettings::instance()->setValue(MFDRUGS_SETTING_USERRECORDEDFORMS, QString() );
//    } else {
//        intakesCombo->setEditText( a->text() );
//    }
//}

/** \todo code this */
void mfDosageDialog::on_innButton_clicked()
{
    mfDrugsModel *m = mfDrugsModel::instance();
    m->setDrugData( d->m_CIS, Prescription::IsINNPrescription, innButton->isChecked() );
    if (innButton->isChecked())
        drugNameButton->setText(m->drugData(d->m_CIS, Drug::InnCompositionString).toString());
    else {
        QString name = m->drugData(d->m_CIS, Drug::Denomination).toString();
        drugNameButton->setText(name.left( name.lastIndexOf(",")));
    }
}
