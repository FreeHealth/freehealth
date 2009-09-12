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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "editoractionhandler.h"
#include "texteditor.h"

#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/uniqueidmanager.h>
#include <coreplugin/actionmanager/actionmanager.h>

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QDebug>


using namespace Editor;
using namespace Editor::Internal;
using namespace Trans::ConstantTranslations;

static QAction *registerAction(const QString &id, const QList<int> &ctx, QObject *parent)
{
    QAction *a = new QAction(parent);
    Core::Command *cmd = Core::ICore::instance()->actionManager()->registerAction(a, id, ctx);
    Q_UNUSED(cmd);
    return a;
}

EditorActionHandler::EditorActionHandler(QObject *parent) :
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
    Core::ActionManager *am = Core::ICore::instance()->actionManager();
    Core::UniqueIDManager *uid = Core::ICore::instance()->uniqueIDManager();
    Core::ITheme *th = Core::ICore::instance()->theme();
    QList<int> basicContext = QList<int>() << uid->uniqueIdentifier(Core::Constants::C_EDITOR_BASIC);
    QList<int> ioContext = QList<int>() << uid->uniqueIdentifier(Core::Constants::C_EDITOR_IO);
    QList<int> tableContext = QList<int>() << uid->uniqueIdentifier(Core::Constants::C_EDITOR_TABLE);

    QList<int> allContexts;
    allContexts << basicContext << ioContext << tableContext;

    // create specific editor menus / actions

    // Editor's Contextual Menu
    Core::ActionContainer *mctx = am->createMenu(Core::Constants::M_EDITOR_CONTEXT);
    mctx->appendGroup(Core::Constants::G_EDITOR_CONTEXT);
    mctx->setTranslations(Trans::Constants::EDITORMENU_TEXT);
    QAction *a = aToggleToolBar = new QAction(this);
    a->setObjectName("aToggleToolBar");
    a->setCheckable(true);
    a->setChecked(false);
    Core::Command *cmd = am->registerAction(a, Core::Constants::A_EDITOR_TOOGLETOOLBAR, allContexts);
    cmd->setTranslations(Trans::Constants::EDITOR_TOGGLETOOLBAR_TEXT);
    connect(a, SIGNAL(triggered()), this, SLOT(toogleToolbar()));
    mctx->addAction(cmd, Core::Constants::G_EDITOR_CONTEXT);

    // Menu structure -- rootMenu is menubar if exists otherwise it is a specific editor's menu Core::Constants::M_EDITOR
    Core::ActionContainer *rootMenu;
    Core::ActionContainer *editMenu;
    Core::ActionContainer *fileMenu;
    if ((am->actionContainer(Core::Constants::MENUBAR)) && (!am->actionContainer(Core::Constants::M_EDITOR))) {
        rootMenu = mctx;//am->createMenu(Core::Constants::M_EDITOR);
        rootMenu->appendGroup(Core::Constants::G_EDIT);
        editMenu = am->createMenu(Core::Constants::M_EDITOR_EDIT);
        editMenu->setTranslations(Trans::Constants::M_EDIT_TEXT);
        rootMenu->addMenu(editMenu, Core::Constants::G_EDIT);
        editMenu->appendGroup(Core::Constants::G_EDIT_UNDOREDO);
        editMenu->appendGroup(Core::Constants::G_EDIT_COPYPASTE);
        editMenu->appendGroup(Core::Constants::G_EDIT_FIND);
        fileMenu = am->createMenu(Core::Constants::M_EDITOR_FILE);
        fileMenu->appendGroup(Core::Constants::G_FILE_OPEN);
        fileMenu->appendGroup(Core::Constants::G_FILE_SAVE);
        fileMenu->appendGroup(Core::Constants::G_FILE_PRINT);
        // create edition actions
        if (!am->command(Core::Constants::A_EDIT_UNDO)) {
            a = aUndo = new QAction(this);
            a->setObjectName("aUndo");
            a->setIcon(th->icon(Core::Constants::ICONUNDO));
            cmd = am->registerAction(a, Core::Constants::A_EDIT_UNDO, allContexts);
            cmd->setDefaultKeySequence(QKeySequence::Undo);
            //    cmd->setAttribute(Core::Command::CA_UpdateText);
            cmd->setTranslations(Trans::Constants::EDITUNDO_TEXT);
            editMenu->addAction(cmd, Core::Constants::G_EDIT_UNDOREDO);
            a->setEnabled(false);
        }
        if (!am->command(Core::Constants::A_EDIT_REDO)) {
            a = aRedo = new QAction(this);
            a->setObjectName("aRedo");
            a->setIcon(th->icon(Core::Constants::ICONREDO));
            cmd = am->registerAction(a, Core::Constants::A_EDIT_REDO, allContexts);
            cmd->setDefaultKeySequence(QKeySequence::Redo);
            cmd->setTranslations(Trans::Constants::EDITREDO_TEXT );
            editMenu->addAction(cmd, Core::Constants::G_EDIT_UNDOREDO);
            a->setEnabled(false);
        }
        if (!am->command(Core::Constants::A_EDIT_CUT)) {
            a = aCut = new QAction(this);
            a->setObjectName("aCut");
            a->setIcon(th->icon(Core::Constants::ICONCUT));
            cmd = am->registerAction(a, Core::Constants::A_EDIT_CUT, allContexts);
            cmd->setDefaultKeySequence(QKeySequence::Cut);
            cmd->setTranslations(Trans::Constants::EDITCUT_TEXT );
            editMenu->addAction(cmd, Core::Constants::G_EDIT_COPYPASTE);
            a->setEnabled(false);
        }
        if (!am->command(Core::Constants::A_EDIT_COPY)) {
            a = aCopy = new QAction(this);
            a->setObjectName("copy");
            a->setIcon(th->icon(Core::Constants::ICONCOPY));
            cmd = am->registerAction(a, Core::Constants::A_EDIT_COPY, allContexts);
            cmd->setDefaultKeySequence(QKeySequence::Copy);
            cmd->setTranslations(Trans::Constants::EDITCOPY_TEXT );
            editMenu->addAction(cmd, Core::Constants::G_EDIT_COPYPASTE);
            a->setEnabled(false);
        }
        if (!am->command(Core::Constants::A_EDIT_PASTE)) {
            a = aPaste = new QAction(this);
            a->setObjectName("paste");
            a->setIcon(th->icon(Core::Constants::ICONPASTE));
            cmd = am->registerAction(a, Core::Constants::A_EDIT_PASTE, allContexts);
            cmd->setDefaultKeySequence(QKeySequence::Paste);
            cmd->setTranslations(Trans::Constants::EDITPASTE_TEXT );
            editMenu->addAction(cmd, Core::Constants::G_EDIT_COPYPASTE);
            a->setEnabled(false);
        }
    } else {
        rootMenu = am->actionContainer(Core::Constants::MENUBAR);
        editMenu = am->actionContainer(Core::Constants::M_EDIT);
        fileMenu = am->actionContainer(Core::Constants::M_FILE);
        // register already existing menu actions
        aUndo = registerAction(Core::Constants::A_EDIT_UNDO,  allContexts, this);
        aRedo = registerAction(Core::Constants::A_EDIT_REDO,  allContexts, this);
        aCut = registerAction(Core::Constants::A_EDIT_CUT,   allContexts, this);
        aCopy = registerAction(Core::Constants::A_EDIT_COPY,  allContexts, this);
        aPaste = registerAction(Core::Constants::A_EDIT_PASTE, allContexts, this);
    }

    // Connect Edit Actions
    connect(aUndo,SIGNAL(triggered()),this,SLOT(undo()));
    connect(aRedo,SIGNAL(triggered()),this,SLOT(redo()));
    connect(aCopy,SIGNAL(triggered()),this,SLOT(copy()));
    connect(aCut,SIGNAL(triggered()),this,SLOT(cut()));
    connect(aPaste,SIGNAL(triggered()),this,SLOT(paste()));

    // Menu Edit --> text formats
    //    tkActionContainer *medit = am->actionContainer(Core::Constants::M_EDIT);
    Core::ActionContainer *formatMenu = am->actionContainer(Core::Constants::M_FORMAT);
    if (!formatMenu) {
        //        tkActionContainer *menubar = am->createMenuBar(Core::Constants::MENUBAR);
        //        Q_ASSERT(menubar);
        rootMenu->appendGroup(Core::Constants::G_FORMAT);
        formatMenu = am->createMenu(Core::Constants::M_FORMAT);
        rootMenu->addMenu(formatMenu, Core::Constants::G_FORMAT);
        formatMenu->setTranslations(Trans::Constants::M_FORMAT_TEXT);
        formatMenu->appendGroup(Core::Constants::G_FORMAT_FONT);
        formatMenu->appendGroup(Core::Constants::G_FORMAT_PARAGRAPH);
        formatMenu->appendGroup(Core::Constants::G_FORMAT_TABLE);
        formatMenu->appendGroup(Core::Constants::G_FORMAT_IMAGE);
        formatMenu->appendGroup(Core::Constants::G_FORMAT_OTHER);
    }
    Core::ActionContainer *fontMenu = am->actionContainer(Core::Constants::M_FORMAT_FONT);
    if (!fontMenu) {
        fontMenu = am->createMenu(Core::Constants::M_FORMAT_FONT);
        formatMenu->addMenu(fontMenu, Core::Constants::G_FORMAT_FONT);
        fontMenu->setTranslations(Trans::Constants::M_FORMAT_FONT_TEXT);
        fontMenu->appendGroup(Core::Constants::G_FORMAT_FONT);
    }
    Core::ActionContainer *pMenu = am->actionContainer(Core::Constants::M_FORMAT_PARAGRAPH);
    if (!pMenu) {
        pMenu = am->createMenu(Core::Constants::M_FORMAT_PARAGRAPH);
        formatMenu->addMenu(pMenu, Core::Constants::G_FORMAT_PARAGRAPH);
        pMenu->setTranslations(Trans::Constants::M_FORMAT_PARAGRAPH_TEXT);
        pMenu->appendGroup(Core::Constants::G_FORMAT_PARAGRAPH);
    }
    Core::ActionContainer *tabMenu = am->actionContainer(Core::Constants::M_FORMAT_TABLE);
    if (!tabMenu) {
        tabMenu = am->createMenu(Core::Constants::M_FORMAT_TABLE);
        formatMenu->addMenu(tabMenu, Core::Constants::G_FORMAT_TABLE);
        tabMenu->setTranslations(Trans::Constants::M_FORMAT_TABLE_TEXT);
        tabMenu->appendGroup(Core::Constants::G_FORMAT_TABLE);
//        tabMenu->setEmptyAction(ActionContainer::EA_Hide);
    }

    // Bold action
    a = aBold = new QAction(this);
    a->setObjectName("aBold");
    a->setIcon(th->icon(Core::Constants::ICONBOLD));
    a->setCheckable(true);
    a->setChecked(false);
    cmd = am->registerAction(a, Core::Constants::A_FORMAT_BOLD, allContexts);
    cmd->setTranslations(Trans::Constants::FORMATBOLD_TEXT);
    connect(a, SIGNAL(triggered()), this, SLOT(textBold()));
    fontMenu->addAction(cmd, Core::Constants::G_FORMAT_FONT);

    // Italic action
    a = aItalic = new QAction(this);
    a->setObjectName("aItalic");
    a->setIcon(th->icon(Core::Constants::ICONITALIC));
    a->setCheckable(true);
    a->setChecked(false);
    cmd = am->registerAction(a, Core::Constants::A_FORMAT_ITALIC, allContexts);
    cmd->setTranslations(Trans::Constants::FORMATITALIC_TEXT);
    connect(a, SIGNAL(triggered()), this, SLOT(textItalic()));
    fontMenu->addAction(cmd, Core::Constants::G_FORMAT_FONT);

    // Underline action
    a = aUnderline = new QAction(this);
    a->setObjectName("aUnderline");
    a->setIcon(th->icon(Core::Constants::ICONUNDERLINE));
    a->setCheckable(true);
    a->setChecked(false);
    cmd = am->registerAction(a, Core::Constants::A_FORMAT_UNDERLINE, allContexts);
    cmd->setTranslations(Trans::Constants::FORMATUNDERLINE_TEXT);
    connect(a, SIGNAL(triggered()), this, SLOT(textUnderline()));
    fontMenu->addAction(cmd, Core::Constants::G_FORMAT_FONT);

    // Strike action
    a = aStrike = new QAction(this);
    a->setObjectName("aStrike");
    a->setIcon(th->icon(Core::Constants::ICONSTRIKE));
    a->setCheckable(true);
    a->setChecked(false);
    cmd = am->registerAction(a, Core::Constants::A_FORMAT_STRIKE, allContexts);
    cmd->setTranslations(Trans::Constants::FORMATSTRIKE_TEXT);
    connect(a, SIGNAL(triggered()), this, SLOT(textStrike()));
    fontMenu->addAction(cmd, Core::Constants::G_FORMAT_FONT);

    // separator
    a = new QAction(this);
    a->setSeparator(true);
    cmd = am->registerAction(a, "Editor.Font.Sep.First", allContexts);
    fontMenu->addAction(cmd, Core::Constants::G_FORMAT_FONT);

    // aFontBigger action
    a = aFontBigger = new QAction(this);
    a->setObjectName("aFontBigger");
    a->setIcon(th->icon(Core::Constants::ICONFONTBIGGER));
    cmd = am->registerAction(a, Core::Constants::A_FORMAT_BIGGER, allContexts);
    cmd->setTranslations(Trans::Constants::FORMATBIGGER_TEXT);
    connect(a, SIGNAL(triggered()), this, SLOT(fontBigger()));
    fontMenu->addAction(cmd, Core::Constants::G_FORMAT_FONT);

    // aFontSmaller action
    a = aFontSmaller = new QAction(this);
    a->setObjectName("aFontSmaller");
    a->setIcon(th->icon(Core::Constants::ICONFONTSMALLER));
    cmd = am->registerAction(a, Core::Constants::A_FORMAT_SMALLER, allContexts);
    cmd->setTranslations(Trans::Constants::FORMATSMALLER_TEXT);
    connect(a, SIGNAL(triggered()), this, SLOT(fontSmaller()));
    fontMenu->addAction(cmd, Core::Constants::G_FORMAT_FONT);

    // separator
    a = new QAction(this);
    a->setSeparator(true);
    cmd = am->registerAction(a, "Editor.Font.Sep.Second", allContexts);
    fontMenu->addAction(cmd, Core::Constants::G_FORMAT_FONT);

    // Font Format action
    a = aFontFormat = new QAction(this);
    a->setObjectName("aFontFormat");
    a->setIcon(th->icon(Core::Constants::ICONFONTFORMAT));
    cmd = am->registerAction(a, Core::Constants::A_FORMAT_FONT, allContexts);
    cmd->setTranslations(Trans::Constants::FORMATFONT_TEXT);
    connect(a, SIGNAL(triggered()), this, SLOT(fontFormat()));
    fontMenu->addAction(cmd, Core::Constants::G_FORMAT_FONT);

    // Color action
    a = aColor = new QAction(this);
    a->setObjectName("aColor");
    QPixmap p(16,16);
    p.fill(Qt::black);
    a->setIcon(p);
    cmd = am->registerAction(a, Core::Constants::A_FORMAT_FONTCOLOR, allContexts);
    cmd->setAttribute(Core::Command::CA_UpdateIcon);
    cmd->setTranslations(Trans::Constants::FORMATFONTCOLOR_TEXT);
    connect(a, SIGNAL(triggered()), this, SLOT(textColor()));
    fontMenu->addAction(cmd, Core::Constants::G_FORMAT_FONT);

    // Align Left action
    a = aLeft = new QAction(this);
    a->setObjectName("aLeft");
    a->setIcon(th->icon(Core::Constants::ICONALIGNLEFT));
    cmd = am->registerAction(a, Core::Constants::A_ALIGN_LEFT, allContexts);
    cmd->setTranslations(Trans::Constants::ALIGNLEFT_TEXT);
