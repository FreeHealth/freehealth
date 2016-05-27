/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/

/**
  \class DrugsWidget::DosageViewer
  \brief QWidget for dosage creation / edition / modification. A dosage is a standard set of data that will be used to help
  doctors when prescribing a drug.
  If you want to create a new dosage, you must create a new row onto the model BEFORE.\n
  If you want to edit or modify a dosage, you must inform the widget of the row and the CIS of the drug.\n

  Please always call done() when the dialog including the DosageViewer is done.
*/
// TODO: update documentation

#include "dosageviewer.h"

#include <drugsbaseplugin/drugbasecore.h>
#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/dosagemodel.h>
#include <drugsbaseplugin/drugsmodel.h>
#include <drugsbaseplugin/dailyschememodel.h>

#include <drugsplugin/constants.h>
#include <drugsplugin/drugswidgetmanager.h>

#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/dialogs/helpdialog.h>
#include <coreplugin/constants_icons.h>

#include <QHeaderView>
#include <QRadioButton>
#include <QCheckBox>
#include <QTableWidget>
#include <QSpinBox>
#include <QDataWidgetMapper>
#include <QDesktopServices>
#include <QUrl>
#include <QMenu>

using namespace DrugsWidget::Constants;
using namespace DrugsWidget::Internal;
using namespace Trans::ConstantTranslations;

static inline DrugsDB::DrugsModel *drugModel() { return DrugsWidget::DrugsWidgetManager::instance()->currentDrugsModel(); }
static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline DrugsDB::DrugsBase &drugsBase() {return DrugsDB::DrugBaseCore::instance().drugsBase();}

namespace DrugsWidget {
namespace Internal {

class DosageViewerPrivate
{
public:
    DosageViewerPrivate(DosageViewer *parent) :
            m_Mapper(0), m_DosageModel(0), m_SpinDelegate(0), q(parent) {}

    void setCheckBoxStateToModel(const int index, const int qtCheckState)
    {
        if (m_DosageModel) {
            m_DosageModel->setData(m_DosageModel->index(m_Mapper->currentIndex(), index), qtCheckState==Qt::Checked);
//            qWarning() << "dosage" << Dosages::Constants::IntakesUsesFromTo << m_Mapper->currentIndex() <<  m_DosageModel->data(m_DosageModel->index(m_Mapper->currentIndex(), index));
        } else {
//            qWarning() << "drug";
            drugModel()->setDrugData(m_DrugId, index, qtCheckState==Qt::Checked);
        }
    }

    /** \brief Create and prepare mapper for drugsModel */
    void createDrugMapper()
    {
        using namespace Dosages::Constants;
        using namespace DrugsDB::Constants;
        if (!m_Mapper) {
            m_Mapper = new QDataWidgetMapper(q);
            m_Mapper->setModel(drugModel());
            m_Mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);

            m_Mapper->addMapping(q->intakesFromSpin, Prescription::IntakesFrom, "value");
            m_Mapper->addMapping(q->intakesToSpin, Prescription::IntakesTo, "value");
            m_Mapper->addMapping(q->intakesCombo, Prescription::IntakesScheme, "currentText");

            if (drugsBase().isRoutesAvailable())
                m_Mapper->addMapping(q->routeCombo, Prescription::Route, "currentText");

            m_Mapper->addMapping(q->periodSchemeCombo, Prescription::PeriodScheme, "currentText");
            m_Mapper->addMapping(q->periodSpin, Prescription::Period, "value");

            m_Mapper->addMapping(q->durationFromSpin, Prescription::DurationFrom);
            m_Mapper->addMapping(q->durationToSpin, Prescription::DurationTo);
            m_Mapper->addMapping(q->durationCombo, Prescription::DurationScheme, "currentText");
            m_Mapper->addMapping(q->refillSpin, Prescription::Refill, "value");

            m_Mapper->addMapping(q->minIntervalIntakesSpin, Prescription::IntakesIntervalOfTime, "value");
            m_Mapper->addMapping(q->intervalTimeSchemeCombo, Prescription::IntakesIntervalSchemeIndex, "currentIndex");
            m_Mapper->addMapping(q->mealTimeCombo, Prescription::MealTimeSchemeIndex, "currentIndex");
            m_Mapper->addMapping(q->noteTextEdit, Prescription::Note, "plainText");

            q->tabWidget->removeTab(6);
            q->tabWidget->removeTab(4);
            q->tabWidget->removeTab(3);
            q->tabWidget->removeTab(2);
        }
    }

