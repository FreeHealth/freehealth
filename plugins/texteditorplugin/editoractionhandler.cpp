/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "editoractionhandler.h"
#include "texteditor.h"

#include <translationutils/constants.h>
#include <translationutils/trans_menu.h>
#include <translationutils/trans_editor.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/iuser.h>

#include <utils/log.h>

#include <QAction>
#include <QApplication>
#include <QClipboard>

#include <QDebug>

using namespace Editor;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ActionManager *actionManager() { return Core::ICore::instance()->actionManager(); }
static inline Core::ITheme *theme() { return Core::ICore::instance()->theme(); }

static QAction *registerAction(const QString &id, const Core::Context &ctx, QObject *parent)
{
    QAction *a = new QAction(parent);
    Core::Command *cmd = Core::ICore::instance()->actionManager()->registerAction(a, Core::Id(id), ctx);
    Q_UNUSED(cmd);
    return a;
}

static inline QAction *createAction(QObject *parent, const QString &name, const QString &icon,
                                    const QString &actionName,
                                    const Core::Context &context, const QString &trans,
                                    Core::Command *cmd,
                                    Core::ActionContainer *menu,
                                    const QString &group,
                                    QKeySequence::StandardKey key = QKeySequence::UnknownKey,
                                    bool checkable = false)
{
    QAction *a = new QAction(parent);
    a->setObjectName(name);
    if (!icon.isEmpty())
        a->setIcon(theme()->icon(icon));
    if (checkable) {
        a->setCheckable(true);
        a->setChecked(false);
    }
    cmd = actionManager()->registerAction(a, Core::Id(actionName), context);
    cmd->setTranslations(trans, trans); // use the Trans::Constants tr context (automatic)
    if (key != QKeySequence::UnknownKey)
        cmd->setDefaultKeySequence(key);
    menu->addAction(cmd, Core::Id(group));
    return a;
}

static inline QAction *createAction(QObject *parent, const QString &name, const QString &icon,
                                    const QString &actionName,
                                    const Core::Context &context, const QString &trans,
                                    Core::Command *cmd,
                                    Core::ActionContainer *menu,
                                    const QString &group,
                                    const QString  &key,
                                    bool checkable = false)
{
    QAction *a = new QAction(parent);
    a->setObjectName(name);
    if (!icon.isEmpty())
        a->setIcon(theme()->icon(icon));
    if (checkable) {
        a->setCheckable(true);
        a->setChecked(false);
    }
    cmd = actionManager()->registerAction(a, Core::Id(actionName), context);
    cmd->setTranslations(trans, trans); // use the Trans::Constants tr context (automatic)
    if (!key.isEmpty())
        cmd->setDefaultKeySequence(QKeySequence::fromString(tkTr(key.toUtf8())));
    menu->addAction(cmd, Core::Id(group));
    return a;
}

EditorActionHandler::EditorActionHandler(QObject *parent) :
    QObject(parent),
    aOpen(0), aSave(0), aPrint(0),
    aUndo(0), aRedo(0),
    aCut(0), aCopy(0), aPaste(0),
    aBold(0), aUnderline(0), aStrike(0),
    aItalic(0), aColor(0),
    aFontFormat(0), aFontBigger(0), aFontSmaller(0), aTypeWriterFont(0),
    aLeft(0), aCenter(0), aRight(0), aJustify(0),
    aAddTable(0), aTableProperties(0),
    aAddRow(0),aAddCol(0),
    aRemoveRow(0), aRemoveCol(0),
    aMergeCells(0), aSplitCells(0),
    aAddDateLong(0), aAddDateShort(0),
    aAddUserName(0),
    aAddPatientName(0),
    m_CurrentEditor(0)
{
    createContexts();
    createMenus();
    createActions();
    connectActions();
}

EditorActionHandler::~EditorActionHandler()
{
}

void EditorActionHandler::createContexts()
{
    charContext = Core::Context(Core::Constants::C_EDITOR_CHAR_FORMAT);
    paragraphContext = Core::Context(Core::Constants::C_EDITOR_PARAGRAPH);
    clipboardContext = Core::Context(Core::Constants::C_EDITOR_CLIPBOARD);
    ioContext = Core::Context(Core::Constants::C_EDITOR_IO);
    printContext = Core::Context(Core::Constants::C_EDITOR_IO);
    tableContext = Core::Context(Core::Constants::C_EDITOR_TABLE);
    textAdderContext = Core::Context(Core::Constants::C_EDITOR_ADDTEXT);
    allContexts.add(charContext);
    allContexts.add(paragraphContext);
    allContexts.add(clipboardContext);
    allContexts.add(ioContext);
    allContexts.add(printContext);
    allContexts.add(tableContext);
    allContexts.add(textAdderContext);
}

