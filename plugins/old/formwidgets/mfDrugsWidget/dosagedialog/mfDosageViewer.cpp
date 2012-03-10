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
  \class mfDosageViewer
  \brief QWidget for dosage creation / edition / modification. A dosage is a standard set of datas that will be used to help
  doctors when prescribing a drug.
  If you want to create a new dosage, you must create a new row onto the model BEFORE.\n
  If you want to edit or modify a dosage, you must inform the widget of the row and the CIS of the drug.\n

  Please always call done() when the dialog including the mfDosageViewer is done.

  \ingroup freediams drugswidget
*/


#include "mfDosageViewer.h"

// include drugwidget headers
//#include <drugsmodel/mfDosage.h>
#include <drugsmodel/mfDrugs.h>
#include <drugsmodel/mfDosageModel.h>
#include <drugsmodel/mfDrugsModel.h>
#include <mfDrugsConstants.h>
#include <mfDrugsManager.h>

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
class mfDosageViewerPrivate
{
public:
    mfDosageViewerPrivate(mfDosageViewer *parent) :
            m_Mapper(0), m_DosageModel(0), m_CIS(-1), m_Parent(parent) {}

    void setCheckBoxStateToModel( const int index, const int qtCheckState )
    {
        if (m_DosageModel) {
            if (qtCheckState==Qt::Checked)
                m_DosageModel->setData( m_DosageModel->index( m_Mapper->currentIndex(), index), true );
            else
                m_DosageModel->setData( m_DosageModel->index( m_Mapper->currentIndex(), index), false );
        } else {
            mfDrugsModel *m = DRUGMODEL;
            if (qtCheckState==Qt::Checked)
                m->setDrugData( m_CIS, index, true );
            else
                m->setDrugData( m_CIS, index, false );
        }
    }

    /** \brief Create and prepare mapper for drugsModel */
    void createDrugMapper()
    {
        if (!m_Mapper) {
            m_Mapper = new QDataWidgetMapper(m_Parent);
            m_Mapper->setModel( DRUGMODEL );
            m_Mapper->setSubmitPolicy( QDataWidgetMapper::AutoSubmit );
            m_Mapper->addMapping( m_Parent->intakesFromSpin, Prescription::IntakesFrom, "value" );
            m_Mapper->addMapping( m_Parent->intakesToSpin, Prescription::IntakesTo, "value" );
            m_Mapper->addMapping( m_Parent->intakesCombo, Prescription::IntakesScheme, "currentText" );
            m_Mapper->addMapping( m_Parent->periodSchemeCombo, Prescription::PeriodScheme, "currentText" );
            m_Mapper->addMapping( m_Parent->periodSpin, Prescription::Period, "value" );

            m_Mapper->addMapping( m_Parent->durationFromSpin, Prescription::DurationFrom );
            m_Mapper->addMapping( m_Parent->durationToSpin, Prescription::DurationTo );
            m_Mapper->addMapping( m_Parent->durationCombo, Prescription::DurationScheme, "currentText"  );

            m_Mapper->addMapping( m_Parent->minIntervalIntakesSpin, Prescription::IntakesIntervalOfTime, "value" );
            m_Mapper->addMapping( m_Parent->intervalTimeSchemeCombo, Prescription::IntakesIntervalScheme, "currentIndex"  );
            m_Mapper->addMapping( m_Parent->mealTimeCombo, Prescription::MealTimeSchemeIndex, "currentIndex" );
            m_Mapper->addMapping( m_Parent->noteTextEdit, Prescription::Note, "plainText" );
            m_Mapper->addMapping( m_Parent->dailySchemeListView, Prescription::DailyScheme, "checkedStringList" );

            m_Parent->tabWidget->removeTab(6);
            m_Parent->tabWidget->removeTab(4);
            m_Parent->tabWidget->removeTab(3);
            m_Parent->tabWidget->removeTab(2);
        }
    }

