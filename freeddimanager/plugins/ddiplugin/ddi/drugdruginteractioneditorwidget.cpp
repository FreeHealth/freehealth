/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "drugdruginteractioneditorwidget.h"
#include "drugdruginteractiontablemodel.h"
#include <ddiplugin/ddicore.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/isettings.h>

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
#include <QSortFilterProxyModel>

#include "ui_drugdruginteractioneditorwidget.h"

using namespace DDI;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline DDI::DDICore *ddiCore()  { return DDI::DDICore::instance(); }
//static inline QString afssapsIamXmlFile()  {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + Core::Constants::AFSSAPS_INTERACTIONS_FILENAME);}
//static inline QString afssapsNewIamXmlFile()  {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + Core::Constants::NEW_AFSSAPS_INTERACTIONS_FILENAME);}

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

}  // namespace anonymous

namespace DDI {
namespace Internal {
class DrugDrugInteractionEditorWidgetPrivate
{
public:
    DrugDrugInteractionEditorWidgetPrivate(DrugDrugInteractionEditorWidget *parent):
        ui(0),
        m_DDIProxyModel(0),
        m_Mapper(0),
        m_ReviewModified(false),
        aSave(0),
        aEdit(0),
        aRemoveCurrent(0),
        aCreateNew(0),
        aTranslateAll(0),
        aTranslateThis(0),
        aReformatOldXmlSources(0),
        aSplitInteractionAccordingToLevel(0),
        googleTranslator(0),
//        firstInteractorRoutes(0), secondInteractorRoutes(0),
        biblioModel(0),
        q(parent)
    {}

    ~DrugDrugInteractionEditorWidgetPrivate()
    {}

    void setupUi()
    {
        ui = new Ui::DrugDrugInteractionEditorWidget;
        ui->setupUi(q);
        ui->tabWidget->setCurrentIndex(0);
    }

    void createActionsAndToolBar()
    {
        QToolBar *b = new QToolBar(q);
        ui->toolbarLayout->addWidget(b);
        b->setIconSize(QSize(16,16));

        aSave = new QAction(q);
        aRemoveCurrent = new QAction(q);
        aEdit = new QAction(q);
        aTranslateAll = new QAction(q);
        aTranslateThis = new QAction(q);
        aReformatOldXmlSources = new QAction(q);
        aCreateNew = new QAction(q);
        aSplitInteractionAccordingToLevel = new QAction(q);

        aSave->setEnabled(false);
        aEdit->setEnabled(false);
        aTranslateThis->setEnabled(false);
        aRemoveCurrent->setEnabled(false);
        aSplitInteractionAccordingToLevel->setEnabled(false);

        aSave->setIcon(theme()->icon(Core::Constants::ICONSAVE));
        aRemoveCurrent->setIcon(theme()->icon(Core::Constants::ICONREMOVE));
        aEdit->setIcon(theme()->icon(Core::Constants::ICONEDIT));
        aTranslateAll->setIcon(theme()->icon(Core::Constants::ICONPROCESS));
        aTranslateThis->setIcon(theme()->icon(Core::Constants::ICONTRANSLATE));
        aReformatOldXmlSources->setIcon(theme()->icon(Core::Constants::ICONPROCESS));
        aCreateNew->setIcon(theme()->icon(Core::Constants::ICONADD));
        aSplitInteractionAccordingToLevel->setIcon(theme()->icon("splitfile.png"));

        b->addAction(aRemoveCurrent);
        b->addAction(aEdit);
        b->addAction(aTranslateThis);
        b->addAction(aSave);
        b->addAction(aTranslateAll);
        b->addAction(aReformatOldXmlSources);
        b->addAction(aSplitInteractionAccordingToLevel);
    }

    void connectActions()
    {
        QObject::connect(aCreateNew, SIGNAL(triggered()), q, SLOT(createNewDDI()));
        QObject::connect(aSave, SIGNAL(triggered()), q, SLOT(save()));
        QObject::connect(aRemoveCurrent, SIGNAL(triggered()), q, SLOT(removeCurrent()));
        QObject::connect(aEdit, SIGNAL(triggered()), q, SLOT(edit()));
        QObject::connect(aTranslateAll, SIGNAL(triggered()), q, SLOT(translateAll()));
        QObject::connect(aTranslateThis, SIGNAL(triggered()), q, SLOT(translateCurrent()));
        QObject::connect(aReformatOldXmlSources, SIGNAL(triggered()), q, SLOT(reformatOldXmlSource()));
        QObject::connect(aSplitInteractionAccordingToLevel, SIGNAL(triggered()), q, SLOT(splitCurrent()));
    }

