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
 *   This code is inspired of the Qt example : Text Edit                   *
 *   Adaptations to FreeMedForms and improvments by : Eric Maeker, MD      *
 *   eric.maeker@gmail.com                                                   *
 ***************************************************************************/

/***************************************************************************
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
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
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.7.1
 * \date 13 Apr 2012
*/

namespace Editor {

namespace Internal {
class TextEditorPrivate;
class EditorActionHandler;
}

class EDITOR_EXPORT ITextControl : public QObject
{
    Q_OBJECT
public:
    ITextControl(QObject *parent = 0) : QObject(parent) {}
    virtual ~ITextControl() {}

    virtual bool canInsertFromMimeData (const QMimeData *source) const = 0;
    virtual QMimeData *createMimeDataFromSelection() const = 0;
    virtual void insertFromMimeData(const QMimeData *source) = 0;
};

class EDITOR_EXPORT TextEditor : public TableEditor
{
    friend class Internal::TextEditorPrivate;
    friend class Internal::EditorActionHandler;
    Q_OBJECT
    Q_PROPERTY(QString html READ getHtml WRITE setHtml USER true)

public:
    enum Type {
        CharFormat        = 0x0001,
        ParagraphFormat   = 0x0002,
        Clipboard         = 0x0004,
        WithTables        = 0x0020,
        WithIO            = 0x0040,
        WithTextCompleter = 0x0080,
        Simple            = CharFormat | ParagraphFormat | Clipboard,
        Full              = Simple | WithTables | WithIO | WithTextCompleter
    };
    Q_DECLARE_FLAGS(Types, Type)

    TextEditor(QWidget *parent = 0, Types type = Simple);
    ~TextEditor();
    void setTypes(Types type);

    virtual QTextEdit *textEdit() const;

    // some QTextEdit wrappers for code simplification
    QTextDocument *document() const {return textEdit()->document();}
    void setTextCursor(const QTextCursor &cursor) {textEdit()->setTextCursor(cursor);}
    QTextCursor textCursor() const {return textEdit()->textCursor();}
    void ensureCursorVisible() {textEdit()->ensureCursorVisible();}
    QTextCursor cursorForPosition(const QPoint &pos) const {return textEdit()->cursorForPosition(pos);}

    QString getHtml()                        { return textEdit()->toHtml(); }
    void    setHtml(const QString & html)    { textEdit()->setHtml( html ); }

    QString toHtml() const {return textEdit()->toHtml();}
    void setPlainText(const QString &s) {textEdit()->setPlainText(s);}
    QString toPlainText() const {return textEdit()->toPlainText();}

    void setTextControl(ITextControl *control);

public Q_SLOTS:
    void setReadOnly(bool ro) {textEdit()->setReadOnly(ro);}
    void clear() {textEdit()->clear();}
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

    void addDate();
    void addUserName();
    void addPatientName();

    virtual void contextMenu(const QPoint &pos);

protected:
    virtual QMenu *getContextMenu();
    virtual bool toolbarIsVisible() const;
    virtual void hideToolbar();

private:
    Internal::TextEditorPrivate *d;

};

}  // End Editor

Q_DECLARE_OPERATORS_FOR_FLAGS(Editor::TextEditor::Types)

#endif // TEXTEDITOR_H