#ifdef Q_OS_MAC
    cmd->setDefaultKeySequence(tkTr(Trans::Constants::K_ALIGNLEFT_MAC));
#else
    cmd->setDefaultKeySequence(tkTr(Trans::Constants::K_ALIGNLEFT));
#endif
    connect(a, SIGNAL(triggered()), this, SLOT(textAlign()));
    pMenu->addAction(cmd, Core::Constants::G_FORMAT_PARAGRAPH);

    // Align Right action
    a = aRight = new QAction(this);
    a->setObjectName("aRight");
    a->setIcon(th->icon(Core::Constants::ICONALIGNRIGHT));
    cmd = am->registerAction(a, Core::Constants::A_ALIGN_RIGHT, allContexts);
    cmd->setTranslations(Trans::Constants::ALIGNRIGHT_TEXT);
#ifdef Q_OS_MAC
    cmd->setDefaultKeySequence(tkTr(Trans::Constants::K_ALIGNRIGHT_MAC));
#else
    cmd->setDefaultKeySequence(tkTr(Trans::Constants::K_ALIGNRIGHT));
#endif
    connect(a, SIGNAL(triggered()), this, SLOT(textAlign()));
    pMenu->addAction(cmd, Core::Constants::G_FORMAT_PARAGRAPH);

    // Align Center action
    a = aCenter = new QAction(this);
    a->setObjectName("aCenter");
    a->setIcon(th->icon(Core::Constants::ICONALIGNCENTER));
    cmd = am->registerAction(a, Core::Constants::A_ALIGN_CENTER, allContexts);
