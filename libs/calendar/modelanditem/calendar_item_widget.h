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
 *   Main Developers :                                                    *
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef LIBCALENDAR_ITEM_WIDGET_H
#define LIBCALENDAR_ITEM_WIDGET_H

#include <QWidget>
#include <QDateTime>

/**
 * \file calendar_item_widget.h
 * \author Guillaume Denry, Eric Maeker
 * \version 0.6.0
 * \date 12 Aug 2011
*/

namespace Calendar {
class AbstractCalendarModel;
namespace Internal {

class CalendarItemWidget : public QWidget
{
    Q_OBJECT
public:
    CalendarItemWidget(QWidget *parent = 0, const QString &uid = QString::null, AbstractCalendarModel *model = 0);

    const QString &uid() const { return m_uid; }

    /*! Returns the beginning of the item */
    const QDateTime &beginDateTime() const { return m_beginDateTime; }

    void setBeginDateTime(const QDateTime &dateTime);

    /*! Returns the ending of the item */
    const QDateTime &endDateTime() const { return m_endDateTime; }

    void setEndDateTime(const QDateTime &dateTime);

    /*! Returns the seconds of the item from beginning to it's end */
    int durationInSeconds() const {return m_beginDateTime.secsTo(m_endDateTime);}

    /*! Returns the internally used AbstractCalendarModel */
    AbstractCalendarModel *model() const { return m_model; }

private:
    bool event(QEvent *event);

private:
    // TMP : all date will probably be moved into a pure data class for events/tasks, etc
    QDateTime m_beginDateTime;
    QDateTime m_endDateTime;
    QString m_uid;
    AbstractCalendarModel *m_model;
};

}  // namespace Internal
}  // namespace Calendar

QDebug operator<<(QDebug dbg, const Calendar::Internal::CalendarItemWidget &c);
QDebug operator<<(QDebug dbg, const Calendar::Internal::CalendarItemWidget *c);


#endif  // LIBCALENDAR_ITEM_WIDGET_H
