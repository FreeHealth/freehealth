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
 *   Main Developpers :                                                    *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "htmldelegate.h"

#include <QPainter>
#include <QStyleOptionViewItemV4>
#include <QVariant>
#include <QModelIndex>
#include <QString>
#include <QIcon>
#include <QTextDocument>

namespace Utils {

void HtmlDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 options = option;
    initStyleOption(&options, index);

    painter->save();

    int leftIndent = 0;
    QVariant decoration = index.data(Qt::DecorationRole);
    if (!decoration.isNull()) {
        QIcon icon = decoration.value<QIcon>();
        if (!icon.isNull()) {
            leftIndent = icon.actualSize(QSize(32,32)).width();
        }
    }

    QTextDocument doc;
    QString text = options.text;
    if (option.state & QStyle::State_Selected) {
        text.replace("color:gray", "color:lightgray");
        text.replace("color:black", "color:white");
    }
    doc.setHtml(text);

    /* Call this to get the focus rect and selection background. */
    options.text = "";
    options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &options, painter, options.widget);

    /* Draw using our rich text document. */
    painter->translate(options.rect.left() + leftIndent, options.rect.top());
    QRect clip(0, 0, options.rect.width(), options.rect.height());
    doc.drawContents(painter, clip);

    painter->restore();
}

QSize HtmlDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 options = option;
    initStyleOption(&options, index);

    QTextDocument doc;
    doc.setHtml(options.text);
    doc.setTextWidth(options.rect.width());
    return QSize(doc.idealWidth(), doc.size().height());
}

} // namespace Utils
