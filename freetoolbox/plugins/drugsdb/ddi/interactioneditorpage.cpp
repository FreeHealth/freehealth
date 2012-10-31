/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "interactioneditorpage.h"
#include "drugdruginteractionmodel.h"
#include "drugdruginteraction.h"
#include "interactorselectordialog.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ftb_constants.h>

#include <drugsdb/tools.h>
#include <drugsdb/routesmodel.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/httpdownloader.h>
#include <translationutils/constants.h>
#include <translationutils/trans_drugs.h>
#include <translationutils/trans_menu.h>
#include <translationutils/trans_msgerror.h>
#include <translationutils/googletranslator.h>

#include <QDataWidgetMapper>
#include <QToolBar>
#include <QDir>
#include <QStringListModel>
#include <QToolButton>

#include "ui_interactioneditorwidget.h"

using namespace IAMDb;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline QString afssapsIamXmlFile()  {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + Core::Constants::AFSSAPS_INTERACTIONS_FILENAME);}
static inline QString afssapsNewIamXmlFile()  {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + Core::Constants::NEW_AFSSAPS_INTERACTIONS_FILENAME);}

namespace {
// INTERACTIONS ICONS
const char * const  INTERACTION_ICONCRITICAL =         "critical.png";      /*!< \brief Themed icon name for Interactions logo. */
const char * const  INTERACTION_ICONDECONSEILLEE =     "unadvised.png";   /*!< \brief Themed icon name for Interactions logo. */
const char * const  INTERACTION_ICONTAKEINTOACCOUNT =  "warning.png";       /*!< \brief Themed icon name for Interactions logo. */
const char * const  INTERACTION_ICONPRECAUTION =       "info.png";          /*!< \brief Themed icon name for Interactions logo. */
const char * const  INTERACTION_ICONINFORMATION =      "information.png";          /*!< \brief Themed icon name for Interactions logo. */
const char * const  INTERACTION_ICONUNKONW =           "help.png";          /*!< \brief Themed icon name for Interactions logo. */
const char * const  INTERACTION_ICONOK =               "ok.png";            /*!< \brief Themed icon name for Interactions logo. */
const char * const  INTERACTION_ICONP450 =             "p450iam.png";            /*!< \brief Themed icon name for Interactions logo. */
const char * const  INTERACTION_ICONGPG =              "gpgiam.png";            /*!< \brief Themed icon name for Interactions logo. */

static void setLevelNamesToCombo(QComboBox *box)
{
    box->clear();
    box->addItem(theme()->icon(INTERACTION_ICONCRITICAL), tkTr(Trans::Constants::CONTRAINDICATION));
    box->addItem(theme()->icon(INTERACTION_ICONDECONSEILLEE), tkTr(Trans::Constants::DISCOURAGED));
    box->addItem(theme()->icon(INTERACTION_ICONP450), tkTr(Trans::Constants::P450_IAM));
    box->addItem(theme()->icon(INTERACTION_ICONGPG), tkTr(Trans::Constants::GPG_IAM));
    box->addItem(theme()->icon(INTERACTION_ICONTAKEINTOACCOUNT), tkTr(Trans::Constants::TAKE_INTO_ACCOUNT));
    box->addItem(theme()->icon(INTERACTION_ICONPRECAUTION), tkTr(Trans::Constants::PRECAUTION_FOR_USE));
    box->addItem(theme()->icon(INTERACTION_ICONINFORMATION), tkTr(Trans::Constants::INFORMATION));
}

}


namespace IAMDb {

class InteractionEditorWidgetPrivate
{
public:
    DrugDrugInteractionModel *m_DDIModel;
    QDataWidgetMapper *m_Mapper;
    QPersistentModelIndex m_EditingIndex;
    bool m_ReviewModified;

    QAction *aSave;
    QAction *aEdit;
    QAction *aRemoveCurrent;
    QAction *aCreateNew;
    QAction *aTranslateAll;
    QAction *aTranslateThis;
    QAction *aReformatOldXmlSources;
    QAction *aSplitInteractionAccordingToLevel;
    QAction *aExpandAll;
    QAction *aCollapseAll;

    Utils::GoogleTranslator *googleTranslator;

    DrugsDB::RoutesModel *firstInteractorRoutes, *secondInteractorRoutes;
    QStringListModel *biblioModel;
};

} // namespace IAMDb

QString InteractionEditorPage::category() const
{
    return tkTr(Trans::Constants::DRUGS) + "|" + tkTr(Trans::Constants::INTERACTIONS_DATABASE);
}

QWidget *InteractionEditorPage::createPage(QWidget *parent)
{
    return new InteractionEditorWidget(parent);
}