#ifdef Q_OS_MAC
    cmd->setDefaultKeySequence(tkTr(Trans::Constants::K_ALIGNCENTER_MAC));
#else
    cmd->setDefaultKeySequence(tkTr(Trans::Constants::K_ALIGNCENTER));
#endif
    cmd->setTranslations(Trans::Constants::ALIGNCENTER_TEXT);
    connect(a, SIGNAL(triggered()), this, SLOT(textAlign()));
    pMenu->addAction(cmd, Core::Constants::G_FORMAT_PARAGRAPH);

    // Align Justify action
    a = aJustify = new QAction(this);
    a->setObjectName("aJustify");
    a->setIcon(th->icon(Core::Constants::ICONALIGNJUSTIFY));
    cmd = am->registerAction(a, Core::Constants::A_ALIGN_JUSTIFY, allContexts);
    cmd->setTranslations(Trans::Constants::ALIGNJUSTIFY_TEXT);
#ifdef Q_OS_MAC
    cmd->setDefaultKeySequence(tkTr(Trans::Constants::K_ALIGNJUSTIFY_MAC));
#else
    cmd->setDefaultKeySequence(tkTr(Trans::Constants::K_ALIGNJUSTIFY));
#endif
    connect(a, SIGNAL(triggered()), this, SLOT(textAlign()));
    pMenu->addAction(cmd, Core::Constants::G_FORMAT_PARAGRAPH);


    // Add Table action
    a = aAddTable = new QAction(this);
    a->setObjectName("aAddTable");
    a->setIcon(th->icon(Core::Constants::ICONTABLE));
    cmd = am->registerAction(a, Core::Constants::A_TABLE_ADDTABLE, tableContext);
    cmd->setTranslations(Trans::Constants::TABLE_ADDTABLE_TEXT);
    connect(a, SIGNAL(triggered()), this, SLOT(addTable()));
    tabMenu->addAction(cmd, Core::Constants::G_FORMAT_TABLE);

    // Add row action
    a = aAddRow = new QAction(this);
    a->setObjectName("aAddRow");
    a->setIcon(th->icon(Core::Constants::ICONTABLEADDROW));
    cmd = am->registerAction(a, Core::Constants::A_TABLE_ADDROW, tableContext);
    cmd->setTranslations(Trans::Constants::TABLE_ADDROW_TEXT);
    connect(a, SIGNAL(triggered()), this, SLOT(tableAddRow()));
    tabMenu->addAction(cmd, Core::Constants::G_FORMAT_TABLE);

    // Add column action
    a = aAddCol = new QAction(this);
    a->setObjectName("aAddCol");
    a->setIcon(th->icon(Core::Constants::ICONTABLEADDCOLUMN));
    cmd = am->registerAction(a, Core::Constants::A_TABLE_ADDCOLUMN, tableContext);
    cmd->setTranslations(Trans::Constants::TABLE_ADDCOLUMN_TEXT);
    connect(a, SIGNAL(triggered()), this, SLOT(tableAddCol()));
    tabMenu->addAction(cmd, Core::Constants::G_FORMAT_TABLE);

    // Remove row action
    a = aRemoveRow = new QAction(this);
    a->setObjectName("aRemoveRow");
    a->setIcon(th->icon(Core::Constants::ICONTABLEREMOVEROW));
    cmd = am->registerAction(a, Core::Constants::A_TABLE_REMOVEROW, tableContext);
    cmd->setTranslations(Trans::Constants::TABLE_REMOVEROW_TEXT);
    connect(a, SIGNAL(triggered()), this, SLOT(tableRemoveRow()));
    tabMenu->addAction(cmd, Core::Constants::G_FORMAT_TABLE);

    // Remove column action
    a = aRemoveCol = new QAction(this);
    a->setObjectName("aRemoveCol");
    a->setIcon(th->icon(Core::Constants::ICONTABLEREMOVEROW));
    cmd = am->registerAction(a, Core::Constants::A_TABLE_REMOVECOLUMN, tableContext);
    cmd->setTranslations(Trans::Constants::TABLE_REMOVECOLUMN_TEXT);
    connect(a, SIGNAL(triggered()), this, SLOT(tableRemoveCol()));
    tabMenu->addAction(cmd, Core::Constants::G_FORMAT_TABLE);

    // MergeCells action
    a = aMergeCells = new QAction(this);
    a->setObjectName("aMergeCells");
    a->setIcon(th->icon(Core::Constants::ICONTABLEMERGECELLS));
    cmd = am->registerAction(a, Core::Constants::A_TABLE_MERGECELLS, tableContext);
    cmd->setTranslations(Trans::Constants::TABLE_MERGECELLS_TEXT);
    connect(a, SIGNAL(triggered()), this, SLOT(tableMergeCells()));
    tabMenu->addAction(cmd, Core::Constants::G_FORMAT_TABLE);

    // Split Cells action
    a = aSplitCells = new QAction(this);
    a->setObjectName("aSplitCells");
    a->setIcon(th->icon(Core::Constants::ICONTABLESPLITCELLS));
    cmd = am->registerAction(a, Core::Constants::A_TABLE_SPLITCELLS, tableContext);
    cmd->setTranslations(Trans::Constants::TABLE_SPLITCELLS_TEXT);
    connect(a, SIGNAL(triggered()), this, SLOT(tableSplitCells()));
    tabMenu->addAction(cmd, Core::Constants::G_FORMAT_TABLE);



