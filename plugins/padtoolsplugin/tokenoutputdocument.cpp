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
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>

#include <QLinearGradient>
#include <QGradientStops>
#include <QMenu>
#include <QKeyEvent>
#include <QApplication>
#include <QToolTip>

#include <QDebug>

using namespace PadTools;
using namespace Trans::ConstantTranslations;

static inline Core::ITokenPool *tokenPool() {return Core::ICore::instance()->padTools()->tokenPool();}

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
        _lastHoveredItem(0),
        _lastUnderCursorItem(0),
        _changeCursor(true)
    {
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

    bool isPadItemAt(const int pos)
    {
        PadItem *item = dynamic_cast<PadItem*>(_pad->padItemForOutputPosition(pos));
        return (item!=0);
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
    PadItem *_lastHoveredItem, *_lastUnderCursorItem; // must not be deleted
    QTextCharFormat _coreFormat;
    QTextCharFormat _tokenFormat;
    QMultiMap<PadItem *, QTextEdit::ExtraSelection> _tokenExtraSelection;
    bool _changeCursor;
};
}
}

TokenOutputDocument::TokenOutputDocument(QWidget *parent) :
    Editor::TextEditor(parent, TokenOutputDocument::Simple | TokenOutputDocument::WithTables),
    d(new Internal::TokenOutputDocumentPrivate)
{
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

void TokenOutputDocument::setChangeCursorOnPadItemHover(bool changeCursor)
{
    d->_changeCursor = changeCursor;
}

/** Manage PadTools::PadDocument clear signal */
void TokenOutputDocument::onPadCleared()
{
    d->_tokenExtraSelection.clear();
    d->_lastHoveredItem = 0;
    d->_lastUnderCursorItem = 0;
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
    // highlight the item under cursor
    d->_lastUnderCursorItem = 0;
    cursorPositionChanged();
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

            // Modify output position for all subitemfragments
            int oldLength = item->outputLength();
            int oldStart = item->outputStart();

            QString html;
            editor.getOutput(html, *item, item->outputStart());
            bef = item->subItem(PadConditionnalSubItem::Defined, PadConditionnalSubItem::Prepend);
            aft = item->subItem(PadConditionnalSubItem::Defined, PadConditionnalSubItem::Append);

            // insert token length to the PadDocument
            int deltaLength = (item->outputLength() - oldLength);
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
        }
    }
}

