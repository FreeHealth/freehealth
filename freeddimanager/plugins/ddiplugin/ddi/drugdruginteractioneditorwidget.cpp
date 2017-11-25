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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "drugdruginteractioneditorwidget.h"
#include "drugdruginteractiontablemodel.h"
#include "drugdruginteractionsortfilterproxymodel.h"
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
        _ddiProxyModel(0),
        _mapper(0),
        m_ReviewModified(false),
        aCheckModelErrors(0),
        aSave(0),
        aEdit(0),
        aRevert(0),
        aRemoveCurrent(0),
        aCreateNew(0),
        aSplitInteractionAccordingToLevel(0),
        aNextProblematic(0),
        aNextMultiLevel(0),
        aNextUntranslated(0),
        aNextUnreviewed(0),
        aNextNoRisk(0),
        aNextUnknownInteractors(0),
        _nextButton(0),
        _firstInteractorRoutes(0), _secondInteractorRoutes(0),
        _biblioModel(0),
        _testsRunning(false),
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
        b->setIconSize(QSize(32, 32));
        b->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        aCheckModelErrors = new QAction(q);
        aSave = new QAction(q);
        aRemoveCurrent = new QAction(q);
        aEdit = new QAction(q);
        aRevert = new QAction(q);
        aCreateNew = new QAction(q);
        aSplitInteractionAccordingToLevel = new QAction(q);
        aNextProblematic = new QAction(q);
        aNextMultiLevel = new QAction(q);
        aNextUntranslated = new QAction(q);
        aNextUnreviewed = new QAction(q);
        aNextNoRisk = new QAction(q);
        aNextUnknownInteractors = new QAction(q);

        aSave->setEnabled(false);
        aEdit->setEnabled(false);
        aRemoveCurrent->setEnabled(false);
        aSplitInteractionAccordingToLevel->setEnabled(false);

        aCheckModelErrors->setIcon(theme()->icon(Core::Constants::ICONPROCESS));
        aSave->setIcon(theme()->icon(Core::Constants::ICONSAVE));
        aRemoveCurrent->setIcon(theme()->icon(Core::Constants::ICONREMOVE));
        aEdit->setIcon(theme()->icon(Core::Constants::ICONEDIT));
        aRevert->setIcon(theme()->icon(Core::Constants::ICONREVERT));
        aCreateNew->setIcon(theme()->icon(Core::Constants::ICONADD));
        aSplitInteractionAccordingToLevel->setIcon(theme()->icon(Core::Constants::ICONSPLITFILE));
        aNextProblematic->setIcon(theme()->icon(Core::Constants::ICONNEXT));
        aNextMultiLevel->setIcon(theme()->icon(Core::Constants::ICONNEXT));
        aNextUntranslated->setIcon(theme()->icon(Core::Constants::ICONNEXT));
        aNextUnreviewed->setIcon(theme()->icon(Core::Constants::ICONNEXT));
        aNextNoRisk->setIcon(theme()->icon(Core::Constants::ICONNEXT));
        aNextUnknownInteractors->setIcon(theme()->icon(Core::Constants::ICONNEXT));

        _nextButton = new QToolButton(q);
        _nextButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        _nextButton->addAction(aNextProblematic);
        _nextButton->addAction(aNextNoRisk);
        _nextButton->addAction(aNextMultiLevel);
        _nextButton->addAction(aNextUnreviewed);
        _nextButton->addAction(aNextUntranslated);
        _nextButton->addAction(aNextUnknownInteractors);
        _nextButton->setPopupMode(QToolButton::MenuButtonPopup);
        _nextButton->setDefaultAction(aNextProblematic);

        b->addAction(aCheckModelErrors);
        b->addWidget(_nextButton);
        b->addSeparator();
        b->addAction(aCreateNew);
        b->addSeparator();
        b->addAction(aRemoveCurrent);
        b->addSeparator();
        b->addAction(aEdit);
        b->addAction(aRevert);
        b->addAction(aSplitInteractionAccordingToLevel);
        b->addSeparator();
        b->addAction(aSave);
    }

    void connectActions()
    {
        QObject::connect(aCheckModelErrors, SIGNAL(triggered()), q, SLOT(onCheckInteractionErrorsRequested()));
        QObject::connect(aCreateNew, SIGNAL(triggered()), q, SLOT(createNewDDI()));
        QObject::connect(aSave, SIGNAL(triggered()), q, SLOT(save()));
        QObject::connect(aRemoveCurrent, SIGNAL(triggered()), q, SLOT(removeCurrent()));
        QObject::connect(aEdit, SIGNAL(triggered()), q, SLOT(edit()));
        QObject::connect(aRevert, SIGNAL(triggered()), q, SLOT(revertCurrentEdition()));
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

        _firstInteractorRoutes = new DDI::RoutesModel(q);
        ui->listViewFirstInteractorRoute->setModel(_firstInteractorRoutes);
        ui->listViewFirstInteractorRoute->setModelColumn(DDI::RoutesModel::FirstTranslatedName);
        _secondInteractorRoutes = new DDI::RoutesModel(q);
        ui->listViewSecondInteractorRoute->setModel(_secondInteractorRoutes);
        ui->listViewSecondInteractorRoute->setModelColumn(DDI::RoutesModel::FirstTranslatedName);

        _biblioModel = new QStringListModel(q);
        ui->bilbioTableView->setModel(_biblioModel);
        ui->bilbioTableView->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
        ui->bilbioTableView->setAlternatingRowColors(true);
        ui->bilbioTableView->horizontalHeader()->hide();
        ui->bilbioTableView->verticalHeader()->hide();

        _ddiProxyModel = new DrugDrugInteractionSortFilterProxyModel(q);
        _ddiProxyModel->setSourceModel(ddiCore()->drugDrugInteractionTableModel());
        _ddiProxyModel->setDynamicSortFilter(true);
        ui->tableView->setModel(_ddiProxyModel);
        ui->tableView->setWordWrap(true);
        for(int i = 0; i < _ddiProxyModel->columnCount(); ++i) {
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
        _mapper = new QDataWidgetMapper(q);
        _mapper->setModel(ddiCore()->drugDrugInteractionTableModel());
        _mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

        _mapper->addMapping(ui->firstInteractor, DrugDrugInteractionTableModel::FirstInteractorLabel, "text");
        _mapper->addMapping(ui->secondInteractor, DrugDrugInteractionTableModel::SecondInteractorLabel, "text");
        _mapper->addMapping(ui->firstInteractor2, DrugDrugInteractionTableModel::FirstInteractorLabel, "text");
        _mapper->addMapping(ui->secondInteractor2, DrugDrugInteractionTableModel::SecondInteractorLabel, "text");
        _mapper->addMapping(ui->isReviewed, DrugDrugInteractionTableModel::IsReviewed, "checked");

        _mapper->addMapping(ui->risk, DrugDrugInteractionTableModel::RiskFr, "plainText");
        _mapper->addMapping(ui->risk_en, DrugDrugInteractionTableModel::RiskEn, "plainText");
        _mapper->addMapping(ui->management, DrugDrugInteractionTableModel::ManagementFr, "plainText");
        _mapper->addMapping(ui->management_en, DrugDrugInteractionTableModel::ManagementEn, "plainText");
        _mapper->addMapping(ui->createdOn, DrugDrugInteractionTableModel::DateCreation, "date");
        _mapper->addMapping(ui->updatedOn, DrugDrugInteractionTableModel::DateLastUpdate, "date");
        _mapper->addMapping(ui->comboLevel, DrugDrugInteractionTableModel::LevelComboIndex, "currentIndex");
        _mapper->addMapping(ui->comment, DrugDrugInteractionTableModel::Comment, "plainText");

        _mapper->addMapping(ui->firstDoseUsesFrom, DrugDrugInteractionTableModel::FirstDoseUseFrom, "checked");
        _mapper->addMapping(ui->firstDoseUsesTo, DrugDrugInteractionTableModel::FirstDoseUsesTo, "checked");
        _mapper->addMapping(ui->firstDoseFromValue, DrugDrugInteractionTableModel::FirstDoseFromValue, "text");
        _mapper->addMapping(ui->firstDoseFromUnits, DrugDrugInteractionTableModel::FirstDoseFromUnitsId, "currentIndex");
        _mapper->addMapping(ui->firstDoseFromRepart, DrugDrugInteractionTableModel::FirstDoseFromRepartitionId, "currentIndex");
        _mapper->addMapping(ui->firstDoseToValue, DrugDrugInteractionTableModel::FirstDoseToValue, "text");
        _mapper->addMapping(ui->firstDoseToUnits, DrugDrugInteractionTableModel::FirstDoseToUnitsId, "currentIndex");
        _mapper->addMapping(ui->firstDoseToRepart, DrugDrugInteractionTableModel::FirstDoseToRepartitionId, "currentIndex");

        _mapper->addMapping(ui->secondDoseUsesFrom, DrugDrugInteractionTableModel::SecondDoseUseFrom, "checked");
        _mapper->addMapping(ui->secondDoseUsesTo, DrugDrugInteractionTableModel::SecondDoseUsesTo, "checked");
        _mapper->addMapping(ui->secondDoseFromValue, DrugDrugInteractionTableModel::SecondDoseFromValue, "text");
        _mapper->addMapping(ui->secondDoseFromUnits, DrugDrugInteractionTableModel::SecondDoseFromUnitsId, "currentIndex");
        _mapper->addMapping(ui->secondDoseFromRepart, DrugDrugInteractionTableModel::SecondDoseFromRepartitionId, "currentIndex");
        _mapper->addMapping(ui->secondDoseToValue, DrugDrugInteractionTableModel::SecondDoseToValue, "text");
        _mapper->addMapping(ui->secondDoseToUnits, DrugDrugInteractionTableModel::SecondDoseToUnitsId, "currentIndex");
        _mapper->addMapping(ui->secondDoseToRepart, DrugDrugInteractionTableModel::SecondDoseToRepartitionId, "currentIndex");

//        _mapper->addMapping(ui->humanSynthesis, DrugDrugInteractionTableModel::HumanReadableSynthesis, "html");

        //    _mapper->addMapping(ui->comboFirstInteractorRoute, DrugDrugInteractionTableModel::FirstInteractorRouteIndex, "currentIndex");
        //    _mapper->addMapping(ui->comboSecondInteractorRoute, DrugDrugInteractionTableModel::SecondInteractorRouteIndex, "currentIndex");
    }

    void clearUi()
    {
        ui->firstInteractor->clear();
        ui->secondInteractor->clear();
        ui->firstInteractor2->clear();
        ui->secondInteractor2->clear();
        ui->overview->clear();
        ui->comboLevel->setCurrentIndex(-1);
        ui->createdOn->clear();
        ui->updatedOn->clear();
        ui->isReviewed->setChecked(false);
        ui->risk->clear();
        ui->risk_en->clear();
        ui->management->clear();
        ui->management_en->clear();
        _biblioModel->setStringList(QStringList());
        _firstInteractorRoutes->setCheckedRouteIds(QList<QVariant>());
        _secondInteractorRoutes->setCheckedRouteIds(QList<QVariant>());
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
    DrugDrugInteractionSortFilterProxyModel *_ddiProxyModel;
    QDataWidgetMapper *_mapper;
    QPersistentModelIndex m_EditingIndex;
    bool m_ReviewModified;
    QAction *aCheckModelErrors;
    QAction *aSave;
    QAction *aEdit;
    QAction *aRevert;
    QAction *aRemoveCurrent;
    QAction *aCreateNew;
    QAction *aSplitInteractionAccordingToLevel;
    QAction *aNextProblematic;
    QAction *aNextMultiLevel;
    QAction *aNextUntranslated;
    QAction *aNextUnreviewed;
    QAction *aNextNoRisk;
    QAction *aNextUnknownInteractors;
    QToolButton *_nextButton;
    DDI::RoutesModel *_firstInteractorRoutes, *_secondInteractorRoutes;
    QStringListModel *_biblioModel;

    bool _testsRunning;

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
    updateDatabaseStats();

    connect(d->ui->tableView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(interactionActivated(QModelIndex)));
    connect(d->ui->searchLine, SIGNAL(textChanged(QString)), SLOT(filterDrugDrugInteractionTableModel(QString)));
    connect(ddiCore()->drugDrugInteractionTableModel(), SIGNAL(modelReset()), this, SLOT(updateDatabaseStats()));
    connect(ddiCore()->drugDrugInteractionTableModel(), SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(updateDatabaseStats()));
    connect(ddiCore()->drugDrugInteractionTableModel(), SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(updateDatabaseStats()));
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
    d->ui->comboLevel->setEnabled(state);
    d->ui->createdOn->setEnabled(state);
    d->ui->updatedOn->setEnabled(state);
    d->ui->isReviewed->setEnabled(state);

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

    // Actions
    d->aEdit->setEnabled(!state);
    d->aRevert->setEnabled(state);
    d->aSave->setEnabled(state);
    // d->aRemoveCurrent->setEnabled(state);
    // d->aSplitInteractionAccordingToLevel->setEnabled(state);
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

#ifdef WITH_TESTS
    if (d->_testsRunning)               // run dialog unit-tests
        dlg.test_runTestAndAccept();
#endif

    if (dlg.exec() == QDialog::Accepted) {
        if (dlg.selectedInteractors().count() != 2)
            return;
        first = dlg.selectedInteractors().at(0);
        second = dlg.selectedInteractors().at(1);
    }
    if (first.second.isEmpty() || second.second.isEmpty())
        return;

    // TODO: Check if some DDI already exists for this pair of interactors

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
    model->setData(f, first.second); // Use UID
    model->setData(s, second.second); // Use UID
    if (!model->submitAll()) {
        LOG_ERROR("Unable to create a new DDI");
        return;
    }
    // the last inserted row is the last row of unsorted the model
    f = model->index(model->rowCount() - 1, DrugDrugInteractionTableModel::FirstInteractorUid);

    // Select newly created interaction in the view
    d->ui->searchLine->setText("");
    filterDrugDrugInteractionTableModel("");
    QModelIndex proxyIndex = d->_ddiProxyModel->mapFromSource(f);
    d->ui->tableView->selectRow(proxyIndex.row());
    d->ui->tableView->scrollTo(proxyIndex, QTableView::EnsureVisible);
    interactionActivated(proxyIndex);
    edit();
}

/** Filter the DDI model with the search terms \e filter */
void DrugDrugInteractionEditorWidget::filterDrugDrugInteractionTableModel(const QString &filter)
{
    d->_ddiProxyModel->invalidate();
    d->_ddiProxyModel->setFilterFixedString(filter);
    updateDatabaseStats();
}

/** Edit the current selected DDI */
void DrugDrugInteractionEditorWidget::edit()
{
    setEditorsEnabled(true);
}

void DrugDrugInteractionEditorWidget::revertCurrentEdition()
{
    d->_mapper->revert();
    setEditorsEnabled(false);
}

/**
 * Activated a specific DDI
 */
void DrugDrugInteractionEditorWidget::interactionActivated(const QModelIndex &index)
{
    QModelIndex sourceIndex = d->_ddiProxyModel->mapToSource(index);

    if (d->m_EditingIndex == sourceIndex)
        return;

    // submit / revert mapper ?
    if (d->ui->risk->isEnabled()) {
#ifdef WITH_TESTS
        if (!d->_testsRunning)    // No user interaction during tests
            save();
#else
        // Ask user what he wants to do
        if (Utils::yesNoMessageBox(tr("Data changed but not saved."),
                                   tr("Do you want to save changes to the database?"))) {
            save();
        } else {
            d->_mapper->revert();
        }
#endif
    }

    d->clearUi();
    setEditorsEnabled(false);
    d->_mapper->setCurrentModelIndex(sourceIndex);

    // routes of interactors
    d->_firstInteractorRoutes->clear();
    d->_secondInteractorRoutes->clear();
    QModelIndex i1routes = d->_ddiProxyModel->index(index.row(), DrugDrugInteractionTableModel::FirstInteractorRouteOfAdministrationIds);
    if (!i1routes.data().isNull()) {
        d->_firstInteractorRoutes->setCheckedRouteIds(i1routes.data().toList());
    }
    QModelIndex i2routes = d->_ddiProxyModel->index(index.row(), DrugDrugInteractionTableModel::SecondInteractorRouteOfAdministrationIds);
    if (!i2routes.data().isNull()) {
        d->_secondInteractorRoutes->setCheckedRouteIds(i2routes.data().toList());
    }

    // bibliography
    QModelIndex pmids = d->_ddiProxyModel->index(index.row(), DrugDrugInteractionTableModel::PMIDStringList);
    d->_biblioModel->setStringList(pmids.data().toStringList());

    // overview
    d->ui->overview->setHtml(ddiCore()->drugDrugInteractionTableModel()->humanReadableDrugDrugInteractionOverView(sourceIndex.row()));

    d->m_EditingIndex = sourceIndex;
}

/** Save the DDI model and all its changes */
void DrugDrugInteractionEditorWidget::save()
{
    if (d->m_EditingIndex.isValid() &&
            d->ui->risk->isEnabled()) {
        d->ui->tableView->setFocus();

        // manage interactor routes
        QModelIndex i1routes = ddiCore()->drugDrugInteractionTableModel()->index(d->m_EditingIndex.row(), DrugDrugInteractionTableModel::FirstInteractorRouteOfAdministrationIds);
        ddiCore()->drugDrugInteractionTableModel()->setData(i1routes, d->_firstInteractorRoutes->checkedRouteIdsInVariant());
        QModelIndex i2routes = ddiCore()->drugDrugInteractionTableModel()->index(d->m_EditingIndex.row(), DrugDrugInteractionTableModel::SecondInteractorRouteOfAdministrationIds);
        ddiCore()->drugDrugInteractionTableModel()->setData(i2routes, d->_secondInteractorRoutes->checkedRouteIdsInVariant());

        // manage bibliography
        QModelIndex pmids = ddiCore()->drugDrugInteractionTableModel()->index(d->m_EditingIndex.row(), DrugDrugInteractionTableModel::PMIDStringList);
        ddiCore()->drugDrugInteractionTableModel()->setData(pmids, d->_biblioModel->stringList());

        // Submit mapper at the end as the model will be resetted on submit
        if (!d->_mapper->submit())
            LOG_ERROR("Unable to submit mapper");
    }
    setEditorsEnabled(false);
    ddiCore()->drugDrugInteractionTableModel()->submitAll();
    updateDatabaseStats();
}

/** Remove the current selected DDI */
void DrugDrugInteractionEditorWidget::removeCurrent()
{
    QModelIndex index = d->_ddiProxyModel->mapToSource(d->ui->tableView->currentIndex());
    if (index.isValid()) {
        d->_ddiProxyModel->removeRow(index.row());
    }
}

void DrugDrugInteractionEditorWidget::onNextActionTriggered(QAction *a)
{
    // Find first problematic DDI
    QModelIndex multi;
    QModelIndex untr;
    QModelIndex unrev;
    QModelIndex norisk;
    QModelIndex wronguid;
    QModelIndex next;
    int rowCount = ddiCore()->drugDrugInteractionTableModel()->rowCount();
    int currentRow = qMax(d->_ddiProxyModel->mapToSource(d->ui->tableView->currentIndex()).row() + 1, 0);
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
        if (!norisk.isValid()
                && ddiCore()->drugDrugInteractionTableModel()->index(i, DrugDrugInteractionTableModel::RiskEn).data().toString().isEmpty()
                && ddiCore()->drugDrugInteractionTableModel()->index(i, DrugDrugInteractionTableModel::RiskFr).data().toString().isEmpty()) {
            norisk = ddiCore()->drugDrugInteractionTableModel()->index(i, DrugDrugInteractionTableModel::FirstInteractorUid);
        }
        if (!wronguid.isValid()
                && (ddiCore()->drugDrugInteractionTableModel()->hasError(i, DrugDrugInteractionTableModel::FirstInteractorDoesNotExists)
                    || ddiCore()->drugDrugInteractionTableModel()->hasError(i, DrugDrugInteractionTableModel::SecondInteractorDoesNotExists)
                    )) {
            wronguid = ddiCore()->drugDrugInteractionTableModel()->index(i, DrugDrugInteractionTableModel::FirstInteractorUid);
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
        if (!norisk.isValid()
                && ddiCore()->drugDrugInteractionTableModel()->index(i, DrugDrugInteractionTableModel::RiskEn).data().toString().isEmpty()
                && ddiCore()->drugDrugInteractionTableModel()->index(i, DrugDrugInteractionTableModel::RiskFr).data().toString().isEmpty()) {
            norisk = ddiCore()->drugDrugInteractionTableModel()->index(i, DrugDrugInteractionTableModel::FirstInteractorUid);
        }
        if (!wronguid.isValid()
                && (ddiCore()->drugDrugInteractionTableModel()->hasError(i, DrugDrugInteractionTableModel::FirstInteractorDoesNotExists)
                    || ddiCore()->drugDrugInteractionTableModel()->hasError(i, DrugDrugInteractionTableModel::SecondInteractorDoesNotExists)
                    )) {
            wronguid = ddiCore()->drugDrugInteractionTableModel()->index(i, DrugDrugInteractionTableModel::FirstInteractorUid);
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
    } else if (a==d->aNextNoRisk) {
        next = norisk;
    } else if (a==d->aNextUnknownInteractors) {
        next = wronguid;
    } else {
        int row = qMin(multi.row(), untr.row());
        row = qMin(row, unrev.row());
        row = qMin(row, norisk.row());
        next = ddiCore()->drugDrugInteractionTableModel()->index(row, DrugDrugInteractionTableModel::FirstInteractorUid);
    }
    if (next.isValid()) {
        d->ui->tableView->selectRow(d->_ddiProxyModel->mapFromSource(next).row());
        interactionActivated(d->_ddiProxyModel->mapFromSource(next));
        // edit();
    }
}

/**
 * When a DDI has multiple levels (contraindication, precaution...)
 * splits this DDI into each level.
 */
void DrugDrugInteractionEditorWidget::splitCurrent()
{
    // Is DDI is multi-level?
    QModelIndex source = d->_ddiProxyModel->mapToSource(d->ui->tableView->currentIndex());
    if (ddiCore()->drugDrugInteractionTableModel()->isMultiLevel(source.row())) {
        ddiCore()->drugDrugInteractionTableModel()->splitMultiLevel(d->m_EditingIndex.row());
    }

    // TODO: keep the currentIndex to the splitted DDI and its mirrored DDIs
}

void DrugDrugInteractionEditorWidget::onCheckInteractionErrorsRequested()
{
    ddiCore()->drugDrugInteractionTableModel()->checkInteractionErrors();
    d->ui->tableView->update();
}

/** Update the database stats in the view */
void DrugDrugInteractionEditorWidget::updateDatabaseStats()
{
    d->ui->dbStats->setText(tr("Showing %1 on %2")
                            .arg(d->_ddiProxyModel->rowCount())
                            .arg(ddiCore()->drugDrugInteractionTableModel()->rowCount()));
}

void DrugDrugInteractionEditorWidget::retranslateUi()
{
    d->aCreateNew->setText(tkTr(Trans::Constants::ADD_TEXT));
    d->aSave->setText(tkTr(Trans::Constants::FILESAVE_TEXT));
    d->aEdit->setText(tkTr(Trans::Constants::M_EDIT_TEXT));
    d->aRevert->setText(tkTr(Trans::Constants::REVERT));
    d->aRemoveCurrent->setText(tkTr(Trans::Constants::REMOVE_TEXT));
    d->aSplitInteractionAccordingToLevel->setText(tr("Split multi-level"));

    d->aNextProblematic->setText(tr("Next problematic"));
    d->aNextMultiLevel->setText(tr("Next multi-level"));
    d->aNextUntranslated->setText(tr("Next untranslated"));
    d->aNextUnreviewed->setText(tr("Next unreviewed"));
    d->aNextNoRisk->setText(tr("Next 'no risk'"));
    d->aNextUnknownInteractors->setText(tr("Next unknown"));
    d->aCheckModelErrors->setText(tr("Check errors"));

    d->aCreateNew->setToolTip(d->aCreateNew->text());
    d->aSave->setToolTip(d->aSave->text());
    d->aEdit->setToolTip(d->aEdit->text());
    d->aRevert->setToolTip(d->aRevert->text());
    d->aRemoveCurrent->setToolTip(d->aRemoveCurrent->text());
    d->aSplitInteractionAccordingToLevel->setToolTip(d->aSplitInteractionAccordingToLevel->text());

    d->aNextProblematic->setToolTip(d->aNextProblematic->text());
    d->aNextMultiLevel->setToolTip(d->aNextMultiLevel->text());
    d->aNextUntranslated->setToolTip(d->aNextUntranslated->text());
    d->aNextUnreviewed->setToolTip(d->aNextUnreviewed->text());
    d->aNextNoRisk->setToolTip(d->aNextNoRisk->text());
    d->aNextUnknownInteractors->setToolTip(d->aNextUnknownInteractors->text());
    d->aCheckModelErrors->setToolTip(d->aCheckModelErrors->text());

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

#ifdef WITH_TESTS
#include <coreplugin/imainwindow.h>
#include <utils/randomizer.h>
#include <QTest>

static inline Core::IMainWindow *mainWindow()  { return Core::ICore::instance()->mainWindow(); }

void DrugDrugInteractionEditorWidget::test_runAllTests()
{
    d->_testsRunning = true;
    test_views();
    test_actions();
    test_itemCreation();
    test_edition();
}

void DrugDrugInteractionEditorWidget::test_views()
{
    // Proxy model over the central DDITableModel
    QCOMPARE(d->_ddiProxyModel->sourceModel(), ddiCore()->drugDrugInteractionTableModel());
    // Views connected to the proxy model
    QCOMPARE(d->ui->tableView->model(), d->_ddiProxyModel);
    // In this editor, mapper runs over the core model not the proxy model
     QCOMPARE(d->_mapper->model(), ddiCore()->drugDrugInteractionTableModel());
}

void DrugDrugInteractionEditorWidget::test_editorEnabledState(bool state)
{
    QCOMPARE(d->ui->comboLevel->isEnabled(), state);
    QCOMPARE(d->ui->createdOn->isEnabled(), state);
    QCOMPARE(d->ui->updatedOn->isEnabled(), state);
    QCOMPARE(d->ui->isReviewed->isEnabled(), state);
    QCOMPARE(d->ui->risk->isEnabled(), state);
    QCOMPARE(d->ui->risk_en->isEnabled(), state);
    QCOMPARE(d->ui->management->isEnabled(), state);
    QCOMPARE(d->ui->management_en->isEnabled(), state);
    QCOMPARE(d->ui->formRiskTableView->isEnabled(), state);
    QCOMPARE(d->ui->bilbioTableView->isEnabled(), state);
    QCOMPARE(d->ui->listViewFirstInteractorRoute->isEnabled(), state);
    QCOMPARE(d->ui->listViewSecondInteractorRoute->isEnabled(), state);
    QCOMPARE(d->ui->firstDoseUsesFrom->isEnabled(), state);
    QCOMPARE(d->ui->firstDoseUsesTo->isEnabled(), state);
    QCOMPARE(d->ui->firstDoseFromValue->isEnabled(), state);
    QCOMPARE(d->ui->firstDoseFromUnits->isEnabled(), state);
    QCOMPARE(d->ui->firstDoseFromRepart->isEnabled(), state);
    QCOMPARE(d->ui->firstDoseToValue->isEnabled(), state);
    QCOMPARE(d->ui->firstDoseToUnits->isEnabled(), state);
    QCOMPARE(d->ui->firstDoseToRepart->isEnabled(), state);
    QCOMPARE(d->ui->secondDoseUsesFrom->isEnabled(), state);
    QCOMPARE(d->ui->secondDoseUsesTo->isEnabled(), state);
    QCOMPARE(d->ui->secondDoseFromValue->isEnabled(), state);
    QCOMPARE(d->ui->secondDoseFromUnits->isEnabled(), state);
    QCOMPARE(d->ui->secondDoseFromRepart->isEnabled(), state);
    QCOMPARE(d->ui->secondDoseToValue->isEnabled(), state);
    QCOMPARE(d->ui->secondDoseToUnits->isEnabled(), state);
    QCOMPARE(d->ui->secondDoseToRepart->isEnabled(), state);

    // Actions
    QCOMPARE(d->aEdit->isEnabled(), !state);
    QCOMPARE(d->aRevert->isEnabled(), state);
    QCOMPARE(d->aSave->isEnabled(), state);
    // QCOMPARE(d->aRemoveCurrent->isEnabled(), state);
    // QCOMPARE(d->aSplitInteractionAccordingToLevel->isEnabled(), state);
}

void DrugDrugInteractionEditorWidget::test_actions()
{
    // First pass: editors & edition action are disabled
    test_editorEnabledState(false);

    // Trigger edit action
    d->aEdit->trigger();
    test_editorEnabledState(true);

    // Trigger revert action
    d->aRevert->trigger();
    test_editorEnabledState(false);

//    d->aCreateNew->setText(tkTr(Trans::Constants::ADD_TEXT));
//    d->aSave->setText(tkTr(Trans::Constants::FILESAVE_TEXT));
//    d->aRemoveCurrent->setText(tkTr(Trans::Constants::REMOVE_TEXT));
//    d->aSplitInteractionAccordingToLevel->setToolTip(d->aSplitInteractionAccordingToLevel->text());
}

void DrugDrugInteractionEditorWidget::test_itemCreation()
{
    // qWarning() << "\n" << Q_FUNC_INFO << "\n";
}

void DrugDrugInteractionEditorWidget::test_edition()
{
    // qWarning() << "\n" << Q_FUNC_INFO << "\n";
    const int loop = 10;

    Utils::Randomizer r;
    r.setPathToFiles(settings()->path(Core::ISettings::BundleResourcesPath) + "/textfiles/");
    QCOMPARE(d->_ddiProxyModel->rowCount(), ddiCore()->drugDrugInteractionTableModel()->rowCount());

    int unitsCount = ddiCore()->drugDrugInteractionTableModel()->units().count() - 1;
    int repartitionsCount = ddiCore()->drugDrugInteractionTableModel()->repartitions().count() - 1;

    // Edit, modify, save, test values
    for(int l = 0; l < loop; ++l) {
        // Select an item
        int selectedRow = r.randomInt(0, ddiCore()->drugDrugInteractionTableModel()->rowCount() - 1);
        QModelIndex selectMe = d->_ddiProxyModel->mapFromSource(ddiCore()->drugDrugInteractionTableModel()->index(selectedRow, 0));
        QString selectedId = ddiCore()->drugDrugInteractionTableModel()->index(selectedRow, 0).data().toString();
        d->m_EditingIndex = QModelIndex(); // no dialog when activating the interactor
        d->ui->tableView->selectionModel()->select(selectMe, QItemSelectionModel::SelectCurrent);
        interactionActivated(selectMe);

        // No real interaction -> continue
        if (selectMe.data().toString().isEmpty())
            continue;

        qWarning() << "\n\n** Running test on interactor Id: " << selectMe.data().toString() << "m_EditingIndex" << d->m_EditingIndex.data().toString();

        // Trigger Edit action
        d->aEdit->trigger();

        // Edit all values
        // select * from ddi where riskfr like "_TEST_%"
        d->ui->risk->clear();
        d->ui->management->clear();
        d->ui->risk_en->clear();
        d->ui->management_en->clear();
        d->ui->comment->clear();

        QString riskFr = Utils::removeAccents(QString("_TEST_%1").arg(r.randomWords(r.randomInt(5, 25))));
        QString riskEn = Utils::removeAccents(QString("_TEST_%1").arg(r.randomWords(r.randomInt(5, 25))));
        QString manFr = Utils::removeAccents(QString("_TEST_%1").arg(r.randomWords(r.randomInt(5, 25))));
        QString manEn = Utils::removeAccents(QString("_TEST_%1").arg(r.randomWords(r.randomInt(5, 25))));
        QString comment = Utils::removeAccents(QString("_TEST_%1").arg(r.randomWords(r.randomInt(5, 25))));
        bool isRev = r.randomBool();
        bool firstUsesFrom = r.randomBool();
        bool firstUsesTo = r.randomBool();
        bool secUsesFrom = r.randomBool();
        bool secUsesTo = r.randomBool();
        int level = r.randomInt(0, 6);
        QDate creationDate = r.randomDate(2000);

        d->ui->isReviewed->setChecked(isRev);
        d->ui->createdOn->setDate(creationDate);
        d->ui->comboLevel->setCurrentIndex(level);

        d->ui->firstDoseUsesFrom->setChecked(firstUsesFrom);
        d->ui->firstDoseUsesTo->setChecked(firstUsesTo);
        int firstFromDoseValue = r.randomInt(0, 100000);
        int firstFromUnitId = r.randomInt(0, unitsCount);
        int firstFromRepartitionId = r.randomInt(0, repartitionsCount);
        d->ui->firstDoseFromUnits->setCurrentIndex(firstFromUnitId);
        d->ui->firstDoseFromRepart->setCurrentIndex(firstFromRepartitionId);
        d->ui->firstDoseFromValue->setText(QString::number(firstFromDoseValue));
        int firstToDoseValue = r.randomInt(0, 100000);
        int firstToUnitId = r.randomInt(0, unitsCount);
        int firstToRepartitionId = r.randomInt(0, repartitionsCount);
        d->ui->firstDoseToUnits->setCurrentIndex(firstToUnitId);
        d->ui->firstDoseToRepart->setCurrentIndex(firstToRepartitionId);
        d->ui->firstDoseToValue->setText(QString::number(firstToDoseValue));

        d->ui->secondDoseUsesFrom->setChecked(secUsesFrom);
        d->ui->secondDoseUsesTo->setChecked(secUsesTo);
        int secondFromDoseValue = r.randomInt(0, 100000);
        int secondFromUnitId = r.randomInt(0, unitsCount);
        int secondFromRepartitionId = r.randomInt(0, repartitionsCount);
        d->ui->secondDoseFromUnits->setCurrentIndex(secondFromUnitId);
        d->ui->secondDoseFromRepart->setCurrentIndex(secondFromRepartitionId);
        d->ui->secondDoseFromValue->setText(QString::number(secondFromDoseValue));
        int secondToDoseValue = r.randomInt(0, 100000);
        int secondToUnitId = r.randomInt(0, unitsCount);
        int secondToRepartitionId = r.randomInt(0, repartitionsCount);
        d->ui->secondDoseToUnits->setCurrentIndex(secondToUnitId);
        d->ui->secondDoseToRepart->setCurrentIndex(secondToRepartitionId);
        d->ui->secondDoseToValue->setText(QString::number(secondToDoseValue));

        QTest::keyClicks(d->ui->risk, riskFr, Qt::NoModifier);
        QTest::keyClicks(d->ui->risk_en, riskEn, Qt::NoModifier);
        QTest::keyClicks(d->ui->management, manFr, Qt::NoModifier);
        QTest::keyClicks(d->ui->management_en, manEn, Qt::NoModifier);
        QTest::keyClicks(d->ui->comment, comment, Qt::NoModifier);

        QStringList bib;
        bib << QString::number(r.randomInt(10000, 122349234))
            << QString::number(r.randomInt(10000, 122349234))
            << QString::number(r.randomInt(10000, 122349234))
            << QString::number(r.randomInt(10000, 122349234))
            << QString::number(r.randomInt(10000, 122349234));
        d->_biblioModel->setStringList(bib);

        // Trigger Save action
        d->aSave->trigger();
        QCOMPARE(ddiCore()->drugDrugInteractionTableModel()->index(selectedRow, 0).data().toString(), selectedId);
        QDate lastUpdate = QDate::currentDate();

        // Test model values
        QAbstractItemModel *model = ddiCore()->drugDrugInteractionTableModel();
        QCOMPARE(model->index(selectedRow, DrugDrugInteractionTableModel::IsValid).data().toBool(), true);
        QCOMPARE(model->index(selectedRow, DrugDrugInteractionTableModel::IsReviewed).data().toBool(), isRev);
        QCOMPARE(model->index(selectedRow, DrugDrugInteractionTableModel::DateCreation).data().toDate(), creationDate);
        QCOMPARE(model->index(selectedRow, DrugDrugInteractionTableModel::DateLastUpdate).data().toDate(), lastUpdate);

        QCOMPARE(model->index(selectedRow, DrugDrugInteractionTableModel::RiskFr).data().toString(), riskFr);
        QCOMPARE(model->index(selectedRow, DrugDrugInteractionTableModel::RiskEn).data().toString(), riskEn);
        QCOMPARE(model->index(selectedRow, DrugDrugInteractionTableModel::ManagementFr).data().toString(), manFr);
        QCOMPARE(model->index(selectedRow, DrugDrugInteractionTableModel::ManagementEn).data().toString(), manEn);
        QCOMPARE(model->index(selectedRow, DrugDrugInteractionTableModel::LevelComboIndex).data().toInt(), level);
        QCOMPARE(model->index(selectedRow, DrugDrugInteractionTableModel::Comment).data().toString(), comment);

        QCOMPARE(model->index(selectedRow, DrugDrugInteractionTableModel::FirstDoseUseFrom).data().toBool(), firstUsesFrom);
        QCOMPARE(model->index(selectedRow, DrugDrugInteractionTableModel::FirstDoseFromValue).data().toInt(), firstFromDoseValue);
        QCOMPARE(model->index(selectedRow, DrugDrugInteractionTableModel::FirstDoseFromUnitsId).data().toInt(), firstFromUnitId);
        QCOMPARE(model->index(selectedRow, DrugDrugInteractionTableModel::FirstDoseFromRepartitionId).data().toInt(), firstFromRepartitionId);

        QCOMPARE(model->index(selectedRow, DrugDrugInteractionTableModel::FirstDoseUsesTo).data().toBool(), firstUsesTo);
        QCOMPARE(model->index(selectedRow, DrugDrugInteractionTableModel::FirstDoseToValue).data().toInt(), firstToDoseValue);
        QCOMPARE(model->index(selectedRow, DrugDrugInteractionTableModel::FirstDoseToUnitsId).data().toInt(), firstToUnitId);
        QCOMPARE(model->index(selectedRow, DrugDrugInteractionTableModel::FirstDoseToRepartitionId).data().toInt(), firstToRepartitionId);

        QCOMPARE(model->index(selectedRow, DrugDrugInteractionTableModel::SecondDoseUseFrom).data().toBool(), secUsesFrom);
        QCOMPARE(model->index(selectedRow, DrugDrugInteractionTableModel::SecondDoseFromValue).data().toInt(), secondFromDoseValue);
        QCOMPARE(model->index(selectedRow, DrugDrugInteractionTableModel::SecondDoseFromUnitsId).data().toInt(), secondFromUnitId);
        QCOMPARE(model->index(selectedRow, DrugDrugInteractionTableModel::SecondDoseFromRepartitionId).data().toInt(), secondFromRepartitionId);

        QCOMPARE(model->index(selectedRow, DrugDrugInteractionTableModel::SecondDoseUsesTo).data().toBool(), secUsesTo);
        QCOMPARE(model->index(selectedRow, DrugDrugInteractionTableModel::SecondDoseToValue).data().toInt(), secondToDoseValue);
        QCOMPARE(model->index(selectedRow, DrugDrugInteractionTableModel::SecondDoseToUnitsId).data().toInt(), secondToUnitId);
        QCOMPARE(model->index(selectedRow, DrugDrugInteractionTableModel::SecondDoseToRepartitionId).data().toInt(), secondToRepartitionId);

        QCOMPARE(model->index(selectedRow, DrugDrugInteractionTableModel::PMIDStringList).data().toStringList(), bib);
    }
}

#endif // WITH_TESTS
