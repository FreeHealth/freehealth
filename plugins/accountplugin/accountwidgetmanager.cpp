/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  The FreeAccount plugins are free, open source FreeMedForms' plugins.   *
 *  (C) 2010-2011 by Pierre-Marie Desombre, MD <pm.desombre@medsyn.fr>     *
 *  and Eric Maeker, MD <eric.maeker@gmail.com>                            *
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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developers:  Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#include "accountwidgetmanager.h"
#include "constants.h"
#include "accountcontextualwidget.h"

#include <accountplugin/receipts/preferredreceipts.h>
#include <accountplugin/receipts/receiptviewer.h>
#include <accountplugin/assets/assetsviewer.h>
#include <accountplugin/ledger/ledgerviewer.h>
#include <accountplugin/movements/movementsviewer.h>
#include <accountplugin/accountview.h>

#ifdef FREEHEALTH
#    include <accountplugin/accountmode.h>
#endif

#include <coreplugin/constants.h>
#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/modemanager/modemanager.h>
#include <coreplugin/modemanager/imode.h>

#include <utils/log.h>
#include <translationutils/constanttranslations.h>

using namespace Account::Constants;
using namespace Account;
using namespace Account::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}
static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }
static inline Core::IMainWindow *mainWindow() { return Core::ICore::instance()->mainWindow(); }
static inline Core::ModeManager *modeManager() {return Core::ICore::instance()->modeManager();}
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }

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

AccountWidgetManager::~AccountWidgetManager()
{
}


AccountWidgetManager::AccountWidgetManager(QObject *parent) :
    AccountActionHandler(parent)
{
    connect(Core::ICore::instance()->contextManager(), SIGNAL(contextChanged(Core::IContext*,Core::Context)),
            this, SLOT(updateContext(Core::IContext*,Core::Context)));
    setObjectName("AccountWidgetManager");
}