void EditorActionHandler::createMenus()
{
    // Editor's Contextual Menu
    m_ContextualMenu = actionManager()->createMenu(Core::Constants::M_EDITOR_CONTEXT);
    m_ContextualMenu->appendGroup(Core::Id(Core::Constants::G_EDITOR_CONTEXT));
    m_ContextualMenu->setTranslations(Trans::Constants::EDITORMENU_TEXT);

    // Menu structure -- rootMenu is menubar if exists otherwise it is a specific editor's menu Core::Constants::M_EDITOR
    Core::ActionContainer *rootMenu;
    generalMenu = false;
    rootMenu = actionManager()->actionContainer(Core::Constants::MENUBAR);
    m_EditMenu = actionManager()->actionContainer(Core::Constants::M_EDIT);
    m_FileMenu = actionManager()->actionContainer(Core::Constants::M_FILE);
    if (!m_FileMenu) {
        m_FileMenu = actionManager()->actionContainer(Core::Constants::M_GENERAL);
        generalMenu = true;
    }
    if (!m_EditMenu || !m_FileMenu || !rootMenu) {
        LOG_ERROR("No menu for texteditor widgets");
    }

    // Menu Edit --> text formats
    m_FormatMenu = actionManager()->actionContainer(Core::Constants::M_FORMAT);
    if (!m_FormatMenu) {
        rootMenu->appendGroup(Core::Constants::G_FORMAT);
        m_FormatMenu = actionManager()->createMenu(Core::Constants::M_FORMAT);
//        rootMenu->addMenu(m_FormatMenu, Core::Constants::G_FORMAT);
        m_FormatMenu->setTranslations(Trans::Constants::M_FORMAT_TEXT);
        m_FormatMenu->appendGroup(Core::Id(Core::Constants::G_FORMAT_FONT));
        m_FormatMenu->appendGroup(Core::Id(Core::Constants::G_FORMAT_PARAGRAPH));
        m_FormatMenu->appendGroup(Core::Id(Core::Constants::G_FORMAT_TABLE));
        m_FormatMenu->appendGroup(Core::Id(Core::Constants::G_FORMAT_IMAGE));
        m_FormatMenu->appendGroup(Core::Id(Core::Constants::G_FORMAT_OTHER));
    }

    m_FontMenu = actionManager()->actionContainer(Core::Constants::M_FORMAT_FONT);
    if (!m_FontMenu) {
        m_FontMenu = actionManager()->createMenu(Core::Constants::M_FORMAT_FONT);
//        m_FormatMenu->addMenu(m_FontMenu, Core::Constants::G_FORMAT_FONT);
        m_FontMenu->setTranslations(Trans::Constants::M_FORMAT_FONT_TEXT);
        m_FontMenu->appendGroup(Core::Id(Core::Constants::G_FORMAT_FONT_BASE));
        m_FontMenu->appendGroup(Core::Id(Core::Constants::G_FORMAT_FONT_SIZE));
        m_FontMenu->appendGroup(Core::Id(Core::Constants::G_FORMAT_FONT_EXTRAS));
    }

    m_ParagraphMenu = actionManager()->actionContainer(Core::Constants::M_FORMAT_PARAGRAPH);
    if (!m_ParagraphMenu) {
        m_ParagraphMenu = actionManager()->createMenu(Core::Constants::M_FORMAT_PARAGRAPH);
//        m_FormatMenu->addMenu(m_ParagraphMenu, Core::Constants::G_FORMAT_PARAGRAPH);
        m_ParagraphMenu->setTranslations(Trans::Constants::M_FORMAT_PARAGRAPH_TEXT);
        m_ParagraphMenu->appendGroup(Core::Id(Core::Constants::G_FORMAT_PARAGRAPH));
    }

    m_TableMenu = actionManager()->actionContainer(Core::Constants::M_FORMAT_TABLE);
    if (!m_TableMenu) {
        m_TableMenu = actionManager()->createMenu(Core::Constants::M_FORMAT_TABLE);
//        m_FormatMenu->addMenu(m_TableMenu, Core::Constants::G_FORMAT_TABLE);
        m_TableMenu->setTranslations(Trans::Constants::M_FORMAT_TABLE_TEXT);
        m_TableMenu->appendGroup(Core::Id(Core::Constants::G_FORMAT_TABLE));
//        m_TableMenu->setEmptyAction(ActionContainer::EA_Hide);
    }

    m_AddTextMenu = actionManager()->actionContainer(Core::Constants::M_EDITOR_ADDTEXT);
    if (!m_AddTextMenu) {
        m_AddTextMenu = actionManager()->createMenu(Core::Constants::M_EDITOR_ADDTEXT);
//        m_EditMenu->addMenu(m_AddTextMenu, Core::Constants::G_EDIT_EDITOR);
        m_AddTextMenu->setTranslations(Trans::Constants::EDITOR_ADDTEXTMENU_TEXT);
    }
}

