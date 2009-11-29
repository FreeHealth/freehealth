#include "spinboxdelegate.h"
#include <QSpinBox>
#include <QDoubleSpinBox>

#include <QDebug>

using namespace Utils;

SpinBoxDelegate::SpinBoxDelegate(QObject *parent, double min, double max, bool isDouble)
        : QItemDelegate(parent), m_IsDouble(isDouble), m_Min(min), m_Max(max)
{
}

void SpinBoxDelegate::setMaximum(double max)
{
    if (max<0) {
        m_Max=0;
        m_Min=0;
    }
    m_Max=max;    
}

QWidget *SpinBoxDelegate::createEditor(QWidget *parent,
                                       const QStyleOptionViewItem &/* option */,
                                       const QModelIndex &/* index */) const
{
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