    void prepareSearchLine()
    {
        // manage search line
        ui->searchLine->setDelayedSignals(true);
        QToolButton *left = new QToolButton(q);
        left->setIcon(theme()->icon(Core::Constants::ICONSEARCH));
        ui->searchLine->setLeftButton(left);
        QToolButton *right = new QToolButton(q);
        right->addAction(aCreateNew);
        right->setDefaultAction(aCreateNew);
        right->setToolButtonStyle(Qt::ToolButtonIconOnly);
        right->setPopupMode(QToolButton::InstantPopup);
        ui->searchLine->setRightButton(right);
    }

    void setLevelNamesToCombo(QComboBox *box)
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

    void createModelsAndViews()
    {
        // Manage combos && views
        setLevelNamesToCombo(ui->comboLevel);
        ui->firstDoseFromUnits->addItems(DrugDrugInteractionTableModel::units());
        ui->firstDoseFromRepart->addItems(DrugDrugInteractionTableModel::repartitions());
        ui->firstDoseToUnits->addItems(DrugDrugInteractionTableModel::units());
        ui->firstDoseToRepart->addItems(DrugDrugInteractionTableModel::repartitions());
        ui->secondDoseFromUnits->addItems(DrugDrugInteractionTableModel::units());
        ui->secondDoseFromRepart->addItems(DrugDrugInteractionTableModel::repartitions());
        ui->secondDoseToUnits->addItems(DrugDrugInteractionTableModel::units());
        ui->secondDoseToRepart->addItems(DrugDrugInteractionTableModel::repartitions());

//        firstInteractorRoutes = new DrugsDB::RoutesModel(q);
//        ui->listViewFirstInteractorRoute->setModel(firstInteractorRoutes);
//        ui->listViewFirstInteractorRoute->setModelColumn(DrugsDB::RoutesModel::FirstTranslatedName);
//        secondInteractorRoutes = new DrugsDB::RoutesModel(q);
//        ui->listViewSecondInteractorRoute->setModel(secondInteractorRoutes);
//        ui->listViewSecondInteractorRoute->setModelColumn(DrugsDB::RoutesModel::FirstTranslatedName);

        biblioModel = new QStringListModel(q);
        ui->bilbioTableView->setModel(biblioModel);
        ui->bilbioTableView->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
        ui->bilbioTableView->setAlternatingRowColors(true);
        ui->bilbioTableView->horizontalHeader()->hide();
        ui->bilbioTableView->verticalHeader()->hide();

        m_DDIProxyModel = new QSortFilterProxyModel(q);
        m_DDIProxyModel->setSourceModel(ddiCore()->drugDrugInteractionTableModel());
        ui->tableView->setModel(m_DDIProxyModel);
        ui->tableView->setWordWrap(true);
        for(int i = 0; i < m_DDIProxyModel->rowCount(); ++i) {
            ui->tableView->hideColumn(i);
        }
        ui->tableView->showColumn(DrugDrugInteractionTableModel::FirstInteractorUid);
        ui->tableView->showColumn(DrugDrugInteractionTableModel::SecondInteractorUid);
        ui->tableView->showColumn(DrugDrugInteractionTableModel::LevelCode);
        ui->tableView->horizontalHeader()->setResizeMode(DrugDrugInteractionTableModel::FirstInteractorUid, QHeaderView::ResizeToContents);
        ui->tableView->horizontalHeader()->setResizeMode(DrugDrugInteractionTableModel::SecondInteractorUid, QHeaderView::ResizeToContents);
        ui->tableView->horizontalHeader()->setResizeMode(DrugDrugInteractionTableModel::LevelCode, QHeaderView::ResizeToContents);
        ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableView->horizontalHeader()->hide();
    }

