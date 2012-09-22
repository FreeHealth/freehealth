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
 *  Main Developers : Eric Maeker <eric.maeker@gmail.com>,                *
 *                    Guillaume Denry <guillaume.denry@gmail.com>          *
 *  Contributors :                                                         *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/

/**
 * \class PadTools::PadWriter
 * To avoid dependencies to the pad plugin, the PadTools::PadWriter can be created
 * by the Core::IPadTools::createWriter().
*/

#include "padwriter.h"
#include "tokenmodel.h"
#include "constants.h"
#include "pad_analyzer.h"
//#include "pad_highlighter.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/ipadtools.h>
#include <coreplugin/itheme.h>
#include <coreplugin/actionmanager/actionmanager.h>

#include <utils/log.h>
#include <utils/global.h>

#include <QTimer>
#include <QToolBar>

#include "ui_padwriter.h"

#include <QDebug>

using namespace PadTools;
using namespace Internal;

static inline Core::ActionManager *actionManager() { return Core::ICore::instance()->actionManager(); }
static inline Core::ITheme *theme() { return Core::ICore::instance()->theme(); }
static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline Core::IPadTools *padTools() {return Core::ICore::instance()->padTools();}

namespace PadTools {
namespace Internal {
class PadWriterPrivate
{
public:
    PadWriterPrivate(PadWriter *parent) :
        ui(0),
        _tokenModel(0),
        _pad(0),
        _followedItem(0),
        _cursorHighlighTimer(0),
        _toolBar(0),
        q(parent)
    {
        _followedCharFormat.setUnderlineColor(QColor(Qt::cyan));
        _followedCharFormat.setUnderlineStyle(QTextCharFormat::WaveUnderline);

        _cursorHighlighTimer = new QTimer(q);
        _cursorHighlighTimer->setSingleShot(true);
    }

    void createActions(QToolButton *button)
    {
        // Add options action
        Core::Context context(Core::Constants::C_GLOBAL);
        QAction *a;
        Core::Command *cmd;

        aFindCursor = a = new QAction(q);
        a->setObjectName("PadWriter.aFindCursor");
    //    a->setIcon(theme()->icon(icon));
        a->setCheckable(true);
        a->setChecked(false);
        cmd = actionManager()->registerAction(a, Core::Id(a->objectName()), context);
        cmd->setTranslations(Constants::FIND_CURSOR_IN_RESULT, Constants::FIND_CURSOR_IN_RESULT, Constants::PADWRITER_TRANS_CONTEXT);
        button->addAction(cmd->action());

        aAutoUpdate = a = new QAction(q);
        a->setObjectName("PadWriter.aAutoUpdate");
    //    a->setIcon(theme()->icon(icon));
        a->setCheckable(true);
        a->setChecked(false);
        cmd = actionManager()->registerAction(a, Core::Id(a->objectName()), context);
        cmd->setTranslations(Constants::AUTO_UPDATE_RESULT, Constants::AUTO_UPDATE_RESULT, Constants::PADWRITER_TRANS_CONTEXT);
        button->addAction(cmd->action());

        aSetDefaultValues = a = new QAction(q);
        a->setObjectName("PadWriter.aSetDefaultValues");
    //    a->setIcon(theme()->icon(icon));
        a->setCheckable(true);
        a->setChecked(false);
        cmd = actionManager()->registerAction(a, Core::Id(a->objectName()), context);
        cmd->setTranslations(Constants::SET_TEST_VALUE_TO_TOKENS, Constants::SET_TEST_VALUE_TO_TOKENS, Constants::PADWRITER_TRANS_CONTEXT);
        button->addAction(cmd->action());

        // TEST
        a = aTest1 = new QAction(q);
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
        QObject::connect(aFindCursor, SIGNAL(triggered()), q, SLOT(highlightCursor()));
        QObject::connect(aAutoUpdate, SIGNAL(triggered(bool)), q, SLOT(setAutoUpdateOfResult(bool)));
        QObject::connect(aSetDefaultValues, SIGNAL(triggered(bool)), q, SLOT(setTestValues(bool)));
        QObject::connect(ui->viewResult, SIGNAL(clicked()), q, SLOT(analyseRawSource()));
        QObject::connect(ui->viewError, SIGNAL(clicked()), q, SLOT(viewErrors()));
        QObject::connect(ui->findCursor, SIGNAL(clicked()), q, SLOT(highlightCursor()));
        QObject::connect(ui->outputToRaw, SIGNAL(clicked()), q, SLOT(outputToRaw()));
    }

