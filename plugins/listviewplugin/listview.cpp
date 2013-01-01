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
 ***************************************************************************/
/**
  * \class Views::ListView
  * \brief This widget shows a QListView with some buttons and a context menu.
  * Actions inside the contextual menu and the toolbar can be set using
    AvailableActions param and setActions().\n
  * You can reimplement addItem(), removeItem() and on_edit_triggered()
    that are called by buttons and menu (add, remove). \n
*/
//TODO: There is a problem when including this widget into a QDataWidgetMapper, when this widget loses focus, data are not retreived.

#include "listview.h"
#include "stringlistmodel.h"
#include "constants.h"
#include "viewmanager.h"

#include <translationutils/constanttranslations.h>
#include <utils/log.h>

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/itheme.h>
#include <coreplugin/icore.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>

#include <QLayout>
#include <QAction>
#include <QMenu>
#include <QWidget>
#include <QListView>
#include <QStringListModel>
#include <QToolButton>
#include <QToolBar>
#include <QToolTip>

using namespace Views;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ActionManager *actionManager() { return Core::ICore::instance()->actionManager(); }
static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }


/////////////////////////////////////////////////////////////////////////// List View
namespace Views {
namespace Internal {

class ListViewPrivate
{
public:
    ListViewPrivate(QWidget *parent, Constants::AvailableActions actions) :
        m_Parent(parent),
        m_ListView(0),
        m_Actions(actions),
        m_Context(0),
        m_ExtView(0),
        m_MaxRows(-1)
    {
    }

    ~ListViewPrivate()
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
    QListView *m_ListView;
    Constants::AvailableActions m_Actions;
    ViewContext *m_Context;
    QToolBar *m_ToolBar;
    QString m_ContextName;
    ExtendedView *m_ExtView;
    int m_MaxRows;
};

}  // End Internal
}  // End Views


/** \brief Constructor */
ListView::ListView(QWidget *parent, Constants::AvailableActions actions) :
    IView(parent),
    d(0)
{
    static int handler = 0;
    ++handler;
    QObject::setObjectName("ListView_"+QString::number(handler));
    setProperty(Constants::HIDDEN_ID, "xx");
    d = new Internal::ListViewPrivate(this, actions);

    // Create the listview && the widget content
    d->m_ListView = new QListView(this);
    setItemView(d->m_ListView);

    // Create the Manager instance and context
    d->m_Context = new ViewContext(this);
    d->calculateContext();
    contextManager()->addContextObject(d->m_Context);

    // Create the extension of the view
    d->m_ExtView = new ExtendedView(this, actions);
}

ListView::~ListView()
{
    contextManager()->removeContextObject(d->m_Context);
    if (d) {
        delete d;
        d = 0;
    }
}

QAbstractItemView *ListView::itemView() const
{
    return d->m_ListView;
}

void ListView::setModelColumn(int column)
{
    d->m_ListView->setModelColumn(column);
}

void ListView::setActions(Constants::AvailableActions actions)
{
    d->m_Actions = actions;
    d->calculateContext();
    d->m_ExtView->setActions(actions);
}

void ListView::setCommands(const QStringList &commandsUid)
{
    d->m_Actions = 0;
    d->calculateContext();
    d->m_ExtView->setCommands(commandsUid);
}

void ListView::addContext(const Core::Context &context)
{
    Core::Context current = d->m_Context->context();
    current.add(context);
    d->m_Context->setContext(current);
}

void ListView::hideButtons() const
{
    d->m_ExtView->hideButtons();
}

void ListView::showButtons()
{
    d->m_ExtView->showButtons();
}

void ListView::useContextMenu(bool state)
{
    d->m_ExtView->useContextMenu(state);
}

/** Limit the number of row (the user can add to the view/model). */
void ListView::setMaximumRows(int max)
{
    d->m_MaxRows = max;
}

int ListView::maximumRows() const
{
    return d->m_MaxRows;
}

void ListView::addItem()
{
    setFocus();
    bool add = true;
    if (d->m_MaxRows > 0) {
        if (model()->rowCount() >= d->m_MaxRows) {
            // FIXME: improve tooltip position & rect
            QPoint pos = itemView()->mapToGlobal(itemView()->rect().bottomLeft());
            QToolTip::showText(pos - QPoint(0,32),
                               tr("Unable to add a new line, you have reached "
                                  "the maximum autorized lines."),
                               itemView());
            add = false;
        }
    }
    if (add) {
        Q_EMIT addRequested();
        Q_EMIT aboutToAddItem();
        d->m_ExtView->addItem();
        Q_EMIT itemAdded();
    }
}

void ListView::removeItem()
{
    Q_EMIT removeRequested();
    Q_EMIT aboutToRemove();
    setFocus();
    d->m_ExtView->removeItem();
    Q_EMIT itemRemoved();
}

void ListView::moveDown()
{
    setFocus();
    d->m_ExtView->moveDown();
}

void ListView::moveUp()
{
    setFocus();
    d->m_ExtView->moveUp();
}

