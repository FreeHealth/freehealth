/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
#ifndef EXTENDEDVIEW_H
#define EXTENDEDVIEW_H

#include <listviewplugin/listview_exporter.h>
#include <listviewplugin/constants.h>

#include <QObject>

class QAbstractItemView;
class QMenu;
class QPoint;

namespace Views {
namespace Internal {
class ExtendedViewPrivate;
}

class LISTVIEW_EXPORT ExtendedView
{
public:
    ExtendedView(QAbstractItemView *parent = 0, Constants::AvailableActions actions = Constants::DefaultActions);
    virtual ~ExtendedView();

    void setActions(Constants::AvailableActions actions);
    void setCommands(const QStringList &commandsUid);

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
