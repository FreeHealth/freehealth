/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developers:  Eric Maeker <eric.maeker@gmail.com>                  *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
/**
 * \class PadTools::TokenOutputDocument
 * Text editor that allow user to interact with the output text of a PadDocument.\n
 * PadItem are highlighted when user hover on them, and the cursor can be modified too.
*/

#include "tokenoutputdocument.h"
#include "constants.h"
#include "tokeneditor.h"
#include "pad_document.h"
#include "pad_item.h"
#include "pad_fragment.h"

#include <coreplugin/icore.h>
#include <coreplugin/ipadtools.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/stylehelper.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>

#include <QLinearGradient>
#include <QGradientStops>
#include <QMenu>
#include <QKeyEvent>
#include <QApplication>
#include <QToolTip>
#include <QMimeData>

#include <QDebug>

using namespace PadTools;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ITokenPool *tokenPool() {return Core::ICore::instance()->padTools()->tokenPool();}

namespace PadTools {
namespace Internal {
class TokenHighlighterEditorPrivate
{
public:
    TokenHighlighterEditorPrivate(TokenHighlighterEditor */*parent*/):
        _pad(0),
        _lastHoveredItem(0),
        _lastUnderCursorItem(0) // , q(parent)
    {
        // Using gnome palette: http://openclipart.org/detail/25332/gnome-color-palette-by-anonymous-25332
        QColor core("#E0C39E");
        _coreFormat.setBackground(core);
        _coreFormat.setUnderlineStyle(QTextCharFormat::DashUnderline);
        _coreFormat.setUnderlineColor(QColor(Qt::darkRed));
//        _coreFormat.setFontOverline(true);

        QColor token("#EFE0CD");
        _tokenFormat.setBackground(token);
        _tokenFormat.setUnderlineStyle(QTextCharFormat::DashUnderline);
        _tokenFormat.setUnderlineColor(QColor(Qt::darkBlue));
    }

    ~TokenHighlighterEditorPrivate()
    {}

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
    PadItem *_lastHoveredItem, *_lastUnderCursorItem; // must not be deleted
    QTextCharFormat _coreFormat;
    QTextCharFormat _tokenFormat;
    QMultiMap<PadItem *, QTextEdit::ExtraSelection> _tokenExtraSelection;

private:
    // TokenHighlighterEditor *q;
};

class TokenOutputDocumentPrivate
{
public:
    TokenOutputDocumentPrivate(TokenOutputDocument *parent) :
//        _changeCursor(true),
        q(parent)
    {
    }

    bool posIsEditable(const int pos)
    {
        return !q->isPadCore(pos);
    }

    bool userWantsToDeletePadItem(int pos)
    {
        PadCore *core = dynamic_cast<PadCore*>(q->padDocument()->padFragmentForOutputPosition(pos));
        if (!core)
            return false;
        bool yes = Utils::yesNoMessageBox(QApplication::translate(Constants::PADWRITER_TRANS_CONTEXT, "Remove token “<b>%1</b>”?").arg(core->uid()),
                                          QApplication::translate(Constants::PADWRITER_TRANS_CONTEXT, "You are about to remove token: “<b>%1</b>”. "
                                                                  "Do you really want to continue?").arg(core->uid()));
        return yes;
    }

    // Correct the position of the DropEvent when it appends inside a PadCore
    int correctDropPosition(int pos)
    {
        PadCore *core = dynamic_cast<PadCore*>(q->padDocument()->padFragmentForOutputPosition(pos));
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
                                                                     "- inside the conditional text before the token, \n"
                                                                     "- inside the conditional text after the token \n"
                                                                     "- or the before/after the token"),
                                             "",
                                             buttons, "",
                                             // with cancel
                                             true
                                             );
        switch (s) {
        case 0: // inside before conditional
            pos = core->start() - 1;
            break;
        case 1: // inside after conditional
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
//    bool _changeCursor;

private:
    TokenOutputDocument *q;
};
}
}