    /** \brief If mapper does not already exists, create it */
    void createDosageMapper()
    {
        Q_ASSERT(m_DosageModel);
        Q_ASSERT(m_Parent);
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
    void changeNonMappedDataFromModelToUi(const int row)
    {
        Q_ASSERT(m_Parent);
        mfDrugsModel *m = DRUGMODEL;
        if (m_DosageModel) {
            // There is a bug with Editable QComboBoxes and the currentText property to be setted !!
            // Need to be filled by hand the comboboxes...
            // Label
            m_Parent->labelLineEdit->setText(m_DosageModel->index( row, Dosage::Label).data().toString());
            // Intakes
            m_Parent->intakesCombo->setCurrentIndex(-1);
            m_Parent->intakesCombo->setEditText(m_DosageModel->index( row, Dosage::IntakesScheme).data().toString());
            // Period
            m_Parent->periodSpin->setValue(m_DosageModel->index( row, Dosage::Period).data().toDouble());
            m_Parent->periodSchemeCombo->setEditText(m_DosageModel->index( row, Dosage::PeriodScheme).data().toString());
            // Duration
            m_Parent->durationCombo->setEditText(m_DosageModel->index( row, Dosage::DurationScheme).data().toString());
            // Interval
            m_Parent->minIntervalIntakesSpin->setValue(m_DosageModel->index( row, Dosage::IntakesIntervalOfTime).data().toDouble());

            if (m_DosageModel->index( row, Dosage::IntakesUsesFromTo).data().toBool()) {
                m_Parent->fromToIntakesCheck->setChecked(true);
                m_Parent->intakesToLabel->show();
                m_Parent->intakesToSpin->show();
            }
            if (m_DosageModel->index( row, Dosage::DurationUsesFromTo).data().toBool()) {
                m_Parent->fromToDurationCheck->setChecked(true);
                m_Parent->durationToLabel->show();
                m_Parent->durationToSpin->show();
            }
//            m_Parent->dosageForAllInnCheck->setEnabled(DRUGMODEL->drugData(m_CIS, Drug::AllInnsKnown).toBool());
            m_Parent->dosageForAllInnCheck->setChecked(m_DosageModel->index(row, Dosage::INN_LK).data().toBool());
            m_Parent->aldCheck->setChecked(m_DosageModel->index(row,Dosage::IsALD).data().toBool());
        } else {
            m_Parent->labelLineEdit->hide();
            m_Parent->labelOfDosageLabel->hide();

            // Intakes
            m_Parent->intakesCombo->setCurrentIndex(-1);
            m_Parent->intakesCombo->setEditText(m->drugData(m_CIS, Prescription::IntakesScheme).toString());
            // Period
            m_Parent->periodSpin->setValue(m->drugData(m_CIS, Prescription::Period).toDouble());
            m_Parent->periodSchemeCombo->setEditText(m->drugData(m_CIS, Prescription::PeriodScheme).toString());
            // Duration
            m_Parent->durationCombo->setEditText(m->drugData(m_CIS, Prescription::DurationScheme).toString());
            // Interval
            m_Parent->minIntervalIntakesSpin->setValue(m->drugData(m_CIS, Prescription::IntakesIntervalOfTime).toDouble());

            if (m->drugData( m_CIS, Prescription::IntakesUsesFromTo).toBool()) {
                m_Parent->fromToIntakesCheck->setChecked(true);
                m_Parent->intakesToLabel->show();
                m_Parent->intakesToSpin->show();
            }
            if (m->drugData( m_CIS, Prescription::DurationUsesFromTo).toBool()) {
                m_Parent->fromToDurationCheck->setChecked(true);
                m_Parent->durationToLabel->show();
                m_Parent->durationToSpin->show();
            }
            m_Parent->aldCheck->setChecked(m->drugData(m_CIS,Prescription::IsALD).toBool());
        }
    }

    /** \brief Resize hourly table widget on view */
    void resizeTableWidget()
    {
        Q_ASSERT(m_Parent);
        int i = 0;
        int size = ( ( m_Parent->hourlyTableWidget->size().width() - m_Parent->style()->pixelMetric( QStyle::PM_DefaultFrameWidth ) ) / 8 );
        for( i = 0; i < 8; i++ )
            m_Parent->hourlyTableWidget->setColumnWidth( i, size );
    }

    /** \brief Update the Ui with the drug informations */
    void fillDrugsData()
    {
        Q_ASSERT(m_Parent);
        mfDrugsModel *m = DRUGMODEL;
        m_Parent->labelOfDosageLabel->setToolTip(m->drugData( m_CIS, Drug::AvailableDosages).toString() );
//        QString toolTip = drugM->drugData( m_CIS, Interaction::ToolTip ).toString();
//        toolTip = drugM->drugData( m_CIS, Drug::CompositionString ).toString();
//        dosageForAllInnCheck.setEnabled(m->drugData(CIS, Drug::AllInnsKnown ).toBool());
    }

    /**
      \brief Reset the Ui to the defaults.
      \li Clears all combo, refill them and set their currentIndex to the default mfDosageModel::periodDefault()
      \li Manage scored tablet
    */
    void resetUiToDefaults()
    {
        Q_ASSERT(m_Parent);
        mfDrugsModel *m = DRUGMODEL;
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
        m_Parent->intakesCombo->addItems(m->drugData(m_CIS, Drug::AvailableForms).toStringList());
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
        bool isScored = m->drugData( m_CIS, Drug::IsScoredTablet ).toBool();
        if ( isScored ) {
            m_Parent->intakesToSpin->setDecimals( 2 );
            m_Parent->intakesFromSpin->setDecimals( 2 );
            m_Parent->intakesToSpin->setSingleStep( 0.25 );
            m_Parent->intakesFromSpin->setSingleStep( 0.25 );
            m_Parent->intakesToSpin->setMinimum( 0.25 );
            m_Parent->intakesFromSpin->setMinimum( 0.25 );
        } else {
            m_Parent->intakesToSpin->setDecimals( 0 );
            m_Parent->intakesFromSpin->setDecimals( 0 );
            m_Parent->intakesToSpin->setSingleStep( 1 );
            m_Parent->intakesFromSpin->setSingleStep( 1 );
            m_Parent->intakesToSpin->setMinimum( 1 );
            m_Parent->intakesFromSpin->setMinimum( 1 );
        }
        resizeTableWidget();
        if (m_DosageModel)
            m_Parent->dosageForAllInnCheck->setEnabled(dosageCanLinkWithInn());
        else
            m_Parent->dosageForAllInnCheck->setVisible(dosageCanLinkWithInn());
    }

    bool dosageCanLinkWithInn()
    {
        if (m_DosageModel) {
            mfDrugsModel *m = DRUGMODEL;
            return ((m->drugData(m_CIS, Drug::MainInnCode).toInt()!=-1) &&
                    (m->drugData(m_CIS,Drug::AllInnsKnown).toBool()));
        }
        return false;
    }

public:
    QDataWidgetMapper  *m_Mapper;
    mfDosageModel      *m_DosageModel;
    QString             m_ActualDosageUuid;
    int                 m_CIS;
private:
    mfDosageViewer *m_Parent;
};



/**
 \todo when showing dosage, make verification of limits +++  ==> for FMF only
 \todo use a QPersistentModelIndex instead of drugRow, dosageRow
*/
mfDosageViewer::mfDosageViewer( QWidget *parent )
    : QWidget( parent ),
    d(0)
{
    // some initializations
    setObjectName( "mfDosageViewer" );
    d = new mfDosageViewerPrivate(this);

    // Ui initialization
    setupUi(this);
    setWindowTitle( tr( "Drug Dosage Creator" ) + " - " + qApp->applicationName() );
    userformsButton->setIcon( tkTheme::icon(ICONEDIT) );
    // remove last page of tabWidget (TODO page)
    tabWidget->removeTab(tabWidget->count()-1);

    // this must be done here
    tkStringListModel * stringModel = new tkStringListModel( this );
    stringModel->setStringList( dailySchemeList() );
    stringModel->setCheckable(true);
    dailySchemeListView->setModel( stringModel );
    dailySchemeListView->hideButtons();
    tabWidget->setCurrentIndex(0);
}

/** \brief Use this function to define a drugsModel behavior. */
void mfDosageViewer::useDrugsModel(const int CIS, const int drugRow)
{
    Q_ASSERT(DRUGMODEL->containsDrug(CIS));
    d->m_CIS = CIS;
    d->m_DosageModel = 0;
    d->resetUiToDefaults();
    d->fillDrugsData();
    d->createDrugMapper();
    changeCurrentRow(drugRow);
}

/** \brief Use this function to define a dosageModel behavior. */
void mfDosageViewer::setDosageModel( mfDosageModel *model )
{
    Q_ASSERT(model);
    d->m_DosageModel = model;
    d->m_CIS = model->drugCIS();
    d->resetUiToDefaults();
    d->fillDrugsData();
    d->createDosageMapper();

    if (model->rowCount()==0) {
        model->insertRow(0);
        changeCurrentRow(0);
    } else {
        changeCurrentRow(0);
    }
}

/** \brief Destructor */
mfDosageViewer::~mfDosageViewer()
{
    if (d) delete d;
    d=0;
}

/** \brief Changes the current editing dosage */
void mfDosageViewer::changeCurrentRow(const int dosageRow)
{
    // manage non mapped datas
    if (dosageRow==d->m_Mapper->currentIndex())
        return;
    d->resetUiToDefaults();
    d->changeNonMappedDataFromModelToUi(dosageRow);
    d->m_Mapper->setCurrentIndex(dosageRow);
    qWarning() << dosageRow << QString("%1 = %2,").arg(DRUGMODEL->drugData(d->m_CIS,Drug::MainInnName).toString().toUpper()).arg(d->m_CIS);
}

/** \brief Changes the current editing dosage */
void mfDosageViewer::changeCurrentRow(const QModelIndex &item )
{
    changeCurrentRow(item.row());
}

/**
   \brief When dialog including mfDosageViewer is validate call this member with the result of the dialog.
   It stores some values inside the users settings.
*/
void mfDosageViewer::done(int r)
{
    if ( r == QDialog::Accepted ) {
        // match the user's form for the settings
        const QStringList &pre = mfDosageModel::predeterminedForms();
        const QStringList &av  = DRUGMODEL->drugData(d->m_CIS,Drug::AvailableForms).toStringList();
        if (( pre.indexOf(intakesCombo->currentText()) == -1 ) &&
            ( av.indexOf(intakesCombo->currentText()) == -1 )) {
            tkSettings::instance()->appendToValue( MFDRUGS_SETTING_USERRECORDEDFORMS, intakesCombo->currentText() );
        }
    }
}

/** \brief Used for hourly table widget resizing */
void mfDosageViewer::resizeEvent( QResizeEvent * event )
{
    d->resizeTableWidget();
    QWidget::resizeEvent( event );
}

void mfDosageViewer::on_fromToIntakesCheck_stateChanged(int state)
{
    if (d->m_DosageModel)
        d->setCheckBoxStateToModel( Dosage::IntakesUsesFromTo, state);
    else
        d->setCheckBoxStateToModel( Prescription::IntakesUsesFromTo, state);
}

void mfDosageViewer::on_fromToDurationCheck_stateChanged(int state)
{
    if (d->m_DosageModel)
        d->setCheckBoxStateToModel( Dosage::DurationUsesFromTo, state );
    else
        d->setCheckBoxStateToModel( Prescription::DurationUsesFromTo, state );
}

/** \brief Redefine the minimum of the "to" intakes */
void mfDosageViewer::on_intakesFromSpin_valueChanged( double d  )
{
    if (intakesToSpin->value() < d) {
        intakesToSpin->setValue( d );
    }
    intakesToSpin->setMinimum(d);
}

/** \brief Redefine the minimum of the "to" duration */
void mfDosageViewer::on_durationFromSpin_valueChanged( double d )
{
    if (durationToSpin->value() < d) {
        durationToSpin->setValue( d );
    }
    durationToSpin->setMinimum(d);
}

/** \brief Show a menu with the user recorded forms */
void mfDosageViewer::on_userformsButton_clicked()
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
        intakesCombo->setEditText(a->text());
        if (d->m_DosageModel)
            d->m_DosageModel->setData(d->m_DosageModel->index(d->m_Mapper->currentIndex(),Dosage::IntakesScheme),a->text());
        else
            DRUGMODEL->setDrugData(d->m_CIS, Prescription::IntakesScheme, a->text());
    }
}

