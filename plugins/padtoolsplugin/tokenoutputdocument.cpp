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

#include <utils/global.h>
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
        _pad(0), _lastHoveredItem(0)
    {
//        qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e7effd, stop: 1 #cbdaf1);
//        QLinearGradient grad(QPointF(0,0), QPointF(0,1));
//        grad.setColorAt(0, QColor(Qt::lightGray).lighter(50));
//        grad.setColorAt(1, QColor(Qt::lightGray));
        QBrush brush(QColor("#eeeeee"));
        _hoveredCharFormat.setBackground(brush);

        _hoveredTokenCoreCharFormat.setUnderlineColor(Qt::darkBlue);
        _hoveredTokenCoreCharFormat.setUnderlineStyle(QTextCharFormat::WaveUnderline);
    }

public:
    PadDocument *_pad;
    PadItem *_lastHoveredItem; // should not be deleted
    QList<QTextCharFormat> _lastHoveredItemCharFormats, _lastHoveredTokenCoreCharFormats;
    QTextCharFormat _hoveredCharFormat;
    QTextCharFormat _hoveredTokenCoreCharFormat;
    QTextFrameFormat _hoveredFrameFormat;
};
}
}

TokenOutputDocument::TokenOutputDocument(QWidget *parent) :
    Editor::TextEditor(parent, TokenOutputDocument::Simple),
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
    if (!d->_lastHoveredItem)
        return;
    Constants::removePadFragmentFormat("Hover", document(), d->_lastHoveredItemCharFormats);
    d->_lastHoveredItem = 0;
}

void TokenOutputDocument::contextMenu(const QPoint &pos)
{
    QTextCursor c = textEdit()->cursorForPosition(pos);
    if (textEdit()->underMouse()) {
        textEdit()->setTextCursor(c);
    }

    if (d->_pad->padItemForOutputPosition(c.position())) {
        QMenu *p = Editor::TextEditor::getContextMenu();
        QAction *a = new QAction(tkTr(Trans::Constants::EDIT_TOKEN), this);
        QAction *before = p->actions().first();
        p->insertAction(before, a);
        connect(a, SIGNAL(triggered()), this, SLOT(editTokenUnderCursor()));
        p->insertSeparator(before);
        p->exec(mapToGlobal(pos));
    } else {
        Editor::TextEditor::contextMenu(pos);
    }
}

void TokenOutputDocument::editTokenUnderCursor()
{
    if (!d->_pad)
        return;

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
            // Remove rawSource PadItem
            QTextCursor cursor(d->_pad->rawSourceDocument());
            cursor.setPosition(item->start());
            cursor.setPosition(item->end(), QTextCursor::KeepAnchor);
            cursor.removeSelectedText();
            // Insert new Item in rawSource
            cursor.setPosition(item->start());
            cursor.insertHtml(editor.toRawSourceHtml());
            // Reset _pad
//            d->_pad->softReset();
        }
    }
}

void TokenOutputDocument::dragEnterEvent(QDragEnterEvent *event)
{
    if (!d->_pad)
        return;

    if (textEdit()->underMouse() &&
            event->mimeData()->hasFormat(Constants::TOKENRAWSOURCE_MIME)) {
        event->acceptProposedAction();
        event->accept();
    } else {
        event->ignore();
    }
}

