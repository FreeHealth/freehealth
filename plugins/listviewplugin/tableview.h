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
#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include <listviewplugin/listview_exporter.h>
#include <listviewplugin/extendedview.h>
#include <listviewplugin/constants.h>

#include <QWidget>
#include <QTableView>
#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QMenu>

/**
 * \file tableview.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 22 Sept 2012
*/

namespace Core {
class Context;
}

namespace Views {
namespace Internal {
class TableViewPrivate;
class ExtendedViewPrivate;
}

class LISTVIEW_EXPORT TableView : public IView
{
    Q_OBJECT
    friend class TableViewPrivate;
public:
    TableView(QWidget *parent = 0, Constants::AvailableActions actions = Constants::DefaultActions);
    virtual ~TableView();

    // IView
    QAbstractItemView *itemView() const;
    QTableView *tableView() const;

    void setActions(Constants::AvailableActions actions);
    void setCommands(const QStringList &commandsUid);
    void addContext(const Core::Context &context);

    void hideButtons() const;
    void showButtons();
    void useContextMenu(bool state = true);

    QHeaderView *horizontalHeader() const {return tableView()->horizontalHeader();}
    QHeaderView *verticalHeader() const {return tableView()->verticalHeader();}
    void hideColumn(int col) {tableView()->hideColumn(col);}
    void showColumn(int col) {tableView()->showColumn(col);}

public Q_SLOTS:
    virtual void addItem();
    virtual void removeItem();
    virtual void moveDown();
    virtual void moveUp();

Q_SIGNALS:
    void addRequested();
    void removeRequested();
    void moveUpRequested();
    void moveDownRequested();

private:
    Internal::TableViewPrivate *d;
};

}  // End Views


#endif // TABLEVIEW_H
