/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
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
#ifndef TKRICHTEXTACTIONHANDLER_H
#define TKRICHTEXTACTIONHANDLER_H

#include <tkExporter.h>
class tkRichTextEditor;

#include <QObject>
#include <QAction>
#include <QActionGroup>
#include <QPointer>
#include <QTextCharFormat>


/**
 * \file tkRichTextActionHandler.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.5
 * \date 19 July  2009
*/

// Not exported because part of the internal part of tkRichTextEditor
class tkRichTextActionHandler : public QObject
{
    Q_OBJECT
public:
    explicit tkRichTextActionHandler(QObject *parent = 0);
    virtual ~tkRichTextActionHandler();

    void setCurrentEditor(tkRichTextEditor *editor);

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
    void fileOpen();
    void saveAs();

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
    QAction *aAddRow;
    QAction *aAddCol;
    QAction *aRemoveRow;
    QAction *aRemoveCol;
    QAction *aMergeCells;
    QAction *aSplitCells;

protected:
    uint m_optionalActions;
    QPointer<tkRichTextEditor> m_CurrentEditor;
    QList<int> m_contextId;
    bool m_initialized;
};

#endif // TKRICHTEXTACTIONHANDLER_H
