/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
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
  * \class tkListView
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

#include "tkListView.h"
#include "tkListView_p.h"

#include <tkLog.h>
#include <tkStringListModel.h>
#include <tkTheme.h>
#include <tkActionManager.h>
#include <tkConstantTranslations.h>

#include <tkActionManager.h>
#include <tkUniqueIdentifier.h>

#include <QVBoxLayout>
#include <QGridLayout>
#include <QAction>
#include <QMenu>
#include <QWidget>
#include <QListView>
#include <QStringListModel>
#include <QToolButton>

Q_TK_USING_CONSTANTS
Q_TK_USING_TRANSLATIONS

namespace tkListViewConstants
{
    const char* const M_TKLISTVIEW       = "menu_tkListView";
    const char* const C_BASIC            = "context.tkListView.basic";
    const char* const C_BASIC_ADDREMOVE  = "context.tkListView.AddRemove";
    const char* const C_BASIC_MOVE       = "context.tkListView.Move";
    const char* const G_LISTVIEW         = "group.tkListView";
    const char* const M_TKLISTVIEW_TEXT  = QT_TRANSLATE_NOOP("tkConstants", "List viewer");
}

/////////////////////////////////////////////////////////////////////////// List View Manager
tkListViewManager *tkListViewManager::m_Instance = 0;

tkListViewManager *tkListViewManager::instance()
{
    if (!m_Instance)
        m_Instance = new tkListViewManager(qApp);
    return m_Instance;
}

tkListViewManager::tkListViewManager(QObject *parent) : tkListViewActionHandler(parent)
{
    connect(tkContextManager::instance(), SIGNAL(contextChanged(tkContext*)),
            this, SLOT(updateContext(tkContext*)));
}

void tkListViewManager::updateContext(tkContext *object)
{

    if (object)
        qWarning() << "context" << object;

    tkListView *view = 0;
    do {
        if (!object) {
            if (!m_CurrentView)
                return;

            m_CurrentView = 0;
            break;
        }
        view = qobject_cast<tkListView *>(object->widget());
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
        tkListViewActionHandler::setCurrentView(view);
    }
}



/////////////////////////////////////////////////////////////////////////// Action Handler
tkListViewActionHandler::tkListViewActionHandler(QObject *parent) :
        QObject(parent),
        aAddRow(0),
        aRemoveRow(0),
        aDown(0),
        aUp(0),
        aEdit(0),
        m_CurrentView(0)
{
    tkActionManager *am = tkActionManager::instance();
    QList<int> basicContext = QList<int>() << tkUID->uniqueIdentifier(tkListViewConstants::C_BASIC);
    QList<int> addContext = QList<int>() << tkUID->uniqueIdentifier(tkListViewConstants::C_BASIC_ADDREMOVE);
    QList<int> moveContext = QList<int>() << tkUID->uniqueIdentifier(tkListViewConstants::C_BASIC_MOVE);

//    QList<int> allContexts;
//    allContexts << basicContext << addContext << moveContext;
//    // register already existing menu actions
//    aUndo = registerAction(tkConstants::A_EDIT_UNDO,  allContexts, this);
//    aRedo = registerAction(tkConstants::A_EDIT_REDO,  allContexts, this);
//    aCut = registerAction(tkConstants::A_EDIT_CUT,   allContexts, this);
//    aCopy = registerAction(tkConstants::A_EDIT_COPY,  allContexts, this);
//    aPaste = registerAction(tkConstants::A_EDIT_PASTE, allContexts, this);

    // Editor's Contextual Menu
    tkActionContainer *editMenu = am->actionContainer(tkConstants::M_EDIT);
    tkActionContainer *cmenu = am->actionContainer(tkConstants::M_EDIT_LIST);
    if (!cmenu) {
        cmenu = am->createMenu(tkConstants::M_EDIT_LIST);
        cmenu->appendGroup(tkConstants::G_EDIT_LIST);
        cmenu->setTranslations(tkConstants::M_EDIT_LIST_TEXT);
        if (editMenu)
            editMenu->addMenu(cmenu, tkConstants::G_EDIT_LIST);
    }

    QAction *a = aAddRow = new QAction(this);
    a->setObjectName("tkListView.aAddRow");
    a->setIcon(tkTheme::icon(tkConstants::ICONADD));
    tkCommand *cmd = am->registerAction(a, tkConstants::A_LIST_ADD, addContext);
    cmd->setTranslations(tkConstants::LISTADD_TEXT);
    cmenu->addAction(cmd, tkConstants::G_EDIT_LIST);
    connect(a, SIGNAL(triggered()), this, SLOT(addItem()));

    a = aRemoveRow = new QAction(this);
    a->setObjectName("tkListView.aRemoveRow");
    a->setIcon(tkTheme::icon(tkConstants::ICONREMOVE));
    cmd = am->registerAction(a, tkConstants::A_LIST_REMOVE, addContext);
    cmd->setTranslations(tkConstants::LISTREMOVE_TEXT);
    cmenu->addAction(cmd, tkConstants::G_EDIT_LIST);
    connect(a, SIGNAL(triggered()), this, SLOT(removeItem()));

    a = aDown = new QAction(this);
    a->setObjectName("tkListView.aDown");
    a->setIcon(tkTheme::icon(tkConstants::ICONMOVEDOWN));
    cmd = am->registerAction(a, tkConstants::A_LIST_MOVEDOWN, moveContext);
    cmd->setTranslations(tkConstants::LISTMOVEDOWN_TEXT);
    cmenu->addAction(cmd, tkConstants::G_EDIT_LIST);
    connect(a, SIGNAL(triggered()), this, SLOT(moveDown()));

    a = aUp = new QAction(this);
    a->setObjectName("tkListView.aUp");
    a->setIcon(tkTheme::icon(tkConstants::ICONMOVEUP));
    cmd = am->registerAction(a, tkConstants::A_LIST_MOVEUP, moveContext);
    cmd->setTranslations(tkConstants::LISTMOVEUP_TEXT);
    cmenu->addAction(cmd, tkConstants::G_EDIT_LIST);
    connect(a, SIGNAL(triggered()), this, SLOT(moveUp()));

}

