/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
/**
  \class Editor::TextEditor
  \todo Document code.
*/

#include "texteditor.h"
#include "editormanager.h"
#include "editorcontext.h"

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_editor.h>
#include <translationutils/trans_menu.h>
#include <translationutils/trans_filepathxml.h>

#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/iuser.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/uniqueidmanager.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/isettings.h>

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

static inline Core::IUser *user() {return Core::ICore::instance()->user();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}

namespace Editor {
namespace Internal {

class TextEditorHtmlPaster : public QTextEdit
{
public:
    TextEditorHtmlPaster(QWidget *parent = 0) : QTextEdit(parent) {}
    TextEditorHtmlPaster(const QString &text, QWidget *parent = 0) : QTextEdit(text, parent) {}
    ~TextEditorHtmlPaster() {}

    bool canInsertFromMimeData(const QMimeData *source) const {qWarning() << source; return true;}
    void insertFromMimeData(const QMimeData *source)
    {
        qWarning() << source->formats();
        qWarning() << "hasHtml" << source->hasHtml();
        qWarning() << "mime" << source->data(source->formats().at(0));
        QString h = "html";
        qWarning() << "clipboard text" << QApplication::clipboard()->text();
        qWarning() << "clip formats" << QApplication::clipboard()->mimeData()->formats();
    }

};

class TextEditorPrivate
{
public:
    TextEditorPrivate(QWidget *parent , TextEditor::Types type)
            : m_Type(type), m_Context(0), textEdit(0), m_Parent(parent), m_ToolBarIsVisible(false)
    {
        textEdit = new QTextEdit(m_Parent);
        //textEdit = new TextEditorHtmlPaster(m_Parent);
        textEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    }

    ~TextEditorPrivate()
    {
        delete m_Context;
        m_Context = 0;
    }

    void createToolBar()
    {
        m_ToolBar = new QToolBar(m_Parent);
#if QT_VERSION > 0x040600
        m_ToolBar->setIconSize(QSize(16,16));
#endif
        m_ToolBar->setFocusPolicy(Qt::ClickFocus);
        populateToolbar();
    }

    void populateToolbar()
    {
        Core::ActionManager *am = Core::ICore::instance()->actionManager();
        m_ToolBar->clear();
        QStringList actions;

        // IO Actions
        if (m_Type & TextEditor::WithIO) {
            actions << Core::Constants::A_EDITOR_FILEOPEN
                    << Core::Constants::A_EDITOR_FILESAVE
                    << Core::Constants::A_EDITOR_FILEPRINT
                    ;
            foreach(const QString &a, actions) {
                Core::Command *cmd = am->command(a);
                if (cmd)
                    m_ToolBar->addAction(cmd->action());
            }
            m_ToolBar->addSeparator();
        }

        actions.clear();
        actions
                << Core::Constants::A_FILE_PRINT
                << Core::Constants::A_EDIT_COPY
                << Core::Constants::A_EDIT_PASTE
                << Core::Constants::A_EDIT_CUT
                ;
        foreach(const QString &a, actions) {
            Core::Command *cmd = am->command(a);
            if (cmd)
                m_ToolBar->addAction(cmd->action());
        }
        m_ToolBar->addSeparator();

        actions.clear();
        actions << Core::Constants::A_EDIT_UNDO
                << Core::Constants::A_EDIT_REDO
                ;
        foreach(const QString &a, actions) {
            Core::Command *cmd = am->command(a);
            if (cmd)
                m_ToolBar->addAction(cmd->action());
        }
        m_ToolBar->addSeparator();

        QAction *previous = 0;
        if (m_Type & TextEditor::WithTables) {
            actions << Core::Constants::M_FORMAT_FONT
                    << Core::Constants::M_FORMAT_PARAGRAPH
                    << Core::Constants::M_FORMAT_TABLE
                    ;
        } else {
            actions << Core::Constants::M_FORMAT_FONT
                    << Core::Constants::M_FORMAT_PARAGRAPH
                    ;
        }

        foreach(const QString &m, actions) {
            Core::ActionContainer *ac = am->actionContainer(m);
            if (ac) {
                foreach (QAction *action, ac->menu()->actions()) {
                    if (previous) {
                        if (previous->isSeparator() && action->isSeparator()) {
                            continue;
                        }
                    } else {
                        if (action->isSeparator())
                            continue;
                    }
                    m_ToolBar->addAction(action);
                    previous = action;
                }
                previous = m_ToolBar->addSeparator();
            }
        }
    }


