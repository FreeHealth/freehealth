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
#include "tkRichTextActionHandler.h"
#include "tkRichTextEditor.h"

#include <tkUniqueIdentifier.h>
#include <tkTheme.h>
#include <tkConstants.h>
#include <tkConstantTranslations.h>
#include <tkActionManager.h>

Q_TK_USING_CONSTANTS
Q_TK_USING_TRANSLATIONS

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QDebug>

namespace tkRichTextActionHandlerPrivate {

    QAction *registerAction(const QString &id,
                            const QList<int> &ctx,
                            QObject *parent)
    {
        QAction *a = new QAction(parent);
        tkCommand *cmd = tkActionManager::instance()->registerAction(a, id, ctx);
        Q_UNUSED(cmd);
        return a;
    }

}

using namespace tkRichTextActionHandlerPrivate;

const char* const M_EDITOR_EDIT = "menu.editor.edition";
const char* const M_EDITOR_FILE = "menu.editor.file";

tkRichTextActionHandler::tkRichTextActionHandler(QObject *parent) :
        QObject(parent),
        aOpen(0), aSave(0),
        aUndo(0), aRedo(0),
        aCut(0), aCopy(0), aPaste(0),
        aBold(0), aUnderline(0), aStrike(0),
        aItalic(0), aColor(0),
        aFontFormat(0), aFontBigger(0), aFontSmaller(0),
        aLeft(0), aCenter(0), aRight(0), aJustify(0),
        aAddTable(0),
        aAddRow(0),aAddCol(0),
        aRemoveRow(0), aRemoveCol(0),
        aMergeCells(0), aSplitCells(0),
        m_CurrentEditor(0)
{
    tkActionManager *am = tkActionManager::instance();
    QList<int> basicContext = QList<int>() << tkUniqueIdentifier::instance()->uniqueIdentifier(C_EDITOR_BASIC);
    QList<int> ioContext = QList<int>() << tkUniqueIdentifier::instance()->uniqueIdentifier(C_EDITOR_IO);
    QList<int> tableContext = QList<int>() << tkUniqueIdentifier::instance()->uniqueIdentifier(C_EDITOR_TABLE);

    QList<int> allContexts;
    allContexts << basicContext << ioContext << tableContext;

    // create specific editor menus / actions

    // Editor's Contextual Menu
    tkActionContainer *mctx = am->createMenu(tkConstants::M_EDITOR_CONTEXT);
    mctx->appendGroup(tkConstants::G_EDITOR_CONTEXT);
    mctx->setTranslations(tkConstants::EDITORMENU_TEXT);
    QAction *a = aToggleToolBar = new QAction(this);
    a->setObjectName("aToggleToolBar");
    a->setCheckable(true);
    a->setChecked(false);
    tkCommand *cmd = am->registerAction(a, tkConstants::A_EDITOR_TOOGLETOOLBAR, allContexts);
    cmd->setTranslations(tkConstants::EDITOR_TOGGLETOOLBAR_TEXT);
    connect(a, SIGNAL(triggered()), this, SLOT(toogleToolbar()));
    mctx->addAction(cmd, tkConstants::G_EDITOR_CONTEXT);

    // Menu structure -- rootMenu is menubar if exists otherwise it is a specific editor's menu tkConstants::M_EDITOR
    tkActionContainer *rootMenu;
    tkActionContainer *editMenu;
    tkActionContainer *fileMenu;
    if ((!am->actionContainer(tkConstants::MENUBAR)) && (!am->actionContainer(tkConstants::M_EDITOR))) {
        rootMenu = am->createMenu(tkConstants::M_EDITOR);
        rootMenu->appendGroup(tkConstants::G_EDIT);
        editMenu = am->createMenu(M_EDITOR_EDIT);
        rootMenu->addMenu(editMenu, tkConstants::G_EDIT);
        editMenu->appendGroup(tkConstants::G_EDIT_UNDOREDO);
        editMenu->appendGroup(tkConstants::G_EDIT_COPYPASTE);
        editMenu->appendGroup(tkConstants::G_EDIT_FIND);
        fileMenu = am->createMenu(M_EDITOR_FILE);
        fileMenu->appendGroup(tkConstants::G_FILE_OPEN);
        fileMenu->appendGroup(tkConstants::G_FILE_SAVE);
        fileMenu->appendGroup(tkConstants::G_FILE_PRINT);

        // create edition actions
        if (!am->command(tkConstants::A_EDIT_UNDO)) {
            QAction *a = new QAction(this);
            a->setIcon(tkTheme::icon(tkConstants::ICONUNDO));
            tkCommand *cmd = am->registerAction(a, tkConstants::A_EDIT_UNDO, allContexts);
            cmd->setDefaultKeySequence(QKeySequence::Undo);
            //    cmd->setAttribute(tkCommand::CA_UpdateText);
            cmd->setTranslations(tkConstants::EDITUNDO_TEXT);
            editMenu->addAction(cmd, tkConstants::G_EDIT_UNDOREDO);
            a->setEnabled(false);
        }
        if (!am->command(tkConstants::A_EDIT_REDO)) {
            a = new QAction(this);
            a->setIcon(tkTheme::icon(tkConstants::ICONREDO));
            cmd = am->registerAction(a, tkConstants::A_EDIT_REDO, allContexts);
            cmd->setDefaultKeySequence(QKeySequence::Redo);
            cmd->setTranslations( tkConstants::EDITREDO_TEXT );
            editMenu->addAction(cmd, tkConstants::G_EDIT_UNDOREDO);
            a->setEnabled(false);
        }
        if (!am->command(tkConstants::A_EDIT_CUT)) {
            a = new QAction(this);
            a->setIcon(tkTheme::icon(tkConstants::ICONCUT));
            cmd = am->registerAction(a, tkConstants::A_EDIT_CUT, allContexts);
            cmd->setDefaultKeySequence(QKeySequence::Cut);
            cmd->setTranslations( tkConstants::EDITCUT_TEXT );
            editMenu->addAction(cmd, tkConstants::G_EDIT_COPYPASTE);
            a->setEnabled(false);
        }
        if (!am->command(tkConstants::A_EDIT_COPY)) {
            a = new QAction(this);
            a->setIcon(tkTheme::icon(tkConstants::ICONCOPY));
            cmd = am->registerAction(a, tkConstants::A_EDIT_COPY, allContexts);
            cmd->setDefaultKeySequence(QKeySequence::Copy);
            cmd->setTranslations( tkConstants::EDITCOPY_TEXT );
            editMenu->addAction(cmd, tkConstants::G_EDIT_COPYPASTE);
            a->setEnabled(false);
        }
        if (!am->command(tkConstants::A_EDIT_PASTE)) {
            a = new QAction(this);
            a->setIcon(tkTheme::icon(tkConstants::ICONPASTE));
            cmd = am->registerAction(a, tkConstants::A_EDIT_PASTE, allContexts);
            cmd->setDefaultKeySequence(QKeySequence::Paste);
            cmd->setTranslations( tkConstants::EDITPASTE_TEXT );
            editMenu->addAction(cmd, tkConstants::G_EDIT_COPYPASTE);
            a->setEnabled(false);
        }
    } else {
        qWarning()<< "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx using menubar";
        rootMenu = am->actionContainer(tkConstants::MENUBAR);
        editMenu = am->actionContainer(tkConstants::M_EDIT);
        fileMenu = am->actionContainer(tkConstants::M_FILE);
    }

    // register already existing menu actions
    aUndo = registerAction(tkConstants::A_EDIT_UNDO,  allContexts, this);
    aRedo = registerAction(tkConstants::A_EDIT_REDO,  allContexts, this);
    aCut = registerAction(tkConstants::A_EDIT_CUT,   allContexts, this);
    aCopy = registerAction(tkConstants::A_EDIT_COPY,  allContexts, this);
    aPaste = registerAction(tkConstants::A_EDIT_PASTE, allContexts, this);


    // Menu Edit --> text formats
    //    tkActionContainer *medit = am->actionContainer(tkConstants::M_EDIT);
    tkActionContainer *formatMenu = am->actionContainer(tkConstants::M_FORMAT);
    if (!formatMenu) {
        //        tkActionContainer *menubar = am->createMenuBar(tkConstants::MENUBAR);
        //        Q_ASSERT(menubar);
        rootMenu->appendGroup(tkConstants::G_FORMAT);
        formatMenu = am->createMenu(tkConstants::M_FORMAT);
        rootMenu->addMenu(formatMenu, tkConstants::G_FORMAT);
        formatMenu->setTranslations(tkConstants::M_FORMAT_TEXT);
        formatMenu->appendGroup(tkConstants::G_FORMAT_FONT);
        formatMenu->appendGroup(tkConstants::G_FORMAT_PARAGRAPH);
        formatMenu->appendGroup(tkConstants::G_FORMAT_TABLE);
        formatMenu->appendGroup(tkConstants::G_FORMAT_IMAGE);
        formatMenu->appendGroup(tkConstants::G_FORMAT_OTHER);
    }
    tkActionContainer *fontMenu = am->actionContainer(tkConstants::M_FORMAT_FONT);
    if (!fontMenu) {
        fontMenu = am->createMenu(tkConstants::M_FORMAT_FONT);
        formatMenu->addMenu(fontMenu, tkConstants::G_FORMAT_FONT);
        fontMenu->setTranslations(tkConstants::M_FORMAT_FONT_TEXT);
        fontMenu->appendGroup(tkConstants::G_FORMAT_FONT);
    }
    tkActionContainer *pMenu = am->actionContainer(tkConstants::M_FORMAT_PARAGRAPH);
    if (!pMenu) {
        pMenu = am->createMenu(tkConstants::M_FORMAT_PARAGRAPH);
        formatMenu->addMenu(pMenu, tkConstants::G_FORMAT_PARAGRAPH);
        pMenu->setTranslations(tkConstants::M_FORMAT_PARAGRAPH_TEXT);
        pMenu->appendGroup(tkConstants::G_FORMAT_PARAGRAPH);
    }
    tkActionContainer *tabMenu = am->actionContainer(tkConstants::M_FORMAT_TABLE);
    if (!tabMenu) {
        tabMenu = am->createMenu(tkConstants::M_FORMAT_TABLE);
        formatMenu->addMenu(tabMenu, tkConstants::G_FORMAT_TABLE);
        tabMenu->setTranslations(tkConstants::M_FORMAT_TABLE_TEXT);
        tabMenu->appendGroup(tkConstants::G_FORMAT_TABLE);
//        tabMenu->setEmptyAction(ActionContainer::EA_Hide);
    }

    // Bold action
    a = aBold = new QAction(this);
    a->setObjectName("aBold");
    a->setIcon(tkTheme::icon(tkConstants::ICONBOLD));
    a->setCheckable(true);
    a->setChecked(false);
    cmd = am->registerAction(a, tkConstants::A_FORMAT_BOLD, allContexts);
    cmd->setTranslations(tkConstants::FORMATBOLD_TEXT);
    connect(a, SIGNAL(triggered()), this, SLOT(textBold()));
    fontMenu->addAction(cmd, tkConstants::G_FORMAT_FONT);

    // Italic action
    a = aItalic = new QAction(this);
    a->setObjectName("aItalic");
    a->setIcon(tkTheme::icon(tkConstants::ICONITALIC));
    a->setCheckable(true);
    a->setChecked(false);
    cmd = am->registerAction(a, tkConstants::A_FORMAT_ITALIC, allContexts);
    cmd->setTranslations(tkConstants::FORMATITALIC_TEXT);
    connect(a, SIGNAL(triggered()), this, SLOT(textItalic()));
    fontMenu->addAction(cmd, tkConstants::G_FORMAT_FONT);

    // Underline action
    a = aUnderline = new QAction(this);
    a->setObjectName("aUnderline");
    a->setIcon(tkTheme::icon(tkConstants::ICONUNDERLINE));
    a->setCheckable(true);
    a->setChecked(false);
    cmd = am->registerAction(a, tkConstants::A_FORMAT_UNDERLINE, allContexts);
    cmd->setTranslations(tkConstants::FORMATUNDERLINE_TEXT);
    connect(a, SIGNAL(triggered()), this, SLOT(textUnderline()));
    fontMenu->addAction(cmd, tkConstants::G_FORMAT_FONT);

    // Strike action
    a = aStrike = new QAction(this);
    a->setObjectName("aStrike");
    a->setIcon(tkTheme::icon(tkConstants::ICONSTRIKE));
    a->setCheckable(true);
    a->setChecked(false);
    cmd = am->registerAction(a, tkConstants::A_FORMAT_STRIKE, allContexts);
    cmd->setTranslations(tkConstants::FORMATSTRIKE_TEXT);
    connect(a, SIGNAL(triggered()), this, SLOT(textStrike()));
    fontMenu->addAction(cmd, tkConstants::G_FORMAT_FONT);

    // separator
    a = new QAction(this);
    a->setSeparator(true);
    cmd = am->registerAction(a, "Editor.Font.Sep.First", allContexts);
    fontMenu->addAction(cmd, tkConstants::G_FORMAT_FONT);

    // aFontBigger action
    a = aFontBigger = new QAction(this);
    a->setObjectName("aFontBigger");
    a->setIcon(tkTheme::icon(tkConstants::ICONFONTBIGGER));
    cmd = am->registerAction(a, tkConstants::A_FORMAT_BIGGER, allContexts);
    cmd->setTranslations(tkConstants::FORMATBIGGER_TEXT);
    connect(a, SIGNAL(triggered()), this, SLOT(fontBigger()));
    fontMenu->addAction(cmd, tkConstants::G_FORMAT_FONT);

    // aFontSmaller action
    a = aFontSmaller = new QAction(this);
    a->setObjectName("aFontSmaller");
    a->setIcon(tkTheme::icon(tkConstants::ICONFONTSMALLER));
    cmd = am->registerAction(a, tkConstants::A_FORMAT_SMALLER, allContexts);
    cmd->setTranslations(tkConstants::FORMATSMALLER_TEXT);
    connect(a, SIGNAL(triggered()), this, SLOT(fontSmaller()));
    fontMenu->addAction(cmd, tkConstants::G_FORMAT_FONT);

    // separator
    a = new QAction(this);
    a->setSeparator(true);
    cmd = am->registerAction(a, "Editor.Font.Sep.Second", allContexts);
    fontMenu->addAction(cmd, tkConstants::G_FORMAT_FONT);

    // Font Format action
    a = aFontFormat = new QAction(this);
    a->setObjectName("aFontFormat");
    a->setIcon(tkTheme::icon(tkConstants::ICONFONTFORMAT));
    cmd = am->registerAction(a, tkConstants::A_FORMAT_FONT, allContexts);
    cmd->setTranslations(tkConstants::FORMATFONT_TEXT);
    connect(a, SIGNAL(triggered()), this, SLOT(fontFormat()));
    fontMenu->addAction(cmd, tkConstants::G_FORMAT_FONT);

    // Color action
    a = aColor = new QAction(this);
    a->setObjectName("aColor");
    QPixmap p(16,16);
    p.fill(Qt::black);
    a->setIcon(p);
    cmd = am->registerAction(a, tkConstants::A_FORMAT_FONTCOLOR, allContexts);
    cmd->setAttribute(tkCommand::CA_UpdateIcon);
    cmd->setTranslations(FORMATFONTCOLOR_TEXT);
    connect(a, SIGNAL(triggered()), this, SLOT(textColor()));
    fontMenu->addAction(cmd, tkConstants::G_FORMAT_FONT);

    // Align Left action
    a = aLeft = new QAction(this);
    a->setObjectName("aLeft");
    a->setIcon(tkTheme::icon(tkConstants::ICONALIGNLEFT));
    cmd = am->registerAction(a, tkConstants::A_ALIGN_LEFT, allContexts);
    cmd->setTranslations(tkConstants::ALIGNLEFT_TEXT);
#ifdef Q_OS_MAC
    cmd->setDefaultKeySequence(tkTr(K_ALIGNLEFT_MAC));
#else
    cmd->setDefaultKeySequence(tkTr(K_ALIGNLEFT));
#endif
    connect(a, SIGNAL(triggered()), this, SLOT(textAlign()));
    pMenu->addAction(cmd, tkConstants::G_FORMAT_PARAGRAPH);

    // Align Right action
    a = aRight = new QAction(this);
    a->setObjectName("aRight");
    a->setIcon(tkTheme::icon(tkConstants::ICONALIGNRIGHT));
    cmd = am->registerAction(a, tkConstants::A_ALIGN_RIGHT, allContexts);
    cmd->setTranslations(tkConstants::ALIGNRIGHT_TEXT);
#ifdef Q_OS_MAC
    cmd->setDefaultKeySequence(tkTr(K_ALIGNRIGHT_MAC));
#else
    cmd->setDefaultKeySequence(tkTr(K_ALIGNRIGHT));
#endif
    connect(a, SIGNAL(triggered()), this, SLOT(textAlign()));
    pMenu->addAction(cmd, tkConstants::G_FORMAT_PARAGRAPH);

    // Align Center action
    a = aCenter = new QAction(this);
    a->setObjectName("aCenter");
    a->setIcon(tkTheme::icon(tkConstants::ICONALIGNCENTER));
    cmd = am->registerAction(a, tkConstants::A_ALIGN_CENTER, allContexts);
#ifdef Q_OS_MAC
    cmd->setDefaultKeySequence(tkTr(K_ALIGNCENTER_MAC));
#else
    cmd->setDefaultKeySequence(tkTr(K_ALIGNCENTER));
#endif
    cmd->setTranslations(tkConstants::ALIGNCENTER_TEXT);
    connect(a, SIGNAL(triggered()), this, SLOT(textAlign()));
    pMenu->addAction(cmd, tkConstants::G_FORMAT_PARAGRAPH);

    // Align Justify action
    a = aJustify = new QAction(this);
    a->setObjectName("aJustify");
    a->setIcon(tkTheme::icon(tkConstants::ICONALIGNJUSTIFY));
    cmd = am->registerAction(a, tkConstants::A_ALIGN_JUSTIFY, allContexts);
    cmd->setTranslations(tkConstants::ALIGNJUSTIFY_TEXT);
#ifdef Q_OS_MAC
    cmd->setDefaultKeySequence(tkTr(K_ALIGNJUSTIFY_MAC));
#else
    cmd->setDefaultKeySequence(tkTr(K_ALIGNJUSTIFY));
#endif
    connect(a, SIGNAL(triggered()), this, SLOT(textAlign()));
    pMenu->addAction(cmd, tkConstants::G_FORMAT_PARAGRAPH);


    // Add Table action
    a = aAddTable = new QAction(this);
    a->setObjectName("aAddTable");
    a->setIcon(tkTheme::icon(tkConstants::ICONTABLE));
    cmd = am->registerAction(a, tkConstants::A_TABLE_ADDTABLE, tableContext);
    cmd->setTranslations(tkConstants::TABLE_ADDTABLE_TEXT);
    connect(a, SIGNAL(triggered()), this, SLOT(addTable()));
    tabMenu->addAction(cmd, tkConstants::G_FORMAT_TABLE);

    // Add row action
    a = aAddRow = new QAction(this);
    a->setObjectName("aAddRow");
    a->setIcon(tkTheme::icon(tkConstants::ICONTABLEADDROW));
    cmd = am->registerAction(a, tkConstants::A_TABLE_ADDROW, tableContext);
    cmd->setTranslations(tkConstants::TABLE_ADDROW_TEXT);
    connect(a, SIGNAL(triggered()), this, SLOT(tableAddRow()));
    tabMenu->addAction(cmd, tkConstants::G_FORMAT_TABLE);

    // Add column action
    a = aAddCol = new QAction(this);
    a->setObjectName("aAddCol");
    a->setIcon(tkTheme::icon(tkConstants::ICONTABLEADDCOLUMN));
    cmd = am->registerAction(a, tkConstants::A_TABLE_ADDCOLUMN, tableContext);
    cmd->setTranslations(tkConstants::TABLE_ADDCOLUMN_TEXT);
    connect(a, SIGNAL(triggered()), this, SLOT(tableAddCol()));
    tabMenu->addAction(cmd, tkConstants::G_FORMAT_TABLE);

    
    // Remove row action
    a = aRemoveRow = new QAction(this);
    a->setObjectName("aRemoveRow");
    a->setIcon(tkTheme::icon(tkConstants::ICONTABLEREMOVEROW));
    cmd = am->registerAction(a, tkConstants::A_TABLE_REMOVEROW, tableContext);
    cmd->setTranslations(tkConstants::TABLE_REMOVEROW_TEXT);
    connect(a, SIGNAL(triggered()), this, SLOT(tableRemoveRow()));
    tabMenu->addAction(cmd, tkConstants::G_FORMAT_TABLE);

    // Remove column action
    a = aRemoveCol = new QAction(this);
    a->setObjectName("aRemoveCol");
    a->setIcon(tkTheme::icon(tkConstants::ICONTABLEREMOVEROW));
    cmd = am->registerAction(a, tkConstants::A_TABLE_REMOVECOLUMN, tableContext);
    cmd->setTranslations(tkConstants::TABLE_REMOVECOLUMN_TEXT);
    connect(a, SIGNAL(triggered()), this, SLOT(tableRemoveCol()));
    tabMenu->addAction(cmd, tkConstants::G_FORMAT_TABLE);

    // MergeCells action
    a = aMergeCells = new QAction(this);
    a->setObjectName("aMergeCells");
    a->setIcon(tkTheme::icon(tkConstants::ICONTABLEMERGECELLS));
    cmd = am->registerAction(a, tkConstants::A_TABLE_MERGECELLS, tableContext);
    cmd->setTranslations(tkConstants::TABLE_MERGECELLS_TEXT);
    connect(a, SIGNAL(triggered()), this, SLOT(tableMergeCells()));
    tabMenu->addAction(cmd, tkConstants::G_FORMAT_TABLE);

    // Split Cells action
    a = aSplitCells = new QAction(this);
    a->setObjectName("aSplitCells");
    a->setIcon(tkTheme::icon(tkConstants::ICONTABLESPLITCELLS));
    cmd = am->registerAction(a, tkConstants::A_TABLE_SPLITCELLS, tableContext);
    cmd->setTranslations(tkConstants::TABLE_SPLITCELLS_TEXT);
    connect(a, SIGNAL(triggered()), this, SLOT(tableSplitCells()));
    tabMenu->addAction(cmd, tkConstants::G_FORMAT_TABLE);



//    // New File Action
//    a = aNew = new QAction(this);
//    cmd = am->registerAction(a, tkConstants::A_FILE_NEW, ioContext);
//    cmd->setTranslations(tkConstants::EDITOR_FILEOPEN_TEXT);
////    connect(aNew, SIGNAL(triggered()), this, SLOT(newFile()));

    // Open Action
    a = aOpen = new QAction(this);
    a->setObjectName("aOpen");
    a->setText(tkTr(EDITOR_FILEOPEN_TEXT));
    a->setIcon(tkTheme::icon(tkConstants::ICONOPEN));
    cmd = am->registerAction(a, tkConstants::A_EDITOR_FILEOPEN, ioContext);
    cmd->setTranslations(tkConstants::EDITOR_FILEOPEN_TEXT);
    fileMenu->addAction(cmd, tkConstants::G_FILE_OPEN);
    connect(aOpen, SIGNAL(triggered()), this, SLOT(fileOpen()));

    // Save Action
    a = aSave = new QAction(this);
    a->setIcon(tkTheme::icon(tkConstants::ICONSAVE));
    a->setText(tkTr(EDITOR_FILESAVE_TEXT));
    cmd = am->registerAction(a, tkConstants::A_EDITOR_FILESAVE, ioContext);
    cmd->setDefaultKeySequence(QKeySequence::Save);
    cmd->setTranslations( tkConstants::EDITOR_FILESAVE_TEXT );
    cmd->setAttribute(tkCommand::CA_UpdateText);
    fileMenu->addAction(cmd, tkConstants::G_FILE_SAVE);
    connect(aSave, SIGNAL(triggered()), this, SLOT(saveAs()));

    connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(clipboardDataChanged()));

    am->retranslateMenusAndActions();
}

