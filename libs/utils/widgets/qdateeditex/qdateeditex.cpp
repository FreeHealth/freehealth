/**************************************************************************
**
** Copyright (c) 2013 Qualiant Software GmbH
**
** Author: Andreas Holzammer, KDAB (andreas.holzammer@kdab.com)
**
** Contact: Qualiant Software (d.oberkofler@qualiant.at)
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you have questions regarding the use of this file, please contact
** Qualiant Software at d.oberkofler@qualiant.at.
**
**************************************************************************/

#include "qdateeditex.h"
#include <QStyle>
#include <QPushButton>
#include <QLineEdit>
#include <QStyleOptionSpinBox>
#include <QKeyEvent>
#include <QCalendarWidget>
#include <QDebug>

/*!
  \class DateEditEx dateeditex.h
  \brief A DateEdit with a nullable date

  This is a subclass of QDateEdit that has the additional feature
  of allowing to select a empty date. This can be achived with the
  clear button or by selecting the whole date and press backspace.

  To set an empty date from code, use the setter with an invalid
  date. To check whether the date is empty check if the date is valid,
  which comes from the getter.

  \sa QDateEdit
*/

using namespace Utils;

class QDateEditEx::Private {
public:
    Private( QDateEditEx* qq ) : q(qq),  clearButton(0), null(false), nullable(false)
    {
        lineEdit = q->findChild<QLineEdit *>("qt_spinbox_lineedit");
    }

    QDateEditEx* const q;

    QPushButton* clearButton;

    QLineEdit* lineEdit;

    bool null;
    bool nullable;

    void setNull(bool n) {
        null = n;
        if (null) {
            //QLineEdit *edit = q->findChild<QLineEdit *>("qt_spinbox_lineedit");
            if (!lineEdit->text().isEmpty()) {
                lineEdit->clear();
            }

        }
    }
};

/*!
  \reimp
*/
QDateEditEx::QDateEditEx(QWidget *parent) :
    QDateEdit(parent), d(new Private(this))
{
}

/*!
 * \brief returns date, if empty date is invalid
 * \return date, if empty date is invalid
 */
QDateTime QDateEditEx::dateTime() const
{
    if (d->nullable && d->null) {
        return QDateTime();
    } else {
        return QDateEdit::dateTime();
    }
}

/*!
 * \brief returns date, if empty date is invalid
 * \return date, if empty date is invalid
 */
QDate QDateEditEx::date() const
{
    if (d->nullable && d->null) {
        return QDate();
    } else {
        return QDateEdit::date();
    }
}

/*!
 * \brief returns date, if empty date is invalid
 * \return date, if empty date is invalid
 */
QTime QDateEditEx::time() const
{
    if (d->nullable && d->null) {
        return QTime();
    } else {
        return QDateEdit::time();
    }
}

/*!
 * \brief sets a date, if date is invalid a
 * empty date is shown
 */
void QDateEditEx::setDateTime(const QDateTime &dateTime)
{
    if (d->nullable && !dateTime.isValid()) {
        d->setNull(true);
    } else {
        d->setNull(false);
        QDateEdit::setDateTime(dateTime);
    }
}

/*!
 * \brief sets a date, if date is invalid a
 * empty date is shown
 */
void QDateEditEx::setDate(const QDate &date)
{
    if (d->nullable && !date.isValid()) {
        d->setNull(true);
    } else {
        d->setNull(false);
        QDateEdit::setDate(date);
    }
}

/*!
 * \brief sets a date, if date is invalid a
 * empty date is shown
 */
void QDateEditEx::setTime(const QTime &time)
{
    if (d->nullable && !time.isValid()) {
        d->setNull(true);
    } else {
        d->setNull(false);
        QDateEdit::setTime(time);
    }
}

/*!
 * \brief returns date can be empty
 * \return true, if date can be emtpy
 */
bool QDateEditEx::isNullable() const
{
    return d->nullable;
}

/*!
 * \brief sets whether the date can be empty
 */
