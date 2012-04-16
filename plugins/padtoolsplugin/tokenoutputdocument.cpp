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

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>

#include <QLinearGradient>
#include <QGradientStops>
#include <QMenu>
#include <QKeyEvent>
#include <QApplication>

#include <QDebug>

using namespace PadTools;
using namespace Trans::ConstantTranslations;

namespace PadTools {
namespace Internal {

//TokenOutputDocumentControl::TokenOutputDocumentControl(QObject *parent)
//{}

//TokenOutputDocumentControl::~TokenOutputDocumentControl()
//{}

//bool TokenOutputDocumentControl::canInsertFromMimeData (const QMimeData *source) const
//{
//    WARN_FUNC;
//}

//QMimeData *TokenOutputDocumentControl::createMimeDataFromSelection() const
//{
//    WARN_FUNC;
//    return new QMimeData();
//}

//void TokenOutputDocumentControl::insertFromMimeData(const QMimeData *source)
//{
//    WARN_FUNC;
//}


class TokenOutputDocumentPrivate
{
public:
    TokenOutputDocumentPrivate() :
        _pad(0),
        _lastHoveredItem(0)
    {
//        qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e7effd, stop: 1 #cbdaf1);
//        QLinearGradient grad(QPointF(0,0), QPointF(0,1));
//        grad.setColorAt(0, QColor(Qt::lightGray));
//        grad.setColorAt(0.8, Qt::white);
//        grad.setColorAt(1, QColor(Qt::lightGray));
//        QBrush brush(QColor("#eeeeee"));
//        _hoveredCharFormat.setBackground(grad);

        _hoveredTokenCoreCharFormat.setUnderlineColor(Qt::darkBlue);
        _hoveredTokenCoreCharFormat.setUnderlineStyle(QTextCharFormat::WaveUnderline);

        _coreFormat.setBackground(QBrush(QColor(Qt::yellow)));//"#FFF8C6"))); //("#dedeff")));
        _tokenFormat.setBackground(QBrush(QColor(Qt::lightGray)));//"#FDEEF4"))); //"#efefef")));
    }

    bool posIsEditable(const int pos)
    {
        return !isPadCoreAt(pos);
    }

    bool isPadCoreAt(const int pos)
    {
        PadCore *core = dynamic_cast<PadCore*>(_pad->padFragmentForOutputPosition(pos));
        return (core!=0);
    }

    bool userWantsToDeletePadItem(int pos)
    {
        PadCore *core = dynamic_cast<PadCore*>(_pad->padFragmentForOutputPosition(pos));
        if (!core)
            return false;
        bool yes = Utils::yesNoMessageBox(QApplication::translate(Constants::PADWRITER_TRANS_CONTEXT, "Remove token “<b>%1</b>”?").arg(core->name()),
                                          QApplication::translate(Constants::PADWRITER_TRANS_CONTEXT, "You are about to remove token: “<b>%1</b>”. "
                                                                  "Do you really want to continue?").arg(core->name()));
        return yes;
    }