tkRichTextActionHandler::~tkRichTextActionHandler()
{
}

void tkRichTextActionHandler::updateRedoAction()
{
    if (aRedo)
        aRedo->setEnabled(m_CurrentEditor && m_CurrentEditor->textEdit()->document()->isRedoAvailable());
}

void tkRichTextActionHandler::updateUndoAction()
{
    if (aUndo)
        aUndo->setEnabled(m_CurrentEditor && m_CurrentEditor->textEdit()->document()->isUndoAvailable());
}

void tkRichTextActionHandler::updateCopyAction()
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

void tkRichTextActionHandler::updateTableActions()
{
    if (!m_CurrentEditor)
        return;
    bool ok = (m_CurrentEditor->textEdit()->textCursor().currentTable());
    aAddRow->setEnabled(ok);
    aAddCol->setEnabled(ok);
    aRemoveRow->setEnabled(ok);
    aRemoveCol->setEnabled(ok);
    aMergeCells->setEnabled(ok);
    aSplitCells->setEnabled(ok);
}

void tkRichTextActionHandler::updateActions()
{
    updateCopyAction();
    updateUndoAction();
    updateRedoAction();
    fontChanged(m_CurrentEditor->textEdit()->currentFont());
}


void tkRichTextActionHandler::updateColorActions()
{
    if (m_CurrentEditor) {
        colorChanged(m_CurrentEditor->textEdit()->currentCharFormat().foreground().color());
    }
}