//    // New File Action
//    a = aNew = new QAction(this);
//    cmd = am->registerAction(a, Core::Constants::A_FILE_NEW, ioContext);
//    cmd->setTranslations(Core::Constants::EDITOR_FILEOPEN_TEXT);
////    connect(aNew, SIGNAL(triggered()), this, SLOT(newFile()));

    // Open Action
    a = aOpen = new QAction(this);
    a->setObjectName("aOpen");
    a->setText(tkTr(Trans::Constants::EDITOR_FILEOPEN_TEXT));
    a->setIcon(th->icon(Core::Constants::ICONOPEN));
    cmd = am->registerAction(a, Core::Constants::A_EDITOR_FILEOPEN, ioContext);
    cmd->setTranslations(Trans::Constants::EDITOR_FILEOPEN_TEXT);
    fileMenu->addAction(cmd, Core::Constants::G_FILE_OPEN);
    connect(aOpen, SIGNAL(triggered()), this, SLOT(fileOpen()));

    // Save Action
    a = aSave = new QAction(this);
    a->setObjectName("aSave");
    a->setIcon(th->icon(Core::Constants::ICONSAVE));
    a->setText(tkTr(Trans::Constants::EDITOR_FILESAVE_TEXT));
    cmd = am->registerAction(a, Core::Constants::A_EDITOR_FILESAVE, ioContext);
    cmd->setDefaultKeySequence(QKeySequence::Save);
    cmd->setTranslations(Trans::Constants::EDITOR_FILESAVE_TEXT );
    cmd->setAttribute(Core::Command::CA_UpdateText);
    fileMenu->addAction(cmd, Core::Constants::G_FILE_SAVE);
    connect(aSave, SIGNAL(triggered()), this, SLOT(saveAs()));

    connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(clipboardDataChanged()));

    am->retranslateMenusAndActions();
}

