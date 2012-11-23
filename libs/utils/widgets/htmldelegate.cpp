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
 *   Main Developers :                                                    *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "htmldelegate.h"

#include <QPainter>
#include <QStyleOptionViewItemV4>
#include <QVariant>
#include <QModelIndex>
#include <QString>
#include <QIcon>
#include <QTextDocument>
#include <QApplication>
#include <QAbstractTextDocumentLayout>

#include <QDebug>

namespace Utils {

static QString changeColors(const QStyleOptionViewItem &option, QString text)
{
    if (option.state & QStyle::State_Selected) {
        text.replace("color:gray", "color:lightgray", Qt::CaseInsensitive);
        text.replace("color:black", "color:white", Qt::CaseInsensitive);
        text.replace("color:blue", "color:lightcyan", Qt::CaseInsensitive);
        text.replace("color:red", "color:bisque", Qt::CaseInsensitive);
        text.replace("color:maroon", "color:#F2E6E6", Qt::CaseInsensitive);
    }
    return text;
}

void HtmlDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 optionV4 = option;
    initStyleOption(&optionV4, index);

    QStyle *style = optionV4.widget? optionV4.widget->style() : QApplication::style();

    QTextDocument doc;
    doc.setHtml(changeColors(option, optionV4.text));

    // Painting item without text
    QString backupText = optionV4.text;
    optionV4.text = QString(); // inhibe text displaying
    style->drawControl(QStyle::CE_ItemViewItem, &optionV4, painter);
    optionV4.text = backupText;

    QAbstractTextDocumentLayout::PaintContext ctx;

    // Highlighting text if item is selected
    if (optionV4.state & QStyle::State_Selected)
        ctx.palette.setColor(QPalette::Text, optionV4.palette.color(QPalette::Active, QPalette::HighlightedText));

    // we only need the left of the plain text rect and the vertical middle. All other data are irrelevant because of HTML.
    QRect plainTextRect = style->subElementRect(QStyle::SE_ItemViewItemText, &optionV4);
    QRect textRect = optionV4.rect;
    textRect.setTop(plainTextRect.center().y() - doc.size().height() / 2);
    textRect.setHeight(doc.size().height());
    painter->save();
    painter->translate(textRect.topLeft());
    painter->setClipRect(textRect.translated(-textRect.topLeft()));
    //doc.setTextWidth(textRect.width());
    QRect htmlRect = textRect.translated(-textRect.topLeft());
    painter->translate(plainTextRect.left(), 0);
    doc.drawContents(painter, htmlRect);
    painter->translate(-plainTextRect.left(), 0);
    painter->restore();
}

QSize HtmlDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 options = option;
    initStyleOption(&options, index);

    QTextDocument doc;
    doc.setHtml(options.text);
//   doc.setTextWidth(options.rect.width()); does not work (the rect width is way too small, I don't know why) but the idea is interesting, TODO: improve it later
	return QSize(doc.idealWidth(), doc.size().height());
}

} // namespace Utils