InteractionEditorWidget::InteractionEditorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InteractionEditorWidget),
    d(new InteractionEditorWidgetPrivate)
{
    d->googleTranslator = 0;
    ui->setupUi(this);
    // Some ui settings
    layout()->setMargin(0);
    layout()->setSpacing(0);
    ui->treeLayout->setMargin(0);
    ui->treeLayout->setSpacing(0);
    ui->tabWidget->setCurrentIndex(0);

    // Create the toolbar and actions
    QToolBar *b = new QToolBar(this);
    ui->toolbarLayout->addWidget(b);
    b->setIconSize(QSize(16,16));

    d->aSave = new QAction(this);
    d->aRemoveCurrent = new QAction(this);
    d->aEdit = new QAction(this);
    d->aTranslateAll = new QAction(this);
    d->aTranslateThis = new QAction(this);
    d->aReformatOldXmlSources = new QAction(this);
    d->aCreateNew = new QAction(this);
    d->aSplitInteractionAccordingToLevel = new QAction(this);
    d->aExpandAll = new QAction(this);
    d->aCollapseAll = new QAction(this);

    d->aSave->setEnabled(false);
    d->aEdit->setEnabled(false);
    d->aTranslateThis->setEnabled(false);
    d->aRemoveCurrent->setEnabled(false);
    d->aSplitInteractionAccordingToLevel->setEnabled(false);

    d->aSave->setIcon(theme()->icon(Core::Constants::ICONSAVE));
    d->aRemoveCurrent->setIcon(theme()->icon(Core::Constants::ICONREMOVE));
    d->aEdit->setIcon(theme()->icon(Core::Constants::ICONEDIT));
    d->aTranslateAll->setIcon(theme()->icon(Core::Constants::ICONPROCESS));
    d->aTranslateThis->setIcon(theme()->icon(Core::Constants::ICONTRANSLATE));
    d->aReformatOldXmlSources->setIcon(theme()->icon(Core::Constants::ICONPROCESS));
    d->aCreateNew->setIcon(theme()->icon(Core::Constants::ICONADD));
    d->aSplitInteractionAccordingToLevel->setIcon(theme()->icon("splitfile.png"));
    d->aExpandAll->setIcon(theme()->icon(Core::Constants::ICONMOVEDOWNLIGHT));
    d->aCollapseAll->setIcon(theme()->icon(Core::Constants::ICONMOVEUPLIGHT));

    b->addAction(d->aRemoveCurrent);
    b->addAction(d->aEdit);
    b->addAction(d->aTranslateThis);
    b->addAction(d->aSave);
    b->addAction(d->aTranslateAll);
    b->addAction(d->aReformatOldXmlSources);
    b->addAction(d->aSplitInteractionAccordingToLevel);

    connect(d->aCreateNew, SIGNAL(triggered()), this, SLOT(createNewDDI()));
    connect(d->aSave, SIGNAL(triggered()), this, SLOT(save()));
    connect(d->aRemoveCurrent, SIGNAL(triggered()), this, SLOT(removeCurrent()));
    connect(d->aEdit, SIGNAL(triggered()), this, SLOT(edit()));
    connect(d->aTranslateAll, SIGNAL(triggered()), this, SLOT(translateAll()));
    connect(d->aTranslateThis, SIGNAL(triggered()), this, SLOT(translateCurrent()));
    connect(d->aReformatOldXmlSources, SIGNAL(triggered()), this, SLOT(reformatOldXmlSource()));
    connect(d->aSplitInteractionAccordingToLevel, SIGNAL(triggered()), this, SLOT(splitCurrent()));
    connect(d->aExpandAll, SIGNAL(triggered()), ui->treeView, SLOT(expandAll()));
    connect(d->aCollapseAll, SIGNAL(triggered()), ui->treeView, SLOT(collapseAll()));

    // manage search line
    ui->searchLine->setDelayedSignals(true);
    QToolButton *left = new QToolButton(this);
    left->setIcon(theme()->icon(Core::Constants::ICONSEARCH));
    ui->searchLine->setLeftButton(left);
    QToolButton *right = new QToolButton(this);
    right->addAction(d->aCreateNew);
    right->addAction(d->aExpandAll);
    right->addAction(d->aCollapseAll);
    right->setDefaultAction(d->aCreateNew);
    right->setToolButtonStyle(Qt::ToolButtonIconOnly);
    right->setPopupMode(QToolButton::InstantPopup);
    ui->searchLine->setRightButton(right);

    // Manage combos && views
    setLevelNamesToCombo(ui->comboLevel);
    ui->firstDoseFromUnits->addItems(DrugDrugInteractionModel::units());
    ui->firstDoseFromRepart->addItems(DrugDrugInteractionModel::repartitions());
    ui->firstDoseToUnits->addItems(DrugDrugInteractionModel::units());
    ui->firstDoseToRepart->addItems(DrugDrugInteractionModel::repartitions());
    ui->secondDoseFromUnits->addItems(DrugDrugInteractionModel::units());
    ui->secondDoseFromRepart->addItems(DrugDrugInteractionModel::repartitions());
    ui->secondDoseToUnits->addItems(DrugDrugInteractionModel::units());
    ui->secondDoseToRepart->addItems(DrugDrugInteractionModel::repartitions());

    d->firstInteractorRoutes = new DrugsDB::RoutesModel(this);
    ui->listViewFirstInteractorRoute->setModel(d->firstInteractorRoutes);
    ui->listViewFirstInteractorRoute->setModelColumn(DrugsDB::RoutesModel::FirstTranslatedName);
    d->secondInteractorRoutes = new DrugsDB::RoutesModel(this);
    ui->listViewSecondInteractorRoute->setModel(d->secondInteractorRoutes);
    ui->listViewSecondInteractorRoute->setModelColumn(DrugsDB::RoutesModel::FirstTranslatedName);

    d->biblioModel = new QStringListModel(this);
    ui->bilbioTableView->setModel(d->biblioModel);
    ui->bilbioTableView->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    ui->bilbioTableView->setAlternatingRowColors(true);
    ui->bilbioTableView->horizontalHeader()->hide();
    ui->bilbioTableView->verticalHeader()->hide();

    // Create DDI Model && manage Mapper
    d->m_DDIModel = new DrugDrugInteractionModel(this);
    d->m_Mapper = new QDataWidgetMapper(this);
    d->m_Mapper->setModel(d->m_DDIModel);
    d->m_Mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    d->m_Mapper->addMapping(ui->risk, DrugDrugInteractionModel::RiskFr, "plainText");
    d->m_Mapper->addMapping(ui->risk_en, DrugDrugInteractionModel::RiskEn, "plainText");
    d->m_Mapper->addMapping(ui->management, DrugDrugInteractionModel::ManagementFr, "plainText");
    d->m_Mapper->addMapping(ui->management_en, DrugDrugInteractionModel::ManagementEn, "plainText");
    d->m_Mapper->addMapping(ui->createdOn, DrugDrugInteractionModel::DateCreation, "date");
    d->m_Mapper->addMapping(ui->updatedOn, DrugDrugInteractionModel::DateLastUpdate, "date");
    d->m_Mapper->addMapping(ui->comboLevel, DrugDrugInteractionModel::LevelComboIndex, "currentIndex");

    d->m_Mapper->addMapping(ui->firstDoseFromValue, DrugDrugInteractionModel::FirstDoseFromValue, "text");
    d->m_Mapper->addMapping(ui->firstDoseFromUnits, DrugDrugInteractionModel::FirstDoseFromUnits, "currentIndex");
    d->m_Mapper->addMapping(ui->firstDoseFromRepart, DrugDrugInteractionModel::FirstDoseFromRepartition, "currentIndex");
    d->m_Mapper->addMapping(ui->firstDoseToValue, DrugDrugInteractionModel::FirstDoseToValue, "text");
    d->m_Mapper->addMapping(ui->firstDoseToUnits, DrugDrugInteractionModel::FirstDoseToUnits, "currentIndex");
    d->m_Mapper->addMapping(ui->firstDoseToRepart, DrugDrugInteractionModel::FirstDoseToRepartition, "currentIndex");

    d->m_Mapper->addMapping(ui->secondDoseFromValue, DrugDrugInteractionModel::SecondDoseFromValue, "text");
    d->m_Mapper->addMapping(ui->secondDoseFromUnits, DrugDrugInteractionModel::SecondDoseFromUnits, "currentIndex");
    d->m_Mapper->addMapping(ui->secondDoseFromRepart, DrugDrugInteractionModel::SecondDoseFromRepartition, "currentIndex");
    d->m_Mapper->addMapping(ui->secondDoseToValue, DrugDrugInteractionModel::SecondDoseToValue, "text");
    d->m_Mapper->addMapping(ui->secondDoseToUnits, DrugDrugInteractionModel::SecondDoseToUnits, "currentIndex");
    d->m_Mapper->addMapping(ui->secondDoseToRepart, DrugDrugInteractionModel::SecondDoseToRepartition, "currentIndex");

    d->m_Mapper->addMapping(ui->humanSynthesis, DrugDrugInteractionModel::HumanReadableSynthesis, "html");

    //    d->m_Mapper->addMapping(ui->comboFirstInteractorRoute, DrugDrugInteractionModel::FirstInteractorRouteIndex, "currentIndex");
//    d->m_Mapper->addMapping(ui->comboSecondInteractorRoute, DrugDrugInteractionModel::SecondInteractorRouteIndex, "currentIndex");

    // Some settings on the treeView
    ui->treeView->setModel(d->m_DDIModel);
    ui->treeView->setWordWrap(true);
    for(int i = 0; i < d->m_DDIModel->rowCount(); ++i) {
        ui->treeView->hideColumn(i);
    }
    ui->treeView->showColumn(0);
    ui->treeView->header()->setResizeMode(0, QHeaderView::ResizeToContents);
    ui->treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->treeView->header()->hide();
    connect(ui->treeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(interactionActivated(QModelIndex)));

    setEditorsEnabled(false);
    connect(ui->searchLine, SIGNAL(textChanged(QString)), SLOT(filterDrugDrugInteractionModel(QString)));
}

