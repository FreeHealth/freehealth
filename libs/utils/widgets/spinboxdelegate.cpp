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
#include "spinboxdelegate.h"
#include <QSpinBox>
#include <QDoubleSpinBox>

using namespace Utils;

SpinBoxDelegate::SpinBoxDelegate(QObject *parent, double min, double max, bool isDouble)
        : QItemDelegate(parent), m_IsDouble(isDouble), m_Min(min), m_Max(max)
{
}

void SpinBoxDelegate::setMaximum(double max)
{
    if (max<=0) {
        m_Max = 0;
        m_Min = 0;
    }
    if (m_Min > m_Max)
        m_Min = m_Max;
    m_Max = max;
}

void SpinBoxDelegate::setDouble(bool isDouble)
{
    m_IsDouble = isDouble;
}

QWidget *SpinBoxDelegate::createEditor(QWidget *parent,
                                       const QStyleOptionViewItem &/* option */,
                                       const QModelIndex &/* index */) const
{
//    if (m_Max == 0.00)
//        return 0;
    if (m_IsDouble) {
        QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
        editor->setMinimum(m_Min);
        editor->setMaximum(m_Max);
        editor->setSingleStep(0.25);
        return editor;
    } else {
        QSpinBox *editor = new QSpinBox(parent);
        editor->setMinimum(m_Min);
        editor->setMaximum(m_Max);
        editor->setSingleStep(1);
        return editor;
    }
    return 0;
}

void SpinBoxDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    QDoubleSpinBox *doubleSpinBox = qobject_cast<QDoubleSpinBox*>(editor);
    if (doubleSpinBox) {
        doubleSpinBox->setValue(index.model()->data(index, Qt::EditRole).toDouble());
    } else {
        QSpinBox *spinBox = qobject_cast<QSpinBox*>(editor);
        if (spinBox) {
            spinBox->setValue(index.model()->data(index, Qt::EditRole).toInt());
        }
    }
}


void SpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    QDoubleSpinBox *doubleSpinBox = qobject_cast<QDoubleSpinBox*>(editor);
    if (doubleSpinBox) {
        double value = doubleSpinBox->value();
        model->setData(index, value, Qt::EditRole);
    } else {
        QSpinBox *spinBox = qobject_cast<QSpinBox*>(editor);
        if (spinBox) {
            spinBox->interpretText();
            int value = spinBox->value();
            model->setData(index, value, Qt::EditRole);
        }
    }
}

void SpinBoxDelegate::updateEditorGeometry(QWidget *editor,
                                           const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}
