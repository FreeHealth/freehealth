/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "treeview.h"

#include "constants.h"
#include "viewmanager.h"

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/icore.h>
#include <coreplugin/uniqueidmanager.h>


using namespace Views;
using namespace Internal;

static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }
static inline Core::ActionManager *actionManager() { return Core::ICore::instance()->actionManager(); }


namespace Views {
namespace Internal {

class TreeViewPrivate
{
public:
    TreeViewPrivate(QWidget *parent, Constants::AvailableActions actions) :
            m_Parent(parent),
            m_Actions(actions),
            m_Context(0),
            m_ExtView(0)
    {
    }

    ~TreeViewPrivate()
    {
        if (m_ExtView)
            delete m_ExtView;
        m_ExtView = 0;
    }

    void calculateContext()
    {
        m_Context->clearContext();
        if (m_Actions & Constants::MoveUpDown)
            m_Context->addContext(Core::ICore::instance()->uniqueIDManager()->uniqueIdentifier(Constants::C_BASIC_MOVE));

        if (m_Actions & Constants::AddRemove)
            m_Context->addContext(Core::ICore::instance()->uniqueIDManager()->uniqueIdentifier(Constants::C_BASIC_ADDREMOVE));
    }

public:
    QWidget *m_Parent;
    Constants::AvailableActions m_Actions;
    ViewContext *m_Context;
    QString m_ContextName;
    ExtendedView *m_ExtView;
};

}  // End Internal
}  // End Views


/** \brief Constructor */
TreeView::TreeView(QWidget *parent, Constants::AvailableActions actions) :
        DeselectableTreeView(parent),
        d(0)
{
    static int handler = 0;
    ++handler;
    QObject::setObjectName("TreeView_"+QString::number(handler));
    setProperty(Constants::HIDDEN_ID, "xx");
    d = new Internal::TreeViewPrivate(this, actions);

    // Create the Manager instance and context
    ViewManager::instance();
    d->m_Context = new ViewContext(this);
    d->calculateContext();
    contextManager()->addContextObject(d->m_Context);

    // Create the extension of the view
    d->m_ExtView = new ExtendedView(this);
}

TreeView::~TreeView()
{
    contextManager()->removeContextObject(d->m_Context);
}

void TreeView::setActions(Constants::AvailableActions actions)
{
    d->m_Actions = actions;
    d->calculateContext();
    d->m_ExtView->setActions(actions);
}

void TreeView::setCommands(const QStringList &commandsUid)
{
    d->m_Actions = 0;
    d->m_ExtView->setActions(0);
    d->calculateContext();
    d->m_ExtView->setCommands(commandsUid);
}

void TreeView::addContext(const int id)
{
    d->m_Context->addContext(id);
}

void TreeView::addContexts(const QList<int> &id)
{
    for(int i = 0; i < id.count(); ++i) {
        d->m_Context->addContext(id.at(i));
    }
}

void TreeView::disconnectActionsToDefaultSlots()
{
    d->m_ExtView->disconnectActionsToDefaultSlots();
}

void TreeView::hideButtons() const
{
    d->m_ExtView->hideButtons();
}

void TreeView::showButtons()
{
    d->m_ExtView->showButtons();
}

void TreeView::useContextMenu(bool state)
{
    d->m_ExtView->useContextMenu(state);
}

void TreeView::addItem()
{
    Q_EMIT addRequested();
    Q_EMIT aboutToAddItem();
    setFocus();
    d->m_ExtView->addItem(true);
    Q_EMIT itemAdded();
}

void TreeView::removeItem()
{
    Q_EMIT removeRequested();
    Q_EMIT aboutToRemove();
    setFocus();
    d->m_ExtView->removeItem();
    Q_EMIT itemRemoved();
}

void TreeView::moveDown()
{
    setFocus();
    d->m_ExtView->moveDown();
    Q_EMIT moveDownRequested();
}

void TreeView::moveUp()
{
    setFocus();
    d->m_ExtView->moveUp();
    Q_EMIT moveUpRequested();
}