InteractionEditorWidget::~InteractionEditorWidget()
{
    delete ui; ui=0;
    if (d) {
        delete d; d=0;
    }
}

void InteractionEditorWidget::setEditorsEnabled(bool state)
{
    ui->generalGroupBox->setEnabled(state);
    ui->risk->setEnabled(state);
    ui->risk_en->setEnabled(state);
    ui->management->setEnabled(state);
    ui->management_en->setEnabled(state);
    ui->formRiskTableView->setEnabled(state);
    ui->bilbioTableView->setEnabled(state);
    ui->listViewFirstInteractorRoute->setEnabled(state);
    ui->listViewSecondInteractorRoute->setEnabled(state);
    ui->firstDoseUsesFrom->setEnabled(state);
    ui->firstDoseUsesTo->setEnabled(state);
    ui->firstDoseFromValue->setEnabled(state);
    ui->firstDoseFromUnits->setEnabled(state);
    ui->firstDoseFromRepart->setEnabled(state);
    ui->firstDoseToValue->setEnabled(state);
    ui->firstDoseToUnits->setEnabled(state);
    ui->firstDoseToRepart->setEnabled(state);
    ui->secondDoseUsesFrom->setEnabled(state);
    ui->secondDoseUsesTo->setEnabled(state);
    ui->secondDoseFromValue->setEnabled(state);
    ui->secondDoseFromUnits->setEnabled(state);
    ui->secondDoseFromRepart->setEnabled(state);
    ui->secondDoseToValue->setEnabled(state);
    ui->secondDoseToUnits->setEnabled(state);
    ui->secondDoseToRepart->setEnabled(state);
}

