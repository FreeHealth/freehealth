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
#include "tkListView.h"
#include "tkListView_p.h"

#include <tkLog.h>
#include <tkStringListModel.h>
#include <tkTheme.h>
#include <tkActionManager.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QAction>
#include <QMenu>
#include <QWidget>
#include <QListView>
#include <QStringListModel>

Q_TK_USING_CONSTANTS

namespace tkListViewConstants
{
    const char * const  TEXTADD      = QT_TRANSLATE_NOOP( "tkListView", "Add");
    const char * const  TEXTREMOVE   = QT_TRANSLATE_NOOP( "tkListView", "Remove");
    const char * const  TEXTMOVEUP   = QT_TRANSLATE_NOOP( "tkListView", "Move Up");
    const char * const  TEXTMOVEDOWN = QT_TRANSLATE_NOOP( "tkListView", "Move Down");
    const char * const  M_TKLISTVIEW = "menu_tkListView";
}

/** \brief Constructor */
tkListView::tkListView( QWidget *parent, AvailableButtons buttons, AvailableActions actions )
    : QWidget(parent),
    d(0)
{
//    setObjectName("tkListView");
    d = new tkListViewPrivate(this, buttons, actions);
    this->setContextMenuPolicy( Qt::CustomContextMenu );
//    setFocusPolicy(Qt::StrongFocus);
//    setFocusProxy(d->m_ListView);
}

