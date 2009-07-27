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
#include "tkRichTextEditorManager.h"
#include "tkRichTextEditorContext.h"

// include toolkit headers
#include <tkGlobal.h>
#include <tkTheme.h>
#include <tkLog.h>
#include <tkConstantTranslations.h>
#include <tkContextManager.h>
#include <tkActionManager.h>

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
#include <QTextList>
#include <QTextCursor>
#include <QMessageBox>
#include <QTextCharFormat>
#include <QVBoxLayout>
#include <QStringList>
#include <QFontDialog>

Q_TK_USING_CONSTANTS
Q_TK_USING_TRANSLATIONS


class tkRichTextEditorPrivate
{
public:
    tkRichTextEditorPrivate( QWidget *parent, tkRichTextEditor::Types type );
    ~tkRichTextEditorPrivate();

    void retranslateUi();

    void createToolBar()
    {
        m_ToolBar = new QToolBar(m_Parent);
        m_ToolBar->setIconSize(QSize(16, 16));
        populateToolbar();
    }

    void populateToolbar()
    {
        tkActionManager *am = tkActionManager::instance();

        // IO Actions
        if (m_Type & tkRichTextEditor::WithIO) {
            tkCommand *cmd = am->command(tkConstants::A_EDITOR_FILEOPEN);
            m_ToolBar->addAction(cmd->action());
            cmd = am->command(tkConstants::A_EDITOR_FILESAVE);
            m_ToolBar->addAction(cmd->action());
            m_ToolBar->addSeparator();
        }

        tkActionContainer *ac = am->actionContainer(tkConstants::M_EDIT);
        if (ac) {
            foreach (QAction *action, ac->menu()->actions())
                m_ToolBar->addAction(action);
            m_ToolBar->addSeparator();
        }
        ac = am->actionContainer(tkConstants::M_FORMAT_FONT);
        if (ac) {
            foreach (QAction *action, ac->menu()->actions())
                m_ToolBar->addAction(action);
            m_ToolBar->addSeparator();
        }
        ac = am->actionContainer(tkConstants::M_FORMAT_PARAGRAPH);
        if (ac) {
            foreach (QAction *action, ac->menu()->actions())
                m_ToolBar->addAction(action);
            m_ToolBar->addSeparator();
        }
        ac = am->actionContainer(tkConstants::M_FORMAT_TABLE);
        if (ac) {
            foreach (QAction *action, ac->menu()->actions()) {
                m_ToolBar->addAction(action);
            }
        }
    }


    void mergeFormatOnWordOrSelection( const QTextCharFormat &format )
    {
        QTextCursor cursor = textEdit->textCursor();
        if ( !cursor.hasSelection() )
            cursor.select( QTextCursor::WordUnderCursor );
        cursor.mergeCharFormat( format );
        textEdit->mergeCurrentCharFormat( format );
    }           
    
public:
    tkRichTextEditor::Types m_Type;
    tkRichTextEditorContext *m_Context;
    QToolBar *m_ToolBar;
    QTextEdit *textEdit;
    QWidget *m_Parent;
    bool m_ToolBarIsVisible;
};




//--------------------------------------------------------------------------------------------------------
//------------------------------------ tkRichTextEditor implementation -----------------------------------
//--------------------------------------------------------------------------------------------------------
tkRichTextEditor::tkRichTextEditor( QWidget *parent, Types type )
          : tkRichTextTableEditor(parent), d(0)
{
    static int handler = 0;
    handler++;
    setObjectName( "tkRichTextEditor_" + QString::number(handler) );

    // instanciate private part
    d = new tkRichTextEditorPrivate(this, type);

    // instanciate context for actions
    d->m_Context = new tkRichTextEditorContext(this);
    setTypes(type);
    tkContextManager::instance()->addContextObject(d->m_Context);

    // instanciate editor manager
    tkRichTextEditorManager::instance();

    // create ToolBar
    d->createToolBar();
    toogleToolbar(false);

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
    // unreg context
    tkContextManager::instance()->removeContextObject(d->m_Context);
    if (d) delete d;
    d=0;
}

