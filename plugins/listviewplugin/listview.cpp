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
 ***************************************************************************/
/**
  * \class ListView
  * \brief This widget shows a QListView with some buttons and a context menu.
  * Actions inside the contextual menu and the toolbar can be setted using
    AvailableActions param and setActions().\n
  * You can reimplement addItem(), removeItem() and on_edit_triggered()
    that are called by buttons and menu (add , remove). \n
  * The getContextMenu() is automatically called when the contextMenu is requiered.\n
    You can reimplement getContextMenu() in order to use your own contextMenu.\n
    Remember that the poped menu will be deleted after execution. \n
  \todo There is a problem when including this widget into a QDataWidgetMapper, when this widget loses focus, datas are not retreived.
  \ingroup toolkit
  \ingroup widget_toolkit
*/

#include "listview.h"
#include "listview_p.h"
#include "stringlistmodel.h"

#include <translationutils/constanttranslations.h>
#include <utils/log.h>

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/uniqueidmanager.h>
#include <coreplugin/itheme.h>
#include <coreplugin/icore.h>
#include <coreplugin/constants.h>

#include <QVBoxLayout>
#include <QGridLayout>
#include <QAction>
#include <QMenu>
#include <QWidget>
#include <QListView>
#include <QStringListModel>
#include <QToolButton>

using namespace Views;
using namespace Views::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ActionManager *actionManager() { return Core::ICore::instance()->actionManager(); }
static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }

namespace ListViewConstants
{
    const char* const C_BASIC            = "context.ListView.basic";
    const char* const C_BASIC_ADDREMOVE  = "context.ListView.AddRemove";
    const char* const C_BASIC_MOVE       = "context.ListView.Move";
}

/////////////////////////////////////////////////////////////////////////// List View Manager
ListViewManager *ListViewManager::m_Instance = 0;

ListViewManager *ListViewManager::instance()
{
    if (!m_Instance)
        m_Instance = new ListViewManager(qApp);
    return m_Instance;
}

ListViewManager::ListViewManager(QObject *parent) : ListViewActionHandler(parent)
{
    connect(contextManager(), SIGNAL(contextChanged(Core::IContext*)),
            this, SLOT(updateContext(Core::IContext*)));
}

void ListViewManager::updateContext(Core::IContext *object)
{
//    if (object)
//        qWarning() << "context" << object;
    ListView *view = 0;
    do {
        if (!object) {
            if (!m_CurrentView)
                return;

            m_CurrentView = 0;
            break;
        }
        view = qobject_cast<ListView *>(object->widget());
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
        ListViewActionHandler::setCurrentView(view);
    }
}



