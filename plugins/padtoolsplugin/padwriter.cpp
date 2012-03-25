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
    PadWriterPrivate(PadWriter *parent) : q(parent) {}

public:
    Ui::PadWriter *ui;
    TokenModel *m_TokenModel;
    QAction *aFollowCursor, *aAutoUpdate, *aSetDefaultValues;

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

    // create tokenmodel
    d->m_TokenModel = new TokenModel(this);
    d->ui->treeView->setModel(d->m_TokenModel);
    d->ui->treeView->header()->setResizeMode(0, QHeaderView::Stretch);

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

    // TEST
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
    const QString &parsed = padTools()->parse(d->ui->rawSource->toHtml(), d->m_TokenModel->tokens(), errors);
    if (Qt::mightBeRichText(parsed))
        d->ui->wysiwyg->setHtml(parsed);
    else
        d->ui->wysiwyg->setPlainText(parsed);

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