/** Create a new drugdruginteraction */
void InteractionEditorWidget::createNewDDI()
{
    // get the first interactor
    QString first;
    QModelIndex index = ui->treeView->currentIndex();
    while (index.parent().isValid())
        index = index.parent();

    // Can not find the first interactor -> ask user
    if (!index.isValid()) {
        Internal::InteractorSelectorDialog dlg(this);
        dlg.setTitle(tr("Select the first interactor"));
        dlg.initialize();
        if (dlg.exec() == QDialog::Accepted) {
            if (dlg.selectedNames().count() == 1)
                first = dlg.selectedNames().at(0);
        }
        if (first.isEmpty())
            return;
        // find the corresponding index in the interactor class model
        QModelIndexList list = d->m_DDIModel->match(d->m_DDIModel->index(0,0), Qt::DisplayRole, first, 1, Qt::MatchFixedString | Qt::MatchWrap | Qt::MatchRecursive);
        if (list.count() > 0)
            index = list.at(0);
    } else {
        first = index.data().toString();
    }

    qWarning() << "first" << index << index.data() << first;

    // ask for the second interactor
    QString second;
    Internal::InteractorSelectorDialog dlg(this);
    dlg.setTitle(tr("Select the second interactor (first: %1)").arg(first));
    dlg.initialize();
    if (dlg.exec() == QDialog::Accepted) {
        if (dlg.selectedNames().count() == 1)
            second = dlg.selectedNames().at(0);
    }
    if (second.isEmpty())
        return;

    // ask for a confirmation
    bool yes = Utils::yesNoMessageBox(tr("Do you really want to create the following drug-drug interaction:<br />"
                                         "- %1<br />"
                                         "- %2 <br />").arg(first).arg(second),
                                      "", "", tr("Drug-drug interaction creation"));
    if (!yes)
        return;

    // no corresponding index for the first interactor -> create one
    if (!index.isValid()) {
        LOG("First interactor does not already have interactions. Trying to create one");
        if (!d->m_DDIModel->insertRow(0, QModelIndex()))
            return;
        index = d->m_DDIModel->index(0, DrugDrugInteractionModel::GeneralLabel);
        d->m_DDIModel->setData(index, first);
    }

    // insert row to model
    if (!d->m_DDIModel->insertRow(0, index))
        return;
    QModelIndex secondIndex = d->m_DDIModel->index(0, DrugDrugInteractionModel::SecondInteractorName, index);
    if (!d->m_DDIModel->setData(secondIndex, second))
        return;

    qWarning() << "second" << secondIndex << secondIndex.data() << second;

    // select the first interactor in the view
    ui->searchLine->setText(first);
    filterDrugDrugInteractionModel(first);
    LOG(tr("Interaction added: %1 - %2").arg(first).arg(second));
}

