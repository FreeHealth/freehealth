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
 * \version 0.0.10
 * \date 21 July 2009
*/

class Q_TK_EXPORT tkListView : public QWidget
{
    Q_OBJECT
    friend class tkListViewPrivate;
public:

    /** \brief Enumerates the available default actions that can be added. Actions are pop-upped. */
    enum AvailableAction {
        AddRemove      = 0x01,
        MoveUpDown     = 0x02,
        Edit           = 0x04,
        DefaultActions = AddRemove,
        AllActions     = AddRemove | MoveUpDown
    };
    Q_DECLARE_FLAGS( AvailableActions, AvailableAction );


    tkListView( QWidget *parent = 0,
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

//    void setButtons( AvailableButtons buttons );
    void setActions( AvailableActions actions );

    void hideButtons() const;
    void showButtons();
    void useContextMenu(bool state = true);

public Q_SLOTS:
    virtual QMenu *getContextMenu();
    virtual void contextMenu(const QPoint & );
    virtual void addItem();
    virtual void removeItem();
    virtual void moveDown();
    virtual void moveUp();
    virtual void on_edit_triggered();

protected:
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);

private:
    tkListViewPrivate *d;
};
Q_DECLARE_OPERATORS_FOR_FLAGS( tkListView::AvailableActions )

#endif // TKLISTVIEW_H