void EditorActionHandler::createActions()
{
    using namespace Core::Constants;
    using namespace Trans::Constants;

    Core::Command *cmd = 0;

    aToggleToolBar = createAction(this, "aToggleToolBar", "", A_EDITOR_TOOGLETOOLBAR, allContexts, EDITOR_TOGGLETOOLBAR_TEXT, cmd, m_ContextualMenu, G_EDITOR_CONTEXT, QKeySequence::UnknownKey, true);

    // Print
    if (!actionManager()->command(Core::Constants::A_FILE_PRINT)) {
        aPrint = createAction(this, "aPrint", ICONPRINT, A_FILE_PRINT, printContext, FILEPRINT_TEXT, cmd, m_EditMenu, G_FILE_PRINT, QKeySequence::Print);
        aPrint->setEnabled(false);
    } else {
        aPrint = registerAction(Core::Constants::A_FILE_PRINT, printContext, this);
    }

    // Undo / Redo / Copy / Cut / Paste
    if (!actionManager()->command(Core::Constants::A_EDIT_UNDO)) {
        aUndo = createAction(this, "aUndo", ICONUNDO, A_EDIT_UNDO, allContexts, EDITUNDO_TEXT, cmd, m_EditMenu, G_EDIT_UNDOREDO, QKeySequence::Undo);
        aUndo->setEnabled(false);
    } else {
        aUndo = registerAction(Core::Constants::A_EDIT_UNDO, allContexts, this);
    }
    if (!actionManager()->command(Core::Constants::A_EDIT_REDO)) {
        aRedo = createAction(this, "aRedo", ICONREDO, A_EDIT_REDO, allContexts, EDITREDO_TEXT, cmd, m_EditMenu, G_EDIT_UNDOREDO, QKeySequence::Redo);
        aRedo->setEnabled(false);
    } else {
        aRedo = registerAction(Core::Constants::A_EDIT_REDO, allContexts, this);
    }
    if (!actionManager()->command(Core::Constants::A_EDIT_CUT)) {
        aCut = createAction(this, "aCut", ICONCUT, A_EDIT_CUT, clipboardContext, EDITCUT_TEXT, cmd, m_EditMenu, G_EDIT_COPYPASTE, QKeySequence::Cut);
        aCut->setEnabled(false);
    } else {
        aCut = registerAction(Core::Constants::A_EDIT_CUT, clipboardContext, this);
    }
    if (!actionManager()->command(Core::Constants::A_EDIT_COPY)) {
        aCopy = createAction(this, "aCopy", ICONCOPY, A_EDIT_COPY, clipboardContext, EDITCOPY_TEXT, cmd, m_EditMenu, G_EDIT_COPYPASTE, QKeySequence::Copy);
        aCopy->setEnabled(false);
    } else {
        aCopy = registerAction(Core::Constants::A_EDIT_COPY, clipboardContext, this);
    }
    if (!actionManager()->command(Core::Constants::A_EDIT_PASTE)) {
        aPaste = createAction(this, "aPaste", ICONPASTE, A_EDIT_PASTE, clipboardContext, EDITPASTE_TEXT, cmd, m_EditMenu, G_EDIT_COPYPASTE, QKeySequence::Paste);
        aPaste->setEnabled(false);
    } else {
        aPaste = registerAction(Core::Constants::A_EDIT_PASTE, clipboardContext, this);
    }

    // Font actions
    aBold = createAction(this, "aBold", ICONBOLD, A_FORMAT_BOLD, charContext, FORMATBOLD_TEXT, cmd, m_FontMenu, G_FORMAT_FONT_BASE, QKeySequence::Bold, true);
    aItalic = createAction(this, "aItalic", ICONITALIC, A_FORMAT_ITALIC, charContext, FORMATITALIC_TEXT, cmd, m_FontMenu, G_FORMAT_FONT_BASE, QKeySequence::Italic, true);
    aUnderline = createAction(this, "aUnderline", ICONUNDERLINE, A_FORMAT_UNDERLINE, charContext, FORMATUNDERLINE_TEXT, cmd, m_FontMenu, G_FORMAT_FONT_BASE, QKeySequence::Underline, true);
    aStrike = createAction(this, "aStrike", ICONSTRIKE, A_FORMAT_STRIKE, charContext, FORMATSTRIKE_TEXT, cmd, m_FontMenu, G_FORMAT_FONT_BASE, QKeySequence::UnknownKey, true);
    aFontBigger = createAction(this, "aFontBigger", ICONFONTBIGGER, A_FORMAT_BIGGER, charContext, FORMATBIGGER_TEXT, cmd, m_FontMenu, G_FORMAT_FONT_SIZE);
    aFontSmaller = createAction(this, "aFontSmaller", ICONFONTSMALLER, A_FORMAT_SMALLER, charContext, FORMATSMALLER_TEXT, cmd, m_FontMenu, G_FORMAT_FONT_SIZE);
    aTypeWriterFont = createAction(this, "aTypeWriterFont", ICONTYPEWRITER, A_FORMAT_TYPEWRITERFONT, charContext, FORMATTYPEWRITER_TEXT, cmd, m_FontMenu, G_FORMAT_FONT_SIZE);
    aFontFormat = createAction(this, "aFontFormat", ICONFONTFORMAT, A_FORMAT_FONT, charContext, FORMATFONT_TEXT, cmd, m_FontMenu, G_FORMAT_FONT_EXTRAS);
    aColor = createAction(this, "aColor", "", A_FORMAT_FONTCOLOR, charContext, FORMATFONTCOLOR_TEXT, cmd, m_FontMenu, G_FORMAT_FONT_EXTRAS);
    actionManager()->command(A_FORMAT_FONTCOLOR)->setAttribute(Core::Command::CA_UpdateIcon);
    QPixmap p(16,16);
    p.fill(Qt::black);
    aColor->setIcon(p);

    // Paragraph actions
#ifdef Q_OS_MAC
    aLeft = createAction(this, "aLeft", ICONALIGNLEFT, A_ALIGN_LEFT, paragraphContext, ALIGNLEFT_TEXT, cmd, m_ParagraphMenu, G_FORMAT_PARAGRAPH, K_ALIGNLEFT_MAC);
#else
    aLeft = createAction(this, "aLeft", ICONALIGNLEFT, A_ALIGN_LEFT, paragraphContext, ALIGNLEFT_TEXT, cmd, m_ParagraphMenu, G_FORMAT_PARAGRAPH, K_ALIGNLEFT);
#endif

#ifdef Q_OS_MAC
    aRight = createAction(this, "aRight", ICONALIGNRIGHT, A_ALIGN_RIGHT, paragraphContext, ALIGNRIGHT_TEXT, cmd, m_ParagraphMenu, G_FORMAT_PARAGRAPH, K_ALIGNRIGHT_MAC);
#else
    aRight = createAction(this, "aRight", ICONALIGNRIGHT, A_ALIGN_RIGHT, paragraphContext, ALIGNRIGHT_TEXT, cmd, m_ParagraphMenu, G_FORMAT_PARAGRAPH, K_ALIGNRIGHT);
#endif

#ifdef Q_OS_MAC
    aCenter = createAction(this, "aCenter", ICONALIGNCENTER, A_ALIGN_CENTER, paragraphContext, ALIGNCENTER_TEXT, cmd, m_ParagraphMenu, G_FORMAT_PARAGRAPH, K_ALIGNCENTER_MAC);
#else
    aCenter = createAction(this, "aCenter", ICONALIGNCENTER, A_ALIGN_CENTER, paragraphContext, ALIGNCENTER_TEXT, cmd, m_ParagraphMenu, G_FORMAT_PARAGRAPH, K_ALIGNCENTER);
#endif

#ifdef Q_OS_MAC
    aJustify = createAction(this, "aJustify", ICONALIGNJUSTIFY, A_ALIGN_JUSTIFY, paragraphContext, ALIGNJUSTIFY_TEXT, cmd, m_ParagraphMenu, G_FORMAT_PARAGRAPH, K_ALIGNJUSTIFY_MAC);
#else
    aJustify = createAction(this, "aJustify", ICONALIGNJUSTIFY, A_ALIGN_JUSTIFY, paragraphContext, ALIGNJUSTIFY_TEXT, cmd, m_ParagraphMenu, G_FORMAT_PARAGRAPH, K_ALIGNJUSTIFY);
#endif

    // Table actions
    aAddTable = createAction(this, "aAddTable", ICONTABLE, A_TABLE_ADDTABLE, tableContext, TABLE_ADDTABLE_TEXT, cmd, m_TableMenu, G_FORMAT_TABLE);
    aTableProperties = createAction(this, "aTableProperties", ICONTABLEPROPERTIES, A_TABLE_TABLEPROP, tableContext, TABLE_PROPERTIES_TEXT, cmd, m_TableMenu, G_FORMAT_TABLE);
    aAddRow = createAction(this, "aAddRow", ICONTABLEADDROW, A_TABLE_ADDROW, tableContext, TABLE_ADDROW_TEXT, cmd, m_TableMenu, G_FORMAT_TABLE);
    aAddCol = createAction(this, "aAddCol", ICONTABLEADDCOLUMN, A_TABLE_ADDCOLUMN, tableContext, TABLE_ADDCOLUMN_TEXT, cmd, m_TableMenu, G_FORMAT_TABLE);
    aRemoveRow = createAction(this, "aRemoveRow", ICONTABLEREMOVEROW, A_TABLE_REMOVEROW, tableContext, TABLE_REMOVEROW_TEXT, cmd, m_TableMenu, G_FORMAT_TABLE);
    aRemoveCol = createAction(this, "aRemoveCol", ICONTABLEREMOVECOLUMN, A_TABLE_REMOVECOLUMN, tableContext, TABLE_REMOVECOLUMN_TEXT, cmd, m_TableMenu, G_FORMAT_TABLE);
    aMergeCells = createAction(this, "aMergeCells", ICONTABLEMERGECELLS, A_TABLE_MERGECELLS, tableContext, TABLE_MERGECELLS_TEXT, cmd, m_TableMenu, G_FORMAT_TABLE);
    aSplitCells = createAction(this, "aSplitCells", ICONTABLESPLITCELLS, A_TABLE_SPLITCELLS, tableContext, TABLE_SPLITCELLS_TEXT, cmd, m_TableMenu, G_FORMAT_TABLE);

//    // New File Action
//    aNew = new QAction(this);
//    cmd = actionManager()->registerAction(a, Core::Constants::A_FILE_NEW, ioContext);
//    cmd->setTranslations(Core::Constants::EDITOR_FILEOPEN_TEXT);
////    connect(aNew, SIGNAL(triggered()), this, SLOT(newFile()));

    // File Actions
    // TODO: change the group for apps that do not use the GeneralMenu
    if (generalMenu)
        aOpen = createAction(this, "aOpen", Core::Constants::ICONOPEN, A_EDITOR_FILEOPEN, ioContext, EDITOR_FILEOPEN_TEXT, cmd, m_FileMenu, G_GENERAL_FILE);
    else
        aOpen = createAction(this, "aOpen", Core::Constants::ICONOPEN, A_EDITOR_FILEOPEN, ioContext, EDITOR_FILEOPEN_TEXT, cmd, m_FileMenu, G_FILE_OPEN);

    if (generalMenu)
        aSave = createAction(this, "aSave", ICONSAVE, A_EDITOR_FILESAVE, ioContext, EDITOR_FILESAVE_TEXT, cmd, m_FileMenu, G_GENERAL_FILE);
    else
        aSave = createAction(this, "aSave", ICONSAVE, A_EDITOR_FILESAVE, ioContext, EDITOR_FILESAVE_TEXT, cmd, m_FileMenu, G_FILE_SAVE);
    actionManager()->command(A_FORMAT_FONTCOLOR)->setAttribute(Core::Command::CA_UpdateText);

    // Text autocompletion
    aAddDateLong = createAction(this, "aAddDateLong", "", A_EDITOR_ADDDATE_LONG, textAdderContext, EDITOR_ADDDATE_LONG_TEXT, cmd, m_AddTextMenu, Core::Constants::G_DEFAULT_ONE);
    aAddDateShort = createAction(this, "aAddDateShort", "", A_EDITOR_ADDDATE_SHORT, textAdderContext, EDITOR_ADDDATE_SHORT_TEXT, cmd, m_AddTextMenu, Core::Constants::G_DEFAULT_ONE);
    aAddUserName = createAction(this, "aAddUserName", "", A_EDITOR_ADDUSERNAME, textAdderContext, EDITOR_ADDUSERNAME_TEXT, cmd, m_AddTextMenu, Core::Constants::G_DEFAULT_ONE);
    aAddPatientName = createAction(this, "aAddPatientName", "", A_EDITOR_ADDPATIENTNAME, textAdderContext, EDITOR_ADDPATIENTNAME_TEXT, cmd, m_AddTextMenu, Core::Constants::G_DEFAULT_ONE);

    actionManager()->retranslateMenusAndActions();
}