void InteractionEditorWidget::filterDrugDrugInteractionModel(const QString &filter)
{
    d->m_DDIModel->filterInteractionsForInteractor(filter);
    ui->treeView->expandAll();
}

void InteractionEditorWidget::edit()
{
    if (d->m_DDIModel->hasChildren(ui->treeView->currentIndex()))
        setEditorsEnabled(false);
    setEditorsEnabled(true);
}

void InteractionEditorWidget::interactionActivated(const QModelIndex &index)
{
    if (d->m_EditingIndex==index)
        return;

    d->aEdit->setEnabled(true);
    d->aSave->setEnabled(true);
    d->aTranslateThis->setEnabled(true);
    d->aRemoveCurrent->setEnabled(true);
    d->aSplitInteractionAccordingToLevel->setEnabled(true);

    // submit / revert mapper ?
    if (ui->risk->isEnabled()) {
        if (Utils::yesNoMessageBox(tr("Data changed but not saved."), tr("Do you want to save changes to the file ?"))) {
            save();
        } else {
            d->m_Mapper->revert();
        }
    }

    setEditorsEnabled(false);

    d->m_Mapper->setRootIndex(index.parent());
    d->m_Mapper->setCurrentModelIndex(index);

    // set data
    // manage a OSX bug with checkboxes in mappers
    QModelIndex rev = d->m_DDIModel->index(index.row(), DrugDrugInteractionModel::IsReviewedCheckState, index.parent());
    ui->isReviewed->setChecked(rev.data().toBool());
    QModelIndex dose1from = d->m_DDIModel->index(index.row(), DrugDrugInteractionModel::FirstDoseUseFrom, index.parent());
    ui->firstDoseUsesFrom->setChecked(dose1from.data().toBool());
    QModelIndex dose1to = d->m_DDIModel->index(index.row(), DrugDrugInteractionModel::FirstDoseUsesTo, index.parent());
    ui->firstDoseUsesTo->setChecked(dose1to.data().toBool());
    QModelIndex dose2from = d->m_DDIModel->index(index.row(), DrugDrugInteractionModel::SecondDoseUseFrom, index.parent());
    ui->secondDoseUsesFrom->setChecked(dose2from.data().toBool());
    QModelIndex dose2to = d->m_DDIModel->index(index.row(), DrugDrugInteractionModel::SecondDoseUsesTo, index.parent());
    ui->secondDoseUsesTo->setChecked(dose2to.data().toBool());

    // manage routes of interactors
    d->firstInteractorRoutes->clear();
    d->secondInteractorRoutes->clear();
    QModelIndex i1routes = d->m_DDIModel->index(index.row(), DrugDrugInteractionModel::FirstInteractorRouteOfAdministrationIds, index.parent());
    if (!i1routes.data().isNull()) {
        d->firstInteractorRoutes->setCheckedRouteIds(i1routes.data().toList());
    }
    QModelIndex i2routes = d->m_DDIModel->index(index.row(), DrugDrugInteractionModel::SecondInteractorRouteOfAdministrationIds, index.parent());
    if (!i2routes.data().isNull()) {
        d->secondInteractorRoutes->setCheckedRouteIds(i2routes.data().toList());
    }
    // and bibliography
    QModelIndex pmids = d->m_DDIModel->index(index.row(), DrugDrugInteractionModel::PMIDStringList, index.parent());
    d->biblioModel->setStringList(pmids.data().toStringList());

    // Manage readonly data
    QModelIndex ro = d->m_DDIModel->index(index.row(), DrugDrugInteractionModel::InternalUidWithInteractors, index.parent());
    ui->generalGroupBox->setTitle(ro.data().toString());
    ro = d->m_DDIModel->index(index.row(), DrugDrugInteractionModel::FirstInteractorName, index.parent());
    ui->firstInteractorLabel->setText(ro.data().toString());
    ro = d->m_DDIModel->index(index.row(), DrugDrugInteractionModel::SecondInteractorName, index.parent());
    ui->secondInteractorLabel->setText(ro.data().toString());

    d->m_EditingIndex = index;
}

