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
#include "drugselector.h"

#include <drugsplugin/constants.h>
#include <drugsplugin/drugswidgetmanager.h>
#include <drugsplugin/drugswidget/textualprescriptiondialog.h>

#include <drugsbaseplugin/drugbasecore.h>
#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/drugsmodel.h>
#include <drugsbaseplugin/globaldrugsmodel.h>
#include <drugsbaseplugin/drugsdatabaseselector.h>
#include <drugsbaseplugin/atctreemodel.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/constants_icons.h>

#include <translationutils/constanttranslations.h>

#include <QApplication>
#include <QToolButton>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QHeaderView>
#include <QScrollBar>
#include <QSortFilterProxyModel>

#include <QDebug>

#ifdef DEBUG
enum { WarnSearchFilter = false };
#else
enum { WarnSearchFilter = false };
#endif

using namespace DrugsWidget;
using namespace DrugsWidget::Internal;
using namespace Trans::ConstantTranslations;

static inline DrugsDB::DrugsBase &drugsBase() {return DrugsDB::DrugBaseCore::instance().drugsBase();}
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Core::ActionManager *actionManager() { return Core::ICore::instance()->actionManager(); }
static inline DrugsDB::DrugsModel *drugModel() { return DrugsWidget::DrugsWidgetManager::instance()->currentDrugsModel(); }
static inline Core::IMainWindow *mainWindow() {return Core::ICore::instance()->mainWindow();}

namespace DrugsWidget {
namespace Internal {
class TreeProxyModel : public QSortFilterProxyModel
{
public:
    TreeProxyModel(QObject *parent = 0)
        : QSortFilterProxyModel(parent)
    {
        setFilterCaseSensitivity(Qt::CaseInsensitive);
    }

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
    {
        if(filterRegExp().isEmpty())
            return true;

        QModelIndex current(sourceModel()->index(sourceRow, filterKeyColumn(), sourceParent));

        if(sourceModel()->hasChildren(current)) {
            bool atLeastOneValidChild = false;
            int i = 0;
            while(!atLeastOneValidChild) {
                const QModelIndex child(current.child(i, current.column()));
                if (!child.isValid())
                    // No valid child
                    break;

                atLeastOneValidChild = filterAcceptsRow(i, current);
                i++;
            }
            return atLeastOneValidChild;
        }

        return sourceModel()->data(current).toString().contains(filterRegExp());
    }
};
}
}

DrugSelector::DrugSelector(QWidget *parent) :
        QWidget(parent),
        m_GlobalDrugsModel(0), m_InnModel(0), m_AtcProxyModel(0),
        m_SearchToolButton(0),
        m_DrugsHistoryButton(0),
        m_HistoryAct(0)
{
    setupUi(this);
}

DrugSelector::~DrugSelector()
{
    mainWindow()->setWindowTitle(m_WinTitle);
}

void DrugSelector::initialize()
{
    textButton->setIcon(theme()->icon(Core::Constants::ICONPENCIL));

    m_WinTitle = mainWindow()->windowTitle();

    createToolButtons();

    createDrugModelView();
    createINNModelView();
    createDrugsHistoryActions();

    // select last search method
    int m = settings()->value(Constants::S_SEARCHMETHOD).toInt();
    if (m==Constants::SearchInn) {
        if (!drugsBase().actualDatabaseInformation() || !drugsBase().actualDatabaseInformation()->atcCompatible)
            m = Constants::SearchCommercial;
    }
    setSearchMethod(m);
    QAction *a = 0;
    switch (m)
    {
    case Constants::SearchCommercial: a = actionManager()->command(Constants::A_SEARCH_COMMERCIAL)->action(); break;
    case Constants::SearchMolecules: a = actionManager()->command(Constants::A_SEARCH_MOLECULES)->action(); break;
    case Constants::SearchInn:
    {
        if (drugsBase().actualDatabaseInformation() && drugsBase().actualDatabaseInformation()->atcCompatible)
            a = actionManager()->command(Constants::A_SEARCH_INN)->action();
        else
            a = actionManager()->command(Constants::A_SEARCH_COMMERCIAL)->action();
        break;
    }
    }

    if (a)
        a->setChecked(true);

    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 3);
    drugsView->setFocus();

    searchLine->setDelayedSignals(true);

    connect(&drugsBase(), SIGNAL(drugsBaseHasChanged()), this, SLOT(onDrugsBaseChanged()));
}

/** \brief Define the \e font to use on all the views */
void DrugSelector::setFont(const QFont &font)
{
    drugsView->setFont(font);
    InnView->setFont(font);
    searchLine->setFont(font);
}

