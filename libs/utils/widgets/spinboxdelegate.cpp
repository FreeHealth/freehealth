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
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
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
