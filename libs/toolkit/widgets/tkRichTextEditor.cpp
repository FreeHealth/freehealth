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
/**
  \class tkRichTextEditor
  \todo Document code.
  \ingroup toolkit
  \ingroup widget_toolkit
*/

#include "tkRichTextEditor.h"
#include "tkRichTextEditor_p.h"

// include toolkit headers
#include <tkGlobal.h>
#include <tkTheme.h>
#include <tkActionManager.h>
#include <tkLog.h>

// include Qt headers
#include <QLocale>
#include <QAction>
#include <QClipboard>
#include <QColorDialog>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QComboBox>
#include <QFontComboBox>
#include <QFontDatabase>
#include <QMenu>
#include <QMenuBar>
#include <QTextCodec>
#include <QTextEdit>
#include <QToolBar>
#include <QTextCursor>
#include <QTextList>
#include <QTextTable>
#include <QTextTableFormat>
#include <QMessageBox>
#include <QTextCharFormat>
#include <QVBoxLayout>
#include <QStringList>
#include <QMessageBox>
#include <QFontDialog>

 #include <QDebug>

// Some constants
const char * const CREATE_TKRICHTEXTMENU = "menuTkRichTextEditor";
const char * const A_TKRICH_FILEOPEN   = "actiontkRichTextEditorOpen";
const char * const A_TKRICH_FILESAVE   = "actiontkRichTextEditorSave";
const char * const M_RICHTEXT          = "menutkRichTextEditor";
const char * const G_RICHTEXT_FILE     = "grouptkRichTextEditor";
const char * const TKRT_FILEOPEN_TEXT  = QT_TRANSLATE_NOOP("tkRichTextEditor", "Open document into text editor");
const char * const TKRT_FILESAVE_TEXT  = QT_TRANSLATE_NOOP("tkRichTextEditor", "Save document of text editor");
const char * const TKRICHTEXTMENU_TEXT = QT_TRANSLATE_NOOP("tkRichTextEditor", "Rich Text Editor");

Q_TK_USING_CONSTANTS

//--------------------------------------------------------------------------------------------------------
//------------------------------------ tkRichTextEditor implementation -----------------------------------
//--------------------------------------------------------------------------------------------------------
tkRichTextEditor::tkRichTextEditor( QWidget *parent, Types type )
          : QWidget( parent ), d( 0 )
{
    setObjectName( "tkRichTextEditor" );
    // instanciate private part
    d = new tkRichTextEditorPrivate( this, type );

    // create QWidget
    QVBoxLayout * vb = new QVBoxLayout( this );
    vb->setObjectName( "tkRichTextEditorLayout" );
    vb->setSpacing( 0 );
    vb->setMargin( 0 );

    vb->addWidget( d->m_ToolBar );
    vb->addWidget( d->textEdit );
}

tkRichTextEditor::~tkRichTextEditor()
{
    if (d) delete d;
    d=0;
}

tkRichTextEditorPrivate::tkRichTextEditorPrivate( QObject *parent , tkRichTextEditor::Types type)
    : QObject(parent), m_Type( type ), actionFileOpen(0), actionFileSave(0), actionAddTable(0),
    actionTableAddRow(0),actionTableAddCol(0), actionTableRemoveRow(0), actionTableRemoveCol(0),
    actionTableMergeCells(0), actionTableSplitCells(0),
    grpAlign(0),
    m_EditMenu(0), m_FormatMenu(0), m_ExtraMenu(0), m_TableMenu(0),
    m_ActionsConnected(false)
{
    setObjectName( "tkRichTextEditorPrivate" );
    // parent is a QWidget
    m_Parent = static_cast<QWidget *>( parent );

    // Create mfWidget
    textEdit = new tkTextEditorPrivate( m_Parent );
    textEdit->setContextMenuPolicy( Qt::CustomContextMenu );

    connect(textEdit, SIGNAL(focusChangedTo(bool)), this, SLOT(editorFocusChangedTo(bool)));

    // Prepare Actions and menus
    m_ToolBar = new QToolBar( m_Parent );
    setupActions();
    setupOptionnalActions();
    setupMenus();
    setupOptionnalMenus();
    setupToolBar();

    m_ToolBar->setEnabled(false);
//    m_ToolBar->setFocusProxy( textEdit );
//    m_Parent->setFocusProxy( textEdit );

    retranslateUi();
    // connection that are not disconnected at lostFocus
    connect( textEdit, SIGNAL( currentCharFormatChanged( const QTextCharFormat & ) ), this, SLOT( currentCharFormatChanged( const QTextCharFormat & ) ) );
    connect( textEdit, SIGNAL( cursorPositionChanged() ), this, SLOT( cursorPositionChanged() ) );
    connect( textEdit, SIGNAL( customContextMenuRequested( const QPoint & ) ), this, SLOT( contextMenu( const QPoint & ) ) );

}

tkRichTextEditorPrivate::~tkRichTextEditorPrivate()
{}