void EditorActionHandler::connectActions()
{
    connect(aToggleToolBar, SIGNAL(triggered()), this, SLOT(toogleToolbar()));
    connect(aPrint,SIGNAL(triggered()),this,SLOT(print()));
    connect(aUndo,SIGNAL(triggered()),this,SLOT(undo()));
    connect(aRedo,SIGNAL(triggered()),this,SLOT(redo()));
    connect(aCopy,SIGNAL(triggered()),this,SLOT(copy()));
    connect(aCut,SIGNAL(triggered()),this,SLOT(cut()));
    connect(aPaste,SIGNAL(triggered()),this,SLOT(paste()));
    connect(aBold, SIGNAL(triggered()), this, SLOT(textBold()));
    connect(aItalic, SIGNAL(triggered()), this, SLOT(textItalic()));
    connect(aUnderline, SIGNAL(triggered()), this, SLOT(textUnderline()));
    connect(aStrike, SIGNAL(triggered()), this, SLOT(textStrike()));
    connect(aFontBigger, SIGNAL(triggered()), this, SLOT(fontBigger()));
    connect(aFontSmaller, SIGNAL(triggered()), this, SLOT(fontSmaller()));
    connect(aTypeWriterFont, SIGNAL(triggered()), this, SLOT(typeWriterFont()));
    connect(aFontFormat, SIGNAL(triggered()), this, SLOT(fontFormat()));
    connect(aColor, SIGNAL(triggered()), this, SLOT(textColor()));
    connect(aLeft, SIGNAL(triggered()), this, SLOT(textAlign()));
    connect(aRight, SIGNAL(triggered()), this, SLOT(textAlign()));
    connect(aCenter, SIGNAL(triggered()), this, SLOT(textAlign()));
    connect(aJustify, SIGNAL(triggered()), this, SLOT(textAlign()));
    connect(aAddTable, SIGNAL(triggered()), this, SLOT(addTable()));
    connect(aTableProperties, SIGNAL(triggered()), this, SLOT(tableProperties()));
    connect(aAddRow, SIGNAL(triggered()), this, SLOT(tableAddRow()));
    connect(aAddCol, SIGNAL(triggered()), this, SLOT(tableAddCol()));
    connect(aRemoveRow, SIGNAL(triggered()), this, SLOT(tableRemoveRow()));
    connect(aRemoveCol, SIGNAL(triggered()), this, SLOT(tableRemoveCol()));
    connect(aMergeCells, SIGNAL(triggered()), this, SLOT(tableMergeCells()));
    connect(aSplitCells, SIGNAL(triggered()), this, SLOT(tableSplitCells()));
    connect(aOpen, SIGNAL(triggered()), this, SLOT(fileOpen()));
    connect(aSave, SIGNAL(triggered()), this, SLOT(saveAs()));
    connect(aAddDateLong, SIGNAL(triggered()), this, SLOT(addDate()));
    connect(aAddDateShort, SIGNAL(triggered()), this, SLOT(addDate()));
    connect(aAddUserName, SIGNAL(triggered()), this, SLOT(addUserName()));
    connect(aAddPatientName, SIGNAL(triggered()), this, SLOT(addPatientName()));

    connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(clipboardDataChanged()));
}

