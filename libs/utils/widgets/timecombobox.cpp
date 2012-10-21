/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main developers: Christian A. Reiter <christian.a.reiter@gmail.com>   *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class Views::DateTimeEdit
 * \brief Combines a QDateEdit with a QComboBox that replaces a QTimeEdit
 *
 * Normally a QDateTimeEdit consists of one long edit with separate sections
 * for date and time. In FMF the date in the calendar often needed as shorttext
 * with a calendar popup, and the time is needed as pre-selected options like
 * 10:00, 10:10, 10:20, 10:30 etc. Using a normal QTimeEdit would suffice, but
 * is very bad in UI design. So here we replaced it with a QComboBox that provides
 * sane values for times, you just have to click on them once.
 * The Widget provides more or less the same Interface (signals, slots, properties)
 * as QDateTimeEdit.
 * \sa QDateEdit, QTimeEdit, QDateTimeEdit
 */

#include "timecombobox.h"

#include <translationutils/constants.h>

#include <QDebug>
#include <QTimeEdit>
#include <QComboBox>
#include <QHBoxLayout>


using namespace Views;
using namespace Views::Internal;
using namespace Trans::ConstantTranslations;

namespace Views {
namespace Internal {
/*!
 * \class Views::DateTimeEditPrivate
 * \brief Private implementation of the Views::DateTimeEdit class.
 *
 * This class implements all the details for DateTimeEdit.
 */
class TimeComboBoxPrivate
{
public:
    TimeComboBoxPrivate(TimeComboBox *parent) :
        time(QTime()),
        q(parent)
    {
        combo = new QComboBox(q);
        QHBoxLayout *layout = new QHBoxLayout(q);
        layout->addWidget(combo);
        q->setLayout(layout);
    }

    ~TimeComboBoxPrivate()
    {
    }

public:
    QTime time;
    QComboBox *combo;
    int interval;

private:
    TimeComboBox *q;
};

} // end namespace Internal
} // end namespace Views

/*! Sets internal time to \e time. */
void TimeComboBox::setTime(const QTime &time)
{
    if (d->time == time)
        return;

    d->time = time.isNull()? QTime(0, 0) : time;
    d->combo->setEditText(time.toString(QLocale::system().timeFormat(QLocale::ShortFormat)));
    Q_EMIT timeChanged(d->time);
    Q_EMIT dateTimeChanged(QDateTime(QDate(), d->time));
}

/*! Tries to set time with given \e timeString. If string is not convertible, set time to QTime(). */
void TimeComboBox::setTime(const QString &timeString)
{
}

/*! Sets the editable state of the combo box. \sa QComboBox::setEditable() */
void TimeComboBox::setEditable(bool editable)
{
    d->combo->setEditable(editable);
}

/*! Returns the \e editable property of the combo box. \sa QComboBox::editable() */
bool TimeComboBox::editable() const
{
    return d->combo->isEditable();
}

/*! Returns the current time that is displayed in the widget */
QTime TimeComboBox::time() const
{
    return d->time;
}


/*! Default constructor of the Views::DateTimeEdit class */
TimeComboBox::TimeComboBox(QWidget *parent) :
    QWidget(parent),
    d(new Internal::TimeComboBoxPrivate(this))
{
    initialize();
    setTime(QTime());
}

/*! Convenience constructor that sets the given \e time after widget creation. */
TimeComboBox::TimeComboBox(QTime &time, QWidget *parent) :
    QWidget(parent),
    d(new Internal::TimeComboBoxPrivate(this))
{
    initialize();
    setTime(time);
}

/*! Destructor of the Views::DateTimeEdit class */
TimeComboBox::~TimeComboBox()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool TimeComboBox::initialize()
{
    // Provide the combobox with a predefined list of times, e.g. 30 minutes intervals
    for(int h = 0; h < 24; ++h) {
        d->combo->addItem(QTime(h, 0).toString(QLocale::system().timeFormat(QLocale::ShortFormat)), QTime(h, 0));
        d->combo->addItem(QTime(h, 30).toString(QLocale::system().timeFormat(QLocale::ShortFormat)), QTime(h, 30));
    }
    connect(d->combo, SIGNAL(currentIndexChanged(int)), this, SLOT(updateTimeFromComboSelection(int)));
    connect(d->combo, SIGNAL(editTextChanged(QString)), this, SLOT(updateTimeFromComboEditText(QString)));
    return true;
}

/*! Sets interval of the times in the combo box. Currently this method does nothing. */
void TimeComboBox::setInterval(int interval)
{
    //TODO: has no function yet.
    d->interval = interval;
}

/*! Auto slot that updates the internal time to the current selected time. */
void TimeComboBox::updateTimeFromComboSelection(const int index)
{
    if (index == -1) {
        setTime(QTime(0,0));
        return;
    }

    QTime time = d->combo->itemData(index).toTime();
    setTime(time);
    qDebug() << d->time;
}

/*! Auto slot that updates the internal time to the currently edited time string. */
void TimeComboBox::updateTimeFromComboEditText(const QString &text)
{
    d->time = QTime::fromString(text, QLocale::system().timeFormat(QLocale::ShortFormat));
    qDebug() << d->time;
}