TokenHighlighterEditor::TokenHighlighterEditor(QWidget *parent) :
    Editor::TextEditor(parent, TokenOutputDocument::Simple | TokenOutputDocument::WithTables),
    d_th(new TokenHighlighterEditorPrivate(this))
{
    textEdit()->viewport()->installEventFilter(this);
    textEdit()->installEventFilter(this);
    textEdit()->setAttribute(Qt::WA_Hover);
}

TokenHighlighterEditor::~TokenHighlighterEditor()
{
    if (d_th)
        delete d_th;
    d_th = 0;
}

/** Set the PadTools::Internal::PadDocument to use inside the editor */
void TokenHighlighterEditor::setPadDocument(PadDocument *pad)
{
    if (d_th->_pad) {
        disconnectPadDocument();
    }
    disconnectOutputDocumentChanges();
    d_th->_pad = pad;
    textEdit()->setDocument(d_th->_pad->outputDocument());
    connectPadDocument();
    connectOutputDocumentChanges();
    onDocumentAnalyzeReset();
}

/** Returns the current PadTools::Internal::PadDocument pointer in use */
PadDocument *TokenHighlighterEditor::padDocument() const
{
   return d_th->_pad;
}

/** Manage PadTools::PadDocument clear signal */
void TokenHighlighterEditor::onPadCleared()
{
    d_th->_tokenExtraSelection.clear();
    d_th->_lastHoveredItem = 0;
    d_th->_lastUnderCursorItem = 0;
}

/**
 * \internal
 * Reset view when Padtools::PadDocument reset its token analyze.
 * \sa PadTools::PadDocument::documentAnalyzeReset()
 */
void TokenHighlighterEditor::onDocumentAnalyzeReset()
{
    // Create extraSelections for the tokens
    textEdit()->setExtraSelections(QList<QTextEdit::ExtraSelection>());
    d_th->_tokenExtraSelection.clear();
    d_th->_lastHoveredItem = 0;
    foreach(PadItem *f, d_th->_pad->padItems()) {
        d_th->itemToExtraSelection(f);
    }
    // highlight the item under cursor
    d_th->_lastUnderCursorItem = 0;
    cursorPositionChanged();
}

/**
 * \internal
 * Update the current highlighted token on cursor moves
 */
void TokenHighlighterEditor::cursorPositionChanged()
{
//    qWarning() << "cursor moves" << textCursor().position() << "isPadItem" << d->isPadItemAt(textCursor().position());
    if (isPadItem(textCursor().position())) {
        PadItem *item = d_th->_pad->padItemForOutputPosition(textCursor().position());
        if (!d_th->_lastUnderCursorItem) {
            d_th->_lastUnderCursorItem = item;
            if (d_th->_lastUnderCursorItem) {
                textEdit()->setExtraSelections(d_th->_tokenExtraSelection.values(d_th->_lastUnderCursorItem));
                Q_EMIT highlighting(item);
            } else {
                textEdit()->setExtraSelections(QList<QTextEdit::ExtraSelection>());
                Q_EMIT highlighting(0);
            }
        } else {
            if (d_th->_lastUnderCursorItem && d_th->_lastUnderCursorItem != item) {
                d_th->_lastUnderCursorItem = item;
                textEdit()->setExtraSelections(d_th->_tokenExtraSelection.values(d_th->_lastUnderCursorItem));
                Q_EMIT highlighting(item);
            }
        }
    } else {
        d_th->_lastUnderCursorItem = 0;
        textEdit()->setExtraSelections(QList<QTextEdit::ExtraSelection>());
        Q_EMIT highlighting(0);
    }
}

