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
  \class mfDosageCreator
  \brief Dialog for dosage creation / edition / modification. A dosage is a standard set of datas that will be used to help
  doctors when prescribing a drug.
  If you want to create a new dosage, you must create a new row onto the model BEFORE.
  If you want to edit or modify a dosage, you must inform the dialog of the row and the CIS of the drug.
  \ingroup drugsinteractions drugswidget
*/


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
#include <tkSettings.h>
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

/**
  \brief Private part
  \internal
*/
class mfDosageCreatorDialogPrivate
{
public:
    mfDosageCreatorDialogPrivate(mfDosageCreatorDialog *parent) :
            m_Mapper(0), m_DosageModel(0), m_Parent(parent) {}

    void setCheckBoxStateToModel( const int index, const int qtCheckState )
    {
        if (qtCheckState==Qt::Checked)
            m_DosageModel->setData( m_DosageModel->index( m_Mapper->currentIndex(), index), true );
        else
            m_DosageModel->setData( m_DosageModel->index( m_Mapper->currentIndex(), index), false );
    }

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
            cb->setCurrentIndex(-1);
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

    bool checkDosageValidity(const int row)
    {
        QStringList list = m_DosageModel->isDosageValid(row);
        if (list.count()) {
            tkGlobal::warningMessageBox(m_Parent->tr("Dosage is not valid."),
                                        list.join("br />"),
                                        "", m_Parent->tr("Drug Dosage Creator"));
            return false;
        }
        return true;
    }

    /** \brief Commits dosage model using a transaction to database */
    void commitModelToDatabase()
    {
        m_DosageModel->database().transaction();
        if (m_DosageModel->submitAll()) {
            if (m_DosageModel->database().commit())
                tkLog::addMessage(m_Parent, m_Parent->tr("Dosage correctly saved to base"));
            else
                tkLog::addError(m_Parent, m_Parent->tr("SQL Error : Dosage can not be added to database : %1")
                                .arg(m_DosageModel->lastError().text()));
        } else {
            m_DosageModel->database().rollback();
            QMessageBox::warning(m_Parent, m_Parent->tr("Drug Dosage Creator"),
                                 tkTr(ERROR_1_FROM_DATABASE_2)
                                 .arg(m_DosageModel->database().lastError().text())
                                 .arg(m_DosageModel->database().connectionName()));
        }
    }

    /** \brief If mapper does not already exists, create it */
    void createMapper()
    {
        if (!m_Mapper) {
            m_Mapper = new QDataWidgetMapper(m_Parent);
            m_Mapper->setModel( m_DosageModel );
            m_Mapper->setSubmitPolicy( QDataWidgetMapper::AutoSubmit );
            m_Mapper->addMapping( m_Parent->labelLineEdit, Dosage::Label, "text" );

            m_Mapper->addMapping( m_Parent->intakesFromSpin, Dosage::IntakesFrom, "value" );
            m_Mapper->addMapping( m_Parent->intakesToSpin, Dosage::IntakesTo, "value" );
            m_Mapper->addMapping( m_Parent->intakesCombo, Dosage::IntakesScheme, "currentText" );
            m_Mapper->addMapping( m_Parent->periodSchemeCombo, Dosage::PeriodScheme, "currentText" );
            m_Mapper->addMapping( m_Parent->periodSpin, Dosage::Period, "value" );

            m_Mapper->addMapping( m_Parent->durationFromSpin, Dosage::DurationFrom );
            m_Mapper->addMapping( m_Parent->durationToSpin, Dosage::DurationTo );
            m_Mapper->addMapping( m_Parent->durationCombo, Dosage::DurationScheme, "currentText"  );

            m_Mapper->addMapping( m_Parent->minIntervalIntakesSpin, Dosage::IntakesIntervalOfTime, "value" );
            m_Mapper->addMapping( m_Parent->intervalTimeSchemeCombo, Dosage::IntakesIntervalScheme, "currentIndex"  );
            m_Mapper->addMapping( m_Parent->mealTimeCombo, Dosage::MealScheme, "currentIndex" );
            m_Mapper->addMapping( m_Parent->noteTextEdit, Dosage::Note, "plainText" );
            m_Mapper->addMapping( m_Parent->dailySchemeListView, Dosage::DailyScheme, "checkedStringList" );

            m_Mapper->addMapping( m_Parent->minAgeSpin, Dosage::MinAge, "value" );
            m_Mapper->addMapping( m_Parent->maxAgeSpin, Dosage::MaxAge, "value" );
            m_Mapper->addMapping( m_Parent->minAgeCombo, Dosage::MinAgeReferenceIndex, "currentIndex" );
            m_Mapper->addMapping( m_Parent->maxAgeCombo, Dosage::MaxAgeReferenceIndex, "currentIndex" );
            m_Mapper->addMapping( m_Parent->minWeightSpin, Dosage::MinWeight, "value" );
            m_Mapper->addMapping( m_Parent->minClearanceSpin, Dosage::MinClearance, "value" );
            m_Mapper->addMapping( m_Parent->maxClearanceSpin, Dosage::MaxClearance, "value" );
            m_Mapper->addMapping( m_Parent->sexLimitCombo, Dosage::SexLimitedIndex, "currentIndex" );
        }
    }

