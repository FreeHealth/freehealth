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
 ***************************************************************************/

#include "viewmanager.h"
#include "stringlistmodel.h"
#include "constants.h"
#include "extendedview.h"
#include "listview.h"
#include "tableview.h"
#include "treeview.h"

#include <translationutils/constanttranslations.h>
#include <utils/log.h>

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/itheme.h>
#include <coreplugin/icore.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>

#include <QVBoxLayout>
#include <QGridLayout>
#include <QAction>
#include <QMenu>
#include <QWidget>
#include <QListView>
#include <QStringListModel>
#include <QToolButton>
#include <QToolBar>

using namespace Views;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }


/////////////////////////////////////////////////////////////////////////// List View Manager
ViewManager *ViewManager::m_Instance = 0;

ViewManager *ViewManager::instance(QObject *parent)
{
    if (!m_Instance)  {
        if (parent)
            m_Instance = new ViewManager(qApp);
        else
            m_Instance = new ViewManager(qApp);
    }
    return m_Instance;
}

ViewManager::ViewManager(QObject *parent) : ViewActionHandler(parent)
{
    connect(contextManager(), SIGNAL(contextChanged(Core::IContext*,Core::Context)),
            this, SLOT(updateContext(Core::IContext*, Core::Context)));
}

void ViewManager::updateContext(Core::IContext *object, const Core::Context &additionalContexts)
{
    Q_UNUSED(additionalContexts);
//    if (object)
//        qWarning() << "context" << object << object->context() << m_CurrentView;
    IView *view = 0;
    do {
        if (!object) {
            if (!m_CurrentView)
                return;

            m_CurrentView = 0;
            break;
        }
        view = qobject_cast<IView *>(object->widget());
        if (view) {
            if (view->property(Constants::HIDDEN_ID).isNull()) {
                view = 0;
            }
        }
        if (!view) {
            if (!m_CurrentView)
                return;

            m_CurrentView = 0;
            break;
        }

        if (view == m_CurrentView) {
            return;
        }

    } while (false);
    if (view) {
        ViewActionHandler::setCurrentView(view);
    }
}

/////////////////////////////////////////////////////////////////////////// Action Handler
ViewActionHandler::ViewActionHandler(QObject *parent) :
    QObject(parent),
    aAddRow(0),
    aRemoveRow(0),
    aDown(0),
    aUp(0),
    aEdit(0),
    aClear(0),
    m_CurrentView(0)
{
    Core::ActionManager *am = Core::ICore::instance()->actionManager();
    Core::ITheme *th = Core::ICore::instance()->theme();
//    Core::Context basicContext(Constants::C_BASIC);
    Core::Context addContext(Constants::C_BASIC_ADDREMOVE);
    Core::Context moveContext(Constants::C_BASIC_MOVE);

    // Editor's Contextual Menu
    Core::ActionContainer *editMenu = am->actionContainer(Core::Constants::M_EDIT);
    Core::ActionContainer *cmenu = am->actionContainer(Core::Constants::M_EDIT_LIST);
    if (!cmenu) {
        cmenu = am->createMenu(Core::Constants::M_EDIT_LIST);
        cmenu->appendGroup(Core::Id(Core::Constants::G_EDIT_LIST));
        cmenu->setTranslations(Trans::Constants::M_EDIT_LIST_TEXT);
        if (editMenu)
            editMenu->addMenu(cmenu, Core::Constants::G_EDIT_LIST);
    }

    QAction *a = aAddRow = new QAction(this);
    a->setObjectName("ListView.aAddRow");
    a->setIcon(th->icon(Core::Constants::ICONADD));
    Core::Command *cmd = am->registerAction(a, Core::Id(Core::Constants::A_LIST_ADD), addContext);
    cmd->setTranslations(Trans::Constants::LISTADD_TEXT);
    cmenu->addAction(cmd, Core::Id(Core::Constants::G_EDIT_LIST));
    connect(a, SIGNAL(triggered()), this, SLOT(addItem()));

    a = aRemoveRow = new QAction(this);
    a->setObjectName("ListView.aRemoveRow");
    a->setIcon(th->icon(Core::Constants::ICONREMOVE));
    cmd = am->registerAction(a, Core::Id(Core::Constants::A_LIST_REMOVE), addContext);
    cmd->setTranslations(Trans::Constants::LISTREMOVE_TEXT);
    cmenu->addAction(cmd, Core::Id(Core::Constants::G_EDIT_LIST));
    connect(a, SIGNAL(triggered()), this, SLOT(removeItem()));

    a = aDown = new QAction(this);
    a->setObjectName("ListView.aDown");
    a->setIcon(th->icon(Core::Constants::ICONMOVEDOWN));
    cmd = am->registerAction(a, Core::Id(Core::Constants::A_LIST_MOVEDOWN), moveContext);
    cmd->setTranslations(Trans::Constants::LISTMOVEDOWN_TEXT);
    cmenu->addAction(cmd, Core::Id(Core::Constants::G_EDIT_LIST));
    connect(a, SIGNAL(triggered()), this, SLOT(moveDown()));

    a = aUp = new QAction(this);
    a->setObjectName("ListView.aUp");
    a->setIcon(th->icon(Core::Constants::ICONMOVEUP));
    cmd = am->registerAction(a, Core::Id(Core::Constants::A_LIST_MOVEUP), moveContext);
    cmd->setTranslations(Trans::Constants::LISTMOVEUP_TEXT);
    cmenu->addAction(cmd, Core::Id(Core::Constants::G_EDIT_LIST));
    connect(a, SIGNAL(triggered()), this, SLOT(moveUp()));

    a = aClear = new QAction(this);
    a->setObjectName("ListView.aClear");
    a->setIcon(th->icon(Core::Constants::ICONCLEAR));
    cmd = am->registerAction(a, Core::Id(Core::Constants::A_LIST_CLEAR), moveContext);
    cmd->setTranslations(Trans::Constants::LISTCLEAR_TEXT);
    cmenu->addAction(cmd, Core::Id(Core::Constants::G_EDIT_LIST));
    // TODO: the clear action cannot be directly connected to views
}