    void createToolBar()
    {
        _toolBar = new QToolBar(q);
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
        ui->toolbarLayout->addWidget(_toolBar);
        aTest1->trigger();
    }

public:
    Ui::PadWriter *ui;
    TokenModel *_tokenModel;
    QAction *aFindCursor, *aAutoUpdate, *aSetDefaultValues;
    QAction *aTest1, *aTest2, *aTest3, *aTest4, *aTest5, *aTest6; // actions used to test different rawsource scenari
    PadDocument *_pad;
    PadFragment *_followedItem; // should not be deleted
    QList<QTextCharFormat> m_LastHoveredItemCharFormats, _followedItemCharFormats;
    QTextCharFormat _followedCharFormat;
    QTimer *_cursorHighlighTimer;
    QToolBar *_toolBar;

private:
    PadWriter *q;
};
}  // Internal
}  // PadTools

PadWriter::PadWriter(QWidget *parent) :
    Core::IPadWriter(parent),
    d(new Internal::PadWriterPrivate(this))
{
    d->ui = new Internal::Ui::PadWriter;
    d->ui->setupUi(this);

    // Create TokenModel
    d->_tokenModel = new TokenModel(this);
    d->ui->treeView->setModel(d->_tokenModel);
    d->ui->treeView->hideColumn(1);
    expandTokenTreeView();
    connect(d->_tokenModel, SIGNAL(modelReset()), this, SLOT(expandTokenTreeView()));

    // Create PadDocument object
    d->_pad = new PadDocument();
    d->ui->wysiwyg->setPadDocument(d->_pad);
    connect(d->_pad, SIGNAL(padFragmentChanged(PadFragment*)), this, SLOT(onPadFragmentChanged(PadFragment*)));

    // Create PadHighlighter
//    PadHighlighter *highlight = new PadHighlighter(d->ui->rawSource->textEdit());

    // Add options action
    d->createActions(d->ui->optionsButton);
    d->connectActions();
    d->createToolBar();

    // TEST

    connect(d->ui->wysiwyg->textEdit(), SIGNAL(cursorPositionChanged()), this, SLOT(wysiwygCursorChanged()));
    connect(d->ui->rawSource->textEdit(), SIGNAL(cursorPositionChanged()), this, SLOT(rawSourceCursorChanged()));
//    d->ui->wysiwyg->textEdit()->setCursorWidth(3);
//    d->ui->rawSource->textEdit()->setCursorWidth(3);
    // END TEST
}

PadWriter::~PadWriter()
{
    if (d) {
        delete d;
        d = 0;
    }
}

void PadWriter::setPlainTextSource(const QString &plainText)
{
    d->ui->rawSource->setPlainText(plainText);
    analyseRawSource();
}

void PadWriter::setHtmlSource(const QString &html)
{
    d->ui->rawSource->setHtml(html);
    analyseRawSource();
}

void PadWriter::filterTokenPool(const QString &tokenNamespace)
{
    // TODO: here
}

void PadWriter::filterTokenPool(const QStringList &tokenNamespaces)
{
    // TODO: here
}

QString PadWriter::outputToPlainText() const
{
    return d->ui->wysiwyg->toPlainText();
}

QString PadWriter::outputToHtml() const
{
    return d->ui->wysiwyg->toHtml();
}

QString PadWriter::rawSourceToPlainText() const
{
    return d->ui->rawSource->toPlainText();
}

QString PadWriter::rawSourceToHtml() const
{
    return d->ui->rawSource->toHtml();
}

void PadWriter::wysiwygCursorChanged()
{
    QTextCursor c = d->ui->wysiwyg->textEdit()->textCursor();
    d->ui->ouputPos->setValue(c.position());
}

void PadWriter::rawSourceCursorChanged()
{
    QTextCursor c = d->ui->rawSource->textEdit()->textCursor();
    d->ui->rawPos->setValue(c.position());
}

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

//    if (d->_pad) {
//        delete d->_pad;
//        d->_pad = 0;
//    }
    d->ui->rawSource->setHtml(source);
    analyseRawSource();
}

void PadWriter::expandTokenTreeView()
{
    for(int i=0; i < d->_tokenModel->rowCount(); ++i)
        d->ui->treeView->expand(d->_tokenModel->index(i,0));
}

void PadWriter::analyseRawSource()
{
    // clear PadDocument && views
    QList<Core::PadAnalyzerError> errors;
    d->_pad->clear();

    // Start analyze && token replacement
    PadAnalyzer().analyze(d->ui->rawSource->document(), d->_pad);
    d->_pad->setTokenModel(d->_tokenModel);
//    d->_pad->run(d->_tokenModel->tokens());
    d->_pad->toOutput(d->_tokenModel->tokenPool());

    d->ui->listWidgetErrors->clear();
    foreach (const Core::PadAnalyzerError &error, errors) {
        switch (error.errorType()) {
        case Core::PadAnalyzerError::Error_UnexpectedChar:
            d->ui->listWidgetErrors->addItem(tr("Unexpected '%1' found at pos %2").arg(error.errorTokens()["char"].toString()).arg(error.pos()));
            break;
        case Core::PadAnalyzerError::Error_CoreDelimiterExpected:
            d->ui->listWidgetErrors->addItem(tr("Expected '%1' at pos %2").arg(error.errorTokens()["char"].toString()).arg(error.pos()));
            break;
        }
    }
}

