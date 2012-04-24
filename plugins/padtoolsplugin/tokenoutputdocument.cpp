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

    // Correct the position of the DropEvent when it appends inside a PadCore
    int correctDropPosition(int pos)
    {
//        qWarning() << "correctDropPosition" << pos;
        PadCore *core = dynamic_cast<PadCore*>(_pad->padFragmentForOutputPosition(pos));
        Q_ASSERT(core);
        if (!core) {
            LOG_ERROR_FOR("TokenOutputDocument", "No core ???");
            return -1;
        }
        PadItem *item = dynamic_cast<PadItem *>(core->parent());
        Q_ASSERT(item);
        if (!item) {
            LOG_ERROR_FOR("TokenOutputDocument", "No item ???");
            return -1;
        }

        QStringList buttons;
        buttons << QApplication::translate(Constants::PADWRITER_TRANS_CONTEXT, Constants::INSIDE_CONDITIONNAL_BEFORE_TEXT);
        buttons << QApplication::translate(Constants::PADWRITER_TRANS_CONTEXT, Constants::INSIDE_CONDITIONNAL_AFTER_TEXT);
        buttons << QApplication::translate(Constants::PADWRITER_TRANS_CONTEXT, Constants::BEFORE_TOKEN);
        buttons << QApplication::translate(Constants::PADWRITER_TRANS_CONTEXT, Constants::AFTER_TOKEN);
        int s = Utils::withButtonsMessageBox(QApplication::translate(Constants::PADWRITER_TRANS_CONTEXT, Constants::NESTED_TOKEN),
                                             QApplication::translate(Constants::PADWRITER_TRANS_CONTEXT,
                                                                     "You have dropped a token inside the value of a token. \n"
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
            pos = core->start() - 1;
            break;
        case 1: // inside after conditionnal
            pos = core->end() + 1;
            break;
        case 2: // before the token
            pos = item->start() - 1;
            break;
        case 3: // after the token
            pos = item->end() + 1;
            break;
        }
        return pos;
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

    bool isNavigationKey(QKeyEvent *k)
    {
        switch (k->key()) {
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_Up:
        case Qt::Key_Down:
        case Qt::Key_PageDown:
        case Qt::Key_PageUp:
        case Qt::Key_Home:
        case Qt::Key_End:
            return true;
        }
        return false;
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
    Editor::TextEditor(parent, TokenOutputDocument::Simple | TokenOutputDocument::WithTables),
    d(new Internal::TokenOutputDocumentPrivate)
{
//    d->_textControl = new Internal::TokenOutputDocumentControl(this);
//    this->setTextControl(d->_textControl);
    setAcceptDrops(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
    textEdit()->viewport()->installEventFilter(this);
    textEdit()->installEventFilter(this);
    textEdit()->setAttribute(Qt::WA_Hover);
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
    disconnectOutputDocumentChanges();
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
        /** manage nested tokens */
        TokenEditor editor(this);
        PadCore *core = item->getCore();
        editor.setTokenName(core->name());
        PadFragment *bef = item->subItem(PadConditionnalSubItem::Defined, PadConditionnalSubItem::Prepend);
        PadFragment *aft = item->subItem(PadConditionnalSubItem::Defined, PadConditionnalSubItem::Append);
        editor.setConditionnalHtml(d->_pad->fragmentHtmlOutput(bef), d->_pad->fragmentHtmlOutput(aft));
        if (editor.exec()==QDialog::Accepted) {
            // Remove old PadItem positions
            textEdit()->document()->blockSignals(true);
            QTextCursor cursor = textCursor();
            cursor.setPosition(item->outputStart());
            cursor.setPosition(item->outputEnd(), QTextCursor::KeepAnchor);
            cursor.removeSelectedText();

            // Save parent && id of the item
            PadFragment *parent = item->parent();
            if (parent)
                parent->removeChild(item);
            int id = item->id();
//            d->_pad->debug();

            // Modify output position for all subitemfragments
            int oldLength = item->outputLength();
            int oldStart = item->outputStart();

            QString html;
            editor.getOutput(html, *item, item->outputStart());
            bef = item->subItem(PadConditionnalSubItem::Defined, PadConditionnalSubItem::Prepend);
            aft = item->subItem(PadConditionnalSubItem::Defined, PadConditionnalSubItem::Append);

            // insert token length to the PadDocument
            int deltaLength = (item->outputLength() - oldLength);
//            qWarning() << "posChanged" << oldStart << oldStart + deltaLength << deltaLength;
            d->_pad->outputPosChanged(oldStart, oldStart + deltaLength);

            // insert html
            cursor.setPosition(item->outputStart());
            cursor.insertHtml(html);
            textEdit()->document()->blockSignals(false);

            item->setParent(parent);
            if (parent) {
                parent->addChild(item);
                parent->sortChildren();
            }
            item->setId(id);

            onDocumentAnalyzeReset();

//            d->_pad->debug();

        }
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
        textEdit()->blockSignals(true);
        // Select the core
        PadCore *core = dynamic_cast<PadCore*>(d->_pad->padFragmentForOutputPosition(textCursor().position()));
        QTextCursor cursor(textEdit()->document());
        cursor.clearSelection();
        cursor.setPosition(core->outputStart());
        cursor.setPosition(core->outputEnd(), QTextCursor::KeepAnchor);
        textEdit()->setTextCursor(cursor);
        textEdit()->blockSignals(false);
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
        QTextCursor cursor = textEdit()->textCursor();
        int pos = cursor.position();
//        qWarning() << "DROP AT OUTPUT:" << pos;

        // drop inside a PadCore ?
        if (d->isPadCoreAt(pos))
            pos = d->correctDropPosition(pos);

        // start token editor dialog
        TokenEditor editor(this);
        editor.setTokenName(event->mimeData()->data(Constants::TOKENNAME_MIME));
        int r = editor.exec();
        if (r == QDialog::Accepted) {

            // get the content of the editor
            setFocus();
            QString html;
            PadItem *item = new PadItem;
            editor.getOutput(html, *item, pos);

            // insert token length to the PadDocument
            d->_pad->outputPosChanged(item->outputStart(), item->outputStart() + item->outputLength());

            // retrieve the item parent
            PadFragment *parent = d->_pad->padFragmentForOutputPosition(pos);
            if (parent) {
                parent->addChild(item);
            } else {
                d->_pad->addChild(item);
            }

            // insert item text in the output document
            textEdit()->document()->blockSignals(true);
            cursor.setPosition(pos);
            cursor.insertHtml(html);
            // create the extraselection for the new item
            d->_pad->sortChildren();
            onDocumentAnalyzeReset();
            textEdit()->document()->blockSignals(false);

            // Manage event
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
        if (e->type()==QEvent::HoverMove) {
            /** \todo improve PadCore highlighting */
            QHoverEvent *me = static_cast<QHoverEvent*>(e);
            // compute pos
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
                return QWidget::eventFilter(o, e);
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
            me->accept();
            return true;
        } else if (e->type()==QEvent::HoverLeave && d->_lastHoveredItem) {
            textEdit()->setExtraSelections(QList<QTextEdit::ExtraSelection>());
            d->_lastHoveredItem = 0;
            e->accept();
            return true;
        }
        if (e->type() == QEvent::InputMethod) {
            // Avoid edition when cursor is inside a padCore
            QInputMethodEvent *input = static_cast<QInputMethodEvent*>(e);
            if (!input)
                return false;
            QTextCursor cursor = textEdit()->textCursor();
            if (cursor.hasSelection()) {
                if (d->isPadCoreAt(cursor.selectionStart())) {
                    // reject event
                    e->ignore();
                    return true;
                }
            } else if (d->isPadCoreAt(cursor.position())) {
                // reject event
                e->ignore();
                return true;
            }
        }
        if (e->type() == QEvent::KeyPress) {
            QTextCursor cursor = textEdit()->textCursor();
            QKeyEvent *kevent = static_cast<QKeyEvent*>(e);
            if (!kevent)
                return false;
//            qWarning() << kevent->key() << kevent->modifiers() << kevent->text() << d->isNavigationKey(kevent);

            textEdit()->setExtraSelections(QList<QTextEdit::ExtraSelection>());
            // cursor inside a token ?
            if (cursor.hasSelection() && d->isPadCoreAt(cursor.selectionStart())) {
                if (!d->isNavigationKey(kevent)) {
                    e->ignore();
                    return true;
                }
            }
            switch (kevent->key()) {
            case Qt::Key_Left:  // Manage tokens
            {
                if (cursor.hasSelection() && d->isPadCoreAt(cursor.selectionStart())) {
                    // move cursor before token start
                    cursor.setPosition(cursor.selectionStart() - 1);
                    textEdit()->setTextCursor(cursor);
                    // reject
                    e->ignore();
                    return true;
                }
                break;
            }
            case Qt::Key_Right:  // Manage tokens
            {
                if (cursor.hasSelection() && d->isPadCoreAt(cursor.selectionStart())) {
                    // move cursor before token start
                    cursor.setPosition(cursor.selectionEnd());
                    textEdit()->setTextCursor(cursor);
                    // reject
                    e->ignore();
                    return true;
                }
                break;
            }
            case Qt::Key_Backspace:
            {
//                qWarning() << "BACKSPACE PRESSED" << cursor.position() << "autorepeat" << kevent->isAutoRepeat();
                int newPosition = cursor.position() - 1;
                if (d->isPadCoreAt(newPosition)) {
                    if (d->userWantsToDeletePadItem(newPosition)) {
                        PadItem *item = d->_pad->padItemForOutputPosition(newPosition);
                        d->_pad->removeAndDeleteFragment(item);
                        e->accept();
                        return true;
                    } else {
                        e->ignore();
                        return false;
                    }
                }
                return false;
            }
            case Qt::Key_Delete:
            {
//                qWarning() << "DEL PRESSED" << cursor.position() << kevent->isAutoRepeat();
                int newPosition = cursor.position() + 1;
                if (d->isPadCoreAt(newPosition)) {
                    if (d->userWantsToDeletePadItem(newPosition)) {
                        PadItem *item = d->_pad->padItemForOutputPosition(newPosition);
                        d->_pad->removeAndDeleteFragment(item);
                        e->accept();
                        return true;
                    } else {
                        e->ignore();
                        return false;
                    }
                }
                return false;
            }
            default:
                return false;
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
    connect(textEdit(), SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
    connect(textEdit(), SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));
    connect(textEdit()->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(contentChanged(int,int,int)));
}

void TokenOutputDocument::disconnectOutputDocumentChanges()
{
    disconnect(textEdit(), SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
    disconnect(textEdit(), SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));
    disconnect(textEdit()->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(contentChanged(int,int,int)));
}

/** Keep PadDocument sync to the output QTextDocument */
void TokenOutputDocument::contentChanged(const int pos, const int rm, const int ins)
{
    qWarning() << "contentChanged pos" << pos << "ins" << ins << "rm" << rm;

    /** \todo improve management of textformatting modifications -> do not re-compute positions */
    if (rm==ins)
        return;

    if (rm)
        d->_pad->outputPosChanged(pos+rm, pos);

    if (ins)
        d->_pad->outputPosChanged(pos, pos+ins);

    onDocumentAnalyzeReset();
}
