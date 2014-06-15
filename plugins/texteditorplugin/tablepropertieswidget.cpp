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
#include "tablepropertieswidget.h"
#include "ui_tablepropertieswidget.h"
#include "ui_tablepropertiesdialog.h"

#include <QDebug>

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
    m_InitialFormat = format;
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
    if (format.borderBrush().style()==Qt::NoBrush) {
        m_ui->borderColorButton->setColor(QColor(255,255,255,255));
    } else {
        m_ui->borderColorButton->setColor(format.borderBrush().color());
    }
    if (format.background().style()==Qt::NoBrush) {
        m_ui->cellBackgroundColorButton->setColor(QColor(255,255,255,255));
    } else {
        m_ui->cellBackgroundColorButton->setColor(format.background().color());
    }
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
    QTextTableFormat format = m_InitialFormat;

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


