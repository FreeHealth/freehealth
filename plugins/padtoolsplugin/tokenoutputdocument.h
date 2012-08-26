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
#ifndef PADTOOLS_TOKENOUTPUTDOCUMENT_H
#define PADTOOLS_TOKENOUTPUTDOCUMENT_H

#include <texteditorplugin/texteditor.h>

/**
 * \file tokenoutputdocument.h
 * \author Eric Maeker
 * \version 0.8.0
 * \date 25 Apr 2012
*/

namespace PadTools {
class PadDocument;

namespace Internal {
class TokenOutputDocumentPrivate;

//class TokenOutputDocumentControl : public Editor::ITextControl
//{
//    Q_OBJECT
//public:
//    TokenOutputDocumentControl(QObject *parent);
//    ~TokenOutputDocumentControl();

//    bool canInsertFromMimeData (const QMimeData *source) const;
//    QMimeData *createMimeDataFromSelection() const;
//    void insertFromMimeData(const QMimeData *source);
//};

}

class TokenOutputDocument : public Editor::TextEditor
{
    Q_OBJECT
public:
    TokenOutputDocument(QWidget *parent = 0);
    ~TokenOutputDocument();

    void setPadDocument(PadDocument *pad);
    void setChangeCursorOnPadItemHover(bool changeCursor);

private Q_SLOTS:
    void onPadCleared();
    void onDocumentAnalyzeReset();
    void contextMenu(const QPoint &pos);
    void editTokenUnderCursor();
    void cursorPositionChanged();
    void connectPadDocument();
    void disconnectPadDocument();
    void connectOutputDocumentChanges();
    void disconnectOutputDocumentChanges();
    void contentChanged(const int pos, const int rm, const int ins);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
    bool event(QEvent *event);
    bool eventFilter(QObject *o, QEvent *e);

private:
    Internal::TokenOutputDocumentPrivate *d;
};

}  // PadTools

#endif // PADTOOLS_TOKENOUTPUTDOCUMENT_H

