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
 *  Main Developers :                                                      *
 *      Eric Maeker                                                        *
 *  Contributors :                                                         *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/

/**
 * \class PadTools::PadWriter
 * To avoid dependencies to the pad plugin, the PadTools::PadWriter can be created
 * by the Core::IPadTools::createWriter().\n
 * The pad writer presents:
 * - a tool bar with some useful actions (show source, show output)
 * - a token tree view (shows all registered tokens of the central PadTools::Internal::TokenPool)
 * - a token's drop text editor that allow user to write comfortably any tokened document.
 * - a tokened document source
 * - and the potential output of the tokened document.
 *
 * Under the hood:
 * The pad writer gets the raw source tokened document using setPlainTextSource() or
 * setHtmlSource(). From this starting point, it analyzes the raw source and creates
 * the token drop text editor. This text editor is the central user interaction interface.\n
 * A tool bar allow user to request: source of the document, a pre-computation of the tokened
 * document. \n
 * When edition is done, you can get the raw source tokened document using rawSourceToPlainText()
 * or rawSourceToHtml(). And get the output document with outputToPlainText() or
 * outputToHtml().
*/

#include "padwriter.h"
#include "tokenmodel.h"
#include "constants.h"
#include "pad_analyzer.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/ipadtools.h>
#include <coreplugin/itheme.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/contextmanager/contextmanager.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/widgets/htmldelegate.h>

#include <QTimer>
#include <QToolBar>
#include <QToolButton>

#include "ui_padwriter.h"

#include <QDebug>

using namespace PadTools;
using namespace Internal;

static inline Core::ActionManager *actionManager() { return Core::ICore::instance()->actionManager(); }
static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }
static inline Core::ITheme *theme() { return Core::ICore::instance()->theme(); }
static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline Core::IPadTools *padTools() {return Core::ICore::instance()->padTools();}

namespace PadTools {
namespace Internal {
class PadWriterPrivate
{
public:
    PadWriterPrivate(PadWriter *parent) :
        _context(0),
        ui(0),
        _tokenModel(0),
        _padForEditor(0),
        _padForViewer(0),
        _toolBar(0),
        q(parent)
    {
    }

    void createUi()
    {
        ui = new Internal::Ui::PadWriter;
        ui->setupUi(q);
        ui->tokenTreeLayout->setMargin(0);
        ui->tokenTreeLayout->setSpacing(0);
        ui->listWidgetErrors->hide();
        ui->rawSource->hide();
        ui->outputTextEditor->setReadOnly(true);
    }

    void registerContext()
    {
        _context = new PadWriterContext(q);
        ui->dropTextEditor->addContext(_context->context());
        ui->rawSource->addContext(_context->context());
        contextManager()->addContextObject(_context);
    }

    void removeContext()
    {
        contextManager()->removeContextObject(_context);
    }

    void createActions()
    {
        // TEST
        QAction *a = aTest1 = new QAction(q);
        a->setText("Tokens and strings");
        a->setIcon(theme()->icon(Core::Constants::ICONHELP));

        a = aTest2 = new QAction(q);
        a->setText("Simple nested tokens & strings");
        a->setIcon(theme()->icon(Core::Constants::ICONHELP));

        a = aTest3 = new QAction(q);
        a->setText("Multinested tokens & strings");
        a->setIcon(theme()->icon(Core::Constants::ICONHELP));

        a = aTest4 = new QAction(q);
        a->setText("Tokens in table");
        a->setIcon(theme()->icon(Core::Constants::ICONHELP));

        a = aTest5 = new QAction(q);
        a->setText("Multinested tokens in table");
        a->setIcon(theme()->icon(Core::Constants::ICONHELP));

        a = aTest6 = new QAction(q);
        a->setText("Read prescription file");
        a->setIcon(theme()->icon(Core::Constants::ICONHELP));
    }

    void connectActions()
    {
//        QObject::connect(ui->viewResult, SIGNAL(clicked()), q, SLOT(analyzeRawSource()));
//        QObject::connect(ui->outputToRaw, SIGNAL(clicked()), q, SLOT(outputToRaw()));
    }

