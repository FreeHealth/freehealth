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
  \class TextEditor
  \todo Document code.
  \ingroup toolkit
  \ingroup widget_toolkit
*/

#include "texteditor.h"
#include "editormanager.h"
#include "editorcontext.h"

#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/global.h>
#include <coreplugin/uniqueidmanager.h>

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

using namespace Editor;
using namespace Editor::Internal;
using namespace Trans::ConstantTranslations;

namespace Editor {
namespace Internal {
class TextEditorPrivate
{
public:
    TextEditorPrivate(QWidget *parent , TextEditor::Types type)
            : m_Type(type), m_Context(0), textEdit(0), m_Parent(parent), m_ToolBarIsVisible(false)
    {
        textEdit = new QTextEdit(m_Parent);
        textEdit->setContextMenuPolicy( Qt::CustomContextMenu );
        retranslateUi();
    }

    ~TextEditorPrivate()
    {
    }

    void retranslateUi();

    void createToolBar()
    {
        m_ToolBar = new QToolBar(m_Parent);
        m_ToolBar->setIconSize(QSize(16, 16));
        m_ToolBar->setFocusPolicy(Qt::ClickFocus);
        populateToolbar();
    }

    void populateToolbar()
    {
        Core::ActionManager *am = Core::ICore::instance()->actionManager();

        // IO Actions
        if (m_Type & TextEditor::WithIO) {
            Core::Command *cmd = am->command(Core::Constants::A_EDITOR_FILEOPEN);
            m_ToolBar->addAction(cmd->action());
            cmd = am->command(Core::Constants::A_EDITOR_FILESAVE);
            m_ToolBar->addAction(cmd->action());
            m_ToolBar->addSeparator();
        }

        Core::ActionContainer *ac = am->actionContainer(Core::Constants::M_EDIT);
        if (ac) {
            foreach (QAction *action, ac->menu()->actions())
                m_ToolBar->addAction(action);
            m_ToolBar->addSeparator();
        }
        ac = am->actionContainer(Core::Constants::M_FORMAT_FONT);
        if (ac) {
            foreach (QAction *action, ac->menu()->actions())
                m_ToolBar->addAction(action);
            m_ToolBar->addSeparator();
        }
        ac = am->actionContainer(Core::Constants::M_FORMAT_PARAGRAPH);
        if (ac) {
            foreach (QAction *action, ac->menu()->actions())
                m_ToolBar->addAction(action);
            m_ToolBar->addSeparator();
        }
        ac = am->actionContainer(Core::Constants::M_FORMAT_TABLE);
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
    TextEditor::Types m_Type;
    EditorContext *m_Context;
    QToolBar *m_ToolBar;
    QTextEdit *textEdit;
    QWidget *m_Parent;
    bool m_ToolBarIsVisible;
};

}  // End Internal
}  // End Editor


//--------------------------------------------------------------------------------------------------------
//------------------------------------ TextEditor implementation -----------------------------------
//--------------------------------------------------------------------------------------------------------
TextEditor::TextEditor( QWidget *parent, Types type )
          : TableEditor(parent), d(0)
{
    static int handler = 0;
    handler++;
    setObjectName( "TextEditor_" + QString::number(handler) );

    // instanciate private part
    d = new TextEditorPrivate(this, type);

    // instanciate context for actions
    d->m_Context = new EditorContext(this);
    setTypes(type);
    Core::ICore::instance()->contextManager()->addContextObject(d->m_Context);

    // instanciate editor manager
    EditorManager::instance();

    // create ToolBar
    d->createToolBar();
    toogleToolbar(false);

    // create QWidget
    QVBoxLayout * vb = new QVBoxLayout( this );
    vb->setObjectName( "TextEditorLayout" );
    vb->setSpacing( 0 );
    vb->setMargin( 0 );

    vb->addWidget( d->m_ToolBar );
    vb->addWidget( d->textEdit );
}

TextEditor::~TextEditor()
{
    // unreg context
    Core::ICore::instance()->contextManager()->removeContextObject(d->m_Context);
    if (d) delete d;
    d=0;
}

QTextEdit * TextEditor::textEdit() const
{
    return d->textEdit;
}

void TextEditor::setTypes(Types type)
{
    d->m_Context->clearContext();
    Core::UniqueIDManager *uid = Core::ICore::instance()->uniqueIDManager();
    d->m_Context->addContext(uid->uniqueIdentifier(Core::Constants::C_EDITOR_BASIC));
    if (type & TextEditor::WithTables) {
        d->m_Context->addContext(uid->uniqueIdentifier(Core::Constants::C_EDITOR_TABLE));
    }
    if (type & TextEditor::WithIO) {
        d->m_Context->addContext(uid->uniqueIdentifier(Core::Constants::C_EDITOR_IO));
    }
}

QMenu* TextEditor::getContextMenu()
{
    QMenu *mc = new QMenu(this);
    Core::ActionManager *am = Core::ICore::instance()->actionManager();
    mc->setTitle(tkTr(Trans::Constants::EDITORMENU_TEXT));

    Core::ActionContainer *cMenu = am->actionContainer(Core::Constants::M_EDITOR_CONTEXT);
    if (cMenu) {
        foreach (QAction *action, cMenu->menu()->actions())
            mc->addAction(action);
    }
    if (d->m_Type & WithIO) {
        QMenu *m = new QMenu(this);
        m->setTitle(tkTr(Trans::Constants::M_FILE_TEXT));
        Core::Command *cmd = am->command(Core::Constants::A_EDITOR_FILEOPEN);
        m->addAction(cmd->action());
        cmd = am->command(Core::Constants::A_EDITOR_FILESAVE);
        m->addAction(cmd->action());
        mc->addMenu(m);
    }
    cMenu = am->actionContainer(Core::Constants::M_EDIT);
    if (cMenu) {
        QMenu *m = new QMenu(cMenu->menu()->title(), this);
        foreach (QAction *action, cMenu->menu()->actions())
            m->addAction(action);
        mc->addMenu(m);
    }
    cMenu = am->actionContainer(Core::Constants::M_FORMAT_FONT);
    if (cMenu) {
        QMenu *m = new QMenu(cMenu->menu()->title(), this);
        foreach (QAction *action, cMenu->menu()->actions())
            m->addAction(action);
        mc->addMenu(m);
    }
    cMenu = am->actionContainer(Core::Constants::M_FORMAT_PARAGRAPH);
    if (cMenu) {
        QMenu *m = new QMenu(cMenu->menu()->title(), this);
        foreach (QAction *action, cMenu->menu()->actions())
            m->addAction(action);
        mc->addMenu(m);
    }
    cMenu = am->actionContainer(Core::Constants::M_FORMAT_TABLE);
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

void TextEditor::contextMenu( const QPoint &pos )
{
    QMenu * p = getContextMenu();
    p->exec(this->mapToGlobal(pos));
}

void TextEditor::toogleToolbar(bool state)
{
    if (d->m_ToolBar) {
        d->m_ToolBar->setVisible(state);
        d->m_ToolBarIsVisible = state;
    }
}

bool TextEditor::toolbarIsVisible() const
{
    return d->m_ToolBarIsVisible;
}

void TextEditor::hideToolbar()
{
    d->m_ToolBar->setVisible(false);
}

//--------------------------------------------------------------------------------------------------------
//------------------------------------------------- File Open / Save -------------------------------------
//--------------------------------------------------------------------------------------------------------
void TextEditor::fileOpen()
{
    QAction *a = qobject_cast<QAction *>(sender());
    QString title;
    if (!a)
        title = tkTr(Trans::Constants::EDITORMENU_TEXT);
    else
        title = a->text();
    QString file = QFileDialog::getOpenFileName( this, title,
                                               QString(), tr( "HTML files (*.htm *.html);;Text files (*.txt);;All Files (*)" ) );
    if ( file.isEmpty() )
        return;
    QString str = Core::tkGlobal::readTextFile(file, Core::tkGlobal::WarnUser, this);
    if ( Qt::mightBeRichText(str) ) {
        textEdit()->setHtml(str);
    } else {
        textEdit()->setPlainText(str);
    }
}

void TextEditor::saveAs()
{
    QAction *a = qobject_cast<QAction *>(sender());
    QString title;
    if (!a)
        title = tkTr(Trans::Constants::EDITORMENU_TEXT);
    else
        title = a->text();
    QString fileName = QFileDialog::getSaveFileName( this, title,
                                                     QString(), tr( "HTML-Files (*.htm *.html);;All Files (*)" ) );
    if ( fileName.isEmpty() )
        return ;
    if (Core::tkGlobal::saveStringToFile( Core::tkGlobal::toHtmlAccent(textEdit()->document()->toHtml("UTF-8")),
                                    fileName, Core::tkGlobal::WarnUser, this ))
        textEdit()->document()->setModified( false );
}

//--------------------------------------------------------------------------------------------------------
//--------------------------------------------- Text Decoration ------------------------------------------
//--------------------------------------------------------------------------------------------------------
void TextEditor::fontFormat()
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

void TextEditor::fontBigger()
{
    QFont font = textEdit()->textCursor().charFormat().font();
    QTextCharFormat fmt;
    font.setPointSize( font.pointSize() + 1 );
    fmt.setFont( font);
    d->mergeFormatOnWordOrSelection( fmt );
}

void TextEditor::fontSmaller()
{
    QFont font = textEdit()->textCursor().charFormat().font();
    QTextCharFormat fmt;
    font.setPointSize( font.pointSize() - 1 );
    fmt.setFont( font);
    d->mergeFormatOnWordOrSelection( fmt );
}

void TextEditor::textBold( bool checked )
{
    QTextCharFormat fmt;
    fmt.setFontWeight( checked ? QFont::Bold : QFont::Normal );
    d->mergeFormatOnWordOrSelection( fmt );
}

void TextEditor::textUnderline( bool checked )
{
    QTextCharFormat fmt;
    fmt.setFontUnderline( checked );
    d->mergeFormatOnWordOrSelection( fmt );
}

void TextEditor::textItalic( bool checked )
{
    QTextCharFormat fmt;
    fmt.setFontItalic( checked );
    d->mergeFormatOnWordOrSelection( fmt );
}

void TextEditor::textStrike( bool checked )
{
    QTextCharFormat fmt;
    fmt.setFontStrikeOut( checked );
    d->mergeFormatOnWordOrSelection( fmt );
}

//void TextEditor::textFamily( const QString &f )
//{
//    QTextCharFormat fmt;
//    fmt.setFontFamily( f );
//    mergeFormatOnWordOrSelection( fmt );
//}
//
//void TextEditor::textSize( const QString &p )
//{
//    QTextCharFormat fmt;
//    fmt.setFontPointSize( p.toFloat() );
//    mergeFormatOnWordOrSelection( fmt );
//}
//
//void TextEditor::textStyle( int styleIndex )
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

void TextEditor::textColor()
{
    QColor col = QColorDialog::getColor( textEdit()->textColor(), this );
    if ( !col.isValid() )
        return;
    QTextCharFormat fmt;
    fmt.setForeground( col );
    d->mergeFormatOnWordOrSelection( fmt );
}

//void TextEditorPrivate::about()
//{
//    QMessageBox::about( m_Parent, tkTr(ABOUT_TEXT),
//                        tr( "This rich text editor is part of FreeMedForms Project. " ) );
//}


//void TextEditorPrivate::colorChanged( const QColor &c )
//{
//    QPixmap pix( 16, 16 );
//    pix.fill( c );
////    actionTextColor->setIcon( pix );
//}


//--------------------------------------------------------------------------------------------------------
//---------------------------------------------- Retranslate UI ------------------------------------------
//--------------------------------------------------------------------------------------------------------
void TextEditor::changeEvent(QEvent *e)
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

void TextEditorPrivate::retranslateUi()
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