void tkRichTextEditorPrivate::createConnections()
{
    if (m_ToolBar->hasFocus())
        return;
    if (m_ActionsConnected)
        return;

//    textEdit->setFocus();
//    qWarning() << "connect" << m_Parent->objectName();
    fontChanged( textEdit->font() );
    colorChanged( textEdit->textColor() );
    alignmentChanged( textEdit->alignment() );
    actionUndo->setEnabled( textEdit->document()->isUndoAvailable() );
    actionRedo->setEnabled( textEdit->document()->isRedoAvailable() );
    actionCut->setEnabled( false );
    actionCopy->setEnabled( false );

    connect( textEdit->document(), SIGNAL( undoAvailable( bool ) ), actionUndo, SLOT( setEnabled( bool ) ) );
    connect( textEdit->document(), SIGNAL( redoAvailable( bool ) ), actionRedo, SLOT( setEnabled( bool ) ) );

    connect( actionUndo, SIGNAL( triggered() ), textEdit, SLOT( undo() ) );
    connect( actionRedo, SIGNAL( triggered() ), textEdit, SLOT( redo() ) );

    connect( actionCut, SIGNAL( triggered() ), textEdit, SLOT( cut() ) );
    connect( actionCopy, SIGNAL( triggered() ), textEdit, SLOT( copy() ) );
    connect( actionPaste, SIGNAL( triggered() ), textEdit, SLOT( paste() ) );

    connect( textEdit, SIGNAL( copyAvailable( bool ) ), actionCut, SLOT( setEnabled( bool ) ) );
    connect( textEdit, SIGNAL( copyAvailable( bool ) ), actionCopy, SLOT( setEnabled( bool ) ) );

    connect( QApplication::clipboard(), SIGNAL( dataChanged() ), this, SLOT( clipboardDataChanged() ) );

    // connect edit actions
    connect( actionTextBold, SIGNAL( triggered() ), this, SLOT( textBold() ) );
    connect( actionTextItalic, SIGNAL( triggered() ), this, SLOT( textItalic() ) );
    connect( actionTextUnderline, SIGNAL( triggered() ), this, SLOT( textUnderline() ) );
    connect( actionTextStrike, SIGNAL( triggered() ), this, SLOT( textStrike() ) );
    connect( actionFontSmaller, SIGNAL( triggered() ), this, SLOT( fontSmaller() ) );
    connect( actionFontBigger, SIGNAL( triggered() ), this, SLOT( fontBigger() ) );
    connect( actionFontFormat, SIGNAL( triggered() ), this, SLOT( on_actionFontFormat_triggered() ) );

    connect( actionAlignLeft, SIGNAL(triggered()), this, SLOT( textAlign() ) );
    connect( actionAlignRight, SIGNAL(triggered()), this, SLOT( textAlign() ) );
    connect( actionAlignCenter, SIGNAL(triggered()), this, SLOT( textAlign() ) );
    connect( actionAlignJustify, SIGNAL(triggered()), this, SLOT( textAlign() ) );
//    connect( grpAlign, SIGNAL(triggered(QAction*)), this, SLOT( textAlign(QAction *) ) );
    connect( actionTextColor, SIGNAL( triggered() ), this, SLOT( textColor() ) );
    connect( comboStyle, SIGNAL( activated( int ) ), this, SLOT( textStyle( int ) ) );
    connect( comboFont, SIGNAL( activated( const QString & ) ), this, SLOT( textFamily( const QString & ) ) );
    connect( comboSize, SIGNAL( activated( const QString & ) ), this, SLOT( textSize( const QString & ) ) );
    if (actionFileOpen)
        connect( actionFileOpen, SIGNAL( triggered() ), this, SLOT( fileOpen() ) );
    if (actionFileSave)
        connect( actionFileSave, SIGNAL( triggered() ), this, SLOT( saveAs() ) );
    if ( ( m_Type & tkRichTextEditor::Full ) || ( m_Type & tkRichTextEditor::WithTables ) ) {
        connect( actionAddTable, SIGNAL( triggered() ), this, SLOT( addTable() ) );
        connect( actionTableAddRow, SIGNAL( triggered() ), this, SLOT( tableAddRow() ) );
        connect( actionTableAddCol, SIGNAL( triggered() ), this, SLOT( tableAddCol() ) );
        connect( actionTableRemoveRow, SIGNAL( triggered() ), this, SLOT( tableRemoveRow() ) );
        connect( actionTableRemoveCol, SIGNAL( triggered() ), this, SLOT( tableRemoveCol() ) );
        connect( actionTableMergeCells, SIGNAL( triggered() ), this, SLOT( tableMergeCells() ) );
    }
    m_ActionsConnected = true;
}

