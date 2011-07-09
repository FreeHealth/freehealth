/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "datetimedelegate.h"

#include <QDate>
#include <QDateTime>
#include <QDateEdit>
#include <QDateTimeEdit>

#include <QDebug>

using namespace Utils;

DateTimeDelegate::DateTimeDelegate(QObject *parent, bool dateOnly) :
        QItemDelegate(parent), m_IsDateOnly(dateOnly)
{
    m_MinDate = QDate::currentDate().addYears(-100);
    m_MaxDate = QDate::currentDate().addYears(100);
    m_MaxTime = QTime(0,0,0,0);
    m_MinTime = QTime(0,0,0,0);
}

void DateTimeDelegate::setDateRange(const QDateTime &min, const QDateTime &max)
{
    m_MaxTime = max.time();
    m_MinTime = min.time();
    m_MaxDate = max.date();
    m_MinDate = min.date();
}

void DateTimeDelegate::setDateRange(const QDate &min, const QDate &max)
{
    if (max.isValid())
        m_MaxDate = max;
    else
        m_MaxDate = QDate::currentDate().addYears(100);
    m_MinDate = min;
    m_MaxTime = QTime(0,0,0,0);
    m_MinTime = QTime(0,0,0,0);
}

void DateTimeDelegate::setDateOnly(bool state)
{
    m_IsDateOnly = state;
}

QWidget *DateTimeDelegate::createEditor(QWidget *parent,
                                       const QStyleOptionViewItem &/* option */,
                                       const QModelIndex &index) const
{
    if (m_IsDateOnly) {
        QDateEdit *editor = new QDateEdit(parent);
        editor->setMinimumDate(m_MinDate);
        editor->setMaximumDate(m_MaxDate);
        editor->setCalendarPopup(true);
        editor->setDisplayFormat(QLocale().dateFormat());
        editor->setDate(index.data(Qt::EditRole).toDate());
        return editor;
    } else {
        QDateTimeEdit *editor = new QDateTimeEdit(parent);
        editor->setMinimumDateTime(QDateTime(m_MinDate, m_MinTime));
        editor->setMaximumDateTime(QDateTime(m_MaxDate, m_MaxTime));
        editor->setDisplayFormat(QLocale().timeFormat());
        editor->setDateTime(index.data().toDateTime());
        return editor;
    }
    return 0;
}

void DateTimeDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    QDateEdit *dateEdit = qobject_cast<QDateEdit*>(editor);
    if (dateEdit) {
        dateEdit->setDate(index.data(Qt::EditRole).toDate());
    } else {
        QDateTimeEdit *dateTimeEdit = qobject_cast<QDateTimeEdit*>(editor);
        if (dateTimeEdit) {
            dateTimeEdit->setDateTime(index.data(Qt::EditRole).toDateTime());
        }
    }
}

void DateTimeDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    QDateEdit *dateEdit = qobject_cast<QDateEdit*>(editor);
    if (dateEdit) {
        model->setData(index, dateEdit->date(), Qt::EditRole);
    } else {
        QDateTimeEdit *dateTimeEdit = qobject_cast<QDateTimeEdit*>(editor);
        if (dateTimeEdit) {
            model->setData(index, dateTimeEdit->dateTime(), Qt::EditRole);
        }
    }
}

void DateTimeDelegate::updateEditorGeometry(QWidget *editor,
                                           const QStyleOptionViewItem &option,
                                           const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}
