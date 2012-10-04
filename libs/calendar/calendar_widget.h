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
 *   Main Developers:                                                      *
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef CALENDAR_WIDGET_H
#define CALENDAR_WIDGET_H

#include <calendar/calendar_exporter.h>
#include <calendar/common.h>

#include <QScrollArea>
#include <QTimer>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QMenu;
QT_END_NAMESPACE

/**
 * \file calendar_widget.h
 * \author Guillaume Denry, Eric Maeker
 * \version 0.8.0
 * \date 04 Oct 2012
*/

namespace Calendar {
struct CalendarWidgetPrivate;
class AbstractCalendarModel;
class CalendarItem;

class CALENDAR_EXPORT CalendarWidget : public QWidget
{
    Q_OBJECT
    /** The number of minutes used when we want to move of size items. Worth 15 by default. */
    Q_PROPERTY(int dayGranularity READ dayGranularity WRITE setDayGranularity)
    /** default item duration. 30 minutes by default. */
    Q_PROPERTY(int dayItemDefaultDuration READ dayItemDefaultDuration WRITE setDayItemDefaultDuration)
    /** Divider for the vertical day scale in views.
     * For instance, to have a line every half-hour, use 2. For a line every fifteen minutes, use 4.
     * Worth 2 by default.
     */
    Q_PROPERTY(int dayScaleHourDivider READ dayScaleHourDivider WRITE setDayScaleHourDivider)
    /** Height (in pixels) of an hour. 40 by default. */
    Q_PROPERTY(int hourHeight READ hourHeight WRITE setHourHeight)
public:
    CalendarWidget(QWidget *parent = 0);

    AbstractCalendarModel *model() const { return m_model; }
    void setModel(AbstractCalendarModel *model);

    ViewType viewType() const;
    void setViewType(Calendar::ViewType viewType);

    void setDate(const QDate &date);

    int dayGranularity() const;
    int dayItemDefaultDuration() const;
    int dayScaleHourDivider() const;
    int hourHeight() const;

    Calendar::CalendarItem getContextualCalendarItem() const;

public Q_SLOTS:
    void setDayGranularity(int minutes);
    void setDayItemDefaultDuration(int minutes);
    void setDayScaleHourDivider(int divider);
    void setHourHeight(int pixels);
    void setContextMenuForItems(QMenu *menu);

    void scrollToTime(const QTime &time);

private Q_SLOTS:
    // navigation bar slots
    void firstDateChanged();
    void viewTypeChanged();

    // timer
    void timeout();

private:
    CalendarWidgetPrivate *m_d;
    ViewType m_viewType;
    AbstractCalendarModel *m_model;
};

}  // End namespace Calendar

#endif
