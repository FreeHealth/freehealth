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
#include <ddiplugin/routes/routesmodel.h>
#include <ddiplugin/interactors/interactorselectordialog.h>

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
        aSplitInteractionAccordingToLevel(0),
        aNextProblematic(0), aNextMultiLevel(0), aNextUntranslated(0), aNextUnreviewed(0),
        _nextButton(0),
        googleTranslator(0),
        firstInteractorRoutes(0), secondInteractorRoutes(0),
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
        aCreateNew = new QAction(q);
        aSplitInteractionAccordingToLevel = new QAction(q);
        aNextProblematic = new QAction(q);
        aNextMultiLevel = new QAction(q);
        aNextUntranslated = new QAction(q);
        aNextUnreviewed = new QAction(q);

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
        aCreateNew->setIcon(theme()->icon(Core::Constants::ICONADD));
        aSplitInteractionAccordingToLevel->setIcon(theme()->icon(Core::Constants::ICONADD));
        aNextProblematic->setIcon(theme()->icon(Core::Constants::ICONNEXT));
        aNextMultiLevel->setIcon(theme()->icon(Core::Constants::ICONNEXT));
        aNextUntranslated->setIcon(theme()->icon(Core::Constants::ICONNEXT));
        aNextUnreviewed->setIcon(theme()->icon(Core::Constants::ICONNEXT));

        _nextButton = new QToolButton(q);
        _nextButton->addAction(aNextProblematic);
        _nextButton->addAction(aNextMultiLevel);
        _nextButton->addAction(aNextUnreviewed);
        _nextButton->addAction(aNextUntranslated);
        _nextButton->setPopupMode(QToolButton::MenuButtonPopup);
        _nextButton->setDefaultAction(aNextProblematic);

        b->addWidget(_nextButton);
        b->addSeparator();
        b->addAction(aCreateNew);
        b->addSeparator();
        b->addAction(aRemoveCurrent);
        b->addSeparator();
        b->addAction(aEdit);
        b->addAction(aSplitInteractionAccordingToLevel);
        b->addAction(aTranslateThis);
        b->addSeparator();
        b->addAction(aSave);
        b->addAction(aTranslateAll);
    }

    void connectActions()
    {
        QObject::connect(aCreateNew, SIGNAL(triggered()), q, SLOT(createNewDDI()));
        QObject::connect(aSave, SIGNAL(triggered()), q, SLOT(save()));
        QObject::connect(aRemoveCurrent, SIGNAL(triggered()), q, SLOT(removeCurrent()));
        QObject::connect(aEdit, SIGNAL(triggered()), q, SLOT(edit()));
        QObject::connect(aTranslateAll, SIGNAL(triggered()), q, SLOT(translateAll()));
        QObject::connect(aTranslateThis, SIGNAL(triggered()), q, SLOT(translateCurrent()));
        QObject::connect(aSplitInteractionAccordingToLevel, SIGNAL(triggered()), q, SLOT(splitCurrent()));
        QObject::connect(_nextButton, SIGNAL(triggered(QAction*)), q, SLOT(onNextActionTriggered(QAction*)));
    }

    void prepareSearchLine()
    {
        // manage search line
        ui->searchLine->setDelayedSignals(true);
        QToolButton *left = new QToolButton(q);
        left->setIcon(theme()->icon(Core::Constants::ICONSEARCH));
        ui->searchLine->setLeftButton(left);
//        QToolButton *right = new QToolButton(q);
//        right->addAction(aCreateNew);
//        right->setDefaultAction(aCreateNew);
//        right->setToolButtonStyle(Qt::ToolButtonIconOnly);
//        right->setPopupMode(QToolButton::InstantPopup);
//        ui->searchLine->setRightButton(right);
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

        firstInteractorRoutes = new DDI::RoutesModel(q);
        ui->listViewFirstInteractorRoute->setModel(firstInteractorRoutes);
        ui->listViewFirstInteractorRoute->setModelColumn(DDI::RoutesModel::FirstTranslatedName);
        secondInteractorRoutes = new DDI::RoutesModel(q);
        ui->listViewSecondInteractorRoute->setModel(secondInteractorRoutes);
        ui->listViewSecondInteractorRoute->setModelColumn(DDI::RoutesModel::FirstTranslatedName);

        biblioModel = new QStringListModel(q);
        ui->bilbioTableView->setModel(biblioModel);
        ui->bilbioTableView->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
        ui->bilbioTableView->setAlternatingRowColors(true);
        ui->bilbioTableView->horizontalHeader()->hide();
        ui->bilbioTableView->verticalHeader()->hide();

        m_DDIProxyModel = new QSortFilterProxyModel(q);
        m_DDIProxyModel->setSourceModel(ddiCore()->drugDrugInteractionTableModel());
        m_DDIProxyModel->setDynamicSortFilter(true);
        ui->tableView->setModel(m_DDIProxyModel);
        ui->tableView->setWordWrap(true);
        for(int i = 0; i < m_DDIProxyModel->rowCount(); ++i) {
            ui->tableView->hideColumn(i);
        }
        ui->tableView->showColumn(DrugDrugInteractionTableModel::FirstInteractorUid);
        ui->tableView->showColumn(DrugDrugInteractionTableModel::SecondInteractorUid);
        ui->tableView->showColumn(DrugDrugInteractionTableModel::LevelCode);
        ui->tableView->horizontalHeader()->setResizeMode(DrugDrugInteractionTableModel::FirstInteractorUid, QHeaderView::Stretch);
        ui->tableView->horizontalHeader()->setResizeMode(DrugDrugInteractionTableModel::SecondInteractorUid, QHeaderView::Stretch);
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

        m_Mapper->addMapping(ui->firstDoseUsesFrom, DrugDrugInteractionTableModel::FirstDoseUseFrom, "checked");
        m_Mapper->addMapping(ui->firstDoseUsesTo, DrugDrugInteractionTableModel::FirstDoseUsesTo, "checked");
        m_Mapper->addMapping(ui->firstDoseFromValue, DrugDrugInteractionTableModel::FirstDoseFromValue, "text");
        m_Mapper->addMapping(ui->firstDoseFromUnits, DrugDrugInteractionTableModel::FirstDoseFromUnits, "currentIndex");
        m_Mapper->addMapping(ui->firstDoseFromRepart, DrugDrugInteractionTableModel::FirstDoseFromRepartition, "currentIndex");
        m_Mapper->addMapping(ui->firstDoseToValue, DrugDrugInteractionTableModel::FirstDoseToValue, "text");
        m_Mapper->addMapping(ui->firstDoseToUnits, DrugDrugInteractionTableModel::FirstDoseToUnits, "currentIndex");
        m_Mapper->addMapping(ui->firstDoseToRepart, DrugDrugInteractionTableModel::FirstDoseToRepartition, "currentIndex");

        m_Mapper->addMapping(ui->secondDoseUsesFrom, DrugDrugInteractionTableModel::SecondDoseUseFrom, "checked");
        m_Mapper->addMapping(ui->secondDoseUsesTo, DrugDrugInteractionTableModel::SecondDoseUsesTo, "checked");
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

    void clearUi()
    {
        ui->firstInteractor->clear();
        ui->secondInteractor->clear();
        ui->firstInteractor2->clear();
        ui->secondInteractor2->clear();
        ui->humanSynthesis->clear();
        ui->comboLevel->setCurrentIndex(-1);
        ui->createdOn->clear();
        ui->updatedOn->clear();
        ui->isReviewed->setChecked(false);
        ui->risk->clear();
        ui->risk_en->clear();
        ui->management->clear();
        ui->management_en->clear();
        biblioModel->setStringList(QStringList());
        firstInteractorRoutes->setCheckedRouteIds(QList<QVariant>());
        secondInteractorRoutes->setCheckedRouteIds(QList<QVariant>());
        ui->firstDoseUsesFrom->setChecked(false);
        ui->firstDoseUsesTo->setChecked(false);
        ui->firstDoseFromValue->clear();
        ui->firstDoseFromUnits->setCurrentIndex(-1);
        ui->firstDoseFromRepart->setCurrentIndex(-1);
        ui->firstDoseToValue->clear();
        ui->firstDoseToUnits->setCurrentIndex(-1);
        ui->firstDoseToRepart->setCurrentIndex(-1);
        ui->secondDoseUsesFrom->setChecked(false);
        ui->secondDoseUsesTo->setChecked(false);
        ui->secondDoseFromValue->clear();
        ui->secondDoseFromUnits->setCurrentIndex(-1);
        ui->secondDoseFromRepart->setCurrentIndex(-1);
        ui->secondDoseToValue->clear();
        ui->secondDoseToUnits->setCurrentIndex(-1);
        ui->secondDoseToRepart->setCurrentIndex(-1);
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
    QAction *aSplitInteractionAccordingToLevel;
    QAction *aNextProblematic, *aNextMultiLevel, *aNextUntranslated, *aNextUnreviewed;
    QToolButton *_nextButton;
    Utils::GoogleTranslator *googleTranslator;
    DDI::RoutesModel *firstInteractorRoutes, *secondInteractorRoutes;
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

/** Create a new drug-drug interaction */
void DrugDrugInteractionEditorWidget::createNewDDI()
{
    // Ask for interactors
    QPair<QString, QString> first, second;
    InteractorSelectorDialog dlg(this);
    dlg.setTitle(tr("Select the interactors (two only)"));
    dlg.setAllowMultipleSelection(2);
    dlg.initialize();
    if (dlg.exec() == QDialog::Accepted) {
        if (dlg.selectedInteractors().count() != 2)
            return;
        first = dlg.selectedInteractors().at(0);
        second = dlg.selectedInteractors().at(1);
    }
    if (first.first.isEmpty() || second.first.isEmpty())
        return;

    // TODO: Ask for the level

    // Ask for a confirmation
    bool yes = Utils::yesNoMessageBox(tr("Do you really want to create the following drug-drug interaction:<br />"
                                         "- %1<br />"
                                         "- %2 <br />").arg(first.first).arg(second.first),
                                      "", "", tr("Drug-drug interaction creation"));
    if (!yes)
        return;

    DrugDrugInteractionTableModel *model = ddiCore()->drugDrugInteractionTableModel();
    int row = 0;
    if (!model->insertRow(row)) {
        LOG_ERROR("Unable to create a new DDI");
        return;
    }

    QModelIndex f = model->index(row, DrugDrugInteractionTableModel::FirstInteractorUid);
    QModelIndex s = model->index(row, DrugDrugInteractionTableModel::SecondInteractorUid);
    model->setData(f, first.first);
    model->setData(s, second.first);
    if (!model->submitAll()) {
        LOG_ERROR("Unable to create a new DDI");
        return;
    }

    // When model is submitted we need to re-fetch all rows
    model->initialize();
    // the last inserted row is the last row of unsorted the model
    f = model->index(model->rowCount() - 1, DrugDrugInteractionTableModel::FirstInteractorUid);

    // Select newly created interaction in the view
    d->ui->searchLine->setText("");
    filterDrugDrugInteractionTableModel("");
    d->ui->tableView->selectRow(d->m_DDIProxyModel->mapFromSource(f).row());
    interactionActivated(d->m_DDIProxyModel->mapFromSource(f));
    edit();
}

/** Filter the DDI model with the search terms \e filter */
void DrugDrugInteractionEditorWidget::filterDrugDrugInteractionTableModel(const QString &filter)
{
    d->m_DDIProxyModel->setFilterRole(Qt::DisplayRole);
    d->m_DDIProxyModel->setFilterKeyColumn(DrugDrugInteractionTableModel::FirstInteractorUid);
    d->m_DDIProxyModel->setFilterFixedString(filter);
    d->m_DDIProxyModel->sort(DrugDrugInteractionTableModel::FirstInteractorUid);
    for(int i = 0; i < d->m_DDIProxyModel->rowCount(); ++i) {
        d->ui->tableView->hideColumn(i);
    }
    d->ui->tableView->showColumn(DrugDrugInteractionTableModel::FirstInteractorUid);
    d->ui->tableView->showColumn(DrugDrugInteractionTableModel::SecondInteractorUid);
    d->ui->tableView->showColumn(DrugDrugInteractionTableModel::LevelCode);
    d->ui->tableView->horizontalHeader()->setResizeMode(DrugDrugInteractionTableModel::FirstInteractorUid, QHeaderView::Stretch);
    d->ui->tableView->horizontalHeader()->setResizeMode(DrugDrugInteractionTableModel::SecondInteractorUid, QHeaderView::Stretch);
    d->ui->tableView->horizontalHeader()->setResizeMode(DrugDrugInteractionTableModel::LevelCode, QHeaderView::ResizeToContents);
    d->ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    d->ui->tableView->horizontalHeader()->hide();
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
    d->clearUi();
    d->aEdit->setEnabled(true);
    d->aSave->setEnabled(true);
    d->aTranslateThis->setEnabled(true);
    d->aRemoveCurrent->setEnabled(true);
    d->aSplitInteractionAccordingToLevel->setEnabled(true);

    // submit / revert mapper ?
    if (d->ui->risk->isEnabled()) {
        if (Utils::yesNoMessageBox(tr("Data changed but not saved."),
                                   tr("Do you want to save changes to the database?"))) {
            save();
        } else {
            d->m_Mapper->revert();
        }
    }

    setEditorsEnabled(false);

    d->m_Mapper->setCurrentModelIndex(index);

    // manage routes of interactors
    d->firstInteractorRoutes->clear();
    d->secondInteractorRoutes->clear();
    QModelIndex i1routes = d->m_DDIProxyModel->index(index.row(), DrugDrugInteractionTableModel::FirstInteractorRouteOfAdministrationIds);
    if (!i1routes.data().isNull()) {
        d->firstInteractorRoutes->setCheckedRouteIds(i1routes.data().toList());
    }
    QModelIndex i2routes = d->m_DDIProxyModel->index(index.row(), DrugDrugInteractionTableModel::SecondInteractorRouteOfAdministrationIds);
    if (!i2routes.data().isNull()) {
        d->secondInteractorRoutes->setCheckedRouteIds(i2routes.data().toList());
    }

    // and bibliography
    QModelIndex pmids = d->m_DDIProxyModel->index(index.row(), DrugDrugInteractionTableModel::PMIDStringList);
    d->biblioModel->setStringList(pmids.data().toStringList());

    d->m_EditingIndex = index;
}

/** Save the DDI model and all its changes */
void DrugDrugInteractionEditorWidget::save()
{
    if (d->m_EditingIndex.isValid()) {
        d->ui->tableView->setFocus();
        d->m_Mapper->submit();

        // manage interactor routes
        QModelIndex i1routes = d->m_DDIProxyModel->index(d->m_EditingIndex.row(), DrugDrugInteractionTableModel::FirstInteractorRouteOfAdministrationIds);
        d->m_DDIProxyModel->setData(i1routes, d->firstInteractorRoutes->checkedRouteIdsInVariant());
        QModelIndex i2routes = d->m_DDIProxyModel->index(d->m_EditingIndex.row(), DrugDrugInteractionTableModel::SecondInteractorRouteOfAdministrationIds);
        d->m_DDIProxyModel->setData(i2routes, d->secondInteractorRoutes->checkedRouteIdsInVariant());

        // manage bibliography
        QModelIndex pmids = d->m_DDIProxyModel->index(d->m_EditingIndex.row(), DrugDrugInteractionTableModel::PMIDStringList);
        d->m_DDIProxyModel->setData(pmids, d->biblioModel->stringList());
    }
    setEditorsEnabled(false);
    ddiCore()->drugDrugInteractionTableModel()->submitAll();
}

/** Remove the current selected DDI */
void DrugDrugInteractionEditorWidget::removeCurrent()
{
//    if (d->m_EditingIndex.isValid()) {
//        d->m_DDIProxyModel->removeRow(d->m_EditingIndex.row(), d->m_EditingIndex.parent());
//    }
}

void DrugDrugInteractionEditorWidget::onNextActionTriggered(QAction *a)
{
    // Find first problematic DDI
    QModelIndex multi;
    QModelIndex untr;
    QModelIndex unrev;
    QModelIndex next;
    int rowCount = ddiCore()->drugDrugInteractionTableModel()->rowCount();
    int currentRow = qMax(d->m_DDIProxyModel->mapToSource(d->ui->tableView->currentIndex()).row() + 1, 0);
    // From current row to end of the model
    for(int i = currentRow; i < rowCount; ++i) {
        if (!multi.isValid() && ddiCore()->drugDrugInteractionTableModel()->isMultiLevel(i)) {
            multi = ddiCore()->drugDrugInteractionTableModel()->index(i, DrugDrugInteractionTableModel::FirstInteractorUid);
        }
        if (!unrev.isValid() && !ddiCore()->drugDrugInteractionTableModel()->index(i, DrugDrugInteractionTableModel::IsReviewed).data().toBool()) {
            unrev = ddiCore()->drugDrugInteractionTableModel()->index(i, DrugDrugInteractionTableModel::FirstInteractorUid);
        }
        if (!untr.isValid() && ddiCore()->drugDrugInteractionTableModel()->isUntranslated(i)) {
            untr = ddiCore()->drugDrugInteractionTableModel()->index(i, DrugDrugInteractionTableModel::FirstInteractorUid);
        }
    }
    // From beginning of the model to current row
    --currentRow;
    for(int i = 0; i < currentRow; ++i) {
        if (!multi.isValid() && ddiCore()->drugDrugInteractionTableModel()->isMultiLevel(i)) {
            multi = ddiCore()->drugDrugInteractionTableModel()->index(i, DrugDrugInteractionTableModel::FirstInteractorUid);
        }
        if (!unrev.isValid() && !ddiCore()->drugDrugInteractionTableModel()->index(i, DrugDrugInteractionTableModel::IsReviewed).data().toBool()) {
            unrev = ddiCore()->drugDrugInteractionTableModel()->index(i, DrugDrugInteractionTableModel::FirstInteractorUid);
        }
        if (!untr.isValid() && ddiCore()->drugDrugInteractionTableModel()->isUntranslated(i)) {
            untr = ddiCore()->drugDrugInteractionTableModel()->index(i, DrugDrugInteractionTableModel::FirstInteractorUid);
        }
    }

    d->ui->searchLine->setText("");
    filterDrugDrugInteractionTableModel("");
    if (a==d->aNextMultiLevel) {
        next = multi;
    } else if (a==d->aNextUntranslated) {
        next = untr;
    } else if (a==d->aNextUnreviewed) {
        next = unrev;
    } else {
        next = ddiCore()->drugDrugInteractionTableModel()->index(qMin(multi.row(), untr.row()), DrugDrugInteractionTableModel::FirstInteractorUid);
    }
    if (next.isValid()) {
        d->ui->tableView->selectRow(d->m_DDIProxyModel->mapFromSource(next).row());
        interactionActivated(d->m_DDIProxyModel->mapFromSource(next));
        edit();
    }
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
    // Is DDI is multi-level?
    QModelIndex source = d->m_DDIProxyModel->mapToSource(d->ui->tableView->currentIndex());
    if (ddiCore()->drugDrugInteractionTableModel()->isMultiLevel(source.row())) {
        ddiCore()->drugDrugInteractionTableModel()->splitMultiLevel(d->m_EditingIndex.row());
    }

    // TODO: keep the currentIndex to the splitted DDI and its mirrored DDIs
}

void DrugDrugInteractionEditorWidget::retranslateUi()
{
    d->aCreateNew->setText(tkTr(Trans::Constants::ADD_TEXT));
    d->aSave->setText(tkTr(Trans::Constants::FILESAVE_TEXT));
    d->aEdit->setText(tkTr(Trans::Constants::M_EDIT_TEXT));
    d->aRemoveCurrent->setText(tkTr(Trans::Constants::REMOVE_TEXT));
    d->aTranslateAll->setText(tr("Translate all untranslated"));
    d->aTranslateThis->setText(tr("Translate current"));
    d->aSplitInteractionAccordingToLevel->setText(tr("Split interaction of multi-level to one interaction by level"));

    d->aNextProblematic->setText(tr("Next problematic interaction"));
    d->aNextMultiLevel->setText(tr("Next multi-level interaction"));
    d->aNextUntranslated->setText(tr("Next untranslated interaction"));
    d->aNextUnreviewed->setText(tr("Next unreviewed interaction"));

    d->aCreateNew->setToolTip(d->aCreateNew->text());
    d->aSave->setToolTip(d->aSave->text());
    d->aEdit->setToolTip(d->aEdit->text());
    d->aRemoveCurrent->setToolTip(d->aRemoveCurrent->text());
    d->aTranslateAll->setToolTip(d->aTranslateAll->text());
    d->aTranslateThis->setToolTip(d->aTranslateThis->text());
    d->aSplitInteractionAccordingToLevel->setToolTip(d->aSplitInteractionAccordingToLevel->text());

    d->aNextProblematic->setToolTip(d->aNextProblematic->text());
    d->aNextMultiLevel->setToolTip(d->aNextMultiLevel->text());
    d->aNextUntranslated->setToolTip(d->aNextUntranslated->text());
    d->aNextUnreviewed->setToolTip(d->aNextUnreviewed->text());

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
