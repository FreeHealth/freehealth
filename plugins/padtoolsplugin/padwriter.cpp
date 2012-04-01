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
  \class PadTools::PadWriter
*/

#include "padwriter.h"
#include "tokenmodel.h"
#include "constants.h"
#include "pad_analyzer.h"
#include "pad_highlighter.h"

#include <coreplugin/icore.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/ipadtools.h>
#include <coreplugin/itheme.h>
#include <coreplugin/uniqueidmanager.h>
#include <coreplugin/actionmanager/actionmanager.h>

#include <utils/log.h>

#include "ui_padwriter.h"

#include <QDebug>

using namespace PadTools;
using namespace Internal;

static inline Core::UniqueIDManager *uid() { return Core::ICore::instance()->uniqueIDManager(); }
static inline Core::ActionManager *actionManager() { return Core::ICore::instance()->actionManager(); }
static inline Core::ITheme *theme() { return Core::ICore::instance()->theme(); }
static inline Core::IPadTools *padTools() {return Core::ICore::instance()->padTools();}

static void setTokenFormat(int s, int e, QTextDocument *doc, QList<QTextCharFormat> &formats)
{
    QTextCursor cursor(doc);
     int count = e-s;
     for(int i=0; i < count; ++i) {
         cursor.setPosition(s + i);
         cursor.setPosition(s + i + 1, QTextCursor::KeepAnchor);
         formats << cursor.charFormat();
         cursor.setCharFormat(Constants::setTokenCharFormat(cursor.charFormat()));
     }
}

static void removeTokenFormat(int s, int e, QTextDocument *doc, QList<QTextCharFormat> &formats)
{
    QTextCursor cursor(doc);
    int count = e-s;
    for(int i=0; i < formats.count(); ++i) {
        cursor.setPosition(s + i);
        cursor.setPosition(s + i + 1, QTextCursor::KeepAnchor);
        cursor.setCharFormat(formats.at(i));
    }
    formats.clear();
}

namespace PadTools {
namespace Internal {
class PadWriterPrivate
{
public:
    PadWriterPrivate(PadWriter *parent) :
        ui(0),
        m_TokenModel(0),
        m_Pad(0),
        m_LastHoveredItem(0),
        q(parent)
    {}

    void createActions(QToolButton *button)
    {
        // Add options action
        QList<int> context = QList<int>() << Core::Constants::C_GLOBAL_ID;
        QAction *a;
        Core::Command *cmd;

        aFollowCursor = a = new QAction(q);
        a->setObjectName("PadWriter.aFollowCursor");
    //    a->setIcon(theme()->icon(icon));
        a->setCheckable(true);
        a->setChecked(false);
        cmd = actionManager()->registerAction(a, a->objectName(), context);
        cmd->setTranslations(Constants::FOLLOW_CURSOR_IN_RESULT_OUTPUT, Constants::FOLLOW_CURSOR_IN_RESULT_OUTPUT, Constants::PADWRITER_TRANS_CONTEXT);
        button->addAction(cmd->action());

        aAutoUpdate = a = new QAction(q);
        a->setObjectName("PadWriter.aAutoUpdate");
    //    a->setIcon(theme()->icon(icon));
        a->setCheckable(true);
        a->setChecked(false);
        cmd = actionManager()->registerAction(a, a->objectName(), context);
        cmd->setTranslations(Constants::AUTO_UPDATE_RESULT, Constants::AUTO_UPDATE_RESULT, Constants::PADWRITER_TRANS_CONTEXT);
        button->addAction(cmd->action());

        aSetDefaultValues = a = new QAction(q);
        a->setObjectName("PadWriter.aSetDefaultValues");
    //    a->setIcon(theme()->icon(icon));
        a->setCheckable(true);
        a->setChecked(false);
        cmd = actionManager()->registerAction(a, a->objectName(), context);
        cmd->setTranslations(Constants::SET_TEST_VALUE_TO_TOKENS, Constants::SET_TEST_VALUE_TO_TOKENS, Constants::PADWRITER_TRANS_CONTEXT);
        button->addAction(cmd->action());
    }

public:
    Ui::PadWriter *ui;
    TokenModel *m_TokenModel;
    QAction *aFollowCursor, *aAutoUpdate, *aSetDefaultValues;
    QAction *aTest1, *aTest2, *aTest3; // actions used to test different rawsource scenari
    PadDocument *m_Pad;
//    PadItem *m_LastHoveredItem, *m_LastFollowedItem; // should not be deleted
    PadItem *m_LastHoveredItem ; // should not be deleted
    PadFragment *m_LastFollowedItem; // should not be deleted
    QList<QTextCharFormat> m_LastHoveredItemCharFormats, m_LastFollowedItemCharFormats;

private:
    PadWriter *q;
};
}  // Internal
}  // PadTools

