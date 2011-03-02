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
#include "accountcontextualwidget.h"

#include <accountreceiptsplugin/receiptviewer.h>

#include <accountmanagerplugin/assets/assetsViewer.h>
#include <accountmanagerplugin/ledger/ledgerviewer.h>
#include <accountmanagerplugin/movements/movementsviewer.h>

#include <coreplugin/constants.h>
#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/uniqueidmanager.h>

#include <utils/log.h>
#include <translationutils/constanttranslations.h>

using namespace Account::Constants;
using namespace Account;
using namespace Account::Internal;
using namespace Trans::ConstantTranslations;

inline static Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}
static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }
static inline Core::IMainWindow *mainWindow() { return Core::ICore::instance()->mainWindow(); }

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

    AccountContextualWidget *view = 0;
    do {
        if (!object) {
            if (!m_CurrentView)
                return;

//            m_CurrentView = 0;  // keep trace of the last active view (we need it in dialogs)
            break;
        }
        view = qobject_cast<AccountContextualWidget *>(object->widget());
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
        AccountActionHandler::setCurrentView(view);
    }
}

AccountContextualWidget *AccountWidgetManager::currentView() const
{
    return AccountActionHandler::m_CurrentView;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////  ACTION HANDLER   ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
AccountActionHandler::AccountActionHandler(QObject *parent) :
        QObject(parent),
        aAddReceipts(0),
        aReceipts(0),
        aLegder(0),
        aMovements(0),
        aAssets(0),
        m_CurrentView(0)
{
    setObjectName("AccountActionHandler");
    Utils::Log::addMessage(this, "Instance created");

    Core::UniqueIDManager *uid = Core::ICore::instance()->uniqueIDManager();
    Core::ITheme *th = Core::ICore::instance()->theme();

    QAction *a = 0;
    Core::Command *cmd = 0;
    QList<int> ctx = QList<int>() << uid->uniqueIdentifier(Account::Constants::C_ACCOUNT);
    QList<int> global = QList<int>() << Core::Constants::C_GLOBAL_ID;

    Core::ActionContainer *menu = actionManager()->actionContainer(Account::Constants::M_PLUGINS_ACCOUNT);
    if (!menu) {
        menu = actionManager()->createMenu(Account::Constants::M_PLUGINS_ACCOUNT);
        menu->appendGroup(Constants::G_ACCOUNT_APPS);
        menu->appendGroup(Constants::G_ACCOUNT_SEARCH);
        menu->appendGroup(Constants::G_ACCOUNT_MODES);
        menu->setTranslations(Account::Constants::ACCOUNTMENU_TEXT, Account::Constants::ACCOUNT_TR_CONTEXT);
    }
    Q_ASSERT(menu);
#ifdef FREEACCOUNT
    actionManager()->actionContainer(Core::Constants::MENUBAR)->addMenu(menu, Core::Constants::G_PLUGINS);
#else
    actionManager()->actionContainer(Core::Constants::M_PLUGINS)->addMenu(menu, Core::Constants::G_PLUGINS_ACCOUNT);
#endif

    // Create local actions
    a = aAddReceipts = new QAction(this);
    a->setObjectName("aAddReceipts");
    a->setIcon(th->icon(Core::Constants::ICONHELP));
    cmd = actionManager()->registerAction(a, Constants::A_ADDRECEIPTS, global);
    cmd->setTranslations(Constants::ADD_RECEIPTS, Constants::ADD_RECEIPTS, Constants::ACCOUNT_TR_CONTEXT);
    menu->addAction(cmd, Constants::G_ACCOUNT_APPS);
    connect(a, SIGNAL(triggered()), this, SLOT(addReceipts()));

    a = aReceipts = new QAction(this);
    a->setObjectName("aReceipts");
    a->setIcon(th->icon(Core::Constants::ICONHELP));
    cmd = actionManager()->registerAction(a, Constants::A_RECEIPTS, global);
    cmd->setTranslations(Constants::RECEIPTS, Constants::RECEIPTS, Constants::ACCOUNT_TR_CONTEXT);
    menu->addAction(cmd, Constants::G_ACCOUNT_APPS);
    connect(a, SIGNAL(triggered()), this, SLOT(receipts()));

    a = aLegder = new QAction(this);
    a->setObjectName("aLegder");
    a->setIcon(th->icon(Core::Constants::ICONHELP));
    cmd = actionManager()->registerAction(a, Constants::A_LEDGER, global);
    cmd->setTranslations(Constants::LEDGER, Constants::LEDGER, Constants::ACCOUNT_TR_CONTEXT);
    menu->addAction(cmd, Constants::G_ACCOUNT_APPS);
    connect(a, SIGNAL(triggered()), this, SLOT(ledger()));

    a = aMovements = new QAction(this);
    a->setObjectName("aMovements");
    a->setIcon(th->icon(Core::Constants::ICONHELP));
    cmd = actionManager()->registerAction(a, Constants::A_MOVEMENTS, global);
    cmd->setTranslations(Constants::MOVEMENTS, Constants::MOVEMENTS, Constants::ACCOUNT_TR_CONTEXT);
    menu->addAction(cmd, Constants::G_ACCOUNT_APPS);
    connect(a, SIGNAL(triggered()), this, SLOT(movements()));

    a = aAssets = new QAction(this);
    a->setObjectName("aAssets");
    a->setIcon(th->icon(Core::Constants::ICONHELP));
    cmd = actionManager()->registerAction(a, Constants::A_ASSETS, global);
    cmd->setTranslations(Constants::ASSETS, Constants::ASSETS, Constants::ACCOUNT_TR_CONTEXT);
    menu->addAction(cmd, Constants::G_ACCOUNT_APPS);
    connect(a, SIGNAL(triggered()), this, SLOT(assets()));

    contextManager()->updateContext();
    actionManager()->retranslateMenusAndActions();
}

void AccountActionHandler::setCurrentView(AccountContextualWidget *view)
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
}

void AccountActionHandler::addReceipts()
{

}

void AccountActionHandler::receipts()
{
    QWidget *w = mainWindow()->centralWidget();
    delete w;
    w = 0;
    mainWindow()->setCentralWidget(new ReceiptViewer(mainWindow()));
}

void AccountActionHandler::ledger()
{
    QWidget *w = mainWindow()->centralWidget();
    delete w;
    w = 0;
    mainWindow()->setCentralWidget(new LedgerViewer(mainWindow()));
}

void AccountActionHandler::movements()
{
    QWidget *w = mainWindow()->centralWidget();
    delete w;
    w = 0;
    mainWindow()->setCentralWidget(new MovementsViewer(mainWindow()));
}

void AccountActionHandler::assets()
{
    QWidget *w = mainWindow()->centralWidget();
    delete w;
    w = 0;
    mainWindow()->setCentralWidget(new AssetsViewer(mainWindow()));
}