    /** \brief If mapper does not already exists, create it */
    void createDosageMapper()
    {
        using namespace DrugsDB::Constants;
        Q_ASSERT(m_DosageModel);
        Q_ASSERT(q);
        if (!m_Mapper) {
            m_Mapper = new QDataWidgetMapper(q);
            m_Mapper->setModel(m_DosageModel);
            m_Mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
            m_Mapper->addMapping(q->labelLineEdit, Dosages::Constants::Label, "text");

//            m_Mapper->addMapping(q->fromToIntakesCheck, Dosages::Constants::IntakesUsesFromTo, "checked");

            m_Mapper->addMapping(q->intakesFromSpin, Dosages::Constants::IntakesFrom, "value");
            m_Mapper->addMapping(q->intakesToSpin, Dosages::Constants::IntakesTo, "value");
            m_Mapper->addMapping(q->intakesCombo, Dosages::Constants::IntakesScheme, "currentText");
            m_Mapper->addMapping(q->periodSchemeCombo, Dosages::Constants::PeriodScheme, "currentText");
            m_Mapper->addMapping(q->periodSpin, Dosages::Constants::Period, "value");
            if (drugsBase().isRoutesAvailable())
                m_Mapper->addMapping(q->routeCombo, Dosages::Constants::Route, "currentText");

            m_Mapper->addMapping(q->durationFromSpin, Dosages::Constants::DurationFrom);
            m_Mapper->addMapping(q->durationToSpin, Dosages::Constants::DurationTo);
            m_Mapper->addMapping(q->durationCombo, Dosages::Constants::DurationScheme, "currentText");
            m_Mapper->addMapping(q->refillSpin, Dosages::Constants::Refill, "value");

            m_Mapper->addMapping(q->minIntervalIntakesSpin, Dosages::Constants::IntakesIntervalOfTime, "value");
            m_Mapper->addMapping(q->intervalTimeSchemeCombo, Dosages::Constants::IntakesIntervalScheme, "currentIndex");
            m_Mapper->addMapping(q->mealTimeCombo, Dosages::Constants::MealScheme, "currentIndex");
            m_Mapper->addMapping(q->noteTextEdit, Dosages::Constants::Note, "plainText");
            m_Mapper->addMapping(q->minAgeSpin, Dosages::Constants::MinAge, "value");
            m_Mapper->addMapping(q->maxAgeSpin, Dosages::Constants::MaxAge, "value");
            m_Mapper->addMapping(q->minAgeCombo, Dosages::Constants::MinAgeReferenceIndex, "currentIndex");
            m_Mapper->addMapping(q->maxAgeCombo, Dosages::Constants::MaxAgeReferenceIndex, "currentIndex");
            m_Mapper->addMapping(q->minWeightSpin, Dosages::Constants::MinWeight, "value");
            m_Mapper->addMapping(q->minClearanceSpin, Dosages::Constants::MinClearance, "value");
            m_Mapper->addMapping(q->maxClearanceSpin, Dosages::Constants::MaxClearance, "value");
            m_Mapper->addMapping(q->sexLimitCombo, Dosages::Constants::SexLimitedIndex, "currentIndex");
        }
    }

