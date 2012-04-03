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
 *  Main Developers : Eric Maeker <eric.maeker@gmail.com>                  *
 *  Contributors :                                                         *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
/**
  \class PadTools::TokenOutputDocument
  Text editor that allow user to interact with the output text of a PadDocument.
*/

#include "tokenoutputdocument.h"
#include "constants.h"
#include "tokeneditor.h"
#include "pad_document.h"

#include <translationutils/constants.h>
#include <translationutils/trans_current.h>

#include <QLinearGradient>
#include <QGradientStops>
#include <QMenu>

#include <QDebug>

using namespace PadTools;
using namespace Trans::ConstantTranslations;

namespace PadTools {
namespace Internal {
class TokenOutputDocumentPrivate
{
public:
    TokenOutputDocumentPrivate() :
        _pad(0), m_LastHoveredItem(0), m_LastFollowedItem(0)
    {
//        qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e7effd, stop: 1 #cbdaf1);
//        QLinearGradient grad(QPointF(0,0), QPointF(0,1));
//        grad.setColorAt(0, QColor(Qt::lightGray).lighter(50));
//        grad.setColorAt(1, QColor(Qt::lightGray));
        QBrush brush(QColor("#eeeeee"));

//        _hoveredCharFormat.setUnderlineColor(Qt::darkBlue);
//        _hoveredCharFormat.setUnderlineStyle(QTextCharFormat::WaveUnderline);
        _hoveredCharFormat.setBackground(brush);

    }

public:
    PadDocument *_pad;
    PadItem *m_LastHoveredItem, *m_LastFollowedItem; // should not be deleted
    QList<QTextCharFormat> m_LastHoveredItemCharFormats, m_LastFollowedItemCharFormats;
    QTextCharFormat _hoveredCharFormat;
    QTextFrameFormat _hoveredFrameFormat;
};
}
}