void tkRichTextActionHandler::clipboardDataChanged()
{
    aPaste->setEnabled(!QApplication::clipboard()->text().isEmpty());
}

void tkRichTextActionHandler::setCurrentEditor(tkRichTextEditor *editor)
{
    // disconnect old editor
    if (m_CurrentEditor) {
        disconnect( m_CurrentEditor->textEdit(), SIGNAL( currentCharFormatChanged( const QTextCharFormat & ) ),
                    this, SLOT( currentCharFormatChanged( const QTextCharFormat & ) ) );
        disconnect( m_CurrentEditor->textEdit(), SIGNAL( cursorPositionChanged() ),
                    this, SLOT( cursorPositionChanged() ) );
        disconnect( m_CurrentEditor->textEdit(), SIGNAL( customContextMenuRequested( const QPoint & ) ),
                    m_CurrentEditor, SLOT( contextMenu( const QPoint & ) ) );
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
    connect( m_CurrentEditor->textEdit(), SIGNAL( currentCharFormatChanged( const QTextCharFormat & ) ),
             this, SLOT( currentCharFormatChanged( const QTextCharFormat & ) ) );
    connect( m_CurrentEditor->textEdit(), SIGNAL( cursorPositionChanged() ),
             this, SLOT( cursorPositionChanged() ) );
    connect( m_CurrentEditor->textEdit(), SIGNAL( customContextMenuRequested( const QPoint & ) ),
             m_CurrentEditor, SLOT( contextMenu( const QPoint & ) ) );
    connect(m_CurrentEditor->textEdit(), SIGNAL(undoAvailable(bool)), this, SLOT(updateUndoAction()));
    connect(m_CurrentEditor->textEdit(), SIGNAL(redoAvailable(bool)), this, SLOT(updateRedoAction()));
    connect(m_CurrentEditor->textEdit(), SIGNAL(copyAvailable(bool)), this, SLOT(updateCopyAction()));

    // show toolbar
    m_CurrentEditor->toogleToolbar(m_CurrentEditor->toolbarIsVisible());
    aToggleToolBar->setChecked(m_CurrentEditor->toolbarIsVisible());
    updateActions();
    updateColorActions();
}


void tkRichTextActionHandler::currentCharFormatChanged( const QTextCharFormat &format )
{
    fontChanged( format.font() );
    colorChanged( format.foreground().color() );
}

void tkRichTextActionHandler::cursorPositionChanged()
{
    if (m_CurrentEditor) {
        alignmentChanged(m_CurrentEditor->textEdit()->alignment());
        updateActions();
    }
}

void tkRichTextActionHandler::fontChanged( const QFont &f )
{
    aBold->setChecked(f.bold());
    aItalic->setChecked(f.italic());
    aUnderline->setChecked(f.underline());
    aStrike->setChecked(f.strikeOut());
}

void tkRichTextActionHandler::colorChanged( const QColor &c )
{
    QPixmap pix(16, 16);
    pix.fill(c);
    aColor->setIcon(pix);
}

void tkRichTextActionHandler::alignmentChanged( Qt::Alignment a )
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

void tkRichTextActionHandler::toogleToolbar()
{
    if (m_CurrentEditor)
        m_CurrentEditor->toogleToolbar(aToggleToolBar->isChecked());
}

void tkRichTextActionHandler::fileOpen()
{ if (m_CurrentEditor) m_CurrentEditor->fileOpen(); }

void tkRichTextActionHandler::saveAs()
{ if (m_CurrentEditor) m_CurrentEditor->saveAs(); }


void tkRichTextActionHandler::textBold()
{
    if (!m_CurrentEditor)
        return;
    QAction *a = qobject_cast<QAction*>(sender());
    m_CurrentEditor->textBold(a->isChecked());
}

void tkRichTextActionHandler::textUnderline()
{
    if (!m_CurrentEditor)
        return;
    QAction *a = qobject_cast<QAction*>(sender());
    m_CurrentEditor->textUnderline(a->isChecked());
}

void tkRichTextActionHandler::textItalic()
{
    if (!m_CurrentEditor)
        return;
    QAction *a = qobject_cast<QAction*>(sender());
    m_CurrentEditor->textItalic(a->isChecked());
}

void tkRichTextActionHandler::textStrike()
{
    if (!m_CurrentEditor)
        return;
    QAction *a = qobject_cast<QAction*>(sender());
    m_CurrentEditor->textStrike(a->isChecked());
}

void tkRichTextActionHandler::fontBigger()
{
    if (m_CurrentEditor)
        m_CurrentEditor->fontBigger();
}

void tkRichTextActionHandler::fontSmaller()
{
    if (m_CurrentEditor)
        m_CurrentEditor->fontSmaller();
}

void tkRichTextActionHandler::textColor()
{
    if (m_CurrentEditor) {
        m_CurrentEditor->textColor();
        /** \todo update color in action */
    }
}

void tkRichTextActionHandler::fontFormat()
{
    if (m_CurrentEditor)
        m_CurrentEditor->fontFormat();
}

void tkRichTextActionHandler::textAlign()
{
    if (m_CurrentEditor) {
        QAction *a = qobject_cast<QAction*>(sender());
        if ( a == aLeft )
            m_CurrentEditor->textEdit()->setAlignment( Qt::AlignLeft );
        else if ( a == aCenter )
            m_CurrentEditor->textEdit()->setAlignment( Qt::AlignHCenter );
        else if ( a == aRight )
            m_CurrentEditor->textEdit()->setAlignment( Qt::AlignRight );
        else if ( a == aJustify )
            m_CurrentEditor->textEdit()->setAlignment( Qt::AlignJustify );
    }
}

void tkRichTextActionHandler::addTable()
{ if (m_CurrentEditor) m_CurrentEditor->addTable (); }
void tkRichTextActionHandler::tableAddRow()
{ if (m_CurrentEditor) m_CurrentEditor->tableAddRow (); }
void tkRichTextActionHandler::tableAddCol()
{ if (m_CurrentEditor) m_CurrentEditor->tableAddCol (); }
void tkRichTextActionHandler::tableRemoveCol()
{ if (m_CurrentEditor) m_CurrentEditor->tableRemoveCol (); }
void tkRichTextActionHandler::tableRemoveRow()
{ if (m_CurrentEditor) m_CurrentEditor->tableRemoveRow (); }
void tkRichTextActionHandler::tableMergeCells()
{ if (m_CurrentEditor) m_CurrentEditor->tableMergeCells (); }
void tkRichTextActionHandler::tableSplitCells()
{ if (m_CurrentEditor) m_CurrentEditor->tableSplitCells (); }

