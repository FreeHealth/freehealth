/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
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
#include "ui_tablepropertiesdialog.h"

using namespace Editor::Internal;

////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////           WIDGET          ///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
TablePropertiesWidget::TablePropertiesWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::TablePropertiesWidget)
{
    m_ui->setupUi(this);
    m_ui->tabWidget->setCurrentWidget(m_ui->tabBorders);
    m_ui->borderStyleCombo->insertItem(QTextFrameFormat::BorderStyle_None, tr("None"));
    m_ui->borderStyleCombo->insertItem(QTextFrameFormat::BorderStyle_Dotted, tr("Dotted"));
    m_ui->borderStyleCombo->insertItem(QTextFrameFormat::BorderStyle_Dashed, tr("Dashed"));
    m_ui->borderStyleCombo->insertItem(QTextFrameFormat::BorderStyle_Solid, tr("Solid"));
    m_ui->borderStyleCombo->insertItem(QTextFrameFormat::BorderStyle_Double, tr("Double"));
    m_ui->borderStyleCombo->insertItem(QTextFrameFormat::BorderStyle_DotDash, tr("Dot dash"));
    m_ui->borderStyleCombo->insertItem(QTextFrameFormat::BorderStyle_DotDotDash, tr("Dot dot dash"));
    m_ui->borderStyleCombo->insertItem(QTextFrameFormat::BorderStyle_Groove, tr("Groove"));
    m_ui->borderStyleCombo->insertItem(QTextFrameFormat::BorderStyle_Ridge, tr("Rigde"));
    m_ui->borderStyleCombo->insertItem(QTextFrameFormat::BorderStyle_Inset, tr("Inset"));
    m_ui->borderStyleCombo->insertItem(QTextFrameFormat::BorderStyle_Outset, tr("Outset"));
}

TablePropertiesWidget::~TablePropertiesWidget()
{
    delete m_ui;
}

void TablePropertiesWidget::setFormat(const QTextTableFormat &format)
{
    m_ui->borderWidthSpin->setValue(format.border());
    m_ui->borderStyleCombo->setCurrentIndex(format.borderStyle());
    m_ui->cellPaddingSpin->setValue(format.cellPadding());
    m_ui->cellSpacingSpin->setValue(format.cellSpacing());
    if (format.margin()!=0) {
        m_ui->leftMarginSpin->setValue(format.margin());
        m_ui->rightMarginSpin->setValue(format.margin());
        m_ui->topMarginSpin->setValue(format.margin());
        m_ui->bottomMarginSpin->setValue(format.margin());
    } else {
        m_ui->leftMarginSpin->setValue(format.leftMargin());
        m_ui->rightMarginSpin->setValue(format.rightMargin());
        m_ui->topMarginSpin->setValue(format.topMargin());
        m_ui->bottomMarginSpin->setValue(format.bottomMargin());
    }
    m_ui->borderColorButton->setColor(format.borderBrush().color());
    m_ui->cellBackgroundColorButton->setColor(format.background().color());
}

int TablePropertiesWidget::cellPadding() const
{ return m_ui->cellPaddingSpin->value(); }
int TablePropertiesWidget::cellSpacing() const
{ return m_ui->cellSpacingSpin->value(); }

int TablePropertiesWidget::cellLeftMargin() const
{ return m_ui->leftMarginSpin->value(); }
int TablePropertiesWidget::cellRightMargin() const
{ return m_ui->rightMarginSpin->value(); }
int TablePropertiesWidget::cellTopMargin() const
{ return m_ui->topMarginSpin->value(); }
int TablePropertiesWidget::cellBottomMargin() const
{ return m_ui->bottomMarginSpin->value(); }

int TablePropertiesWidget::borderWidth() const
{ return m_ui->borderWidthSpin->value(); }


QTextTableFormat TablePropertiesWidget::format() const
{
    QTextTableFormat format;

    // Border
    format.setBorder(borderWidth());
    format.setBorderStyle(QTextFrameFormat::BorderStyle(m_ui->borderStyleCombo->currentIndex()));

    format.setCellPadding(cellPadding());
    format.setCellSpacing(cellSpacing());
    format.setLeftMargin(cellLeftMargin());
    format.setRightMargin(cellRightMargin());
    format.setTopMargin(cellTopMargin());
    format.setBottomMargin(cellBottomMargin());

    format.setBorderBrush(QBrush(m_ui->borderColorButton->color()));
    format.setBackground(QBrush(m_ui->cellBackgroundColorButton->color()));

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



////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////           DIALOG          ///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

TablePropertiesDialog::TablePropertiesDialog(QWidget *parent) :
        QDialog(parent), applyToCell(false), applyToTable(false), m_Widget(0),
        m_ui(new Ui::TablePropertiesDialog)
{
    m_ui->setupUi(this);
    m_Widget = new TablePropertiesWidget(this);
    m_ui->verticalLayout->addWidget(m_Widget);
}

TablePropertiesDialog::~TablePropertiesDialog()
{
    delete m_ui;
}

void TablePropertiesDialog::setFormat(const QTextTableFormat &format)
{
    if (m_Widget)
        m_Widget->setFormat(format);
}

bool TablePropertiesDialog::applyToSelectedCells() const
{
    return applyToCell;
}

bool TablePropertiesDialog::applyToWholeTable() const
{
    return applyToTable;
}

int TablePropertiesDialog::cellPadding() const
{
    if (m_Widget)
        return m_Widget->cellPadding();
    return 0;
}
int TablePropertiesDialog::cellSpacing() const
{
    if (m_Widget)
        return m_Widget->cellSpacing();
    return 0;
}

int TablePropertiesDialog::cellLeftMargin() const
{
    if (m_Widget)
        return m_Widget->cellLeftMargin();
    return 0;
}
int TablePropertiesDialog::cellRightMargin() const
{
    if (m_Widget)
        return m_Widget->cellRightMargin();
    return 0;
}
int TablePropertiesDialog::cellTopMargin() const
{
    if (m_Widget)
        return m_Widget->cellTopMargin();
    return 0;
}
int TablePropertiesDialog::cellBottomMargin() const
{
    if (m_Widget)
        return m_Widget->cellBottomMargin();
    return 0;
}

int TablePropertiesDialog::borderWidth() const
{
    if (m_Widget)
        return m_Widget->borderWidth();
    return 0;
}

QTextTableFormat TablePropertiesDialog::format() const
{
    if (m_Widget)
        return m_Widget->format();
    return QTextTableFormat();
}