    void createToolBar()
    {
        _toolBar = new QToolBar(q);
        _toolBar->setFocusPolicy(Qt::ClickFocus);
        if (!Utils::isReleaseCompilation()) {
            QToolButton *scenariTester = new QToolButton(q);
            scenariTester->setIcon(theme()->icon(Core::Constants::ICONHELP));
            scenariTester->setToolButtonStyle(Qt::ToolButtonIconOnly);
            scenariTester->setPopupMode(QToolButton::InstantPopup);
            scenariTester->addAction(aTest1);
            scenariTester->addAction(aTest2);
            scenariTester->addAction(aTest3);
            scenariTester->addAction(aTest4);
            scenariTester->addAction(aTest5);
            scenariTester->addAction(aTest6);
            scenariTester->setDefaultAction(aTest1);
            _toolBar->addWidget(scenariTester);
        }
        // Add options action
        Core::Command *cmd;
        cmd = actionManager()->command(Constants::A_PADTOOLS_VIEWOUTPUT);
        _toolBar->addAction(cmd->action());

        cmd = actionManager()->command(Constants::A_PADTOOLS_SHOWSOURCE);
        _toolBar->addAction(cmd->action());

        ui->toolbarLayout->addWidget(_toolBar);
        aTest1->trigger();
    }

    void manageModelAndView()
    {
        // Create TokenModel
        _tokenModel = new TokenModel(q);
        ui->treeView->setModel(_tokenModel);
        ui->treeView->setItemDelegate(new Utils::HtmlDelegate(q));
        ui->treeView->setUniformRowHeights(false);
    #if QT_VERSION < 0x050000
        ui->treeView->header()->setResizeMode(0, QHeaderView::Stretch);
    #else
        // Qt5
        ui->treeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    #endif
        QObject::connect(_tokenModel, SIGNAL(modelReset()), q, SLOT(expandTokenTreeView()));
    }

    void createPadDocument()
    {
        // Create PadDocument for the editor
        _padForEditor = new PadDocument();
        ui->dropTextEditor->setPadDocument(_padForEditor);
        // Create PadDocument for the viewer (showing the token replacement result)
        _padForViewer = new PadDocument();
    }

    // Show the raw source of the document + output
    void switchToRawSourceEdition()
    {
        ui->dropTextEditor->setVisible(false);
        ui->rawSource->setVisible(true);
    }

    // Show the text editor of the document + output
    void switchToDropTextEdition()
    {
        ui->dropTextEditor->setVisible(true);
        ui->rawSource->setVisible(false);
    }

public:
    PadWriterContext *_context;
    Ui::PadWriter *ui;
    TokenModel *_tokenModel;
    QAction *aTest1, *aTest2, *aTest3, *aTest4, *aTest5, *aTest6; // actions used to test different rawsource scenari
    PadDocument *_padForEditor, *_padForViewer;
    QToolBar *_toolBar;

private:
    PadWriter *q;
};
}  // Internal
}  // PadTools

PadWriterContext::PadWriterContext(PadWriter *w) :
    Core::IContext(w)
{
    setObjectName("PadWriterContext");
    setWidget(w);
    setContext(Core::Context(PadTools::Constants::C_PADTOOLS_PLUGINS));
}

/** Creates a PadTools::Internal::PadWriter in text edition mode */
PadWriter::PadWriter(QWidget *parent) :
    Core::IPadWriter(parent),
    d(new Internal::PadWriterPrivate(this))
{
    d->createUi();
    d->createActions();
    d->connectActions();
    d->createToolBar();
    d->registerContext();

    d->manageModelAndView();
    d->createPadDocument();

    d->switchToDropTextEdition();

    expandTokenTreeView();

//    connect(d->ui->dropTextEditor->textEdit(), SIGNAL(cursorPositionChanged()), this, SLOT(dropTextEditorCursorChanged()));
//    connect(d->ui->rawSource->textEdit(), SIGNAL(cursorPositionChanged()), this, SLOT(rawSourceCursorChanged()));
}

/** Dtor */
PadWriter::~PadWriter()
{
    d->removeContext();
    if (d) {
        delete d;
        d = 0;
    }
}