void tkRichTextEditorPrivate::disconnectAll()
{
    if (!m_ActionsConnected)
        return;
//    qWarning() << "disconnect" << m_Parent->objectName();

    disconnect( textEdit->document(), SIGNAL( undoAvailable( bool ) ), actionUndo, SLOT( setEnabled( bool ) ) );
    disconnect( textEdit->document(), SIGNAL( redoAvailable( bool ) ), actionRedo, SLOT( setEnabled( bool ) ) );
    disconnect( actionUndo, SIGNAL( triggered() ), textEdit, SLOT( undo() ) );
    disconnect( actionRedo, SIGNAL( triggered() ), textEdit, SLOT( redo() ) );
    disconnect( actionCut, SIGNAL( triggered() ), textEdit, SLOT( cut() ) );
    disconnect( actionCopy, SIGNAL( triggered() ), textEdit, SLOT( copy() ) );
    disconnect( actionPaste, SIGNAL( triggered() ), textEdit, SLOT( paste() ) );
    disconnect( textEdit, SIGNAL( copyAvailable( bool ) ), actionCut, SLOT( setEnabled( bool ) ) );
    disconnect( textEdit, SIGNAL( copyAvailable( bool ) ), actionCopy, SLOT( setEnabled( bool ) ) );
    disconnect( QApplication::clipboard(), SIGNAL( dataChanged() ), this, SLOT( clipboardDataChanged() ) );
    disconnect( actionTextBold, SIGNAL( triggered() ), this, SLOT( textBold() ) );
    disconnect( actionTextItalic, SIGNAL( triggered() ), this, SLOT( textItalic() ) );
    disconnect( actionTextUnderline, SIGNAL( triggered() ), this, SLOT( textUnderline() ) );
    disconnect( actionTextStrike, SIGNAL( triggered() ), this, SLOT( textStrike() ) );
    disconnect( actionFontSmaller, SIGNAL( triggered() ), this, SLOT( fontSmaller() ) );
    disconnect( actionFontBigger, SIGNAL( triggered() ), this, SLOT( fontBigger() ) );
    disconnect( actionFontFormat, SIGNAL( triggered() ), this, SLOT( on_actionFontFormat_triggered() ) );
    disconnect( actionAlignLeft, SIGNAL(triggered()), this, SLOT( textAlign() ) );
    disconnect( actionAlignRight, SIGNAL(triggered()), this, SLOT( textAlign() ) );
    disconnect( actionAlignCenter, SIGNAL(triggered()), this, SLOT( textAlign() ) );
    disconnect( actionAlignJustify, SIGNAL(triggered()), this, SLOT( textAlign() ) );
//    disconnect( grpAlign, SIGNAL( triggered( QAction * ) ), this, SLOT( textAlign( QAction * ) ) );
    disconnect( actionTextColor, SIGNAL( triggered() ), this, SLOT( textColor() ) );
    disconnect( comboStyle, SIGNAL( activated( int ) ), this, SLOT( textStyle( int ) ) );
    disconnect( comboFont, SIGNAL( activated( const QString & ) ), this, SLOT( textFamily( const QString & ) ) );
    disconnect( comboSize, SIGNAL( activated( const QString & ) ), this, SLOT( textSize( const QString & ) ) );

    if (actionFileOpen)
        disconnect( actionFileOpen, SIGNAL( triggered() ), this, SLOT( fileOpen() ) );
    if (actionFileSave)
        disconnect( actionFileSave, SIGNAL( triggered() ), this, SLOT( saveAs() ) );

    if ( ( m_Type & tkRichTextEditor::Full ) || ( m_Type & tkRichTextEditor::WithTables ) ) {
        disconnect( actionAddTable, SIGNAL( triggered() ), this, SLOT( addTable() ) );
        disconnect( actionTableAddRow, SIGNAL( triggered() ), this, SLOT( tableAddRow() ) );
        disconnect( actionTableAddCol, SIGNAL( triggered() ), this, SLOT( tableAddCol() ) );
        disconnect( actionTableRemoveRow, SIGNAL( triggered() ), this, SLOT( tableRemoveRow() ) );
        disconnect( actionTableRemoveCol, SIGNAL( triggered() ), this, SLOT( tableRemoveCol() ) );
        disconnect( actionTableMergeCells, SIGNAL( triggered() ), this, SLOT( tableMergeCells() ) );
    }
    m_ActionsConnected = false;
}

void tkRichTextEditorPrivate::editorFocusChangedTo(bool focused)
{
//    qWarning() << "focus" << focused << "undermouse" << m_ToolBar->underMouse();
    if (focused) {
        createConnections();
    } else {
        if (m_ToolBar->underMouse()) {
            textEdit->setFocus();
            return;
        }
        // this part is correcting a bug into Qt
        // if one element of the toolbar is focused abord (this happens when user click on '>>' menu of toolbars)
        foreach( QWidget *w, m_ToolBar->findChildren<QWidget*>() ) {
            if (w->hasFocus() || w->underMouse()) {
                textEdit->setFocus();
                return;
            }
        }
        // end bug correction
        disconnectAll();
    }
//    if (!m_ToolBar->hasFocus())
//        m_ToolBar->setEnabled(focused);
    m_ToolBar->setEnabled(focused);
}

QTextEdit * tkRichTextEditor::textEdit()
{
    return d->textEdit;
}

void tkRichTextEditor::setTypes( Types types )
{
    d->m_Type = types;
    if (d->actionFileOpen)
        d->m_ToolBar->removeAction( d->actionFileOpen );
    if (d->actionFileSave)
        d->m_ToolBar->removeAction( d->actionFileSave );
    if (d->actionAddTable)
        d->m_ToolBar->removeAction( d->actionAddTable );
    d->setupOptionnalActions();
    d->setupOptionnalMenus();
    if (d->actionFileOpen)
        d->m_ToolBar->insertAction( d->actionUndo, d->actionFileOpen );
    if (d->actionFileSave)
        d->m_ToolBar->insertAction( d->actionFileOpen, d->actionFileSave );
    if (d->actionAddTable)
        d->m_ToolBar->insertAction( d->actionAlignLeft, d->actionAddTable );
}

QMenu* tkRichTextEditor::getContextMenu()
{
    if ( !d->m_ContextMenu )
        d->setupMenus();

    if ( ( d->m_Type & tkRichTextEditor::Full ) || ( d->m_Type & tkRichTextEditor::WithTables ) ) {
        d->m_ContextMenu->addMenu( d->m_ExtraMenu );
        if ( d->textEdit->textCursor().currentTable() )
            d->m_ExtraMenu->addMenu( d->m_TableMenu );
    }
    return d->m_ContextMenu;
}

