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
 *  Main Developers:  Eric Maeker <eric.maeker@gmail.com>,                *
 *                    Guillaume Denry <guillaume.denry@gmail.com>          *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#include "tokeneditor.h"

#include <translationutils/constants.h>
#include <translationutils/trans_current.h>

#include "ui_tokeneditor.h"

using namespace PadTools;
using namespace Internal;
using namespace Trans::ConstantTranslations;

TokenEditor::TokenEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TokenEditor)
{
    setWindowModality(Qt::WindowModal);
    setWindowTitle(tkTr(Trans::Constants::EDIT_TOKEN));
    ui->setupUi(this);
}

TokenEditor::~TokenEditor()
{
    delete ui;
}

void TokenEditor::done(int result)
{
//    if (result==QDialog::Accepted) {
//        ui->tokenEditor->toHtml();
//    }
    QDialog::done(result);
}

void TokenEditor::setTokenModel(TokenModel *model)
{
    ui->tokenEditor->setTokenModel(model);
}

void TokenEditor::setCurrentIndex(const QModelIndex &index)
{
    ui->tokenEditor->setCurrentIndex(index);
}

void TokenEditor::setTokenUid(const QString &uid)
{
    ui->tokenEditor->setTokenUid(uid);
}

void TokenEditor::setConditionnalHtml(const QString &before, const QString &after)
{
    ui->tokenEditor->setConditionnalBeforeHtml(before);
    ui->tokenEditor->setConditionnalAfterHtml(after);
}

void TokenEditor::setConditionnalPlainText(const QString &before, const QString &after)
{
    ui->tokenEditor->setConditionnalBeforePlainText(before);
    ui->tokenEditor->setConditionnalAfterPlainText(after);
}

QString TokenEditor::toRawSourceHtml() const
{
    return ui->tokenEditor->toRawSourceHtml();
}

void TokenEditor::getOutput(QString &html, PadItem &item, int startingOutputPos) const
{
    return ui->tokenEditor->getOutput(html, item, startingOutputPos);
}