EditorActionHandler::~EditorActionHandler()
{
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
}


void EditorActionHandler::updateColorActions()
{
    if (m_CurrentEditor) {
        colorChanged(m_CurrentEditor->textEdit()->currentCharFormat().foreground().color());
    }
}

void EditorActionHandler::clipboardDataChanged()
{
    qWarning() << "EditorActionHandler::clipboardDataChanged()";
    aPaste->setEnabled(!QApplication::clipboard()->text().isEmpty());
}

void EditorActionHandler::setCurrentEditor(TextEditor *editor)
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

void EditorActionHandler::fileOpen()
{ if (m_CurrentEditor) m_CurrentEditor->fileOpen(); }

void EditorActionHandler::saveAs()
{ if (m_CurrentEditor) m_CurrentEditor->saveAs(); }

    void EditorActionHandler::copy()
{ if (m_CurrentEditor) m_CurrentEditor->textEdit()->copy(); }
    void EditorActionHandler::cut()
{ if (m_CurrentEditor) m_CurrentEditor->textEdit()->cut(); }
    void EditorActionHandler::paste()
{ if (m_CurrentEditor) m_CurrentEditor->textEdit()->paste(); }
    void EditorActionHandler::undo()
{ if (m_CurrentEditor) m_CurrentEditor->textEdit()->undo(); }

    void EditorActionHandler::redo()
{ if (m_CurrentEditor) m_CurrentEditor->textEdit()->redo(); }


