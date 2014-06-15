/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "drugswidgetmanager.h"

#include <drugsplugin/constants.h>
#include <drugsplugin/drugswidget/drugscentralwidget.h>
#include <drugsplugin/drugswidget/prescriptionviewer.h>
#include <drugsplugin/drugswidget/drugselector.h>

#include <drugsbaseplugin/idrugengine.h>
#include <drugsbaseplugin/drugbasecore.h>
#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/drugsbaseinfo.h>

#include <coreplugin/constants_icons.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_database.h>
#include <translationutils/trans_menu.h>
#include <translationutils/trans_drugs.h>
#include <translationutils/trans_editor.h>
#include <translationutils/trans_datetime.h>
#include <extensionsystem/pluginmanager.h>

#include <QDockWidget>
#include <QGridLayout>
#include <QTreeView>
#include <QStandardItemModel>
#include <QTextDocument>

using namespace DrugsWidget;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}
static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}
static inline DrugsDB::DrugsBase &drugsBase() {return DrugsDB::DrugBaseCore::instance().drugsBase();}
static inline Core::ITheme *theme() { return Core::ICore::instance()->theme(); }

// Register an existing Core action
//static QAction *registerAction(const QString &id, const Core::Context &ctx, QObject *parent)
//{
//    QAction *a = new QAction(parent);
//    Core::Command *cmd = Core::ICore::instance()->actionManager()->registerAction(a, Core::Id(id), ctx);
//    Q_UNUSED(cmd);
//    return a;
//}