void InteractionEditorWidget::save()
{
    if (d->m_EditingIndex.isValid()) {
        ui->treeView->setFocus();
        d->m_Mapper->submit();
        // bug with mapper / checkbox in macos
        QModelIndex reviewed = d->m_DDIModel->index(d->m_EditingIndex.row(), DrugDrugInteractionModel::IsReviewedCheckState,d->m_EditingIndex.parent());
        d->m_DDIModel->setData(reviewed, ui->isReviewed->isChecked());
        QModelIndex dose1from = d->m_DDIModel->index(d->m_EditingIndex.row(), DrugDrugInteractionModel::FirstDoseUseFrom, d->m_EditingIndex.parent());
        d->m_DDIModel->setData(dose1from, ui->firstDoseUsesFrom->isChecked());
        QModelIndex dose1to = d->m_DDIModel->index(d->m_EditingIndex.row(), DrugDrugInteractionModel::FirstDoseUsesTo, d->m_EditingIndex.parent());
        d->m_DDIModel->setData(dose1to, ui->firstDoseUsesTo->isChecked());
        QModelIndex dose2from = d->m_DDIModel->index(d->m_EditingIndex.row(), DrugDrugInteractionModel::SecondDoseUseFrom, d->m_EditingIndex.parent());
        d->m_DDIModel->setData(dose2from, ui->secondDoseUsesFrom->isChecked());
        QModelIndex dose2to = d->m_DDIModel->index(d->m_EditingIndex.row(), DrugDrugInteractionModel::SecondDoseUsesTo, d->m_EditingIndex.parent());
        d->m_DDIModel->setData(dose2to, ui->secondDoseUsesTo->isChecked());

        // manage interactor routes
        QModelIndex i1routes = d->m_DDIModel->index(d->m_EditingIndex.row(), DrugDrugInteractionModel::FirstInteractorRouteOfAdministrationIds, d->m_EditingIndex.parent());
        d->m_DDIModel->setData(i1routes, d->firstInteractorRoutes->checkedRouteIdsInVariant());
        QModelIndex i2routes = d->m_DDIModel->index(d->m_EditingIndex.row(), DrugDrugInteractionModel::SecondInteractorRouteOfAdministrationIds, d->m_EditingIndex.parent());
        d->m_DDIModel->setData(i2routes, d->secondInteractorRoutes->checkedRouteIdsInVariant());

        // manage bibliography
        QModelIndex pmids = d->m_DDIModel->index(d->m_EditingIndex.row(), DrugDrugInteractionModel::PMIDStringList, d->m_EditingIndex.parent());
        d->m_DDIModel->setData(pmids, d->biblioModel->stringList());
    }
    d->m_DDIModel->saveModel();

    setEditorsEnabled(false);
}

void InteractionEditorWidget::removeCurrent()
{
    if (d->m_EditingIndex.isValid()) {
        d->m_DDIModel->removeRow(d->m_EditingIndex.row(), d->m_EditingIndex.parent());
    }
}

void InteractionEditorWidget::translateCurrent()
{
    if (!d->googleTranslator) {
        d->googleTranslator = new Utils::GoogleTranslator(this);
        connect(d->googleTranslator, SIGNAL(translationComplete(QString)), this, SLOT(translationDone(QString)));
    }

    QModelIndex index = ui->treeView->currentIndex();
    index = d->m_DDIModel->index(index.row(), DrugDrugInteractionModel::RiskFr, index.parent());
    const QString &risk = d->m_DDIModel->data(index).toString();
    index = d->m_DDIModel->index(index.row(), DrugDrugInteractionModel::ManagementFr, index.parent());
    const QString &management = d->m_DDIModel->data(index).toString();

    QString trans = risk + ".........." + management;
    d->googleTranslator->startTranslation("fr", "en", trans);
}

void InteractionEditorWidget::translationDone(const QString &trans)
{
    qWarning() << "translationDone" << trans;
    if (trans.startsWith(" null")) {
        return;
    }

    QString tmp = trans;
    tmp.replace("...........", ".||");
    tmp.replace("..........", "||");
    QStringList val = tmp.split("||");
    QString riskEn = val.at(0).simplified();
    QModelIndex index = d->m_EditingIndex;
    index = d->m_DDIModel->index(index.row(), DrugDrugInteractionModel::RiskEn, index.parent());
    d->m_DDIModel->setData(index, riskEn);

    if (val.count() == 2) {
        QString managementEn = val.at(1).simplified();
        index = d->m_DDIModel->index(index.row(), DrugDrugInteractionModel::ManagementEn, index.parent());
        d->m_DDIModel->setData(index, managementEn);
    }
}