void TokenHighlighterEditor::connectPadDocument()
{
    connect(d_th->_pad, SIGNAL(cleared()), this, SLOT(onPadCleared()));

    connect(d_th->_pad, SIGNAL(aboutToClear()), this, SLOT(disconnectOutputDocumentChanges()));
    connect(d_th->_pad, SIGNAL(cleared()), this, SLOT(connectOutputDocumentChanges()));

    connect(d_th->_pad, SIGNAL(rawSourceAnalyzeStarted()), this, SLOT(disconnectOutputDocumentChanges()));
    connect(d_th->_pad, SIGNAL(rawSourceAnalyseFinished()), this, SLOT(connectOutputDocumentChanges()));

    connect(d_th->_pad, SIGNAL(beginTokenReplacement()), this, SLOT(disconnectOutputDocumentChanges()));
    connect(d_th->_pad, SIGNAL(endTokenReplacement()), this, SLOT(connectOutputDocumentChanges()));
    connect(d_th->_pad, SIGNAL(endTokenReplacement()), this, SLOT(onDocumentAnalyzeReset()));

    connect(d_th->_pad, SIGNAL(padFragmentAboutToRemoved(PadFragment*)), this, SLOT(onPadFragmentAboutToRemoved(PadFragment*)));
}

void TokenHighlighterEditor::disconnectPadDocument()
{
    disconnect(d_th->_pad, SIGNAL(cleared()), this, SLOT(onPadCleared()));

    disconnect(d_th->_pad, SIGNAL(aboutToClear()), this, SLOT(disconnectOutputDocumentChanges()));
    disconnect(d_th->_pad, SIGNAL(cleared()), this, SLOT(connectOutputDocumentChanges()));

    disconnect(d_th->_pad, SIGNAL(rawSourceAnalyzeStarted()), this, SLOT(disconnectOutputDocumentChanges()));
    disconnect(d_th->_pad, SIGNAL(rawSourceAnalyseFinished()), this, SLOT(connectOutputDocumentChanges()));

    disconnect(d_th->_pad, SIGNAL(beginTokenReplacement()), this, SLOT(disconnectOutputDocumentChanges()));
    disconnect(d_th->_pad, SIGNAL(endTokenReplacement()), this, SLOT(connectOutputDocumentChanges()));
    disconnect(d_th->_pad, SIGNAL(endTokenReplacement()), this, SLOT(onDocumentAnalyzeReset()));

    disconnect(d_th->_pad, SIGNAL(padFragmentAboutToRemoved(PadFragment*)), this, SLOT(onPadFragmentAboutToRemoved(PadFragment*)));
}

void TokenHighlighterEditor::connectOutputDocumentChanges()
{
    connect(textEdit(), SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));
    connect(textEdit()->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(contentChanged(int,int,int)));
}

void TokenHighlighterEditor::disconnectOutputDocumentChanges()
{
    disconnect(textEdit(), SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));
    disconnect(textEdit()->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(contentChanged(int,int,int)));
}

/** Keep PadDocument sync to the output QTextDocument */
void TokenHighlighterEditor::contentChanged(const int pos, const int rm, const int ins)
{
//    qWarning() << "contentChanged pos" << pos << "ins" << ins << "rm" << rm;

    // TODO: improve management of textformatting modifications -> do not re-compute positions */
    if (rm==ins)
        return;

    if (rm)
        padDocument()->outputPosChanged(pos+rm, pos);

    if (ins)
        padDocument()->outputPosChanged(pos, pos+ins);

    onDocumentAnalyzeReset();
}

void TokenHighlighterEditor::onPadFragmentAboutToRemoved(PadFragment *)
{
    d_th->_lastHoveredItem = 0;
    d_th->_lastUnderCursorItem = 0;
}

/** Returns \e true if the \e textEditorPos is inside a PadTools::Internal::PadItem */
bool TokenHighlighterEditor::isPadItem(int textEditorPos)
{
    PadItem *item = dynamic_cast<PadItem*>(d_th->_pad->padItemForOutputPosition(textEditorPos));
    return (item!=0);
}

/** Returns \e true if the \e textEditorPos is inside a PadTools::Internal::PadCore */
bool TokenHighlighterEditor::isPadCore(int textEditorPos)
{
    PadCore *core = dynamic_cast<PadCore*>(d_th->_pad->padFragmentForOutputPosition(textEditorPos));
    return (core!=0);
}