    /*!
     * \brief Manage non mapped data from the model to the UI
     *
     *  Manages Sex limitation, Age limitations, Clearance limitations
     */
    void changeNonMappedDataFromModelToUi(const int row)
    {
        Q_ASSERT(q);
        if (m_DosageModel) {
            // There is a bug with Editable QComboBoxes and the currentText property to be set !!
            // Need to be filled by hand the comboboxes...
            // Label
            q->labelLineEdit->setText(m_DosageModel->index(row, Dosages::Constants::Label).data().toString());
            // Intakes
            QString intakescheme = m_DosageModel->index(row, Dosages::Constants::IntakesScheme).data().toString();
            q->intakesCombo->setCurrentIndex(-1);
            q->intakesCombo->setEditText(intakescheme);
            // Period
            q->periodSpin->setValue(m_DosageModel->index(row, Dosages::Constants::Period).data().toDouble());
            q->periodSchemeCombo->setEditText(m_DosageModel->index(row, Dosages::Constants::PeriodScheme).data().toString());
            // Duration
            q->durationCombo->setEditText(m_DosageModel->index(row, Dosages::Constants::DurationScheme).data().toString());
            // Interval
            q->minIntervalIntakesSpin->setValue(m_DosageModel->index(row, Dosages::Constants::IntakesIntervalOfTime).data().toDouble());

            bool intakeRange = m_DosageModel->index(row, Dosages::Constants::IntakesUsesFromTo).data().toBool();
            q->fromToIntakesCheck->setChecked(intakeRange);
            q->intakesToLabel->setVisible(intakeRange);
            q->intakesToSpin->setVisible(intakeRange);

            bool durationRange = m_DosageModel->index(row, Dosages::Constants::DurationUsesFromTo).data().toBool();
            q->fromToDurationCheck->setChecked(durationRange);
            q->durationToLabel->setVisible(durationRange);
            q->durationToSpin->setVisible(durationRange);

            // populate DailSchemeModel
            DrugsDB::DailySchemeModel *daily = q->dailyScheme->model();
            Q_ASSERT(daily);
            daily->setSerializedContent(m_DosageModel->index(row, Dosages::Constants::SerializedDailyScheme).data().toString());
//            q->dailySchemeView->resizeColumnsToContents();

            int  inn = m_DosageModel->index(row, Dosages::Constants::INN_LK).data().toInt();
            bool innPrescr = (inn > 0);
            q->dosageForAllInnCheck->setChecked(innPrescr);
            q->innCompositionLabel->setVisible(innPrescr);

            q->aldCheck->setChecked(m_DosageModel->index(row, Dosages::Constants::IsALD).data().toBool());
        } else {
            using namespace DrugsDB::Constants;
            q->labelLineEdit->hide();
            q->labelOfDosageLabel->hide();

            // Intakes
            QString intakescheme = drugModel()->drugData(m_DrugId, Prescription::IntakesScheme).toString();
            q->intakesCombo->setCurrentIndex(-1);
            q->intakesCombo->setEditText(intakescheme);

            // Period
            q->periodSpin->setValue(drugModel()->drugData(m_DrugId, Prescription::Period).toDouble());
            q->periodSchemeCombo->setEditText(drugModel()->drugData(m_DrugId, Prescription::PeriodScheme).toString());
            // Duration
            q->durationCombo->setEditText(drugModel()->drugData(m_DrugId, Prescription::DurationScheme).toString());
            // Interval
            q->minIntervalIntakesSpin->setValue(drugModel()->drugData(m_DrugId, Prescription::IntakesIntervalOfTime).toDouble());

            bool intakeRange = drugModel()->index(row, Prescription::IntakesUsesFromTo).data().toBool();
            q->fromToIntakesCheck->setChecked(intakeRange);
            q->intakesToLabel->setVisible(intakeRange);
            q->intakesToSpin->setVisible(intakeRange);

            bool durationRange = drugModel()->index(row, Prescription::DurationUsesFromTo).data().toBool();
            q->fromToDurationCheck->setChecked(durationRange);
            q->durationToLabel->setVisible(durationRange);
            q->durationToSpin->setVisible(durationRange);

            q->aldCheck->setChecked(drugModel()->drugData(m_DrugId, Prescription::IsALD).toBool());
            // populate DailSchemeModel
            DrugsDB::DailySchemeModel *daily = q->dailyScheme->model();
            Q_ASSERT(daily);
            daily->setSerializedContent(drugModel()->drugData(m_DrugId, Prescription::SerializedDailyScheme).toString());
//            q->dailySchemeView->resizeColumnsToContents();
        }

        // Link to SCP
        if (!drugModel()->drugData(m_DrugId, DrugsDB::Constants::Drug::LinkToSCP).isNull()) {
            q->monographButton->setEnabled(true);
            q->monographButton->setToolTip(drugModel()->drugData(m_DrugId, DrugsDB::Constants::Drug::LinkToSCP).toString());
        } else {
            q->monographButton->setEnabled(false);
        }
    }

