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
#ifndef TREEVIEW_H
#define TREEVIEW_H

#include <listviewplugin/listview_exporter.h>
#include <listviewplugin/deselectabletreeview.h>
#include <listviewplugin/extendedview.h>
#include <listviewplugin/constants.h>

#include <QWidget>
#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QFlags>
#include <QMenu>

/**
 * \file treeview.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.5.0
 * \date 22 Feb 2011
*/


namespace Views {
namespace Internal {
class TreeViewPrivate;
}

class LISTVIEW_EXPORT TreeView : public DeselectableTreeView
{
    Q_OBJECT
    friend class TreeViewPrivate;
public:
    TreeView(QWidget *parent = 0, Constants::AvailableActions actions = Constants::DefaultActions);
    virtual ~TreeView();

    void setActions(Constants::AvailableActions actions);
    void setCommands(const QStringList &commandsUid);
    void addContext(const int id);
    void addContexts(const QList<int> &id);

    void disconnectActionsToDefaultSlots();

    void hideButtons() const;
    void showButtons();
    void useContextMenu(bool state = true);

public Q_SLOTS:
    virtual void addItem();
    virtual void removeItem();
    virtual void moveDown();
    virtual void moveUp();

Q_SIGNALS:
    void aboutToAddItem();
    void itemAdded();
    void addRequested();

    void aboutToRemove();
    void itemRemoved();
    void removeRequested();

    void moveUpRequested();
    void moveDownRequested();

private:
    Internal::TreeViewPrivate *d;
};

}  // End Views

#endif // TREEVIEW_H