void EditorActionHandler::updateRedoAction()
{
    if (aRedo)
        aRedo->setEnabled(m_CurrentEditor && m_CurrentEditor->textEdit()->document()->isRedoAvailable());
}

void EditorActionHandler::updateUndoAction()
{
    if (aUndo)
        aUndo->setEnabled(m_CurrentEditor && m_CurrentEditor->textEdit()->document()->isUndoAvailable());
}

void EditorActionHandler::updateCopyAction()
{
    if (!m_CurrentEditor)
        return;
    const bool hasCopyableText = m_CurrentEditor->textEdit()->textCursor().hasSelection();
    if (aCut)
        aCut->setEnabled(hasCopyableText);
    if (aCopy) {
        aCopy->setEnabled(hasCopyableText);
    }
}

void EditorActionHandler::updateTableActions()
{
    if (!m_CurrentEditor)
        return;
    bool ok = (m_CurrentEditor->textEdit()->textCursor().currentTable());
    aTableProperties->setEnabled(ok);
    aAddRow->setEnabled(ok);
    aAddCol->setEnabled(ok);
    aRemoveRow->setEnabled(ok);
    aRemoveCol->setEnabled(ok);
    aMergeCells->setEnabled(ok);
    aSplitCells->setEnabled(ok);
}

void EditorActionHandler::updateActions()
{
    updateCopyAction();
    updateUndoAction();
    updateRedoAction();
    fontChanged(m_CurrentEditor->textEdit()->currentFont());
    updateTableActions();
}