    /** \brief Resize hourly table widget on view */
    void resizeTableWidget()
    {
        Q_ASSERT(q);
        int i = 0;
        int size = ((q->hourlyTableWidget->size().width() - q->style()->pixelMetric(QStyle::PM_DefaultFrameWidth)) / 8);
        for(i = 0; i < 8; i++)
            q->hourlyTableWidget->setColumnWidth(i, size);
    }

    /** \brief Update the Ui with the drug information */
    void fillDrugsData()
    {
        Q_ASSERT(q);
        q->labelOfDosageLabel->setToolTip(drugModel()->drugData(m_DrugId, DrugsDB::Constants::Drug::AvailableDosages).toString());
//        q->drugNameLabel->setText(drugModel()->drugData(m_DrugId, DrugsDB::Constants::Drug::Denomination).toString());
//        q->drugNameLabel->setToolTip(drugModel()->drugData(m_DrugId, DrugsDB::Constants::Drug::CompositionString).toString());
//        q->interactionLabel->setPixmap(drugModel()->drugData(m_DrugId, DrugsDB::Constants::Interaction::Icon).value<QIcon>().pixmap(16,16));
//        q->interactionLabel->setToolTip(drugModel()->drugData(m_DrugId, DrugsDB::Constants::Interaction::ToolTip).toString());
    }

