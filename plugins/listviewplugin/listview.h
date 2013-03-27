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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef LISTVIEW_H
#define LISTVIEW_H

#include <listviewplugin/listview_exporter.h>
#include <listviewplugin/extendedview.h>
#include <listviewplugin/constants.h>

#include <QWidget>
#include <QListView>
#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QMenu>

/**
 * \file listview.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 22 Sept 2012
*/

namespace Core {
class Context;
}

namespace Views {
namespace Internal {
class ListViewPrivate;
}

class LISTVIEW_EXPORT ListView : public IView
{
    Q_OBJECT
    friend class ListViewPrivate;

public:
    ListView(QWidget *parent = 0, Views::Constants::AvailableActions actions = Views::Constants::DefaultActions);
    virtual ~ListView();

    // IView
    QAbstractItemView *itemView() const;

    void setModelColumn(int column);

    void setActions(Views::Constants::AvailableActions actions);
    void setCommands(const QStringList &commandsUid);
    void addContext(const Core::Context &context);

    void hideButtons() const;
    void showButtons();
    void useContextMenu(bool state = true);

    void setMaximumRows(int max);
    int maximumRows() const;

public Q_SLOTS:
    virtual void addItem();
    virtual void removeItem();
    virtual void moveDown();
    virtual void moveUp();

Q_SIGNALS:
    void aboutToAddItem();
    void addRequested();
    void itemAdded();
    void aboutToRemove();
    void removeRequested();
    void itemRemoved();
    void moveUpRequested();
    void moveDownRequested();

private:
    Internal::ListViewPrivate *d;
};

}  // End Views


#endif // LISTVIEW_H