void InteractionEditorWidget::translateAll()
{
//    InteractionModel *model = InteractionModel::instance();
//    model->correctTranslations();
}

void InteractionEditorWidget::reformatOldXmlSource()
{
    // read the old thesaurus XML
    QDomDocument doc;
    QFile file(afssapsIamXmlFile());
    if (file.open(QIODevice::ReadOnly)) {
        QString error;
        int line, col;
        if (!doc.setContent(&file, &error,&line,&col)) {
            LOG_ERROR(tr("Can not read XML file content %1").arg(file.fileName()));
            LOG_ERROR(QString("DOM(%1;%2): %3").arg(line).arg(col).arg(error));
        } else {
            LOG(tr("Reading file: %1").arg(file.fileName()));
        }
        file.close();
    } else {
        LOG_ERROR(tr("Can not open XML file %1").arg(file.fileName()));
    }
    QDomElement rootNode = doc.firstChildElement("InteractionModel");
    QDomElement mainNode = rootNode.firstChildElement("MainInteractor");

    QList<DrugDrugInteraction> ddis;
    QTime chrono;
    chrono.start();

    // create the drugdruginteractions objects
    while (!mainNode.isNull()) {

        // get the second interaction
        QDomElement interactorNode = mainNode.firstChildElement("Interactor");
        while (!interactorNode.isNull()) {
            DrugDrugInteraction ddi;
            ddi.setData(DrugDrugInteraction::FirstInteractorName, mainNode.attribute("name"));
            ddi.setData(DrugDrugInteraction::SecondInteractorName, interactorNode.attribute("name"));
            ddi.setData(DrugDrugInteraction::LevelCode, interactorNode.attribute("level"));

            if (mainNode.attribute("added").isEmpty()) {
                if (interactorNode.attribute("added").isEmpty()) {
                    ddi.setData(DrugDrugInteraction::DateCreation, "01-01-2010");
                } else {
                    ddi.setData(DrugDrugInteraction::DateCreation, interactorNode.attribute("added"));
                }
            } else {
                ddi.setData(DrugDrugInteraction::DateCreation, mainNode.attribute("added"));
            }

            if (mainNode.attribute("updated").isEmpty()) {
                if (!interactorNode.attribute("updated").isEmpty()) {
                    ddi.setData(DrugDrugInteraction::DateLastUpdate, "01-01-2010");
                }
            } else {
                ddi.setData(DrugDrugInteraction::DateLastUpdate, mainNode.attribute("updated"));
            }

            // get risk // management
            QDomElement riskNode = interactorNode.firstChildElement("Risk");
            while (!riskNode.isNull()) {
                ddi.setRisk(riskNode.attribute("text"), riskNode.attribute("lang"));
                riskNode = riskNode.nextSiblingElement("Risk");
            }

            QDomElement managementNode = interactorNode.firstChildElement("Management");
            while (!managementNode.isNull()) {
                ddi.setManagement(managementNode.attribute("text"), managementNode.attribute("lang"));
                managementNode = managementNode.nextSiblingElement("Management");
            }

            // get FormalizedRisk
            QDomElement formalizedNode = interactorNode.firstChildElement("FormalizedRisk");
            if (!formalizedNode.isNull()) {
                QDomNamedNodeMap attributeMap = formalizedNode.attributes();
                for(int i=0; i < attributeMap.size(); ++i) {
                    ddi.addFormalized(attributeMap.item(i).nodeName(), attributeMap.item(i).nodeValue());
                }
            }

            ddis << ddi;

            // go next interactor
            interactorNode = interactorNode.nextSiblingElement("Interactor");
        }

        // go next maininteractor
        mainNode = mainNode.nextSiblingElement("MainInteractor");
//        if (ddis.count() > 1000) {
//            break;
//        }
    }

    Utils::Log::logTimeElapsed(chrono, "DDI", "Sources reading");
    chrono.restart();

    // check duplicates
    QList<int> remove;
    for(int i=0; i < ddis.count(); ++i) {
        DrugDrugInteraction &first = ddis[i];
        if (first.data(DrugDrugInteraction::IsDuplicated).toBool()) {
            continue;
        }

        for(int j=0; j < ddis.count(); ++j) {
            // don't test the same DDI...
            if (i==j) {
                continue;
            }
            DrugDrugInteraction &second = ddis[j];
            if (second.data(DrugDrugInteraction::IsDuplicated).toBool()) {
                continue;
            }
            if (first==second) {
                // Test the DateLastUpdate on the duplicates an keep the most recent one
                if (first.data(DrugDrugInteraction::DateLastUpdate).toDate() > second.data(DrugDrugInteraction::DateLastUpdate).toDate()) {
                    // keep first (i)
                    remove << j;
                    second.setData(DrugDrugInteraction::IsDuplicated, true);
                } else {
                    // keep second (j)
                    remove << i;
                    first.setData(DrugDrugInteraction::IsDuplicated, true);
//                    break;
                }
//                qWarning() << "Duplication "
//                           << first.data(DrugDrugInteraction::FirstInteractorName).toString()
//                           << first.data(DrugDrugInteraction::SecondInteractorName).toString()
//                           << first.data(DrugDrugInteraction::LevelName).toString();
            }
        }
    }

    Utils::Log::logTimeElapsed(chrono, "DDI", "Remove duplicates: " + QString::number(remove.count()));
    chrono.restart();

    qSort(ddis.begin(), ddis.end(), DrugDrugInteraction::lowerThan);

    // save the resulting XML
    QString xml;
    for(int i=0; i < ddis.count(); ++i) {
//        if (remove.contains(i))
//            continue;
        if (ddis.at(i).data(DrugDrugInteraction::IsDuplicated).toBool()) {
            qWarning() << "**" << ddis.at(i).data(DrugDrugInteraction::FirstInteractorName).toString()
                          << ddis.at(i).data(DrugDrugInteraction::SecondInteractorName).toString();
            continue;
        }
        xml += ddis.at(i).toXml();
    }

    Utils::Log::logTimeElapsed(chrono, "DDI", "to Xml");
    xml.prepend("<?xml version='1.0' encoding='UTF-8'?>\n"
            "<!-- date format = yyyy-MM-dd -->\n"
            "<!--\n"
            "  Interaction levels :\n"
            "     P: Precaution\n"
            "     C: ContreIndication\n"
            "     D: Deconseille\n"
            "     T: APrendreEnCompte Take into Account\n"
            "     450: P450\n"
            "     I: Information\n"
            "     Y: Glycoprotein P\n"
            "-->\n"
            "<DrugDrugInteractions>\n");
    xml.append("</DrugDrugInteractions>\n");
    Utils::saveStringToFile(xml, afssapsNewIamXmlFile());
}