    void createMapper()
    {
        // Create DDI Model && manage Mapper
        m_Mapper = new QDataWidgetMapper(q);
        m_Mapper->setModel(m_DDIProxyModel);
        m_Mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

        m_Mapper->addMapping(ui->firstInteractor, DrugDrugInteractionTableModel::FirstInteractorUid, "text");
        m_Mapper->addMapping(ui->secondInteractor, DrugDrugInteractionTableModel::SecondInteractorUid, "text");
        m_Mapper->addMapping(ui->firstInteractor2, DrugDrugInteractionTableModel::FirstInteractorUid, "text");
        m_Mapper->addMapping(ui->secondInteractor2, DrugDrugInteractionTableModel::SecondInteractorUid, "text");

        m_Mapper->addMapping(ui->risk, DrugDrugInteractionTableModel::RiskFr, "plainText");
        m_Mapper->addMapping(ui->risk_en, DrugDrugInteractionTableModel::RiskEn, "plainText");
        m_Mapper->addMapping(ui->management, DrugDrugInteractionTableModel::ManagementFr, "plainText");
        m_Mapper->addMapping(ui->management_en, DrugDrugInteractionTableModel::ManagementEn, "plainText");
        m_Mapper->addMapping(ui->createdOn, DrugDrugInteractionTableModel::DateCreation, "date");
        m_Mapper->addMapping(ui->updatedOn, DrugDrugInteractionTableModel::DateLastUpdate, "date");
        m_Mapper->addMapping(ui->comboLevel, DrugDrugInteractionTableModel::LevelComboIndex, "currentIndex");

        m_Mapper->addMapping(ui->firstDoseFromValue, DrugDrugInteractionTableModel::FirstDoseFromValue, "text");
        m_Mapper->addMapping(ui->firstDoseFromUnits, DrugDrugInteractionTableModel::FirstDoseFromUnits, "currentIndex");
        m_Mapper->addMapping(ui->firstDoseFromRepart, DrugDrugInteractionTableModel::FirstDoseFromRepartition, "currentIndex");
        m_Mapper->addMapping(ui->firstDoseToValue, DrugDrugInteractionTableModel::FirstDoseToValue, "text");
        m_Mapper->addMapping(ui->firstDoseToUnits, DrugDrugInteractionTableModel::FirstDoseToUnits, "currentIndex");
        m_Mapper->addMapping(ui->firstDoseToRepart, DrugDrugInteractionTableModel::FirstDoseToRepartition, "currentIndex");

        m_Mapper->addMapping(ui->secondDoseFromValue, DrugDrugInteractionTableModel::SecondDoseFromValue, "text");
        m_Mapper->addMapping(ui->secondDoseFromUnits, DrugDrugInteractionTableModel::SecondDoseFromUnits, "currentIndex");
        m_Mapper->addMapping(ui->secondDoseFromRepart, DrugDrugInteractionTableModel::SecondDoseFromRepartition, "currentIndex");
        m_Mapper->addMapping(ui->secondDoseToValue, DrugDrugInteractionTableModel::SecondDoseToValue, "text");
        m_Mapper->addMapping(ui->secondDoseToUnits, DrugDrugInteractionTableModel::SecondDoseToUnits, "currentIndex");
        m_Mapper->addMapping(ui->secondDoseToRepart, DrugDrugInteractionTableModel::SecondDoseToRepartition, "currentIndex");

//        m_Mapper->addMapping(ui->humanSynthesis, DrugDrugInteractionTableModel::HumanReadableSynthesis, "html");

        //    m_Mapper->addMapping(ui->comboFirstInteractorRoute, DrugDrugInteractionTableModel::FirstInteractorRouteIndex, "currentIndex");
        //    m_Mapper->addMapping(ui->comboSecondInteractorRoute, DrugDrugInteractionTableModel::SecondInteractorRouteIndex, "currentIndex");
    }

public:
    Ui::DrugDrugInteractionEditorWidget *ui;
    QSortFilterProxyModel *m_DDIProxyModel;
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
    Utils::GoogleTranslator *googleTranslator;
//    DrugsDB::RoutesModel *firstInteractorRoutes, *secondInteractorRoutes;
    QStringListModel *biblioModel;

private:
    DrugDrugInteractionEditorWidget *q;
};
}  // namespace Internal
}  // namespace DDI