/** \brief Defines the objectName */
void tkListView::setObjectName( const QString &name )
{
    d->setObjectName(name+"Private");
    d->m_ListView->setObjectName(name+"FocusedListViewPrivate");
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

int tkListView::modelColumn () const
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

void tkListView::setButtons( AvailableButtons buttons )
{
    d->m_Buttons = buttons;
    d->createButtons();
}

void tkListView::setActions( AvailableActions actions )
{
    d->m_Actions = actions;
    d->createActions();
}

void tkListView::setAddToolTip( const QString & text )
{
    d->m_AddText = text;
    if (d->addButton)
        d->addButton->setToolTip( text );
    if (d->addRowAct)
        d->addRowAct->setText( text );
}

void tkListView::setRemoveToolTip( const QString & text )
{
    d->m_RemoveText = text;
    if (d->removeButton)
        d->removeButton->setToolTip( text );
    if (d->removeRowAct)
        d->removeRowAct->setText( text );
}

void tkListView::hideButtons() const
{
    d->wgtButtons->hide();
}

void tkListView::showButtons()
{
    d->wgtButtons->show();
}

void tkListView::useContextMenu( bool state )
{
    d->m_UseContextMenu = state;
}

QMenu * tkListView::getContextMenu()
{
    QMenu *pop = new QMenu( d->m_Parent );
    if ( d->addRowAct )
        pop->addAction( d->addRowAct );
    if ( d->removeRowAct )
        pop->addAction( d->removeRowAct );
    if (d->canMoveDown())
        if ( d->downAct )
            pop->addAction( d->downAct );
    if (d->canMoveUp())
        if ( d->upAct )
            pop->addAction( d->upAct );
    return pop;
}

tkListViewPrivate::tkListViewPrivate( QWidget * parent, tkListView::AvailableButtons buttons, tkListView::AvailableActions actions ) :
    QWidget(parent),
    m_Parent(parent),
    wgtButtons(0),
    m_ListView(0),
    addButton(0), removeButton(0), moveUpButton(0), moveDownButton(0),
    butLayout(0),
    addRowAct(0), removeRowAct(0), downAct(0), upAct(0), editAct(0),
    m_UseContextMenu(true),
    m_Buttons(buttons),
    m_Actions(actions),
    actionConnected(false)

{
    setObjectName(parent->objectName());
    wgtButtons = new QWidget(m_Parent);
    QVBoxLayout *vb = new QVBoxLayout(m_Parent);
    butLayout = new QHBoxLayout(wgtButtons);
    butLayout->setSpacing(12);
    butLayout->setMargin(0);
    vb->setSpacing(0);
    vb->setMargin(0);
    m_ListView = new tkListViewFocusedPrivate(m_Parent);
    m_ListView->setObjectName(parent->objectName());
    vb->addWidget(m_ListView);
    vb->addWidget(wgtButtons);

    createButtons();
    createActions();
//    createConnections();

    connect( m_Parent, SIGNAL( customContextMenuRequested(const QPoint &)), this, SLOT(contextMenu(const QPoint &)));
    connect( m_ListView, SIGNAL(focusChangedTo(bool)), this, SLOT(createConnections(bool)));

    // TODO connect listView currentIndexChanged with actions/buttons moveEnabled
//    connect( m_ListView, SIGNAL(activated(const QModelIndex &)), this, SLOT(checkButtons(const QModelIndex &)));
//    connect( m_ListView, SIGNAL(pressed(QModelIndex)), this, SLOT(checkButtons(const QModelIndex &)));
}

void tkListViewPrivate::createConnections(bool hasFocus)
{
    if (hasFocus && (!actionConnected)) {
        actionConnected = true;
        if (addRowAct)    connect( addRowAct, SIGNAL(triggered()), m_Parent, SLOT(on_add_triggered()) );
        if (removeRowAct) connect( removeRowAct, SIGNAL(triggered()), m_Parent, SLOT(on_remove_triggered()) );
        if (downAct)      connect( downAct, SIGNAL(triggered()), m_Parent, SLOT(on_moveDown_triggered()));
        if (upAct)        connect( upAct, SIGNAL(triggered()), m_Parent, SLOT(on_moveUp_triggered()));
    } if ((!hasFocus) && actionConnected) {
        actionConnected = false;
        if (addRowAct)    disconnect( addRowAct, SIGNAL(triggered()), m_Parent, SLOT(on_add_triggered()) );
        if (removeRowAct) disconnect( removeRowAct, SIGNAL(triggered()), m_Parent, SLOT(on_remove_triggered()) );
        if (downAct)      disconnect( downAct, SIGNAL(triggered()), m_Parent, SLOT(on_moveDown_triggered()));
        if (upAct)        disconnect( upAct, SIGNAL(triggered()), m_Parent, SLOT(on_moveUp_triggered()));
    }
}

void tkListViewPrivate::createButtons()
{
    if ( addButton ) {
        delete addButton;
        addButton = 0;
    }
    if ( removeButton ) {
        delete removeButton;
        removeButton = 0;
    }
    if ( moveUpButton ) {
        delete moveUpButton;
        moveUpButton = 0;
    }
    if ( moveDownButton ) {
        delete moveDownButton;
        moveDownButton = 0;
    }
    if ( butLayout )
        delete butLayout;
    butLayout = new QHBoxLayout(wgtButtons);
    butLayout->setSpacing(12);
    butLayout->setMargin(0);


    if (m_Buttons & tkListView::AddButton) {
        addButton = new QPushButton(wgtButtons);
        addButton->setMinimumSize( 22, 22 );
        addButton->setMaximumSize( 22, 22 );
        addButton->setIcon( tkTheme::icon(ICONADD) );
        addButton->setToolTip( tr("Add") );
        butLayout->addWidget( addButton );
        connect( addButton, SIGNAL(clicked()), m_Parent, SLOT(on_add_triggered()) );
    }

    if (m_Buttons & tkListView::RemoveButton) {
        removeButton = new QPushButton(wgtButtons);
        removeButton->setMinimumSize( 22, 22 );
        removeButton->setMaximumSize( 22, 22 );
        removeButton->setIcon( tkTheme::icon(ICONREMOVE) );
        removeButton->setToolTip( tr("Remove") );
        butLayout->addWidget( removeButton );
        connect( removeButton, SIGNAL(clicked()), m_Parent, SLOT(on_remove_triggered()) );
    }

    if (m_Buttons & tkListView::MoveUpButton) {
        moveUpButton = new  QPushButton(wgtButtons);
        moveUpButton->setToolTip( tr("Move up") );
        moveUpButton->setMinimumSize( 22, 22 );
        moveUpButton->setMaximumSize( 22, 22 );
        moveUpButton->setIcon( tkTheme::icon(ICONMOVEUP) );
        butLayout->addWidget( moveUpButton );
        connect( moveUpButton, SIGNAL(clicked()), m_Parent, SLOT(on_moveUp_triggered()) );
    }

    if (m_Buttons & tkListView::MoveDownButton) {
        moveDownButton = new  QPushButton(wgtButtons);
        moveDownButton->setToolTip( tr("Move down") );
        moveDownButton->setMinimumSize( 22, 22 );
        moveDownButton->setMaximumSize( 22, 22 );
        moveDownButton->setIcon( tkTheme::icon(ICONMOVEDOWN) );
        butLayout->addWidget( moveDownButton );
        connect( moveDownButton, SIGNAL(clicked()), m_Parent, SLOT(on_moveDown_triggered()) );
    }

    butLayout->addSpacerItem( new QSpacerItem( 10, 10, QSizePolicy::Expanding,QSizePolicy::Fixed ) );
}

void tkListViewPrivate::createActions()
{
    tkActionManager *m = tkActionManager::instance();
    if (!m->actionsExists(QStringList() << A_LIST_ADD << A_LIST_REMOVE << A_LIST_MOVEUP << A_LIST_MOVEDOWN)) {
        m->createPopMenu(tkListViewConstants::M_TKLISTVIEW, "", "", this);
        m->createDefaultEditMenu(this, tkListViewConstants::M_TKLISTVIEW);
        m->createDefaultEditMenuActions(this);
        m->refreshAll();
    }
    if ( addRowAct ) {
        addRowAct = 0;
    }
    if ( removeRowAct ) {
        removeRowAct = 0;
    }
    if ( downAct ) {
        downAct = 0;
    }
    if ( upAct ) {
        upAct = 0;
    }
    if ( editAct ) {
        editAct = 0;
    }

    if (m_Actions & tkListView::AddAction) {
        addRowAct = m->action(A_LIST_ADD);
    }

    if (m_Actions & tkListView::RemoveAction) {
        removeRowAct = m->action(A_LIST_REMOVE);
    }

    if (m_Actions & tkListView::MoveDownAction) {
        downAct = m->action(A_LIST_MOVEDOWN);
    }

    if (m_Actions & tkListView::MoveUpAction) {
        upAct = m->action(A_LIST_MOVEUP);
    }

//    if (m_Actions & tkListView::EditAction) {
//        editAct = new QAction( this );
//        editAct->setText( tr( "Edit" ) );

//        connect( editAct, SIGNAL(triggered()), m_Parent, SLOT(on_edit_triggered()));
//    }
}

void tkListViewPrivate::checkButtons(const QModelIndex &idx )
{
    Q_UNUSED(idx);
    if (moveUpButton)
        moveUpButton->setEnabled(canMoveUp());
    if (moveDownButton)
        moveDownButton->setEnabled(canMoveDown());
}

void tkListView::on_add_triggered()
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

void tkListView::on_remove_triggered()
{
    if (!d->m_ListView->model() )
        return;

    if ( d->m_ListView->currentIndex().isValid() ) {
        d->m_ListView->closePersistentEditor( d->m_ListView->currentIndex() );
        if ( ! d->m_ListView->model()->removeRows( d->m_ListView->currentIndex().row(), 1 ) )
            tkLog::addError( this, tr("tkListView can not remove row %1 to the model %1")
                             .arg( d->m_ListView->currentIndex().row() )
                             .arg( model()->objectName() ) );
    }
}

void tkListView::on_moveDown_triggered()
{
    if (!d->canMoveDown())
        return;

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

bool tkListViewPrivate::canMoveDown()
{
    QModelIndex idx = m_ListView->currentIndex();
    if (!idx.isValid())
        return false;
    if (idx.row() < (m_ListView->model()->rowCount()-1))
        return true;
    return false;
}

bool tkListViewPrivate::canMoveUp()
{
    QModelIndex idx = m_ListView->currentIndex();
    if (!idx.isValid())
        return false;
    if (idx.row() >= 1)
        return true;
    return false;
}

void tkListView::on_moveUp_triggered()
{
    if (!d->canMoveUp())
        return;

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

void tkListViewPrivate::contextMenu( const QPoint & p )
{
    if ( ! m_UseContextMenu )
        return;
    createConnections(true);
    QMenu *pop = static_cast<tkListView*>(m_Parent)->getContextMenu();
    pop->popup( m_Parent->mapToGlobal( p ) );
    pop->exec();
    delete pop;
    pop = 0;
}