void ViewActionHandler::setCurrentView(IView *view)
{
//    if (view)
//        qWarning() << "current view " << view << m_CurrentView;
    // disconnect old view
    if (m_CurrentView) {
        disconnect(m_CurrentView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                   this, SLOT(listViewItemChanged()));
    }
    m_CurrentView = view;
    if (!view) { // this should never be the case
        return;
    }
    // reconnect some actions
    if (m_CurrentView->selectionModel()) {
        connect(m_CurrentView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                this, SLOT(listViewItemChanged()));
    }
    updateActions();
}

void ViewActionHandler::listViewItemChanged()
{
    aUp->setEnabled(canMoveUp());
    aDown->setEnabled(canMoveDown());
}

void ViewActionHandler::updateActions()
{
    listViewItemChanged();
}

bool ViewActionHandler::canMoveUp()
{
    if (!m_CurrentView)
        return false;
    QModelIndex idx = m_CurrentView->currentIndex();
    if (!idx.isValid())
        return false;
    if (idx.row() >= 1)
        return true;
    return false;
}

bool ViewActionHandler::canMoveDown()
{
    if (!m_CurrentView)
        return false;
    QModelIndex idx = m_CurrentView->currentIndex();
    if (!idx.isValid())
        return false;
    if (idx.row() < (idx.model()->rowCount()-1))
        return true;
    return false;
}

void ViewActionHandler::moveUp()
{
    if (m_CurrentView) {
        ListView *view = qobject_cast<ListView*>(m_CurrentView);
        if (view) {
            view->moveUp();
            return;
        }
        TableView *tview = qobject_cast<TableView*>(m_CurrentView);
        if (tview) {
            tview->moveUp();
            return;
        }
        TreeView *trview = qobject_cast<TreeView*>(m_CurrentView);
        if (trview) {
            trview->moveUp();
            return;
        }
    }
}

void ViewActionHandler::moveDown()
{
    if (m_CurrentView) {
        ListView *view = qobject_cast<ListView*>(m_CurrentView);
        if (view) {
            view->moveDown();
            return;
        }
        TableView *tview = qobject_cast<TableView*>(m_CurrentView);
        if (tview) {
            tview->moveDown();
            return;
        }
        TreeView *trview = qobject_cast<TreeView*>(m_CurrentView);
        if (trview) {
            trview->moveDown();
            return;
        }
    }
}

void ViewActionHandler::addItem()
{
    if (m_CurrentView) {
        ListView *view = qobject_cast<ListView*>(m_CurrentView);
        if (view) {
            view->addItem();
            return;
        }
        TableView *tview = qobject_cast<TableView*>(m_CurrentView);
        if (tview) {
            tview->addItem();
            return;
        }
        TreeView *trview = qobject_cast<TreeView*>(m_CurrentView);
        if (trview) {
            trview->addItem();
            return;
        }
    }
}

void ViewActionHandler::removeItem()
{
    if (m_CurrentView) {
        ListView *view = qobject_cast<ListView*>(m_CurrentView);
        if (view) {
            view->removeItem();
            return;
        }
        TableView *tview = qobject_cast<TableView*>(m_CurrentView);
        if (tview) {
            tview->removeItem();
            return;
        }
        TreeView *trview = qobject_cast<TreeView*>(m_CurrentView);
        if (trview) {
            trview->removeItem();
            return;
        }
    }
}