void InteractionEditorWidget::splitCurrent()
{
    // Not acting on categories
    if (d->m_DDIModel->hasChildren(d->m_EditingIndex))
        return;
    // Test LevelCode
    QModelIndex index = d->m_DDIModel->index(d->m_EditingIndex.row(), DrugDrugInteractionModel::LevelCode, d->m_EditingIndex.parent());
    const QString &l = index.data().toString();
    if (l=="450" || l.size()==1)
        return;
    // tell the model to split this interaction
    d->m_DDIModel->splitMultipleLevelInteraction(d->m_EditingIndex);
    qWarning() << "Split Me";

}

void InteractionEditorWidget::changeEvent(QEvent *e)
{
    if (e->type()==QEvent::LanguageChange) {
        d->aCreateNew->setText(tkTr(Trans::Constants::ADD_TEXT));
        d->aSave->setText(tkTr(Trans::Constants::FILESAVE_TEXT));
        d->aEdit->setText(tkTr(Trans::Constants::M_EDIT_TEXT));
        d->aRemoveCurrent->setText(tkTr(Trans::Constants::REMOVE_TEXT));
        d->aTranslateAll->setText(tr("Translate all untranslated"));
        d->aTranslateThis->setText(tr("Translate current"));
        d->aReformatOldXmlSources->setText(tr("Reformat old XML thesaurus"));
        d->aSplitInteractionAccordingToLevel->setText(tr("Split interaction of multi-level to one interaction by level"));
        d->aCollapseAll->setText(tr("Collapse all"));
        d->aExpandAll->setText(tr("Expand all"));
        d->aCreateNew->setToolTip(d->aCreateNew->text());
        d->aSave->setToolTip(d->aSave->text());
        d->aEdit->setToolTip(d->aEdit->text());
        d->aRemoveCurrent->setToolTip(d->aRemoveCurrent->text());
        d->aTranslateAll->setToolTip(d->aTranslateAll->text());
        d->aTranslateThis->setToolTip(d->aTranslateThis->text());
        d->aReformatOldXmlSources->setToolTip(d->aReformatOldXmlSources->text());
        d->aSplitInteractionAccordingToLevel->setToolTip(d->aSplitInteractionAccordingToLevel->text());
        d->aCollapseAll->setToolTip(d->aCollapseAll->text());
        d->aExpandAll->setToolTip(d->aExpandAll->text());
        ui->retranslateUi(this);
        int current = ui->comboLevel->currentIndex();
        setLevelNamesToCombo(ui->comboLevel);
        ui->comboLevel->setCurrentIndex(current);
    }
}
