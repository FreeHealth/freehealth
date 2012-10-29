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
 * \class Views::TimeComboBox
 * \brief Combines a QTimeEdit with a QComboBox and replaces a QTimeEdit
 *
 * Normally a QDateTimeEdit consists of one long edit with separate sections
 * for date and time. In FMF the date in the calendar often needed as shorttext
 * with a calendar popup, and the time is needed as pre-selected options like
 * 10:00, 10:10, 10:20, 10:30 etc. Using a normal QTimeEdit would suffice, but
 * is very unconvenient in daily use. So here we replaced it with a QComboBox that provides
 * sane values for times, you just have to click on them once. The intervals can be
 * set individually.
 * The Widget provides more or less the same interface (signals, slots, properties)
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
 * \class Views::TimeComboBoxPrivate
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

    const int index = d->combo->findData(time);
    if (index == -1) {
        // custom time, not found in combo items

        // QTime() is not the same as QTime(0,0)!!
        d->time = time.isNull()? QTime(0, 0) : time;
        d->combo->setEditText(time.toString(QLocale::system().timeFormat(QLocale::ShortFormat)));
        qDebug() << "setEditText" << time;
    } else {
        // given time is one of the combo items
        d->combo->setCurrentIndex(index);
        qDebug() << "setCurrentIndex" << index << d->combo->itemData(index);
    }
    Q_EMIT timeChanged(d->time);
    Q_EMIT dateTimeChanged(QDateTime(QDate(), d->time));
}

/*! Sets the editable state of the combo box. \sa QComboBox::setEditable(). */
void TimeComboBox::setEditable(bool editable)
{
    d->combo->setEditable(editable);
}

/*! Returns the \e editable property of the combo box. \sa QComboBox::editable(). */
bool TimeComboBox::editable() const
{
    return d->combo->isEditable();
}

int TimeComboBox::interval() const
{
    return d->interval;
}

/*! Returns the current time that is displayed in the widget. */
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
void TimeComboBox::initialize()
{
    setInterval(30);
    updateComboItems();
    connect(d->combo, SIGNAL(currentIndexChanged(int)), this, SLOT(updateTimeFromComboSelection(int)));
    connect(d->combo, SIGNAL(editTextChanged(QString)), this, SLOT(updateTimeFromComboEditText(QString)));
}

/*! \brief Fills the combo box with a list of time slices, per default 30 minutes intervals.
 *
 * The interval can be set with setInterval(). Each time the user changes the combo value, the signal
 * \e timeChanged() is emitted. The current selected time can be retrieved anytime with time().
 */
void TimeComboBox::updateComboItems()
{
    // save current displayed time for restoring later
    QTime oldTime = this->time();

//    d->combo->clear();

    for (QTime time = QTime(0, 0); time < QTime(23, 59); time = time.addSecs(d->interval * 60)) {
        // Each combo item is filled with the current locale's time string, and the item data is set to the QTime,
        d->combo->addItem(time.toString(QLocale::system().timeFormat(QLocale::ShortFormat)), time);

        // prevent wrapping of time to next day -> infinite loop
        if (time.addSecs(d->interval * 60) < time)
            break;
    }
    setTime(oldTime);
}

/*! Sets interval of the times in the combo box. Currently this method does nothing. */
void TimeComboBox::setInterval(int minutes)
{
    const bool changed = (d->interval != minutes);
    if (changed) {
        d->interval = minutes;
        updateComboItems();
    }
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
}

/*! Auto slot that updates the internal time to the currently edited time string. */
void TimeComboBox::updateTimeFromComboEditText(const QString &text)
{
    d->time = QTime::fromString(text, QLocale::system().timeFormat(QLocale::ShortFormat));
    if (d->time.isNull())
        // allow "military" input method like 1200 (= 12:00), 2030 (= 20:30 or 8:30pm)
        d->time = QTime::fromString(text, "hhmm");
}