void EditorActionHandler::textBold()
{
    if (!m_CurrentEditor)
        return;
    QAction *a = qobject_cast<QAction*>(sender());
    m_CurrentEditor->textBold(a->isChecked());
}

void EditorActionHandler::textUnderline()
{
    if (!m_CurrentEditor)
        return;
    QAction *a = qobject_cast<QAction*>(sender());
    m_CurrentEditor->textUnderline(a->isChecked());
}

void EditorActionHandler::textItalic()
{
    if (!m_CurrentEditor)
        return;
    QAction *a = qobject_cast<QAction*>(sender());
    m_CurrentEditor->textItalic(a->isChecked());
}

void EditorActionHandler::textStrike()
{
    if (!m_CurrentEditor)
        return;
    QAction *a = qobject_cast<QAction*>(sender());
    m_CurrentEditor->textStrike(a->isChecked());
}

void EditorActionHandler::fontBigger()
{
    if (m_CurrentEditor)
        m_CurrentEditor->fontBigger();
}

void EditorActionHandler::fontSmaller()
{
    if (m_CurrentEditor)
        m_CurrentEditor->fontSmaller();
}

void EditorActionHandler::textColor()
{
    if (m_CurrentEditor) {
        m_CurrentEditor->textColor();
        /** \todo update color in action */
    }
}

void EditorActionHandler::fontFormat()
{
    if (m_CurrentEditor)
        m_CurrentEditor->fontFormat();
}

void EditorActionHandler::textAlign()
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

void EditorActionHandler::addTable()
{ if (m_CurrentEditor) m_CurrentEditor->addTable (); }
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