    void mergeFormatOnWordOrSelection(const QTextCharFormat &format)
    {
        QTextCursor cursor = textEdit->textCursor();
        if (!cursor.hasSelection())
            cursor.select(QTextCursor::WordUnderCursor);
        cursor.mergeCharFormat(format);
        textEdit->mergeCurrentCharFormat(format);
    }           
    
public:
    TextEditor::Types m_Type;
    EditorContext *m_Context;
    QToolBar *m_ToolBar;
    QTextEdit *textEdit;
    //TextEditorHtmlPaster *textEdit;
    QWidget *m_Parent;
    bool m_ToolBarIsVisible;
};

}  // End Internal
}  // End Editor


//--------------------------------------------------------------------------------------------------------
//------------------------------------ TextEditor implementation -----------------------------------
//--------------------------------------------------------------------------------------------------------
TextEditor::TextEditor(QWidget *parent, Types type)
          : TableEditor(parent), d(0)
{
    static int handler = 0;
    handler++;
    setObjectName("TextEditor_" + QString::number(handler));

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
    QVBoxLayout * vb = new QVBoxLayout(this);
    vb->setObjectName("TextEditorLayout");
    vb->setSpacing(0);
    vb->setMargin(0);

    vb->addWidget(d->m_ToolBar);
    vb->addWidget(d->textEdit);
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
    if (type & TextEditor::WithTextCompleter) {
        d->m_Context->addContext(uid->uniqueIdentifier(Core::Constants::C_EDITOR_ADDTEXT));
    }
}

QMenu *TextEditor::getContextMenu()
{
    QMenu *mc = new QMenu(this);
    Core::ActionManager *am = Core::ICore::instance()->actionManager();
    mc->setTitle(tkTr(Trans::Constants::EDITORMENU_TEXT));

//    Core::ActionContainer *cMenu = am->actionContainer(Core::Constants::M_EDITOR_CONTEXT);
//    if (cMenu) {
//        foreach (QAction *action, cMenu->menu()->actions())
//            mc->addAction(action);
//    }
    Core::Command *cmd = 0;
    QStringList actions;

    cmd = am->command(Core::Constants::A_EDITOR_TOOGLETOOLBAR);
    if (cmd) {
        mc->addAction(cmd->action());
        mc->addSeparator();
    }

    if (d->m_Type & WithTextCompleter) {
        QMenu *m = new QMenu(this);
        m->setTitle(tkTr(Trans::Constants::EDITOR_ADDTEXTMENU_TEXT));
        actions << Core::Constants::A_EDITOR_ADDDATE
                << Core::Constants::A_EDITOR_ADDUSERNAME
                << Core::Constants::A_EDITOR_ADDPATIENTNAME
                ;
        foreach(const QString &a, actions) {
            cmd = am->command(a);
            if (cmd)
                m->addAction(cmd->action());
        }
        mc->addMenu(m);
    }
    mc->addSeparator();
    actions.clear();

    actions.clear();

    if (d->m_Type & WithIO) {
        QMenu *m = new QMenu(this);
        m->setTitle(tkTr(Trans::Constants::M_FILE_TEXT));
        actions << Core::Constants::A_EDITOR_FILEOPEN
                << Core::Constants::A_EDITOR_FILESAVE
                << Core::Constants::A_FILE_PRINT
                << Core::Constants::A_FILE_PRINTPREVIEW
                ;
        foreach(const QString &a, actions) {
            cmd = am->command(a);
            if (cmd)
                m->addAction(cmd->action());
        }
        mc->addMenu(m);
    }

    actions.clear();
    QMenu *medit = new QMenu(this);
    medit->setTitle(tkTr(Trans::Constants::M_EDIT_TEXT));
    actions << Core::Constants::A_EDIT_COPY
            << Core::Constants::A_EDIT_PASTE
            << Core::Constants::A_EDIT_CUT
            ;
    foreach(const QString &a, actions) {
        Core::Command *cmd = am->command(a);
        if (cmd)
            medit->addAction(cmd->action());
    }
    medit->addSeparator();

    actions.clear();
    actions << Core::Constants::A_EDIT_UNDO
            << Core::Constants::A_EDIT_REDO
            ;
    foreach(const QString &a, actions) {
        Core::Command *cmd = am->command(a);
        if (cmd)
            medit->addAction(cmd->action());
    }
    mc->addMenu(medit);

    QAction *previous = 0;
    if (d->m_Type & WithTables) {
        actions << Core::Constants::M_FORMAT_FONT
                << Core::Constants::M_FORMAT_PARAGRAPH
                << Core::Constants::M_FORMAT_TABLE
                ;
    } else {
        actions << Core::Constants::M_FORMAT_FONT
                << Core::Constants::M_FORMAT_PARAGRAPH
                ;
    }

    foreach(const QString &m, actions) {
        Core::ActionContainer *ac = am->actionContainer(m);
        if (ac) {
            QMenu *menu = new QMenu(this);
            menu->setTitle(ac->menu()->title());
            foreach (QAction *action, ac->menu()->actions()) {
                if (previous) {
                    if (previous->isSeparator() && action->isSeparator()) {
                        continue;
                    }
                } else {
                    if (action->isSeparator())
                        continue;
                }
                menu->addAction(action);
                previous = action;
            }
            previous = 0;
            mc->addMenu(menu);
        }
    }
    return mc;
}