void AccountWidgetManager::updateContext(Core::IContext *object, const Core::Context &additionalContexts)
{
    Q_UNUSED(additionalContexts);
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
        aAccount(0),
        m_CurrentView(0)
{
    setObjectName("AccountActionHandler");
    LOG("Instance created");
    QAction *a = 0;
    Core::Command *cmd = 0;
    Core::Context ctx(Account::Constants::C_ACCOUNT);
    Core::Context global(Core::Constants::C_GLOBAL);

    Core::ActionContainer *menu = actionManager()->actionContainer(Core::Id(Account::Constants::M_PLUGINS_ACCOUNT));
    if (!menu) {
        menu = actionManager()->createMenu(Account::Constants::M_PLUGINS_ACCOUNT);
        menu->appendGroup(Core::Id(Constants::G_ACCOUNT_APPS));
        menu->appendGroup(Core::Id(Constants::G_ACCOUNT_SEARCH));
        menu->appendGroup(Core::Id(Constants::G_ACCOUNT_MODES));
        menu->setTranslations(Account::Constants::ACCOUNTMENU_TEXT, Account::Constants::ACCOUNT_TR_CONTEXT);
    }
    Q_ASSERT(menu);
#ifdef FREEACCOUNT
    actionManager()->actionContainer(Core::Id(Core::Constants::MENUBAR))->addMenu(menu, Core::Id(Core::Constants::G_PLUGINS));
#else
    actionManager()->actionContainer(Core::Id(Core::Constants::M_PLUGINS))->addMenu(menu, Core::Id(Core::Constants::G_PLUGINS_ACCOUNT));
#endif

    // Create local actions
    a = aAddReceipts = new QAction(this);
    a->setObjectName("aAddReceipts");
    a->setIcon(theme()->icon(Core::Constants::ICONHELP));
    cmd = actionManager()->registerAction(a, Core::Id(Constants::A_ADDRECEIPTS), global);
    cmd->setTranslations(Constants::ADD_RECEIPTS, Constants::ADD_RECEIPTS, Constants::ACCOUNT_TR_CONTEXT);
    cmd->setDefaultKeySequence(QKeySequence("Ctrl+r"));
    menu->addAction(cmd, Core::Id(Constants::G_ACCOUNT_APPS));
    connect(a, SIGNAL(triggered()), this, SLOT(addReceipts()));

    a = aReceipts = new QAction(this);
    a->setObjectName("aReceipts");
    a->setIcon(theme()->icon(Core::Constants::ICONHELP));
    cmd = actionManager()->registerAction(a, Core::Id(Constants::A_RECEIPTS), global);
    cmd->setTranslations(Constants::RECEIPTS, Constants::RECEIPTS, Constants::ACCOUNT_TR_CONTEXT);
    cmd->setDefaultKeySequence(QKeySequence("Shift+r"));
    menu->addAction(cmd, Core::Id(Constants::G_ACCOUNT_APPS));
    connect(a, SIGNAL(triggered()), this, SLOT(receipts()));

    a = aLegder = new QAction(this);
    a->setObjectName("aLegder");
    a->setIcon(theme()->icon(Core::Constants::ICONHELP));
    cmd = actionManager()->registerAction(a, Core::Id(Constants::A_LEDGER), global);
    cmd->setTranslations(Constants::LEDGER, Constants::LEDGER, Constants::ACCOUNT_TR_CONTEXT);
    cmd->setDefaultKeySequence(QKeySequence("Alt+l"));
    menu->addAction(cmd, Core::Id(Constants::G_ACCOUNT_APPS));
    connect(a, SIGNAL(triggered()), this, SLOT(ledger()));

    a = aMovements = new QAction(this);
    a->setObjectName("aMovements");
    a->setIcon(theme()->icon(Core::Constants::ICONHELP));
    cmd = actionManager()->registerAction(a, Core::Id(Constants::A_MOVEMENTS), global);
    cmd->setTranslations(Constants::MOVEMENTS, Constants::MOVEMENTS, Constants::ACCOUNT_TR_CONTEXT);
    cmd->setDefaultKeySequence(QKeySequence("Ctrl+m"));
    menu->addAction(cmd, Core::Id(Constants::G_ACCOUNT_APPS));
    connect(a, SIGNAL(triggered()), this, SLOT(movements()));

    a = aAssets = new QAction(this);
    a->setObjectName("aAssets");
    a->setIcon(theme()->icon(Core::Constants::ICONHELP));
    cmd = actionManager()->registerAction(a, Core::Id(Constants::A_ASSETS), global);
    cmd->setTranslations(Constants::ASSETS, Constants::ASSETS, Constants::ACCOUNT_TR_CONTEXT);
    cmd->setDefaultKeySequence(QKeySequence("Alt+z"));
    menu->addAction(cmd, Core::Id(Constants::G_ACCOUNT_APPS));
    connect(a, SIGNAL(triggered()), this, SLOT(assets()));

    a = aAccount = new QAction(this);
    a->setObjectName("aAccount");
    a->setIcon(theme()->icon(Core::Constants::ICONHELP));
    cmd = actionManager()->registerAction(a, Core::Id(Constants::A_ACCOUNT), global);
    cmd->setTranslations(Constants::ACCOUNT, Constants::ACCOUNT, Constants::ACCOUNT_TR_CONTEXT);
    cmd->setDefaultKeySequence(QKeySequence("Alt+A"));
    menu->addAction(cmd, Core::Id(Constants::G_ACCOUNT_APPS));
    connect(a, SIGNAL(triggered()), this, SLOT(account()));

    contextManager()->updateContext();
    actionManager()->retranslateMenusAndActions();
}

void AccountActionHandler::setCurrentView(AccountContextualWidget *view)
{
    Q_ASSERT(view);
    if (!view) { // this should never be the case
        LOG_ERROR("setCurrentView: no view");
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

#ifndef FREEMEDFORMS
void AccountActionHandler::addReceipts()
{
    QWidget *w = mainWindow()->centralWidget();
    delete w;
    w = 0;
    mainWindow()->setCentralWidget(new PreferredReceipts(mainWindow()));
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

void AccountActionHandler::account(){
    QWidget *w = mainWindow()->centralWidget();
    delete w;
    w = 0;
    mainWindow()->setCentralWidget(new AccountView(mainWindow()));
}
#else
void AccountActionHandler::addReceipts()
{
    AccountMode *accountMode = qobject_cast<AccountMode*>(modeManager()->mode(Core::Constants::MODE_ACCOUNT));
    Q_ASSERT(accountMode);
    accountMode->showAddReceipts();
}

void AccountActionHandler::receipts()
{
    AccountMode *accountMode = qobject_cast<AccountMode*>(modeManager()->mode(Core::Constants::MODE_ACCOUNT));
    Q_ASSERT(accountMode);
    accountMode->showReceipts();
}

void AccountActionHandler::ledger()
{
    AccountMode *accountMode = qobject_cast<AccountMode*>(modeManager()->mode(Core::Constants::MODE_ACCOUNT));
    Q_ASSERT(accountMode);
    accountMode->showLedger();
}

void AccountActionHandler::movements()
{
    AccountMode *accountMode = qobject_cast<AccountMode*>(modeManager()->mode(Core::Constants::MODE_ACCOUNT));
    Q_ASSERT(accountMode);
    accountMode->showMovements();
}

void AccountActionHandler::assets()
{
    AccountMode *accountMode = qobject_cast<AccountMode*>(modeManager()->mode(Core::Constants::MODE_ACCOUNT));
    Q_ASSERT(accountMode);
    accountMode->showAssets();
}

void AccountActionHandler::account()
{
    AccountMode *accountMode = qobject_cast<AccountMode*>(modeManager()->mode(Core::Constants::MODE_ACCOUNT));
    Q_ASSERT(accountMode);
    accountMode->showAccount();
}
#endif