/** Highlight a pad item in the editor */
void TokenHighlighterEditor::hightlight(PadItem *item)
{
    // No item or item zero length
    if (!item || (item->outputLength() == 0)) {
        d_th->_lastHoveredItem = 0;
        d_th->_lastUnderCursorItem = 0;
        return;
    }

    // same as last one ? -> exit
    if (d_th->_lastHoveredItem) {
        if (d_th->_lastHoveredItem == item)
            return;
    }
    d_th->_lastHoveredItem = item;

    if (padDocument()->padItems().contains(item)) {
        // item comes from the same PadDocument
        textEdit()->setExtraSelections(d_th->_tokenExtraSelection.values(item));
    } else {
        // item comes from a mirrored PadDocument (find matching the rawSource start/end)
        foreach(PadItem *it, padDocument()->padItems()) {
            if (it->rawLength() == item->rawLength()
                    && it->start() == item->start()
                    && it->end() == item->end()) {
                textEdit()->setExtraSelections(d_th->_tokenExtraSelection.values(it));
                d_th->_lastHoveredItem = it;
                break;
            }
        }
    }

    Q_EMIT highlighting(item);
}

/** Overload QWidget::event() and manage QToolTip event */
bool TokenHighlighterEditor::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTip) {
        QHelpEvent *helpEvent = static_cast<QHelpEvent*>(event);
        int position = cursorForPosition(helpEvent->pos()).position();
        PadItem *item = d_th->_pad->padItemForOutputPosition(position);
        if (item) {
            Core::IToken *token = tokenPool()->token(item->getCore()->uid());
            if (token) {
                QRect rect(QPoint(helpEvent->globalPos().x() -10, helpEvent->globalPos().y() -10),
                           QPoint(helpEvent->globalPos().x() +10, helpEvent->globalPos().y() +10));
                QToolTip::showText(helpEvent->globalPos(), token->tooltip(), this, rect);
                return Editor::TextEditor::event(event);
            }
        }
        QToolTip::hideText();
        event->ignore();
        return true;
    }
    return Editor::TextEditor::event(event);
}

/** Manage Hovering of tokens */
bool TokenHighlighterEditor::eventFilter(QObject *o, QEvent *e)
{
    if (!d_th->_pad)
        return QWidget::eventFilter(o, e);
    if (o!=textEdit())
        return QWidget::eventFilter(o, e);

    switch (e->type()) {
    case QEvent::HoverEnter:
    {
//        qWarning() << "HoverLeave";
        // TODO: code QEvent::HoverEnter?
        return QWidget::eventFilter(o, e);
    }
    case QEvent::HoverMove:
    {
//        qWarning() << "HoverMove";
        // TODO: improve PadCore highlighting
        QHoverEvent *me = static_cast<QHoverEvent*>(e);
        // compute pos
        int position = cursorForPosition(me->pos()).position();
        // do we change of hovered paditem ?
        if (d_th->_lastHoveredItem) {
            if (d_th->_lastHoveredItem->containsOutputPosition(position)) {
                // hover does not change of item
                return true;
            }
        }

        PadItem *item = d_th->_pad->padItemForOutputPosition(position);
        if (!item) {
            // we are outside any paditem
            if (d_th->_lastHoveredItem) {
                // but before we was hovering a paditem
                textEdit()->setExtraSelections(QList<QTextEdit::ExtraSelection>());
                d_th->_lastHoveredItem = 0;
//                if (d_th->_changeCursor)
//                    qApp->restoreOverrideCursor();
            }
            Q_EMIT highlighting(0);
            return QWidget::eventFilter(o, e);
        }

        // we are hovering a paditem
        hightlight(item);
        me->accept();
        return true;
    }
    case QEvent::HoverLeave :
    {
//        qWarning() << "HoverLeave";
        if (d_th->_lastHoveredItem) {
            textEdit()->setExtraSelections(QList<QTextEdit::ExtraSelection>());
            d_th->_lastHoveredItem = 0;
//            if (d_th->_changeCursor)
//                qApp->restoreOverrideCursor();
            Q_EMIT highlighting(0);
            e->accept();
            return true;
        }
        return QWidget::eventFilter(o, e);
    }
    default: break;
    } // switch
    return QWidget::eventFilter(o, e);
}