void TextEditor::contextMenu(const QPoint &pos)
{
    if (textEdit()->underMouse()) {
        QTextCursor c = textEdit()->cursorForPosition(pos);
        textEdit()->setTextCursor(c);
    }
    QMenu *p = getContextMenu();
    p->exec(mapToGlobal(pos));
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
    QStringList filters;
    filters << tkTr(Trans::Constants::FILE_FILTER_HTML)
            << tkTr(Trans::Constants::FILE_FILTER_TXT)
            << tkTr(Trans::Constants::FILE_FILTER_RTF)
            << tkTr(Trans::Constants::FILE_FILTER_ALL_FILES);
    QString selected = tkTr(Trans::Constants::FILE_FILTER_HTML);

    QString file = QFileDialog::getOpenFileName(this, title,
                                                settings()->path(Core::ISettings::UserDocumentsPath),
                                                filters.join(";;"),
                                                &selected);
    if (file.isEmpty())
        return;
    QString str = Utils::readTextFile(file, Utils::WarnUser, this);
    if (Qt::mightBeRichText(str)) {
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
    QStringList filters;
    filters << tkTr(Trans::Constants::FILE_FILTER_HTML)
            << tkTr(Trans::Constants::FILE_FILTER_TXT);
    QString selected = tkTr(Trans::Constants::FILE_FILTER_HTML);
    QString fileName = QFileDialog::getSaveFileName(this, title,
                                                    settings()->path(Core::ISettings::UserDocumentsPath),
                                                    filters.join(";;"),
                                                    &selected);
    if (fileName.isEmpty())
        return;
    if (selected==tkTr(Trans::Constants::FILE_FILTER_HTML)) {
        if (Utils::saveStringToFile(Utils::toHtmlAccent(textEdit()->document()->toHtml("UTF-8")),
                                    fileName, Utils::Overwrite, Utils::WarnUser, this))
            textEdit()->document()->setModified(false);
    } else if (selected==tkTr(Trans::Constants::FILE_FILTER_TXT)) {
        bool yes = Utils::yesNoMessageBox(tr("Save in pure textual format ?"),
                                          tr("The conversion of the document to a pure textual format will cause "
                                             "the lost of the paragraph and characters format. Do you really want "
                                             "to save in pure textual format ?"));
        if (yes) {
            if (Utils::saveStringToFile(textEdit()->document()->toPlainText(),
                                        fileName, Utils::Overwrite, Utils::WarnUser, this))
                textEdit()->document()->setModified(false);
        }

    }
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
#if QT_VERSION >= 0x040500
    QFont selectedfont = QFontDialog::getFont(&ok, font, this, tr("Font format"), QFontDialog::DontUseNativeDialog);
#else
    QFont selectedfont = QFontDialog::getFont(&ok, font, this, tr("Font format"));
#endif
    // 3. apply, if accepted, font style
    if (ok) {
        QTextCharFormat fmt;
        fmt.setFont(selectedfont);
        d->mergeFormatOnWordOrSelection(fmt);
    }
}

void TextEditor::fontBigger()
{
    QFont font = textEdit()->textCursor().charFormat().font();
    QTextCharFormat fmt;
    font.setPointSize(font.pointSize() + 1);
    fmt.setFont(font);
    d->mergeFormatOnWordOrSelection(fmt);
}

void TextEditor::fontSmaller()
{
    QFont font = textEdit()->textCursor().charFormat().font();
    QTextCharFormat fmt;
    font.setPointSize(font.pointSize() - 1);
    fmt.setFont(font);
    d->mergeFormatOnWordOrSelection(fmt);
}

void TextEditor::textBold(bool checked)
{
    QTextCharFormat fmt;
    fmt.setFontWeight(checked ? QFont::Bold : QFont::Normal);
    d->mergeFormatOnWordOrSelection(fmt);
}

void TextEditor::textUnderline(bool checked)
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(checked);
    d->mergeFormatOnWordOrSelection(fmt);
}

