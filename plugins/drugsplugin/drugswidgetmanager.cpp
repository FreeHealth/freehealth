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
#include "drugswidgetmanager.h"

// include drugs widget headers
#include <drugsplugin/constants.h>
#include <drugsplugin/drugswidget/mfDrugsCentralWidget.h>
#include <drugsplugin/drugswidget/mfPrescriptionViewer.h>
#include <drugsplugin/drugswidget/mfDrugSelector.h>

#include <drugsbaseplugin/drugsbasemanager.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/constants.h>
#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/uniqueidmanager.h>

using namespace DrugsWidget::Constants;
using namespace DrugsWidget;
using namespace DrugsWidget::Internal;
using namespace Trans::ConstantTranslations;

inline static Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}
static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }

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
    Utils::Log::addMessage(this, "Instance created");
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
        aAddRow(0),
        aRemoveRow(0),
        aDown(0),
        aUp(0),
        aSort(0),
        aEdit(0),
        aClear(0),
        aViewInteractions(0),
        gSearchMethod(0),
        aSearchCommercial(0),
        aSearchMolecules(0),
        aSearchInn(0),
        aPrintPrescription(0),
        aPrintPreview(0),
        aToggleTestingDrugs(0),
        aChangeDuration(0),
        aToTemplate(0),
        aDrugsDatabaseInformations(0),
        aDosagesDatabaseInformations(0),
        aIamDatabaseInformations(0),
        gModes(0),
        aPrescriberMode(0),
        aSelectOnlyMode(0),
        aOpenDosageDialog(0),
        aOpenPrescriptionSentencePreferences(0),
        aResetPrescriptionSentenceToDefault(0),
        m_CurrentView(0)
{
    setObjectName("DrugsActionHandler");
    Utils::Log::addMessage(this, "Instance created");

    Core::UniqueIDManager *uid = Core::ICore::instance()->uniqueIDManager();
    Core::ITheme *th = Core::ICore::instance()->theme();

    QAction *a = 0;
    Core::Command *cmd = 0;
    QList<int> ctx = QList<int>() << uid->uniqueIdentifier(DrugsWidget::Constants::C_DRUGS_PLUGINS);
    QList<int> globalcontext = QList<int>() << Core::Constants::C_GLOBAL_ID;

    Core::ActionContainer *menu = actionManager()->actionContainer(DrugsWidget::Constants::M_PLUGINS_DRUGS);
    if (!menu) {
        menu = actionManager()->createMenu(DrugsWidget::Constants::M_PLUGINS_DRUGS);
        menu->appendGroup(DrugsWidget::Constants::G_PLUGINS_MODES);
        menu->appendGroup(DrugsWidget::Constants::G_PLUGINS_SEARCH);
        menu->appendGroup(DrugsWidget::Constants::G_PLUGINS_DRUGS);
        menu->appendGroup(DrugsWidget::Constants::G_PLUGINS_INTERACTIONS);
        menu->setTranslations(DrugsWidget::Constants::DRUGSMENU_TEXT);
    }
    Q_ASSERT(menu);
#ifdef FREEDIAMS
    actionManager()->actionContainer(Core::Constants::MENUBAR)->addMenu(menu, DrugsWidget::Constants::G_PLUGINS_DRUGS);
#else
    actionManager()->actionContainer(Core::Constants::M_PLUGINS)->addMenu(menu, Core::Constants::G_PLUGINS_DRUGS);
#endif

    // Create local actions
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
    a->setIcon(th->icon(Core::Constants::ICONEYES));
    cmd = actionManager()->registerAction(a, DrugsWidget::Constants::A_VIEW_INTERACTIONS, ctx);
    cmd->setTranslations(Trans::Constants::VIEWINTERACTIONS_TEXT);
    menu->addAction(cmd, DrugsWidget::Constants::G_PLUGINS_INTERACTIONS);
    connect(a, SIGNAL(triggered()), this, SLOT(viewInteractions()));

    a = aToggleTestingDrugs = new QAction(this);
    a->setIcon(th->icon(DrugsWidget::Constants::I_TOGGLETESTINGDRUGS));
    a->setCheckable(true);
    a->setChecked(true);
    cmd = actionManager()->registerAction(a, DrugsWidget::Constants::A_TOGGLE_TESTINGDRUGS, ctx);
    cmd->setTranslations(DrugsWidget::Constants::TOGGLETESTINGDRUGS_TEXT);
    menu->addAction(cmd, DrugsWidget::Constants::G_PLUGINS_INTERACTIONS);
    connect(a, SIGNAL(triggered()), this, SLOT(toggleTestingDrugs()));

    // Search method menu
    Core::ActionContainer *searchmenu = actionManager()->actionContainer(DrugsWidget::Constants::M_PLUGINS_SEARCH);
    if (!searchmenu) {
        searchmenu = actionManager()->createMenu(DrugsWidget::Constants::M_PLUGINS_SEARCH);
        searchmenu->appendGroup(DrugsWidget::Constants::G_PLUGINS_SEARCH);
        searchmenu->setTranslations(DrugsWidget::Constants::SEARCHMENU_TEXT, DRUGCONSTANTS_TR_CONTEXT);
        menu->addMenu(searchmenu, DrugsWidget::Constants::G_PLUGINS_SEARCH);
    }
    Q_ASSERT(searchmenu);

    gSearchMethod = new QActionGroup(this);
    a = aSearchCommercial = new QAction(this);
    a->setCheckable(true);
    a->setChecked(false);
    a->setIcon(th->icon(DrugsDB::Constants::I_SEARCHCOMMERCIAL));
    cmd = actionManager()->registerAction(a, DrugsWidget::Constants::A_SEARCH_COMMERCIAL, ctx);
    cmd->setTranslations(DrugsWidget::Constants::SEARCHCOMMERCIAL_TEXT, "", DRUGCONSTANTS_TR_CONTEXT);
    searchmenu->addAction(cmd, DrugsWidget::Constants::G_PLUGINS_SEARCH);
    gSearchMethod->addAction(a);

    a = aSearchMolecules = new QAction(this);
    a->setCheckable(true);
    a->setChecked(false);
    a->setIcon(th->icon(DrugsDB::Constants::I_SEARCHMOLS));
    cmd = actionManager()->registerAction(a, DrugsWidget::Constants::A_SEARCH_MOLECULES, ctx);
    cmd->setTranslations(DrugsWidget::Constants::SEARCHMOLECULES_TEXT, "", DRUGCONSTANTS_TR_CONTEXT);
    searchmenu->addAction(cmd, DrugsWidget::Constants::G_PLUGINS_SEARCH);
    gSearchMethod->addAction(a);

    a = aSearchInn = new QAction(this);
    a->setCheckable(true);
    a->setChecked(false);
    a->setIcon(th->icon(DrugsDB::Constants::I_SEARCHINN));
    cmd = actionManager()->registerAction(a, DrugsWidget::Constants::A_SEARCH_INN, ctx);
    cmd->setTranslations(DrugsWidget::Constants::SEARCHINN_TEXT, "", DRUGCONSTANTS_TR_CONTEXT);
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
    a->setIcon(th->icon(Core::Constants::ICONPRINT));
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

    // Databases informations
    Core::ActionContainer *hmenu = actionManager()->actionContainer(Core::Constants::M_HELP_DATABASES);
    a = aDrugsDatabaseInformations = new QAction(this);
    a->setIcon(th->icon(Core::Constants::ICONHELP));
    cmd = actionManager()->registerAction(a, DrugsWidget::Constants::A_DRUGSDATABASE_INFORMATIONS, QList<int>() << Core::Constants::C_GLOBAL_ID);
    cmd->setTranslations(Trans::Constants::DRUGS_DATABASE_INFORMATIONS);
    cmd->retranslate();
    if (hmenu) {
        hmenu->addAction(cmd, Core::Constants::G_HELP_DATABASES);
    }
    connect(aDrugsDatabaseInformations,SIGNAL(triggered()), this, SLOT(showDrugsDatabaseInformations()));

    a = aDosagesDatabaseInformations = new QAction(this);
    a->setIcon(th->icon(Core::Constants::ICONHELP));
    cmd = actionManager()->registerAction(a, DrugsWidget::Constants::A_DOSAGESDATABASE_INFORMATIONS, QList<int>() << Core::Constants::C_GLOBAL_ID);
    cmd->setTranslations(Trans::Constants::DOSAGES_DATABASE_INFORMATIONS);
    cmd->retranslate();
    if (hmenu) {
        hmenu->addAction(cmd, Core::Constants::G_HELP_DATABASES);
    }
    connect(aDosagesDatabaseInformations,SIGNAL(triggered()), this, SLOT(showDosagesDatabaseInformations()));

    a = aIamDatabaseInformations = new QAction(this);
    a->setIcon(th->icon(Core::Constants::ICONHELP));
    cmd = actionManager()->registerAction(a, DrugsWidget::Constants::A_IAMDATABASE_INFORMATIONS, QList<int>() << Core::Constants::C_GLOBAL_ID);
    cmd->setTranslations(Trans::Constants::INTERACTIONS_DATABASE_INFORMATIONS);
    cmd->retranslate();
    if (hmenu) {
        hmenu->addAction(cmd, Core::Constants::G_HELP_DATABASES);
    }
    connect(aIamDatabaseInformations,SIGNAL(triggered()), this, SLOT(showInteractionsDatabaseInformations()));


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

    contextManager()->updateContext();
    actionManager()->retranslateMenusAndActions();
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
        Utils::Log::addError(this, "setCurrentView : no view");
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

void DrugsActionHandler::showDrugsDatabaseInformations()
{
    if (m_CurrentView) {
        m_CurrentView->showDrugsDatabaseInformations();
    }
}

void DrugsActionHandler::showDosagesDatabaseInformations()
{
    if (m_CurrentView) {
        m_CurrentView->showDosagesDatabaseInformations();
    }
}

void DrugsActionHandler::showInteractionsDatabaseInformations()
{
    if (m_CurrentView) {
        m_CurrentView->showInteractionsDatabaseInformations();
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
        yes = Utils::yesNoMessageBox(tr("Prescription is not empty. Clear it ?"),
                                     tr("You select another editing mode than the actual one. "
                                        "Changing of mode during edition may cause prescription lose.\n"
                                        "Do you really want to change the editing mode ?"));
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

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

void DrugsActionHandler::resetPrescriptionSentenceToDefault()
{
    settings()->setValue(DrugsDB::Constants::S_PRESCRIPTIONFORMATTING_HTML,
                         QCoreApplication::translate(
                                 Constants::DRUGCONSTANTS_TR_CONTEXT,
                                 DrugsDB::Constants::S_DEF_PRESCRIPTIONFORMATTING));
    settings()->setValue(DrugsDB::Constants::S_PRESCRIPTIONFORMATTING_PLAIN,
                         QCoreApplication::translate(
                                 Constants::DRUGCONSTANTS_TR_CONTEXT,
                                 DrugsDB::Constants::S_DEF_PRESCRIPTIONFORMATTING_PLAIN));
    DrugsDB::DrugsModel::activeModel()->resetModel();
}