void tkListViewActionHandler::setCurrentView(tkListView *view)
{
    if (view)
        qWarning() << "current view " << view;
    // disconnect old view
    if (m_CurrentView) {
        disconnect( m_CurrentView->listView()->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                    this, SLOT(listViewItemChanged()));
    }
    m_CurrentView = view;
    if (!view) { // this should never be the case
        return;
    }
    // reconnect some actions
    connect( m_CurrentView->listView()->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
             this, SLOT(listViewItemChanged()));
    updateActions();
}

void tkListViewActionHandler::listViewItemChanged()
{
    aUp->setEnabled(canMoveUp());
    aDown->setEnabled(canMoveDown());
}

void tkListViewActionHandler::updateActions()
{
    listViewItemChanged();
}

bool tkListViewActionHandler::canMoveUp()
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

bool tkListViewActionHandler::canMoveDown()
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

void tkListViewActionHandler::moveUp()
{ if (m_CurrentView) m_CurrentView->moveUp(); }
void tkListViewActionHandler::moveDown()
{ if (m_CurrentView) m_CurrentView->moveDown(); }
void tkListViewActionHandler::addItem()
{ if (m_CurrentView) m_CurrentView->addItem(); }
void tkListViewActionHandler::removeItem()
{ if (m_CurrentView) m_CurrentView->removeItem(); }

/////////////////////////////////////////////////////////////////////////// List View

/** \brief Constructor */
tkListView::tkListView(QWidget *parent,
                       AvailableActions actions)
    : QWidget(parent),
    d(0)
{
    static int handler = 0;
    ++handler;
    QObject::setObjectName("tkListView_"+QString::number(handler));
    d = new tkListViewPrivate(this, actions);

    // Create the Manager instance and context
    tkListViewManager::instance();
    d->m_Context = new tkListViewContext(this);
    d->calculateContext();
    tkContextManager::instance()->addContextObject(d->m_Context);

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


    connect(d->m_ListView, SIGNAL( customContextMenuRequested(const QPoint &)),
            this, SLOT(contextMenu(const QPoint &)));

    d->populateToolbar();
}

tkListViewPrivate::tkListViewPrivate( QWidget * parent, tkListView::AvailableActions actions ) :
    m_Parent(parent),
    m_ListView(0),
    m_Actions(actions),
    m_Context(0)

{
}

void tkListView::focusInEvent(QFocusEvent *event)
{
    qWarning() << objectName() << "focus in";
    Q_UNUSED(event);
    d->m_ListView->setFocus();
    QWidget::focusInEvent(event);
}

void tkListView::focusOutEvent(QFocusEvent *event)
{
    qWarning() << objectName() << "focus out";
    Q_UNUSED(event);
    this->setFocus();
    QWidget::focusOutEvent(event);
}


void tkListViewPrivate::calculateContext()
{
    m_Context->clearContext();
    if (m_Actions & tkListView::MoveUpDown)
        m_Context->addContext(tkUID->uniqueIdentifier(tkListViewConstants::C_BASIC_MOVE));

    if (m_Actions & tkListView::AddRemove)
        m_Context->addContext(tkUID->uniqueIdentifier(tkListViewConstants::C_BASIC_ADDREMOVE));
}

