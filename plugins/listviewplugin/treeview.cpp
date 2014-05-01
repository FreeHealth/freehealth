/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "treeview.h"

#include "constants.h"
#include "viewmanager.h"

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/icore.h>

using namespace Views;
using namespace Internal;

static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }

namespace Views {
namespace Internal {
class TreeViewPrivate
{
public:
    TreeViewPrivate(QWidget *parent, Constants::AvailableActions actions) :
        m_Parent(parent),
        m_TreeView(0),
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
        Core::Context context;
        if (m_Actions & Constants::MoveUpDown)
            context.add(Constants::C_BASIC_MOVE);

        if (m_Actions & Constants::AddRemove)
            context.add(Constants::C_BASIC_ADDREMOVE);

        m_Context->setContext(context);
    }

public:
    QWidget *m_Parent;
    QTreeView *m_TreeView;
    Constants::AvailableActions m_Actions;
    ViewContext *m_Context;
    QString m_ContextName;
    ExtendedView *m_ExtView;
    bool m_IsDeselectable;
};

}  // End Internal
}  // End Views


/** \brief Constructor */
TreeView::TreeView(QWidget *parent, Constants::AvailableActions actions) :
        IView(parent),
        d(0)
{
    static int handler = 0;
    ++handler;
    QObject::setObjectName("TreeView_"+QString::number(handler));
    setProperty(Constants::HIDDEN_ID, "xx");
    d = new Internal::TreeViewPrivate(this, actions);
    d->m_IsDeselectable = false;

    // Create the treeview && the widget content
    d->m_TreeView = new QTreeView(this);
    setItemView(d->m_TreeView);
    d->m_TreeView->viewport()->installEventFilter(this);

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
    if (d)
        delete d;
    d = 0;
}

QAbstractItemView *TreeView::itemView() const
{
    return d->m_TreeView;
}

QTreeView *TreeView::treeView() const
{
    return d->m_TreeView;
}

void TreeView::setDeselectable(bool deselectable)
{
    d->m_IsDeselectable = deselectable;
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

void TreeView::addContext(const Core::Context &context)
{
    Core::Context current = d->m_Context->context();
    current.add(context);
    d->m_Context->setContext(current);
}

void TreeView::disconnectActionsToDefaultSlots()
{
    d->m_ExtView->disconnectActionsFromDefaultSlots();
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

bool TreeView::eventFilter(QObject *o, QEvent *event)
{
    if (o!=d->m_TreeView->viewport())
        return QWidget::eventFilter(o, event);

    if (d->m_IsDeselectable && selectionModel() && event->type() == QEvent::MouseButtonPress) {
        QWidget::eventFilter(o, event);
        QMouseEvent *me = static_cast<QMouseEvent*>(event);
        QModelIndex item = indexAt(me->pos());
        bool selected = selectionModel()->isSelected(indexAt(me->pos()));
//        IView::mousePressEvent(event);
        if (selected) {
            if (selectionBehavior()==QAbstractItemView::SelectItems) {
                selectionModel()->select(item, QItemSelectionModel::Deselect);
            } else if (selectionBehavior()==QAbstractItemView::SelectRows) {
                //                    QModelIndexList items = selectionModel()->selectedColumns(item.row());
                for(int i=0; i<selectionModel()->model()->columnCount(item); ++i) {
                    selectionModel()->select(selectionModel()->model()->index(item.row(), i, item.parent()), QItemSelectionModel::Deselect);
                }
            } else if (selectionBehavior()==QAbstractItemView::SelectColumns) {
                for(int i=0; i<selectionModel()->model()->rowCount(item); ++i) {
                    selectionModel()->select(selectionModel()->model()->index(i, item.column(), item.parent()), QItemSelectionModel::Deselect);
                }
            }
        }
    }

    return QWidget::eventFilter(o, event);
}