/////////////////////////////////////////////////////////////////////////// Action Handler
ListViewActionHandler::ListViewActionHandler(QObject *parent) :
        QObject(parent),
        aAddRow(0),
        aRemoveRow(0),
        aDown(0),
        aUp(0),
        aEdit(0),
        m_CurrentView(0)
{
    Core::ActionManager *am = Core::ICore::instance()->actionManager();
    Core::UniqueIDManager *uid = Core::ICore::instance()->uniqueIDManager();
    Core::ITheme *th = Core::ICore::instance()->theme();
    QList<int> basicContext = QList<int>() << uid->uniqueIdentifier(ListViewConstants::C_BASIC);
    QList<int> addContext = QList<int>() << uid->uniqueIdentifier(ListViewConstants::C_BASIC_ADDREMOVE);
    QList<int> moveContext = QList<int>() << uid->uniqueIdentifier(ListViewConstants::C_BASIC_MOVE);

//    QList<int> allContexts;
//    allContexts << basicContext << addContext << moveContext;
//    // register already existing menu actions
//    aUndo = registerAction(Core::Constants::A_EDIT_UNDO,  allContexts, this);
//    aRedo = registerAction(Core::Constants::A_EDIT_REDO,  allContexts, this);
//    aCut = registerAction(Core::Constants::A_EDIT_CUT,   allContexts, this);
//    aCopy = registerAction(Core::Constants::A_EDIT_COPY,  allContexts, this);
//    aPaste = registerAction(Core::Constants::A_EDIT_PASTE, allContexts, this);

    // Editor's Contextual Menu
    Core::ActionContainer *editMenu = am->actionContainer(Core::Constants::M_EDIT);
    Core::ActionContainer *cmenu = am->actionContainer(Core::Constants::M_EDIT_LIST);
    if (!cmenu) {
        cmenu = am->createMenu(Core::Constants::M_EDIT_LIST);
        cmenu->appendGroup(Core::Constants::G_EDIT_LIST);
        cmenu->setTranslations(Trans::Constants::M_EDIT_LIST_TEXT);
        if (editMenu)
            editMenu->addMenu(cmenu, Core::Constants::G_EDIT_LIST);
    }

    QAction *a = aAddRow = new QAction(this);
    a->setObjectName("ListView.aAddRow");
    a->setIcon(th->icon(Core::Constants::ICONADD));
    Core::Command *cmd = am->registerAction(a, Core::Constants::A_LIST_ADD, addContext);
    cmd->setTranslations(Trans::Constants::LISTADD_TEXT);
    cmenu->addAction(cmd, Core::Constants::G_EDIT_LIST);
    connect(a, SIGNAL(triggered()), this, SLOT(addItem()));

    a = aRemoveRow = new QAction(this);
    a->setObjectName("ListView.aRemoveRow");
    a->setIcon(th->icon(Core::Constants::ICONREMOVE));
    cmd = am->registerAction(a, Core::Constants::A_LIST_REMOVE, addContext);
    cmd->setTranslations(Trans::Constants::LISTREMOVE_TEXT);
    cmenu->addAction(cmd, Core::Constants::G_EDIT_LIST);
    connect(a, SIGNAL(triggered()), this, SLOT(removeItem()));

    a = aDown = new QAction(this);
    a->setObjectName("ListView.aDown");
    a->setIcon(th->icon(Core::Constants::ICONMOVEDOWN));
    cmd = am->registerAction(a, Core::Constants::A_LIST_MOVEDOWN, moveContext);
    cmd->setTranslations(Trans::Constants::LISTMOVEDOWN_TEXT);
    cmenu->addAction(cmd, Core::Constants::G_EDIT_LIST);
    connect(a, SIGNAL(triggered()), this, SLOT(moveDown()));

    a = aUp = new QAction(this);
    a->setObjectName("ListView.aUp");
    a->setIcon(th->icon(Core::Constants::ICONMOVEUP));
    cmd = am->registerAction(a, Core::Constants::A_LIST_MOVEUP, moveContext);
    cmd->setTranslations(Trans::Constants::LISTMOVEUP_TEXT);
    cmenu->addAction(cmd, Core::Constants::G_EDIT_LIST);
    connect(a, SIGNAL(triggered()), this, SLOT(moveUp()));

}

void ListViewActionHandler::setCurrentView(ListView *view)
{
    if (view)
        qWarning() << "current view " << view;
    // disconnect old view
    if (m_CurrentView) {
        disconnect(m_CurrentView->listView()->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                   this, SLOT(listViewItemChanged()));
    }
    m_CurrentView = view;
    if (!view) { // this should never be the case
        return;
    }
    // reconnect some actions
    connect(m_CurrentView->listView()->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this, SLOT(listViewItemChanged()));
    updateActions();
}

void ListViewActionHandler::listViewItemChanged()
{
    aUp->setEnabled(canMoveUp());
    aDown->setEnabled(canMoveDown());
}

void ListViewActionHandler::updateActions()
{
    listViewItemChanged();
}

bool ListViewActionHandler::canMoveUp()
{
    if (!m_CurrentView)
        return false;
    QModelIndex idx = m_CurrentView->listView()->currentIndex();
    if (!idx.isValid())
        return false;
    if (idx.row() >= 1)
        return true;
    return false;
}

bool ListViewActionHandler::canMoveDown()
{
    if (!m_CurrentView)
        return false;
    QModelIndex idx = m_CurrentView->listView()->currentIndex();
    if (!idx.isValid())
        return false;
    if (idx.row() < (m_CurrentView->listView()->model()->rowCount()-1))
        return true;
    return false;
}