void EditorActionHandler::updateColorActions()
{
    if (m_CurrentEditor) {
        colorChanged(m_CurrentEditor->textEdit()->currentCharFormat().foreground().color());
    }
}

void EditorActionHandler::clipboardDataChanged()
{
    aPaste->setEnabled(!QApplication::clipboard()->text().isEmpty());
}

void EditorActionHandler::setCurrentEditor(TextEditor *editor)
{
    // disconnect old editor
    if (m_CurrentEditor) {
        disconnect(m_CurrentEditor->textEdit(), SIGNAL(currentCharFormatChanged(QTextCharFormat)),
                   this, SLOT(currentCharFormatChanged(QTextCharFormat)));
        disconnect(m_CurrentEditor->textEdit(), SIGNAL(cursorPositionChanged()),
                   this, SLOT(cursorPositionChanged()));
        disconnect(m_CurrentEditor->textEdit(), SIGNAL(customContextMenuRequested(QPoint)),
                   m_CurrentEditor, SLOT(contextMenu(QPoint)));
        disconnect(m_CurrentEditor->textEdit(), SIGNAL(undoAvailable(bool)), this, SLOT(updateUndoAction()));
        disconnect(m_CurrentEditor->textEdit(), SIGNAL(redoAvailable(bool)), this, SLOT(updateRedoAction()));
        disconnect(m_CurrentEditor->textEdit(), SIGNAL(copyAvailable(bool)), this, SLOT(updateCopyAction()));
        // hide toolbar
        m_CurrentEditor->hideToolbar();
    }

    m_CurrentEditor = editor;
    if (!editor) { // this should never be the case
        return;
    }

    // reconnect some actions
    connect(m_CurrentEditor->textEdit(), SIGNAL(currentCharFormatChanged(QTextCharFormat)),
            this, SLOT(currentCharFormatChanged(QTextCharFormat)), Qt::UniqueConnection);
    connect(m_CurrentEditor->textEdit(), SIGNAL(cursorPositionChanged()),
            this, SLOT(cursorPositionChanged()), Qt::UniqueConnection);
    connect(m_CurrentEditor->textEdit(), SIGNAL(customContextMenuRequested(QPoint)),
            m_CurrentEditor, SLOT(contextMenu(QPoint)), Qt::UniqueConnection);
    connect(m_CurrentEditor->textEdit(), SIGNAL(undoAvailable(bool)), this, SLOT(updateUndoAction()), Qt::UniqueConnection);
    connect(m_CurrentEditor->textEdit(), SIGNAL(redoAvailable(bool)), this, SLOT(updateRedoAction()), Qt::UniqueConnection);
    connect(m_CurrentEditor->textEdit(), SIGNAL(copyAvailable(bool)), this, SLOT(updateCopyAction()), Qt::UniqueConnection);

    // show toolbar
    m_CurrentEditor->toogleToolbar(m_CurrentEditor->toolbarIsVisible());
    aToggleToolBar->setChecked(m_CurrentEditor->toolbarIsVisible());
    updateActions();
    updateColorActions();
}