PadWriter::PadWriter(QWidget *parent) :
    QWidget(parent),
    d(new Internal::PadWriterPrivate(this))
{
    d->ui = new Internal::Ui::PadWriter;
    d->ui->setupUi(this);

    // Create TokenModel
    d->m_TokenModel = new TokenModel(this);
    d->ui->treeView->setModel(d->m_TokenModel);
    d->ui->treeView->header()->setResizeMode(0, QHeaderView::Stretch);

    // Create PadHighlighter
//    PadHighlighter *highlight = new PadHighlighter(d->ui->rawSource->textEdit());

    // Add options action
    d->createActions(d->ui->optionsButton);
    connect(d->aFollowCursor, SIGNAL(triggered(bool)), this, SLOT(setFollowCursorInResultOutput(bool)));
    connect(d->aAutoUpdate, SIGNAL(triggered(bool)), this, SLOT(setAutoUpdateOfResult(bool)));
    connect(d->aSetDefaultValues, SIGNAL(triggered(bool)), this, SLOT(setTestValues(bool)));
    connect(d->ui->viewResult, SIGNAL(clicked()), this, SLOT(analyseRawSource()));
    connect(d->ui->viewError, SIGNAL(clicked()), this, SLOT(viewErrors()));

    // TEST
    setFollowCursorInResultOutput(true);
    QAction *a;
    a = d->aTest1 = new QAction(this);
    a->setText("Test raw source 1");
    a->setToolTip("Two tokens, some strings, no nested tokens");
    d->ui->scenari->addAction(a);

    a = d->aTest2 = new QAction(this);
    a->setText("Test raw source 2");
    a->setToolTip("Simple nested tokens with extra strings");
    d->ui->scenari->addAction(a);

    a = d->aTest3 = new QAction(this);
    a->setText("Test raw source 1");
    a->setToolTip("Multinested tokens with extra strings");
    d->ui->scenari->addAction(a);

    connect(d->ui->scenari, SIGNAL(triggered(QAction*)), this, SLOT(changeRawSourceScenario(QAction*)));
    d->ui->scenari->setDefaultAction(d->aTest1);
    d->aTest1->trigger();

    // END TEST
}

PadWriter::~PadWriter()
{
    if (d) {
        delete d;
        d = 0;
    }
}
void PadWriter::changeRawSourceScenario(QAction *a)
{
    QString source;
    if (a == d->aTest1) {
        source = "<p><b>^$ <span style=' text-decoration: underline; color:#0000ff;'>before </span> 'a'  ~A~  after 'a'$^ qdsfsdqf qdf qsdf </b><br />"
                 "^$ <span style=' text-decoration: underline; color:#0000ff;'>before</span> 'b' ~B~ after 'b'$^<br />";
    } else if (a == d->aTest2) {
        source = "<p><b>^$ <span style=' text-decoration: underline; color:#0000ff;'>before </span> 'a'  ~A~  after 'a'$^ qdsfsdqf qdf qsdf </b><br />"
                 "^$ <span style=' text-decoration: underline; color:#0000ff;'>before</span> ^$ Before nested C ~C~ After nested C $^ 'b' ~B~ after 'b'$^<br />";
    } else if (a == d->aTest3) {
    } else if (a == d->aTest2) {
        source = "<p><b>^$ <span style=' text-decoration: underline; color:#0000ff;'>before </span> 'a'  ~A~  after 'a'$^ qdsfsdqf qdf qsdf </b><br />"
                 "^$ <span style=' text-decoration: underline; color:#0000ff;'>before</span> ^$ Before nested C ~C~ After ^$ Before D ~D~ After D $^nested C $^ 'b' ~B~ after 'b'$^<br />";
    }
    d->ui->rawSource->setHtml(source);
    analyseRawSource();
}