void TextEditor::textItalic(bool checked)
{
    QTextCharFormat fmt;
    fmt.setFontItalic(checked);
    d->mergeFormatOnWordOrSelection(fmt);
}

void TextEditor::textStrike(bool checked)
{
    QTextCharFormat fmt;
    fmt.setFontStrikeOut(checked);
    d->mergeFormatOnWordOrSelection(fmt);
}

//void TextEditor::textFamily(const QString &f)
//{
//    QTextCharFormat fmt;
//    fmt.setFontFamily(f);
//    mergeFormatOnWordOrSelection(fmt);
//}
//
//void TextEditor::textSize(const QString &p)
//{
//    QTextCharFormat fmt;
//    fmt.setFontPointSize(p.toFloat());
//    mergeFormatOnWordOrSelection(fmt);
//}
//
//void TextEditor::textStyle(int styleIndex)
//{
//    QTextCursor cursor = textEdit->textCursor();
//
//    if (styleIndex != 0)  {
//        QTextListFormat::Style style = QTextListFormat::ListDisc;
//        switch (styleIndex)
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
//        if (cursor.currentList()) {
//            listFmt = cursor.currentList()->format();
//        } else {
//            listFmt.setIndent(blockFmt.indent() + 1);
//            blockFmt.setIndent(0);
//            cursor.setBlockFormat(blockFmt);
//        }
//        listFmt.setStyle(style);
//        cursor.createList(listFmt);
//        cursor.endEditBlock();
//    } else {
//        // ####
//        QTextBlockFormat bfmt;
//        bfmt.setObjectIndex(-1);
//        cursor.mergeBlockFormat(bfmt);
//    }
//}

void TextEditor::textColor()
{
    QColor col = QColorDialog::getColor(textEdit()->textColor(), this);
    if (!col.isValid())
        return;
    QTextCharFormat fmt;
    fmt.setForeground(col);
    d->mergeFormatOnWordOrSelection(fmt);
}

void TextEditor::addDate()
{
    textEdit()->insertHtml(QDateTime::currentDateTime().toString(QLocale().dateTimeFormat(QLocale::LongFormat)));
}

void TextEditor::addUserName()
{
    textEdit()->insertHtml(user()->value(Core::IUser::FullName).toString());
}

void TextEditor::addPatientName()
{
    textEdit()->insertHtml(patient()->data(Core::IPatient::FullName).toString());
}

//void TextEditorPrivate::about()
//{
//    QMessageBox::about(m_Parent, tkTr(ABOUT_TEXT),
//                        tr("This rich text editor is part of FreeMedForms Project. "));
//}


//void TextEditorPrivate::colorChanged(const QColor &c)
//{
//    QPixmap pix(16, 16);
//    pix.fill(c);
////    actionTextColor->setIcon(pix);
//}

