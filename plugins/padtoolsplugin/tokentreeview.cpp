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
 *   Main Developers:                                                      *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "tokentreeview.h"

#include <QMimeData>
#include <QDrag>
#include <QPixmap>
#include <QPainter>
#include <QLinearGradient>
#include <QHeaderView>

#include <QDebug>

using namespace PadTools;

TokenTreeView::TokenTreeView(QWidget *parent) :
    QTreeView(parent)
{
    setSelectionMode(QAbstractItemView::SingleSelection);
    header()->setResizeMode(0, QHeaderView::Stretch);
}

QPixmap TokenTreeView::renderToPixmap(const QModelIndexList &indexes, QRect *r)
{
    int h, w;
    h = 0;
    w = 0;
    QStringList names;
    foreach(const QModelIndex &i, indexes) {
        QString n = i.data().toString();
        if (n.isEmpty())
            continue;
        QModelIndex parent = i.parent();
        while (parent.parent() != QModelIndex()) {
            n.prepend(parent.data().toString() + ".");
            parent = parent.parent();
        }
        const QRect &rect = visualRect(i);
        h += rect.height();
        w = qMax(w, rect.width());
        names += n;
    }

    QFontMetrics metric(font());
    QSize size = metric.size(Qt::TextSingleLine, names.join("\n"));

    w = size.width() + 10;
    h = size.height() + 6;
    r->setRect(0, 0, w, h);

    // Create the pixmap
    QPixmap pix(w, h);
    pix.fill(Qt::transparent);

    QLinearGradient gradient(0, 0, 0, pix.height()-1);
    gradient.setColorAt(0, Qt::white);
    gradient.setColorAt(0.3, QColor("#eeeeee"));
    gradient.setColorAt(0.5, QColor("#e9e9e9"));
    gradient.setColorAt(0.7, QColor("#eeeeee"));
    gradient.setColorAt(1.0, Qt::white);

    QPainter painter;
    painter.begin(&pix);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(gradient);
    painter.drawRoundedRect(QRectF(0, 0, pix.width()-1, pix.height()-1), 5, 5);
    painter.setBrush(Qt::black);
    painter.drawText(QRect(QPoint(5, 3), size), Qt::AlignCenter, names.join("\n"));
    painter.end();
    return pix;
}

void TokenTreeView::startDrag(Qt::DropActions supportedActions)
{
    QModelIndexList idx = selectedIndexes();
    if (idx.count() > 0) {
        QMimeData *data = model()->mimeData(idx);
        if (!data)
            return;
        QRect rect;
        QPixmap pix = renderToPixmap(idx, &rect);
        rect.adjust(horizontalOffset(), verticalOffset(), 0, 0);
        QDrag *drag = new QDrag(this);
        drag->setPixmap(pix);
        drag->setMimeData(data);
        drag->setHotSpot(QPoint(-20,-10));
        drag->start(supportedActions);
//        if (drag->start(supportedActions) == Qt::MoveAction)
            // clear or remove idx
    }
}