    void itemToExtraSelection(PadItem *item)
    {
        QTextEdit::ExtraSelection sel;
        // get token's core
        PadCore *core = item->getCore();
        if (core) {
            if (item->outputStart() == item->outputEnd())
                return;
            QTextCursor c1(_pad->outputDocument());
            c1.setPosition(item->outputStart());
            c1.setPosition(core->outputStart(), QTextCursor::KeepAnchor);
            sel.cursor = c1;
            sel.format = _tokenFormat;
            _tokenExtraSelection.insertMulti(item, sel);

            QTextCursor c2(_pad->outputDocument());
            c2.setPosition(core->outputStart());
            c2.setPosition(core->outputEnd(), QTextCursor::KeepAnchor);
            sel.cursor = c2;
            sel.format = _coreFormat;
            _tokenExtraSelection.insertMulti(item, sel);

            QTextCursor c3(_pad->outputDocument());
            c3.setPosition(core->outputEnd());
            c3.setPosition(item->outputEnd(), QTextCursor::KeepAnchor);
            sel.cursor = c3;
            sel.format = _tokenFormat;
            _tokenExtraSelection.insertMulti(item, sel);
        } else {
            QTextCursor c(_pad->outputDocument());
            c.setPosition(item->outputStart());
            c.setPosition(item->outputEnd(), QTextCursor::KeepAnchor);
            sel.cursor = c;
            sel.format = _tokenFormat;
            _tokenExtraSelection.insertMulti(item, sel);
        }
    }

public:
    PadDocument *_pad;
    PadItem *_lastHoveredItem; // should not be deleted
//    TokenOutputDocumentControl *_textControl;
    QList<QTextCharFormat> _lastHoveredItemCharFormats, _lastHoveredTokenCoreCharFormats;
    QTextCharFormat _hoveredCharFormat;
    QTextCharFormat _hoveredTokenCoreCharFormat;
    QTextFrameFormat _hoveredFrameFormat;
    QTextCharFormat _coreFormat;
    QTextCharFormat _tokenFormat;
    QMultiMap<PadItem *, QTextEdit::ExtraSelection> _tokenExtraSelection;
};
}
}