TokenOutputDocument::TokenOutputDocument(QWidget *parent) :
    Editor::TextEditor(parent, TokenOutputDocument::Full),
    d(new Internal::TokenOutputDocumentPrivate)
{
    setAttribute(Qt::WA_Hover);
    setAcceptDrops(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
    textEdit()->viewport()->installEventFilter(this);
}

TokenOutputDocument::~TokenOutputDocument()
{
    if (d) {
        delete d;
        d = 0;
    }
}

void TokenOutputDocument::setPadDocument(PadDocument *pad)
{
    d->_pad = pad;
    connect(pad, SIGNAL(cleared()), this, SLOT(onPadCleared()));
}

void TokenOutputDocument::onPadCleared()
{
    if (!d->m_LastHoveredItem)
        return;
    Constants::removePadFragmentFormat("Hover", document(), d->m_LastHoveredItemCharFormats);
    d->m_LastHoveredItem = 0;
}

void TokenOutputDocument::contextMenu(const QPoint &pos)
{
    if (textEdit()->underMouse()) {
        QTextCursor c = textEdit()->cursorForPosition(pos);
        textEdit()->setTextCursor(c);
    }
    QMenu *p = Editor::TextEditor::getContextMenu();

    QAction *a = new QAction(tkTr(Trans::Constants::EDIT_TOKEN), this);
    QAction *before = p->actions().first();
    p->insertAction(before, a);
    connect(a, SIGNAL(triggered()), this, SLOT(editTokenUnderCursor()));
    p->insertSeparator(before);

    p->exec(mapToGlobal(pos));
}

void TokenOutputDocument::editTokenUnderCursor()
{
    int position = textCursor().position();
    PadItem *item = d->_pad->padItemForOutputPosition(position);
    if (item) {
        TokenEditor editor(this);
        PadFragment *f = item->fragment(PadItem::Core);
        editor.setTokenName(d->_pad->fragmentRawSource(f));
        PadFragment *bef = item->fragment(PadItem::ConditionnalBeforeText);
        PadFragment *aft = item->fragment(PadItem::ConditionnalAfterText);
        editor.setConditionnalHtml(d->_pad->fragmentHtmlOutput(bef), d->_pad->fragmentHtmlOutput(aft));
        if (editor.exec()==QDialog::Accepted) {
            // Update before && after pads
//            d->_pad->rawSource();
            // Update formatting of token value/core
        }
    }
}

void TokenOutputDocument::dragEnterEvent(QDragEnterEvent *event)
{
    if (underMouse() &&
            event->mimeData()->hasFormat(Constants::TOKENVALUE_MIME)) {
        event->acceptProposedAction();
        event->accept();
    } else {
        event->ignore();
    }
}

void TokenOutputDocument::dragMoveEvent(QDragMoveEvent *event)
{
    if (underMouse() &&
            event->mimeData()->hasFormat(Constants::TOKENRAWSOURCE_MIME)) {
        textEdit()->setFocus();
        QTextCursor cursor = cursorForPosition(event->pos());
        setTextCursor(cursor);
        ensureCursorVisible();
        // if event pos y <=10 scroll up
        event->acceptProposedAction();
        event->accept();
    } else {
        event->ignore();
    }
}

void TokenOutputDocument::dragLeaveEvent(QDragLeaveEvent *event)
{
    if (underMouse()) {
        event->ignore();
    } else {
        event->accept();
    }
}

void TokenOutputDocument::dropEvent(QDropEvent *event)
{
    if (underMouse()) {
        TokenEditor editor(this);
//        editor.setCurrentIndex();
        editor.setTokenName(event->mimeData()->data(Constants::TOKENNAME_MIME));
        int r = editor.exec();
        if (r == QDialog::Accepted) {
            setFocus();
            QTextCursor cursor = cursorForPosition(event->pos());
            cursor.insertHtml(editor.toHtml());
            event->acceptProposedAction();
            event->accept();
            return;
        }
    }
    event->ignore();
}

bool TokenOutputDocument::event(QEvent *event)
{
    if (!d->_pad)
        return QObject::event(event);

    if (event->type()==QEvent::HoverMove) {
        QHoverEvent *me = static_cast<QHoverEvent*>(event);
        int position = cursorForPosition(me->pos()).position();
        PadItem *item = d->_pad->padItemForOutputPosition(position);
        QTextDocument *doc = document();
        if (!item) {
            if (d->m_LastHoveredItem) {
                Constants::removePadFragmentFormat("Hover", doc, d->m_LastHoveredItemCharFormats);
                d->m_LastHoveredItem = 0;
            }
            return QObject::event(event);
        }

        if (d->m_LastHoveredItem) {
            if (d->m_LastHoveredItem == item)
                return true;
            Constants::removePadFragmentFormat("Hover", doc, d->m_LastHoveredItemCharFormats);
            d->m_LastHoveredItem = item;
        } else {
            d->m_LastHoveredItem = item;
        }

        qWarning() << d->m_LastHoveredItem->id();

        Constants::setPadFragmentFormat("Hover", d->m_LastHoveredItem->outputStart(), d->m_LastHoveredItem->outputEnd(), doc, d->m_LastHoveredItemCharFormats, d->_hoveredCharFormat);
        me->accept();
        return true;
    } else if (event->type()==QEvent::HoverLeave && d->m_LastHoveredItem) {
        Constants::removePadFragmentFormat("Hover", document(), d->m_LastHoveredItemCharFormats);
        d->m_LastHoveredItem = 0;
        event->accept();
        return true;
    }


    return QObject::event(event);
}

bool TokenOutputDocument::eventFilter(QObject *o, QEvent *e)
{
    if (!d->_pad || o!=textEdit()->viewport())
        return false;

    if (e->type()==QEvent::MouseButtonDblClick) {
//        qWarning() << "filter" << o << e->type();
        // get the PadItem under mouse
        QMouseEvent *me = static_cast<QMouseEvent*>(e);
        setTextCursor(cursorForPosition(me->pos()));
        editTokenUnderCursor();
        return true;
    }
//    e->ignore();
    return QWidget::eventFilter(o, e);
}
