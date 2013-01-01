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
#ifndef EXTENDEDVIEW_H
#define EXTENDEDVIEW_H

#include <listviewplugin/listview_exporter.h>
#include <listviewplugin/constants.h>

#include <QObject>
#include <QWidget>
#include <QAbstractItemView>

QT_BEGIN_NAMESPACE
class QMenu;
class QPoint;
class QToolBar;
QT_END_NAMESPACE

/**
 * \file iview.h
 * \author Eric Maeker
 * \version 0.8.2
 * \date 01 Jan 2013
*/

namespace Views {
namespace Internal {
class ExtendedViewPrivate;
}

class LISTVIEW_EXPORT IView : public QWidget
{
    Q_OBJECT
public:
    IView(QWidget *parent);
    virtual ~IView() {}

    virtual QAbstractItemView *itemView() const = 0;
    virtual void addToolBar(QToolBar *bar);

    void setCurrentIndex(const QModelIndex &index) {itemView()->setCurrentIndex(index);}
    QModelIndex currentIndex() const {return itemView()->currentIndex();}
    void setModel(QAbstractItemModel *model) {itemView()->setModel(model);}
    QItemSelectionModel *selectionModel() const {if (itemView()) return itemView()->selectionModel(); return 0;}
    QAbstractItemModel *model() const {return itemView()->model();}
    void setEditTriggers(QAbstractItemView::EditTriggers trig) {itemView()->setEditTriggers(trig);}
    QModelIndex indexAt(const QPoint &point) const {return itemView()->indexAt(point);}

    QAbstractItemView::SelectionMode selectionMode() const {return itemView()->selectionMode();}
    void setSelectionMode(QAbstractItemView::SelectionMode mode) {itemView()->setSelectionMode(mode);}

    QAbstractItemView::SelectionBehavior selectionBehavior() const {return itemView()->selectionBehavior();}
    void setSelectionBehavior(QAbstractItemView::SelectionBehavior behavior) {itemView()->setSelectionBehavior(behavior);}

    void setAlternatingRowColors(bool enable) {itemView()->setAlternatingRowColors(enable);}

protected:
    void setItemView(QAbstractItemView *view);

Q_SIGNALS:
    // QAbstractItemView overload
    void activated(const QModelIndex &index);
    void clicked(const QModelIndex &index);
    void doubleClicked(const QModelIndex &index);
    void entered(const QModelIndex &index);
    void pressed(const QModelIndex &index);
    void viewportEntered();

private:
    QList<QToolBar*> m_AddedToolBars;
};

class LISTVIEW_EXPORT ExtendedView
{
public:
    ExtendedView(IView *parent = 0, Constants::AvailableActions actions = Constants::DefaultActions);
    virtual ~ExtendedView();

    void setActions(Constants::AvailableActions actions);
    void setCommands(const QStringList &commandsUid);

    void disconnectActionsFromDefaultSlots();

    void hideButtons() const;
    void showButtons();
    void useContextMenu(bool state = true);

    virtual QMenu *getContextMenu();
    virtual void contextMenu(const QPoint &);
    virtual void addItem(bool hasChildOfCurrentIndex = false);
    virtual void removeItem();
    virtual void moveDown();
    virtual void moveUp();
    virtual void on_edit_triggered();

private:
    Internal::ExtendedViewPrivate *d;
};


}  // End namespace Views


#endif // EXTENDEDVIEW_H