TokenOutputDocument::TokenOutputDocument(QWidget *parent) :
    Editor::TextEditor(parent, TokenOutputDocument::Simple),
    d(new Internal::TokenOutputDocumentPrivate)
{
//    d->_textControl = new Internal::TokenOutputDocumentControl(this);
//    this->setTextControl(d->_textControl);
    setAttribute(Qt::WA_Hover);
    setAcceptDrops(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
    textEdit()->viewport()->installEventFilter(this);
    textEdit()->installEventFilter(this);
    connect(textEdit(), SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
    connect(textEdit(), SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));
}

TokenOutputDocument::~TokenOutputDocument()
{
    if (d) {
        delete d;
        d = 0;
    }
}

/** Define the PadTools::PadDocument to use with this object */
void TokenOutputDocument::setPadDocument(PadDocument *pad)
{
    if (d->_pad) {
        disconnectPadDocument();
    }
    d->_pad = pad;
    textEdit()->setDocument(d->_pad->outputDocument());
    connectPadDocument();
    connectOutputDocumentChanges();
    onDocumentAnalyzeReset();
}

/** Manage PadTools::PadDocument clear signal */
void TokenOutputDocument::onPadCleared()
{
    d->_tokenExtraSelection.clear();
    if (!d->_lastHoveredItem)
        return;
//    Constants::removePadFragmentFormat("Hover", document(), d->_lastHoveredItemCharFormats);
    d->_lastHoveredItem = 0;
}

/** Reset view when Padtools::PadDocument reset its token analyze. \sa PadTools::PadDocument::documentAnalyzeReset() */
void TokenOutputDocument::onDocumentAnalyzeReset()
{
    // Create extraSelections for the tokens
    textEdit()->setExtraSelections(QList<QTextEdit::ExtraSelection>());
    d->_tokenExtraSelection.clear();
    d->_lastHoveredItem = 0;
    foreach(PadItem *f, d->_pad->padItems()) {
        d->itemToExtraSelection(f);
    }
}

/** Overwrite the context menu (add token editor action is mouse under a PadTools::PadItem */
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

/** Start edition of the PadTools::PadItem under the QTextCursor */
void TokenOutputDocument::editTokenUnderCursor()
{
    if (!d->_pad)
        return;

    int position = textCursor().position();
    PadItem *item = d->_pad->padItemForOutputPosition(position);
    if (item) {
        /** \todo recode here: no mode PadItem::fragment() */
//        TokenEditor editor(this);
//        PadFragment *f = item->getCore();
//        editor.setTokenName(d->_pad->fragmentRawSource(f));
//        PadFragment *bef = item->fragment(PadItem::DefinedCore_PrependText);
//        PadFragment *aft = item->fragment(PadItem::DefinedCore_AppendText);
//        editor.setConditionnalHtml(d->_pad->fragmentHtmlOutput(bef), d->_pad->fragmentHtmlOutput(aft));
//        if (editor.exec()==QDialog::Accepted) {
//            // Remove rawSource PadItem
//            QTextCursor cursor(d->_pad->rawSourceDocument());
//            cursor.setPosition(item->start());
//            cursor.setPosition(item->end(), QTextCursor::KeepAnchor);
//            cursor.removeSelectedText();
//            // Insert new Item in rawSource
//            cursor.setPosition(item->start());
//            cursor.insertHtml(editor.toRawSourceHtml());
//            // Reset _pad
////            d->_pad->softReset();
//        }
    }
}

/** Manages the text selection according to the tokens. */
void TokenOutputDocument::selectionChanged()
{
    // mirror the selection in rawSource document
    QTextCursor output(document());
    if (output.hasSelection()) {
        int start = d->_pad->positionTranslator().outputToRaw(output.selectionStart());
        int end = d->_pad->positionTranslator().outputToRaw(output.selectionEnd());
        QTextCursor cursor(d->_pad->rawSourceDocument());
        cursor.setPosition(start);
        cursor.setPosition(end, QTextCursor::KeepAnchor);
    }
}

void TokenOutputDocument::cursorPositionChanged()
{
    qWarning() << "cursor moves" << textCursor().position() << d->isPadCoreAt(textCursor().position());
    if (d->isPadCoreAt(textCursor().position())) {
        // Select the core
        PadCore *core = dynamic_cast<PadCore*>(d->_pad->padFragmentForOutputPosition(textCursor().position()));
        QTextCursor cursor(textEdit()->document());
        cursor.clearSelection();
        cursor.setPosition(core->outputStart());
        cursor.setPosition(core->outputEnd(), QTextCursor::KeepAnchor);
        textEdit()->setTextCursor(cursor);
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

    if (textEdit()->underMouse()) {
        // Where did the user drop the token ?
        QTextCursor cursor = d->_pad->rawSourceCursorForOutputPosition(this->cursorForPosition(event->pos()).position());
        int pos = cursor.position();
        int dropRawPosition = pos;

//        qWarning() << "DROP AT OUTPUT:" << this->cursorForPosition(event->pos()).position() << "RAW" << pos;

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
//            qWarning() << "INSERT" << "rawPos" << dropRawPosition << "outputPos" << this->cursorForPosition(event->pos()).position();
            cursor.setPosition(dropRawPosition);
            cursor.insertHtml(editor.toRawSourceHtml());
            d->_pad->softReset();
            textEdit()->setDocument(d->_pad->outputDocument());
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
        return Editor::TextEditor::event(event);

    QTime c;
    c.start();

    if (event->type()==QEvent::HoverMove) {
        /** \todo improve PadCore highlighting */
        QHoverEvent *me = static_cast<QHoverEvent*>(event);
        int position = cursorForPosition(me->pos()).position();
        if (d->_lastHoveredItem) {
            if (d->_lastHoveredItem->containsOutputPosition(position))
                return true;
        }
        PadItem *item = d->_pad->padItemForOutputPosition(position);
        if (!item) {
            if (d->_lastHoveredItem) {
                textEdit()->setExtraSelections(QList<QTextEdit::ExtraSelection>());
                d->_lastHoveredItem = 0;
            }
            return Editor::TextEditor::event(event);
        }

        if (d->_lastHoveredItem) {
            if (d->_lastHoveredItem == item)
                return true;
            textEdit()->setExtraSelections(QList<QTextEdit::ExtraSelection>());
            d->_lastHoveredItem = item;
        } else {
            d->_lastHoveredItem = item;
        }
        textEdit()->setExtraSelections(d->_tokenExtraSelection.values(item));

        Utils::Log::logTimeElapsed(c, "TokenOutputDocument", "Hover");
        me->accept();
        return Editor::TextEditor::event(event);
    } else if (event->type()==QEvent::HoverLeave && d->_lastHoveredItem) {
        textEdit()->setExtraSelections(QList<QTextEdit::ExtraSelection>());
        d->_lastHoveredItem = 0;
        event->accept();
        return Editor::TextEditor::event(event);
    }

    return Editor::TextEditor::event(event);;
}

bool TokenOutputDocument::eventFilter(QObject *o, QEvent *e)
{
    if (!d->_pad)
        return false;

    // Catch mouse actions on viewport
    if (o==textEdit()->viewport()) {
        if (e->type()==QEvent::MouseButtonDblClick) {
            // get the PadItem under mouse
            QMouseEvent *me = static_cast<QMouseEvent*>(e);
            setTextCursor(cursorForPosition(me->pos()));
            editTokenUnderCursor();
        }
        return QWidget::eventFilter(o, e);
    }

    // Catch KeyEvent in QTextEdit
    if (o==textEdit()) {
        if (e->type() == QEvent::KeyPress) {
            QTextCursor cursor = textEdit()->textCursor();
            QKeyEvent *kevent = static_cast<QKeyEvent*>(e);
            if (!kevent)
                return false;

            textEdit()->setExtraSelections(QList<QTextEdit::ExtraSelection>());

            switch (kevent->key()) {
            case Qt::Key_Backspace:
            {
                qWarning() << "BACKSPACE PRESSED" << cursor.position() << "autorepeat" << kevent->isAutoRepeat();
                if (kevent->isAutoRepeat()) {
                    // if the next deleted char == PadCore
                    // ask user if he wants to totally removed the PadItem
                    //                int pos = l->cursorPosition();
                    //                removeChar(currentId, pos);
                    //                --pos;
                    //                if (pos==0) --pos;
                    //                setCursorPosition(currentId, pos);
                } else {
                    int newPosition = cursor.position() - 1;
                    qWarning() << "BACKSPACE PRESSED: newpos" << newPosition << "isPadCore" << d->isPadCoreAt(newPosition);
                    if (d->isPadCoreAt(newPosition)) {
                        if (d->userWantsToDeletePadItem(newPosition)) {
                            PadItem *item = d->_pad->padItemForOutputPosition(newPosition);
                            d->_pad->removeAndDeleteFragment(item);
        //                    d->_pad->softReset();
                            e->accept();
                            return true;
                        } else {
                            e->ignore();
                            return true;
                        }
                    } else {
                        return false;
                    }
                }
            }
            case Qt::Key_Delete:
                qWarning() << "DEL PRESSED" << cursor.position() << kevent->isAutoRepeat();
                if (kevent->isAutoRepeat()) {
                    // if the next deleted char == PadCore
                    // ask user if he wants to totally removed the PadItem
                    //                int pos = l->cursorPosition();
                    //                ++pos;
                    //                removeChar(currentId, pos);
                    //                setCursorPosition(currentId, pos-1);
                }
                e->ignore();
                return true;
            default:
//                qWarning() << "DEFAULT PRESS KEY" << d->isEditKey(kevent);
                break;
            }
        } else if (e->type() == QEvent::KeyRelease) {
            QTextCursor cursor = textEdit()->textCursor();
            QKeyEvent *kevent = static_cast<QKeyEvent*>(e);
            if (!kevent)
                return false;

            switch (kevent->key()) {
            case Qt::Key_Backspace:
            {
                qWarning() << "BACKSPACE RELEASED" << cursor.position() << kevent->isAutoRepeat();
                int newPosition = cursor.position()-1;
                if (d->isPadCoreAt(newPosition)) {
                    if (d->userWantsToDeletePadItem(newPosition)) {
                        PadItem *item = d->_pad->padItemForOutputPosition(newPosition);
                        d->_pad->removeAndDeleteFragment(item);
    //                    d->_pad->softReset();
                    }
                } else {
                    return QWidget::eventFilter(o, e);
                }
                return true;
            }
            case Qt::Key_Delete:
                qWarning() << "DEL RELEASED" << cursor.position() << kevent->isAutoRepeat();
                if (kevent->isAutoRepeat()) {
                    // if the next deleted char == PadCore
                    // ask user if he wants to totally removed the PadItem
                    //                int pos = l->cursorPosition();
                    //                ++pos;
                    //                removeChar(currentId, pos);
                    //                setCursorPosition(currentId, pos-1);
                }
                e->ignore();
                return true;
            default:
            {
//                qWarning() << "DEFAULT RELEASE KEY" << d->isEditKey(kevent);
                if (!d->posIsEditable(cursor.position())) {
                    // send the event to the textedit
                    QWidget::eventFilter(o,e);
                    if (textCursor().position() > cursor.position()) {
                        // Remove edition
                        cursor.setPosition(textCursor().position(), QTextCursor::KeepAnchor);
                        cursor.removeSelectedText();
                        return true;
                    }
                }

                break;
            }
            }
        }
        }
    return QWidget::eventFilter(o, e);
}

void TokenOutputDocument::connectPadDocument()
{
    connect(d->_pad, SIGNAL(cleared()), this, SLOT(onPadCleared()));

    connect(d->_pad, SIGNAL(aboutToClear()), this, SLOT(disconnectOutputDocumentChanges()));
    connect(d->_pad, SIGNAL(cleared()), this, SLOT(connectOutputDocumentChanges()));

    connect(d->_pad, SIGNAL(rawSourceAnalyzeStarted()), this, SLOT(disconnectOutputDocumentChanges()));
    connect(d->_pad, SIGNAL(rawSourceAnalyseFinished()), this, SLOT(connectOutputDocumentChanges()));

    connect(d->_pad, SIGNAL(beginTokenReplacement()), this, SLOT(disconnectOutputDocumentChanges()));
    connect(d->_pad, SIGNAL(endTokenReplacement()), this, SLOT(connectOutputDocumentChanges()));
    connect(d->_pad, SIGNAL(endTokenReplacement()), this, SLOT(onDocumentAnalyzeReset()));
}

void TokenOutputDocument::disconnectPadDocument()
{
    disconnect(d->_pad, SIGNAL(cleared()), this, SLOT(onPadCleared()));

    disconnect(d->_pad, SIGNAL(aboutToClear()), this, SLOT(disconnectOutputDocumentChanges()));
    disconnect(d->_pad, SIGNAL(cleared()), this, SLOT(connectOutputDocumentChanges()));

    disconnect(d->_pad, SIGNAL(rawSourceAnalyzeStarted()), this, SLOT(disconnectOutputDocumentChanges()));
    disconnect(d->_pad, SIGNAL(rawSourceAnalyseFinished()), this, SLOT(connectOutputDocumentChanges()));

    disconnect(d->_pad, SIGNAL(beginTokenReplacement()), this, SLOT(disconnectOutputDocumentChanges()));
    disconnect(d->_pad, SIGNAL(endTokenReplacement()), this, SLOT(connectOutputDocumentChanges()));
    disconnect(d->_pad, SIGNAL(endTokenReplacement()), this, SLOT(onDocumentAnalyzeReset()));
}

void TokenOutputDocument::connectOutputDocumentChanges()
{
    connect(textEdit()->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(contentChanged(int,int,int)));
}

void TokenOutputDocument::disconnectOutputDocumentChanges()
{
    disconnect(textEdit()->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(contentChanged(int,int,int)));
}

/** Keep PadDocument sync to the output QTextDocument */
void TokenOutputDocument::contentChanged(const int pos, const int rm, const int ins)
{
    qWarning() << "contentChanged pos" << pos << "ins" << ins << "rm" << rm;

    /** \todo manage textformatting modifications -> do not re-compute positions */
    if (rm)
        d->_pad->outputPosChanged(pos, pos-rm);

    if (ins)
        d->_pad->outputPosChanged(pos, pos+ins);

    onDocumentAnalyzeReset();
}
