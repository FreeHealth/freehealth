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
#ifndef TKRICHTEXTEDITOR_P_H
#define TKRICHTEXTEDITOR_P_H

// include toolkit headers
#include "tkRichTextEditorManager.h"
#include <tkUniqueIdentifier.h>
#include <tkConstants.h>

#include <tkRichTextEditor.h>
#include <tkRichTextEditorContext.h>
#include <tkContextManager.h>
class tkTextEditorPrivate;


// include Qt headers
#include <QObject>
#include <QDialog>
#include <QTextTableFormat>
#include <QTextLength>
#include <QVector>
#include <QApplication>
#include <QTextEdit>
#include <QPointer>
QT_BEGIN_NAMESPACE
class QTextCharFormat;
class QFont;
class QColor;
class QAction;
class QToolBar;
class QMenu;
class QComboBox;
class QFontComboBox;
QT_END_NAMESPACE


class tkRichTextEditorContext;

class tkRichTextEditorPrivate : public QWidget
{
    Q_OBJECT
public:
    tkRichTextEditorPrivate( QWidget *parent, tkRichTextEditor::Types type );
    ~tkRichTextEditorPrivate();

//    void setupActions();
//    void setupOptionnalActions();
//    void setupMenus();
//    void setupOptionnalMenus();
//    void setupToolBar();
//    void createConnections();
//    void disconnectAll();
    void retranslateUi();

public Q_SLOTS:
//    void editorFocusChangedTo(bool focused);
    void contextMenu( const QPoint & );

//    void fileOpen();
//    void saveAs();
//
//    void undo();
//    void redo();
//    void copy();
//    void paste();
//    void cut();
//    void selectall();
//    void clipboardDataChanged();


//    void fontBigger();
//    void fontSmaller();
//    void textBold( bool checked );
//    void textUnderline( bool checked );
//    void textItalic( bool checked );
//    void textStrike( bool checked );
//    void fontFormat();
    void textFamily( const QString &f );
    void textSize( const QString &p );
    void textStyle( int styleIndex );
//    void textColor();
//    void textAlign();//( QAction *a );

//    void currentCharFormatChanged( const QTextCharFormat &format );
//    void cursorPositionChanged();

    void about();

//    void addTable();
//    void tableAddRow();
//    void tableAddCol();
//    void tableRemoveCol();
//    void tableRemoveRow();
//    void tableMergeCells();
//    void tableSplitCells();

public:
    void mergeFormatOnWordOrSelection( const QTextCharFormat &format );
//    void fontChanged( const QFont &f );
//    void colorChanged( const QColor &c );
//    void alignmentChanged( Qt::Alignment a );

public:
    tkRichTextEditor::Types m_Type;
    tkRichTextEditorContext *m_Context;

//    QComboBox *comboStyle;
//    QFontComboBox *comboFont;
//    QComboBox *comboSize;
//
//    QToolBar *m_ToolBar;
//    QMenu *m_ContextMenu, *m_EditMenu, *m_FormatMenu, *m_ExtraMenu, *m_TableMenu;

    QTextEdit *textEdit;

    QWidget *m_Parent;
    bool m_ActionsConnected;
};


#endif // TKRICHTEXTEDITOR_P_H