void EditorActionHandler::currentCharFormatChanged( const QTextCharFormat &format )
{
    fontChanged( format.font() );
    colorChanged( format.foreground().color() );
}

void EditorActionHandler::cursorPositionChanged()
{
    if (m_CurrentEditor) {
        alignmentChanged(m_CurrentEditor->textEdit()->alignment());
        updateActions();
    }
}

void EditorActionHandler::fontChanged( const QFont &f )
{
    aBold->setChecked(f.bold());
    aItalic->setChecked(f.italic());
    aUnderline->setChecked(f.underline());
    aStrike->setChecked(f.strikeOut());
}

void EditorActionHandler::colorChanged( const QColor &c )
{
    QPixmap pix(16, 16);
    pix.fill(c);
    aColor->setIcon(pix);
}

void EditorActionHandler::alignmentChanged( Qt::Alignment a )
{
    if ( a & Qt::AlignLeft )
        aLeft->setChecked( true );
    else if ( a & Qt::AlignHCenter )
        aCenter->setChecked( true );
    else if ( a & Qt::AlignRight )
        aRight->setChecked( true );
    else if ( a & Qt::AlignJustify )
        aJustify->setChecked( true );
}

void EditorActionHandler::toogleToolbar()
{
    if (m_CurrentEditor)
        m_CurrentEditor->toogleToolbar(aToggleToolBar->isChecked());
}

void EditorActionHandler::print()
{
    if (m_CurrentEditor)
        m_CurrentEditor->print();
}

void EditorActionHandler::fileOpen()
{ if (m_CurrentEditor) m_CurrentEditor->fileOpen(); }

void EditorActionHandler::saveAs()
{ if (m_CurrentEditor) m_CurrentEditor->saveAs(); }

void EditorActionHandler::copy()
{
    if (m_CurrentEditor) {
        m_CurrentEditor->textEdit()->copy();
        m_CurrentEditor->textEdit()->setFocus();
    }
}

void EditorActionHandler::cut()
{
    if (m_CurrentEditor) {
        m_CurrentEditor->textEdit()->cut();
        m_CurrentEditor->textEdit()->setFocus();
    }
}

void EditorActionHandler::paste()
{
    if (m_CurrentEditor) {
        m_CurrentEditor->textEdit()->paste();
        m_CurrentEditor->textEdit()->setFocus();
    }
}

void EditorActionHandler::undo()
{
    if (m_CurrentEditor) {
        m_CurrentEditor->textEdit()->undo();
        m_CurrentEditor->textEdit()->setFocus();
    }
}

