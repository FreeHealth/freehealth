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
#include <QPointer>
#include <QPushButton>
#include <QEvent>
#include <QString>
#include <QObject>
#include <QWidget>
#include <QFocusEvent>
class QHBoxLayout;
class QAction;
#include <tkListView.h>
#include <tkContextManager.h>

#include <QDebug>

class tkListViewContext;


class tkListViewPrivate : public QWidget
{
    Q_OBJECT
public:
    tkListViewPrivate( QWidget *parent, tkListView::AvailableActions actions );

    void populateToolbar();
    void calculateContext();

public:
    QWidget *m_Parent;
    QListView *m_ListView;
    tkListView::AvailableActions m_Actions;
    tkListViewContext *m_Context;
    QToolBar *m_ToolBar;
};


class tkListViewContext : public tkContext
{
public:
    tkListViewContext(tkListView *parent) : tkContext(parent), w(parent)
    {
    }

    void addContext( int uid )
    {
        if (!m_Context.contains(uid))
            m_Context.append(uid);
    }
    void clearContext() { m_Context.clear(); }

    QList<int> context() const { return m_Context; }
    QWidget *widget()          { return w; }

private:
    tkListView *w;
    QList<int> m_Context;
};



class tkListViewActionHandler : public QObject
{
    Q_OBJECT
public:
    tkListViewActionHandler(QObject *parent = 0);
    virtual ~tkListViewActionHandler() {}

    void setCurrentView(tkListView *view);


private Q_SLOTS:
    void moveUp();
    void moveDown();
    void addItem();
    void removeItem();
    void listViewItemChanged();

private:
    bool canMoveUp();
    bool canMoveDown();
    void updateActions();

protected:
    QAction *aAddRow;
    QAction *aRemoveRow;
    QAction *aDown;
    QAction *aUp;
    QAction *aEdit;

    QPointer<tkListView> m_CurrentView;
};



class tkListViewManager : public tkListViewActionHandler
{
    Q_OBJECT
public:
    static tkListViewManager *instance();
    ~tkListViewManager() {}

private Q_SLOTS:
    void updateContext(tkContext *object);

private:
    tkListViewManager(QObject *parent = 0);
    static tkListViewManager *m_Instance;
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
