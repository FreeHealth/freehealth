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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef EDITORACTIONHANDLER_H
#define EDITORACTIONHANDLER_H

#include <texteditorplugin/texteditor_exporter.h>
#include <coreplugin/contextmanager/icontext.h>

#include <QObject>
#include <QAction>
#include <QActionGroup>
#include <QPointer>
#include <QTextCharFormat>

/**
 * \file editoractionhandler.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 22 Sept 2012
*/

namespace Core {
class ActionContainer;
class Context;
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
    void typeWriterFont();
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

    void addDate();
    void addUserName();
    void addPatientName();

private:
    bool generalMenu;          /**< set to true if the application uses the Core::MainWindowActionHandler::createGeneralMenu() */
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
    QAction *aTypeWriterFont;
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

    QAction *aAddDate;
    QAction *aAddUserName;
    QAction *aAddPatientName;

    Core::ActionContainer *m_ContextualMenu;
    Core::ActionContainer *m_FileMenu;
    Core::ActionContainer *m_EditMenu;
    Core::ActionContainer *m_FormatMenu;
    Core::ActionContainer *m_FontMenu;
    Core::ActionContainer *m_ParagraphMenu;
    Core::ActionContainer *m_TableMenu;
    Core::ActionContainer *m_AddTextMenu;

    Core::Context charContext, paragraphContext, clipboardContext, basicContext, ioContext, tableContext, textAdderContext, allContexts;

private:
    void createContexts();
    void createMenus();
    void createActions();
    void connectActions();

protected:
    uint m_optionalActions;
    QPointer<TextEditor> m_CurrentEditor;
    bool m_initialized;
};

}  // End Internal
}  // End Editor

#endif // EDITORACTIONHANDLER_H
