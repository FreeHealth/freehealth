/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "textualprescriptiondialog.h"
#include "ui_textualprescriptiondialog.h"

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