void tkRichTextEditorPrivate::setupOptionnalActions()
{
    tkActionManager *m = tkActionManager::instance();
    if ( m_Type & tkRichTextEditor::WithLoad ) {
        if (m->actionExists(A_TKRICH_FILEOPEN)) {
            actionFileOpen = m->action(A_TKRICH_FILEOPEN);
        } else {
            actionFileOpen = m->createAction(A_TKRICH_FILEOPEN,G_RICHTEXT_FILE,this);
            m->setActionDatas(A_TKRICH_FILEOPEN, TKRT_FILEOPEN_TEXT , TKRT_FILEOPEN_TEXT , ICONOPEN );
            actionFileOpen->setShortcut( QKeySequence::Open );
        }
    } else {
        if (actionFileOpen) {
            m_ToolBar->removeAction(actionFileOpen);
            delete actionFileOpen;
             actionFileOpen = 0;
         }
    }

    if ( m_Type & tkRichTextEditor::WithSave ) {
        if (m->actionExists(A_TKRICH_FILESAVE)) {
            actionFileSave = m->action(A_TKRICH_FILESAVE);
        } else {
            actionFileSave = m->createAction(A_TKRICH_FILESAVE,G_RICHTEXT_FILE,this);
            m->setActionDatas(A_TKRICH_FILESAVE, TKRT_FILESAVE_TEXT , TKRT_FILESAVE_TEXT , ICONSAVE );
            actionFileSave->setShortcut( QKeySequence::Save );
        }
    } else {
        if (actionFileSave) {
            m_ToolBar->removeAction(actionFileSave);
            delete actionFileSave;
             actionFileSave = 0;
         }
    }

    if ( ( m_Type & tkRichTextEditor::Full ) || ( m_Type & tkRichTextEditor::WithTables ) ) {
        actionAddTable = new QAction( this );
        actionAddTable->setIcon( tkTheme::icon( ICONTABLE ) );

        actionTableAddRow = new QAction( this );
        actionTableAddRow->setIcon( tkTheme::icon( ICONTABLEADDROW ) );

        actionTableAddCol = new QAction( this );
        actionTableAddCol->setIcon( tkTheme::icon( ICONTABLEADDCOLUMN ) );

        actionTableRemoveRow = new QAction( this );
        actionTableRemoveRow->setIcon( tkTheme::icon( ICONTABLEREMOVEROW ) );

        actionTableRemoveCol = new QAction( this );
        actionTableRemoveCol->setIcon( tkTheme::icon( ICONTABLEREMOVECOLUMN ) );

        actionTableMergeCells = new QAction( this );
        actionTableMergeCells->setIcon( tkTheme::icon( ICONTABLEMERGECELLS ) );

        actionTableSplitCells = new QAction( this );
        connect( actionTableSplitCells, SIGNAL( triggered() ), this, SLOT( tableSplitCells() ) );
        actionTableSplitCells->setIcon( tkTheme::icon( ICONTABLESPLITCELLS ) );
    } else {
        if (actionAddTable) {
            m_ToolBar->removeAction(actionAddTable);
            delete actionAddTable;
             actionAddTable = 0;
         }
        delete actionTableAddRow;
        actionTableAddRow = 0;
        delete actionTableAddCol;
        actionTableAddCol = 0;
        delete actionTableRemoveRow;
        actionTableRemoveRow = 0;
        delete actionTableRemoveCol;
        actionTableRemoveCol = 0;
        delete actionTableMergeCells;
        actionTableMergeCells = 0;
        delete actionTableSplitCells;
        actionTableSplitCells = 0;
    }
}

