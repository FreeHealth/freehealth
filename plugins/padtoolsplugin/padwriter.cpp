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
#include <coreplugin/ipadtools.h>

#include <utils/log.h>

#include "ui_padwriter.h"

#include <QDebug>

using namespace PadTools;
using namespace Internal;

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
    d->ui->treeView->setDragEnabled(true);
    d->ui->treeView->setAcceptDrops(false);
    d->ui->treeView->setDropIndicatorShown(false);
    d->ui->treeView->setDragDropMode(QAbstractItemView::DragOnly);

    this->setAcceptDrops(true);
    d->ui->rawSource->setAcceptDrops(true);
    connect(d->ui->viewResult, SIGNAL(clicked()), this, SLOT(analyseRawSource()));
    connect(d->ui->viewError, SIGNAL(clicked()), this, SLOT(viewErrors()));

    // TEST
    d->ui->rawSource->setPlainText(
                "[before 'a'  ~A~  after 'a']\n"
                "[before 'b'  ~B~  after 'b']\n"
                "[before 'c'  ~C~  after 'c']\n"
                "[before 'html'  ~HTMLTOKEN~  after 'html']\n"
                "\n\n"
                "---------- TESTING NESTED ----------\n"
                "[ba [bb ~B~ ab] ~A~  aa]  -----> ba bb B ab A aa\n"
                "[bb ~B~ ab [ba ~A~  aa]]  -----> bb B ab ba A aa\n"
                "[[bb ~B~ ab ba ~NULL~  aa]]  -----> \n"
                "[bb ~B~ ab [ba ~NULL~  aa]]  -----> bb B ab\n"
                "\n"
                "[ab ~A~ aa [ba [nnn ~NULL~ nnn [ac ~C~ bc]] ~B~  aa]]  -----> ab A aa ba B aa\n"
                "[ab ~A~ aa [ba [[nnn ~NULL~ nnn ] ac ~C~ bc ] ~B~  aa]]  -----> ab A aa ba B aa\n"
                "\n"
                "\n"
                "---------- TESTING INSIDE HTML CODE ----------\n"
                "[<p>Test a  ~A~ is Ok </p>]"
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
    const QString &parsed = padTools()->parse(d->ui->rawSource->toPlainText(), d->m_TokenModel->tokens(), errors);
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

//void PadWriter::dragEnterEvent(QDragEnterEvent *event)
//{
//    qWarning() << "E";
//    if (event->source() != d->ui->treeView) {
//        event->ignore();
//        return;
//    }

//    if (d->ui->rawSource->underMouse() &&
//            event->mimeData()->hasFormat(Constants::TOKENVALUE_MIME)) {
//        qWarning() << "    accept";
//        event->acceptProposedAction();
//        event->accept();
//    } else {
//        qWarning() << "    ignore";
//        event->ignore();
//    }
//}

//void PadWriter::dragMoveEvent(QDragMoveEvent *event)
//{
//    qWarning() << "M" << event->pos();
//    if (event->source() != d->ui->treeView) {
//        event->ignore();
//        return;
//    }


//    if (d->ui->rawSource->underMouse() &&
//            event->mimeData()->hasFormat(Constants::TOKENVALUE_MIME)) {
////        d->ui->rawSource->setFocus();
////        qWarning() << event->pos();

//        QTextCursor cursor = d->ui->rawSource->cursorForPosition(event->pos());
//        d->ui->rawSource->setTextCursor(cursor);
//        d->ui->rawSource->ensureCursorVisible();
//        // if event pos y <=10 scroll up
//        event->acceptProposedAction();
//        event->accept();
//    } else {
//        event->ignore();
//    }
//}

//void PadWriter::dragLeaveEvent(QDragLeaveEvent *event)
//{
//    qWarning() << "L";
//    if (d->ui->rawSource->underMouse()) {
//        event->ignore();
//    } else {
//        event->accept();
//    }
//}

//void PadWriter::dropEvent(QDropEvent *event)
//{
//    qWarning() << "Drop" << event->pos() << event->mimeData()->formats();
//    if (d->ui->rawSource->underMouse()) {
//        qWarning() << "UNDER MOUSE";
//        d->ui->rawSource->setFocus();
//        QTextCursor cursor = d->ui->rawSource->cursorForPosition(event->pos());
//        qWarning() << cursor.position() << event->mimeData()->data(Constants::TOKENVALUE_MIME);
//        cursor.insertText(event->mimeData()->data(Constants::TOKENVALUE_MIME));
//    }

//    event->acceptProposedAction();
//    event->accept();
//}