tkRichTextEditorPrivate::tkRichTextEditorPrivate(QWidget *parent , tkRichTextEditor::Types type)
    : m_Type(type), m_Context(0), textEdit(0), m_Parent(parent), m_ToolBarIsVisible(false)
{
    // Create mfWidget
    textEdit = new QTextEdit(m_Parent);
    textEdit->setContextMenuPolicy( Qt::CustomContextMenu );
    retranslateUi();
}

tkRichTextEditorPrivate::~tkRichTextEditorPrivate()
{
}

QTextEdit * tkRichTextEditor::textEdit() const
{
    return d->textEdit;
}

void tkRichTextEditor::setTypes(Types type)
{
    d->m_Context->clearContext();
    d->m_Context->addContext(tkUID->uniqueIdentifier(tkConstants::C_EDITOR_BASIC));
    if (type & tkRichTextEditor::WithTables) {
        d->m_Context->addContext(tkUID->uniqueIdentifier(tkConstants::C_EDITOR_TABLE));
    }
    if (type & tkRichTextEditor::WithIO) {
        d->m_Context->addContext(tkUID->uniqueIdentifier(tkConstants::C_EDITOR_IO));
    }
}

QMenu* tkRichTextEditor::getContextMenu()
{
    QMenu *mc = new QMenu(this);
    tkActionManager *am = tkActionManager::instance();
    mc->setTitle(tkTr(tkConstants::EDITORMENU_TEXT));

    tkActionContainer *cMenu = am->actionContainer(tkConstants::M_EDITOR_CONTEXT);
    if (cMenu) {
        foreach (QAction *action, cMenu->menu()->actions())
            mc->addAction(action);
    }
    if (d->m_Type & WithIO) {
        QMenu *m = new QMenu(this);
        m->setTitle(tkTr(tkConstants::M_FILE_TEXT));
        tkCommand *cmd = am->command(tkConstants::A_EDITOR_FILEOPEN);
        m->addAction(cmd->action());
        cmd = am->command(tkConstants::A_EDITOR_FILESAVE);
        m->addAction(cmd->action());
        mc->addMenu(m);
    }
    cMenu = am->actionContainer(tkConstants::M_EDIT);
    if (cMenu) {
        QMenu *m = new QMenu(cMenu->menu()->title(), this);
        foreach (QAction *action, cMenu->menu()->actions())
            m->addAction(action);
        mc->addMenu(m);
    }
    cMenu = am->actionContainer(tkConstants::M_FORMAT_FONT);
    if (cMenu) {
        QMenu *m = new QMenu(cMenu->menu()->title(), this);
        foreach (QAction *action, cMenu->menu()->actions())
            m->addAction(action);
        mc->addMenu(m);
    }
    cMenu = am->actionContainer(tkConstants::M_FORMAT_PARAGRAPH);
    if (cMenu) {
        QMenu *m = new QMenu(cMenu->menu()->title(), this);
        foreach (QAction *action, cMenu->menu()->actions())
            m->addAction(action);
        mc->addMenu(m);
    }
    cMenu = am->actionContainer(tkConstants::M_FORMAT_TABLE);
    if (cMenu) {
        QMenu *m = new QMenu(cMenu->menu()->title(), this);
        int i = 0;
        foreach (QAction *action, cMenu->menu()->actions()) {
            if (i==0)
                m->addAction(action);
            ++i;
            if (d->textEdit->textCursor().currentTable())
                m->addAction(action);
        }
        mc->addMenu(m);
    }
    return mc;
}

void tkRichTextEditor::contextMenu( const QPoint &pos )
{
    QMenu * p = getContextMenu();
    p->exec(this->mapToGlobal(pos));
}

void tkRichTextEditor::toogleToolbar(bool state)
{
    if (d->m_ToolBar) {
        d->m_ToolBar->setVisible(state);
        d->m_ToolBarIsVisible = state;
    }
}

bool tkRichTextEditor::toolbarIsVisible() const
{
    return d->m_ToolBarIsVisible;
}

void tkRichTextEditor::hideToolbar()
{
    d->m_ToolBar->setVisible(false);
}

