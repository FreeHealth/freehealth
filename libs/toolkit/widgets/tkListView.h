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
#ifndef TKLISTVIEW_H
#define TKLISTVIEW_H

#include <tkExporter.h>

class tkListViewPrivate;
#include <QWidget>
#include <QListView>
#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QFlags>
#include <QMenu>

/**
 * \file tkListView.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.7
 * \date 24 April 2009
*/

class Q_TK_EXPORT tkListView : public QWidget
{
    Q_OBJECT
    friend class tkListViewPrivate;
public:

    /** \brief Enumerates the available default buttons that can be added. Buttons lies under the list view*/
    enum AvailableButton {
        AddButton      = 0x1,
        RemoveButton   = 0x2,
        MoveUpButton   = 0x4,
        MoveDownButton = 0x8,
        DefaultButtons = AddButton | RemoveButton,
        AllButtons     = AddButton | RemoveButton | MoveUpButton | MoveDownButton
    };
    Q_DECLARE_FLAGS( AvailableButtons, AvailableButton );

    /** \brief Enumerates the available default actions that can be added. Actions are pop-upped. */
    enum AvailableAction {
        AddAction      = 0x01,
        RemoveAction   = 0x02,
        MoveUpAction   = 0x04,
        MoveDownAction = 0x08,
        EditAction     = 0x10,
        DefaultActions = AddAction | RemoveAction,
        AllActions = AddAction | RemoveAction | MoveUpAction | MoveDownAction
    };
    Q_DECLARE_FLAGS( AvailableActions, AvailableAction );


    tkListView( QWidget *parent = 0,
                AvailableButtons buttons = DefaultButtons,
                AvailableActions actions = DefaultActions );
    virtual ~tkListView() {}
    void setObjectName( const QString &name );

    virtual QListView *listView() const;
    virtual QAbstractItemModel * model () const;
    virtual void setModel( QAbstractItemModel * model );
    void setModelColumn( int column );
    int modelColumn () const;
    void setCurrentIndex( const QModelIndex & item );

    void setEditTriggers( QAbstractItemView::EditTriggers triggers );

    void setButtons( AvailableButtons buttons );
    void setActions( AvailableActions actions );

    void setAddToolTip( const QString & text );
    void setRemoveToolTip( const QString & text );
    void hideButtons() const;
    void showButtons();
    void useContextMenu( bool state = true );

protected Q_SLOTS:
    virtual QMenu * getContextMenu();
    virtual void on_add_triggered();
    virtual void on_remove_triggered();
    virtual void on_moveDown_triggered();
    virtual void on_moveUp_triggered();
    virtual void on_edit_triggered();

private:
    tkListViewPrivate *d;
};
Q_DECLARE_OPERATORS_FOR_FLAGS( tkListView::AvailableButtons )
Q_DECLARE_OPERATORS_FOR_FLAGS( tkListView::AvailableActions )

#endif // TKLISTVIEW_H