/** \brief If INN linking is available, shows the inn name and the dosage used for the link */
void mfDosageViewer::on_dosageForAllInnCheck_stateChanged(int state)
{
    mfDrugsModel *m = DRUGMODEL;
    if (d->m_DosageModel) {
        // INN Prescription ?
        int row = d->m_Mapper->currentIndex();
            if ((dosageForAllInnCheck->isEnabled()) && (state==Qt::Checked)) {
                d->m_DosageModel->setData( d->m_DosageModel->index(row, Dosage::CIS_LK), d->m_CIS );
                d->m_DosageModel->setData( d->m_DosageModel->index(row, Dosage::CIP_LK), QVariant() );
                d->m_DosageModel->setData( d->m_DosageModel->index(row, Dosage::INN_LK), m->drugData(d->m_CIS,Drug::MainInnCode) );
                d->m_DosageModel->setData( d->m_DosageModel->index(row, Dosage::InnLinkedDosage), m->drugData(d->m_CIS,Drug::MainInnDosage) );
            } else {
                d->m_DosageModel->setData( d->m_DosageModel->index(row, Dosage::CIS_LK), d->m_CIS );
                d->m_DosageModel->setData( d->m_DosageModel->index(row, Dosage::CIP_LK), QVariant() );
                d->m_DosageModel->setData( d->m_DosageModel->index(row, Dosage::INN_LK), QVariant() );
                d->m_DosageModel->setData( d->m_DosageModel->index(row, Dosage::InnLinkedDosage), m->drugData(d->m_CIS,Drug::MainInnCode) );
            }
        innCompositionLabel->show();
        innCompositionLabel->setText( tr("Linking to : ")
                                      + m->drugData(d->m_CIS, Drug::MainInnName).toString() + " "
//                                      + tr("Dosage of molecule : ")
                                      + m->drugData(d->m_CIS, Drug::MainInnDosage).toString());
    } else
        innCompositionLabel->hide();
}

void mfDosageViewer::on_aldCheck_stateChanged(int state)
{
    if (d->m_DosageModel)
        d->setCheckBoxStateToModel( Dosage::IsALD, state );
    else
        d->setCheckBoxStateToModel( Prescription::IsALD, state );
}
