/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developers:                                                       *
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
  \class Calendar::CalendarWidget
  This widget is the central view of the calendar. It provides:
  - a navigation bar
  - a full calendar viewer.

  You must define the Calendar::AbstractCalendarModel to use with.
*/

#include "calendar_widget.h"
#include "calendar_navbar.h"
#include <calendar/dayview/dayrangeheader.h>
#include <calendar/dayview/dayrangebody.h>
#include <calendar/monthview/month_view.h>
#include <calendar/modelanditem/basic_calendar_model.h>

#include <QRect>
#include <QPushButton>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QLabel>
#include <QMenu>

#include <QDebug>

using namespace Calendar;
using namespace Internal;

namespace {
    enum { RefreshInterval = 60 }; // in secondes
}

struct Calendar::CalendarWidgetPrivate
{
	CalendarWidgetPrivate(CalendarWidget *calendar);
	QVBoxLayout *m_mainLayout;
	QScrollArea *m_scrollArea;
	CalendarNavbar *m_navbar;
	ViewWidget *m_header;
	ViewWidget *m_body;
	int m_dayGranularity;
	int m_dayItemDefaultDuration;
    int m_dayScaleHourDivider;
    int m_hourHeight;
	QTimer m_timer; // used to refresh every date/time stuffs
    QMenu *_contextMenu;
};

CalendarWidgetPrivate::CalendarWidgetPrivate(CalendarWidget *calendar) :
    m_header(0), m_body(0), _contextMenu(0)
{
    m_mainLayout = new QVBoxLayout(calendar);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // scroll area
    m_scrollArea = new QScrollArea;
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameShape(QFrame::NoFrame);

    m_dayGranularity = 15; // in minutes
    m_dayItemDefaultDuration = 30; // in minutes
    m_dayScaleHourDivider = 4;
    m_hourHeight = 40; // in pixels

    // navigation bar
    m_navbar = new CalendarNavbar(calendar);
    m_navbar->setDayGranularity(m_dayGranularity);
    m_mainLayout->addWidget(m_navbar);

    m_mainLayout->addWidget(m_scrollArea);
}

// -----------------------------

CalendarWidget::CalendarWidget(QWidget *parent) :
    QWidget(parent),
    m_d(new CalendarWidgetPrivate(this)),
    m_model(0)
{
    QList<CalendarItem*> list;
    QDateTime begin = QDateTime::currentDateTime();
    QDateTime end = begin;

    // navigation bar stuffs
    connect(m_d->m_navbar, SIGNAL(firstDateChanged()), this, SLOT(firstDateChanged()));
    connect(m_d->m_navbar, SIGNAL(viewTypeChanged()), this, SLOT(viewTypeChanged()));
    connect(m_d->m_navbar, SIGNAL(granularityChanged(int)), this, SLOT(setDayGranularity(int)));
    m_d->m_navbar->setViewType(Calendar::View_Week);
    m_d->m_navbar->setDate(QDate::currentDate());

    // start the hour timer
    m_d->m_timer.setInterval(RefreshInterval * 1000);
    connect(&m_d->m_timer, SIGNAL(timeout()), this, SLOT(timeout()));
    m_d->m_timer.start();
}

/** Define the Calendar::AbstractCalendarModel to use with this view */
void CalendarWidget::setModel(AbstractCalendarModel *model) {
	m_model = model;

	if (m_d->m_header)
		m_d->m_header->setModel(model);
	if (m_d->m_body)
		m_d->m_body->setModel(model);
}

void CalendarWidget::firstDateChanged()
{
    Q_ASSERT(m_d->m_header);
    Q_ASSERT(m_d->m_body);
    m_d->m_header->setFirstDate(m_d->m_navbar->firstDate());
    m_d->m_body->setFirstDate(m_d->m_navbar->firstDate());
}