    /**
      \brief Reset the Ui to the defaults.
      \li Clears all combo, refill them and set their currentIndex to the default DosageModel::periodDefault()
      \li Manage scored tablet
    */
    void resetUiToDefaults()
    {
        using namespace DrugsDB::Constants;
        Q_ASSERT(q);
        q->intakesToLabel->hide();
        q->intakesToSpin->hide();
        q->durationToLabel->hide();
        q->durationToSpin->hide();

        // Prepare some combos
        q->durationCombo->clear();
        q->durationCombo->addItems(Trans::ConstantTranslations::periods());
        q->durationCombo->setCurrentIndex(Trans::Constants::Time::Months);

        q->periodSchemeCombo->clear();
        q->periodSchemeCombo->addItems(Trans::ConstantTranslations::periods());
        q->periodSchemeCombo->setCurrentIndex(Trans::Constants::Time::Days);

        q->intervalTimeSchemeCombo->clear();
        q->intervalTimeSchemeCombo->addItems(Trans::ConstantTranslations::periods());
        q->intervalTimeSchemeCombo->setCurrentIndex(Trans::Constants::Time::Days);

        q->routeCombo->clear();
        if (drugsBase().isRoutesAvailable()) {
            q->routeCombo->addItems(drugModel()->drugData(m_DrugId, Drug::AvailableRoutes).toStringList());
            q->routeCombo->setCurrentIndex(0);
        }

        q->intakesCombo->fancyClear();
        q->intakesCombo->fancyAddItems(drugModel()->drugData(m_DrugId, Drug::AvailableForms).toStringList(), "Model");
        q->intakesCombo->fancyAddItems(settings()->value(Constants::S_USERRECORDEDFORMS).toStringList(), Constants::USERRECORDEDFORMS_COLOR);
        q->intakesCombo->setCurrentIndex(0);

//        q->intakesCombo->clear();
//        q->intakesCombo->addItems(drugModel()->drugData(m_DrugId, Drug::AvailableForms).toStringList());
//        q->intakesCombo->addItems(settings()->value(Constants::S_USERRECORDEDFORMS).toStringList());
//        q->intakesCombo->setCurrentIndex(0);

        q->mealTimeCombo->clear();
        q->mealTimeCombo->addItems(Trans::ConstantTranslations::mealTime());

        q->minAgeCombo->clear();
        q->minAgeCombo->addItems(Trans::ConstantTranslations::preDeterminedAges());
        q->maxAgeCombo->clear();
        q->maxAgeCombo->addItems(Trans::ConstantTranslations::preDeterminedAges());

        q->hourlyTableWidget->verticalHeader()->hide();
        q->hourlyTableWidget->horizontalHeader()->hide();
        q->hourlyTableWidget->resizeColumnsToContents();
        bool isScored = drugModel()->drugData(m_DrugId, Drug::IsScoredTablet).toBool();
        if (isScored) {
            q->intakesToSpin->setDecimals(2);
            q->intakesFromSpin->setDecimals(2);
            q->intakesToSpin->setSingleStep(0.25);
            q->intakesFromSpin->setSingleStep(0.25);
            q->intakesToSpin->setMinimum(0.25);
            q->intakesFromSpin->setMinimum(0.25);
        } else {
            q->intakesToSpin->setDecimals(0);
            q->intakesFromSpin->setDecimals(0);
            q->intakesToSpin->setSingleStep(1);
            q->intakesFromSpin->setSingleStep(1);
            q->intakesToSpin->setMinimum(1);
            q->intakesFromSpin->setMinimum(1);
        }
        resizeTableWidget();

//        if (m_SpinDelegate) {
//            delete m_SpinDelegate;
//            m_SpinDelegate = 0;
//        }
//        if (isScored) {
//            m_SpinDelegate = new Utils::SpinBoxDelegate(q,0.0,1.0,true);
//        } else {
//            m_SpinDelegate = new Utils::SpinBoxDelegate(q,0,1,false);
//        }
//        q->dailySchemeView->setItemDelegateForColumn(DrugsDB::DailySchemeModel::Value, m_SpinDelegate);
        if (m_DosageModel)
            q->dosageForAllInnCheck->setEnabled(dosageCanLinkWithInn());
        else
            q->dosageForAllInnCheck->setVisible(dosageCanLinkWithInn());
    }

    void recalculateDailySchemeMaximum()
    {
        if (!q->fromToIntakesCheck->isChecked()) {
            q->dailyScheme->setDailyMaximum(q->intakesFromSpin->value());
        } else {
            q->dailyScheme->setDailyMaximum(q->intakesToSpin->value());
        }
    }

    bool dosageCanLinkWithInn()
    {
        if (m_DosageModel) {
            return ((drugModel()->drugData(m_DrugId, DrugsDB::Constants::Drug::MainInnCode).toInt()!=-1) &&
                    (drugModel()->drugData(m_DrugId, DrugsDB::Constants::Drug::AllInnsKnown).toBool()));
        }
        return false;
    }

public:
    QDataWidgetMapper  *m_Mapper;
    DrugsDB::Internal::DosageModel *m_DosageModel;
    QString m_ActualDosageUuid;
    QVariant m_DrugId;
    Utils::SpinBoxDelegate *m_SpinDelegate;

private:
    DosageViewer *q;
};

}  // End Internal
}  // End Drugs


