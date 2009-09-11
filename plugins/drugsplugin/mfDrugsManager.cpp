/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
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
#include "mfDrugsManager.h"

// include drugs widget headers
#include <mfDrugsConstants.h>
#include <drugswidget/mfDrugsCentralWidget.h>
#include <drugswidget/mfPrescriptionViewer.h>

#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/constants.h>
#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/uniqueidmanager.h>

using namespace mfDrugsConstants;
using namespace Drugs::Internal;
using namespace Drugs;
using namespace Trans::ConstantTranslations;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////      MANAGER      ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
DrugsManager *DrugsManager::m_Instance = 0;

DrugsManager *DrugsManager::instance()
{
    if (!m_Instance)
        m_Instance = new DrugsManager(qApp);
    return m_Instance;
}

DrugsManager::DrugsManager(QObject *parent) : DrugsActionHandler(parent)
{
    connect(Core::ICore::instance()->contextManager(), SIGNAL(contextChanged(Core::IContext*)),
            this, SLOT(updateContext(Core::IContext*)));
    setObjectName("DrugsManager");
    Utils::Log::addMessage(this, "Instance created");
}

void DrugsManager::updateContext(Core::IContext *object)
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

DrugsCentralWidget *DrugsManager::currentView() const
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
        aToogleTestingDrugs(0),
        m_CurrentView(0)
{
    setObjectName("DrugsActionHandler");
    Utils::Log::addMessage(this, "Instance created");

    Core::ActionManager *am = Core::ICore::instance()->actionManager();
    Core::UniqueIDManager *uid = Core::ICore::instance()->uniqueIDManager();
    Core::ITheme *th = Core::ICore::instance()->theme();

    QAction *a = 0;
    Core::Command *cmd = 0;
    QList<int> ctx = QList<int>() << uid->uniqueIdentifier(mfDrugsConstants::C_DRUGS_PLUGINS);

    Core::ActionContainer *menu = am->actionContainer(mfDrugsConstants::M_PLUGINS_DRUGS);
    if (!menu) {
        menu = am->createMenu(mfDrugsConstants::M_PLUGINS_DRUGS);
        menu->appendGroup(mfDrugsConstants::G_PLUGINS_SEARCH);
        menu->appendGroup(mfDrugsConstants::G_PLUGINS_DRUGS);
        menu->setTranslations(mfDrugsConstants::DRUGSMENU_TEXT);
    }
    Q_ASSERT(menu);
#ifdef FREEDIAMS
    am->actionContainer(Core::Constants::MENUBAR)->addMenu(menu, mfDrugsConstants::G_PLUGINS_DRUGS);
#else
    am->actionContainer(Core::Constants::M_PLUGINS)->addMenu(menu, mfDrugsConstants::G_PLUGINS_DRUGS);
#endif

    // Create local actions
    a = aClear = new QAction(this);
    a->setIcon(th->icon(Core::Constants::ICONCLEAR));
    cmd = am->registerAction(a, Core::Constants::A_LIST_CLEAR, ctx);
    cmd->setTranslations(Trans::Constants::LISTCLEAR_TEXT);
    menu->addAction(cmd, mfDrugsConstants::G_PLUGINS_DRUGS);
    connect(a, SIGNAL(triggered()), this, SLOT(clear()));

    a = aRemoveRow = new QAction(this);
    a->setIcon(th->icon(Core::Constants::ICONREMOVE));
    cmd = am->registerAction(a, Core::Constants::A_LIST_REMOVE, ctx);
    cmd->setTranslations(Trans::Constants::LISTREMOVE_TEXT);
    menu->addAction(cmd, mfDrugsConstants::G_PLUGINS_DRUGS);
    connect(a, SIGNAL(triggered()), this, SLOT(removeItem()));

    a = aDown = new QAction(this);
    a->setIcon(th->icon(Core::Constants::ICONMOVEDOWN));
    cmd = am->registerAction(a, Core::Constants::A_LIST_MOVEDOWN, ctx);
    cmd->setTranslations(Trans::Constants::LISTMOVEDOWN_TEXT);
    menu->addAction(cmd, mfDrugsConstants::G_PLUGINS_DRUGS);
    connect(a, SIGNAL(triggered()), this, SLOT(moveDown()));

    a = aUp = new QAction(this);
    a->setIcon(th->icon(Core::Constants::ICONMOVEUP));
    cmd = am->registerAction(a, Core::Constants::A_LIST_MOVEUP, ctx);
    cmd->setTranslations(Trans::Constants::LISTMOVEUP_TEXT);
    menu->addAction(cmd, mfDrugsConstants::G_PLUGINS_DRUGS);
    connect(a, SIGNAL(triggered()), this, SLOT(moveUp()));

    a = aSort = new QAction(this);
    a->setIcon(th->icon(Core::Constants::ICONSORT));
    cmd = am->registerAction(a, Core::Constants::A_LIST_SORT, ctx);
    cmd->setTranslations(Trans::Constants::LISTSORT_TEXT);
    menu->addAction(cmd, mfDrugsConstants::G_PLUGINS_DRUGS);
    connect(a, SIGNAL(triggered()), this, SLOT(sortDrugs()));

    a = aViewInteractions = new QAction(this);
    a->setIcon(th->icon(Core::Constants::ICONEYES));
    cmd = am->registerAction(a, Core::Constants::A_VIEW_INTERACTIONS, ctx);
    cmd->setTranslations(Trans::Constants::VIEWINTERACTIONS_TEXT);
    menu->addAction(cmd, mfDrugsConstants::G_PLUGINS_DRUGS);
    connect(a, SIGNAL(triggered()), this, SLOT(viewInteractions()));

    a = aToogleTestingDrugs = new QAction(this);
    a->setIcon(th->icon(mfDrugsConstants::ICONTOOGLETESTINGDRUGS));
    a->setCheckable(true);
    a->setChecked(true);
    cmd = am->registerAction(a, mfDrugsConstants::A_TOOGLE_TESTINGDRUGS, ctx);
    cmd->setTranslations(mfDrugsConstants::TOOGLETESTINGDRUGS_TEXT);
    menu->addAction(cmd, mfDrugsConstants::G_PLUGINS_DRUGS);
    connect(a, SIGNAL(triggered()), this, SLOT(toogleTestingDrugs()));

    // Search method menu
    Core::ActionContainer *searchmenu = am->actionContainer(mfDrugsConstants::M_PLUGINS_SEARCH);
    if (!searchmenu) {
        searchmenu = am->createMenu(mfDrugsConstants::M_PLUGINS_SEARCH);
        searchmenu->appendGroup(mfDrugsConstants::G_PLUGINS_SEARCH);
        searchmenu->setTranslations(mfDrugsConstants::SEARCHMENU_TEXT, DRUGCONSTANTS_TR_CONTEXT);
        menu->addMenu(searchmenu, mfDrugsConstants::G_PLUGINS_SEARCH);
    }
    Q_ASSERT(searchmenu);

    gSearchMethod = new QActionGroup(this);
    a = aSearchCommercial = new QAction(this);
    a->setCheckable(true);
    a->setChecked(false);
    a->setIcon(th->icon(mfDrugsConstants::MFDRUGS_ICONSEARCHCOMMERCIAL));
    cmd = am->registerAction(a, mfDrugsConstants::A_SEARCH_COMMERCIAL, ctx);
    cmd->setTranslations(mfDrugsConstants::SEARCHCOMMERCIAL_TEXT, "", DRUGCONSTANTS_TR_CONTEXT);
    searchmenu->addAction(cmd, mfDrugsConstants::G_PLUGINS_SEARCH);
    gSearchMethod->addAction(a);

    a = aSearchMolecules = new QAction(this);
    a->setCheckable(true);
    a->setChecked(false);
    a->setIcon(th->icon(mfDrugsConstants::MFDRUGS_ICONSEARCHMOLS));
    cmd = am->registerAction(a, mfDrugsConstants::A_SEARCH_MOLECULES, ctx);
    cmd->setTranslations(mfDrugsConstants::SEARCHMOLECULES_TEXT, "", DRUGCONSTANTS_TR_CONTEXT);
    searchmenu->addAction(cmd, mfDrugsConstants::G_PLUGINS_SEARCH);
    gSearchMethod->addAction(a);

    a = aSearchInn = new QAction(this);
    a->setCheckable(true);
    a->setChecked(false);
    a->setIcon(th->icon(mfDrugsConstants::MFDRUGS_ICONSEARCHINN));
    cmd = am->registerAction(a, mfDrugsConstants::A_SEARCH_INN, ctx);
    cmd->setTranslations(mfDrugsConstants::SEARCHINN_TEXT, "", DRUGCONSTANTS_TR_CONTEXT);
    searchmenu->addAction(cmd, mfDrugsConstants::G_PLUGINS_SEARCH);
    gSearchMethod->addAction(a);
    connect(gSearchMethod,SIGNAL(triggered(QAction*)),this,SLOT(searchActionChanged(QAction*)));

    Core::ActionContainer *filemenu = am->actionContainer(Core::Constants::M_FILE);
    a = aPrintPrescription = new QAction(this);
    a->setIcon(th->icon(Core::Constants::ICONPRINT));
    a->setShortcut(tkTr(Trans::Constants::K_PRINT_PRESCRIPTION));
    cmd = am->registerAction(a, mfDrugsConstants::A_PRINT_PRESCRIPTION, ctx);
    cmd->setTranslations(mfDrugsConstants::PRINTPRESCRIPTION_TEXT, "", DRUGCONSTANTS_TR_CONTEXT);
    cmd->setKeySequence(tkTr(Trans::Constants::K_PRINT_PRESCRIPTION));
    cmd->retranslate();
    if (filemenu) {
        filemenu->addAction(cmd, Core::Constants::G_FILE_PRINT);
    }
    connect(aPrintPrescription,SIGNAL(triggered()), this, SLOT(printPrescription()));

    am->retranslateMenusAndActions();
}