void tkListViewPrivate::populateToolbar()
{
    tkActionManager *am = tkActionManager::instance();
    if (m_Actions & tkListView::AddRemove) {
        tkCommand *cmd = am->command(tkConstants::A_LIST_ADD);
        m_ToolBar->addAction(cmd->action());
        cmd = am->command(tkConstants::A_LIST_REMOVE);
        m_ToolBar->addAction(cmd->action());
    }

    if (m_Actions & tkListView::MoveUpDown) {
        tkCommand *cmd = am->command(tkConstants::A_LIST_MOVEUP);
        m_ToolBar->addAction(cmd->action());
        cmd = am->command(tkConstants::A_LIST_MOVEDOWN);
        m_ToolBar->addAction(cmd->action());
    }
}

/** \brief Defines the objectName */
void tkListView::setObjectName( const QString &name )
{
    d->m_ListView->setObjectName(name+"ListView");
    QWidget::setObjectName(name);
}


/** \brief Returns the list view. */
QListView *tkListView::listView() const
{
    return d->m_ListView;
}

QAbstractItemModel * tkListView::model() const
{
    return d->m_ListView->model();
}

void tkListView::setModel( QAbstractItemModel * model )
{
    d->m_ListView->setModel( model );
}

void tkListView::setModelColumn( int column )
{
    d->m_ListView->setModelColumn( column );
}

int tkListView::modelColumn() const
{
    return d->m_ListView->modelColumn();
}

void tkListView::setCurrentIndex( const QModelIndex & item )
{
    d->m_ListView->setCurrentIndex( item );
}

void tkListView::setEditTriggers( QAbstractItemView::EditTriggers triggers )
{
    d->m_ListView->setEditTriggers(triggers);
}

void tkListView::setActions( AvailableActions actions )
{
    d->m_Actions = actions;
    d->calculateContext();
}

void tkListView::hideButtons() const
{
    d->m_ToolBar->hide();
}

void tkListView::showButtons()
{
    d->m_ToolBar->show();
}

void tkListView::useContextMenu( bool state )
{
    if (state)
        d->m_ListView->setContextMenuPolicy(Qt::CustomContextMenu);
    else
        d->m_ListView->setContextMenuPolicy(Qt::NoContextMenu);
}

QMenu * tkListView::getContextMenu()
{
    QMenu *pop = new QMenu(d->m_Parent);
    pop->addActions(d->m_ToolBar->actions());
    return pop;
}

void tkListView::addItem()
{
    if ( ! d->m_ListView->model() )
        return;
    
    // insert a row into model
    int row = 0;
    if ( d->m_ListView->currentIndex().isValid() )
        row = d->m_ListView->currentIndex().row() + 1;
    else {
        row = d->m_ListView->model()->rowCount();
        if (row<0)
            row = 0;
    }
    if ( !d->m_ListView->model()->insertRows( row, 1 ) )
        tkLog::addError( this, tr("tkListView can not add a row to the model %1").arg(model()->objectName()) );

    // select inserted row and edit it
    if ( d->m_ListView->editTriggers() != QAbstractItemView::NoEditTriggers ) {
        QModelIndex index = d->m_ListView->model()->index( row, 0 ) ;
        d->m_ListView->edit( index );
    }
}

void tkListView::removeItem()
{
    if (!d->m_ListView->model() )
        return;

    if ( d->m_ListView->currentIndex().isValid() ) {
        d->m_ListView->closePersistentEditor( d->m_ListView->currentIndex() );
        if ( ! d->m_ListView->model()->removeRows( d->m_ListView->currentIndex().row(), 1 ) )
            tkLog::addError( this, tr("tkListView can not remove row %1 to the model %2")
                             .arg( d->m_ListView->currentIndex().row() )
                             .arg( model()->objectName() ) );
    }
}

void tkListView::moveDown()
{
//    if (!d->canMoveDown())
//        return;

    QModelIndex idx = d->m_ListView->currentIndex();
    bool moved = false;

    tkStringListModel *m = qobject_cast<tkStringListModel*>(model());
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
        d->m_ListView->setCurrentIndex( d->m_ListView->model()->index(idx.row()+1,0) );
}

void tkListView::moveUp()
{
    QModelIndex idx = d->m_ListView->currentIndex();
//    d->m_ListView->closePersistentEditor( idx );
    bool moved = false;

    tkStringListModel *m = qobject_cast<tkStringListModel*>(model());
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
        d->m_ListView->setCurrentIndex( d->m_ListView->model()->index(idx.row()-1,0) );
}

void tkListView::on_edit_triggered()
{}

void tkListView::contextMenu( const QPoint & p )
{
    QMenu *pop = getContextMenu();
    pop->popup(this->mapToGlobal(p));
    pop->exec();
    delete pop;
    pop = 0;
}