DrugDrugInteractionEditorWidget::DrugDrugInteractionEditorWidget(QWidget *parent) :
    QWidget(parent),
    d(new DrugDrugInteractionEditorWidgetPrivate(this))
{
    d->setupUi();
    layout()->setMargin(0);
    layout()->setSpacing(0);

    d->createActionsAndToolBar();
    d->connectActions();
    d->prepareSearchLine();
    d->createModelsAndViews();
    d->createMapper();
    setEditorsEnabled(false);

    connect(d->ui->tableView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(interactionActivated(QModelIndex)));
    connect(d->ui->searchLine, SIGNAL(textChanged(QString)), SLOT(filterDrugDrugInteractionTableModel(QString)));
    retranslateUi();
}

DrugDrugInteractionEditorWidget::~DrugDrugInteractionEditorWidget()
{
    if (d) {
        delete d->ui;
        delete d;
        d=0;
    }
}

/** Enable/Disable the whole editor user interface */
void DrugDrugInteractionEditorWidget::setEditorsEnabled(bool state)
{
    d->ui->generalGroupBox->setEnabled(state);
    d->ui->risk->setEnabled(state);
    d->ui->risk_en->setEnabled(state);
    d->ui->management->setEnabled(state);
    d->ui->management_en->setEnabled(state);
    d->ui->formRiskTableView->setEnabled(state);
    d->ui->bilbioTableView->setEnabled(state);
    d->ui->listViewFirstInteractorRoute->setEnabled(state);
    d->ui->listViewSecondInteractorRoute->setEnabled(state);
    d->ui->firstDoseUsesFrom->setEnabled(state);
    d->ui->firstDoseUsesTo->setEnabled(state);
    d->ui->firstDoseFromValue->setEnabled(state);
    d->ui->firstDoseFromUnits->setEnabled(state);
    d->ui->firstDoseFromRepart->setEnabled(state);
    d->ui->firstDoseToValue->setEnabled(state);
    d->ui->firstDoseToUnits->setEnabled(state);
    d->ui->firstDoseToRepart->setEnabled(state);
    d->ui->secondDoseUsesFrom->setEnabled(state);
    d->ui->secondDoseUsesTo->setEnabled(state);
    d->ui->secondDoseFromValue->setEnabled(state);
    d->ui->secondDoseFromUnits->setEnabled(state);
    d->ui->secondDoseFromRepart->setEnabled(state);
    d->ui->secondDoseToValue->setEnabled(state);
    d->ui->secondDoseToUnits->setEnabled(state);
    d->ui->secondDoseToRepart->setEnabled(state);
}

/** Create a new drugdruginteraction */
void DrugDrugInteractionEditorWidget::createNewDDI()
{
//    // get the first interactor
//    QModelIndex index = d->ui->treeView->currentIndex();
//    while (index.parent().isValid())
//        index = index.parent();

//    DrugDrugInteraction ddi;
//    QString first;
//    // Can not find the first interactor -> ask user
//    if (!index.isValid()) {
//        Internal::InteractorSelectorDialog dlg(this);
//        dlg.setTitle(tr("Select the first interactor"));
//        dlg.initialize();
//        if (dlg.exec() == QDialog::Accepted) {
//            if (dlg.selectedNames().count() == 1)
//                first = dlg.selectedNames().at(0);
//        }
//        if (first.isEmpty())
//            return;
//        ddi.setData(DrugDrugInteraction::FirstInteractorName, first);
//    } else {
//        first = index.data().toString();
//        ddi.setData(DrugDrugInteraction::FirstInteractorName, first);
//    }

//    // ask for the second interactor
//    Internal::InteractorSelectorDialog dlg(this);
//    dlg.setTitle(tr("Select the second interactor (first: %1)").arg(first));
//    dlg.initialize();
//    QString second;
//    if (dlg.exec() == QDialog::Accepted) {
//        if (dlg.selectedNames().count() == 1)
//            second = dlg.selectedNames().at(0);
//    }
//    if (second.isEmpty())
//        return;

//    // ask for a confirmation
//    bool yes = Utils::yesNoMessageBox(tr("Do you really want to create the following drug-drug interaction:<br />"
//                                         "- %1<br />"
//                                         "- %2 <br />").arg(first).arg(second),
//                                      "", "", tr("Drug-drug interaction creation"));
//    if (!yes)
//        return;
//    ddi.setData(DrugDrugInteraction::SecondInteractorName, second);
//    ddi.setData(DrugDrugInteraction::DateCreation, QDate::currentDate());
//    ddi.setData(DrugDrugInteraction::DateLastUpdate, QDate::currentDate());

//    if (!d->m_DDIModel->addDrugDrugInteraction(ddi))
//        LOG_ERROR(QString("Unable to add the DDI: %1 <-> %2").arg(ddi.firstInteractor()));

//    // select the first interactor in the view
//    d->ui->searchLine->setText(first);
//    filterDrugDrugInteractionTableModel(first);
//    LOG(tr("Interaction added: %1 - %2").arg(first).arg(second));
}

