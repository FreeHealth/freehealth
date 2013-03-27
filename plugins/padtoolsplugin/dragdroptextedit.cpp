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
 *  Main Developers:  Eric Maeker <eric.maeker@gmail.com>                  *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#include "dragdroptextedit.h"
#include "constants.h"
#include "tokeneditor.h"

#include <QMimeData>
#include <QDebug>

using namespace PadTools;
using namespace Internal;

DragDropTextEdit::DragDropTextEdit(QWidget *parent) :
    Editor::TextEditor(parent, DragDropTextEdit::Full)
{
    setAcceptDrops(true);
}

DragDropTextEdit::~DragDropTextEdit()
{
}

void DragDropTextEdit::dragEnterEvent(QDragEnterEvent *event)
{
    if (textEdit()->underMouse() &&
            event->mimeData()->hasFormat(Constants::TOKENRAWSOURCE_MIME)) {
        event->acceptProposedAction();
        event->accept();
    } else {
        event->ignore();
    }
}

void DragDropTextEdit::dragMoveEvent(QDragMoveEvent *event)
{
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

void DragDropTextEdit::dragLeaveEvent(QDragLeaveEvent *event)
{
    if (textEdit()->underMouse()) {
        event->ignore();
    } else {
        event->accept();
    }
}

void DragDropTextEdit::dropEvent(QDropEvent *event)
{
    if (textEdit()->underMouse()) {
        TokenEditor editor(this);
        editor.setTokenUid(event->mimeData()->data(Constants::TOKENUID_MIME));
        if (editor.exec() == QDialog::Accepted) {
            setFocus();
            QTextCursor cursor = cursorForPosition(event->pos());
            cursor.insertHtml(editor.toRawSourceHtml());
            event->acceptProposedAction();
            event->accept();
            return;
        }
    }
    event->ignore();
}