// Create an action
static inline QAction *createAction(QObject *parent, const QString &name, const QString &icon,
                                    const QString &actionName,
                                    const Core::Context &context,
                                    const QString &trans, const QString &transContext,
                                    Core::Command *cmd,
                                    Core::ActionContainer *menu,
                                    const QString &group,
                                    QKeySequence::StandardKey key = QKeySequence::UnknownKey,
                                    bool checkable = false)
{
    QAction *a = new QAction(parent);
    a->setObjectName(name);
    if (!icon.isEmpty())
        a->setIcon(theme()->icon(icon));
    if (checkable) {
        a->setCheckable(true);
        a->setChecked(false);
    }
    cmd = actionManager()->registerAction(a, Core::Id(actionName), context);
    if (!transContext.isEmpty())
        cmd->setTranslations(trans, trans, transContext);
    else
        cmd->setTranslations(trans, trans); // use the Trans::Constants tr context (automatic)
    if (key != QKeySequence::UnknownKey)
        cmd->setDefaultKeySequence(key);
    if (menu)
        menu->addAction(cmd, Core::Id(group));
    return a;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////      MANAGER      ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
DrugsWidgetManager *DrugsWidgetManager::m_Instance = 0;

DrugsWidgetManager *DrugsWidgetManager::instance()
{
    if (!m_Instance)
        m_Instance = new DrugsWidgetManager(qApp);
    return m_Instance;
}

// TODO: remove the singleton?
DrugsWidgetManager::DrugsWidgetManager(QObject *parent) :
    DrugsActionHandler(parent)
{
    connect(Core::ICore::instance()->contextManager(), SIGNAL(contextChanged(Core::IContext*,Core::Context)),
            this, SLOT(updateContext(Core::IContext*,Core::Context)));
    setObjectName("DrugsWidgetManager");
}

void DrugsWidgetManager::updateContext(Core::IContext *object, const Core::Context &additionalContexts)
{
    Q_UNUSED(additionalContexts)
    //    qWarning() << "DrugsManager::updateContext(Core::IContext *object)";
    //    if (object)
    //        qWarning() << "DrugsManager::updateContext(Core::IContext *object)" << object->widget();

    DrugsCentralWidget *view = 0;
    do {
        if (!object) {
            if (!m_CurrentView)
                return;

            //            m_CurrentView = 0;  // keep trace of the last active view (we need it in dialogs)
            break;
        }
        view = qobject_cast<DrugsCentralWidget *>(object->widget());
        if (!view) {
            if (!m_CurrentView)
                return;

            //            m_CurrentView = 0;   // keep trace of the last active view (we need it in dialogs)
            break;
        }

        if (view == m_CurrentView) {
            return;
        }

    } while (false);
    if (view) {
        DrugsActionHandler::setCurrentView(view);
    }
}

DrugsCentralWidget *DrugsWidgetManager::currentView() const
{
    return DrugsActionHandler::m_CurrentView;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////  ACTION HANDLER   ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
DrugsActionHandler::DrugsActionHandler(QObject *parent) :
    QObject(parent),
    aSavePrescription(0),
    aSaveAsPrescription(0),
    aToggleDrugSelector(0),
    aAddRow(0),
    aRemoveRow(0),
    aDown(0),
    aUp(0),
    aSort(0),
    aEdit(0),
    aClear(0),
    aViewInteractions(0),
    aSearch(0),
    gSearchMethod(0),
    aSearchCommercial(0),
    aSearchMolecules(0),
    aSearchInn(0),
    aPrintPrescription(0),
    aPrintPreview(0),
    aToggleTestingDrugs(0),
    aChangeDuration(0),
    aToTemplate(0),
    aDrugsDatabaseInformation(0),
    aDosagesDatabaseInformation(0),
    gModes(0),
    aPrescriberMode(0),
    aSelectOnlyMode(0),
    aOpenDosageDialog(0),
    aOpenPrescriptionSentencePreferences(0),
    aResetPrescriptionSentenceToDefault(0),
    aToggleDrugPrecautions(0),
    aCopyPrescriptionItem(0),
    m_CurrentView(0),
    m_PrecautionDock(0)
{
    setObjectName("DrugsActionHandler");
    QAction *a = 0;
    Core::Command *cmd = 0;
    Core::Context ctx(DrugsWidget::Constants::C_DRUGS_PLUGINS);
    using namespace DrugsWidget::Constants;
    using namespace DrugsDB::Constants;

    Core::ActionContainer *menu = actionManager()->actionContainer(Core::Id(M_PLUGINS_DRUGS));
    if (!menu) {
        menu = actionManager()->createMenu(M_PLUGINS_DRUGS);
        menu->appendGroup(Core::Id(G_PLUGINS_VIEWS));
        menu->appendGroup(Core::Id(G_PLUGINS_MODES));
        menu->appendGroup(Core::Id(G_PLUGINS_SEARCH));
        menu->appendGroup(Core::Id(G_PLUGINS_DRUGS));
        menu->appendGroup(Core::Id(G_PLUGINS_INTERACTIONS));
        menu->setTranslations(DRUGSMENU_TEXT);
#ifdef FREEDIAMS
        actionManager()->actionContainer(Core::Id(Core::Constants::MENUBAR))->addMenu(menu, Core::Id(Core::Constants::G_PLUGINS));
#else
        actionManager()->actionContainer(Core::Id(Core::Constants::M_PLUGINS))->addMenu(menu, Core::Id(Core::Constants::G_PLUGINS_DRUGS));
#endif
    }
    Q_ASSERT(menu);

    // Create local actions

#ifndef FREEDIAMS
    aSavePrescription = createAction(this, "aSavePrescription", Core::Constants::ICONSAVE,
                                     A_SAVE_PRESCRIPTION,
                                     ctx,
                                     Trans::Constants::FILESAVE_TEXT, "",
                                     cmd,
                                     menu, G_PLUGINS_DRUGS,
                                     QKeySequence::UnknownKey, false);
    connect(aSavePrescription, SIGNAL(triggered()), this, SLOT(savePrescription()));

    aSaveAsPrescription = createAction(this, "aSaveAsPrescription", Core::Constants::ICONSAVEAS,
                                       A_SAVEAS_PRESCRIPTION,
                                       ctx,
                                       Trans::Constants::FILESAVEAS_TEXT, "",
                                       cmd,
                                       menu, G_PLUGINS_DRUGS,
                                       QKeySequence::UnknownKey, false);
    connect(aSaveAsPrescription, SIGNAL(triggered()), this, SLOT(saveAsPrescription()));
#endif

    aToggleDrugSelector = createAction(this, "aToggleDrugSelector", I_TOGGLEDRUGSELECTOR,
                                       A_TOGGLE_DRUGSELECTOR,
                                       ctx,
                                       TOGGLEDRUGSELECTOR_TEXT, DRUGCONSTANTS_TR_CONTEXT,
                                       cmd,
                                       menu, G_PLUGINS_VIEWS,
                                       QKeySequence::UnknownKey, false);
    connect(aToggleDrugSelector, SIGNAL(triggered()), this, SLOT(toggleDrugSelector()));

    aClear = createAction(this, "aClear", Core::Constants::ICONCLEAR,
                          A_CLEAR_PRESCRIPTION,
                          ctx,
                          CLEARPRESCRIPTION_TEXT, DRUGCONSTANTS_TR_CONTEXT,
                          cmd,
                          menu, G_PLUGINS_DRUGS,
                          QKeySequence::UnknownKey, false);
    connect(aClear, SIGNAL(triggered()), this, SLOT(clear()));

    aRemoveRow = createAction(this, "aRemoveRow", Core::Constants::ICONREMOVE,
                              Core::Constants::A_LIST_REMOVE,
                              ctx,
                              Trans::Constants::LISTREMOVE_TEXT, "",
                              cmd,
                              menu, G_PLUGINS_DRUGS,
                              QKeySequence::UnknownKey, false);
    connect(aRemoveRow, SIGNAL(triggered()), this, SLOT(removeItem()));

    aDown = createAction(this, "aDown", Core::Constants::ICONMOVEDOWN,
                         Core::Constants::A_LIST_MOVEDOWN,
                         ctx,
                         Trans::Constants::LISTMOVEDOWN_TEXT, "",
                         cmd,
                         menu, G_PLUGINS_DRUGS,
                         QKeySequence::UnknownKey, false);
    connect(aDown, SIGNAL(triggered()), this, SLOT(moveDown()));

    aUp = createAction(this, "aUp", Core::Constants::ICONMOVEUP,
                       Core::Constants::A_LIST_MOVEUP,
                       ctx,
                       Trans::Constants::LISTMOVEUP_TEXT, "",
                       cmd,
                       menu, G_PLUGINS_DRUGS,
                       QKeySequence::UnknownKey, false);
    connect(aUp, SIGNAL(triggered()), this, SLOT(moveUp()));

    aSort = createAction(this, "aSort", Core::Constants::ICONSORT,
                         Core::Constants::A_LIST_SORT,
                         ctx,
                         Trans::Constants::LISTSORT_TEXT, "",
                         cmd,
                         menu, G_PLUGINS_DRUGS,
                         QKeySequence::UnknownKey, false);
    connect(aSort, SIGNAL(triggered()), this, SLOT(sortDrugs()));

    aViewInteractions = createAction(this, "aViewInteractions", I_DRUGENGINE,
                                     A_VIEW_INTERACTIONS,
                                     ctx,
                                     Trans::Constants::VIEWINTERACTIONS_TEXT, "",
                                     cmd,
                                     menu, G_PLUGINS_INTERACTIONS,
                                     QKeySequence::UnknownKey, false);
    connect(aViewInteractions, SIGNAL(triggered()), this, SLOT(viewInteractions()));

    a = aSearch = new QAction(this);
#ifdef FREEDIAMS
    Core::Context globalcontext(Core::Constants::C_GLOBAL);
    cmd = actionManager()->registerAction(a, Core::Id(Core::Constants::A_EDIT_SEARCH), globalcontext);
#else
    cmd = actionManager()->registerAction(a, Core::Id(Core::Constants::A_EDIT_SEARCH), ctx);
#endif
    connect(aSearch, SIGNAL(triggered()), this, SLOT(searchTriggered()));

    aToggleTestingDrugs = createAction(this, "aToggleTestingDrugs", I_TOGGLETESTINGDRUGS,
                                       A_TOGGLE_TESTINGDRUGS,
                                       ctx,
                                       TOGGLETESTINGDRUGS_TEXT, DRUGCONSTANTS_TR_CONTEXT,
                                       cmd,
                                       menu, G_PLUGINS_VIEWS,
                                       QKeySequence::UnknownKey, true);
    connect(aToggleTestingDrugs, SIGNAL(triggered()), this, SLOT(toggleTestingDrugs()));

    // Search method menu
    Core::ActionContainer *searchmenu = actionManager()->actionContainer(Core::Id(M_PLUGINS_SEARCH));
    if (!searchmenu) {
        searchmenu = actionManager()->createMenu(M_PLUGINS_SEARCH);
        searchmenu->appendGroup(Core::Id(G_PLUGINS_SEARCH));
        searchmenu->setTranslations(Trans::Constants::SEARCHMENU_TEXT);
        menu->addMenu(searchmenu, Core::Id(G_PLUGINS_SEARCH));
    }
    Q_ASSERT(searchmenu);

    gSearchMethod = new QActionGroup(this);
    int searchMethod = settings()->value(S_SEARCHMETHOD).toInt();
    aSearchCommercial = createAction(this, "aSearchCommercial", I_SEARCHCOMMERCIAL,
                                     A_SEARCH_COMMERCIAL,
                                     ctx,
                                     SEARCHCOMMERCIAL_TEXT, DRUGCONSTANTS_TR_CONTEXT,
                                     cmd,
                                     searchmenu, G_PLUGINS_SEARCH,
                                     QKeySequence::UnknownKey, true);
    aSearchCommercial->setChecked(searchMethod == SearchCommercial);
    gSearchMethod->addAction(aSearchCommercial);

    aSearchMolecules = createAction(this, "aSearchMolecules", I_SEARCHMOLS,
                                    A_SEARCH_MOLECULES,
                                    ctx,
                                    SEARCHMOLECULES_TEXT, DRUGCONSTANTS_TR_CONTEXT,
                                    cmd,
                                    searchmenu, G_PLUGINS_SEARCH,
                                    QKeySequence::UnknownKey, true);
    aSearchMolecules->setChecked(searchMethod == SearchMolecules);
    gSearchMethod->addAction(aSearchMolecules);

    aSearchInn = createAction(this, "aSearchInn", I_SEARCHINN,
                              A_SEARCH_INN,
                              ctx,
                              SEARCHINN_TEXT, DRUGCONSTANTS_TR_CONTEXT,
                              cmd,
                              searchmenu, G_PLUGINS_SEARCH,
                              QKeySequence::UnknownKey, true);
    aSearchInn->setChecked(searchMethod == SearchInn);
    gSearchMethod->addAction(aSearchInn);
    connect(gSearchMethod,SIGNAL(triggered(QAction*)),this,SLOT(searchActionChanged(QAction*)));

    Core::ActionContainer *tmenu = actionManager()->actionContainer(Core::Id(Core::Constants::M_TEMPLATES));
    aToTemplate = createAction(this, "aToTemplate", I_SEARCHINN,
                               Core::Constants::A_TEMPLATE_CREATE,
                               ctx,
                               Trans::Constants::CREATETEMPLATE_TEXT, "",
                               cmd,
                               tmenu, Core::Constants::G_TEMPLATES_NEW,
                               QKeySequence::UnknownKey, true);
    connect(aToTemplate, SIGNAL(triggered()), this, SLOT(createTemplate()));

    Core::ActionContainer *fmenu = actionManager()->actionContainer(Core::Id(Core::Constants::M_FILE));
//#ifdef FREEDIAMS
    aPrintPrescription = createAction(this, "aPrintPrescription", Core::Constants::ICONPRINT,
                                      A_PRINT_PRESCRIPTION,
                                      ctx,
                                      PRINTPRESCRIPTION_TEXT, DRUGCONSTANTS_TR_CONTEXT,
                                      cmd,
                                      fmenu, Core::Constants::G_FILE_PRINT,
                                      QKeySequence::Print,
                                      false);
//#else
//    aPrintPrescription = createAction(this, "aPrintPrescription", Core::Constants::ICONPRINT,
//                                      A_PRINT_PRESCRIPTION,
//                                      ctx,
//                                      PRINTPRESCRIPTION_TEXT, DRUGCONSTANTS_TR_CONTEXT,
//                                      cmd,
//                                      fmenu, Core::Constants::G_FILE_PRINT,
//                                      QKeySequence(tkTr(Trans::Constants::K_PRINT_PRESCRIPTION)),
//                                      false);
//#endif
    connect(aPrintPrescription,SIGNAL(triggered()), this, SLOT(printPrescription()));

    aPrintPreview = createAction(this, "aPrintPreview", Core::Constants::ICONPRINTPREVIEW,
                                 Core::Constants::A_FILE_PRINTPREVIEW,
                                 ctx,
                                 Trans::Constants::PRINTPREVIEW_TEXT, "",
                                 cmd,
                                 fmenu, Core::Constants::G_FILE_PRINT,
                                 QKeySequence::UnknownKey, false);
    connect(aPrintPreview,SIGNAL(triggered()), this, SLOT(printPreview()));

    aChangeDuration = createAction(this, "aChangeDuration", Core::Constants::ICONDATE,
                                   A_CHANGE_DURATION,
                                   ctx,
                                   Trans::Constants::DURATION, "",
                                   cmd,
                                   0, "",
                                   QKeySequence::UnknownKey, false);
    connect(aChangeDuration,SIGNAL(triggered()),this,SLOT(changeDuration()));

    // Databases information
    Core::ActionContainer *hmenu = actionManager()->actionContainer(Core::Id(Core::Constants::M_HELP_DATABASES));
    aDrugsDatabaseInformation = createAction(this, "aDrugsDatabaseInformation", Core::Constants::ICONHELP,
                                             A_DRUGSDATABASE_INFORMATION,
                                             Core::Context(Core::Constants::C_GLOBAL),
                                             Trans::Constants::DRUGS_DATABASE_INFORMATION, "",
                                             cmd,
                                             hmenu, Core::Constants::G_HELP_DATABASES,
                                             QKeySequence::UnknownKey, false);
    connect(aDrugsDatabaseInformation,SIGNAL(triggered()), this, SLOT(showDrugsDatabaseInformation()));

    aDosagesDatabaseInformation = createAction(this, "aDosagesDatabaseInformation", Core::Constants::ICONHELP,
                                               A_DOSAGESDATABASE_INFORMATION,
                                               Core::Context(Core::Constants::C_GLOBAL),
                                               Trans::Constants::DOSAGES_DATABASE_INFORMATION, "",
                                               cmd,
                                               hmenu, Core::Constants::G_HELP_DATABASES,
                                               QKeySequence::UnknownKey, false);
    connect(aDosagesDatabaseInformation,SIGNAL(triggered()), this, SLOT(showDosagesDatabaseInformation()));

    // Mode menu
    Core::ActionContainer *modemenu = actionManager()->actionContainer(Core::Id(M_PLUGINS_MODES));
    if (!modemenu) {
        modemenu = actionManager()->createMenu(M_PLUGINS_MODES);
        modemenu->appendGroup(Core::Id(G_PLUGINS_MODES));
        modemenu->setTranslations(MODEMENU_TEXT, DRUGCONSTANTS_TR_CONTEXT);
        menu->addMenu(modemenu, Core::Id(G_PLUGINS_MODES));
    }
    Q_ASSERT(modemenu);

    gModes = new QActionGroup(this);
    aPrescriberMode = createAction(this, "aPrescriberMode", Core::Constants::ICONHELP,
                                   A_PRESCRIBERMODE,
                                   ctx,
                                   PRESCRIBERMODE_TEXT, DRUGCONSTANTS_TR_CONTEXT,
                                   cmd,
                                   modemenu, G_PLUGINS_MODES,
                                   QKeySequence::UnknownKey, true);
    aPrescriberMode->setChecked(true);
    gModes->addAction(aPrescriberMode);

    aSelectOnlyMode = createAction(this, "aSelectOnlyMode", Core::Constants::ICONHELP,
                                   A_SELECTONLYMODE,
                                   ctx,
                                   SELECTONLYMODE_TEXT, DRUGCONSTANTS_TR_CONTEXT,
                                   cmd,
                                   modemenu, G_PLUGINS_MODES,
                                   QKeySequence::UnknownKey, true);
    aSelectOnlyMode->setChecked(false);
    gModes->addAction(aSelectOnlyMode);
    connect(gModes,SIGNAL(triggered(QAction*)),this,SLOT(modeActionChanged(QAction*)));

    aOpenDosageDialog = createAction(this, "aOpenDosageDialog", Core::Constants::ICONDATE,
                                     A_OPENDOSAGEDIALOG,
                                     ctx,
                                     OPENDOSAGEDIALOG_TEXT, DRUGCONSTANTS_TR_CONTEXT,
                                     cmd,
                                     0, "",
                                     QKeySequence::UnknownKey, false);
    connect(aOpenDosageDialog,SIGNAL(triggered()),this,SLOT(openDosageDialog()));

    aOpenPrescriptionSentencePreferences = createAction(this, "aOpenPrescriptionSentencePreferences", Core::Constants::ICONDATE,
                                                        A_OPENDOSAGEPREFERENCES,
                                                        ctx,
                                                        OPENDOSAGEPREFERENCES_TEXT, DRUGCONSTANTS_TR_CONTEXT,
                                                        cmd,
                                                        0, "",
                                                        QKeySequence::UnknownKey, false);
    connect(aOpenPrescriptionSentencePreferences,SIGNAL(triggered()),this,SLOT(openProtocolPreferencesDialog()));

    aResetPrescriptionSentenceToDefault = createAction(this, "aOpenPrescriptionSentencePreferences", Core::Constants::ICONDATE,
                                                       A_RESETPRESCRIPTIONSENTENCE_TODEFAULT,
                                                       ctx,
                                                       RESETPRESCRIPTIONSENTENCETODEFAULT_TEXT, DRUGCONSTANTS_TR_CONTEXT,
                                                       cmd,
                                                       menu, G_PLUGINS_DRUGS,
                                                       QKeySequence::UnknownKey, false);
    connect(aResetPrescriptionSentenceToDefault,SIGNAL(triggered()),this,SLOT(resetPrescriptionSentenceToDefault()));

    aToggleDrugPrecautions = createAction(this, "aToggleDrugPrecautions", I_ALLERGYENGINE,
                                        A_TOGGLEDRUGPRECAUTIONS,
                                        Core::Context(Core::Constants::C_GLOBAL),
                                        TOGGLEDRUGPRECAUTIONS_TEXT, DRUGCONSTANTS_TR_CONTEXT,
                                        cmd,
                                        menu, G_PLUGINS_VIEWS,
                                        QKeySequence::UnknownKey, false);
    connect(aToggleDrugPrecautions, SIGNAL(triggered()), this, SLOT(toggleDrugPrecautions()));

    aCopyPrescriptionItem = createAction(this, "aCopyPrescriptionItem", Core::Constants::ICONCOPY,
                                         A_COPYPRESCRIPTIONITEM,
                                         ctx,
                                         COPYPRESCRIPTIONITEM_TEXT, DRUGCONSTANTS_TR_CONTEXT,
                                         cmd,
                                         menu, G_PLUGINS_VIEWS,
                                         QKeySequence::UnknownKey, false);
    connect(aCopyPrescriptionItem, SIGNAL(triggered()), this, SLOT(copyPrescriptionItem()));

    connect(&drugsBase(), SIGNAL(drugsBaseHasChanged()), this, SLOT(onDrugsBaseChanged()));
    onDrugsBaseChanged();
    contextManager()->updateContext();
    actionManager()->retranslateMenusAndActions();
}

void DrugsActionHandler::onDrugsBaseChanged()
{
    aSearchInn->setEnabled(drugsBase().actualDatabaseInformation() && drugsBase().actualDatabaseInformation()->atcCompatible);
}

void DrugsActionHandler::searchActionChanged(QAction *a)
{
    if (!m_CurrentView)
        return;
    if (a==aSearchCommercial)
        m_CurrentView->setCurrentSearchMethod(DrugsWidget::Constants::SearchCommercial);
    else if (a==aSearchMolecules)
        m_CurrentView->setCurrentSearchMethod(DrugsWidget::Constants::SearchMolecules);
    else if (a==aSearchInn)
        m_CurrentView->setCurrentSearchMethod(DrugsWidget::Constants::SearchInn);
}

void DrugsActionHandler::setCurrentView(DrugsCentralWidget *view)
{
    Q_ASSERT(view);
    if (!view) { // this should never be the case
        Utils::Log::addError(this, "setCurrentView: no view", __FILE__, __LINE__);
        return;
    }
    //    qWarning() << "DrugsActionHandler::setCurrentView(DrugsCentralWidget *view)";

    // disconnect old view
    if (m_CurrentView) {
        if (view == m_CurrentView.data())
            return;
        m_CurrentView->disconnect();
        disconnect(m_CurrentView->prescriptionListView()->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                   this, SLOT(listViewItemChanged()));
        disconnect(m_CurrentView->currentDrugsModel(), SIGNAL(numberOfRowsChanged()),
                   this, SLOT(drugsModelChanged()));
        m_CurrentView->drugSelector()->disconnectFilter();
    }
    m_CurrentView = view;

    DrugsDB::DrugsModel::setActiveModel(view->currentDrugsModel());
    // reconnect some actions
    m_CurrentView->createConnections();
    connect(m_CurrentView->prescriptionListView()->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this, SLOT(listViewItemChanged()));
    connect(m_CurrentView->currentDrugsModel(), SIGNAL(numberOfRowsChanged()),
            this, SLOT(drugsModelChanged()));
    m_CurrentView->drugSelector()->connectFilter();
    updateActions();
}

void DrugsActionHandler::listViewItemChanged()
{
    const bool valid = m_CurrentView && m_CurrentView->prescriptionListView()->currentIndex().isValid();

    aUp->setEnabled(valid && canMoveUp());
    aDown->setEnabled(valid && canMoveDown());
    aRemoveRow->setEnabled(valid);
    aSort->setEnabled(valid);

}

void DrugsActionHandler::drugsModelChanged()
{
    if (!m_CurrentView)
        return;
    aViewInteractions->setEnabled(m_CurrentView->currentDrugsModel()->prescriptionHasInteractions());
}

void DrugsActionHandler::updateActions()
{
    listViewItemChanged();
    drugsModelChanged();
}

void DrugsActionHandler::savePrescription()
{
    if (m_CurrentView) {
        m_CurrentView->prescriptionView()->savePrescription();
    }
}

void DrugsActionHandler::saveAsPrescription()
{
    if (m_CurrentView) {
        m_CurrentView->prescriptionView()->saveAsPrescription();
    }
}

void DrugsActionHandler::toggleDrugSelector()
{
    if (m_CurrentView && m_CurrentView->drugSelector()) {
        bool setToVisible = !m_CurrentView->drugSelector()->isVisible();
        m_CurrentView->setMinimumHeight(setToVisible ? 600 : 200);
        m_CurrentView->drugSelector()->setVisible(setToVisible);
        aToggleDrugSelector->setChecked(setToVisible);
    }
}

bool DrugsActionHandler::canMoveUp() const
{
    if (!m_CurrentView)
        return false;
    const QModelIndex idx = m_CurrentView->prescriptionListView()->currentIndex();
    if (!idx.isValid())
        return false;
    if (idx.row() >= 1)
        return true;
    return false;
}

bool DrugsActionHandler::canMoveDown() const
{
    if (!m_CurrentView)
        return false;
    QModelIndex idx = m_CurrentView->prescriptionListView()->currentIndex();
    if (!idx.isValid())
        return false;
    if (idx.row() < (m_CurrentView->prescriptionListView()->model()->rowCount()-1))
        return true;
    return false;
}

void DrugsActionHandler::moveUp()
{
    if (m_CurrentView) {
        Q_ASSERT(m_CurrentView->prescriptionView());
        m_CurrentView->prescriptionView()->moveUp();
    }
}

void DrugsActionHandler::moveDown()
{
    if (m_CurrentView) {
        Q_ASSERT(m_CurrentView->prescriptionView());
        m_CurrentView->prescriptionView()->moveDown();
    }
}

void DrugsActionHandler::sortDrugs()
{
    if (m_CurrentView) {
        Q_ASSERT(m_CurrentView->currentDrugsModel());
        m_CurrentView->currentDrugsModel()->sort(0);
    }
}

void DrugsActionHandler::removeItem()
{
    if (m_CurrentView) {
        Q_ASSERT(m_CurrentView->prescriptionView());
        m_CurrentView->prescriptionView()->removeTriggered();
    }
}

void DrugsActionHandler::clear()
{
    if (m_CurrentView) {
        Q_ASSERT(m_CurrentView->prescriptionView());
        m_CurrentView->prescriptionView()->clear();
    }
}

void DrugsActionHandler::viewInteractions()
{
    if (m_CurrentView) {
        Q_ASSERT(m_CurrentView->prescriptionView());
        m_CurrentView->prescriptionView()->viewInteractions();
    }
}

void DrugsActionHandler::searchTriggered()
{
    if (m_CurrentView) {
        m_CurrentView->drugSelector()->onSearchShortCutTriggered();
    }
}

void DrugsActionHandler::printPrescription()
{
    if (m_CurrentView)
        m_CurrentView->printPrescription();
}

void DrugsActionHandler::toggleTestingDrugs()
{
    if (m_CurrentView) {
        Q_ASSERT(m_CurrentView->currentDrugsModel());
        m_CurrentView->currentDrugsModel()->showTestingDrugs(aToggleTestingDrugs->isChecked());
    }
}

void DrugsActionHandler::changeDuration()
{
    if (m_CurrentView) {
        Q_ASSERT(m_CurrentView->prescriptionView());
        m_CurrentView->prescriptionView()->changeDuration();
    }
}

void DrugsActionHandler::createTemplate()
{
    if (m_CurrentView)
        m_CurrentView->createTemplate();
}

void DrugsActionHandler::printPreview()
{
    if (m_CurrentView) {
        m_CurrentView->printPreview();
    }
}

void DrugsActionHandler::showDrugsDatabaseInformation()
{
    if (m_CurrentView) {
        m_CurrentView->showDrugsDatabaseInformation();
    }
}

void DrugsActionHandler::showDosagesDatabaseInformation()
{
    if (m_CurrentView) {
        m_CurrentView->showDosagesDatabaseInformation();
    }
}

void DrugsActionHandler::setEditMode(const Modes mode)
{
    // no change
    if (mode==SelectOnly && m_SelectionOnlyMode)
        return;
    if (mode==Prescriber && !m_SelectionOnlyMode)
        return;

    // ask user
    if (DrugsDB::DrugsModel::activeModel()->rowCount() > 0) {
        bool yes;
        yes = Utils::yesNoMessageBox(tr("Prescription is not empty. Clear it?"),
                                     tr("You select another editing mode than the actual one. "
                                        "Changing of mode during edition may cause prescription lose.\n"
                                        "Do you really want to change the editing mode?"));
        if (yes) {
            DrugsDB::DrugsModel::activeModel()->clearDrugsList();
        } else {
            return;
        }
    }

    // change the mode
    if (mode == SelectOnly) {
        m_SelectionOnlyMode = true;
        DrugsDB::DrugsModel::activeModel()->setSelectionOnlyMode(true);
        this->aSelectOnlyMode->setChecked(true);
        this->aPrescriberMode->setChecked(false);
    } else {
        m_SelectionOnlyMode = false;
        DrugsDB::DrugsModel::activeModel()->setSelectionOnlyMode(false);
        this->aSelectOnlyMode->setChecked(false);
        this->aPrescriberMode->setChecked(true);
    }
}

DrugsActionHandler::Modes DrugsActionHandler::editMode() const
{
    if (m_SelectionOnlyMode)
        return SelectOnly;
    return Prescriber;
}

void DrugsActionHandler::modeActionChanged(QAction *a)
{
    if (!m_CurrentView)
        return;
    if (a==aPrescriberMode)
        setEditMode(Prescriber);
    else
        setEditMode(SelectOnly);
}

void DrugsActionHandler::openDosageDialog()
{
    if (m_CurrentView) {
        Q_ASSERT(m_CurrentView->prescriptionView());
        m_CurrentView->prescriptionView()->showDosageDialog();
    }
}

void DrugsActionHandler::openProtocolPreferencesDialog()
{
    if (m_CurrentView) {
        Q_ASSERT(m_CurrentView->prescriptionView());
        m_CurrentView->prescriptionView()->openProtocolPreferencesDialog();
    }
}

static QString getPrescriptionTokenHtmlFileContent()
{
    QString content;
    if (QFile(settings()->path(Core::ISettings::BundleResourcesPath) + QString(DrugsDB::Constants::S_DEF_PRESCRIPTION_TOKENFILE_1_LANG).arg(QLocale().name().left(2).toLower())).exists()) {
        content = Utils::readTextFile(settings()->path(Core::ISettings::BundleResourcesPath) + QString(DrugsDB::Constants::S_DEF_PRESCRIPTION_TOKENFILE_1_LANG).arg(QLocale().name().left(2).toLower()), Utils::DontWarnUser);
    } else if (QFile(settings()->path(Core::ISettings::BundleResourcesPath) + QString(DrugsDB::Constants::S_DEF_PRESCRIPTION_TOKENFILE_1_LANG).arg(Trans::Constants::ALL_LANGUAGE)).exists()) {
        content = Utils::readTextFile(settings()->path(Core::ISettings::BundleResourcesPath) + QString(DrugsDB::Constants::S_DEF_PRESCRIPTION_TOKENFILE_1_LANG).arg(Trans::Constants::ALL_LANGUAGE), Utils::DontWarnUser);
    } else {
        LOG_ERROR_FOR("DrugPosologicSentencePreferencesWidget", "No token'd prescription file found");
    }
    if (content.contains("<body"))
        content = content.remove("\n");
    return content;
}

/** Reset the posologic sentence to the default one */
void DrugsActionHandler::resetPrescriptionSentenceToDefault()
{
    QString content = getPrescriptionTokenHtmlFileContent();
    QString css = Utils::htmlTakeAllCssContent(content);
    content = Utils::htmlReplaceAccents(content);
    content = Utils::htmlBodyContent(content, false);
    content.prepend(css);
    settings()->setValue(DrugsDB::Constants::S_PRESCRIPTIONFORMATTING_HTML, content);
    QTextDocument doc;
    doc.setHtml(content);
    settings()->setValue(DrugsDB::Constants::S_PRESCRIPTIONFORMATTING_PLAIN, doc.toPlainText());
    DrugsDB::DrugsModel::activeModel()->resetModel();
}

void DrugsActionHandler::toggleDrugPrecautions()
{
    DrugsDB::IDrugAllergyEngine *engine = pluginManager()->getObject<DrugsDB::IDrugAllergyEngine>();
    if (!engine) {
        LOG_ERROR("No allergy engine");
        return;
    }
    if (!m_PrecautionDock) {
        m_PrecautionDock = new QDockWidget(QCoreApplication::translate(Constants::DRUGCONSTANTS_TR_CONTEXT, Constants::DRUGPRECAUTIONS_TEXT), Core::ICore::instance()->mainWindow());
        QTreeView *tree = new QTreeView(m_PrecautionDock);
        tree->header()->hide();
        tree->setModel(engine->drugPrecautionModel());
        tree->expandAll();
        tree->setEditTriggers(QTreeView::NoEditTriggers);
        m_PrecautionDock->setWidget(tree);
        m_PrecautionDock->setFloating(false);
        m_PrecautionDock->setAllowedAreas(Qt::RightDockWidgetArea);
        Core::ICore::instance()->mainWindow()->addDockWidget(Qt::RightDockWidgetArea, m_PrecautionDock);
        m_PrecautionDock->show();
    } else {
        m_PrecautionDock->setVisible(!m_PrecautionDock->isVisible());
    }
}

void DrugsActionHandler::copyPrescriptionItem()
{
    if (m_CurrentView) {
        Q_ASSERT(m_CurrentView->prescriptionView());
        m_CurrentView->prescriptionView()->copyPrescriptionItem();
    }
}
