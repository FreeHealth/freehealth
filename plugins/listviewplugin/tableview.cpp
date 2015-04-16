/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "tableview.h"
#include "constants.h"
#include "viewmanager.h"

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/icore.h>

#include <QLayout>

using namespace Views;
using namespace Internal;

static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }

namespace Views {
namespace Internal {
class TableViewPrivate
{
public:
    TableViewPrivate(QWidget *parent, Constants::AvailableActions actions) :
        m_Parent(parent),
        m_TableView(0),
        m_Actions(actions),
        m_Context(0),
        m_ExtView(0)
    {
    }

    ~TableViewPrivate()
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
    QTableView *m_TableView;
    Constants::AvailableActions m_Actions;
    ViewContext *m_Context;
    QToolBar *m_ToolBar;
    QString m_ContextName;
    ExtendedView *m_ExtView;
};

}  // End Internal
}  // End Views


/** \brief Constructor */
TableView::TableView(QWidget *parent, Constants::AvailableActions actions) :
    IView(parent),
    d(0)
{
    static int handler = 0;
    ++handler;
    QObject::setObjectName("TableView_"+QString::number(handler));
    setProperty(Constants::HIDDEN_ID, "xx");
    d = new Internal::TableViewPrivate(this, actions);

    // Create the listview && the widget content
    d->m_TableView = new QTableView(this);
    setItemView(d->m_TableView);

    // Create the Manager instance and context
    ViewManager::instance();
    d->m_Context = new ViewContext(this);
    d->calculateContext();
    contextManager()->addContextObject(d->m_Context);

    // Create the extension of the view
    d->m_ExtView = new ExtendedView(this);
}

TableView::~TableView()
{
    contextManager()->removeContextObject(d->m_Context);
    if (d)
        delete d;
    d = 0;
}

QAbstractItemView *TableView::itemView() const
{
    return d->m_TableView;
}

QTableView *TableView::tableView() const
{
    return d->m_TableView;
}

void TableView::setActions(Constants::AvailableActions actions)
{
    d->m_Actions = actions;
    d->calculateContext();
    d->m_ExtView->setActions(actions);
}

void TableView::setCommands(const QStringList &commandsUid)
{
    d->m_Actions = 0;
    d->calculateContext();
    d->m_ExtView->setCommands(commandsUid);
}

void TableView::addContext(const Core::Context &context)
{
    Core::Context current = d->m_Context->context();
    current.add(context);
    d->m_Context->setContext(current);
}

void TableView::hideButtons() const
{
    d->m_ExtView->hideButtons();
}

void TableView::showButtons()
{
    d->m_ExtView->showButtons();
}

void TableView::useContextMenu(bool state)
{
    d->m_ExtView->useContextMenu(state);
}

void TableView::addItem()
{
    setFocus();
    d->m_ExtView->addItem();
}

void TableView::removeItem()
{
    setFocus();
    d->m_ExtView->removeItem();
}

void TableView::moveDown()
{
    setFocus();
    d->m_ExtView->moveDown();
}

void TableView::moveUp()
{
    setFocus();
    d->m_ExtView->moveUp();
}