QString PadWriter::htmlResult() const
{
    return d->ui->wysiwyg->toHtml();
}

QString PadWriter::rawSource() const
{
    return d->ui->rawSource->toPlainText();
}

void PadWriter::analyseRawSource()
{
    QList<Core::PadAnalyzerError> errors;
    if (d->m_Pad) {
        delete d->m_Pad;
        d->m_Pad = 0;
    }
    d->m_Pad = PadAnalyzer().analyze(d->ui->rawSource->document());
    d->ui->wysiwyg->document()->clear();
    d->m_Pad->run(d->m_TokenModel->tokens(), d->ui->rawSource->document(), d->ui->wysiwyg->document());
    d->ui->wysiwyg->setPadDocument(d->m_Pad);

    d->ui->listWidgetErrors->clear();
    foreach (const Core::PadAnalyzerError &error, errors) {
        switch (error.errorType()) {
        case Core::PadAnalyzerError::Error_UnexpectedChar:
            d->ui->listWidgetErrors->addItem(tr("Unexpected '%1' found at line %2 and pos %3").arg(error.errorTokens()["char"].toString()).arg(error.line()).arg(error.pos()));
            break;
        case Core::PadAnalyzerError::Error_CoreDelimiterExpected:
            d->ui->listWidgetErrors->addItem(tr("Expected '%1' at line %2 and pos %3").arg(error.errorTokens()["char"].toString()).arg(error.line()).arg(error.pos()));
            break;
        }
    }
}

void PadWriter::viewErrors()
{
}

void PadWriter::setFollowCursorInResultOutput(bool state)
{
    if (state) {
        connect(d->ui->rawSource->textEdit(), SIGNAL(cursorPositionChanged()), this, SLOT(findCursorPositionInOutput()));
    } else {
        disconnect(d->ui->rawSource->textEdit(), SIGNAL(cursorPositionChanged()), this, SLOT(findCursorPositionInOutput()));
    }
}

void PadWriter::findCursorPositionInOutput()
{
    if (!d->m_Pad)
        return;

    // Find corresponding fragment Id
    const int cursorPos = d->ui->rawSource->textEdit()->textCursor().position();
//    PadItem *item = d->m_Pad->padItemForSourcePosition(cursorPos);
    PadFragment *item = d->m_Pad->padFragmentForSourcePosition(cursorPos);
    if (!item)
        return;
    QTextDocument *doc = d->ui->wysiwyg->document();
    if (!item) {
        if (d->m_LastFollowedItem) {
            removeTokenFormat(d->m_LastFollowedItem->outputStart(), d->m_LastFollowedItem->outputEnd(), doc, d->m_LastFollowedItemCharFormats);
            d->m_LastFollowedItem = 0;
        }
        return;
    }

    if (d->m_LastFollowedItem) {
        if (d->m_LastFollowedItem == item)
            return;
        removeTokenFormat(d->m_LastFollowedItem->outputStart(), d->m_LastFollowedItem->outputEnd(), doc, d->m_LastFollowedItemCharFormats);
        d->m_LastFollowedItem = item;
    } else {
        d->m_LastFollowedItem = item;
    }
    setTokenFormat(d->m_LastFollowedItem->outputStart(), d->m_LastFollowedItem->outputEnd(), doc, d->m_LastFollowedItemCharFormats);
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

void PadWriter::setTestValues(bool state)
{
    analyseRawSource();
}