void DrugsActionHandler::searchActionChanged(QAction *a)
{
    if (!m_CurrentView)
        return;
    if (a==aSearchCommercial)
        m_CurrentView->setCurrentSearchMethod(mfDrugsConstants::SearchCommercial);
    else if (a==aSearchMolecules)
        m_CurrentView->setCurrentSearchMethod(mfDrugsConstants::SearchMolecules);
    else if (a==aSearchInn)
        m_CurrentView->setCurrentSearchMethod(mfDrugsConstants::SearchInn);
}

void DrugsActionHandler::setCurrentView(DrugsCentralWidget *view)
{
    Q_ASSERT(view);
    if (!view) { // this should never be the case
        return;
    }
    qWarning() << "DrugsActionHandler::setCurrentView(DrugsCentralWidget *view)";

    // disconnect old view
    if (m_CurrentView) {
        if (view == m_CurrentView.data())
            return;
        m_CurrentView->disconnect();
        disconnect(m_CurrentView->prescriptionListView()->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                   this, SLOT(listViewItemChanged()));
        disconnect(m_CurrentView->currentDrugsModel(), SIGNAL(numberOfRowsChanged()),
                   this, SLOT(drugsModelChanged()));
    }
    m_CurrentView = view;
    // reconnect some actions
    m_CurrentView->createConnections();
    connect(m_CurrentView->prescriptionListView()->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this, SLOT(listViewItemChanged()));
    connect(m_CurrentView->currentDrugsModel(), SIGNAL(numberOfRowsChanged()),
            this, SLOT(drugsModelChanged()));
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
    if (m_CurrentView)
        m_CurrentView->prescriptionView()->moveUp();
}

void DrugsActionHandler::moveDown()
{
    if (m_CurrentView)
        m_CurrentView->prescriptionView()->moveDown();
}

void DrugsActionHandler::sortDrugs()
{
    if (m_CurrentView)
       m_CurrentView->currentDrugsModel()->sort(0);
}

void DrugsActionHandler::removeItem()
{
    if (m_CurrentView)
        m_CurrentView->prescriptionView()->removeTriggered();
}

void DrugsActionHandler::clear()
{
    if (m_CurrentView)
        m_CurrentView->prescriptionView()->clearTriggered();
}

void DrugsActionHandler::viewInteractions()
{
    if (m_CurrentView)
        m_CurrentView->prescriptionView()->viewInteractions();
}

void DrugsActionHandler::printPrescription()
{
    if (m_CurrentView)
        m_CurrentView->printPrescription();
}

void DrugsActionHandler::toogleTestingDrugs()
{
    if (m_CurrentView)
       m_CurrentView->currentDrugsModel()->showTestingDrugs(aToogleTestingDrugs->isChecked());
}
