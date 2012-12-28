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
#include <QTreeView>
#include <QMap>
#include <QTimer>

#include <QDebug>

enum { DrawDelegateRect = false };

namespace Utils {
namespace Internal {
class HtmlDelegatePrivate
{
public:
    HtmlDelegatePrivate(HtmlDelegate *parent) :
        q(parent)
    {}

    ~HtmlDelegatePrivate()
    {
    }

public:
    QTextDocument document;
    QMap<QPersistentModelIndex, int> heights;
    QTimer timer;

private:
    HtmlDelegate *q;
};
} // namespace Internal

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

HtmlDelegate::HtmlDelegate(QObject *parent) :
    QStyledItemDelegate(parent),
    d_html(new Internal::HtmlDelegatePrivate(this))
{
    d_html->timer.setInterval(250);
    d_html->timer.setSingleShot(true);
}

HtmlDelegate::~HtmlDelegate()
{
    if (d_html)
        delete d_html;
    d_html = 0;
}

void HtmlDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 optionV4 = option;
    initStyleOption(&optionV4, index);
    QStyle *style = optionV4.widget? optionV4.widget->style() : QApplication::style();

    d_html->document.setHtml(changeColors(option, optionV4.text));
//    document.setTextWidth( view->viewport()->width() -view->iconSize().width() -( iconMargin *2 ) );

    // Painting item without text
    QString backupText = optionV4.text;
    optionV4.text = QString(); // inhibe text displaying
    style->drawControl(QStyle::CE_ItemViewItem, &optionV4, painter);
    optionV4.text = backupText;

    QAbstractTextDocumentLayout::PaintContext ctx;

    // Highlighting text if item is selected
    if (optionV4.state & QStyle::State_Selected)
        ctx.palette.setColor(QPalette::Text, optionV4.palette.color(QPalette::Active, QPalette::HighlightedText));

    // plainTextRect == rect where to draw the HTML
    // We only need the left of the plain text rect and the vertical middle.
    // All other data are irrelevant because of HTML.
    QRect plainTextRect = style->subElementRect(QStyle::SE_ItemViewItemText, &optionV4);
    plainTextRect = plainTextRect.adjusted(1,1,-1,-1);

    //d_html->document.setTextWidth(plainTextRect.width());

    // TextRect includes the decoration + item data. On treeview it does not include the branch.
    QRect textRect = optionV4.rect;
    textRect.setTop(plainTextRect.center().y() - d_html->document.size().height() / 2);
    textRect.setHeight(d_html->document.size().height());
    textRect = textRect.adjusted(1,1,-1,-1);

    QPen pen;
    if (DrawDelegateRect) {
        pen.setColor(QColor("red"));
        painter->setPen(pen);
        painter->drawRect(plainTextRect);
        pen.setColor(QColor("blue"));
        painter->setPen(pen);
        painter->drawRoundedRect(textRect, 5, 5);
    }

    painter->save();
    painter->translate(textRect.topLeft());
    painter->setClipRect(textRect.translated(-textRect.topLeft()));
    QRect htmlRect = textRect.translated(-textRect.topLeft());
    painter->translate(plainTextRect.left(), 0);

    // Connect timer
    //QAbstractItemView *view = qobject_cast<QAbstractItemView*>(const_cast<QWidget*>(optionV4.widget));
    //QObject::connect(&d_html->timer, SIGNAL(timeout()), view->model(), SLOT(update()), Qt::UniqueConnection);

    // Manage indentation?
    QTreeView *treeview = qobject_cast<QTreeView*>(const_cast<QWidget*>(optionV4.widget));
    if (treeview) {
        QModelIndex idx = index;
        int indent = treeview->indentation();
        while (idx.parent().isValid()) {
            idx = idx.parent();
            indent += treeview->indentation();
        }
        painter->translate(-QPointF(indent, 0));
    }

    if (DrawDelegateRect) {
        pen.setColor(QColor("green"));
        painter->setPen(pen);
        painter->drawRoundedRect(htmlRect.adjusted(2,2,-2,-2), 5, 5);
    }

    d_html->document.setTextWidth(htmlRect.width());
    d_html->document.drawContents(painter, htmlRect);
    painter->translate(-plainTextRect.left(), 0);
    painter->restore();
}

QSize HtmlDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    qWarning() << "SIZEHINT";
    QStyleOptionViewItemV4 options = option;
    initStyleOption(&options, index);
    QStyle *style = options.widget? options.widget->style() : QApplication::style();

    d_html->document.setHtml(options.text);

    QRect plainTextRect = style->subElementRect(QStyle::SE_ItemViewItemText, &options);
    plainTextRect = plainTextRect.adjusted(1,1,-1,-1);

    // TextRect includes the decoration + item data. On treeview it does not include the branch.
    QRect textRect = options.rect;
    textRect.setTop(plainTextRect.center().y() - d_html->document.size().height() / 2);
    textRect.setHeight(d_html->document.size().height());
    textRect = textRect.adjusted(1,1,-1,-1);
    QRect htmlRect = textRect.translated(-textRect.topLeft());

//    if (d_html->heights.value(index, 0) != d_html->document.size().height()) {
//        d_html->heights[index] = d_html->document.size().height();
//        d_html->timer.start();
//    }

//    qWarning() << d_html->heights;

    d_html->document.setTextWidth(htmlRect.width());
    //   d_html->document.setTextWidth(options.rect.width()); does not work (the rect width is way too small, I don't know why) but the idea is interesting, TODO: improve it later
    return QSize(d_html->document.size().width(), d_html->document.size().height());
}

} // namespace Utils