void QDateEditEx::setNullable(bool enable)
{
    d->nullable = enable;

    if (enable && !d->clearButton) {
        d->clearButton = new QPushButton(this);
        d->clearButton->setFlat(true);
#if defined(WIDGETS_LIBRARY)
        static bool initres = false;
        if (!initres) {
            Q_INIT_RESOURCE(widgets);
            initres = true;
        }
#endif // defined(WIDGETS_LIBRARY)
        d->clearButton->setIcon(QIcon(":/images/edit-clear-locationbar-rtl.png"));
        d->clearButton->setFocusPolicy(Qt::NoFocus);
        d->clearButton->setFixedSize(17, d->clearButton->sizeHint().height()-6);
        connect(d->clearButton,SIGNAL(clicked()),this,SLOT(clearButtonClicked()));
        d->clearButton->setVisible(!d->null);
    } /*else if (d->clearButton) {
        disconnect(d->clearButton,SIGNAL(clicked()),this,SLOT(clearButtonClicked()));
        delete d->clearButton;
        d->clearButton = 0;
    }*/

    update();
}

/*!
  \reimp
*/
QSize QDateEditEx::sizeHint() const
{
    const QSize sz = QDateEdit::sizeHint();
    if (!d->clearButton)
        return sz;
    return QSize(sz.width() + d->clearButton->width() + 3, sz.height());
}

/*!
  \reimp
*/
QSize QDateEditEx::minimumSizeHint() const
{
    const QSize sz = QDateEdit::minimumSizeHint();
    if (!d->clearButton)
        return sz;
    return QSize(sz.width() + d->clearButton->width() + 3, sz.height());
}

void QDateEditEx::showEvent(QShowEvent *event)
{
    QDateEdit::showEvent(event);
    d->setNull(d->null); // force empty string back in
}

/*!
  \reimp
*/
void QDateEditEx::resizeEvent(QResizeEvent *event)
{
    if (d->clearButton) {
        QStyleOptionSpinBox opt;
        initStyleOption(&opt);
        opt.subControls = QStyle::SC_SpinBoxUp;

        int left = style()->subControlRect(QStyle::CC_SpinBox, &opt, QStyle::SC_SpinBoxUp, this).left() - d->clearButton->width() - 3;
        d->clearButton->move(left, (height() - d->clearButton->height()) / 2);
    }

    QDateEdit::resizeEvent(event);
}

/*!
  \reimp
*/
void QDateEditEx::paintEvent(QPaintEvent *event)
{
    QDateEdit::paintEvent(event);
}

/*!
  \reimp
*/
void QDateEditEx::keyPressEvent(QKeyEvent *event)
{
    QDateEdit::keyPressEvent(event);
    if (d->nullable &&
            (event->key() >= Qt::Key_0) &&
            (event->key() <= Qt::Key_9) &&
            d->null) {
        d->setNull(false);
        setCurrentSectionIndex(0);
        d->lineEdit->setCursorPosition(0);
        QString string = d->lineEdit->text();
        string.replace(0, 1, event->text());
        d->lineEdit->setText(string);
        d->lineEdit->setCursorPosition(1);
    }
    if (event->key() == Qt::Key_Backspace && d->nullable) {
        if (d->lineEdit->selectedText() == d->lineEdit->text()) {
            setDateTime(QDateTime());
        }
    }
}

/*!
  \reimp
*/
void QDateEditEx::mousePressEvent(QMouseEvent *event)
{
    bool saveNull = d->null;
    QDateEdit::mousePressEvent(event);
    if (d->nullable && saveNull && calendarWidget()->isVisible()) {
        setDateTime(QDateTime::currentDateTime());
    }
}

/*!
  \reimp
*/
void QDateEditEx::focusOutEvent(QFocusEvent *event)
{
    QDateEdit::focusOutEvent(event);
    if (d->nullable && d->null) {
        d->setNull(true);
    }
}

/*!
  \reimp
*/
bool QDateEditEx::focusNextPrevChild(bool next)
{
    if (d->nullable && d->null){
        return QAbstractSpinBox::focusNextPrevChild(next);
    } else {
        return QDateEdit::focusNextPrevChild(next);
    }
}

QValidator::State QDateEditEx::validate(QString &input, int &pos) const
{
    if (d->nullable && d->null){
        return QValidator::Acceptable;
    }
    return QDateEdit::validate(input, pos);
}

void QDateEditEx::clearButtonClicked()
{
    d->setNull(true);
}