/** Set the plain text raw source to analyze */
void PadWriter::setPlainTextSource(const QString &plainText)
{
    d->ui->rawSource->setPlainText(plainText);
    analyzeRawSource();
}

/** Set the rich text (HTML) raw source to analyze */
void PadWriter::setHtmlSource(const QString &html)
{
    d->ui->rawSource->setHtml(html);
    analyzeRawSource();
}

/** Filter available namespaces with the \e tokenNamespace */
void PadWriter::filterTokenPool(const QString &tokenNamespace)
{
    Q_UNUSED(tokenNamespace)
    // TODO: here
}

/** Filter available namespaces with the \e tokenNamespaces */
void PadWriter::filterTokenPool(const QStringList &tokenNamespaces)
{
    Q_UNUSED(tokenNamespaces)
    // TODO: here
}

/**
 * Returns the token output constructed document to plain text
 * (does include the tokens but their values)
 */
QString PadWriter::outputToPlainText() const
{
    return d->ui->dropTextEditor->toPlainText();
}

/**
 * Returns the token output constructed document to rich text
 * (does include the tokens but their values)
 */
QString PadWriter::outputToHtml() const
{
    return d->ui->dropTextEditor->toHtml();
}

/** Returns the token raw source document to plain text */
QString PadWriter::rawSourceToPlainText() const
{
    return d->ui->rawSource->toPlainText();
}

/** Returns the token raw source document to rich text */
QString PadWriter::rawSourceToHtml() const
{
    return d->ui->rawSource->toHtml();
}

/**
 * \internal
 * In \b debug \b mode some "testing scenari" are available through a button,
 * apply the selected scenrio
 */
void PadWriter::changeRawSourceScenario(QAction *a)
{
    QString source;
    if (a == d->aTest1) {
        source = "<p>"
                "<b><center>Simple token test</center></b></p><p>"
                "&nbsp;&nbsp;* To{{~test.A~) no before text}}ken D: {{\"...~test.D~...\"}}<br />"
//                "&nbsp;&nbsp;* Null token: ({{All this text ~NULL~ should not appear in the output}})<br />"
                "&nbsp;&nbsp;* Token D without 'after conditional text':{{ ~test.D~}}<br />"
                "&nbsp;&nbsp;* Token D without 'before conditional text': {{~test.D~. }}<br />"
                "&nbsp;&nbsp;* Long token A: {{this text should appear in the output document, <u>including the core value</u> \"<b>~test.A~</b>\" (in bold) as defined in the <span style=' text-decoration: underline; color:#ff00ff;'>TokenModel</span>.}}<br />"
                "&nbsp;&nbsp;* HTML Token:<br />"
                "&nbsp;&nbsp;&nbsp;&nbsp;* Result should be \" <u><b>htmlToken</b></u> \"<br />"
                "&nbsp;&nbsp;&nbsp;&nbsp;* Result should be {{\" <u>~test.HTMLTOKEN~</u> \"}}<br />"
                "</p>"
                ;
    } else if (a == d->aTest2) {
        source = "<p>"
                "<b><center>Nested tokens test</center></b></p><p>"
                "&nbsp;&nbsp;* Testing tokens:<br />"
                "&nbsp;&nbsp;&nbsp;&nbsp;* {{\"Token B: (~test.B~) {{[[Token {{this text ~NULL~ should not appear in output}}C: ~test.C~]]}}.\"}}<br />"
                "&nbsp;&nbsp;* Result should be:<br />"
                "&nbsp;&nbsp;&nbsp;&nbsp;* \"Token B: (This is B) [[Token C: This is C]].\"<br />"
//                "{{ _{{‘‘nestedB’’~test.B~}}C_ ~test.C~ _C_}}<br />"
//                " 10 chars {{ _D_ ~test.D~ _D_}}<br />"
                ;
    } else if (a == d->aTest3) {
        source = "<p><b>{{(<span style='text-decoration: underline; color:#ff00ff;'>A:</span> ~test.A~)}}. Some strings.</b><br />"
                "{{(<span style='text-decoration: underline; color:#0000ff;'>D:</span> {{[C: ~test.C~]}} ~test.D~)}}<br/>"
                "{{(B: ~test.B~)}}<br />";
    } else if (a == d->aTest4) {
    source = "<p><b>Testing tokens inside a table</b><br />"
            "<table border=1>"
            "<tr>"
            "  <td>{{_<span style=' text-decoration: underline; color:#ff00ff;'>A_</span> ~test.A~ _A_}} 10 chars </td>"
            "</tr>"
            "<tr>"
            "  <td> 10 chars {{ _D_ ~test.D~ _D_}}</td>"
            "</tr>"
            "</table>"
            "</p>";
    } else if (a == d->aTest5) {
        source = "<p><b>Testing nested tokens inside a table</b><br />"
                "<table border=1>"
                "<tr>"
                "  <td>{{<span style=' text-decoration: underline; color:#ff00ff;'>_A_</span> ~test.A~ _A_}} 10 chars </td>"
                "</tr>"
                "<tr>"
                "  <td> 10 chars {{ _D_ ~test.D~ _D_}}</td>"
                "</tr>"
                "<tr>"
                "  <td>Two nested: {{ _D_ ~test.D~ _{{Nested C ~test.C~... }}D_}}</td>"
                "</tr>"
                "<tr>"
                "  <td>Multi-nested: {{ _D_ ~test.D~ _{{Nested C ~test.C~..{{//~test.A~//}}.. }}D_}}</td>"
                "</tr>"
                "</table>"
                "</p>";
    } else if (a == d->aTest6) {
        source = Utils::readTextFile(settings()->path(Core::ISettings::BundleResourcesPath) + "/textfiles/prescription/padtoolsstyle_fr.txt");
    }

//    if (d->_padForEditor) {
//        delete d->_padForEditor;
//        d->_padForEditor = 0;
//    }
    d->ui->rawSource->setHtml(source);
    analyzeRawSource();
}