    /**
       \brief Manage non mapped datas from the model to the ui
       Manages Sex limitation, Age limitations, Clearance limitations
    */
    void changeNonMappedDataFromModelToUi(const int dosageRow)
    {
        // There is a bug with Editable QComboBoxes and the currentText property to be setted !!
        // Need to be filled by hand the comboboxes...
        // Label
        m_Parent->labelLineEdit->setText(m_DosageModel->index( dosageRow, Dosage::Label).data().toString());
        // Intakes
        m_Parent->intakesCombo->setCurrentIndex(-1);
        m_Parent->intakesCombo->setEditText(m_DosageModel->index( dosageRow, Dosage::IntakesScheme).data().toString());
        // Intakes scheme
        m_Parent->periodSpin->setValue(m_DosageModel->index( dosageRow, Dosage::Period).data().toDouble());
        m_Parent->periodSchemeCombo->setEditText(m_DosageModel->index( dosageRow, Dosage::PeriodScheme).data().toString());
        // Duration
        m_Parent->durationCombo->setEditText(m_DosageModel->index( dosageRow, Dosage::DurationScheme).data().toString());
        // Interval
        m_Parent->minIntervalIntakesSpin->setValue(m_DosageModel->index( dosageRow, Dosage::IntakesIntervalOfTime).data().toDouble());

        if (m_DosageModel->index( dosageRow, Dosage::IntakesUsesFromTo).data().toBool()) {
            m_Parent->fromToIntakesCheck->setChecked(true);
            m_Parent->intakesToLabel->show();
            m_Parent->intakesToSpin->show();
        }
        if (m_DosageModel->index( dosageRow, Dosage::DurationUsesFromTo).data().toBool()) {
            m_Parent->fromToDurationCheck->setChecked(true);
            m_Parent->durationToLabel->show();
            m_Parent->durationToSpin->show();
        }
    }

    /** \brief Resize hourly table widget on view */
    void resizeTableWidget()
    {
        int i = 0;
        int size = ( ( m_Parent->hourlyTableWidget->size().width() - m_Parent->style()->pixelMetric( QStyle::PM_DefaultFrameWidth ) ) / 8 );
        for( i = 0; i < 8; i++ )
            m_Parent->hourlyTableWidget->setColumnWidth( i, size );
    }

    /** \brief Update the Ui with the drug informations */
    void fillDrugsData()
    {
        mfDrugsModel *drugM = mfDrugsModel::instance();
        int CIS = m_DosageModel->drugCIS();
        m_Parent->drugNameLabel->setText( drugM->drugData( CIS, Drug::Denomination).toString() );
        m_Parent->labelOfDosageLabel->setToolTip( drugM->drugData( CIS, Drug::AvailableDosages).toString() );
        QString toolTip = drugM->drugData( CIS, Interaction::ToolTip ).toString();
        toolTip = drugM->drugData( CIS, Drug::CompositionString ).toString();
        m_Parent->drugNameLabel->setToolTip( toolTip );
    }

