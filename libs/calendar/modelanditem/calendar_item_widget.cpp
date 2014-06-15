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
 *   Main Developers:                                                     *
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "calendar_item_widget.h"
#include "abstract_calendar_model.h"

#include <QEvent>
#include <QPainter>

using namespace Calendar;
using namespace Internal;

CalendarItemWidget::CalendarItemWidget(QWidget *parent, const QString &uid, AbstractCalendarModel *model) :
    QWidget(parent),
    m_uid(uid),
    m_model(model)
{
}

/*! Sets the beginning of the item to the given \e dateTime */
void CalendarItemWidget::setBeginDateTime(const QDateTime &dateTime)
{
    if (m_beginDateTime == dateTime)
        return;

    m_beginDateTime = dateTime;
    update();
}

/*! Sets the end of the item to the given \e dateTime */
void CalendarItemWidget::setEndDateTime(const QDateTime &dateTime)
{
    if (m_endDateTime == dateTime)
        return;

    m_endDateTime = dateTime;
    update();
}

bool CalendarItemWidget::event(QEvent *event)
{
    if (m_model) {
        if (event->type()==QEvent::ToolTip) {
            // get the CalendarItem
            CalendarItem item = m_model->getItemByUid(m_uid);
            // get the tooltip
            setToolTip(m_model->data(item, 0, Qt::ToolTipRole).toString());
        }
    }
    return QWidget::event(event);
}

QDebug operator<<(QDebug dbg, const Calendar::Internal::CalendarItemWidget &c)
{
    dbg.nospace() << "CalendarItemWidget("
                  << c.beginDateTime().toString(QLocale().dateTimeFormat(QLocale::ShortFormat))
                  << ", "
                  << c.endDateTime().toString(QLocale().dateTimeFormat(QLocale::ShortFormat))
                  << ", " << c.durationInSeconds() << "s, " << c.durationInSeconds()/60 << "m)";
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const Calendar::Internal::CalendarItemWidget *c)
{
    if (!c) {
        dbg.nospace() << "CalendarItemWidget(0x0)";
        return dbg.space();
    }
    return operator<<(dbg, *c);
}
