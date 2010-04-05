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
#include "accountwidgetmanager.h"
#include "constants.h"

#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/constants.h>
#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/uniqueidmanager.h>

using namespace Account::Constants;
using namespace Account;
using namespace Account::Internal;
using namespace Trans::ConstantTranslations;

inline static Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////      MANAGER      ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
AccountWidgetManager *AccountWidgetManager::m_Instance = 0;

AccountWidgetManager *AccountWidgetManager::instance()
{
    if (!m_Instance)
        m_Instance = new AccountWidgetManager(qApp);
    return m_Instance;
}

AccountWidgetManager::AccountWidgetManager(QObject *parent) : AccountActionHandler(parent)
{
    connect(Core::ICore::instance()->contextManager(), SIGNAL(contextChanged(Core::IContext*)),
            this, SLOT(updateContext(Core::IContext*)));
    setObjectName("AccountWidgetManager");
    Utils::Log::addMessage(this, "Instance created");
}

void AccountWidgetManager::updateContext(Core::IContext *object)
{
//    qWarning() << "AccountManager::updateContext(Core::IContext *object)";
//    if (object)
//        qWarning() << "AccountManager::updateContext(Core::IContext *object)" << object->widget();

//    AccountCentralWidget *view = 0;
//    do {
//        if (!object) {
//            if (!m_CurrentView)
//                return;
//
////            m_CurrentView = 0;  // keep trace of the last active view (we need it in dialogs)
//            break;
//        }
//        view = qobject_cast<AccountCentralWidget *>(object->widget());
//        if (!view) {
//            if (!m_CurrentView)
//                return;
//
////            m_CurrentView = 0;   // keep trace of the last active view (we need it in dialogs)
//            break;
//        }
//
//        if (view == m_CurrentView) {
//            return;
//        }
//
//    } while (false);
//    if (view) {
//        AccountActionHandler::setCurrentView(view);
//    }
}

//AccountCentralWidget *AccountWidgetManager::currentView() const
//{
//    return AccountActionHandler::m_CurrentView;
//}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////  ACTION HANDLER   ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
AccountActionHandler::AccountActionHandler(QObject *parent) :
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
        aDatabaseInformations(0),
        gModes(0),
        aPrescriberMode(0),
        aSelectOnlyMode(0)