/**
 \todo when showing dosage, make verification of limits +++  ==> for FMF only
 \todo use a QPersistentModelIndex instead of drugRow, dosageRow
*/
DosageViewer::DosageViewer(QWidget *parent)
    : QWidget(parent),
    d(0)
{
    // some initializations
    setObjectName("DosageViewer");
    d = new DosageViewerPrivate(this);

    // Ui initialization
    setupUi(this);
    setWindowTitle(tr("Drug Dosage Creator") + " - " + qApp->applicationName());
    userformsButton->setIcon(theme()->icon(Core::Constants::ICONEDIT));
    intakesCombo->setRemoveLightIcon(theme()->icon(Core::Constants::ICONCLOSELIGHT));
    intakesCombo->setMoveUpLightIcon(theme()->icon(Core::Constants::ICONMOVEUPLIGHT));
    intakesCombo->setMoveDownLightIcon(theme()->icon(Core::Constants::ICONMOVEDOWNLIGHT));
    // remove last page of tabWidget (TODO page)
    tabWidget->removeTab(tabWidget->count()-1);
    drugNameLabel->hide();
    interactionLabel->hide();

    // define models
    DrugsDB::DailySchemeModel *model = new DrugsDB::DailySchemeModel(this);
    dailyScheme->setModel(model);
    connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SLOT(onDailySchemeModelDataChanged(QModelIndex)));

    // show first page of the tabwidget   ;;   Hide unused tables
    tabWidget->setCurrentIndex(0);
    this->hourlyTableWidget->hide();

    // manage routes ?
    if (!drugsBase().isRoutesAvailable()) {
        routeCombo->hide();
        routeLabel->hide();
    }
}

/** \brief Use this function to define a drugsModel behavior. */
void DosageViewer::useDrugsModel(const QVariant &drugId, const int drugRow)
{
    Q_ASSERT(drugModel()->containsDrug(drugId));
    d->m_DrugId = drugId;
    d->m_DosageModel = 0;
    d->resetUiToDefaults();
    d->fillDrugsData();
    d->createDrugMapper();
    changeCurrentRow(drugRow);
}

/** \brief Use this function to define a dosageModel behavior. */
void DosageViewer::setDosageModel(DrugsDB::Internal::DosageModel *model)
{
    Q_ASSERT(model);
    d->m_DosageModel = model;
    d->m_DrugId = model->drugId();
    d->resetUiToDefaults();
    d->fillDrugsData();
    d->createDosageMapper();

    if (model->rowCount()==0) {
        model->insertRow(0);
        changeCurrentRow(0);
    } else {
        changeCurrentRow(0);
    }

    // Connect Widgets data modifications to dataChanged signal
    connect(d->m_DosageModel,SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SIGNAL(protocolDataChanged()));
}

/** \brief Destructor */
DosageViewer::~DosageViewer()
{
    if (d) delete d;
    d=0;
}

/** \brief Changes the current editing dosage */
void DosageViewer::changeCurrentRow(const int dosageRow)
{
    if (dosageRow==d->m_Mapper->currentIndex())
        return;

    d->resetUiToDefaults();
    d->m_Mapper->setCurrentIndex(dosageRow);
    d->changeNonMappedDataFromModelToUi(dosageRow);
    d->recalculateDailySchemeMaximum();
    qWarning() << QString("MAIN INN %1 = UID %2").arg(drugModel()->drugData(d->m_DrugId, DrugsDB::Constants::Drug::MainInnName).toString().toUpper()).arg(d->m_DrugId.toString())<<__FILE__<<__LINE__;
}

