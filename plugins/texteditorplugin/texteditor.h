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
#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <texteditorplugin/texteditor_exporter.h>
#include <texteditorplugin/tableeditor.h>

#include <QObject>
#include <QWidget>
#include <QTextEdit>
#include <QFocusEvent>
class QMenu;

/**
 * \file texteditor.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.9
 * \date 08 Sept 2009
*/

namespace Editor {

namespace Internal {
class TextEditorPrivate;
class EditorActionHandler;
}

class EDITOR_EXPORT TextEditor : public TableEditor
{
    friend class Internal::TextEditorPrivate;
    friend class Internal::EditorActionHandler;
    Q_OBJECT
    Q_PROPERTY( QString html READ getHtml WRITE setHtml USER true)

public:
    enum Type
    {
        Simple     = 0x01,
        WithTables = 0x02,
        WithIO     = 0x04,
        Full       = Simple | WithTables | WithIO
    };
    Q_DECLARE_FLAGS( Types, Type );

    TextEditor(QWidget *parent = 0, Types type = Simple);
    ~TextEditor();

    virtual QTextEdit *textEdit() const;

    QString getHtml()                        { return textEdit()->toHtml(); }
    void    setHtml(const QString & html)    { textEdit()->setHtml( html ); }
    void    setTypes(Types type);

public Q_SLOTS:
    virtual void toogleToolbar(bool state);

protected Q_SLOTS:
    void fileOpen();
    void saveAs();

//    void undo();
//    void redo();
//    void copy();
//    void paste();
//    void cut();
//    void selectall();
//    void clipboardDataChanged();

    void fontBigger();
    void fontSmaller();
    void textBold( bool checked );
    void textUnderline( bool checked );
    void textItalic( bool checked );
    void textStrike( bool checked );
    void textColor();
    void fontFormat();

//    void about();

    virtual void contextMenu(const QPoint &pos);

protected:
    virtual void changeEvent(QEvent *e);
    virtual QMenu* getContextMenu();
    virtual bool toolbarIsVisible() const;
    virtual void hideToolbar();

private:
    Internal::TextEditorPrivate *d;

};

}  // End Editor

Q_DECLARE_OPERATORS_FOR_FLAGS(Editor::TextEditor::Types)

#endif // TEXTEDITOR_H