void PadWriter::outputToRaw()
{
    // clear PadDocument && views
//    QList<Core::PadAnalyzerError> errors;
//    d->_pad->clear();

    // Start raw source composition
    d->_pad->toRaw();

//    PadAnalyzer().analyze(d->ui->rawSource->document(), d->_pad);
//    d->_pad->setTokenModel(d->_tokenModel);
//    d->_pad->run(d->_tokenModel->tokens());

//    d->ui->listWidgetErrors->clear();
//    foreach (const Core::PadAnalyzerError &error, errors) {
//        switch (error.errorType()) {
//        case Core::PadAnalyzerError::Error_UnexpectedChar:
//            d->ui->listWidgetErrors->addItem(tr("Unexpected '%1' found at pos %2").arg(error.errorTokens()["char"].toString()).arg(error.pos()));
//            break;
//        case Core::PadAnalyzerError::Error_CoreDelimiterExpected:
//            d->ui->listWidgetErrors->addItem(tr("Expected '%1' at pos %2").arg(error.errorTokens()["char"].toString()).arg(error.pos()));
//            break;
//        }
//    }
}

void PadWriter::viewErrors()
{
}

void PadWriter::highlightCursor()
{
//    int cursorPos;
//    // highlight cursor in output textdocument
//    cursorPos = d->ui->wysiwyg->textEdit()->textCursor().position();
//    PadFragment *outputFragment = d->_pad->padFragmentForOutputPosition(cursorPos);

//    // highlight cursor in source textdocument
//    cursorPos = d->ui->rawSource->textEdit()->textCursor().position();
//    PadFragment *rawFragment = d->_pad->padFragmentForSourcePosition(cursorPos);

//    QTextDocument *doc = d->ui->wysiwyg->document();
//    if (!item) {
//        if (d->_followedItem) {
//            Constants::removePadFragmentFormat("Follow", doc, d->_followedItemCharFormats);
//            d->_followedItem = 0;
//        }
//        return;
//    }

//    if (d->_followedItem) {
//        if (d->_followedItem == item)
//            return;
//        Constants::removePadFragmentFormat("Follow", doc, d->_followedItemCharFormats);
//        d->_followedItem = item;
//    } else {
//        d->_followedItem = item;
//    }
//    Constants::setPadFragmentFormat("Follow", d->_followedItem->outputStart(), d->_followedItem->outputEnd(), doc, d->_followedItemCharFormats, d->_followedCharFormat);

    // set timer
}

void PadWriter::findCursorPositionInOutput()
{
    if (!d->_pad)
        return;

//    // Find corresponding fragment Id
//    const int cursorPos = d->ui->rawSource->textEdit()->textCursor().position();
////    PadItem *item = d->_pad->padItemForSourcePosition(cursorPos);
//    PadFragment *item = d->_pad->padFragmentForSourcePosition(cursorPos);
//    QTextDocument *doc = d->ui->wysiwyg->document();
//    if (!item) {
//        if (d->_followedItem) {
//            Constants::removePadFragmentFormat("Follow", doc, d->_followedItemCharFormats);
//            d->_followedItem = 0;
//        }
//        return;
//    }

//    if (d->_followedItem) {
//        if (d->_followedItem == item)
//            return;
//        Constants::removePadFragmentFormat("Follow", doc, d->_followedItemCharFormats);
//        d->_followedItem = item;
//    } else {
//        d->_followedItem = item;
//    }
//    Constants::setPadFragmentFormat("Follow", d->_followedItem->outputStart(), d->_followedItem->outputEnd(), doc, d->_followedItemCharFormats, d->_followedCharFormat);
}

void PadWriter::setAutoUpdateOfResult(bool state)
{
    if (state) {
        connect(d->ui->rawSource->textEdit(), SIGNAL(textChanged()), this, SLOT(analyseRawSource()));
        analyseRawSource();
    } else {
        disconnect(d->ui->rawSource->textEdit(), SIGNAL(textChanged()), this, SLOT(analyseRawSource()));
    }
}

void PadWriter::setTestValues(bool /*state*/)
{
    analyseRawSource();
}

void PadWriter::onPadFragmentChanged(PadFragment *fragment)
{
    if (!d->_followedItem)
        return;
    if (d->_followedItem!=fragment)
        return;
//    // The followed fragment was modified
//    QTextDocument *doc = d->ui->wysiwyg->document();
//    Constants::removePadFragmentFormat("Follow", doc, d->_followedItemCharFormats);
//    Constants::setPadFragmentFormat("Follow", d->_followedItem->outputStart(), d->_followedItem->outputEnd(), doc, d->_followedItemCharFormats, d->_followedCharFormat);
}

//bool PadWriter::event(QEvent *event)
//{
//    if (event->type()==QEvent::ToolTip) {

//    }
//    return Core::IPadWriter::event(event);
//}
