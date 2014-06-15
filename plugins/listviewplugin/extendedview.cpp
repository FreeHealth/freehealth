/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "extendedview.h"
#include "viewmanager.h"
#include "stringlistmodel.h"

#include <translationutils/constanttranslations.h>
#include <utils/log.h>
#include <utils/global.h>

#include <coreplugin/actionmanager/actionmanager.h>
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
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>

using namespace Views;
using namespace Views::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ActionManager *actionManager() { return Core::ICore::instance()->actionManager(); }

IView::IView(QWidget *parent) :
    QWidget(parent)
{
    // Create the listview && the widget content
    QVBoxLayout *lay = new QVBoxLayout(this);
    lay->setMargin(0);
    lay->setSpacing(0);
    setLayout(lay);
}

void IView::addToolBar(QToolBar *bar)
{
    Q_ASSERT(bar);
    if (!bar)
        return;
    if (m_AddedToolBars.contains(bar))
        return;
    m_AddedToolBars << bar;
    layout()->addWidget(bar);
}

void IView::setItemView(QAbstractItemView *view)
{
    layout()->addWidget(view);
    // Connect signal overload
    connect(view, SIGNAL(activated(QModelIndex)), this, SIGNAL(activated(QModelIndex)));
    connect(view, SIGNAL(clicked(QModelIndex)), this, SIGNAL(clicked(QModelIndex)));
    connect(view, SIGNAL(doubleClicked(QModelIndex)), this, SIGNAL(doubleClicked(QModelIndex)));
    connect(view, SIGNAL(entered(QModelIndex)), this, SIGNAL(entered(QModelIndex)));
    connect(view, SIGNAL(pressed(QModelIndex)), this, SIGNAL(pressed(QModelIndex)));
    connect(view, SIGNAL(viewportEntered()), this, SIGNAL(viewportEntered()));
}

namespace Views {
namespace Internal {
class ExtendedViewPrivate
{
public:
    ExtendedViewPrivate(IView *parent, Constants::AvailableActions actions) :
        m_Parent(parent),
        m_Actions(actions),
        m_DefaultSlots(true)
    {
    }

    ~ExtendedViewPrivate()
    {
    }

    void populateToolbar()
    {
        Core::ActionManager *am = Core::ICore::instance()->actionManager();
        m_ToolBar->clear();
        if (m_Actions & Constants::AddRemove) {
            Core::Command *cmd = am->command(Core::Constants::A_LIST_ADD);
            m_ToolBar->addAction(cmd->action());
            cmd = am->command(Core::Constants::A_LIST_REMOVE);
            m_ToolBar->addAction(cmd->action());
        }

        if (m_Actions & Constants::MoveUpDown) {
            Core::Command *cmd = am->command(Core::Constants::A_LIST_MOVEUP);
            m_ToolBar->addAction(cmd->action());
            cmd = am->command(Core::Constants::A_LIST_MOVEDOWN);
            m_ToolBar->addAction(cmd->action());
        }
    }

public:
    IView *m_Parent;
    Constants::AvailableActions m_Actions;
    QToolBar *m_ToolBar;
    QString m_ContextName;
    bool m_DefaultSlots;
};

}  // End Internal
}  // End Views


/** \brief Constructor */
ExtendedView::ExtendedView(IView *parent, Constants::AvailableActions actions) :
    d(0)
{
//    static int handler = 0;
//    ++handler;
//    QObject::setObjectName("ExtendedView_"+QString::number(handler));
    d = new Internal::ExtendedViewPrivate(parent, actions);

    // Add toolbar to the horizontal scroolbar
    ViewManager::instance();
    d->m_ToolBar = new QToolBar(parent);
    d->m_ToolBar->setMinimumHeight(20);
    d->m_ToolBar->setIconSize(QSize(16,16));
    d->m_ToolBar->setFocusPolicy(Qt::ClickFocus);
    d->m_ToolBar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    d->populateToolbar();
    parent->addToolBar(d->m_ToolBar);
//    parent->addScrollBarWidget(d->m_ToolBar, Qt::AlignLeft);
//    if (!Utils::isRunningOnMac()) {
//        QWidget *w = parent->scrollBarWidgets(Qt::AlignLeft).at(0);
//        w->layout()->setMargin(0);
//        w->layout()->setSpacing(0);
//    }

    // Manage context menu
    parent->setContextMenuPolicy(Qt::CustomContextMenu);
//    connect(parent, SIGNAL(customContextMenuRequested(const QPoint &)),
//            this, SLOT(contextMenu(const QPoint &)));
}

ExtendedView::~ExtendedView()
{
    if (d)
        delete d;
    d = 0;
}

void ExtendedView::setActions(Constants::AvailableActions actions)
{
    d->m_Actions = actions;
    d->populateToolbar();
}

void ExtendedView::setCommands(const QStringList &commandsUid)
{
    d->m_Actions = 0;
    foreach(const QString &uid, commandsUid) {
        if (uid=="--") {
            d->m_ToolBar->addSeparator();
            continue;
        } else if (uid=="->") {
            QWidget *w = new QWidget(d->m_ToolBar);
            w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            w->setLayout(new QHBoxLayout(w));
            w->layout()->addItem(new QSpacerItem(10,10, QSizePolicy::Expanding, QSizePolicy::Expanding));
            d->m_ToolBar->addWidget(w);
            continue;
        } else {
            Core::Command *cmd = actionManager()->command(Core::Id(uid));
            if (cmd)
                d->m_ToolBar->addAction(cmd->action());
        }
    }
    d->m_ToolBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}