void ListViewActionHandler::moveUp()
{ if (m_CurrentView) m_CurrentView->moveUp(); }
void ListViewActionHandler::moveDown()
{ if (m_CurrentView) m_CurrentView->moveDown(); }
void ListViewActionHandler::addItem()
{ if (m_CurrentView) m_CurrentView->addItem(); }
void ListViewActionHandler::removeItem()
{ if (m_CurrentView) m_CurrentView->removeItem(); }


/////////////////////////////////////////////////////////////////////////// List View
namespace Views {
namespace Internal {

class ListViewPrivate
{
public:
    ListViewPrivate(QWidget * parent, ListView::AvailableActions actions) :
            m_Parent(parent),
            m_ListView(0),
            m_Actions(actions),
            m_Context(0)
    {
    }

    ~ListViewPrivate()
    {
    }

    void calculateContext()
    {
        m_Context->clearContext();
        if (m_Actions & ListView::MoveUpDown)
            m_Context->addContext(Core::ICore::instance()->uniqueIDManager()->uniqueIdentifier(ListViewConstants::C_BASIC_MOVE));

        if (m_Actions & ListView::AddRemove)
            m_Context->addContext(Core::ICore::instance()->uniqueIDManager()->uniqueIdentifier(ListViewConstants::C_BASIC_ADDREMOVE));
    }

    void populateToolbar()
    {
        Core::ActionManager *am = Core::ICore::instance()->actionManager();
        if (m_Actions & ListView::AddRemove) {
            Core::Command *cmd = am->command(Core::Constants::A_LIST_ADD);
            m_ToolBar->addAction(cmd->action());
            cmd = am->command(Core::Constants::A_LIST_REMOVE);
            m_ToolBar->addAction(cmd->action());
        }

        if (m_Actions & ListView::MoveUpDown) {
            Core::Command *cmd = am->command(Core::Constants::A_LIST_MOVEUP);
            m_ToolBar->addAction(cmd->action());
            cmd = am->command(Core::Constants::A_LIST_MOVEDOWN);
            m_ToolBar->addAction(cmd->action());
        }
    }

public:
    QWidget *m_Parent;
    QListView *m_ListView;
    ListView::AvailableActions m_Actions;
    ListViewContext *m_Context;
    QToolBar *m_ToolBar;
    QString m_ContextName;
};

}  // End Internal
}  // End Views


/** \brief Constructor */
ListView::ListView(QWidget *parent, AvailableActions actions)
    : QWidget(parent),
    d(0)
{
    static int handler = 0;
    ++handler;
    QObject::setObjectName("ListView_"+QString::number(handler));
    d = new Internal::ListViewPrivate(this, actions);

    // Create the Manager instance and context
    ListViewManager::instance();
    d->m_Context = new ListViewContext(this);
    d->calculateContext();
    contextManager()->addContextObject(d->m_Context);

    // Create widget
    QGridLayout *layout = new QGridLayout(this);
    layout->setSpacing(3);
    layout->setMargin(0);
    d->m_ListView = new QListView(this);
    d->m_ListView->setContextMenuPolicy(Qt::CustomContextMenu);
    d->m_ToolBar = new QToolBar(this);
    d->m_ToolBar->setIconSize(QSize(16,16));
    d->m_ToolBar->setFocusPolicy(Qt::ClickFocus);
    layout->addWidget(d->m_ListView, 0, 0 , 1, 2);
    layout->addWidget(d->m_ToolBar, 1, 0);

    connect(d->m_ListView, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(contextMenu(const QPoint &)));

    d->populateToolbar();
}

ListView::~ListView()
{
    contextManager()->removeContextObject(d->m_Context);
}

/** \brief Defines the objectName */
void ListView::setObjectName(const QString &name)
{
    d->m_ListView->setObjectName(name+"ListView");
    QWidget::setObjectName(name);
}


/** \brief Returns the list view. */
QListView *ListView::listView() const
{
    return d->m_ListView;
}

QAbstractItemModel * ListView::model() const
{
    return d->m_ListView->model();
}

void ListView::setModel(QAbstractItemModel * model)
{
    d->m_ListView->setModel(model);
}

void ListView::setModelColumn(int column)
{
    d->m_ListView->setModelColumn(column);
}