void TokenOutputDocument::cursorPositionChanged()
{
//    qWarning() << "cursor moves" << textCursor().position() << "isPadItem" << d->isPadItemAt(textCursor().position());
    if (d->isPadItemAt(textCursor().position())) {
        PadItem *item = d->_pad->padItemForOutputPosition(textCursor().position());
        if (!d->_lastUnderCursorItem) {
            d->_lastUnderCursorItem = item;
            if (d->_lastUnderCursorItem) {
                textEdit()->setExtraSelections(d->_tokenExtraSelection.values(d->_lastUnderCursorItem));
            } else {
                textEdit()->setExtraSelections(QList<QTextEdit::ExtraSelection>());
            }
        } else {
            if (d->_lastUnderCursorItem && d->_lastUnderCursorItem != item) {
                d->_lastUnderCursorItem = item;
                textEdit()->setExtraSelections(d->_tokenExtraSelection.values(d->_lastUnderCursorItem));
            }
        }
    } else {
        d->_lastUnderCursorItem = 0;
        textEdit()->setExtraSelections(QList<QTextEdit::ExtraSelection>());
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

static QString fullNamespace(Core::TokenNamespace &ns)
{
    QStringList r;
    Core::TokenNamespace *current = const_cast<Core::TokenNamespace *>(&ns);
    while (current->children().count() > 0) {
        r << current->humanReadableName();
        current = const_cast<Core::TokenNamespace *>(&current->children().at(0));
    }
    if (current)
        r << current->humanReadableName();
    return r.join(".");
}

bool TokenOutputDocument::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTip) {
        QHelpEvent *helpEvent = static_cast<QHelpEvent*>(event);
        int position = cursorForPosition(helpEvent->pos()).position();
        PadItem *item = d->_pad->padItemForOutputPosition(position);
        if (item) {
            Core::IToken *token = tokenPool()->token(item->getCore()->name());
            if (token) {
                // Construct name
                Core::TokenNamespace ns = tokenPool()->getTokenNamespace(token->uid());
                QString name;
                token->humanReadableName().isEmpty() ? name = token->uid() : name = token->humanReadableName();
                QString tooltip;
                tooltip = QString("<b>%1<br /><b>%2<br /><b>%3: %4")
                        .arg(tkTr(Trans::Constants::TOKEN_NAMESPACE_1).arg(fullNamespace(ns).replace(" ", "&nbsp;")))
                        .arg(tkTr(Trans::Constants::TOKEN_1).arg(name))
                        .arg(tr("Value"))
                        .arg(token->value().toString())
                        .replace(":", ":</b>");
                if (!token->tooltip().isEmpty())
                    tooltip += "<br />" + token->tooltip().replace("\n", "<br />");

                QToolTip::showText(helpEvent->globalPos(), tooltip);
                return Editor::TextEditor::event(event);
            }
        }
        QToolTip::hideText();
        event->ignore();
    }
    return Editor::TextEditor::event(event);
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
            // TODO: improve PadCore highlighting
            QHoverEvent *me = static_cast<QHoverEvent*>(e);
            // compute pos
            int position = cursorForPosition(me->pos()).position();
            // do we change of hovered paditem ?
            if (d->_lastHoveredItem) {
                if (d->_lastHoveredItem->containsOutputPosition(position)) {
                    // hover does not change of item
                    return true;
                }
            }

            PadItem *item = d->_pad->padItemForOutputPosition(position);
            if (!item) {
                // we are outside any paditem
                if (d->_lastHoveredItem) {
                    // but before we was hovering a paditem
                    textEdit()->setExtraSelections(QList<QTextEdit::ExtraSelection>());
                    d->_lastHoveredItem = 0;
                    if (d->_changeCursor)
                        qApp->restoreOverrideCursor();
                }
                return QWidget::eventFilter(o, e);
            }

            // we are hovering a paditem
            if (d->_lastHoveredItem) {
                // same as last one ? -> exit
                if (d->_lastHoveredItem == item)
                    return true;
                // clear old selections
                textEdit()->setExtraSelections(QList<QTextEdit::ExtraSelection>());
                d->_lastHoveredItem = item;
            } else {
                d->_lastHoveredItem = item;
            }
            if (d->_changeCursor) {
                if ((qApp->overrideCursor() && qApp->overrideCursor()->shape() != Qt::PointingHandCursor) ||
                        !qApp->overrideCursor())
                    qApp->setOverrideCursor(QCursor(Qt::PointingHandCursor));
            }
            textEdit()->setExtraSelections(d->_tokenExtraSelection.values(item));
            me->accept();
            return true;
        } else if (e->type()==QEvent::HoverLeave && d->_lastHoveredItem) {
            textEdit()->setExtraSelections(QList<QTextEdit::ExtraSelection>());
            d->_lastHoveredItem = 0;
            if (d->_changeCursor)
                qApp->restoreOverrideCursor();
            e->accept();
            return true;
        } else if (e->type() == QEvent::InputMethod) {
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
        } else if (e->type() == QEvent::KeyPress) {
            QTextCursor cursor = textEdit()->textCursor();
            QKeyEvent *kevent = static_cast<QKeyEvent*>(e);
            if (!kevent)
                return false;
            // cursor inside a token ?
            if (d->isPadCoreAt(cursor.position())) {
                if (!d->isNavigationKey(kevent)) {
                    e->ignore();
                    return true;
                }
            }
            switch (kevent->key()) {
            case Qt::Key_Left:  // Manage tokens
            {
                int pos = cursor.position() - 1;
                if (d->isPadCoreAt(pos)) {
                    // move cursor before PadCore start
                    PadCore *core = dynamic_cast<PadCore*>(d->_pad->padFragmentForOutputPosition(pos));
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
                if (d->isPadCoreAt(pos)) {
                    // move cursor before token start
                    PadCore *core = dynamic_cast<PadCore*>(d->_pad->padFragmentForOutputPosition(pos));
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
                if (d->isPadCoreAt(newPosition)) {
                    if (d->userWantsToDeletePadItem(newPosition)) {
                        PadItem *item = d->_pad->padItemForOutputPosition(newPosition);
                        Q_ASSERT(item);
                        if (!item)
                            return true;
                        newPosition = item->outputStart();
                        d->_pad->removeAndDeleteFragment(item);
                        d->_lastHoveredItem = 0;
                        d->_lastUnderCursorItem = 0;
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
                if (d->isPadCoreAt(newPosition)) {
                    if (d->userWantsToDeletePadItem(newPosition)) {
                        PadItem *item = d->_pad->padItemForOutputPosition(newPosition);
                        Q_ASSERT(item);
                        if (!item)
                            return true;
                        d->_pad->removeAndDeleteFragment(item);
                        d->_lastHoveredItem = 0;
                        d->_lastUnderCursorItem = 0;
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
    connect(textEdit(), SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));
    connect(textEdit()->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(contentChanged(int,int,int)));
}

void TokenOutputDocument::disconnectOutputDocumentChanges()
{
    disconnect(textEdit(), SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));
    disconnect(textEdit()->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(contentChanged(int,int,int)));
}

/** Keep PadDocument sync to the output QTextDocument */
void TokenOutputDocument::contentChanged(const int pos, const int rm, const int ins)
{
//    qWarning() << "contentChanged pos" << pos << "ins" << ins << "rm" << rm;

    // TODO: improve management of textformatting modifications -> do not re-compute positions */
    if (rm==ins)
        return;

    if (rm)
        d->_pad->outputPosChanged(pos+rm, pos);

    if (ins)
        d->_pad->outputPosChanged(pos, pos+ins);

    onDocumentAnalyzeReset();
}