void DrugSelector::onSearchShortCutTriggered()
{
    searchLine->setFocus();
    searchLine->selectAll();
}

void DrugSelector::createToolButtons()
{
    m_SearchToolButton = new QToolButton(searchLine);   // parent object will be redefined
    m_SearchToolButton->setPopupMode(QToolButton::InstantPopup);
    m_SearchToolButton->setIcon(theme()->icon(Core::Constants::ICONSEARCH));

    m_DrugsHistoryButton = new QToolButton(searchLine);
    m_DrugsHistoryButton->setPopupMode(QToolButton::InstantPopup);
    m_DrugsHistoryButton->setToolTip(tr("Selected drugs history"));
    m_DrugsHistoryButton->setIcon(theme()->icon(Core::Constants::ICONEDIT));

    // add buttons to search line
    searchLine->setLeftButton(m_SearchToolButton);
    searchLine->setRightButton(m_DrugsHistoryButton);

    refreshSearchToolButton();
    refreshAvailableDrugsDatabaseButtons();
    connect(drugsDatabaseSelectorButton, SIGNAL(triggered(QAction*)), drugsDatabaseSelectorButton, SLOT(setDefaultAction(QAction*)));
    connect(drugsDatabaseSelectorButton, SIGNAL(triggered(QAction*)), this, SLOT(changeDrugBaseUid(QAction*)));
}

void DrugSelector::refreshSearchToolButton()
{
    // clear button
    foreach(QAction *a, m_SearchToolButton->actions())
        m_SearchToolButton->removeAction(a);

    bool includeInn = true;
    if (drugsBase().actualDatabaseInformation()) {
        if (!drugsBase().actualDatabaseInformation()->atcCompatible) {
            includeInn = false;
        }
    } else {
        includeInn = false;
    }
    Core::ActionManager *am = actionManager();
    Core::Command *cmd = am->command(Constants::A_SEARCH_COMMERCIAL);
    m_SearchToolButton->addAction(cmd->action());
    cmd->action()->trigger();
    cmd = am->command(Constants::A_SEARCH_MOLECULES);
    m_SearchToolButton->addAction(cmd->action());
    if (includeInn) {
        cmd = am->command(Constants::A_SEARCH_INN);
        m_SearchToolButton->addAction(cmd->action());
    }
}

void DrugSelector::refreshAvailableDrugsDatabaseButtons()
{
    for(int i= drugsDatabaseSelectorButton->actions().count()-1; i > -1 ; --i) {
        drugsDatabaseSelectorButton->removeAction(drugsDatabaseSelectorButton->actions().at(i));
    }

    // Create drug database selector toolbutton
    QAction *defaultAction = 0;
    QVector<DrugsDB::DatabaseInfos *> list = drugsBase().getAllDrugSourceInformation();
    for(int i=0; i < list.count(); ++i) {
        DrugsDB::DatabaseInfos *info = list.at(i);
        QAction *a = new QAction(this);
        a->setText(info->translatedName());
        a->setToolTip(info->translatedName());
        a->setData(info->identifier);
        a->setIcon(theme()->icon("/flags/"+info->lang_country.mid(3)+".png"));
        drugsDatabaseSelectorButton->addAction(a);
        if (info->identifier==drugsBase().actualDatabaseInformation()->identifier) {
            defaultAction = a;
        }
    }
    drugsDatabaseSelectorButton->setDefaultAction(defaultAction);
}

/** \brief Update the views if user selected another drugs database */
void DrugSelector::onDrugsBaseChanged()
{
    refreshAvailableDrugsDatabaseButtons();
    refreshSearchToolButton();
}

void DrugSelector::changeDrugBaseUid(QAction *a)
{
    drugsBase().changeCurrentDrugSourceUid(a->data());
}

void DrugSelector::createDrugModelView()
{
    using namespace DrugsDB::Constants;
    // insert SQL drugs model and table view
    if (!m_GlobalDrugsModel) {
        if (m_SearchMethod == Constants::SearchCommercial)
            m_GlobalDrugsModel = new DrugsDB::GlobalDrugsModel(DrugsDB::GlobalDrugsModel::SearchByBrandName, this);
        else if (m_SearchMethod == Constants::SearchMolecules)
            m_GlobalDrugsModel = new DrugsDB::GlobalDrugsModel(DrugsDB::GlobalDrugsModel::SearchByMolecularName, this);
        else
            m_GlobalDrugsModel = new DrugsDB::GlobalDrugsModel(DrugsDB::GlobalDrugsModel::SearchByInnName, this);
    }
    // create the view
    drugsView->setModel(m_GlobalDrugsModel);
    //      drugsView->sortByColumn(1 , Qt::AscendingOrder);  // NOT SUPPORTED BY WIN32 CROSS-COMPILATION !!!!
    drugsView->setColumnHidden(0 , true);
    updateDrugsViewColumns();
    drugsView->verticalHeader()->hide();
    drugsView->horizontalHeader()->hide();
//    drugsView->resizeColumnsToContents();
}