int ListView::modelColumn() const
{
    return d->m_ListView->modelColumn();
}

void ListView::setCurrentIndex(const QModelIndex & item)
{
    d->m_ListView->setCurrentIndex(item);
}

void ListView::setEditTriggers(QAbstractItemView::EditTriggers triggers)
{
    d->m_ListView->setEditTriggers(triggers);
}

void ListView::setActions(AvailableActions actions)
{
    d->m_Actions = actions;
    d->calculateContext();
}

void ListView::hideButtons() const
{
    d->m_ToolBar->hide();
}

void ListView::showButtons()
{
    d->m_ToolBar->show();
}

void ListView::useContextMenu(bool state)
{
    if (state)
        d->m_ListView->setContextMenuPolicy(Qt::CustomContextMenu);
    else
        d->m_ListView->setContextMenuPolicy(Qt::NoContextMenu);
}

QMenu * ListView::getContextMenu()
{
    QMenu *pop = new QMenu(d->m_Parent);
    pop->addActions(d->m_ToolBar->actions());
    return pop;
}

void ListView::addItem()
{
    if (! d->m_ListView->model())
        return;
    
    // insert a row into model
    int row = 0;
    if (d->m_ListView->currentIndex().isValid()) {
        row = d->m_ListView->currentIndex().row() + 1;
    } else {
        row = d->m_ListView->model()->rowCount();
        if (row<0)
            row = 0;
    }
    if (!d->m_ListView->model()->insertRows(row, 1))
        Utils::Log::addError(this, QString("ListView can not add a row to the model %1").arg(model()->objectName()));

    // select inserted row and edit it
    QModelIndex index = d->m_ListView->model()->index(row, d->m_ListView->modelColumn());
    d->m_ListView->setCurrentIndex(index);
    if (d->m_ListView->editTriggers() != QAbstractItemView::NoEditTriggers) {
        d->m_ListView->edit(index);
    }
}

void ListView::removeItem()
{
    if (!d->m_ListView->model())
        return;
    QModelIndex idx = d->m_ListView->currentIndex();
    if (idx.isValid()) {
        d->m_ListView->closePersistentEditor(idx);
        int row = idx.row();
        qWarning() << idx.data() << idx.row();
        if (!d->m_ListView->model()->removeRows(row, 1))
            Utils::Log::addError(this, QString("ListView can not remove row %1 to the model %2")
                             .arg(row)
                             .arg(model()->objectName()));
    }
}

void ListView::moveDown()
{
//    if (!d->canMoveDown())
//        return;

    QModelIndex idx = d->m_ListView->currentIndex();
    bool moved = false;

    StringListModel *m = qobject_cast<StringListModel*>(model());
    if (m) {
        m->moveDown(idx);
        moved = true;
    } else {
        QStringListModel *strModel = qobject_cast<QStringListModel*>(model());
        if (strModel) {
            QStringList list = strModel->stringList();
            list.move(idx.row(), idx.row()+1);
            strModel->setStringList(list);
            moved=true;
        }
    }
    // TODO : else swap the two rows.

    if (moved)
        d->m_ListView->setCurrentIndex(d->m_ListView->model()->index(idx.row()+1,0));
}

void ListView::moveUp()
{
    QModelIndex idx = d->m_ListView->currentIndex();
//    d->m_ListView->closePersistentEditor(idx);
    bool moved = false;

    StringListModel *m = qobject_cast<StringListModel*>(model());
    if (m) {
        m->moveUp(idx);
        moved = true;
    } else {
        QStringListModel *strModel = qobject_cast<QStringListModel*>(model());
        if (strModel) {
            QStringList list = strModel->stringList();
            list.move(idx.row(), idx.row()-1);
            strModel->setStringList(list);
            moved=true;
        }
    }
    // TODO : else swap the two rows.

    if (moved)
        d->m_ListView->setCurrentIndex(d->m_ListView->model()->index(idx.row()-1,0));
}

void ListView::on_edit_triggered()
{}

void ListView::contextMenu(const QPoint & p)
{
    QMenu *pop = getContextMenu();
    pop->popup(this->mapToGlobal(p));
    pop->exec();
    delete pop;
    pop = 0;
}
