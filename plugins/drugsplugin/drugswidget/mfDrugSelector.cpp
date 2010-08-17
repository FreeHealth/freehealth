/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
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
#include "mfDrugSelector.h"

#include <drugsplugin/constants.h>
#include <drugsplugin/drugswidgetmanager.h>
#include <drugsplugin/drugswidget/textualprescriptiondialog.h>

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

#include <QDebug>

#ifdef DEBUG
enum { WarnSearchFilter = false };
#else
enum { WarnSearchFilter = false };
#endif

using namespace DrugsWidget;
using namespace DrugsWidget::Internal;
using namespace Trans::ConstantTranslations;

static inline DrugsDB::Internal::DrugsBase *drugsBase() {return DrugsDB::Internal::DrugsBase::instance();}
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Core::ActionManager *actionManager() { return Core::ICore::instance()->actionManager(); }
inline static DrugsDB::DrugsModel *drugModel() { return DrugsWidget::DrugsWidgetManager::instance()->currentDrugsModel(); }
static inline Core::IMainWindow *mainWindow() {return Core::ICore::instance()->mainWindow();}


DrugSelector::DrugSelector(QWidget *parent) :
        QWidget(parent),
        m_DrugsModel(0), m_InnModel(0),
        m_SearchToolButton(0),
        m_DrugsHistoricButton(0),
        m_HistoryAct(0)
{
}

DrugSelector::~DrugSelector()
{
    mainWindow()->setWindowTitle(m_WinTitle);
}

void DrugSelector::initialize()
{
    setupUi(this);
    textButton->setIcon(theme()->icon(Core::Constants::ICONPENCIL));

    m_WinTitle = mainWindow()->windowTitle();

    createToolButtons();

    createDrugModelView();
    createINNModelView();
    createDrugsHistoryActions();

    // select last search method
    int m = settings()->value(Constants::S_SEARCHMETHOD).toInt();
    setSearchMethod(m);
    QAction *a = 0;
    switch (m)
    {
        case Constants::SearchCommercial : a = actionManager()->command(Constants::A_SEARCH_COMMERCIAL)->action(); break;
        case Constants::SearchMolecules : a = actionManager()->command(Constants::A_SEARCH_MOLECULES)->action(); break;
        case Constants::SearchInn : a = actionManager()->command(Constants::A_SEARCH_INN)->action(); break;
    }
    if (a) {
        /** \todo Check the action in the menu/toolbutton... */
        a->trigger();
    }

    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 3);
    drugsView->setFocus();

    retranslateUi("");

    searchLine->setDelayedSignals(true);

    connect(drugsBase(), SIGNAL(drugsBaseHasChanged()), this, SLOT(onDrugsBaseChanged()));
}

/** \brief Define the \e font to use on all the views */
void DrugSelector::setFont(const QFont &font)
{
    drugsView->setFont(font);
    InnView->setFont(font);
    searchLine->setFont(font);
}

void DrugSelector::createToolButtons()
{
    m_SearchToolButton = new QToolButton(searchLine);   // parent object will be redefined
    m_SearchToolButton->setPopupMode(QToolButton::InstantPopup);
    m_SearchToolButton->setIcon(theme()->icon(Core::Constants::ICONSEARCH));
    Core::ActionManager *am = actionManager();
    Core::Command *cmd = am->command(Constants::A_SEARCH_COMMERCIAL);
    m_SearchToolButton->addAction(cmd->action());
    cmd->action()->trigger();
    cmd = am->command(Constants::A_SEARCH_MOLECULES);
    m_SearchToolButton->addAction(cmd->action());
    cmd = am->command(Constants::A_SEARCH_INN);
    m_SearchToolButton->addAction(cmd->action());

    m_DrugsHistoricButton = new QToolButton(searchLine);
    m_DrugsHistoricButton->setPopupMode(QToolButton::InstantPopup);
    m_DrugsHistoricButton->setToolTip(tr("Selected drugs historic."));
    m_DrugsHistoricButton->setIcon(theme()->icon(Core::Constants::ICONEDIT));

    // add buttons to search line
    searchLine->setLeftButton(m_SearchToolButton);
    searchLine->setRightButton(m_DrugsHistoricButton);
}