void tkRichTextEditorPrivate::setupActions()
{
    tkActionManager *m = tkActionManager::instance();
    if (!m->menuExists( M_EDIT ) ) {
//        tkLog::addMessage( m_Parent, tr("INFO : Object %1 creates menu %2.").arg(m_Parent->objectName()).arg(M_EDIT));
        m->createPopMenu(CREATE_TKRICHTEXTMENU, TKRICHTEXTMENU_TEXT, "", m_Parent);
        m->createDefaultEditMenu(m_Parent, CREATE_TKRICHTEXTMENU);
        m->createDefaultEditMenuActions(m_Parent);
        m->refreshMenu( CREATE_TKRICHTEXTMENU );
    }
    if (!m->menuExists( M_FORMAT ) ) {
//        tkLog::addMessage( m_Parent, tr("INFO : Object %1 creates menu %2.").arg(m_Parent->objectName()).arg(M_FORMAT));
        if (!m->menuExists(CREATE_TKRICHTEXTMENU))
            m->createPopMenu(CREATE_TKRICHTEXTMENU, TKRICHTEXTMENU_TEXT, "", m_Parent);
        m->createDefaultFormatMenu(m_Parent, CREATE_TKRICHTEXTMENU);
        m->createDefaultFormatMenuActions(m_Parent);
        m->refreshMenu( CREATE_TKRICHTEXTMENU );
    }

    // create ActionManager specific structure
    if (!m->menuExists(M_RICHTEXT)) {
        m->createMenu(M_RICHTEXT, G_EDIT_EDITOR, QT_TR_NOOP("TextEditor Commands"), "", m_Parent);
        m->appendGroup(G_RICHTEXT_FILE, M_RICHTEXT);
    }

    // Create the receiving toolbar
    // Go for actions
    actionUndo = m->action(A_EDIT_UNDO);
    actionRedo = m->action(A_EDIT_REDO);
    actionCut = m->action(A_EDIT_CUT);
    actionCopy = m->action(A_EDIT_COPY);
    actionPaste = m->action(A_EDIT_PASTE);
    actionPaste->setEnabled( !QApplication::clipboard()->text().isEmpty() );

    actionTextBold = m->action(A_FORMAT_BOLD);
    actionTextBold->setCheckable( true );
    textEdit->addAction( actionTextBold );

    actionTextItalic = m->action(A_FORMAT_ITALIC);
    textEdit->addAction( actionTextItalic );

    actionTextUnderline = m->action(A_FORMAT_UNDERLINE);
    textEdit->addAction( actionTextUnderline );

    actionTextStrike = m->action(A_FORMAT_STRIKE);
    textEdit->addAction( actionTextStrike );

    actionFontBigger = m->action(A_FORMAT_BIGGER);
    textEdit->addAction( actionFontBigger );
    actionFontSmaller = m->action(A_FORMAT_SMALLER);
    textEdit->addAction( actionTextStrike );

    actionFontFormat = m->action(A_FORMAT_FONT);
    textEdit->addAction( actionFontFormat );

    grpAlign = new QActionGroup( this );
    actionAlignLeft = m->action(A_ALIGN_LEFT);
    actionAlignLeft->setShortcut( Qt::CTRL + Qt::Key_L );
    actionAlignLeft->setShortcutContext( Qt::WidgetShortcut );
    actionAlignLeft->setCheckable( true );
    actionAlignCenter = m->action(A_ALIGN_CENTER);
    actionAlignCenter->setShortcut( Qt::CTRL + Qt::Key_E );
    actionAlignCenter->setShortcutContext( Qt::WidgetShortcut );
    actionAlignCenter->setCheckable( true );
    actionAlignRight = m->action(A_ALIGN_RIGHT);
    actionAlignRight->setShortcut( Qt::CTRL + Qt::Key_R );
    actionAlignRight->setShortcutContext( Qt::WidgetShortcut );
    actionAlignRight->setCheckable( true );
    actionAlignJustify = m->action(A_ALIGN_JUSTIFY);
    actionAlignJustify->setShortcut( Qt::CTRL + Qt::Key_J );
    actionAlignJustify->setShortcutContext( Qt::WidgetShortcut );
    actionAlignJustify->setCheckable( true );
    grpAlign->addAction( actionAlignLeft );
    grpAlign->addAction( actionAlignCenter );
    grpAlign->addAction( actionAlignRight );
    grpAlign->addAction( actionAlignJustify );

    textEdit->addAction( actionAlignLeft );
    textEdit->addAction( actionAlignCenter );
    textEdit->addAction( actionAlignRight );
    textEdit->addAction( actionAlignJustify );

    QPixmap pix( 16, 16 );
    pix.fill( Qt::black );
    actionTextColor = new QAction( this );
    actionTextColor->setIcon( pix ) ;
}

void tkRichTextEditorPrivate::setupOptionnalMenus()
{
    if ( ( m_Type & tkRichTextEditor::Full ) || ( m_Type & tkRichTextEditor::WithTables ) ) {
        m_ExtraMenu = new QMenu( m_ContextMenu );
        m_ExtraMenu->setObjectName("ExtraMenu");
        m_ExtraMenu->addAction( actionAddTable );
        m_TableMenu = new QMenu( m_ExtraMenu );
        m_TableMenu->setObjectName("TableMenu");

        m_TableMenu->addAction( actionTableAddRow );
        m_TableMenu->addAction( actionTableAddCol );
        m_TableMenu->addAction( actionTableRemoveRow );
        m_TableMenu->addAction( actionTableRemoveCol );
        m_TableMenu->addAction( actionTableMergeCells );
        m_TableMenu->addAction( actionTableSplitCells );
    } else {
        if (m_ExtraMenu) delete m_ExtraMenu;
        m_ExtraMenu = 0;
        if (m_TableMenu) delete m_TableMenu;
        m_TableMenu = 0;
    }
}

void tkRichTextEditorPrivate::setupMenus()
{
    tkActionManager *m = tkActionManager::instance();
    m_ContextMenu = new QMenu( textEdit );

    m_EditMenu = m->menu(M_EDIT);
    m_ContextMenu->addMenu( m_EditMenu );

    m_FormatMenu = m->menu(M_FORMAT);
    m_ContextMenu->addMenu( m_FormatMenu );
}

void tkRichTextEditorPrivate::setupToolBar()
{
    m_ToolBar->setIconSize( QSize( 16, 16 ) );
    if (actionFileOpen)  m_ToolBar->addAction( actionFileOpen );
    if (actionFileSave)  m_ToolBar->addAction( actionFileSave );
    m_ToolBar->addAction( actionUndo );
    m_ToolBar->addAction( actionRedo );
    m_ToolBar->addAction( actionCut );
    m_ToolBar->addAction( actionCopy );
    m_ToolBar->addAction( actionPaste );
    m_ToolBar->addAction( actionTextBold );
    m_ToolBar->addAction( actionTextItalic );
    m_ToolBar->addAction( actionTextUnderline );
    m_ToolBar->addAction( actionTextStrike );
    m_ToolBar->addAction( actionFontBigger );
    m_ToolBar->addAction( actionFontSmaller );
    m_ToolBar->addAction( actionFontFormat );
    if (actionAddTable)  m_ToolBar->addAction( actionAddTable );
    m_ToolBar->addAction( actionAlignLeft );
    m_ToolBar->addAction( actionAlignCenter );
    m_ToolBar->addAction( actionAlignRight );
    m_ToolBar->addAction( actionAlignJustify );
    m_ToolBar->addAction( actionTextColor );

    comboStyle = new QComboBox( m_Parent );
    comboStyle->setObjectName( "comboStyle" );

    comboFont = new QFontComboBox( m_Parent );
    comboFont->setObjectName( "comboFont" );

    comboSize = new QComboBox( m_Parent );
    comboSize->setObjectName( "comboSize" );
    comboSize->setEditable( true );

    QFontDatabase db;
    foreach( int size, db.standardSizes() )
        comboSize->addItem( QString::number( size ) );
    comboSize->setCurrentIndex( comboSize->findText( QString::number( QApplication::font().pointSize() ) ) );

    m_ToolBar->addWidget( comboStyle );
    m_ToolBar->addWidget( comboFont );
    m_ToolBar->addWidget( comboSize );
}