//--------------------------------------------------------------------------------------------------------
//------------------------------------------------- File Open / Save -------------------------------------
//--------------------------------------------------------------------------------------------------------
void tkRichTextEditor::fileOpen()
{
    QAction *a = qobject_cast<QAction *>(sender());
    QString title;
    if (!a)
        title = tkTr(EDITORMENU_TEXT);
    else
        title = a->text();
    QString file = QFileDialog::getOpenFileName( this, title,
                                               QString(), tr( "HTML files (*.htm *.html);;Text files (*.txt);;All Files (*)" ) );
    if ( file.isEmpty() )
        return;
    QString str = tkGlobal::readTextFile(file, tkGlobal::WarnUser, this);
    if ( Qt::mightBeRichText( str ) ) {
        textEdit()->setHtml( str );
    } else {
        textEdit()->setPlainText( str );
    }
}

void tkRichTextEditor::saveAs()
{
    QAction *a = qobject_cast<QAction *>(sender());
    QString title;
    if (!a)
        title = tkTr(EDITORMENU_TEXT);
    else
        title = a->text();
    QString fileName = QFileDialog::getSaveFileName( this, title,
                                                     QString(), tr( "HTML-Files (*.htm *.html);;All Files (*)" ) );
    if ( fileName.isEmpty() )
        return ;
    if (tkGlobal::saveStringToFile( tkGlobal::toHtmlAccent(textEdit()->document()->toHtml("UTF-8")),
                                    fileName, tkGlobal::WarnUser, this ))
        textEdit()->document()->setModified( false );
}

//--------------------------------------------------------------------------------------------------------
//--------------------------------------------- Text Decoration ------------------------------------------
//--------------------------------------------------------------------------------------------------------
void tkRichTextEditor::fontFormat()
{
    // 1. get actual font under cursor
    QFont font = textEdit()->textCursor().charFormat().font();
    // 2. show font dialog
    bool ok;
    QFont selectedfont = QFontDialog::getFont( &ok, font, this, tr("Font format"), QFontDialog::DontUseNativeDialog );
    // 3. apply, if accepted, font style
    if (ok) {
        QTextCharFormat fmt;
        fmt.setFont( selectedfont );
        d->mergeFormatOnWordOrSelection( fmt );
    }
}

void tkRichTextEditor::fontBigger()
{
    QFont font = textEdit()->textCursor().charFormat().font();
    QTextCharFormat fmt;
    font.setPointSize( font.pointSize() + 1 );
    fmt.setFont( font);
    d->mergeFormatOnWordOrSelection( fmt );
}

void tkRichTextEditor::fontSmaller()
{
    QFont font = textEdit()->textCursor().charFormat().font();
    QTextCharFormat fmt;
    font.setPointSize( font.pointSize() - 1 );
    fmt.setFont( font);
    d->mergeFormatOnWordOrSelection( fmt );
}

void tkRichTextEditor::textBold( bool checked )
{
    QTextCharFormat fmt;
    fmt.setFontWeight( checked ? QFont::Bold : QFont::Normal );
    d->mergeFormatOnWordOrSelection( fmt );
}

void tkRichTextEditor::textUnderline( bool checked )
{
    QTextCharFormat fmt;
    fmt.setFontUnderline( checked );
    d->mergeFormatOnWordOrSelection( fmt );
}

void tkRichTextEditor::textItalic( bool checked )
{
    QTextCharFormat fmt;
    fmt.setFontItalic( checked );
    d->mergeFormatOnWordOrSelection( fmt );
}

void tkRichTextEditor::textStrike( bool checked )
{
    QTextCharFormat fmt;
    fmt.setFontStrikeOut( checked );
    d->mergeFormatOnWordOrSelection( fmt );
}