void DrugSelector::updateDrugsViewColumns()
{
    for(int i = 0; i < m_GlobalDrugsModel->columnCount(); ++i)
        drugsView->hideColumn(i);
    drugsView->showColumn(DrugsDB::GlobalDrugsModel::BrandName);
    drugsView->setColumnHidden(DrugsDB::GlobalDrugsModel::Routes, !settings()->value(DrugsDB::Constants::S_SELECTOR_SHOWROUTE).toBool());
    drugsView->setColumnHidden(DrugsDB::GlobalDrugsModel::Forms , !settings()->value(DrugsDB::Constants::S_SELECTOR_SHOWFORM).toBool());
    drugsView->setColumnHidden(DrugsDB::GlobalDrugsModel::Strength , !settings()->value(DrugsDB::Constants::S_SELECTOR_SHOWSTRENGTH).toBool());
    drugsView->horizontalHeader()->setStretchLastSection(false);
#if QT_VERSION < 0x050000
    drugsView->horizontalHeader()->setResizeMode(DrugsDB::GlobalDrugsModel::BrandName, QHeaderView::Stretch);
#else
    // Qt5
    drugsView->horizontalHeader()->setSectionResizeMode(DrugsDB::GlobalDrugsModel::BrandName, QHeaderView::Stretch);
#endif
}

/**
 * Clear the view:
 * - clear the search line edit,
 * - keep the search method (brand name, INN, molecule search)
 * - reset the search filter
 */
void DrugSelector::clear()
{
    searchLine->clear();
    updateModelFilter();
}

void DrugSelector::createINNModelView()
{
    using namespace DrugsDB::Constants;
    // create model and tableview for Iam Class / INNs
    m_InnModel = new DrugsDB::AtcTreeModel(this);
    m_InnModel->init();

    m_AtcProxyModel = new Internal::TreeProxyModel(this);
    m_AtcProxyModel->setSourceModel(m_InnModel);
    InnView->setModel(m_AtcProxyModel);

    //InnView->setModel(m_InnModel);
    InnView->header()->setStretchLastSection(false);
#if QT_VERSION < 0x050000
    InnView->header()->setResizeMode(0, QHeaderView::Stretch);
#else
    // Qt5
    InnView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
#endif
    InnView->hide();
}

void DrugSelector::createDrugsHistoryActions()
{
    if (m_HistoryAct)
        delete m_HistoryAct;

    m_HistoryAct = new QActionGroup(this);
    foreach(QString s, settings()->value(Constants::S_DRUGHISTORY).toStringList()) {
        QAction *a = new QAction(s.left(s.lastIndexOf(",")).left(70) + "..." , m_HistoryAct);
        a->setToolTip(s);
        m_HistoryAct->addAction(a);
    }

    connect(m_HistoryAct, SIGNAL(triggered(QAction *)),
             this, SLOT(historyAct_triggered(QAction *)));

    m_DrugsHistoryButton->addActions(m_HistoryAct->actions());
}

/** \brief Connect the search line edit to the filter update */
void DrugSelector::connectFilter()
{
    connect(searchLine, SIGNAL(textChanged(const QString &)), this, SLOT(updateModelFilter()));
}

/** \brief Disconnect the search line edit to the filter update */
void DrugSelector::disconnectFilter()
{
    disconnect(searchLine, SIGNAL(textChanged(const QString &)), this, SLOT(updateModelFilter()));
}

void DrugSelector::historyAct_triggered(QAction *action)
{
    using namespace DrugsDB::Constants;
    // action tooltip == full drug name
    //    searchLine->setText(action->toolTip());
    drugsView->setFocus();
    QHashWhere where;
    where.insert(DRUGS_MARKET, "=1");
    where.insert(DRUGS_NAME , QString("= \"%1\"").arg(action->toolTip()));
    m_GlobalDrugsModel->setFilter(drugsBase().getWhereClause(Table_DRUGS, where));
}