/**
 * \internal
 * Expand all root branches of the token treeView
 */
void PadWriter::expandTokenTreeView()
{
    for(int i=0; i < d->_tokenModel->rowCount(); ++i)
        d->ui->treeView->expand(d->_tokenModel->index(i,0));
}

/**
 * \internal
 * Analyze the current token raw source document
 * and create the tokened output document
 */
void PadWriter::analyzeRawSource()
{
    // clear PadDocument && views
    d->_padForEditor->clear();

    // Start analyze && token replacement (for the editor)
    PadAnalyzer().analyze(d->ui->rawSource->document(), d->_padForEditor);
    d->_padForEditor->setTokenModel(d->_tokenModel);
    d->_padForEditor->toOutput(d->_tokenModel->tokenPool(), PadFragment::ReplaceWithTokenDisplayName);
    // TODO: manage PadAnalyzer errors
//    d->ui->dropTextEditor->setDocument(d->_padForEditor->outputDocument());

    // Start analyze && token replacement (for the viewer)
    PadAnalyzer().analyze(d->ui->rawSource->document(), d->_padForViewer);
    d->_padForViewer->setTokenModel(d->_tokenModel);
    d->_padForViewer->toOutput(d->_tokenModel->tokenPool(), PadFragment::ReplaceWithTokenValue);
    // TODO: manage PadAnalyzer errors
    d->ui->outputTextEditor->setDocument(d->_padForViewer->outputDocument());
}

/**
 * \internal
 * Analyze the current token edited document (in the rich texteditor)
 * and create the tokened raw source document
 */
void PadWriter::outputToRaw()
{
    // Start raw source composition
    d->_padForEditor->toRaw();
}

/**
 * \internal
 * Switch to the rich text tokened edition (using the drag'n drop feature).
 * This is the default viewing mode of the pad writer.
 * \sa onShowSourceRequested()
 */
void PadWriter::onViewOutputRequested()
{
    analyzeRawSource();
}

/**
 * \internal
 * Switch to the raw source tokened edition (using the drag'n drop feature).
 * \sa onViewOutputRequested()
 */
void PadWriter::onShowSourceRequested()
{
    outputToRaw();
}
