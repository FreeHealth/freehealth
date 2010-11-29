/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef LISTVIEW_H
#define LISTVIEW_H

#include <listviewplugin/listview_exporter.h>

#include <QWidget>
#include <QListView>
#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QFlags>
#include <QMenu>

/**
 * \file listview.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.11
 * \date 08 Sept 2009
*/
namespace Views {
namespace Internal {
class ListViewPrivate;
}

class LISTVIEW_EXPORT ListView : public QListView
{
    Q_OBJECT
    friend class ListViewPrivate;
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

    ListView(QWidget *parent = 0, AvailableActions actions = DefaultActions);
    virtual ~ListView();

    void setObjectName(const QString &name);

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

Q_SIGNALS:
    void addRequested();
    void removeRequested();
    void moveUpRequested();
    void moveDownRequested();

private:
    Internal::ListViewPrivate *d;
};

}  // End Views

Q_DECLARE_OPERATORS_FOR_FLAGS(Views::ListView::AvailableActions)

#endif // TKLISTVIEW_H
