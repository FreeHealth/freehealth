/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "extendedview.h"
#include "viewmanager.h"
#include "stringlistmodel.h"

#include <translationutils/constanttranslations.h>
#include <utils/log.h>

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/uniqueidmanager.h>
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
using namespace Views::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ActionManager *actionManager() { return Core::ICore::instance()->actionManager(); }
static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }


namespace Views {
namespace Internal {

class ExtendedViewPrivate
{
public:
    ExtendedViewPrivate(QAbstractItemView *parent, Constants::AvailableActions actions) :
            m_Parent(parent),
            m_Actions(actions)
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
    QAbstractItemView *m_Parent;
    Constants::AvailableActions m_Actions;
    QToolBar *m_ToolBar;
    QString m_ContextName;
};

}  // End Internal
}  // End Views


/** \brief Constructor */
ExtendedView::ExtendedView(QAbstractItemView *parent, Constants::AvailableActions actions) :
        d(0)
{
    static int handler = 0;
    ++handler;
//    QObject::setObjectName("ExtendedView_"+QString::number(handler));
    d = new Internal::ExtendedViewPrivate(parent, actions);

    // Add toolbar to the horizontal scroolbar
    ViewManager::instance();
    parent->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    d->m_ToolBar = new QToolBar(parent);
    d->m_ToolBar->setIconSize(QSize(16,16));
    d->m_ToolBar->setFocusPolicy(Qt::ClickFocus);
    d->m_ToolBar->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    d->populateToolbar();
    parent->addScrollBarWidget(d->m_ToolBar, Qt::AlignLeft);

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
        Core::Command *cmd = actionManager()->command(uid);
        if (cmd)
            d->m_ToolBar->addAction(cmd->action());
    }
    d->m_ToolBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
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
    if (state)
        d->m_Parent->setContextMenuPolicy(Qt::CustomContextMenu);
    else
        d->m_Parent->setContextMenuPolicy(Qt::NoContextMenu);
}

QMenu *ExtendedView::getContextMenu()
{
    QMenu *pop = new QMenu(d->m_Parent);
    pop->addActions(d->m_ToolBar->actions());
    return pop;
}

/**
  \brief Add a row to the model in two ways: list way, tree way.
  In the list way \e hasChildOfCurrentIndex is \e false, the new item is added under the current item.\n
  In the tree way \e hasChildOfCurrentIndex is \e true, the new item is added as a child of the current item.
*/
void ExtendedView::addItem(bool hasChildOfCurrentIndex)
{
    if (!d->m_Parent->model())
        return;

    // insert a row into model
    int row = 0;
    int col = 0;
    QModelIndex parentIndex;
    if (d->m_Parent->selectionModel()->hasSelection()) {
        if (hasChildOfCurrentIndex) {
            parentIndex = d->m_Parent->currentIndex();
        } else {
            row = d->m_Parent->currentIndex().row() + 1;
            col = d->m_Parent->currentIndex().column();
            parentIndex = d->m_Parent->currentIndex().parent();
        }
    } else {
        row = d->m_Parent->model()->rowCount();
        if (row<0)
            row = 0;
    }
    if (!d->m_Parent->model()->insertRows(row, 1, parentIndex))
        Utils::Log::addError("ExtendedView", QString("Can not add a row to the model %1").arg(d->m_Parent->model()->objectName()),
                             __FILE__, __LINE__);

    // select inserted row and edit it
    QModelIndex index = d->m_Parent->model()->index(row, col, parentIndex);
    d->m_Parent->setCurrentIndex(index);
    if (d->m_Parent->editTriggers() != QAbstractItemView::NoEditTriggers) {
        d->m_Parent->edit(index);
    }
//    Q_EMIT addRequested();
}

void ExtendedView::removeItem()
{
    if (!d->m_Parent->model())
        return;
    if (!d->m_Parent->selectionModel()->hasSelection())
        return;
    const QModelIndex &idx = d->m_Parent->currentIndex();
    if (idx.isValid()) {
        // Do this to keep QDataWidgetMapper informed of the modification
        d->m_Parent->edit(idx);
        d->m_Parent->closePersistentEditor(idx);
        // Now delete row
        int row = idx.row();
        if (!d->m_Parent->model()->removeRow(row, idx.parent())) {
            Utils::Log::addError("ExtendedView", QString("Can not remove row %1 to the model %2")
                             .arg(row)
                             .arg(d->m_Parent->model()->objectName()),
                             __FILE__, __LINE__);
        }
    }
//    Q_EMIT removeRequested();
}

void ExtendedView::moveDown()
{
//    if (!d->canMoveDown())
//        return;

    QModelIndex idx = d->m_Parent->currentIndex();
    bool moved = false;

    StringListModel *m = qobject_cast<StringListModel*>(d->m_Parent->model());
    if (m) {
        m->moveDown(idx);
        moved = true;
    } else {
        QStringListModel *strModel = qobject_cast<QStringListModel*>(d->m_Parent->model());
        if (strModel) {
            QStringList list = strModel->stringList();
            list.move(idx.row(), idx.row()+1);
            strModel->setStringList(list);
            moved=true;
        }
    }
    // TODO : else swap the two rows.

    if (moved)
        d->m_Parent->setCurrentIndex(d->m_Parent->model()->index(idx.row()+1,0));
//    Q_EMIT moveDownRequested();
}

void ExtendedView::moveUp()
{
    QModelIndex idx = d->m_Parent->currentIndex();
//    closePersistentEditor(idx);
    bool moved = false;

    StringListModel *m = qobject_cast<StringListModel*>(d->m_Parent->model());
    if (m) {
        m->moveUp(idx);
        moved = true;
    } else {
        QStringListModel *strModel = qobject_cast<QStringListModel*>(d->m_Parent->model());
        if (strModel) {
            QStringList list = strModel->stringList();
            list.move(idx.row(), idx.row()-1);
            strModel->setStringList(list);
            moved=true;
        }
    }

    /** \todo else swap the two rows. */

    if (moved)
        d->m_Parent->setCurrentIndex(d->m_Parent->model()->index(idx.row()-1,0));
//    Q_EMIT moveUpRequested();
}

void ExtendedView::on_edit_triggered()
{}

void ExtendedView::contextMenu(const QPoint &p)
{
    QMenu *pop = getContextMenu();
    pop->popup(d->m_Parent->mapToGlobal(p));
    pop->exec();
    delete pop;
    pop = 0;
}