/** \brief Update the views if user selected another drugs database */
void DrugSelector::onDrugsBaseChanged()
{
    delete m_DrugsModel;
    m_DrugsModel = 0;
    createDrugModelView();
}

void DrugSelector::createDrugModelView()
{
    using namespace DrugsDB::Constants;
    // insert SQL drugs model and table view
    if (!m_DrugsModel) {
        m_DrugsModel = new DrugsDB::GlobalDrugsModel(this);
    }
    // create the view
    drugsView->setModel(m_DrugsModel);
    //      drugsView->sortByColumn(1 , Qt::AscendingOrder);  // NOT SUPPORTED BY WIN32 CROSS-COMPILATION !!!!
    drugsView->setColumnHidden(0 , true);
    updateDrugsViewColumns();
    drugsView->verticalHeader()->hide();
    drugsView->horizontalHeader()->hide();
    drugsView->horizontalHeader()->setStretchLastSection(false);
    drugsView->horizontalHeader()->setResizeMode(DrugsDB::Constants::DRUGS_NAME, QHeaderView::Stretch);
//    drugsView->resizeColumnsToContents();
}

void DrugSelector::updateDrugsViewColumns()
{
    drugsView->setColumnHidden(DrugsDB::Constants::DRUGS_ROUTE, !settings()->value(DrugsDB::Constants::S_SELECTOR_SHOWFORM).toBool());
    drugsView->setColumnHidden(DrugsDB::Constants::DRUGS_FORM , !settings()->value(DrugsDB::Constants::S_SELECTOR_SHOWROUTE).toBool());
    drugsView->setColumnHidden(DrugsDB::Constants::DRUGS_STRENGTH , !settings()->value(DrugsDB::Constants::S_SELECTOR_SHOWSTRENGTH).toBool());
    // managing model fields
    drugsView->hideColumn(DrugsDB::Constants::DRUGS_LINK_SPC);
    drugsView->hideColumn(DrugsDB::Constants::DRUGS_MARKET);
    drugsView->hideColumn(DrugsDB::Constants::DRUGS_AUTHORIZATION);
    drugsView->hideColumn(DrugsDB::Constants::DRUGS_TYPE_MP);
    drugsView->hideColumn(DrugsDB::Constants::DRUGS_ATC);
}

void DrugSelector::createINNModelView()
{
    using namespace DrugsDB::Constants;
    // create model and tableview for Iam Class / INNs
    m_InnModel = new DrugsDB::AtcTreeModel(this);
    m_InnModel->init();
    InnView->setModel(m_InnModel);
    InnView->header()->setStretchLastSection(false);
    InnView->header()->setResizeMode(0, QHeaderView::Stretch);
    InnView->hide();
}

void DrugSelector::createDrugsHistoryActions()
{
    if (m_HistoryAct)
        delete m_HistoryAct;

    m_HistoryAct = new QActionGroup(this);
    foreach(QString s, settings()->value(Constants::S_DRUGHISTORY).toStringList()) {
        QAction * a = new QAction(s.left(s.lastIndexOf(",")).left(70) + "..." , m_HistoryAct);
        a->setToolTip(s);
        m_HistoryAct->addAction(a);
    }

    connect(m_HistoryAct, SIGNAL(triggered(QAction *)),
             this, SLOT(historyAct_triggered(QAction *)));

    m_DrugsHistoricButton->addActions(m_HistoryAct->actions());
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
    m_DrugsModel->setFilter(drugsBase()->getWhereClause(Table_DRUGS, where));
}

//--------------------------------------------------------------------------------------------------------
//-------------------------------------- DrugSelector private slots -------------------------------------
//--------------------------------------------------------------------------------------------------------
void DrugSelector::retranslateUi(const QString &)
{
//    if (m_DrugsModel) {
//        m_DrugsModel->setHeaderData(1, Qt::Horizontal, tr("Short Name"));
//        m_DrugsModel->setHeaderData(2, Qt::Horizontal, tr("Form"));
//        m_DrugsModel->setHeaderData(3, Qt::Horizontal, tr("Administration"));
//    }
}