void EditorActionHandler::redo()
{
    if (m_CurrentEditor) {
        m_CurrentEditor->textEdit()->redo();
        m_CurrentEditor->textEdit()->setFocus();
    }
}


void EditorActionHandler::textBold()
{
    if (!m_CurrentEditor)
        return;
    QAction *a = qobject_cast<QAction*>(sender());
    m_CurrentEditor->textBold(a->isChecked());
    m_CurrentEditor->textEdit()->setFocus();
}

void EditorActionHandler::textUnderline()
{
    if (!m_CurrentEditor)
        return;
    QAction *a = qobject_cast<QAction*>(sender());
    m_CurrentEditor->textUnderline(a->isChecked());
    m_CurrentEditor->textEdit()->setFocus();
}

void EditorActionHandler::textItalic()
{
    if (!m_CurrentEditor)
        return;
    QAction *a = qobject_cast<QAction*>(sender());
    m_CurrentEditor->textItalic(a->isChecked());
    m_CurrentEditor->textEdit()->setFocus();
}

void EditorActionHandler::textStrike()
{
    if (!m_CurrentEditor)
        return;
    QAction *a = qobject_cast<QAction*>(sender());
    m_CurrentEditor->textStrike(a->isChecked());
    m_CurrentEditor->textEdit()->setFocus();
}

void EditorActionHandler::fontBigger()
{
    if (m_CurrentEditor)
        m_CurrentEditor->fontBigger();
    m_CurrentEditor->textEdit()->setFocus();
}

void EditorActionHandler::fontSmaller()
{
    if (m_CurrentEditor)
        m_CurrentEditor->fontSmaller();
    m_CurrentEditor->textEdit()->setFocus();
}

void EditorActionHandler::typeWriterFont()
{
    if (m_CurrentEditor)
        m_CurrentEditor->typeWriterFont();
    m_CurrentEditor->textEdit()->setFocus();
}

void EditorActionHandler::textColor()
{
    if (m_CurrentEditor) {
        m_CurrentEditor->textColor();
        // TODO: update color in action
    }
    m_CurrentEditor->textEdit()->setFocus();
}

void EditorActionHandler::fontFormat()
{
    if (m_CurrentEditor)
        m_CurrentEditor->fontFormat();
}

void EditorActionHandler::textAlign()
{
    if (m_CurrentEditor) {
        bool focus = m_CurrentEditor->textEdit()->hasFocus();
        QAction *a = qobject_cast<QAction*>(sender());
        if ( a == aLeft )
            m_CurrentEditor->textEdit()->setAlignment( Qt::AlignLeft );
        else if ( a == aCenter )
            m_CurrentEditor->textEdit()->setAlignment( Qt::AlignHCenter );
        else if ( a == aRight )
            m_CurrentEditor->textEdit()->setAlignment( Qt::AlignRight );
        else if ( a == aJustify )
            m_CurrentEditor->textEdit()->setAlignment( Qt::AlignJustify );
        if (focus)
            m_CurrentEditor->textEdit()->setFocus();
    }
}

void EditorActionHandler::addTable()
{ if (m_CurrentEditor) m_CurrentEditor->addTable (); }

void EditorActionHandler::tableProperties()
{ if (m_CurrentEditor) m_CurrentEditor->tableProperties (); }

void EditorActionHandler::tableAddRow()
{ if (m_CurrentEditor) m_CurrentEditor->tableAddRow (); }
void EditorActionHandler::tableAddCol()
{ if (m_CurrentEditor) m_CurrentEditor->tableAddCol (); }
void EditorActionHandler::tableRemoveCol()
{ if (m_CurrentEditor) m_CurrentEditor->tableRemoveCol (); }
void EditorActionHandler::tableRemoveRow()
{ if (m_CurrentEditor) m_CurrentEditor->tableRemoveRow (); }
void EditorActionHandler::tableMergeCells()
{ if (m_CurrentEditor) m_CurrentEditor->tableMergeCells (); }
void EditorActionHandler::tableSplitCells()
{ if (m_CurrentEditor) m_CurrentEditor->tableSplitCells (); }

void EditorActionHandler::addDate()
{
    QAction *a = qobject_cast<QAction*>(sender());
    if (!a)
        return;

    if (m_CurrentEditor) {
        if (a == aAddDateLong)
            m_CurrentEditor->addDate(TextEditor::LongFormat);
        else if (a == aAddDateShort)
            m_CurrentEditor->addDate(TextEditor::ShortFormat);
    }
}

void EditorActionHandler::addUserName()
{
    if (m_CurrentEditor) {
        m_CurrentEditor->addUserName();
    }
}

void EditorActionHandler::addPatientName()
{
    if (m_CurrentEditor) {
        m_CurrentEditor->addPatientName();
    }
}

