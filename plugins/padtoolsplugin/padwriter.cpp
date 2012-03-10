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

#include "ui_padwriter.h"

#include <QDebug>

using namespace PadTools;
using namespace Internal;

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
//    d->ui->treeView->header()->hide();
    d->ui->treeView->setModel(d->m_TokenModel);

//    ui->wysiwyg
//    ui->rawSource
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

//void PadWriter::rawSourceTextChanged()
//{
//    QList<Core::PadAnalyzerError> errors;
//    // TODO : use a timer based on key strokes instead of realtime analysis
////**    m_ui->previewTextEdit->setPlainText(m_padTools->parse(m_ui->padTextEdit->textEdit()->toPlainText(), m_TokenModel->tokens(), errors));

////**	m_ui->listWidgetErrors->clear();
//    foreach (const Core::PadAnalyzerError &error, errors) {
//        switch (error.errorType()) {
//        case Core::PadAnalyzerError::Error_UnexpectedChar:
////**			m_ui->listWidgetErrors->addItem(tr("Unexpected '%1' found at line %2 and pos %3").arg(error.errorTokens()["char"].toString()).arg(error.line()).arg(error.pos()));
//            break;
//        case Core::PadAnalyzerError::Error_CoreDelimiterExpected:
////**			m_ui->listWidgetErrors->addItem(tr("Expected '%1' at line %2 and pos %3").arg(error.errorTokens()["char"].toString()).arg(error.line()).arg(error.pos()));
//            break;
//        }
//    }
//}

