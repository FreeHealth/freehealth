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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   This code is inspired of the Qt example : Text Edit                   *
 *   Adaptations to FreeMedForms and improvments by : Eric Maeker, MD      *
 *   eric.maeker@gmail.com                                                 *
 ***************************************************************************/

/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
  \class Editor::ITextControl
  Overloads the QTextEdit copy/paste mimeData engine.
*/

/**
  \class Editor::TextEditor
  Complex text editor for the FreeMedForms project.
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

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/iuser.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/ipadtools.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>

#include <extensionsystem/pluginmanager.h>

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
#include <QTextCodec>
#include <QTextEdit>
#include <QToolBar>
#include <QTextList>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QVBoxLayout>
#include <QStringList>
#include <QFontDialog>
#include <QPointer>
#include <QMimeData>

using namespace Editor;
using namespace Editor::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::IUser *user() {return Core::ICore::instance()->user();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::IDocumentPrinter *printer() {return ExtensionSystem::PluginManager::instance()->getObject<Core::IDocumentPrinter>();}
#ifdef WITH_PAD
    static inline Core::IPadTools *padTools() {return Core::ICore::instance()->padTools();}
#endif


namespace Editor {
namespace Internal {

// TODO: manage multiple controls
class TextEditorWithControl : public QTextEdit
{
public:
    TextEditorWithControl(QWidget *parent = 0) : QTextEdit(parent), _control(0) {}
    TextEditorWithControl(const QString &text, QWidget *parent = 0) : QTextEdit(text, parent), _control(0) {}
    ~TextEditorWithControl() {}

    void setTextControl(ITextControl *control)
    {
        _control = control;
    }

    bool canInsertFromMimeData(const QMimeData *source) const
    {
        if (_control)
            return _control->canInsertFromMimeData(source);
        return QTextEdit::canInsertFromMimeData(source);
    }

    void insertFromMimeData(const QMimeData *source)
    {
        if (_control)
            _control->insertFromMimeData(source);
        else
            QTextEdit::insertFromMimeData(source);
    }

    QMimeData *createMimeDataFromSelection() const
    {
        if (_control)
            return _control->createMimeDataFromSelection();
        else
            return QTextEdit::createMimeDataFromSelection();
    }


private:
    QPointer<ITextControl> _control;
};

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
    TextEditorPrivate(TextEditor *parent , TextEditor::Types type) :
        m_Type(type),
        m_Context(0),
        m_ToolBar(0),
        textEdit(0),
        m_Parent(parent),
        m_ToolBarIsVisible(false),
        _papers(Core::IDocumentPrinter::Papers_Generic_User),
        _alwaysPrintDuplicata(false),
        q(parent)
    {
        textEdit = new TextEditorWithControl(m_Parent);
        //textEdit = new TextEditorHtmlPaster(m_Parent);
        textEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    }

    ~TextEditorPrivate()
    {
        if (m_Context) {
            delete m_Context;
            m_Context = 0;
        }
    }

    void createToolBar()
    {
        m_ToolBar = new QToolBar(m_Parent);
        m_ToolBar->setIconSize(QSize(16,16));
        m_ToolBar->setFocusPolicy(Qt::ClickFocus);
//        populateToolbar();
    }

    void populateToolbar()
    {
        Core::ActionManager *am = Core::ICore::instance()->actionManager();
        m_ToolBar->clear();
        QStringList actions;

        // Actions
        if (m_Type & TextEditor::WithIO) {
            actions << Core::Constants::A_EDITOR_FILEOPEN
                    << Core::Constants::A_EDITOR_FILESAVE
                    << "--"
                    ;
        }

        if (m_Type & TextEditor::Print)
            actions << Core::Constants::A_FILE_PRINT
                << "--";

        if (m_Type & TextEditor::Clipboard) {
            actions << Core::Constants::A_EDIT_COPY
                    << Core::Constants::A_EDIT_PASTE
                    << Core::Constants::A_EDIT_CUT
                    << "--"
                    ;
        }

        actions << Core::Constants::A_EDIT_UNDO
                << Core::Constants::A_EDIT_REDO
                << "--"
                ;
        foreach(const QString &a, actions) {
            if (a=="--") {
                m_ToolBar->addSeparator();
                continue;
            }
            Core::Command *cmd = am->command(Core::Id(a));
            if (cmd)
                m_ToolBar->addAction(cmd->action());
        }

        // Menu contents
        QAction *previous = 0;
        actions.clear();
        if (m_Type & TextEditor::CharFormat) {
            actions << Core::Constants::M_FORMAT_FONT;
        }
        if (m_Type & TextEditor::ParagraphFormat) {
            actions << Core::Constants::M_FORMAT_PARAGRAPH;
        }
        if (m_Type & TextEditor::WithTables) {
            actions << Core::Constants::M_FORMAT_TABLE;
        }

        foreach(const QString &m, actions) {
            Core::ActionContainer *ac = am->actionContainer(Core::Id(m));
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

    // Read file content and run padtools on the content according to the running app
    // Does not modify the _currentFileName string
    void readFile(const QString &file)
    {
        QString str = Utils::readTextFile(file, Utils::WarnUser);
        // run token if app is EHR
    #ifdef FREEHEALTH
        patient()->replaceTokens(str);
        user()->replaceTokens(str);

    #ifdef WITH_PAD
        str = padTools()->processPlainText(str);
    #endif

    #endif
        if (Qt::mightBeRichText(str)) {
            q->textEdit()->setHtml(str);
        } else {
            q->textEdit()->setPlainText(str);
        }
    }

public:
    TextEditor::Types m_Type;
    EditorContext *m_Context;
    QToolBar *m_ToolBar;
    TextEditorWithControl *textEdit;
    //TextEditorHtmlPaster *textEdit;
    QWidget *m_Parent;
    bool m_ToolBarIsVisible;

    Core::IDocumentPrinter::PapersToUse _papers;
    bool _alwaysPrintDuplicata;
    QString _docTitle, _currentFileName;

private:
    TextEditor *q;
};

}  // End Internal
}  // End Editor


//--------------------------------------------------------------------------------------------------------
//------------------------------------ TextEditor implementation -----------------------------------
//--------------------------------------------------------------------------------------------------------
TextEditor::TextEditor(QWidget *parent, Types type) :
    TableEditor(parent), d(0)
{
    static int handler = 0;
    handler++;
    setObjectName("TextEditor_" + QString::number(handler));

    // instanciate private part
    d = new TextEditorPrivate(this, type);
    // TODO: remove this: instanciate editor manager
    EditorManager::instance();

    // create ToolBar
    d->createToolBar();
    toogleToolbar(false);

    // instanciate context for actions
    d->m_Context = new EditorContext(this);
    Core::ICore::instance()->contextManager()->addContextObject(d->m_Context);
    setTypes(type);

    // create QWidget
    QVBoxLayout * vb = new QVBoxLayout(this);
    vb->setObjectName("TextEditorLayout");
    vb->setSpacing(0);
    vb->setMargin(0);

    vb->addWidget(d->m_ToolBar);
    vb->addWidget(d->textEdit);

    setFocusProxy(d->textEdit);
}

TextEditor::~TextEditor()
{
    // unreg context
    // FIXME: manage a segfault with freepad when updating context.
    Core::ICore::instance()->contextManager()->removeContextObject(d->m_Context);
    if (d) {
        delete d;
        d=0;
    }
}

QTextEdit *TextEditor::textEdit() const
{
    return d->textEdit;
}

void TextEditor::setTextControl(ITextControl *control)
{
    d->textEdit->setTextControl(control);
}

void TextEditor::setTypes(Types type)
{
    d->m_Type = type;
    Core::Context context;
    if (type & TextEditor::CharFormat)
        context.add(Core::Constants::C_EDITOR_CHAR_FORMAT);
    if (type & TextEditor::ParagraphFormat)
        context.add(Core::Constants::C_EDITOR_PARAGRAPH);
    if (type & TextEditor::Clipboard)
        context.add(Core::Constants::C_EDITOR_CLIPBOARD);
    if (type & TextEditor::Print)
        context.add(Core::Constants::C_EDITOR_PRINT);
    if (type & TextEditor::WithTables)
        context.add(Core::Constants::C_EDITOR_TABLE);
    if (type & TextEditor::WithIO)
        context.add(Core::Constants::C_EDITOR_IO);
    if (type & TextEditor::WithTextCompleter)
        context.add(Core::Constants::C_EDITOR_ADDTEXT);
    d->m_Context->setContext(context);
    // update toolbar
    d->populateToolbar();
    Core::ICore::instance()->contextManager()->updateContext();
}

/**
 * Define the papers for the document. Papers are header, footer and watermark.
 * By default, set to Core::IDocumentPrinter::Papers_Generic_User.
 * \sa Core::IDocumentPrinter
 */