/** \brief Prepare the filter's masks according to the selected search method. */
void DrugSelector::setSearchMethod(int method)
{
    if (method == Constants::SearchCommercial) {
#ifdef FREEDIAMS
        mainWindow()->setWindowTitle(m_WinTitle + " - " + tkTr(Constants::SEARCHCOMMERCIAL_TEXT));
#endif
        m_filterModel = "";
        InnView->hide();
        m_SearchMethod = method;
        QHashWhere where;
        if (drugsBase()->actualDatabaseInformations()) {
            where.insert(DrugsDB::Constants::DRUGS_MARKET, "=1");
            m_filterModel = drugsBase()->getWhereClause(DrugsDB::Constants::Table_DRUGS, where);
            m_filterModel.prepend(drugsBase()->actualDatabaseInformations()->drugsNameConstructorSearchFilter +
                                  " LIKE '__replaceit__%' AND ");
        } else {
            where.insert(DrugsDB::Constants::DRUGS_MARKET, "=1");
            where.insert(DrugsDB::Constants::DRUGS_NAME , "LIKE '__replaceit__%'");
            m_filterModel = drugsBase()->getWhereClause(DrugsDB::Constants::Table_DRUGS, where);
        }
    }
    else if (method == Constants::SearchMolecules) {
#ifdef FREEDIAMS
        mainWindow()->setWindowTitle(m_WinTitle + " - " + tkTr(Constants::SEARCHMOLECULES_TEXT));
#endif
        m_filterModel = "";
        InnView->hide();
        m_SearchMethod = method;
        // retreive all CIS for the searched molecule
        QHashWhere where;
        where.insert(DrugsDB::Constants::COMPO_MOL_NAME, "LIKE '__replaceit__%'");
        m_filterModel = drugsBase()->selectDistinct(DrugsDB::Constants::Table_COMPO, DrugsDB::Constants::COMPO_UID , where);
//        m_filterModel += QString("SELECT DISTINCT %1 FROM %2 WHERE %3")
//                         .arg(mfDrugsTables::getField(mfDrugsTables::COMPO_UID))
//                         .arg(mfDrugsTables::getTable(mfDrugsTables::Table_COMPO))
//                         .arg(mfDrugsTables::getWhereClause(map2));
        // retreive drug name
        where.clear();
        where.insert(DrugsDB::Constants::DRUGS_MARKET, "=1");
        where.insert(DrugsDB::Constants::DRUGS_UID , QString("IN (%1) ").arg(m_filterModel));
        m_filterModel = drugsBase()->getWhereClause(DrugsDB::Constants::Table_DRUGS, where);
    }
    else if (method == Constants::SearchInn) {
#ifdef FREEDIAMS
        mainWindow()->setWindowTitle(m_WinTitle + " - " + tkTr(Constants::SEARCHINN_TEXT));
#endif
        m_filterModel = "";
        // show inn model and view
        InnView->show();
        // refresh inn select ?
        m_SearchMethod = method;
        // retreive all CIS for the searched inn
        QHashWhere where;
        where.insert(DrugsDB::Constants::COMPO_MOL_CODE , "IN (__replaceit__)");
        m_filterModel += drugsBase()->selectDistinct(DrugsDB::Constants::Table_COMPO, DrugsDB::Constants::COMPO_UID, where);
//        m_filterModel += QString("SELECT DISTINCT %1 FROM %2 WHERE %3")
//                         .arg(mfDrugsTables::getField(mfDrugsTables::COMPO_UID))
//                         .arg(mfDrugsTables::getTable(mfDrugsTables::Table_COMPO))
//                         .arg(mfDrugsTables::getWhereClause(map2));
        // retreive drug name
        where.clear();
        where.insert(DrugsDB::Constants::DRUGS_MARKET, "=1");
        where.insert(DrugsDB::Constants::DRUGS_UID , QString("IN (%1) ").arg(m_filterModel));
        m_filterModel = drugsBase()->getWhereClause(DrugsDB::Constants::Table_DRUGS, where);
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
    if (searchLine->searchText().isEmpty()) {
        if (WarnSearchFilter)
            qWarning() << "No search filter";
        m_DrugsModel->setFilter("");
        return;
    }
    QString tmp = m_filterModel;
    QString search = searchLine->searchText().replace("*", "%");
    if (m_SearchMethod != Constants::SearchInn) {
        m_DrugsModel->setFilter(tmp.replace("__replaceit__", search));
        if (WarnSearchFilter)
            qWarning() << "Search filter" << tmp;
    } else {
        // Search By INN
        // refresh inn view and model
//        QHashWhere where;
//        where.insert(DrugsDB::Constants::IAM_DENOMINATION , QString("LIKE '%1%' ").arg(search));
//        m_InnModel->setFilter(drugsBase()->getIamWhereClause(DrugsDB::Constants::Table_IAM_DENOMINATION, where));
//        // retreive code_subst associated with searched text
//        QList<int> codes = drugsBase()->getLinkedMoleculeCodes(search);
//        QString list = "";
//        foreach(int i, codes)
//            list += QString::number(i) + ", " ;
//        list.chop(2);
//        m_DrugsModel->setFilter(tmp.replace("__replaceit__", list));

        QHashWhere where;
        QString l = QLocale().name().left(2);
        if (l == "fr")
            where.insert(DrugsDB::Constants::ATC_FR , QString("LIKE '%1%' ").arg(search));
        else if (l == "de")
            where.insert(DrugsDB::Constants::ATC_DE , QString("LIKE '%1%' ").arg(search));
        else
            where.insert(DrugsDB::Constants::ATC_EN , QString("LIKE '%1%' ").arg(search));

//        m_InnModel->setFilter(drugsBase()->getIamWhereClause(DrugsDB::Constants::Table_ATC, where));

        // retreive molecule_codes associated with searched text
        QList<int> codes = drugsBase()->getLinkedMoleculeCodes(search);
        QString list = "";
        foreach(int i, codes)
            list += QString::number(i) + ", " ;
        list.chop(2);
        m_DrugsModel->setFilter(tmp.replace("__replaceit__", list));

        if (WarnSearchFilter)
            qWarning() << "Search filter" << tmp;
    }
}

void DrugSelector::on_InnView_clicked(const QModelIndex &index)
{
    if (m_SearchMethod != Constants::SearchInn)
        return;

    if (!index.isValid())
        return;

    QString inn = m_InnModel->index(index.row(), DrugsDB::AtcTreeModel::ATC_Code, index.parent()).data().toString();
    // retreive molecule_codes associated with searched text
    QString tmp = m_filterModel;
    QList<int> codes = drugsBase()->getAllMoleculeCodeWithAtcStartingWith(inn);
    QString list = "";
    foreach(int i, codes)
        list += QString::number(i) + ", " ;
    list.chop(2);
    if (WarnSearchFilter)
        qWarning() << "Search filter" << tmp.replace("__replaceit__", list);
    m_DrugsModel->setFilter(tmp.replace("__replaceit__", list));
}

void DrugSelector::on_drugsView_doubleClicked(const QModelIndex &index)
{
    // retreive CIS of drug and emit signal
    if (!index.isValid())
        return;

    int selectedCIS = m_DrugsModel->index(index.row(), DrugsDB::Constants::DRUGS_UID).data().toInt();

    // store drug into history
    QStringList hist = settings()->value(Constants::S_DRUGHISTORY).toStringList();
    hist.removeAll(index.data().toString());
    if (hist.count() && (hist.count() == settings()->value(Constants::S_HISTORYSIZE).toInt()))
        hist.removeFirst();
    hist << index.data().toString();
    settings()->setValue(Constants::S_DRUGHISTORY, hist);

    // refresh drug history
    createDrugsHistoryActions();

    emit drugSelected(selectedCIS);
    emit drugSelected(index);
}

/** \brief User asked to add a textual prescription. This member creates a dialog and send the text to the model. */
void DrugSelector::on_textButton_clicked()
{
    TextualPrescriptionDialog dlg(this);
    int r = dlg.exec();
    if (r==QDialog::Accepted) {
        int row = drugModel()->addTextualPrescription(dlg.drugLabel(), dlg.drugNote());
        drugModel()->setData(drugModel()->index(row, DrugsDB::Constants::Prescription::IsALD), dlg.isALD());
    }
}
