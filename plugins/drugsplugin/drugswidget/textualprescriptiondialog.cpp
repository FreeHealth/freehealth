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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "textualprescriptiondialog.h"
#include "ui_textualprescriptiondialog.h"

#include <utils/global.h>

#include <QTextEdit>

using namespace DrugsWidget;

TextualPrescriptionDialog::TextualPrescriptionDialog(QWidget *parent) :
        QDialog(parent),
        m_ui(new Ui::TextualPrescriptionDialog)
{
    m_ui->setupUi(this);
}

TextualPrescriptionDialog::~TextualPrescriptionDialog()
{
    delete m_ui;
}

void TextualPrescriptionDialog::done(int result)
{
    if (result==QDialog::Accepted) {
        if (drugLabel().contains("[") || drugNote().contains("]")) {
            Utils::warningMessageBox(tr("Text can not contain any '[' or ']'."), tr("Please remove these chars from your texts"),"","");
            return;
        }
    }
    QDialog::done(result);
}

QString TextualPrescriptionDialog::drugLabel() const
{
    return  m_ui->drugNameTextEdit->toPlainText();
}

QString TextualPrescriptionDialog::drugNote() const
{
    return  m_ui->drugNoteTextEdit->toPlainText();
}

bool TextualPrescriptionDialog::isALD() const
{
    return m_ui->aldCheck->isChecked();
}


void TextualPrescriptionDialog::setDrugLabel(const QString &label)
{
    m_ui->drugNameTextEdit->setText(label);
}
void TextualPrescriptionDialog::setDrugNote(const QString &note)
{
    m_ui->drugNoteTextEdit->setText(note);
}
void TextualPrescriptionDialog::setALD(const bool ald)
{
    m_ui->aldCheck->setChecked(ald);
}