void ExtendedView::disconnectActionsFromDefaultSlots()
{
    d->m_DefaultSlots = false;
}

void ExtendedView::hideButtons() const
{
    d->m_ToolBar->hide();
}

void ExtendedView::showButtons()
{
    d->m_ToolBar->show();
}

void ExtendedView::useContextMenu(bool state)
{
    QAbstractItemView *view = d->m_Parent->itemView();
    if (state)
        view->setContextMenuPolicy(Qt::CustomContextMenu);
    else
        view->setContextMenuPolicy(Qt::NoContextMenu);
}

QMenu *ExtendedView::getContextMenu()
{
    QMenu *pop = new QMenu(d->m_Parent);
    pop->addActions(d->m_ToolBar->actions());
    return pop;
}

/**
  \brief Add a row to the model in two ways: list way, tree way.
  In the list way \e asChildOfCurrentIndex is \e false, the new item is added under the current item.\n
  In the tree way \e asChildOfCurrentIndex is \e true, the new item is added as a child of the current item.
*/
void ExtendedView::addItem(bool asChildOfCurrentIndex)
{
    if (!d->m_DefaultSlots) {
//        Q_EMIT addRequested();
        return;
    }
    QAbstractItemView *view = d->m_Parent->itemView();
    if (!view->model())
        return;

    // insert a row into model
    int row = 0;
    int col = 0;
    QModelIndex parentIndex;
    if (view->selectionModel()->hasSelection()) {
        if (asChildOfCurrentIndex) {
            parentIndex = view->currentIndex();
        } else {
            row = view->currentIndex().row() + 1;
            col = view->currentIndex().column();
            parentIndex = view->currentIndex().parent();
        }
    } else {
        row = view->model()->rowCount();
        if (row<0)
            row = 0;
    }
    if (!view->model()->insertRows(row, 1, parentIndex))
        LOG_ERROR_FOR("ExtendedView", QString("Can not add a row to the model %1").arg(view->model()->objectName()));

    // select inserted row and edit it
    QModelIndex index = view->model()->index(row, col, parentIndex);
    view->setCurrentIndex(index);
    if (view->editTriggers() != QAbstractItemView::NoEditTriggers) {
        view->edit(index);
    }
}

void ExtendedView::removeItem()
{
    if (!d->m_DefaultSlots)
        return;

    QAbstractItemView *view = d->m_Parent->itemView();
    if (!view->model())
        return;
    if (!view->selectionModel()->hasSelection())
        return;

    const QModelIndex &idx = view->currentIndex();
    if (idx.isValid()) {
        // Do this to keep QDataWidgetMapper informed of the modification
        view->edit(idx);
        view->closePersistentEditor(idx);
        // Now delete row
        int row = idx.row();
        if (!view->model()->removeRow(row, idx.parent())) {
            Utils::Log::addError("ExtendedView", QString("Can not remove row %1 from the model %2")
                             .arg(row)
                             .arg(view->model()->objectName()),
                             __FILE__, __LINE__);
        }
    }
//    Q_EMIT removeRequested();
}

void ExtendedView::moveDown()
{
    if (!d->m_DefaultSlots) {
        return;
    }
//    if (!d->canMoveDown())
//        return;

    QAbstractItemView *view = d->m_Parent->itemView();
    QModelIndex idx = view->currentIndex();
    bool moved = false;

    StringListModel *m = qobject_cast<StringListModel*>(view->model());
    if (m) {
        m->moveDown(idx);
        moved = true;
    } else {
        QStringListModel *strModel = qobject_cast<QStringListModel*>(view->model());
        if (strModel) {
            QStringList list = strModel->stringList();
            list.move(idx.row(), idx.row()+1);
            strModel->setStringList(list);
            moved=true;
        }
    }
    // TODO: else swap the two rows.

    if (moved)
        view->setCurrentIndex(view->model()->index(idx.row()+1,0));
//    Q_EMIT moveDownRequested();
}

void ExtendedView::moveUp()
{
    if (!d->m_DefaultSlots)
        return;

    QAbstractItemView *view = d->m_Parent->itemView();
    QModelIndex idx = view->currentIndex();
//    closePersistentEditor(idx);
    bool moved = false;

    StringListModel *m = qobject_cast<StringListModel*>(view->model());
    if (m) {
        m->moveUp(idx);
        moved = true;
    } else {
        QStringListModel *strModel = qobject_cast<QStringListModel*>(view->model());
        if (strModel) {
            QStringList list = strModel->stringList();
            list.move(idx.row(), idx.row()-1);
            strModel->setStringList(list);
            moved=true;
        }
    }

    // TODO: else swap the two rows.

    if (moved)
        view->setCurrentIndex(view->model()->index(idx.row()-1,0));
//    Q_EMIT moveUpRequested();
}

void ExtendedView::on_edit_triggered()
{
    if (!d->m_DefaultSlots)
        return;
}

void ExtendedView::contextMenu(const QPoint &p)
{
    Q_UNUSED(p);
    // TODO: improve this
//    QMenu *pop = getContextMenu();
//    pop->popup(view->mapToGlobal(p));
//    pop->exec();
//    delete pop;
//    pop = 0;
}