void tkRichTextEditorPrivate::contextMenu( const QPoint & p )
{
    QMenu *m = static_cast<tkRichTextEditor *>( parent() )->getContextMenu();
    m->popup( textEdit->mapToGlobal( p ) );
    m->exec();
    // menu must not be deleted
}

//--------------------------------------------------------------------------------------------------------
//------------------------------------------------- File Open / Save -------------------------------------
//--------------------------------------------------------------------------------------------------------
void tkRichTextEditorPrivate::fileOpen()
{
    tkActionManager *m = tkActionManager::instance();
    QString file = QFileDialog::getOpenFileName( m_Parent, m->action(A_TKRICH_FILEOPEN)->text(),
                                               QString(), tr( "HTML files (*.htm *.html);;Text files (*.txt);;All Files (*)" ) );
    if ( file.isEmpty() )
        return;
    QString str = tkGlobal::readTextFile(file, tkGlobal::WarnUser, m_Parent);
    if ( Qt::mightBeRichText( str ) ) {
        textEdit->setHtml( str );
    } else {
        textEdit->setPlainText( str );
    }
}

void tkRichTextEditorPrivate::saveAs()
{
    tkActionManager *m = tkActionManager::instance();
    QString fileName = QFileDialog::getSaveFileName( m_Parent, m->action(A_TKRICH_FILESAVE)->text(),
                                                     QString(), tr( "HTML-Files (*.htm *.html);;All Files (*)" ) );
    if ( fileName.isEmpty() )
        return ;
    if (tkGlobal::saveStringToFile( tkGlobal::toHtmlAccent(textEdit->document()->toHtml( "UTF-8" )),
                                    fileName, tkGlobal::WarnUser, m_Parent ))
        textEdit->document()->setModified( false );
}

//--------------------------------------------------------------------------------------------------------
//--------------------------------------------- Text Decoration ------------------------------------------
//--------------------------------------------------------------------------------------------------------
void tkRichTextEditorPrivate::on_actionFontFormat_triggered()
{
    // 1. get actual font under cursor
    QFont font = textEdit->textCursor().charFormat().font();
    // 2. show font dialog
    bool ok;
    QFont selectedfont = QFontDialog::getFont( &ok, font, m_Parent, tr("Font format"), QFontDialog::DontUseNativeDialog );
    // 3. apply, if accepted, font style
    if (ok) {
        QTextCharFormat fmt;
        fmt.setFont( selectedfont );
        mergeFormatOnWordOrSelection( fmt );
    }
}

void tkRichTextEditorPrivate::fontBigger()
{
    QFont font = textEdit->textCursor().charFormat().font();
    QTextCharFormat fmt;
    font.setPointSize( font.pointSize() + 1 );
    fmt.setFont( font);
    mergeFormatOnWordOrSelection( fmt );
}

void tkRichTextEditorPrivate::fontSmaller()
{
    QFont font = textEdit->textCursor().charFormat().font();
    QTextCharFormat fmt;
    font.setPointSize( font.pointSize() - 1 );
    fmt.setFont( font);
    mergeFormatOnWordOrSelection( fmt );
}

void tkRichTextEditorPrivate::textBold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight( actionTextBold->isChecked() ? QFont::Bold : QFont::Normal );
    mergeFormatOnWordOrSelection( fmt );
}

void tkRichTextEditorPrivate::textUnderline()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline( actionTextUnderline->isChecked() );
    mergeFormatOnWordOrSelection( fmt );
}

void tkRichTextEditorPrivate::textItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic( actionTextItalic->isChecked() );
    mergeFormatOnWordOrSelection( fmt );
}

void tkRichTextEditorPrivate::textStrike()
{
    QTextCharFormat fmt;
    fmt.setFontStrikeOut( actionTextStrike->isChecked() );
    mergeFormatOnWordOrSelection( fmt );
}

void tkRichTextEditorPrivate::textFamily( const QString &f )
{
    QTextCharFormat fmt;
    fmt.setFontFamily( f );
    mergeFormatOnWordOrSelection( fmt );
}

void tkRichTextEditorPrivate::textSize( const QString &p )
{
    QTextCharFormat fmt;
    fmt.setFontPointSize( p.toFloat() );
    mergeFormatOnWordOrSelection( fmt );
}

