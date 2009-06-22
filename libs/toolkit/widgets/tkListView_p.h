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
#ifndef TKLISTVIEW_P_H
#define TKLISTVIEW_P_H

#include <QListView>
#include <QPushButton>
#include <QEvent>
#include <QString>
#include <QObject>
#include <QWidget>
#include <QFocusEvent>
class QHBoxLayout;
class QAction;
#include <tkListView.h>
class tkListViewFocusedPrivate;

#include <QDebug>

class tkListViewPrivate : public QWidget
{
    Q_OBJECT
public:
    tkListViewPrivate( QWidget *parent,
                       tkListView::AvailableButtons buttons, tkListView::AvailableActions actions );

    void createButtons();
    void createActions();

    bool canMoveDown();
    bool canMoveUp();
    QListView *listView();

private Q_SLOTS:
//    void changeEvent( QEvent *e );
    void contextMenu(const QPoint & );
    void checkButtons(const QModelIndex &idx );
    void createConnections(bool hasFocus = true);

public:
    QWidget *m_Parent;
    QWidget * wgtButtons;
    tkListViewFocusedPrivate *m_ListView;
    QPushButton *addButton, *removeButton, *moveUpButton, *moveDownButton;
    QString m_RemoveText, m_AddText;
    QHBoxLayout *butLayout;
    QAction *addRowAct, *removeRowAct, *downAct, *upAct, *editAct;
    bool m_UseContextMenu;
    tkListView::AvailableButtons m_Buttons;
    tkListView::AvailableActions m_Actions;
    bool actionConnected;
};

class tkListViewFocusedPrivate : public QListView
{
    Q_OBJECT
public:
    tkListViewFocusedPrivate( QWidget * parent = 0 ) : QListView(parent) {}

    void focusInEvent( QFocusEvent * event )
    {
        if (event->gotFocus()  ) {
            emit focusChangedTo(true);
        }
    }

    void focusOutEvent( QFocusEvent * event )
    {
        if (event->lostFocus()) {
            emit focusChangedTo(false);
        }
    }

    bool isEditing()  { return state() == QAbstractItemView::EditingState; }

Q_SIGNALS:
    void focusChangedTo(bool focused);
};

#endif // TKLISTVIEW_P_H