/** Filter the DDI model with the search terms \e filter */
void DrugDrugInteractionEditorWidget::filterDrugDrugInteractionTableModel(const QString &filter)
{
    d->m_DDIProxyModel->setFilterRole(Qt::DisplayRole);
    d->m_DDIProxyModel->setFilterKeyColumn(DrugDrugInteractionTableModel::FirstInteractorUid);
    d->m_DDIProxyModel->setFilterFixedString(filter);
}

/** Edit the current selected DDI */
void DrugDrugInteractionEditorWidget::edit()
{
    setEditorsEnabled(true);
}

/**
 * Activated a specific DDI
 */
void DrugDrugInteractionEditorWidget::interactionActivated(const QModelIndex &index)
{
    if (d->m_EditingIndex==index)
        return;

    d->aEdit->setEnabled(true);
    d->aSave->setEnabled(true);
    d->aTranslateThis->setEnabled(true);
    d->aRemoveCurrent->setEnabled(true);
    d->aSplitInteractionAccordingToLevel->setEnabled(true);

    // submit / revert mapper ?
    if (d->ui->risk->isEnabled()) {
        if (Utils::yesNoMessageBox(tr("Data changed but not saved."), tr("Do you want to save changes to the file ?"))) {
            save();
        } else {
            d->m_Mapper->revert();
        }
    }

    setEditorsEnabled(false);

    d->m_Mapper->setCurrentModelIndex(index);

    // set data
    // manage a OSX bug with checkboxes in mappers
//    QModelIndex rev = d->m_DDIModel->index(index.row(), DrugDrugInteractionTableModel::IsReviewedCheckState, index.parent());
//    d->ui->isReviewed->setChecked(rev.data().toBool());
//    QModelIndex dose1from = d->m_DDIModel->index(index.row(), DrugDrugInteractionTableModel::FirstDoseUseFrom, index.parent());
//    d->ui->firstDoseUsesFrom->setChecked(dose1from.data().toBool());
//    QModelIndex dose1to = d->m_DDIModel->index(index.row(), DrugDrugInteractionTableModel::FirstDoseUsesTo, index.parent());
//    d->ui->firstDoseUsesTo->setChecked(dose1to.data().toBool());
//    QModelIndex dose2from = d->m_DDIModel->index(index.row(), DrugDrugInteractionTableModel::SecondDoseUseFrom, index.parent());
//    d->ui->secondDoseUsesFrom->setChecked(dose2from.data().toBool());
//    QModelIndex dose2to = d->m_DDIModel->index(index.row(), DrugDrugInteractionTableModel::SecondDoseUsesTo, index.parent());
//    d->ui->secondDoseUsesTo->setChecked(dose2to.data().toBool());

    // manage routes of interactors
//    d->firstInteractorRoutes->clear();
//    d->secondInteractorRoutes->clear();
//    QModelIndex i1routes = d->m_DDIModel->index(index.row(), DrugDrugInteractionTableModel::FirstInteractorRouteOfAdministrationIds, index.parent());
//    if (!i1routes.data().isNull()) {
//        d->firstInteractorRoutes->setCheckedRouteIds(i1routes.data().toList());
//    }
//    QModelIndex i2routes = d->m_DDIModel->index(index.row(), DrugDrugInteractionTableModel::SecondInteractorRouteOfAdministrationIds, index.parent());
//    if (!i2routes.data().isNull()) {
//        d->secondInteractorRoutes->setCheckedRouteIds(i2routes.data().toList());
//    }
//    // and bibliography
//    QModelIndex pmids = d->m_DDIModel->index(index.row(), DrugDrugInteractionTableModel::PMIDStringList, index.parent());
//    d->biblioModel->setStringList(pmids.data().toStringList());

    // Manage readonly data
//    QModelIndex ro = d->m_DDIModel->index(index.row(), DrugDrugInteractionTableModel::InternalUidWithInteractors, index.parent());
//    d->ui->generalGroupBox->setTitle(ro.data().toString());
//    ro = d->m_DDIModel->index(index.row(), DrugDrugInteractionTableModel::FirstInteractorName, index.parent());
//    d->ui->firstInteractorLabel->setText(ro.data().toString());
//    ro = d->m_DDIModel->index(index.row(), DrugDrugInteractionTableModel::SecondInteractorName, index.parent());
//    d->ui->secondInteractorLabel->setText(ro.data().toString());

    d->m_EditingIndex = index;
}