//--------------------------------------------------------------------------------------------------------
//-------------------------------------- DrugSelector private slots -------------------------------------
//--------------------------------------------------------------------------------------------------------
/** \brief Prepare the filter's masks according to the selected search method. */
void DrugSelector::setSearchMethod(int method)
{
    if (method == Constants::SearchCommercial) {
#ifdef FREEDIAMS
        mainWindow()->setWindowTitle(m_WinTitle + " - " + QApplication::translate(Constants::DRUGCONSTANTS_TR_CONTEXT, Constants::SEARCHCOMMERCIAL_TEXT));
#endif
        m_filterModel = "";
        InnView->hide();
        m_SearchMethod = method;
        m_GlobalDrugsModel->setSearchMode(DrugsDB::GlobalDrugsModel::SearchByBrandName);
    }
    else if (method == Constants::SearchMolecules) {
#ifdef FREEDIAMS
        mainWindow()->setWindowTitle(m_WinTitle + " - " + QApplication::translate(Constants::DRUGCONSTANTS_TR_CONTEXT, Constants::SEARCHMOLECULES_TEXT));
#endif
        m_filterModel = "";
        InnView->hide();
        m_SearchMethod = method;
        m_GlobalDrugsModel->setSearchMode(DrugsDB::GlobalDrugsModel::SearchByMolecularName);
    }
    else if (method == Constants::SearchInn) {
#ifdef FREEDIAMS
        mainWindow()->setWindowTitle(m_WinTitle + " - " + QApplication::translate(Constants::DRUGCONSTANTS_TR_CONTEXT, Constants::SEARCHINN_TEXT));
#endif
        m_filterModel = "";
        InnView->show();
        m_SearchMethod = method;
        m_GlobalDrugsModel->setSearchMode(DrugsDB::GlobalDrugsModel::SearchByInnName);
    }

    // store search method into settings
    settings()->setValue(Constants::S_SEARCHMETHOD, m_SearchMethod);

    // update model
    updateModelFilter();
}

/**
  \brief update the GlobalDrugModel filter according to the search method and the search text.
  \sa void DrugSelector::setSearchMethod(int method)
*/
void DrugSelector::updateModelFilter()
{
    if (searchLine->text().isEmpty()) {
        if (WarnSearchFilter)
            qWarning() << "No search filter";
        m_GlobalDrugsModel->setFilter("");
        m_AtcProxyModel->setFilterWildcard("*");
        return;
    }
    QString tmp = m_filterModel;
    QString search = searchLine->text().replace("*", "%");
    m_GlobalDrugsModel->setFilter(search);
    if (m_SearchMethod == Constants::SearchInn) {
        // Inform filter proxymodel
        m_AtcProxyModel->setFilterWildcard(search + "*");
        m_AtcProxyModel->setFilterKeyColumn(0);
        InnView->expandAll();
    }
}

void DrugSelector::on_InnView_clicked(const QModelIndex &index)
{
    if (m_SearchMethod != Constants::SearchInn)
        return;
    if (!index.isValid())
        return;
    QString inn = m_AtcProxyModel->index(index.row(), DrugsDB::AtcTreeModel::ATC_Label, index.parent()).data().toString();
    m_GlobalDrugsModel->setFilter(inn);
}

/** When a user select a drug, this slot is called. The drug is added to the drug history and signals are emitted */
void DrugSelector::on_drugsView_doubleClicked(const QModelIndex &index)
{
    // retrieve CIS of drug and emit signal
    if (!index.isValid())
        return;

    // store drug into history
    QStringList hist = settings()->value(Constants::S_DRUGHISTORY).toStringList();
    hist.removeAll(index.data().toString());
    if (hist.count() && (hist.count() == settings()->value(Constants::S_HISTORYSIZE).toInt()))
        hist.removeFirst();
    hist << index.data().toString();
    settings()->setValue(Constants::S_DRUGHISTORY, hist);
    createDrugsHistoryActions();

    const QVariant &drugId = m_GlobalDrugsModel->index(index.row(), DrugsDB::GlobalDrugsModel::DrugId).data();
    Q_EMIT drugSelected(drugId);
    Q_EMIT drugSelected(index);
}

/** User asked to add a textual prescription. This member creates a dialog and send the text to the model. */
void DrugSelector::on_textButton_clicked()
{
    TextualPrescriptionDialog dlg(this);
    int r = dlg.exec();
    if (r==QDialog::Accepted) {
        int row = drugModel()->addTextualPrescription(dlg.drugLabel(), dlg.drugNote());
        drugModel()->setData(drugModel()->index(row, DrugsDB::Constants::Prescription::IsALD), dlg.isALD());
    }
}

void DrugSelector::changeEvent(QEvent *e)
{
    if (e->type()==QEvent::LanguageChange) {
        refreshAvailableDrugsDatabaseButtons();
    }
}
