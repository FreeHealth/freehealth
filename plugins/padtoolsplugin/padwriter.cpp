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

public:
    Ui::PadWriter *ui;
    TokenModel *m_TokenModel;
    QAction *aFollowCursor, *aAutoUpdate, *aSetDefaultValues;
    PadDocument *m_Pad;
    PadItem *m_LastHoveredItem; // should not be deleted

private:
    PadWriter *q;
};
}  // Internal
}  // PadTools

PadWriter::PadWriter(QWidget *parent) :
    QWidget(parent),
    d(new Internal::PadWriterPrivate(this))
{
    d->ui = new Ui::PadWriter;
    d->ui->setupUi(this);

    // Create TokenModel
    d->m_TokenModel = new TokenModel(this);
    d->ui->treeView->setModel(d->m_TokenModel);
    d->ui->treeView->header()->setResizeMode(0, QHeaderView::Stretch);

    // Create PadHighlighter
    PadHighlighter *highlight = new PadHighlighter(d->ui->rawSource->textEdit());

    // Add options action
    QList<int> context = QList<int>() << Core::Constants::C_GLOBAL_ID;
    QAction *a;
    Core::Command *cmd;

    d->aFollowCursor = a = new QAction(parent);
    a->setObjectName("PadWriter.aFollowCursor");
//    a->setIcon(theme()->icon(icon));
    a->setCheckable(true);
    a->setChecked(false);
    cmd = actionManager()->registerAction(a, a->objectName(), context);
    cmd->setTranslations(Constants::FOLLOW_CURSOR_IN_RESULT_OUTPUT, Constants::FOLLOW_CURSOR_IN_RESULT_OUTPUT, Constants::PADWRITER_TRANS_CONTEXT);
    connect(cmd->action(), SIGNAL(triggered(bool)), this, SLOT(setFollowCursorInResultOutput(bool)));
    d->ui->optionsButton->addAction(cmd->action());

    d->aAutoUpdate = a = new QAction(parent);
    a->setObjectName("PadWriter.aAutoUpdate");
//    a->setIcon(theme()->icon(icon));
    a->setCheckable(true);
    a->setChecked(false);
    cmd = actionManager()->registerAction(a, a->objectName(), context);
    cmd->setTranslations(Constants::AUTO_UPDATE_RESULT, Constants::AUTO_UPDATE_RESULT, Constants::PADWRITER_TRANS_CONTEXT);
    connect(cmd->action(), SIGNAL(triggered(bool)), this, SLOT(setAutoUpdateOfResult(bool)));
    d->ui->optionsButton->addAction(cmd->action());

    d->aSetDefaultValues = a = new QAction(parent);
    a->setObjectName("PadWriter.aSetDefaultValues");
//    a->setIcon(theme()->icon(icon));
    a->setCheckable(true);
    a->setChecked(false);
    cmd = actionManager()->registerAction(a, a->objectName(), context);
    cmd->setTranslations(Constants::SET_TEST_VALUE_TO_TOKENS, Constants::SET_TEST_VALUE_TO_TOKENS, Constants::PADWRITER_TRANS_CONTEXT);
    connect(cmd->action(), SIGNAL(triggered(bool)), this, SLOT(setTestValues(bool)));
    d->ui->optionsButton->addAction(cmd->action());

    // Connect buttons
    connect(d->ui->viewResult, SIGNAL(clicked()), this, SLOT(analyseRawSource()));
    connect(d->ui->viewError, SIGNAL(clicked()), this, SLOT(viewErrors()));

    // Catch events for all ui widgets
//    setMouseTracking(true);
    d->ui->wysiwyg->installEventFilter(this);
    d->ui->wysiwyg->setAttribute(Qt::WA_Hover);

    // TEST
    setFollowCursorInResultOutput(true);
    d->ui->rawSource->setPlainText(
                "<$before 'a'  ~A~  after 'a'$>\n"
                "<$before 'b'  ~B~  after 'b'$>\n"
                "<$before 'c'  ~C~  after 'c'$>\n"
                "<$before 'html'  ~HTMLTOKEN~  after 'html'$>\n"
                "\n\n"
                "---------- TESTING NESTED ----------\n"
                "<$ba <$bb ~B~ ab$> ~A~  aa$>  -----> ba bb B ab A aa\n"
                "<$bb ~B~ ab <$ba ~A~  aa$>$>  -----> bb B ab ba A aa\n"
                "<$<$bb ~B~ ab ba ~NULL~  aa$>$>  -----> \n"
                "<$bb ~B~ ab <$ba ~NULL~  aa$>$>  -----> bb B ab\n"
                "\n"
                "<$ab ~A~ aa <$ba <$nnn ~NULL~ nnn <$ac ~C~ bc$>$> ~B~  aa$>$>  -----> ab A aa ba B aa\n"
                "<$ab ~A~ aa <$ba <$<$nnn ~NULL~ nnn $> ac ~C~ bc $> ~B~  aa$>$>  -----> ab A aa ba B aa\n"
                "\n"
                "\n"
                "---------- TESTING INSIDE HTML CODE ----------\n"
                "<$<p><i><b>Test</b></i> a  ~A~ is Ok </p>$>"
                );
    // END TEST
}

