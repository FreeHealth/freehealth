/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   This code is inspired of the Qt example : Text Edit                   *
 *   Adaptations to FreeMedForms and improvments by : Eric Maeker, MD      *
 *   eric.maeker@free.fr                                                   *
 ***************************************************************************/

/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef EDITORACTIONHANDLER_H
#define EDITORACTIONHANDLER_H

#include <texteditorplugin/texteditor_exporter.h>

#include <QObject>
#include <QAction>
#include <QActionGroup>
#include <QPointer>
#include <QTextCharFormat>
#include <QList>


/**
 * \file editoractionhandler.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.4.0
 * \date 29 Apr 2010
*/


namespace Core {
class ActionContainer;
}

namespace Editor {
class TextEditor;

namespace Internal {

class EditorActionHandler : public QObject
{
    Q_OBJECT
public:
    explicit EditorActionHandler(QObject *parent = 0);
    virtual ~EditorActionHandler();

    void setCurrentEditor(TextEditor *editor);

public Q_SLOTS:
    void updateActions();
    void updateRedoAction();
    void updateUndoAction();
    void updateCopyAction();
    void updateTableActions();
    void updateColorActions();

private Q_SLOTS:
    void currentCharFormatChanged(const QTextCharFormat &format);
    void cursorPositionChanged();
    void fontChanged( const QFont &f );
    void colorChanged( const QColor &c );
    void alignmentChanged( Qt::Alignment a );
    void clipboardDataChanged();
    void toogleToolbar();

    // wrap actions slots to current editor slots
    void print();
    void fileOpen();
    void saveAs();

    void copy();
    void cut();
    void paste();
    void undo();
    void redo();

    void fontBigger();
    void fontSmaller();
    void textBold();
    void textUnderline();
    void textItalic();
    void textStrike();
    void textColor();

    void fontFormat();

    void textAlign();

    void addTable();
    void tableProperties();
    void tableAddRow();
    void tableAddCol();
    void tableRemoveCol();
    void tableRemoveRow();
    void tableMergeCells();
    void tableSplitCells();

private:
    QAction *aToggleToolBar;

    QAction *aOpen;
    QAction *aSave;
    QAction *aPrint;

    QAction *aUndo;
    QAction *aRedo;
    QAction *aCut;
    QAction *aCopy;
    QAction *aPaste;

    QAction *aBold;
    QAction *aUnderline;
    QAction *aStrike;
    QAction *aItalic;
    QAction *aColor;
    QAction *aFontFormat;
    QAction *aFontBigger;
    QAction *aFontSmaller;
    QAction *aLeft;
    QAction *aCenter;
    QAction *aRight;
    QAction *aJustify;

    QAction *aAddTable;
    QAction *aTableProperties;
    QAction *aAddRow;
    QAction *aAddCol;
    QAction *aRemoveRow;
    QAction *aRemoveCol;
    QAction *aMergeCells;
    QAction *aSplitCells;

    Core::ActionContainer *m_ContextualMenu;
    Core::ActionContainer *m_FileMenu;
    Core::ActionContainer *m_EditMenu;
    Core::ActionContainer *m_FormatMenu;
    Core::ActionContainer *m_FontMenu;
    Core::ActionContainer *m_ParagraphMenu;
    Core::ActionContainer *m_TableMenu;

    QList<int> basicContext, ioContext, tableContext, allContexts;

private:
    void createContexts();
    void createMenus();
    void createActions();
    void connectActions();

protected:
    uint m_optionalActions;
    QPointer<TextEditor> m_CurrentEditor;
    QList<int> m_contextId;
    bool m_initialized;
};

}  // End Internal
}  // End Editor

#endif // EDITORACTIONHANDLER_H