//        m_CurrentView(0)
{
    setObjectName("AccountActionHandler");
    Utils::Log::addMessage(this, "Instance created");

    Core::UniqueIDManager *uid = Core::ICore::instance()->uniqueIDManager();
    Core::ITheme *th = Core::ICore::instance()->theme();

//    QAction *a = 0;
//    Core::Command *cmd = 0;
//    QList<int> ctx = QList<int>() << uid->uniqueIdentifier(Account::Constants::C_ACCOUNT_PLUGINS);
//
//    Core::ActionContainer *menu = actionManager()->actionContainer(Account::Constants::M_PLUGINS_ACCOUNT);
//    if (!menu) {
//        menu = actionManager()->createMenu(Account::Constants::M_PLUGINS_ACCOUNT);
//        menu->appendGroup(DrugsWidget::Constants::G_PLUGINS_MODES);
//        menu->appendGroup(DrugsWidget::Constants::G_PLUGINS_SEARCH);
//        menu->appendGroup(DrugsWidget::Constants::G_PLUGINS_DRUGS);
//        menu->setTranslations(Account::Constants::ACCOUNTMENU_TEXT);
//    }
//    Q_ASSERT(menu);
//#ifdef FREEDIAMS
//    actionManager()->actionContainer(Core::Constants::MENUBAR)->addMenu(menu, DrugsWidget::Constants::G_PLUGINS_DRUGS);
//#else
//    actionManager()->actionContainer(Core::Constants::M_PLUGINS)->addMenu(menu, Core::Constants::G_PLUGINS_DRUGS);
//#endif

    // Create local actions
//    QAction *a = aClear = new QAction(this);
//    a->setIcon(th->icon(Core::Constants::ICONCLEAR));
//    Core::Command *cmd = actionManager()->registerAction(a, Core::Constants::A_LIST_CLEAR, ctx);
//    cmd->setTranslations(Trans::Constants::LISTCLEAR_TEXT);
////    menu->addAction(cmd, DrugsWidget::Constants::G_PLUGINS_DRUGS);
//    connect(a, SIGNAL(triggered()), this, SLOT(clear()));

//    a = aToggleTestingDrugs = new QAction(this);
//    a->setIcon(th->icon(DrugsWidget::Constants::I_TOGGLETESTINGDRUGS));
//    a->setCheckable(true);
//    a->setChecked(true);
//    cmd = actionManager()->registerAction(a, DrugsWidget::Constants::A_TOGGLE_TESTINGDRUGS, ctx);
//    cmd->setTranslations(DrugsWidget::Constants::TOGGLETESTINGDRUGS_TEXT);
//    menu->addAction(cmd, DrugsWidget::Constants::G_PLUGINS_DRUGS);
//    connect(a, SIGNAL(triggered()), this, SLOT(toggleTestingDrugs()));
//
//    // Search method menu
//    Core::ActionContainer *searchmenu = actionManager()->actionContainer(DrugsWidget::Constants::M_PLUGINS_SEARCH);
//    if (!searchmenu) {
//        searchmenu = actionManager()->createMenu(DrugsWidget::Constants::M_PLUGINS_SEARCH);
//        searchmenu->appendGroup(DrugsWidget::Constants::G_PLUGINS_SEARCH);
//        searchmenu->setTranslations(DrugsWidget::Constants::SEARCHMENU_TEXT, DRUGCONSTANTS_TR_CONTEXT);
//        menu->addMenu(searchmenu, DrugsWidget::Constants::G_PLUGINS_SEARCH);
//    }
//    Q_ASSERT(searchmenu);
//
//    gSearchMethod = new QActionGroup(this);
//    a = aSearchCommercial = new QAction(this);
//    a->setCheckable(true);
//    a->setChecked(false);
//    a->setIcon(th->icon(DrugsDB::Constants::I_SEARCHCOMMERCIAL));
//    cmd = actionManager()->registerAction(a, DrugsWidget::Constants::A_SEARCH_COMMERCIAL, ctx);
//    cmd->setTranslations(DrugsWidget::Constants::SEARCHCOMMERCIAL_TEXT, "", DRUGCONSTANTS_TR_CONTEXT);
//    searchmenu->addAction(cmd, DrugsWidget::Constants::G_PLUGINS_SEARCH);
//    gSearchMethod->addAction(a);

    actionManager()->retranslateMenusAndActions();
}

//void AccountActionHandler::setCurrentView(AccountCentralWidget *view)
        void AccountActionHandler::setCurrentView(QWidget *view)
{
    Q_ASSERT(view);
    if (!view) { // this should never be the case
        Utils::Log::addError(this, "setCurrentView : no view");
        return;
    }
//    qWarning() << "AccountActionHandler::setCurrentView(AccountCentralWidget *view)";

    // disconnect old view
//    if (m_CurrentView) {
//        if (view == m_CurrentView.data())
//            return;
//        m_CurrentView->disconnect();
//        disconnect(m_CurrentView->prescriptionListView()->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
//                   this, SLOT(listViewItemChanged()));
//        disconnect(m_CurrentView->currentDrugsModel(), SIGNAL(numberOfRowsChanged()),
//                   this, SLOT(drugsModelChanged()));
//        m_CurrentView->drugSelector()->disconnectFilter();
//    }
//    m_CurrentView = view;

//    DrugsDB::DrugsModel::setActiveModel(view->currentDrugsModel());
//    // reconnect some actions
//    m_CurrentView->createConnections();
//    connect(m_CurrentView->prescriptionListView()->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
//            this, SLOT(listViewItemChanged()));
//    connect(m_CurrentView->currentDrugsModel(), SIGNAL(numberOfRowsChanged()),
//            this, SLOT(drugsModelChanged()));
//    m_CurrentView->drugSelector()->connectFilter();
    updateActions();
}

void AccountActionHandler::updateActions()
{
//    listViewItemChanged();
//    drugsModelChanged();
}