PadWriter::~PadWriter()
{
    if (d) {
        delete d;
        d = 0;
    }
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
//    d->m_Pad = PadAnalyzer().analyze(d->ui->rawSource->toHtml().replace("&lt;","<").replace("&gt;",">"));
//    const QString &parsed = d->m_Pad->run(d->m_TokenModel->tokens());
////    const QString &parsed = padTools()->parse(d->ui->rawSource->toHtml(), d->m_TokenModel->tokens(), errors);

//    qWarning() << "\n\n";
//    qWarning() << parsed;
//    qWarning() << "\n\n";

//    if (Qt::mightBeRichText(parsed))
//        d->ui->wysiwyg->setHtml(parsed);
//    else
//        d->ui->wysiwyg->setPlainText(parsed);

    d->m_Pad = PadAnalyzer().analyze(d->ui->rawSource->textEdit()->document());
    d->ui->wysiwyg->document()->clear();
    d->m_Pad->run(d->m_TokenModel->tokens(), d->ui->rawSource->textEdit()->document(), d->ui->wysiwyg->document());

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
//    WARN_FUNC << d->ui->rawSource->textEdit()->textCursor().position();
    if (!d->m_Pad)
        return;

    // Find corresponding fragment Id
//    const int cursorPos = d->ui->rawSource->textEdit()->textCursor().position();
//    const QList<PadFragment *> &fragments = d->m_Pad->getAllFragments();
//    QList<PadFragment *> currentFragments;
//    for(int i = 0; i < fragments.count(); ++i) {
//        PadFragment *fragment = fragments.at(i);
//        qWarning() << "TEST" << fragment->start() << fragment->end();
//        if (fragment->start() >= cursorPos && fragment->end() <= cursorPos) {
//            currentFragments << fragment;
//            qWarning() << "FOUND" << fragment->start() << fragment->end();
//        }
//    }

    // Find the block in the QTextDocument

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

bool PadWriter::eventFilter(QObject *obj, QEvent *event)
{
    if (obj!=d->ui->wysiwyg)
        return QObject::eventFilter(obj, event);
    if (!d->m_Pad)
        return QObject::eventFilter(obj, event);

    if (event->type()==QEvent::HoverMove) {
        QHoverEvent *me = static_cast<QHoverEvent*>(event);
        int position = d->ui->wysiwyg->cursorForPosition(me->pos()).position();
        PadItem *item = d->m_Pad->padItemForOutputPosition(position);
        if (!item)
            return QObject::eventFilter(obj, event);

        QTextCursor cursor(d->ui->wysiwyg->document());
        if (d->m_LastHoveredItem) {
            if (d->m_LastHoveredItem == item)
                return true;
            // remove token char format
            cursor.setPosition(d->m_LastHoveredItem->outputStart());
            cursor.setPosition(d->m_LastHoveredItem->outputEnd(), QTextCursor::KeepAnchor);
            cursor.setCharFormat(Constants::removeTokenCharFormat(cursor.charFormat()));
            d->m_LastHoveredItem = item;
        } else {
            d->m_LastHoveredItem = item;
        }
        cursor.setPosition(item->outputStart());
        cursor.setPosition(item->outputEnd(), QTextCursor::KeepAnchor);
        cursor.setCharFormat(Constants::tokenCharFormat());
        me->accept();
        return true;
    }
    return false;
}