/** \fn void PadTools::Internal::TokenHighlighterEditor::highlighting(PadItem *item)
 * Signal emitted when a new PadItem is highlighted or \e zero when no paditems are hightlighted.
 */

TokenOutputDocument::TokenOutputDocument(QWidget *parent) :
    TokenHighlighterEditor(parent),
    d(new Internal::TokenOutputDocumentPrivate(this))
{
    setAcceptDrops(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
}

TokenOutputDocument::~TokenOutputDocument()
{
    if (d) {
        delete d;
        d = 0;
    }
}

/** Overwrite the context menu (add token editor action is mouse under a PadTools::PadItem */
void TokenOutputDocument::contextMenu(const QPoint &pos)
{
    QTextCursor c = textEdit()->cursorForPosition(pos);
    if (textEdit()->underMouse()) {
        textEdit()->setTextCursor(c);
    }

    if (padDocument()->padItemForOutputPosition(c.position())) {
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
    if (!padDocument())
        return;

    int position = textCursor().position();
    PadItem *item = padDocument()->padItemForOutputPosition(position);
    if (!item)
        return;

    // TODO: manage nested tokens
    TokenEditor editor(this);
    PadCore *core = item->getCore();
    editor.setTokenUid(core->uid());
    PadFragment *bef = item->subItem(PadConditionnalSubItem::Defined, PadConditionnalSubItem::Prepend);
    PadFragment *aft = item->subItem(PadConditionnalSubItem::Defined, PadConditionnalSubItem::Append);
    editor.setConditionnalHtml(padDocument()->fragmentHtmlOutput(bef), padDocument()->fragmentHtmlOutput(aft));
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

        // Modify output position for all subitemfragments
        int oldLength = item->outputLength();
        int oldStart = item->outputStart();

        QString html;
        editor.getOutput(html, *item, item->outputStart());
        bef = item->subItem(PadConditionnalSubItem::Defined, PadConditionnalSubItem::Prepend);
        aft = item->subItem(PadConditionnalSubItem::Defined, PadConditionnalSubItem::Append);

        // insert token length to the PadDocument
        int deltaLength = (item->outputLength() - oldLength);
        padDocument()->outputPosChanged(oldStart, oldStart + deltaLength);

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
    }
}

void TokenOutputDocument::dragEnterEvent(QDragEnterEvent *event)
{
    if (!padDocument())
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
    if (!padDocument())
        return;

    if (textEdit()->underMouse() &&
            event->mimeData()->hasFormat(Constants::TOKENRAWSOURCE_MIME)) {
        textEdit()->setFocus();
        QTextCursor cursor = cursorForPosition(event->pos());
        setTextCursor(cursor);
        ensureCursorVisible();
        // if event pos y <= 10 scroll up
        event->acceptProposedAction();
        event->accept();
    } else {
        event->ignore();
    }
}

void TokenOutputDocument::dragLeaveEvent(QDragLeaveEvent *event)
{
    if (!padDocument())
        return;

    if (textEdit()->underMouse()) {
        event->ignore();
    } else {
        event->accept();
    }
}

void TokenOutputDocument::dropEvent(QDropEvent *event)
{
    if (!padDocument())
        return;

    if (textEdit()->underMouse()) {
        QTextCursor cursor = textEdit()->textCursor();
        int pos = cursor.position();

        // drop inside a PadCore ?
        if (isPadCore(pos))
            pos = d->correctDropPosition(pos);

        // start token editor dialog
        TokenEditor editor(this);
        editor.setTokenUid(event->mimeData()->data(Constants::TOKENUID_MIME));
        int r = editor.exec();
        if (r == QDialog::Accepted) {
            // get the content of the editor
            setFocus();
            QString html;
            PadItem *item = new PadItem;
            editor.getOutput(html, *item, pos);

            // insert token length to the PadDocument
            padDocument()->outputPosChanged(item->outputStart(), item->outputStart() + item->outputLength());

            // retrieve the item parent
            PadFragment *parent = padDocument()->padFragmentForOutputPosition(pos);
            if (parent) {
                parent->addChild(item);
            } else {
                padDocument()->addChild(item);
            }

            // insert item text in the output document
            textEdit()->document()->blockSignals(true);
            cursor.setPosition(pos);
            cursor.insertHtml(html);
            // create the extraselection for the new item
            padDocument()->sortChildren();
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

/** Manage text editor key pressed */
bool TokenOutputDocument::eventFilter(QObject *o, QEvent *e)
{
    if (!padDocument())
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
        if (e->type() == QEvent::InputMethod) {
            // Avoid edition when cursor is inside a padCore
            QInputMethodEvent *input = static_cast<QInputMethodEvent*>(e);
            if (!input)
                return false;
            QTextCursor cursor = textEdit()->textCursor();
            if (cursor.hasSelection()) {
                if (isPadCore(cursor.selectionStart())) {
                    // reject event
                    e->ignore();
                    return true;
                }
            } else if (isPadCore(cursor.position())) {
                // reject event
                e->ignore();
                return true;
            }
        } else if (e->type() == QEvent::KeyPress) {
            QTextCursor cursor = textEdit()->textCursor();
            QKeyEvent *kevent = static_cast<QKeyEvent*>(e);
            if (!kevent)
                return false;
            // cursor inside a token ?
            if (isPadCore(cursor.position())) {
                if (!d->isNavigationKey(kevent)) {
                    e->ignore();
                    return true;
                }
            }
            switch (kevent->key()) {
            case Qt::Key_Left:  // Manage tokens
            {
                int pos = cursor.position() - 1;
                if (isPadCore(pos)) {
                    // move cursor before PadCore start
                    PadCore *core = dynamic_cast<PadCore*>(padDocument()->padFragmentForOutputPosition(pos));
                    Q_ASSERT(core);
                    cursor.setPosition(core->outputStart());
                    textEdit()->setTextCursor(cursor);
                    // reject
                    e->ignore();
                    return true;
                }
                break;
            }
            case Qt::Key_Right:  // Manage tokens
            {
                int pos = cursor.position() + 1;
                if (isPadCore(pos)) {
                    // move cursor before token start
                    PadCore *core = dynamic_cast<PadCore*>(padDocument()->padFragmentForOutputPosition(pos));
                    Q_ASSERT(core);
                    cursor.setPosition(core->outputEnd());
                    textEdit()->setTextCursor(cursor);
                    // reject
                    e->ignore();
                    return true;
                }
                break;
            }
            case Qt::Key_Backspace:
            {
                int newPosition = cursor.position() - 1;
                if (isPadCore(newPosition)) {
                    if (d->userWantsToDeletePadItem(newPosition)) {
                        PadItem *item = padDocument()->padItemForOutputPosition(newPosition);
                        Q_ASSERT(item);
                        if (!item)
                            return true;
                        newPosition = item->outputStart();
                        padDocument()->removeAndDeleteFragment(item);
                        cursor.setPosition(newPosition);
                        textEdit()->setTextCursor(cursor);
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
                int newPosition = cursor.position() + 1;
                if (isPadCore(newPosition)) {
                    if (d->userWantsToDeletePadItem(newPosition)) {
                        PadItem *item = padDocument()->padItemForOutputPosition(newPosition);
                        Q_ASSERT(item);
                        if (!item)
                            return true;
                        padDocument()->removeAndDeleteFragment(item);
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
    return TokenHighlighterEditor::eventFilter(o, e);
}