/** Save the DDI model and all its changes */
void DrugDrugInteractionEditorWidget::save()
{
//    if (d->m_EditingIndex.isValid()) {
//        d->ui->treeView->setFocus();
//        d->m_Mapper->submit();
//        // bug with mapper / checkbox in macos
//        QModelIndex reviewed = d->m_DDIModel->index(d->m_EditingIndex.row(), DrugDrugInteractionTableModel::IsReviewedCheckState,d->m_EditingIndex.parent());
//        d->m_DDIModel->setData(reviewed, d->ui->isReviewed->isChecked());
//        QModelIndex dose1from = d->m_DDIModel->index(d->m_EditingIndex.row(), DrugDrugInteractionTableModel::FirstDoseUseFrom, d->m_EditingIndex.parent());
//        d->m_DDIModel->setData(dose1from, d->ui->firstDoseUsesFrom->isChecked());
//        QModelIndex dose1to = d->m_DDIModel->index(d->m_EditingIndex.row(), DrugDrugInteractionTableModel::FirstDoseUsesTo, d->m_EditingIndex.parent());
//        d->m_DDIModel->setData(dose1to, d->ui->firstDoseUsesTo->isChecked());
//        QModelIndex dose2from = d->m_DDIModel->index(d->m_EditingIndex.row(), DrugDrugInteractionTableModel::SecondDoseUseFrom, d->m_EditingIndex.parent());
//        d->m_DDIModel->setData(dose2from, d->ui->secondDoseUsesFrom->isChecked());
//        QModelIndex dose2to = d->m_DDIModel->index(d->m_EditingIndex.row(), DrugDrugInteractionTableModel::SecondDoseUsesTo, d->m_EditingIndex.parent());
//        d->m_DDIModel->setData(dose2to, d->ui->secondDoseUsesTo->isChecked());

//        // manage interactor routes
//        QModelIndex i1routes = d->m_DDIModel->index(d->m_EditingIndex.row(), DrugDrugInteractionTableModel::FirstInteractorRouteOfAdministrationIds, d->m_EditingIndex.parent());
//        d->m_DDIModel->setData(i1routes, d->firstInteractorRoutes->checkedRouteIdsInVariant());
//        QModelIndex i2routes = d->m_DDIModel->index(d->m_EditingIndex.row(), DrugDrugInteractionTableModel::SecondInteractorRouteOfAdministrationIds, d->m_EditingIndex.parent());
//        d->m_DDIModel->setData(i2routes, d->secondInteractorRoutes->checkedRouteIdsInVariant());

//        // manage bibliography
//        QModelIndex pmids = d->m_DDIModel->index(d->m_EditingIndex.row(), DrugDrugInteractionTableModel::PMIDStringList, d->m_EditingIndex.parent());
//        d->m_DDIModel->setData(pmids, d->biblioModel->stringList());
//    }
//    d->m_DDIModel->saveModel();

//    setEditorsEnabled(false);
}

/** Remove the current selected DDI */
void DrugDrugInteractionEditorWidget::removeCurrent()
{
//    if (d->m_EditingIndex.isValid()) {
//        d->m_DDIProxyModel->removeRow(d->m_EditingIndex.row(), d->m_EditingIndex.parent());
//    }
}

