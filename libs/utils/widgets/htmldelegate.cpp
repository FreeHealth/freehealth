/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developers:                                                     *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 * \class Utils::HtmlDelegate
 * Allow to use Html inside any QAbstractItemView.
 *
 * HTML Composition requirements: \n
 * For best adjustement of colors,
 * use <em><span style="color:..."></em> everywhere around your text with color \b names:
 * "gray", "black", "blue", "red", "maroon". These are correctly managed
 * when the item is selected.
 *
 * All view types: \n
 * The textElideMode is not managed yet. The cells will resize automatically everywhere except in QTableView.
 *
 * For QListView: \n
 * Ensure that the resizeMode is set to \e Adjust and uniformItemSizes is \e false.
 * \code
 * QListView::setResizeMode(QListView::Adjust);
 * QListView::setUniformItemSizes(false);
 * \endcode
 *
 * For QTreeView: \n
 * Just set uniformItemSizes to false.
 */

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
#include <QListView>
#include <QTableView>

#include <QDebug>

// Note: we can also use the sizeHintChanged(index); signal to force view to
// update.

enum { DrawDelegateRect = false };

using namespace Utils;
using namespace Internal;

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

    QString changeColors(const QStyleOptionViewItemV4 &optionV4, QString text)
    {
        if (optionV4.state & QStyle::State_Selected) {
            text.replace(QRegExp("color\\s*:\\s*gray", Qt::CaseInsensitive), "color:lightgray");
            text.replace(QRegExp("color\\s*:\\s*black", Qt::CaseInsensitive), "color:white");
            text.replace(QRegExp("color\\s*:\\s*blue", Qt::CaseInsensitive), "color:lightcyan");
            text.replace(QRegExp("color\\s*:\\s*red", Qt::CaseInsensitive), "color:bisque");
            text.replace(QRegExp("color\\s*:\\s*marron", Qt::CaseInsensitive), "color:#F2E6E6");
        }
        return text;
    }

    void setHtml(const QModelIndex &index, const QStyleOptionViewItemV4 &optionV4)
    {
        QTextDocument *doc;
        if (_documents.contains(index)) {
            doc = _documents.value(index);
        } else {
            doc = new QTextDocument(q);
            _documents.insert(index, doc);
        }
        doc->setHtml(changeColors(optionV4, optionV4.text));
    }

    void setDocumentWidth(const QModelIndex &index, const QStyleOptionViewItemV4 &optionV4)
    {
        QTextDocument *doc = _documents.value(index);
        if (!doc) {
            qWarning() << "No Doc?" << index;
            return;
        }
        doc->setTextWidth(getMaxWidth(optionV4));
    }

    QSize documentSize(const QModelIndex &index)
    {
        QTextDocument *doc = _documents.value(index);
        if (!doc) {
            qWarning() << "No Doc?" << index;
            return QSize(1,1);
        }
        return QSize(doc->textWidth(), doc->size().height());
    }

    void drawDocument(QPainter *painter, const QModelIndex &index, const QRect &rect)
    {
        QTextDocument *doc = _documents.value(index, 0);
        if (!doc) {
            qWarning() << "  **** HtmlDelegate: No doc registered for index:" << index;
            return;
        }
        doc->drawContents(painter, rect);
    }

    int getMaxWidth(const QStyleOptionViewItemV4 &optionV4)
    {
        int max = optionV4.rect.width();

        // List view -> take viewport
        QListView *view = qobject_cast<QListView*>(const_cast<QWidget*>(optionV4.widget));
        if (view)
            max = qMin(max, view->viewport()->width());

        // Treeview -> Manage indentation (only for the first shown column)
        QTreeView *treeview = qobject_cast<QTreeView*>(const_cast<QWidget*>(optionV4.widget));
        if (treeview) {
            QModelIndex idx = optionV4.index;
            max = treeview->columnWidth(idx.column());
            int indent = treeview->indentation();
            while (idx.parent().isValid()) {
                idx = idx.parent();
                indent += treeview->indentation();
            }
            max -= indent;
        }

        // Manage decoration
        if (!optionV4.decorationSize.isNull() && !optionV4.icon.isNull()) {
            max -= optionV4.decorationSize.width();
        }

        return max;
    }

public:
    QMap<QPersistentModelIndex, QTextDocument*> _documents;
    QTimer timer;
    QMultiMap<QTreeView *, QPersistentModelIndex> _treeViewDataChanged;

private:
    HtmlDelegate *q;
};
} // namespace Internal
} // namespace Utils


HtmlDelegate::HtmlDelegate(QObject *parent) :
    QStyledItemDelegate(parent),
    d_html(new HtmlDelegatePrivate(this))
{
    d_html->timer.setInterval(75);
    d_html->timer.setSingleShot(true);
    this->connect(&d_html->timer, SIGNAL(timeout()), this, SLOT(treeView_indexDataChanged()));
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

//    qWarning() << "paint" << index;

    // Force any treeview using this delegate to resize the items
    QTreeView *treeview = qobject_cast<QTreeView*>(const_cast<QWidget*>(optionV4.widget));
    if (treeview) {
        if (d_html->getMaxWidth(optionV4) != d_html->documentSize(index).width()) {
            if (!d_html->_treeViewDataChanged.values(treeview).contains(index)) {
                d_html->_treeViewDataChanged.insertMulti(treeview, index);
                d_html->timer.start();
            }
        }
    }

    // Manage QTableView which do not call sizeHint() of the delegate
    QTableView *tableview = qobject_cast<QTableView*>(const_cast<QWidget*>(optionV4.widget));
    if (tableview) {
        d_html->setHtml(index, optionV4);
        d_html->setDocumentWidth(index, optionV4);
    }

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

    // TextRect includes the decoration + item data. On treeview it does not include the branch.
    QRect textRect = optionV4.rect;
    textRect.setTop(plainTextRect.top());
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

    // Manage indentation (only for the first shown column)
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

    // redefine html rect width
    htmlRect.setWidth(d_html->getMaxWidth(optionV4));

    // Update html colors according to the state
    // TODO: improve this using a better cache system, instead of using QMap<QPersistentIndexModel, QTextDocument*> use internal struc with {index, state, doc}
    d_html->setHtml(index, optionV4);
    d_html->setDocumentWidth(index, optionV4);

    // Adapt document width & draw it
    d_html->drawDocument(painter, index, htmlRect);
    painter->translate(-plainTextRect.left(), 0);
    painter->restore();
}

QSize HtmlDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 optionV4 = option;
    initStyleOption(&optionV4, index);

    d_html->setHtml(index, optionV4);
    d_html->setDocumentWidth(index, optionV4);

//    if (option.textElideMode != Qt::ElideNone) {
//        qWarning() << option.rect.size();
//        return option.rect.size();
//    }

//    qWarning() << "\nsizeHint";
//    qWarning() << "  index: "<< index << "docSize:" << d_html->documentSize(index);
//    qWarning() << "  html:" << optionV4.text.length();

    return d_html->documentSize(index);

}

// Force any treeview using this delegate to resize the items
void HtmlDelegate::treeView_indexDataChanged()
{
    foreach(QTreeView *tree, d_html->_treeViewDataChanged.keys()) {
        foreach(const QPersistentModelIndex &index, d_html->_treeViewDataChanged.values(tree)) {
            tree->dataChanged(index, index);
        }
    }
    d_html->_treeViewDataChanged.clear();
}