void TokenOutputDocument::dragMoveEvent(QDragMoveEvent *event)
{
    if (!d->_pad)
        return;

    if (textEdit()->underMouse() &&
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
    if (!d->_pad)
        return;

    if (textEdit()->underMouse()) {
        event->ignore();
    } else {
        event->accept();
    }
}

void TokenOutputDocument::dropEvent(QDropEvent *event)
{
    if (!d->_pad)
        return;

    if (underMouse()) {
        // Where did the user drop the token ?
        QTextCursor cursor = d->_pad->rawSourceCursorForOutputPosition(this->cursorForPosition(event->pos()).position());
        int pos = cursor.position();
        int dropRawPosition = pos;

        qWarning() << "DROP AT OUTPUT:"<<this->cursorForPosition(event->pos()).position() << "RAW" << pos;

        PadItem *item = d->_pad->padItemForSourcePosition(pos);
        if (item) {
            // Inside a PadItem
            PadFragment *core = item->getCore();
            if (core) {
                // Inside its PadCore -> manage error
                if (core->containsRawPosition(pos)) {
                    // drop inside a PadCore, ask user what to do
                    QStringList buttons;
                    buttons << tr("Inside conditionnal before text");
                    buttons << tr("Inside conditionnal after text");
                    buttons << tr("Before");
                    buttons << tr("After");
                    int s = Utils::withButtonsMessageBox(tr("Nested token"),
                                                         tr("You have dropped a token inside the value of a token. \n"
                                                            "You must specify where the dropped token should be inserted:\n"
                                                            "- inside the conditionnal text before the token, \n"
                                                            "- inside the conditionnal text after the token \n"
                                                            "- or the before/after the token"),
                                                         "",
                                                         buttons, "",
                                                         // with cancel
                                                         true
                                                         );
                    switch (s) {
                    case 0: // inside before conditionnal
                        qWarning() << "inside before cond";
                        dropRawPosition = core->start() - 1;
                        break;
                    case 1: // inside after conditionnal
                        qWarning() << "inside after cond";
                        dropRawPosition = core->end() + 1;
                        break;
                    case 2: // before the token
                        qWarning() << "before token";
                        dropRawPosition = item->start() - 1;
                        break;
                    case 3: // after the token
                        qWarning() << "after token";
                        dropRawPosition = item->end() + 1;
                        break;
                    }
                    qWarning() << "CORE" << item->start() << item->end() << dropRawPosition;
                }
            }
        }

        TokenEditor editor(this);
        editor.setTokenName(event->mimeData()->data(Constants::TOKENNAME_MIME));
        int r = editor.exec();
        if (r == QDialog::Accepted) {
            setFocus();
            qWarning() << "INSERT" << "rawPos" << dropRawPosition << "outputPos" << this->cursorForPosition(event->pos()).position();
            cursor.setPosition(dropRawPosition);
            cursor.insertHtml(editor.toRawSourceHtml());
            //            d->_pad->softReset();
            event->acceptProposedAction();
            event->accept();
            return;
        }
    }
    event->ignore();
}

bool TokenOutputDocument::event(QEvent *event)
{
    static int i = 0;
    if (!d->_pad)
        return Editor::TextEditor::event(event);

    if (event->type()==QEvent::HoverMove) {
        /** \todo improve PadCore highlighting */
        QHoverEvent *me = static_cast<QHoverEvent*>(event);
        int position = cursorForPosition(me->pos()).position();
        if (d->_lastHoveredItem) {
            if (d->_lastHoveredItem->containsOutputPosition(position))
                return true;
        }
        PadItem *item = d->_pad->padItemForOutputPosition(position);
        QTextDocument *doc = document();
        if (!item) {
            if (d->_lastHoveredItem) {
                Constants::removePadFragmentFormat("Hover", doc, d->_lastHoveredItemCharFormats);
                Constants::removePadFragmentFormat("CoreH", doc, d->_lastHoveredTokenCoreCharFormats);
                d->_lastHoveredItem = 0;
            }
            return Editor::TextEditor::event(event);
        }

        if (d->_lastHoveredItem) {
            if (d->_lastHoveredItem == item)
                return true;
            Constants::removePadFragmentFormat("Hover", doc, d->_lastHoveredItemCharFormats);
            Constants::removePadFragmentFormat("CoreH", doc, d->_lastHoveredTokenCoreCharFormats);
            d->_lastHoveredItem = item;
        } else {
            d->_lastHoveredItem = item;
        }

        if (d->_lastHoveredItem->getCore())
            Constants::setPadFragmentFormat("CoreH", d->_lastHoveredItem->getCore()->outputStart(), d->_lastHoveredItem->getCore()->outputEnd(), doc, d->_lastHoveredTokenCoreCharFormats, d->_hoveredTokenCoreCharFormat);
        Constants::setPadFragmentFormat("Hover", d->_lastHoveredItem->outputStart(), d->_lastHoveredItem->outputEnd(), doc, d->_lastHoveredItemCharFormats, d->_hoveredCharFormat);
        me->accept();
        return Editor::TextEditor::event(event);
    } else if (event->type()==QEvent::HoverLeave && d->_lastHoveredItem) {
        Constants::removePadFragmentFormat("Hover", document(), d->_lastHoveredItemCharFormats);
        Constants::removePadFragmentFormat("CoreH", document(), d->_lastHoveredTokenCoreCharFormats);
        d->_lastHoveredItem = 0;
        event->accept();
        return Editor::TextEditor::event(event);
    }

    return Editor::TextEditor::event(event);;
}

bool TokenOutputDocument::eventFilter(QObject *o, QEvent *e)
{
    if (!d->_pad || o!=textEdit()->viewport())
        return false;

    if (e->type()==QEvent::MouseButtonDblClick) {
        // get the PadItem under mouse
        QMouseEvent *me = static_cast<QMouseEvent*>(e);
        setTextCursor(cursorForPosition(me->pos()));
        editTokenUnderCursor();
    }

    return QWidget::eventFilter(o, e);
}
