/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developers:  Eric Maeker <eric.maeker@gmail.com>,                *
 *                    Guillaume Denry <guillaume.denry@gmail.com>          *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
/**
 * \class PadTools::TokenEditorWidget
 * Widget that allow to edit pre and post conditional texts of a token.
 */

#include "tokeneditorwidget.h"
#include "constants.h"
#include "pad_item.h"

#include <translationutils/constants.h>
#include <translationutils/trans_current.h>

#include "ui_tokeneditorwidget.h"

#include <QDebug>

using namespace PadTools;
using namespace Internal;
using namespace Trans::ConstantTranslations;

TokenEditorWidget::TokenEditorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TokenEditorWidget),
    _model(0)
{
    ui->setupUi(this);
    ui->tokenValueFormatting->setTypes(Editor::TextEditor::CharFormat);
    ui->tokenValueFormatting->toogleToolbar(false);
    ui->tokenValueFormatting->setReadOnly(true);

    ui->before->setTypes(Editor::TextEditor::Simple);
    ui->before->toogleToolbar(true);

    ui->after->setTypes(Editor::TextEditor::Simple);
    ui->after->toogleToolbar(true);

    layout()->setMargin(0);
    clear();

    // As this object is a dialog, the edit actions are unabled by default
    // We have to manage here to reset them enable in the editors
}

TokenEditorWidget::~TokenEditorWidget()
{
    delete ui;
}

void TokenEditorWidget::clear()
{
    ui->tokenGroup->setTitle(tkTr(Trans::Constants::TOKEN));
    ui->currentTokenValue->clear();
    ui->testTokenValue->clear();
    ui->tokenValueFormatting->clear();
    ui->before->clear();
    ui->after->clear();
}

void TokenEditorWidget::setTokenModel(TokenModel *model)
{
    _model = model;
}

void TokenEditorWidget::setCurrentIndex(const QModelIndex &index)
{
    clear();
    if (!index.isValid())
        return;

    _tokenUid = index.data().toString();
    ui->tokenGroup->setTitle(tkTr(Trans::Constants::TOKEN));
    ui->currentTokenValue->setText(tkTr(Trans::Constants::UNDEFINED));
    ui->testTokenValue->setText(tkTr(Trans::Constants::UNDEFINED));
    ui->tokenValueFormatting->setPlainText(_tokenUid);
}

void TokenEditorWidget::setTokenUid(const QString &uid)
{
    _tokenUid=uid;
    ui->tokenValueFormatting->clear();
    ui->tokenValueFormatting->setPlainText(_tokenUid);
}

void TokenEditorWidget::setConditionnalBeforeHtml(const QString &html)
{
    ui->before->setHtml(html);
}

void TokenEditorWidget::setConditionnalAfterHtml(const QString &html)
{
    ui->after->setHtml(html);
}

void TokenEditorWidget::setConditionnalBeforePlainText(const QString &txt)
{
    ui->before->setPlainText(txt);
}

void TokenEditorWidget::setConditionnalAfterPlainText(const QString &txt)
{
    ui->after->setPlainText(txt);
}

QString TokenEditorWidget::toRawSourceHtml() const
{
    QTextDocument doc;
    QTextCursor cursor(&doc);

    cursor.insertText(Constants::TOKEN_OPEN_DELIMITER);
    cursor.movePosition(QTextCursor::End);

    cursor.insertHtml(ui->before->toHtml());
    cursor.movePosition(QTextCursor::End);

    cursor.insertText(Constants::TOKEN_CORE_DELIMITER);
    cursor.movePosition(QTextCursor::End);

    // TODO: insert charFormat for the token value */
    cursor.insertText(_tokenUid);
    cursor.movePosition(QTextCursor::End);

    cursor.insertText(Constants::TOKEN_CORE_DELIMITER);
    cursor.movePosition(QTextCursor::End);

    cursor.insertHtml(ui->after->toHtml());
    cursor.movePosition(QTextCursor::End);

    cursor.insertText(Constants::TOKEN_CLOSE_DELIMITER);
    cursor.movePosition(QTextCursor::End);

    return doc.toHtml();
}

void TokenEditorWidget::getOutput(QString &html, PadItem &item, int startingOutputPos) const
{
    QTextDocument doc;
    QTextCursor cursor(&doc);
    html.clear();
    // Keep parent && id of item
    item.clear();
    item.setOutputStart(startingOutputPos);
    int previousPosition = 0;

    // Insert SubItemFragment before (id ?)
    PadConditionnalSubItem *before = new PadConditionnalSubItem(PadConditionnalSubItem::Defined, PadConditionnalSubItem::Prepend, &item);
    before->setOutputStart(startingOutputPos);
    cursor.insertHtml(ui->before->toHtml());
    cursor.movePosition(QTextCursor::End);
    startingOutputPos += cursor.position() - previousPosition;
    before->setOutputEnd(startingOutputPos);

    // Insert PadCore (id ?)
    previousPosition = cursor.position();
    PadCore *core = new PadCore;
    core->setUid(_tokenUid);
    core->setOutputStart(startingOutputPos);
    // TODO: insert charFormat for the token value */
    cursor.insertText(_tokenUid);
    cursor.movePosition(QTextCursor::End);
    startingOutputPos += cursor.position() - previousPosition;
    core->setOutputEnd(startingOutputPos);

    // Insert SubItemFragment after (id ?)
    previousPosition = cursor.position();
    PadConditionnalSubItem *after = new PadConditionnalSubItem(PadConditionnalSubItem::Defined, PadConditionnalSubItem::Append, &item);
    after->setOutputStart(startingOutputPos);
    cursor.insertHtml(ui->after->toHtml());
    cursor.movePosition(QTextCursor::End);
    startingOutputPos += cursor.position() - previousPosition;
    after->setOutputEnd(startingOutputPos);

    item.addChild(before);
    item.addChild(core);
    item.addChild(after);
    item.setOutputEnd(startingOutputPos);
    html = doc.toHtml();
}