/** \brief Only provided because of focus bug */
void DosageViewer::commitToModel()
{
    // populate DailyShemeModel
    // TODO: Create and Use DailySchemeViewer Properties
    DrugsDB::DailySchemeModel *daily = dailyScheme->model();
    Q_ASSERT(daily);
    if (d->m_DosageModel) {
        if (daily) {
            d->m_DosageModel->setData(d->m_DosageModel->index(d->m_Mapper->currentIndex(), Dosages::Constants::SerializedDailyScheme), daily->serializedContent());
        }
//        d->m_DosageModel->setData(d->m_DosageModel->index(d->m_Mapper->currentIndex(), Dosages::Constants::Route), routeId);
    } else {
        if (daily) {
            drugModel()->setDrugData(d->m_DrugId, DrugsDB::Constants::Prescription::SerializedDailyScheme, daily->serializedContent());
        }
//        drugModel()->setDrugData(d->m_DrugId, DrugsDB::Constants::Prescription::Route, routeId);
    }
    d->m_Mapper->submit();
}


/** \brief Changes the current editing dosage */
void DosageViewer::changeCurrentRow(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    changeCurrentRow(current.row());
}

/**
   \brief When dialog including mfDosageViewer is validate call this member with the result of the dialog.
   It stores some values inside the users settings.
*/
void DosageViewer::done(int r)
{
    if (r == QDialog::Accepted) {
        // save user's intake forms
        QStringList list = intakesCombo->fancyItems(Constants::USERRECORDEDFORMS_COLOR);
        QStringList doNotInsert = intakesCombo->fancyItems("Model");
        if (!doNotInsert.contains(intakesCombo->currentText()))
            list.insert(0, intakesCombo->currentText());
        list.removeDuplicates();
        list.removeAll(tkTr(Trans::Constants::INTAKES));
        settings()->setValue(S_USERRECORDEDFORMS, list);
        settings()->sync();
    }
}

/** \brief Used for hourly table widget resizing */
void DosageViewer::resizeEvent(QResizeEvent * event)
{
    d->resizeTableWidget();
    QWidget::resizeEvent(event);
}

void DosageViewer::on_fromToIntakesCheck_stateChanged(int state)
{
    if (d->m_DosageModel)
        d->setCheckBoxStateToModel(Dosages::Constants::IntakesUsesFromTo, state);
    else
        d->setCheckBoxStateToModel(DrugsDB::Constants::Prescription::IntakesUsesFromTo, state);
    d->recalculateDailySchemeMaximum();
}

void DosageViewer::on_fromToDurationCheck_stateChanged(int state)
{
    if (d->m_DosageModel)
        d->setCheckBoxStateToModel(Dosages::Constants::DurationUsesFromTo, state);
    else
        d->setCheckBoxStateToModel(DrugsDB::Constants::Prescription::DurationUsesFromTo, state);
}

/** \brief Redefine the minimum of the "to" intakes */
void DosageViewer::on_intakesFromSpin_valueChanged(double value)
{
    if (intakesToSpin->value() < value) {
        intakesToSpin->setValue(value);
    }
    intakesToSpin->setMinimum(value);
    d->recalculateDailySchemeMaximum();
}

/** \brief Redefine the minimum of the "to" duration */
void DosageViewer::on_durationFromSpin_valueChanged(double value)
{
    if (durationToSpin->value() < value) {
        durationToSpin->setValue(value);
    }
    durationToSpin->setMinimum(value);
    d->recalculateDailySchemeMaximum();
}

/** \brief Show a menu with the user recorded forms */
void DosageViewer::on_userformsButton_clicked()
{
    if (settings()->value(S_USERRECORDEDFORMS, QVariant()).isNull())
        return;

    const QStringList &ulist = settings()->value(S_USERRECORDEDFORMS).toStringList();
    QList<QAction*> list;
    foreach(const QString &form, ulist) {
        if (!form.isEmpty())
            list << new QAction(form, this);
    }
    QAction *aclear = new QAction(tr("Clear this list", "Clear the user's intakes recorded forms"), this);
    list << aclear;

    QAction *a = QMenu::exec(list, userformsButton->mapToGlobal(QPoint(0,20)));
    if (!a)
        return;
    if (a == aclear) {
        settings()->setValue(S_USERRECORDEDFORMS, QString());
    } else {
        intakesCombo->setEditText(a->text());
        if (d->m_DosageModel)
            d->m_DosageModel->setData(d->m_DosageModel->index(d->m_Mapper->currentIndex(),Dosages::Constants::IntakesScheme),a->text());
        else
            drugModel()->setDrugData(d->m_DrugId, DrugsDB::Constants::Prescription::IntakesScheme, a->text());
    }
}

