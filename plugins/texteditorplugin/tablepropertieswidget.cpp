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
#include "tablepropertieswidget.h"
#include "ui_tablepropertieswidget.h"

using namespace Editor::Internal;

TablePropertiesWidget::TablePropertiesWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::TablePropertiesWidget)
{
    m_ui->setupUi(this);
}

TablePropertiesWidget::~TablePropertiesWidget()
{
    delete m_ui;
}

int TablePropertiesWidget::cellPadding() const
{ return m_ui->cellPaddingSpin->value(); }

int TablePropertiesWidget::cellMargin() const
{ return m_ui->cellMarginSpin->value(); }

int TablePropertiesWidget::borderWidth() const
{ return m_ui->borderWidthSpin->value(); }


QTextTableFormat TablePropertiesWidget::format() const
{
    QTextTableFormat format;

    // Border
    format.setBorder(borderWidth());
//    format.setBorderStyle(BorderStyle style);

    // Space
    format.setCellPadding(cellPadding());
    format.setMargin(cellMargin());

    // Colors
//    format.setBorderBrush();
//    format.setBackground();

//    format.setWidth(QTextLength(QTextLength::PercentageLength, 100));
//    int i = 0;
//    QVector<QTextLength> lengths;
//    for ( i = 0; i < cols(); i++ ) {
//        lengths << QTextLength( QTextLength::PercentageLength, 100 / cols() );
//    }
//    format.setColumnWidthConstraints(lengths);
//    if (header->isChecked())
//        format.setHeaderRowCount(1);
//    else
//        format.setHeaderRowCount(0);
    return format;
}