void tkRichTextEditorPrivate::textStyle( int styleIndex )
{
    QTextCursor cursor = textEdit->textCursor();

    if ( styleIndex != 0 )  {
        QTextListFormat::Style style = QTextListFormat::ListDisc;
        switch ( styleIndex )
        {
               default:
               case 1:
            style = QTextListFormat::ListDisc;
            break;
               case 2:
            style = QTextListFormat::ListCircle;
            break;
               case 3:
            style = QTextListFormat::ListSquare;
            break;
               case 4:
            style = QTextListFormat::ListDecimal;
            break;
               case 5:
            style = QTextListFormat::ListLowerAlpha;
            break;
               case 6:
            style = QTextListFormat::ListUpperAlpha;
            break;
        }

        cursor.beginEditBlock();

        QTextBlockFormat blockFmt = cursor.blockFormat();

        QTextListFormat listFmt;

        if ( cursor.currentList() ) {
            listFmt = cursor.currentList()->format();
        } else {
            listFmt.setIndent( blockFmt.indent() + 1 );
            blockFmt.setIndent( 0 );
            cursor.setBlockFormat( blockFmt );
        }
        listFmt.setStyle( style );
        cursor.createList( listFmt );
        cursor.endEditBlock();
    } else {
        // ####
        QTextBlockFormat bfmt;
        bfmt.setObjectIndex( -1 );
        cursor.mergeBlockFormat( bfmt );
    }
}

void tkRichTextEditorPrivate::textColor()
{
    QColor col = QColorDialog::getColor( textEdit->textColor(), m_Parent );
    if ( !col.isValid() )
        return;
    QTextCharFormat fmt;
    fmt.setForeground( col );
    mergeFormatOnWordOrSelection( fmt );
    colorChanged( col );
}

void tkRichTextEditorPrivate::textAlign()
{
    QAction *a = qobject_cast<QAction*>(sender());
    if ( a == actionAlignLeft )
        textEdit->setAlignment( Qt::AlignLeft );
    else if ( a == actionAlignCenter )
        textEdit->setAlignment( Qt::AlignHCenter );
    else if ( a == actionAlignRight )
        textEdit->setAlignment( Qt::AlignRight );
    else if ( a == actionAlignJustify )
        textEdit->setAlignment( Qt::AlignJustify );
}

void tkRichTextEditorPrivate::currentCharFormatChanged( const QTextCharFormat &format )
{
    fontChanged( format.font() );
    colorChanged( format.foreground().color() );
}

void tkRichTextEditorPrivate::cursorPositionChanged()
{
    alignmentChanged( textEdit->alignment() );
}

void tkRichTextEditorPrivate::clipboardDataChanged()
{
    actionPaste->setEnabled( !QApplication::clipboard()->text().isEmpty() );
}

void tkRichTextEditorPrivate::about()
{
    QMessageBox::about( m_Parent, tr( "About" ),
                        tr( "This rich text editor is part of FreeMedForms Project. " ) );
}

void tkRichTextEditorPrivate::mergeFormatOnWordOrSelection( const QTextCharFormat &format )
{
    QTextCursor cursor = textEdit->textCursor();
    if ( !cursor.hasSelection() )
        cursor.select( QTextCursor::WordUnderCursor );
    cursor.mergeCharFormat( format );
    textEdit->mergeCurrentCharFormat( format );
}

void tkRichTextEditorPrivate::fontChanged( const QFont &f )
{
    comboFont->setCurrentIndex( comboFont->findText( QFontInfo( f ).family() ) );
    comboSize->setCurrentIndex( comboSize->findText( QString::number( f.pointSize() ) ) );
    actionTextBold->setChecked( f.bold() );
    actionTextItalic->setChecked( f.italic() );
    actionTextUnderline->setChecked( f.underline() );
}

void tkRichTextEditorPrivate::colorChanged( const QColor &c )
{
    QPixmap pix( 16, 16 );
    pix.fill( c );
    actionTextColor->setIcon( pix );
}

void tkRichTextEditorPrivate::alignmentChanged( Qt::Alignment a )
{
    if ( a & Qt::AlignLeft )
        actionAlignLeft->setChecked( true );
    else if ( a & Qt::AlignHCenter )
        actionAlignCenter->setChecked( true );
    else if ( a & Qt::AlignRight )
        actionAlignRight->setChecked( true );
    else if ( a & Qt::AlignJustify )
        actionAlignJustify->setChecked( true );
}

//--------------------------------------------------------------------------------------------------------
//------------------------------------------- Managing Tables --------------------------------------------
//--------------------------------------------------------------------------------------------------------
void tkRichTextEditorPrivate::addTable()
{
    tkTableDialog dialog( m_Parent );
    if ( dialog.exec() == QDialog::Rejected ) return;

    QTextCursor cursor( textEdit->textCursor() );

    QTextTable *table = cursor.insertTable( dialog.rows(), dialog.cols(), dialog.format() );

    // Set Font Header to bold
    if ( dialog.format().headerRowCount() ) {
        int i = 0;
        for ( i = 0; i < dialog.cols() ; ++i ) {
            QTextCharFormat txtformat = table->cellAt( 0, i ).format();
            txtformat.setFontWeight( QFont::Bold );
            txtformat.setFontItalic( true );
            table->cellAt( 0, i ).setFormat( txtformat );
            textEdit->setTextCursor( table->cellAt( 0, i ).firstCursorPosition() ) ;
            textEdit->setAlignment( Qt::AlignCenter );
        }
    }
    textEdit->setTextCursor( cursor );
}

void tkRichTextEditorPrivate::tableAddRow()
{
    QTextTable * table = textEdit->textCursor().currentTable();
    if ( !table ) return;
    const QTextTableCell & cell = table->cellAt( textEdit->textCursor() );
    table->insertRows( cell.row() + 1, 1 );
}