//void tkRichTextEditor::textFamily( const QString &f )
//{
//    QTextCharFormat fmt;
//    fmt.setFontFamily( f );
//    mergeFormatOnWordOrSelection( fmt );
//}
//
//void tkRichTextEditor::textSize( const QString &p )
//{
//    QTextCharFormat fmt;
//    fmt.setFontPointSize( p.toFloat() );
//    mergeFormatOnWordOrSelection( fmt );
//}
//
//void tkRichTextEditor::textStyle( int styleIndex )
//{
//    QTextCursor cursor = textEdit->textCursor();
//
//    if ( styleIndex != 0 )  {
//        QTextListFormat::Style style = QTextListFormat::ListDisc;
//        switch ( styleIndex )
//        {
//               default:
//               case 1:
//            style = QTextListFormat::ListDisc;
//            break;
//               case 2:
//            style = QTextListFormat::ListCircle;
//            break;
//               case 3:
//            style = QTextListFormat::ListSquare;
//            break;
//               case 4:
//            style = QTextListFormat::ListDecimal;
//            break;
//               case 5:
//            style = QTextListFormat::ListLowerAlpha;
//            break;
//               case 6:
//            style = QTextListFormat::ListUpperAlpha;
//            break;
//        }
//
//        cursor.beginEditBlock();
//
//        QTextBlockFormat blockFmt = cursor.blockFormat();
//
//        QTextListFormat listFmt;
//
//        if ( cursor.currentList() ) {
//            listFmt = cursor.currentList()->format();
//        } else {
//            listFmt.setIndent( blockFmt.indent() + 1 );
//            blockFmt.setIndent( 0 );
//            cursor.setBlockFormat( blockFmt );
//        }
//        listFmt.setStyle( style );
//        cursor.createList( listFmt );
//        cursor.endEditBlock();
//    } else {
//        // ####
//        QTextBlockFormat bfmt;
//        bfmt.setObjectIndex( -1 );
//        cursor.mergeBlockFormat( bfmt );
//    }
//}

void tkRichTextEditor::textColor()
{
    QColor col = QColorDialog::getColor( textEdit()->textColor(), this );
    if ( !col.isValid() )
        return;
    QTextCharFormat fmt;
    fmt.setForeground( col );
    d->mergeFormatOnWordOrSelection( fmt );
}

//void tkRichTextEditorPrivate::about()
//{
//    QMessageBox::about( m_Parent, tkTr(ABOUT_TEXT),
//                        tr( "This rich text editor is part of FreeMedForms Project. " ) );
//}


//void tkRichTextEditorPrivate::colorChanged( const QColor &c )
//{
//    QPixmap pix( 16, 16 );
//    pix.fill( c );
////    actionTextColor->setIcon( pix );
//}


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
//    // Manage optionnals actions / menus
//    if ( actionAddTable ) actionAddTable->setText( tr( "&Table" ) );
//    actionTextColor->setText( tr( "&Color..." ) );
//    comboStyle->clear();
//    comboStyle->addItem( tr( "Standard" ) );
//    comboStyle->addItem( tr( "Bullet List (Disc)" ) );
//    comboStyle->addItem( tr( "Bullet List (Circle)" ) );
//    comboStyle->addItem( tr( "Bullet List (Square)" ) );
//    comboStyle->addItem( tr( "Ordered List (Decimal)" ) );
//    comboStyle->addItem( tr( "Ordered List (Alpha lower)" ) );
//    comboStyle->addItem( tr( "Ordered List (Alpha upper)" ) );
//
//    m_ContextMenu->setTitle( tr( "Context menu" ) );
////    m_EditMenu->setTitle( tr ( "&Edition" ) );
////    m_FormatMenu->setTitle( tr( "&Format" ) );
//
//    if ( m_ExtraMenu )
//        m_ExtraMenu->setTitle( tr( "E&xtras" ) );
//
//    if ( m_TableMenu ) {
//        m_TableMenu->setTitle( tr( "&Table" ) );
//        actionAddTable->setText( tr( "Add a table" ) );
//        actionTableAddRow->setText( tr( "Add a row" ) );
//        actionTableAddCol->setText( tr( "Add a column" ) );
//        actionTableRemoveRow->setText( tr( "Remove row(s)" ) );
//        actionTableRemoveCol->setText( tr( "Remove col(s)" ) );
//        actionTableMergeCells->setText( tr( "Merge selected cells" ) );
//        actionTableSplitCells->setText( tr( "Split cell" ) );
//    }
}