    /**
      \brief Reset the Ui to the defaults.
      \li Clears all combo, refill them and set their currentIndex to the default mfDosageModel::periodDefault()
      \li Manage scored tablet
    */
    void resetUiToDefaults()
    {
        mfDrugsModel *drugM = mfDrugsModel::instance();
        int CIS = m_DosageModel->drugCIS();
        m_Parent->intakesToLabel->hide();
        m_Parent->intakesToSpin->hide();
        m_Parent->durationToLabel->hide();
        m_Parent->durationToSpin->hide();
        // Prepare some combos
        m_Parent->durationCombo->clear();
        m_Parent->durationCombo->addItems( periods() );
        m_Parent->durationCombo->setCurrentIndex( Time::Months );
        m_Parent->periodSchemeCombo->clear();
        m_Parent->periodSchemeCombo->addItems( periods() );
        m_Parent->periodSchemeCombo->setCurrentIndex( Time::Days );
        m_Parent->intervalTimeSchemeCombo->clear();
        m_Parent->intervalTimeSchemeCombo->addItems( periods() );
        m_Parent->intervalTimeSchemeCombo->setCurrentIndex( Time::Days );
        m_Parent->intakesCombo->addItems(drugM->drugData(CIS, Drug::AvailableForms).toStringList());
        m_Parent->intakesCombo->setCurrentIndex(0);
        m_Parent->mealTimeCombo->clear();
        m_Parent->mealTimeCombo->addItems( mealTime() );

        m_Parent->minAgeCombo->clear();
        m_Parent->minAgeCombo->addItems( preDeterminedAges() );
        m_Parent->maxAgeCombo->clear();
        m_Parent->maxAgeCombo->addItems( preDeterminedAges() );

        m_Parent->hourlyTableWidget->verticalHeader()->hide();
        m_Parent->hourlyTableWidget->horizontalHeader()->hide();
        m_Parent->hourlyTableWidget->resizeColumnsToContents();
        bool isScored = drugM->drugData( CIS, Drug::IsScoredTablet ).toBool();
        if ( isScored ) {
            m_Parent->intakesToSpin->setDecimals( 2 );
            m_Parent->intakesFromSpin->setDecimals( 2 );
            m_Parent->intakesToSpin->setSingleStep( 0.25 );
            m_Parent->intakesFromSpin->setSingleStep( 0.25 );
        } else {
            m_Parent->intakesToSpin->setDecimals( 0 );
            m_Parent->intakesFromSpin->setDecimals( 0 );
            m_Parent->intakesToSpin->setSingleStep( 1 );
            m_Parent->intakesFromSpin->setSingleStep( 1 );
        }
        resizeTableWidget();
    }

public:
    QDataWidgetMapper  *m_Mapper;
    mfDosageModel      *m_DosageModel;
    QString             m_ActualDosageUuid;
private:
    mfDosageCreatorDialog *m_Parent;
};



/**
 \todo when showing dosage, make verification of limits +++  ==> for FMF only
 \todo use a QPersistentModelIndex instead of drugRow, dosageRow
*/
mfDosageCreatorDialog::mfDosageCreatorDialog( QWidget *parent, mfDosageModel *dosageModel )
    : QDialog( parent ),
    d(0)
{
    // some initializations
    setObjectName( "mfDosageCreatorDialog" );
    d = new mfDosageCreatorDialogPrivate(this);
    d->m_DosageModel = dosageModel;

    // Ui initialization
    setupUi(this);
    setWindowTitle( tr( "Drug Dosage Creator" ) + " - " + qApp->applicationName() );
    userformsButton->setIcon( tkTheme::icon(ICONEDIT) );

    // this must be done here
    tkStringListModel * stringModel = new tkStringListModel( this );
    stringModel->setStringList( dailySchemeList() );
    stringModel->setCheckable(true);
    dailySchemeListView->setModel( stringModel );
    dailySchemeListView->hideButtons();

    d->resetUiToDefaults();
    d->fillDrugsData();

    availableDosagesListView->setModel(dosageModel);
    availableDosagesListView->setModelColumn(Dosage::Label);
    availableDosagesListView->setEditTriggers( QListView::NoEditTriggers );

    d->createMapper();

    if (dosageModel->rowCount()==0) {
//        qWarning() << "creating default dosage";
        dosageModel->insertRow(0);
        changeCurrentRow(0);
    } else {
        changeCurrentRow(0);
    }

    connect(availableDosagesListView->listView(), SIGNAL(activated(QModelIndex)),this,SLOT(changeCurrentRow(QModelIndex)));
}