void TextEditor::setUserDocumentForPrintingProcess(Core::IDocumentPrinter::PapersToUse papers)
{
    d->_papers = papers;
}

/** Define the duplicata printing. By default, set to false. */
void TextEditor::setAlwaysPrintDuplicata(bool printDuplicata)
{
    d->_alwaysPrintDuplicata = printDuplicata;
}

/** Define the document title to use for the print process (token content) */
void TextEditor::setDocumentTitle(const QString &title)
{
    d->_docTitle = title;
}

/** Open a selected file in the editor */
void TextEditor::setCurrentFileName(const QString &absPath)
{
    d->_currentFileName = absPath;
}

/** If the editor is currently editing a file, returns the abspath of the file */
QString TextEditor::currentFileName() const
{
    return d->_currentFileName;
}

/** Adds a Core::Context to this contextual widget */
void TextEditor::addContext(const Core::Context &context)
{
    Core::Context ctx = d->m_Context->context();
    ctx.add(context);
    d->m_Context->setContext(ctx);
    Core::ICore::instance()->contextManager()->updateContext();
}

/** Creates the context menu for the Editor::TextEditor according to its Editor::TextEditor::Type. The menu is not executed. */
QMenu *TextEditor::getContextMenu()
{
    QMenu *mc = new QMenu(this);
    Core::ActionManager *am = Core::ICore::instance()->actionManager();
    mc->setTitle(tkTr(Trans::Constants::EDITORMENU_TEXT).remove("&"));

//    Core::ActionContainer *cMenu = am->actionContainer(Core::Constants::M_EDITOR_CONTEXT);
//    if (cMenu) {
//        foreach (QAction *action, cMenu->menu()->actions())
//            mc->addAction(action);
//    }
    Core::Command *cmd = 0;
    QStringList actions;

    cmd = am->command(Core::Id(Core::Constants::A_EDITOR_TOOGLETOOLBAR));
    if (cmd) {
        mc->addAction(cmd->action());
        mc->addSeparator();
    }

    if (d->m_Type & WithTextCompleter) {
        QMenu *m = new QMenu(this);
        m->setTitle(tkTr(Trans::Constants::EDITOR_ADDTEXTMENU_TEXT).remove("&"));
        actions << Core::Constants::A_EDITOR_ADDDATE_LONG
                << Core::Constants::A_EDITOR_ADDDATE_SHORT
                << Core::Constants::A_EDITOR_ADDUSERNAME
                << Core::Constants::A_EDITOR_ADDPATIENTNAME
                ;
        foreach(const QString &a, actions) {
            cmd = am->command(Core::Id(a));
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
        m->setTitle(tkTr(Trans::Constants::M_FILE_TEXT).remove("&"));
        actions << Core::Constants::A_EDITOR_FILEOPEN
                << Core::Constants::A_EDITOR_FILESAVE
                << Core::Constants::A_FILE_PRINT
                << Core::Constants::A_FILE_PRINTPREVIEW
                ;
        foreach(const QString &a, actions) {
            cmd = am->command(Core::Id(a));
            if (cmd)
                m->addAction(cmd->action());
        }
        mc->addMenu(m);
    }

    actions.clear();
    QMenu *medit = new QMenu(this);
    medit->setTitle(tkTr(Trans::Constants::M_EDIT_TEXT).remove("&"));
    actions << Core::Constants::A_EDIT_COPY
            << Core::Constants::A_EDIT_PASTE
            << Core::Constants::A_EDIT_CUT
            ;
    foreach(const QString &a, actions) {
        Core::Command *cmd = am->command(Core::Id(a));
        if (cmd)
            medit->addAction(cmd->action());
    }
    medit->addSeparator();

    actions.clear();
    actions << Core::Constants::A_EDIT_UNDO
            << Core::Constants::A_EDIT_REDO
            ;
    foreach(const QString &a, actions) {
        Core::Command *cmd = am->command(Core::Id(a));
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
        Core::ActionContainer *ac = am->actionContainer(Core::Id(m));
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
    QString selected = tkTr(Trans::Constants::FILE_FILTER_ALL_FILES);

    QString file = QFileDialog::getOpenFileName(this, title,
                                                settings()->path(Core::ISettings::UserDocumentsPath),
                                                filters.join(";;"),
                                                &selected);
    if (file.isEmpty())
        return;
    d->_currentFileName = file;
    d->readFile(file);
}

/** Save the content of the text editor */
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
    d->_currentFileName = fileName;
    if (selected==tkTr(Trans::Constants::FILE_FILTER_HTML)) {
        if (Utils::saveStringToFile(Utils::htmlReplaceAccents(textEdit()->document()->toHtml("UTF-8")),
                                    fileName, Utils::Overwrite, Utils::WarnUser, this))
            textEdit()->document()->setModified(false);
    } else if (selected==tkTr(Trans::Constants::FILE_FILTER_TXT)) {
        bool yes = Utils::yesNoMessageBox(tr("Save in pure textual format?"),
                                          tr("The conversion of the document to a pure textual format will cause "
                                             "the lost of the paragraph and characters format. Do you really want "
                                             "to save in pure textual format?"));
        if (yes) {
            if (Utils::saveStringToFile(textEdit()->document()->toPlainText(),
                                        fileName, Utils::Overwrite, Utils::WarnUser, this))
                textEdit()->document()->setModified(false);
        }

    }
}

void TextEditor::print()
{
    Core::IDocumentPrinter *print = printer();
    print->clearTokens();
    QHash<QString, QVariant> tokens;
    tokens.insert(Core::Constants::TOKEN_DOCUMENTTITLE, d->_docTitle);
    print->addTokens(Core::IDocumentPrinter::Tokens_Global, tokens);
    print->print(d->textEdit->document(), d->_papers, d->_alwaysPrintDuplicata);
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
    QFont selectedfont = QFontDialog::getFont(&ok, font, this, tr("Font format"), QFontDialog::DontUseNativeDialog);
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

void TextEditor::typeWriterFont()
{
    QFont font = textEdit()->textCursor().charFormat().font();
    font.setFamily("Courier");
    QTextCharFormat fmt;
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

void TextEditor::addDate(DateFormat format)
{
    if (format==LongFormat)
        textEdit()->insertHtml(QDateTime::currentDateTime().toString(QLocale().dateTimeFormat(QLocale::LongFormat)));
    else //if (format==ShortFormat)
        textEdit()->insertHtml(QDateTime::currentDateTime().toString(QLocale().dateTimeFormat(QLocale::ShortFormat)));
}

void TextEditor::addUserName()
{
    textEdit()->insertHtml(user()->value(Core::IUser::FullName).toString());
}

void TextEditor::addPatientName()
{
    textEdit()->insertHtml(patient()->data(Core::IPatient::FullName).toString());
}


//void TextEditorPrivate::colorChanged(const QColor &c)
//{
//    QPixmap pix(16, 16);
//    pix.fill(c);
////    actionTextColor->setIcon(pix);
//}