/** Translated DDI texts of the currently selected one */
void DrugDrugInteractionEditorWidget::translateCurrent()
{
//    if (!d->googleTranslator) {
//        d->googleTranslator = new Utils::GoogleTranslator(this);
//        connect(d->googleTranslator, SIGNAL(translationComplete(QString)), this, SLOT(translationDone(QString)));
//    }

//    QModelIndex index = d->ui->treeView->currentIndex();
//    index = d->m_DDIModel->index(index.row(), DrugDrugInteractionTableModel::RiskFr, index.parent());
//    const QString &risk = d->m_DDIModel->data(index).toString();
//    index = d->m_DDIModel->index(index.row(), DrugDrugInteractionTableModel::ManagementFr, index.parent());
//    const QString &management = d->m_DDIModel->data(index).toString();

//    QString trans = risk + ".........." + management;
//    d->googleTranslator->startTranslation("fr", "en", trans);
}

/**
 * \internal
 * Acts when translations are downloaded
 */
void DrugDrugInteractionEditorWidget::translationDone(const QString &trans)
{
//    qWarning() << "translationDone" << trans;
//    if (trans.startsWith(" null")) {
//        return;
//    }

//    QString tmp = trans;
//    tmp.replace("...........", ".||");
//    tmp.replace("..........", "||");
//    QStringList val = tmp.split("||");
//    QString riskEn = val.at(0).simplified();
//    QModelIndex index = d->m_EditingIndex;
//    index = d->m_DDIModel->index(index.row(), DrugDrugInteractionTableModel::RiskEn, index.parent());
//    d->m_DDIModel->setData(index, riskEn);

//    if (val.count() == 2) {
//        QString managementEn = val.at(1).simplified();
//        index = d->m_DDIModel->index(index.row(), DrugDrugInteractionTableModel::ManagementEn, index.parent());
//        d->m_DDIModel->setData(index, managementEn);
//    }
}

/**
 * \internal
 * DISCONNECTED
 */
void DrugDrugInteractionEditorWidget::translateAll()
{
//    InteractionModel *model = InteractionModel::instance();
//    model->correctTranslations();
}

/**
 * \internal
 * When a DDI has multiple levels (contraindication, precaution...)
 * splits this DDI into each level.
 */
void DrugDrugInteractionEditorWidget::splitCurrent()
{
    // Not acting on categories
//    if (d->m_DDIModel->hasChildren(d->m_EditingIndex))
//        return;
//    // Test LevelCode
//    QModelIndex index = d->m_DDIModel->index(d->m_EditingIndex.row(), DrugDrugInteractionTableModel::LevelCode, d->m_EditingIndex.parent());
//    const QString &l = index.data().toString();
//    if (l=="450" || l.size()==1)
//        return;
//    // tell the model to split this interaction
//    d->m_DDIModel->splitMultipleLevelInteraction(d->m_EditingIndex);
//    qWarning() << "Split Me";

}

void DrugDrugInteractionEditorWidget::retranslateUi()
{
    d->aCreateNew->setText(tkTr(Trans::Constants::ADD_TEXT));
    d->aSave->setText(tkTr(Trans::Constants::FILESAVE_TEXT));
    d->aEdit->setText(tkTr(Trans::Constants::M_EDIT_TEXT));
    d->aRemoveCurrent->setText(tkTr(Trans::Constants::REMOVE_TEXT));
    d->aTranslateAll->setText(tr("Translate all untranslated"));
    d->aTranslateThis->setText(tr("Translate current"));
    d->aReformatOldXmlSources->setText(tr("Reformat old XML thesaurus"));
    d->aSplitInteractionAccordingToLevel->setText(tr("Split interaction of multi-level to one interaction by level"));
    d->aCreateNew->setToolTip(d->aCreateNew->text());
    d->aSave->setToolTip(d->aSave->text());
    d->aEdit->setToolTip(d->aEdit->text());
    d->aRemoveCurrent->setToolTip(d->aRemoveCurrent->text());
    d->aTranslateAll->setToolTip(d->aTranslateAll->text());
    d->aTranslateThis->setToolTip(d->aTranslateThis->text());
    d->aReformatOldXmlSources->setToolTip(d->aReformatOldXmlSources->text());
    d->aSplitInteractionAccordingToLevel->setToolTip(d->aSplitInteractionAccordingToLevel->text());
    d->ui->retranslateUi(this);
    int current = d->ui->comboLevel->currentIndex();
    d->setLevelNamesToCombo(d->ui->comboLevel);
    d->ui->comboLevel->setCurrentIndex(current);
}

void DrugDrugInteractionEditorWidget::changeEvent(QEvent *e)
{
    if (e->type()==QEvent::LanguageChange) {
        retranslateUi();
    }
}