void tkRichTextEditorPrivate::tableAddCol()
{
    QTextTable * table = textEdit->textCursor().currentTable();
    if ( !table ) return;
    const QTextTableCell & cell = table->cellAt( textEdit->textCursor() );
    table->insertColumns( cell.column() + 1, 1 );
    // Resize columns
    QTextTableFormat format = table->format();
    int i = 0;
    QVector<QTextLength> lengths;
    for ( i = 0; i < table->columns(); i++ ) {
        lengths << QTextLength( QTextLength::PercentageLength, 100 / table->columns() );
    }
    format.setColumnWidthConstraints( lengths );
    table->setFormat( format );
}

void tkRichTextEditorPrivate::tableRemoveRow()
{
    QTextCursor cursor = textEdit->textCursor();
    QTextTable * table = cursor.currentTable();
    if ( !table ) return;
    int col = 0;
    int row = 0;
    int nrows, ncols;
    if ( cursor.hasSelection() ) {
        cursor.selectedTableCells( &row, &nrows, &col, &ncols );
        if ( nrows == 0 ) nrows++;
    } else {
        QTextTableCell cell = table->cellAt( cursor );
        col = cell.column();
        row = cell.row();
        nrows = 1;
    }
    table->removeRows( row , nrows );
}

void tkRichTextEditorPrivate::tableRemoveCol()
{
    QTextCursor cursor = textEdit->textCursor();
    QTextTable * table = cursor.currentTable();
    int numberOfCols = table->columns();
    if ( !table ) return;
    int col = 0;
    int row = 0;
    int nrows, ncols;
    if ( cursor.hasSelection() ) {
        cursor.selectedTableCells( &row, &nrows, &col, &ncols );
        if ( nrows == 0 ) nrows++;
    } else {
        QTextTableCell cell = table->cellAt( cursor );
        col = cell.column();
        row = cell.row();
        ncols = 1;
    }
    table->removeColumns( col , ncols );

    if ( (numberOfCols - ncols ) <= 0 )
        return;

    // Resize columns
    QTextTableFormat format = table->format();
    int i = 0;
    QVector<QTextLength> lengths;
    for ( i = 0; i < table->columns(); i++ ) {
        lengths << QTextLength( QTextLength::PercentageLength, 100 / table->columns() );
    }
    format.setColumnWidthConstraints( lengths );
    table->setFormat( format );
}

void tkRichTextEditorPrivate::tableMergeCells()
{
    if ( !textEdit->textCursor().hasSelection() ) return;
    QTextTable * table = textEdit->textCursor().currentTable();
    if ( !table ) return;
    int col = 0;
    int row = 0;
    int nrows, ncols;
    textEdit->textCursor().selectedTableCells( &row, &nrows, &col, &ncols );
    if ( ( nrows == 0 ) && ( ncols == 0 ) ) return;
    table->mergeCells( textEdit->textCursor() );

    QTextTableCell cell = table->cellAt( row, col );
    textEdit->setTextCursor( cell.firstCursorPosition() );
}

void tkRichTextEditorPrivate::tableSplitCells()
{
    // TODO : tableSplitCells
    QTextTable * table = textEdit->textCursor().currentTable();
    if ( !table ) return;
    QTextTableCell cell = table->cellAt( textEdit->textCursor() );

    //      table->splitCell( cell.row(), cell.column(), 2, 2 );
}

//--------------------------------------------------------------------------------------------------------
//---------------------------------------------- Retranslate UI ------------------------------------------
//--------------------------------------------------------------------------------------------------------
void tkRichTextEditor::changeEvent(QEvent *e)
{
    switch ( e->type() )
    {
    case QEvent::LanguageChange:
        d->retranslateUi();
        break;
    default:
        break;
    }
}

void tkRichTextEditorPrivate::retranslateUi()
{
    // Manage optionnals actions / menus
    if ( actionAddTable ) actionAddTable->setText( tr( "&Table" ) );
    actionTextColor->setText( tr( "&Color..." ) );
    comboStyle->clear();
    comboStyle->addItem( tr( "Standard" ) );
    comboStyle->addItem( tr( "Bullet List (Disc)" ) );
    comboStyle->addItem( tr( "Bullet List (Circle)" ) );
    comboStyle->addItem( tr( "Bullet List (Square)" ) );
    comboStyle->addItem( tr( "Ordered List (Decimal)" ) );
    comboStyle->addItem( tr( "Ordered List (Alpha lower)" ) );
    comboStyle->addItem( tr( "Ordered List (Alpha upper)" ) );

    m_ContextMenu->setTitle( tr( "Context menu" ) );
//    m_EditMenu->setTitle( tr ( "&Edition" ) );
//    m_FormatMenu->setTitle( tr( "&Format" ) );

    if ( m_ExtraMenu )
        m_ExtraMenu->setTitle( tr( "E&xtras" ) );

    if ( m_TableMenu ) {
        m_TableMenu->setTitle( tr( "&Table" ) );
        actionAddTable->setText( tr( "Add a table" ) );
        actionTableAddRow->setText( tr( "Add a row" ) );
        actionTableAddCol->setText( tr( "Add a column" ) );
        actionTableRemoveRow->setText( tr( "Remove row(s)" ) );
        actionTableRemoveCol->setText( tr( "Remove col(s)" ) );
        actionTableMergeCells->setText( tr( "Merge selected cells" ) );
        actionTableSplitCells->setText( tr( "Split cell" ) );
    }
}

tkTableDialog::tkTableDialog(QWidget *parent) :
        QDialog( parent )
{
    setupUi( this );
    setWindowTitle( qApp->applicationName() + " - Rich Text Widget" );
    setWindowIcon( tkTheme::icon( ICONTABLE ) );
}