void CalendarWidget::viewTypeChanged() {
	// delete old view elements. No need to destroy the body since QScrollArea::setWidget() already does.
	if (m_d->m_header)
		delete m_d->m_header;

	Calendar::ViewType viewType = m_d->m_navbar->viewType();
	switch (viewType) {
	case View_Day:
		m_d->m_header = new DayRangeHeader(0, 1);
		m_d->m_body = new DayRangeBody(0, 1);
		break;
	case View_Week:
		m_d->m_header = new DayRangeHeader;
		m_d->m_body = new DayRangeBody;
		break;
	case View_Month:
		m_d->m_header = new MonthHeader;
		m_d->m_body = new MonthBody;
		break;
	default:
		Q_ASSERT(true); // should never happend :)
	}

        DayRangeBody *body = qobject_cast<DayRangeBody*>(m_d->m_body);
        if (body) {
                body->setGranularity(m_d->m_dayGranularity);
                body->setItemDefaultDuration(m_d->m_dayItemDefaultDuration);
                body->setDayScaleHourDivider(m_d->m_dayScaleHourDivider);
                body->setHourHeight(m_d->m_hourHeight);
	}

    m_d->m_scrollArea->verticalScrollBar()->setSingleStep(50);
    m_d->m_scrollArea->setWidget(m_d->m_body);
    m_d->m_body->setFirstDate(m_d->m_navbar->firstDate());
    m_d->m_header->setMasterScrollArea(m_d->m_scrollArea);
    m_d->m_mainLayout->insertWidget(1, m_d->m_header);
    m_d->m_header->setFirstDate(m_d->m_navbar->firstDate());
    m_d->m_header->setModel(m_model);
    m_d->m_body->setModel(m_model);
}

/** Define the calendar view type \e viewType that can be: day view, week view and month view. */
void CalendarWidget::setViewType(Calendar::ViewType viewType) {
	m_d->m_navbar->setViewType(viewType);
}

/** set the view to the selected date */
void CalendarWidget::setDate(const QDate &date) {
    m_d->m_navbar->setDate(date);
}

void CalendarWidget::timeout() {
	m_d->m_body->refreshCurrentDateTimeStuff();
}

/** Return the current view type */
ViewType CalendarWidget::viewType() const {
	return m_d->m_navbar->viewType();
}

int CalendarWidget::dayGranularity() const {
	return m_d->m_dayGranularity;
}

void CalendarWidget::setDayGranularity(int minutes) {
	if (m_d->m_dayGranularity == minutes)
		return;

	m_d->m_dayGranularity = minutes;
	if (qobject_cast<DayRangeBody*>(m_d->m_body))
		qobject_cast<DayRangeBody*>(m_d->m_body)->setGranularity(minutes);
}

int CalendarWidget::dayItemDefaultDuration() const {
	return m_d->m_dayItemDefaultDuration;
}

void CalendarWidget::setDayItemDefaultDuration(int minutes) {
	if (m_d->m_dayItemDefaultDuration == minutes)
		return;

	m_d->m_dayItemDefaultDuration = minutes;
	if (qobject_cast<DayRangeBody*>(m_d->m_body))
		qobject_cast<DayRangeBody*>(m_d->m_body)->setItemDefaultDuration(minutes);
}

int CalendarWidget::dayScaleHourDivider() const {
    return m_d->m_dayScaleHourDivider;
}

void CalendarWidget::setDayScaleHourDivider(int divider) {
	if (m_d->m_dayScaleHourDivider == divider)
		return;

	m_d->m_dayScaleHourDivider = divider;
	if (qobject_cast<DayRangeBody*>(m_d->m_body))
		qobject_cast<DayRangeBody*>(m_d->m_body)->setDayScaleHourDivider(divider);
}

int CalendarWidget::hourHeight() const {
    return m_d->m_hourHeight;
}

void CalendarWidget::setHourHeight(int pixels) {
    if (m_d->m_hourHeight == pixels)
        return;

    m_d->m_hourHeight = pixels;
    if (qobject_cast<DayRangeBody*>(m_d->m_body))
        qobject_cast<DayRangeBody*>(m_d->m_body)->setHourHeight(pixels);
}

/**
 * Define the QMenu to use as contextual menu on calendar items.
 * You can get the calendar item clicked when requesting the contextual menu
 * using the getContextCalendarItem()
 */
void CalendarWidget::setContextMenuForItems(QMenu *menu)
{
    m_d->_contextMenu = menu;
    DayRangeBody *body = qobject_cast<DayRangeBody*>(m_d->m_body);
    if (body)
        body->setContextMenuForItems(menu);
}

/** Return the clicked calendar item when requesting the contextual menu */
Calendar::CalendarItem CalendarWidget::getContextualCalendarItem() const
{
    DayRangeBody *body = qobject_cast<DayRangeBody*>(m_d->m_body);
    if (body)
        return body->getContextualCalendarItem();
    return CalendarItem();
}

void CalendarWidget::scrollToTime(const QTime &time)
{
    int y = (time.hour()-1)*m_d->m_hourHeight;
    m_d->m_scrollArea->verticalScrollBar()->setValue(y);
}
