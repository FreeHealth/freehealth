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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
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
#include <coreplugin/uniqueidmanager.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>
#include <extensionsystem/pluginmanager.h>

#include <QDockWidget>
#include <QGridLayout>
#include <QTreeView>
#include <QStandardItemModel>
#include <QTextDocument>

using namespace DrugsWidget::Constants;
using namespace DrugsWidget;
using namespace DrugsWidget::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}
static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}
static inline DrugsDB::DrugsBase &drugsBase() {return DrugsDB::DrugBaseCore::instance().drugsBase();}

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

DrugsWidgetManager::DrugsWidgetManager(QObject *parent) : DrugsActionHandler(parent)
{
    connect(Core::ICore::instance()->contextManager(), SIGNAL(contextChanged(Core::IContext*)),
            this, SLOT(updateContext(Core::IContext*)));
    setObjectName("DrugsWidgetManager");
}

void DrugsWidgetManager::updateContext(Core::IContext *object)
{
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
        aShowDrugPrecautions(0),
        aCopyPrescriptionItem(0),
        m_CurrentView(0),
        m_PrecautionDock(0)
{
    setObjectName("DrugsActionHandler");

    Core::UniqueIDManager *uid = Core::ICore::instance()->uniqueIDManager();
    Core::ITheme *th = Core::ICore::instance()->theme();

    QAction *a = 0;
    Core::Command *cmd = 0;
    QList<int> ctx = QList<int>() << uid->uniqueIdentifier(DrugsWidget::Constants::C_DRUGS_PLUGINS);

    Core::ActionContainer *menu = actionManager()->actionContainer(DrugsWidget::Constants::M_PLUGINS_DRUGS);
    if (!menu) {
        menu = actionManager()->createMenu(DrugsWidget::Constants::M_PLUGINS_DRUGS);
        menu->appendGroup(DrugsWidget::Constants::G_PLUGINS_VIEWS);
        menu->appendGroup(DrugsWidget::Constants::G_PLUGINS_MODES);
        menu->appendGroup(DrugsWidget::Constants::G_PLUGINS_SEARCH);
        menu->appendGroup(DrugsWidget::Constants::G_PLUGINS_DRUGS);
        menu->appendGroup(DrugsWidget::Constants::G_PLUGINS_INTERACTIONS);
        menu->setTranslations(DrugsWidget::Constants::DRUGSMENU_TEXT);
#ifdef FREEDIAMS
        actionManager()->actionContainer(Core::Constants::MENUBAR)->addMenu(menu, Core::Constants::G_PLUGINS);
#else
        actionManager()->actionContainer(Core::Constants::M_PLUGINS)->addMenu(menu, Core::Constants::G_PLUGINS_DRUGS);
#endif
    }
    Q_ASSERT(menu);

    // Create local actions

#ifndef FREEDIAMS
    a = aSavePrescription = new QAction(this);
    a->setObjectName("aSavePrescription");
    a->setIcon(th->icon(Core::Constants::ICONSAVE));
    cmd = actionManager()->registerAction(a, Constants::A_SAVE_PRESCRIPTION, ctx);
    cmd->setTranslations(Trans::Constants::FILESAVE_TEXT);
    menu->addAction(cmd, DrugsWidget::Constants::G_PLUGINS_DRUGS);
    connect(a, SIGNAL(triggered()), this, SLOT(savePrescription()));

    a = aSaveAsPrescription = new QAction(this);
    a->setObjectName("aSaveAsPrescription");
    a->setIcon(th->icon(Core::Constants::ICONSAVEAS));
    cmd = actionManager()->registerAction(a, Constants::A_SAVEAS_PRESCRIPTION, ctx);
    cmd->setTranslations(Trans::Constants::FILESAVEAS_TEXT);
    menu->addAction(cmd, DrugsWidget::Constants::G_PLUGINS_DRUGS);
    connect(a, SIGNAL(triggered()), this, SLOT(saveAsPrescription()));
#endif

    a = aToggleDrugSelector = new QAction(this);
    a->setObjectName("aToggleDrugSelector");
    a->setIcon(th->icon(Constants::I_TOGGLEDRUGSELECTOR));
    a->setCheckable(true);
    cmd = actionManager()->registerAction(a, Constants::A_TOGGLE_DRUGSELECTOR, ctx);
    cmd->setTranslations(Constants::TOGGLEDRUGSELECTOR_TEXT, Constants::TOGGLEDRUGSELECTOR_TEXT, Constants::DRUGCONSTANTS_TR_CONTEXT);
    menu->addAction(cmd, DrugsWidget::Constants::G_PLUGINS_VIEWS);
    connect(a, SIGNAL(triggered()), this, SLOT(toggleDrugSelector()));

    a = aClear = new QAction(this);
    a->setIcon(th->icon(Core::Constants::ICONCLEAR));
    cmd = actionManager()->registerAction(a, Core::Constants::A_LIST_CLEAR, ctx);
    cmd->setTranslations(Trans::Constants::LISTCLEAR_TEXT);
    menu->addAction(cmd, DrugsWidget::Constants::G_PLUGINS_DRUGS);
    connect(a, SIGNAL(triggered()), this, SLOT(clear()));

    a = aRemoveRow = new QAction(this);
    a->setIcon(th->icon(Core::Constants::ICONREMOVE));
    cmd = actionManager()->registerAction(a, Core::Constants::A_LIST_REMOVE, ctx);
    cmd->setTranslations(Trans::Constants::LISTREMOVE_TEXT);
    menu->addAction(cmd, DrugsWidget::Constants::G_PLUGINS_DRUGS);
    connect(a, SIGNAL(triggered()), this, SLOT(removeItem()));

    a = aDown = new QAction(this);
    a->setIcon(th->icon(Core::Constants::ICONMOVEDOWN));
    cmd = actionManager()->registerAction(a, Core::Constants::A_LIST_MOVEDOWN, ctx);
    cmd->setTranslations(Trans::Constants::LISTMOVEDOWN_TEXT);
    menu->addAction(cmd, DrugsWidget::Constants::G_PLUGINS_DRUGS);
    connect(a, SIGNAL(triggered()), this, SLOT(moveDown()));

    a = aUp = new QAction(this);
    a->setIcon(th->icon(Core::Constants::ICONMOVEUP));
    cmd = actionManager()->registerAction(a, Core::Constants::A_LIST_MOVEUP, ctx);
    cmd->setTranslations(Trans::Constants::LISTMOVEUP_TEXT);
    menu->addAction(cmd, DrugsWidget::Constants::G_PLUGINS_DRUGS);
    connect(a, SIGNAL(triggered()), this, SLOT(moveUp()));

    a = aSort = new QAction(this);
    a->setIcon(th->icon(Core::Constants::ICONSORT));
    cmd = actionManager()->registerAction(a, Core::Constants::A_LIST_SORT, ctx);
    cmd->setTranslations(Trans::Constants::LISTSORT_TEXT);
    menu->addAction(cmd, DrugsWidget::Constants::G_PLUGINS_DRUGS);
    connect(a, SIGNAL(triggered()), this, SLOT(sortDrugs()));

    a = aViewInteractions = new QAction(this);
    a->setIcon(th->icon(DrugsDB::Constants::I_DRUGENGINE));
    cmd = actionManager()->registerAction(a, DrugsWidget::Constants::A_VIEW_INTERACTIONS, ctx);
    cmd->setTranslations(Trans::Constants::VIEWINTERACTIONS_TEXT);
    menu->addAction(cmd, DrugsWidget::Constants::G_PLUGINS_INTERACTIONS);
    connect(a, SIGNAL(triggered()), this, SLOT(viewInteractions()));

    a = aSearch = new QAction(this);
#ifdef FREEDIAMS
    QList<int> globalcontext = QList<int>() << Core::Constants::C_GLOBAL_ID;
    cmd = actionManager()->registerAction(a, Core::Constants::A_EDIT_SEARCH, globalcontext);
#else
    cmd = actionManager()->registerAction(a, Core::Constants::A_EDIT_SEARCH, ctx);
#endif
    connect(a, SIGNAL(triggered()), this, SLOT(searchTriggered()));

    a = aToggleTestingDrugs = new QAction(this);
    a->setIcon(th->icon(DrugsWidget::Constants::I_TOGGLETESTINGDRUGS));
    a->setCheckable(true);
    a->setChecked(true);
    cmd = actionManager()->registerAction(a, DrugsWidget::Constants::A_TOGGLE_TESTINGDRUGS, ctx);
    cmd->setTranslations(DrugsWidget::Constants::TOGGLETESTINGDRUGS_TEXT, Constants::TOGGLETESTINGDRUGS_TEXT, Constants::DRUGCONSTANTS_TR_CONTEXT);
    menu->addAction(cmd, DrugsWidget::Constants::G_PLUGINS_VIEWS);
    connect(a, SIGNAL(triggered()), this, SLOT(toggleTestingDrugs()));

    // Search method menu
    Core::ActionContainer *searchmenu = actionManager()->actionContainer(DrugsWidget::Constants::M_PLUGINS_SEARCH);
    if (!searchmenu) {
        searchmenu = actionManager()->createMenu(DrugsWidget::Constants::M_PLUGINS_SEARCH);
        searchmenu->appendGroup(DrugsWidget::Constants::G_PLUGINS_SEARCH);
        searchmenu->setTranslations(Trans::Constants::SEARCHMENU_TEXT);
        menu->addMenu(searchmenu, DrugsWidget::Constants::G_PLUGINS_SEARCH);
    }
    Q_ASSERT(searchmenu);

    gSearchMethod = new QActionGroup(this);
    int m = settings()->value(Constants::S_SEARCHMETHOD).toInt();
    a = aSearchCommercial = new QAction(this);
    a->setCheckable(true);
    a->setChecked(m==Constants::SearchCommercial);
    a->setIcon(th->icon(DrugsDB::Constants::I_SEARCHCOMMERCIAL));
    cmd = actionManager()->registerAction(a, DrugsWidget::Constants::A_SEARCH_COMMERCIAL, ctx);
    cmd->setTranslations(DrugsWidget::Constants::SEARCHCOMMERCIAL_TEXT, "", DRUGCONSTANTS_TR_CONTEXT);
//    cmd->setAttribute(Core::Command::CA_UpdateText);
    searchmenu->addAction(cmd, DrugsWidget::Constants::G_PLUGINS_SEARCH);
    gSearchMethod->addAction(a);

    a = aSearchMolecules = new QAction(this);
    a->setCheckable(true);
    a->setChecked(m==Constants::SearchMolecules);
    a->setIcon(th->icon(DrugsDB::Constants::I_SEARCHMOLS));
    cmd = actionManager()->registerAction(a, DrugsWidget::Constants::A_SEARCH_MOLECULES, ctx);
    cmd->setTranslations(DrugsWidget::Constants::SEARCHMOLECULES_TEXT, "", DRUGCONSTANTS_TR_CONTEXT);
//    cmd->setAttribute(Core::Command::CA_UpdateText);
    searchmenu->addAction(cmd, DrugsWidget::Constants::G_PLUGINS_SEARCH);
    gSearchMethod->addAction(a);

    a = aSearchInn = new QAction(this);
    a->setCheckable(true);
    a->setChecked(m==Constants::SearchInn);
    a->setIcon(th->icon(DrugsDB::Constants::I_SEARCHINN));
    cmd = actionManager()->registerAction(a, DrugsWidget::Constants::A_SEARCH_INN, ctx);
    cmd->setTranslations(DrugsWidget::Constants::SEARCHINN_TEXT, "", DRUGCONSTANTS_TR_CONTEXT);
//    cmd->setAttribute(Core::Command::CA_UpdateText);
    searchmenu->addAction(cmd, DrugsWidget::Constants::G_PLUGINS_SEARCH);
    gSearchMethod->addAction(a);
    connect(gSearchMethod,SIGNAL(triggered(QAction*)),this,SLOT(searchActionChanged(QAction*)));

    Core::ActionContainer *tmenu = actionManager()->actionContainer(Core::Constants::M_TEMPLATES);
    a = aToTemplate = new QAction(this);
    a->setIcon(th->icon(Core::Constants::ICONTEMPLATES));
    cmd = actionManager()->registerAction(a, Core::Constants::A_TEMPLATE_CREATE, ctx);
    cmd->setTranslations(Trans::Constants::CREATETEMPLATE_TEXT, Trans::Constants::CREATETEMPLATE_TEXT);
    //    cmd->setKeySequence();
    cmd->retranslate();
    if (tmenu) {
        tmenu->addAction(cmd, Core::Constants::G_TEMPLATES_NEW);
    }
    connect(a, SIGNAL(triggered()), this, SLOT(createTemplate()));

    Core::ActionContainer *fmenu = actionManager()->actionContainer(Core::Constants::M_FILE);
    a = aPrintPrescription = new QAction(this);
    a->setIcon(th->icon(Core::Constants::ICONPRINT));
    //    a->setShortcut(tkTr(Trans::Constants::K_PRINT_PRESCRIPTION));
    cmd = actionManager()->registerAction(a, DrugsWidget::Constants::A_PRINT_PRESCRIPTION, ctx);
    cmd->setTranslations(DrugsWidget::Constants::PRINTPRESCRIPTION_TEXT, "", DRUGCONSTANTS_TR_CONTEXT);
#ifdef FREEDIAMS
    cmd->setKeySequence(QKeySequence::Print);
#else
    cmd->setKeySequence(tkTr(Trans::Constants::K_PRINT_PRESCRIPTION));
#endif
    cmd->retranslate();
    if (fmenu) {
        fmenu->addAction(cmd, Core::Constants::G_FILE_PRINT);
    }
    connect(aPrintPrescription,SIGNAL(triggered()), this, SLOT(printPrescription()));

    a = aPrintPreview = new QAction(this);
    a->setIcon(th->icon(Core::Constants::ICONPRINTPREVIEW));
    //    a->setShortcut(tkTr(Trans::Constants::K_PRINT_PRESCRIPTION));
    cmd = actionManager()->registerAction(a, Core::Constants::A_FILE_PRINTPREVIEW, ctx);
    cmd->setTranslations(Trans::Constants::PRINTPREVIEW_TEXT, Trans::Constants::PRINTPREVIEW_TEXT);
    cmd->retranslate();
    if (fmenu) {
        fmenu->addAction(cmd, Core::Constants::G_FILE_PRINT);
    }
    connect(aPrintPreview,SIGNAL(triggered()), this, SLOT(printPreview()));

    a = aChangeDuration = new QAction(this);
    a->setObjectName("aChangeDuration");
    a->setIcon(th->icon(Core::Constants::ICONDATE));
    cmd = actionManager()->registerAction(a, DrugsWidget::Constants::A_CHANGE_DURATION, ctx);
    cmd->setTranslations(Trans::Constants::DURATION);
    connect(aChangeDuration,SIGNAL(triggered()),this,SLOT(changeDuration()));

    // Databases information
    Core::ActionContainer *hmenu = actionManager()->actionContainer(Core::Constants::M_HELP_DATABASES);
    a = aDrugsDatabaseInformation = new QAction(this);
    a->setIcon(th->icon(Core::Constants::ICONHELP));
    cmd = actionManager()->registerAction(a, DrugsWidget::Constants::A_DRUGSDATABASE_INFORMATION, QList<int>() << Core::Constants::C_GLOBAL_ID);
    cmd->setTranslations(Trans::Constants::DRUGS_DATABASE_INFORMATION);
    cmd->retranslate();
    if (hmenu) {
        hmenu->addAction(cmd, Core::Constants::G_HELP_DATABASES);
    }
    connect(aDrugsDatabaseInformation,SIGNAL(triggered()), this, SLOT(showDrugsDatabaseInformation()));

    a = aDosagesDatabaseInformation = new QAction(this);
    a->setIcon(th->icon(Core::Constants::ICONHELP));
    cmd = actionManager()->registerAction(a, DrugsWidget::Constants::A_DOSAGESDATABASE_INFORMATION, QList<int>() << Core::Constants::C_GLOBAL_ID);
    cmd->setTranslations(Trans::Constants::DOSAGES_DATABASE_INFORMATION);
    cmd->retranslate();
    if (hmenu) {
        hmenu->addAction(cmd, Core::Constants::G_HELP_DATABASES);
    }
    connect(aDosagesDatabaseInformation,SIGNAL(triggered()), this, SLOT(showDosagesDatabaseInformation()));

    // Mode menu
    Core::ActionContainer *modemenu = actionManager()->actionContainer(DrugsWidget::Constants::M_PLUGINS_MODES);
    if (!modemenu) {
        modemenu = actionManager()->createMenu(DrugsWidget::Constants::M_PLUGINS_MODES);
        modemenu->appendGroup(DrugsWidget::Constants::G_PLUGINS_MODES);
        modemenu->setTranslations(DrugsWidget::Constants::MODEMENU_TEXT, DRUGCONSTANTS_TR_CONTEXT);
        menu->addMenu(modemenu, DrugsWidget::Constants::G_PLUGINS_MODES);
    }
    Q_ASSERT(modemenu);

    gModes = new QActionGroup(this);
    a = aPrescriberMode = new QAction(this);
    a->setCheckable(true);
    a->setChecked(true);
    cmd = actionManager()->registerAction(a, DrugsWidget::Constants::A_PRESCRIBERMODE, ctx);
    cmd->setTranslations(DrugsWidget::Constants::PRESCRIBERMODE_TEXT, DrugsWidget::Constants::PRESCRIBERMODE_TEXT, DRUGCONSTANTS_TR_CONTEXT);
    modemenu->addAction(cmd, DrugsWidget::Constants::G_PLUGINS_MODES);
    gModes->addAction(a);

    a = aSelectOnlyMode = new QAction(this);
    a->setCheckable(true);
    a->setChecked(false);
    cmd = actionManager()->registerAction(a, DrugsWidget::Constants::A_SELECTONLYMODE, ctx);
    cmd->setTranslations(DrugsWidget::Constants::SELECTONLYMODE_TEXT, DrugsWidget::Constants::SELECTONLYMODE_TEXT, DRUGCONSTANTS_TR_CONTEXT);
    modemenu->addAction(cmd, DrugsWidget::Constants::G_PLUGINS_MODES);
    gModes->addAction(a);
    connect(gModes,SIGNAL(triggered(QAction*)),this,SLOT(modeActionChanged(QAction*)));

    a = aOpenDosageDialog = new QAction(this);
    a->setObjectName("aOpenDosageDialog");
    //    a->setIcon(th->icon(Core::Constants::ICONDATE));
    cmd = actionManager()->registerAction(a, Constants::A_OPENDOSAGEDIALOG, ctx);
    cmd->setTranslations(Constants::OPENDOSAGEDIALOG_TEXT, Constants::OPENDOSAGEDIALOG_TEXT, Constants::DRUGCONSTANTS_TR_CONTEXT);
    connect(aOpenDosageDialog,SIGNAL(triggered()),this,SLOT(openDosageDialog()));

    a = aOpenPrescriptionSentencePreferences = new QAction(this);
    a->setObjectName("aOpenPrescriptionSentencePreferences");
    //    a->setIcon(th->icon(Core::Constants::ICONDATE));
    cmd = actionManager()->registerAction(a, Constants::A_OPENDOSAGEPREFERENCES, ctx);
    cmd->setTranslations(Constants::OPENDOSAGEPREFERENCES_TEXT, Constants::OPENDOSAGEPREFERENCES_TEXT, Constants::DRUGCONSTANTS_TR_CONTEXT);
    connect(aOpenPrescriptionSentencePreferences,SIGNAL(triggered()),this,SLOT(openProtocolPreferencesDialog()));

    a = aResetPrescriptionSentenceToDefault = new QAction(this);
    a->setObjectName("aResetPrescriptionSentenceToDefault");
    //    a->setIcon(th->icon(Core::Constants::ICONDATE));
    cmd = actionManager()->registerAction(a, Constants::A_RESETPRESCRIPTIONSENTENCE_TODEFAULT, ctx);
    cmd->setTranslations(Constants::RESETPRESCRIPTIONSENTENCETODEFAULT_TEXT, Constants::RESETPRESCRIPTIONSENTENCETODEFAULT_TEXT, Constants::DRUGCONSTANTS_TR_CONTEXT);
    menu->addAction(cmd, G_PLUGINS_DRUGS);
    connect(aResetPrescriptionSentenceToDefault,SIGNAL(triggered()),this,SLOT(resetPrescriptionSentenceToDefault()));

    a = aShowDrugPrecautions = new QAction(this);
    a->setObjectName("aShowDrugPrecautions");
    a->setIcon(th->icon(DrugsDB::Constants::I_ALLERGYENGINE));
    cmd = actionManager()->registerAction(a, Constants::A_SHOWDRUGPRECAUTIONS, ctx);
    cmd->setTranslations(Constants::SHOWDRUGPRECAUTIONS_TEXT, Constants::SHOWDRUGPRECAUTIONS_TEXT, Constants::DRUGCONSTANTS_TR_CONTEXT);
    menu->addAction(cmd, DrugsWidget::Constants::G_PLUGINS_VIEWS);
    connect(aShowDrugPrecautions, SIGNAL(triggered()), this, SLOT(showDrugPrecautions()));

    a = aCopyPrescriptionItem = new QAction(this);
    a->setObjectName("aCopyPrescriptionItem");
    a->setIcon(th->icon(Core::Constants::ICONCOPY));
    cmd = actionManager()->registerAction(a, Constants::A_COPYPRESCRIPTIONITEM, ctx);
    cmd->setTranslations(Constants::COPYPRESCRIPTIONITEM_TEXT, Constants::COPYPRESCRIPTIONITEM_TEXT, Constants::DRUGCONSTANTS_TR_CONTEXT);
    menu->addAction(cmd, DrugsWidget::Constants::G_PLUGINS_VIEWS);
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
    aUp->setEnabled(canMoveUp());
    aDown->setEnabled(canMoveDown());
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

bool DrugsActionHandler::canMoveUp()
{
    if (!m_CurrentView)
        return false;
    QModelIndex idx = m_CurrentView->prescriptionListView()->currentIndex();
    if (!idx.isValid())
        return false;
    if (idx.row() >= 1)
        return true;
    return false;
}

bool DrugsActionHandler::canMoveDown()
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
        m_CurrentView->prescriptionView()->clearTriggered();
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

void DrugsActionHandler::resetPrescriptionSentenceToDefault()
{
#ifdef WITH_PAD
    QString content = Utils::readTextFile(settings()->path(Core::ISettings::BundleResourcesPath) + QString(DrugsDB::Constants::S_DEF_PRESCRIPTION_TOKENFILE_1_LANG).arg(QLocale().name().left(2).toLower()));
    if (content.isEmpty()) {
        content = Utils::readTextFile(settings()->path(Core::ISettings::BundleResourcesPath) + QString(DrugsDB::Constants::S_DEF_PRESCRIPTION_TOKENFILE_1_LANG).arg(Trans::Constants::ALL_LANGUAGE));
        if (content.isEmpty()) {
            LOG_ERROR_FOR("DrugsPrintWidget", "No token'd prescription file found");
        }
    }
    if (content.contains("<body"))
        content = content.remove("\n");

    settings()->setValue(DrugsDB::Constants::S_PRESCRIPTIONFORMATTING_HTML, content);
    QTextDocument doc;
    doc.setHtml(content);
    settings()->setValue(DrugsDB::Constants::S_PRESCRIPTIONFORMATTING_PLAIN, doc.toPlainText());
#else
    settings()->setValue(DrugsDB::Constants::S_PRESCRIPTIONFORMATTING_HTML,
                         QCoreApplication::translate(
                                 Constants::DRUGCONSTANTS_TR_CONTEXT,
                                 DrugsDB::Constants::S_DEF_PRESCRIPTIONFORMATTING));
    settings()->setValue(DrugsDB::Constants::S_PRESCRIPTIONFORMATTING_PLAIN,
                         QCoreApplication::translate(
                                 Constants::DRUGCONSTANTS_TR_CONTEXT,
                                 DrugsDB::Constants::S_DEF_PRESCRIPTIONFORMATTING_PLAIN));
#endif
    DrugsDB::DrugsModel::activeModel()->resetModel();
}

void DrugsActionHandler::showDrugPrecautions()
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
        m_PrecautionDock->setWidget(tree);
        m_PrecautionDock->setFloating(false);
        m_PrecautionDock->setAllowedAreas(Qt::RightDockWidgetArea);
        Core::ICore::instance()->mainWindow()->addDockWidget(Qt::RightDockWidgetArea, m_PrecautionDock);
    }
    m_PrecautionDock->show();
}

void DrugsActionHandler::copyPrescriptionItem()
{
    if (m_CurrentView) {
        Q_ASSERT(m_CurrentView->prescriptionView());
        m_CurrentView->prescriptionView()->copyPrescriptionItem();
    }
}