/** \brief Destructor */
mfDosageCreatorDialog::~mfDosageCreatorDialog()
{
    if (d) delete d;
    d=0;
}

/** \brief Changes the current editing dosage */
void mfDosageCreatorDialog::changeCurrentRow(const int dosageRow)
{
    // manage non mapped datas
    if (dosageRow==d->m_Mapper->currentIndex())
        return;
    d->resetUiToDefaults();
    d->changeNonMappedDataFromModelToUi(dosageRow);
    d->m_Mapper->setCurrentIndex(dosageRow);
}

/** \brief Changes the current editing dosage */
void mfDosageCreatorDialog::changeCurrentRow(const QModelIndex &item )
{
    changeCurrentRow(item.row());
}

/**
   \brief Validate the dialog
   \todo Check dosage validity before validate the dialog
*/
void mfDosageCreatorDialog::done( int r )
{
    int row;
    if (d->m_Mapper)
        row =  d->m_Mapper->currentIndex();
    else
        QDialog::done(r);

    if ( r == QDialog::Rejected ) {
        d->m_DosageModel->revertRow( row );
    }  else {
        // match the user's form for the settings
        const QStringList &pre = mfDosageModel::predeterminedForms();
        const QStringList &av  = mfDrugsModel::instance()->drugData(d->m_DosageModel->drugCIS(),Drug::AvailableForms).toStringList();
        if (( pre.indexOf(intakesCombo->currentText()) == -1 ) &&
            ( av.indexOf(intakesCombo->currentText()) == -1 )) {
            tkSettings::instance()->appendToValue( MFDRUGS_SETTING_USERRECORDEDFORMS, intakesCombo->currentText() );
        }

        // ******************************************************
        // * TODO check validity of the dosage before submition *
        // ******************************************************
        //        if ( m_DosageModel->isDosageValid( row ) ) {
        // submit mapper to model
//        d->m_Mapper->submit();


        // commit model changes to database
    }
    QDialog::done(r);
}

void mfDosageCreatorDialog::on_saveButton_clicked()
{
    int row = d->m_Mapper->currentIndex();
    if (!d->checkDosageValidity(row))
        return;
    d->commitModelToDatabase();
    QDialog::done(QDialog::Accepted);
}

void mfDosageCreatorDialog::on_prescribeButton_clicked()
{
    // make sure that the drug was inserted to the drugsmodel
    int row = d->m_Mapper->currentIndex();
    d->m_DosageModel->toPrescription(row);
    QDialog::done(QDialog::Accepted);
}

void mfDosageCreatorDialog::on_saveAndPrescribeButton_clicked()
{
    int row = d->m_Mapper->currentIndex();
    if (!d->checkDosageValidity(row))
        return;
    d->m_DosageModel->toPrescription(row);
    d->commitModelToDatabase();
    QDialog::done(QDialog::Accepted);
}

/** \brief Used for hourly table widget resizing */
void mfDosageCreatorDialog::resizeEvent( QResizeEvent * event )
{
    d->resizeTableWidget();
    QWidget::resizeEvent( event );
}

void mfDosageCreatorDialog::on_fromToIntakesCheck_stateChanged(int state)
{
    d->setCheckBoxStateToModel( Dosage::IntakesUsesFromTo, state);
}

void mfDosageCreatorDialog::on_fromToDurationCheck_stateChanged(int state)
{
    d->setCheckBoxStateToModel( Dosage::DurationUsesFromTo, state );
}

void mfDosageCreatorDialog::on_intakesFromSpin_valueChanged( double d  )
{
    if (intakesToSpin->value() < d) {
        intakesToSpin->setValue( d );
    }
    intakesToSpin->setMinimum(d);
}

void mfDosageCreatorDialog::on_durationFromSpin_valueChanged( double d )
{
    if (durationToSpin->value() < d) {
        durationToSpin->setValue( d );
    }
    durationToSpin->setMinimum(d);
}

/** \brief Show a menu with the user recorded forms */
void mfDosageCreatorDialog::on_userformsButton_clicked()
{
    if (tkSettings::instance()->value(MFDRUGS_SETTING_USERRECORDEDFORMS, QVariant()).isNull())
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
