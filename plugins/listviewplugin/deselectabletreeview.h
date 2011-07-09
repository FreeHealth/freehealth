/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef DESELECTABLETREEVIEW_H
#define DESELECTABLETREEVIEW_H

#include <listviewplugin/listview_exporter.h>
#include <QTreeView>
#include <QMouseEvent>

#include <QDebug>

namespace Views {

class LISTVIEW_EXPORT DeselectableTreeView : public QTreeView
{
public:
    DeselectableTreeView(QWidget *parent) : QTreeView(parent), m_IsDeselectable(true) {}
    virtual ~DeselectableTreeView() {}

    void setDeselectable(bool deselectable) {m_IsDeselectable = deselectable;}

private:
    virtual void mousePressEvent(QMouseEvent *event)
    {
        if (m_IsDeselectable && selectionModel()) {
            QModelIndex item = indexAt(event->pos());
            bool selected = selectionModel()->isSelected(indexAt(event->pos()));
            QTreeView::mousePressEvent(event);
            if (selected) {
                if (selectionBehavior()==QAbstractItemView::SelectItems) {
                    selectionModel()->select(item, QItemSelectionModel::Deselect);
                } else if (selectionBehavior()==QAbstractItemView::SelectRows) {
                    QModelIndexList items = selectionModel()->selectedColumns(item.row());
                    for(int i=0; i<selectionModel()->model()->columnCount(item); ++i) {
                        selectionModel()->select(selectionModel()->model()->index(item.row(), i, item.parent()), QItemSelectionModel::Deselect);
                    }
                } else if (selectionBehavior()==QAbstractItemView::SelectColumns) {
                    for(int i=0; i<selectionModel()->model()->rowCount(item); ++i) {
                        selectionModel()->select(selectionModel()->model()->index(i, item.column(), item.parent()), QItemSelectionModel::Deselect);
                    }
                }
            }
        } else {
            QTreeView::mousePressEvent(event);
        }
    }

    bool m_IsDeselectable;
};

}  // End namespace Views


#endif // DESELECTABLETREEVIEW_H
