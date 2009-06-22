/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
 *   eric.maeker@free.fr                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "QCheckableComboBox.h"

#include <QLineEdit>
#include <QKeyEvent>

QCheckableComboBox::QCheckableComboBox()
{
}


/*
QCheckableComboBox::QCheckableComboBox() : containerMousePress(false)
{
    separator = QLatin1String(",");
}
*/

bool QCheckableComboBox::eventFilter( QObject * receiver, QEvent * event)
{
    switch (event->type())
    {
        case QEvent::KeyPress:
        case QEvent::KeyRelease:
        {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            if (receiver == &qxt_p() && (keyEvent->key() == Qt::Key_Up || keyEvent->key() == Qt::Key_Down))
            {
                qxt_p().showPopup();
                return true;
            }
            else if (keyEvent->key() == Qt::Key_Enter ||
                     keyEvent->key() == Qt::Key_Return ||
                     keyEvent->key() == Qt::Key_Escape)
            {
                // it is important to call QComboBox implementation
                qxt_p().QComboBox::hidePopup();
                if (keyEvent->key() != Qt::Key_Escape)
                    return true;
            }
        }
        case QEvent::MouseButtonPress:
            containerMousePress = (receiver == qxt_p().view()->window());
            break;
        case QEvent::MouseButtonRelease:
            containerMousePress = false;;
            break;
        default:
            break;
    }
    return false;
}

void QCheckableComboBox::updateCheckedItems()
{
    QStringList items = qxt_p().checkedItems();
    if (items.isEmpty())
        qxt_p().setEditText(defaultText);
    else
        qxt_p().setEditText(items.join(separator));

    // TODO: find a way to recalculate a meaningful size hint

    emit qxt_p().checkedItemsChanged(items);
}

void QCheckableComboBox::toggleCheckState(int index)
{
    QVariant value = qxt_p().itemData(index, Qt::CheckStateRole);
    if (value.isValid())
    {
        Qt::CheckState state = static_cast<Qt::CheckState>(value.toInt());
        qxt_p().setItemData(index, (state == Qt::Unchecked ? Qt::Checked : Qt::Unchecked), Qt::CheckStateRole);
    }
}

QCheckableComboModel::QCheckableComboModel(QObject* parent)
        : QStandardItemModel(0, 1, parent) // rows,cols
{
}

Qt::ItemFlags QCheckableComboModel::flags(const QModelIndex& index) const
{
    return QStandardItemModel::flags(index) | Qt::ItemIsUserCheckable;
}

QVariant QCheckableComboModel::data(const QModelIndex& index, int role) const
{
    QVariant value = QStandardItemModel::data(index, role);
    if (index.isValid() && role == Qt::CheckStateRole && !value.isValid())
        value = Qt::Unchecked;
    return value;
}

bool QCheckableComboModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    bool ok = QStandardItemModel::setData(index, value, role);
    if (ok && role == Qt::CheckStateRole)
    {
        emit dataChanged(index, index);
        emit checkStateChanged();
    }
    return ok;
}

/*!
    \class QxtCheckComboBox QxtCheckComboBox
    \ingroup QxtGui
    \brief An extended QComboBox with checkable items.

    QxtComboBox is a specialized combo box with checkable items.
    Checked items are collected together in the line edit.

    \image html qxtcheckcombobox.png "QxtCheckComboBox in Plastique style."
 */

/*!
    \enum QxtCheckComboBox::CheckMode

    This enum describes the check mode.

    \sa QxtCheckComboBox::checkMode
 */

/*!
    \var QxtCheckComboBox::CheckMode QxtCheckComboBox::CheckIndicator

    The check state changes only via the check indicator (like in item views).
 */

/*!
    \var QxtCheckComboBox::CheckMode QxtCheckComboBox::CheckWholeItem

    The check state changes via the whole item (like with a combo box).
 */

/*!
    \fn QxtCheckComboBox::checkedItemsChanged(const QStringList& items)

    This signal is emitted whenever the checked items have been changed.
 */

/*!
    Constructs a new QxtCheckComboBox with \a parent.
 */
QCheckableComboBox::QCheckableComboBox( QWidget * parent )
    : QComboBox(parent)
{
    setModel(new QxtCheckComboModel(this));
    connect(this, SIGNAL(activated(int)), &qxt_d(), SLOT(toggleCheckState(int)));
    connect(model(), SIGNAL(checkStateChanged()), &qxt_d(), SLOT(updateCheckedItems()));

    // read-only contents
    QLineEdit* lineEdit = new QLineEdit(this);
    lineEdit->setReadOnly(true);
    setLineEdit(lineEdit);
    setInsertPolicy(QComboBox::NoInsert);

    view()->installEventFilter(&qxt_d());
    view()->window()->installEventFilter(&qxt_d());
    view()->viewport()->installEventFilter(&qxt_d());
    this->installEventFilter(&qxt_d());
}

/*!
    Destructs the combo box.
 */
QCheckableComboBox::QCheckableComboBox()
{
}

/*!
    \reimp
 */
void QCheckableComboBox::hidePopup()
{
    if (qxt_d().containerMousePress)
        QComboBox::hidePopup();
}

/*!
    Returns the check state of the item at \a index.
 */
Qt::CheckState QCheckableComboBox::itemCheckState(int index) const
{
    return static_cast<Qt::CheckState>(itemData(index, Qt::CheckStateRole).toInt());
}

/*!
    Sets the check state of the item at \a index to \a state.
 */
void QCheckableComboBox::setItemCheckState(int index, Qt::CheckState state)
{
    setItemData(index, state, Qt::CheckStateRole);
}

/*!
    \property QxtCheckComboBox::checkedItems
    \brief This property holds the checked items.
 */
QStringList QCheckableComboBox::checkedItems() const
{
    QStringList items;
    if (model())
    {
        QModelIndex index = model()->index(0, modelColumn(), rootModelIndex());
        QModelIndexList indexes = model()->match(index, Qt::CheckStateRole, Qt::Checked, -1, Qt::MatchExactly);
        foreach (const QModelIndex& index, indexes)
            items += index.data().toString();
    }
    return items;
}

void QCheckableComboBox::setCheckedItems(const QStringList& items)
{
    // not the most efficient solution but most likely nobody
    // will put too many items into a combo box anyway so...
    foreach (const QString& text, items)
    {
        const int index = findText(text);
        setItemCheckState(index, index != -1 ? Qt::Checked : Qt::Unchecked);
    }
}

/*!
    \property QxtCheckComboBox::defaultText
    \brief This property holds the default text.

    The default text is shown when there are no checked items.
    The default value is an empty string.
 */
QString QCheckableComboBox::defaultText() const
{
    return qxt_d().defaultText;
}

void QCheckableComboBox::setDefaultText(const QString& text)
{
    if (qxt_d().defaultText != text)
    {
        qxt_d().defaultText = text;
        qxt_d().updateCheckedItems();
    }
}

/*!
    \property QxtCheckComboBox::separator
    \brief This property holds the default separator.

    The checked items are joined together with the separator string.
    The default value is a comma (",").
 */
QString QCheckableComboBox::separator() const
{
    return qxt_d().separator;
}

void QCheckableComboBox::setSeparator(const QString & separator)
{
    if (qxt_d().separator != separator)
    {
        qxt_d().separator = separator;
        qxt_d().updateCheckedItems();
    }
}