/** \brief If INN linking is available, shows the inn name and the dosage used for the link */
void DosageViewer::on_dosageForAllInnCheck_stateChanged(int state)
{
    if (d->m_DosageModel) {
        // INN Prescription ?
        int row = d->m_Mapper->currentIndex();
            if ((dosageForAllInnCheck->isEnabled()) && (state==Qt::Checked)) {
//                qWarning() << "INN" << drugModel()->drugData(d->m_DrugId, DrugsDB::Constants::Drug::MainInnCode);
                d->m_DosageModel->setData(d->m_DosageModel->index(row, Dosages::Constants::DrugUid_LK), d->m_DrugId);
                d->m_DosageModel->setData(d->m_DosageModel->index(row, Dosages::Constants::CIP_LK), -1);
                d->m_DosageModel->setData(d->m_DosageModel->index(row, Dosages::Constants::INN_LK),
                                           drugModel()->drugData(d->m_DrugId, DrugsDB::Constants::Drug::MainInnCode));
                d->m_DosageModel->setData(d->m_DosageModel->index(row, Dosages::Constants::InnLinkedDosage),
                                           drugModel()->drugData(d->m_DrugId, DrugsDB::Constants::Drug::MainInnDosage));
            } else {
//                qWarning() << "not INN";
                d->m_DosageModel->setData(d->m_DosageModel->index(row, Dosages::Constants::DrugUid_LK), d->m_DrugId);
                d->m_DosageModel->setData(d->m_DosageModel->index(row, Dosages::Constants::CIP_LK), -1);
                d->m_DosageModel->setData(d->m_DosageModel->index(row, Dosages::Constants::INN_LK), -1);
                d->m_DosageModel->setData(d->m_DosageModel->index(row, Dosages::Constants::InnLinkedDosage), "");
            }
        innCompositionLabel->show();
        innCompositionLabel->setText(tr("Linking to: ")
                                      + drugModel()->drugData(d->m_DrugId, DrugsDB::Constants::Drug::MainInnName).toString() + " "
//                                      + tr("Dosage of molecule: ")
                                      + drugModel()->drugData(d->m_DrugId, DrugsDB::Constants::Drug::MainInnDosage).toString());
    } else
        innCompositionLabel->hide();
}

void DosageViewer::on_aldCheck_stateChanged(int state)
{
    if (d->m_DosageModel)
        d->setCheckBoxStateToModel(Dosages::Constants::IsALD, state);
    else
        d->setCheckBoxStateToModel(DrugsDB::Constants::Prescription::IsALD, state);
}

void DosageViewer::on_monographButton_clicked()
{
#if DRUGS_DATABASE_VERSION >= 0x000804
    // Get the SPC from the drugs database
    QString file = QString("file://%1").arg(drugsBase().getDrugSpc(d->m_DrugId));
    QDesktopServices::openUrl(QUrl(file));
#else
    // Open the SPC from the web using the link recorded in the database
    QDesktopServices::openUrl(QUrl(drugModel()->drugData(d->m_DrugId, DrugsDB::Constants::Drug::LinkToSCP).toString()));
#endif
}

void DosageViewer::on_tabWidget_currentChanged(int)
{
    if (tabWidget->currentWidget()==tabSchemes)
        d->resizeTableWidget();
}

void DosageViewer::onDailySchemeModelDataChanged(const QModelIndex &index)
{
    Q_UNUSED(index);
    d->recalculateDailySchemeMaximum();
}

