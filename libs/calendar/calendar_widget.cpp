/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developpers :                                                    *
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "calendar_widget.h"
#include "day_range_view.h"
#include "month_view.h"
#include "basic_calendar_model.h"
#include "calendar_navbar.h"

#include <QPainter>
#include <QPaintEvent>
#include <QRect>
#include <QPushButton>
#include <QVBoxLayout>

using namespace Calendar;

namespace {
    enum { REFRESH_INTERVAL = 60 }; // in secondes
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
	QTimer m_timer; // used to refresh every date/time stuffs
};

CalendarWidgetPrivate::CalendarWidgetPrivate(CalendarWidget *calendar)
	: m_header(0), m_body(0) {
	m_mainLayout = new QVBoxLayout(calendar);
	m_mainLayout->setContentsMargins(0, 0, 0, 0);
	m_mainLayout->setSpacing(0);

	// scroll area
	m_scrollArea = new QScrollArea;
	m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	m_scrollArea->setWidgetResizable(true);
	m_scrollArea->setFrameShape(QFrame::NoFrame);

	// navigation bar
	m_mainLayout->addWidget(m_navbar = new CalendarNavbar(calendar));

	m_mainLayout->addWidget(m_scrollArea);

	m_dayGranularity = 15;
}

// -----------------------------

CalendarWidget::CalendarWidget(QWidget *parent)
	: QWidget(parent),
	  m_d(new CalendarWidgetPrivate(this)),
	  m_model(0) {

	CalendarItem *item;
	QList<CalendarItem*> list;
	QDateTime begin = QDateTime::currentDateTime();
	QDateTime end = begin;

	// navigation bar stuffs
	connect(m_d->m_navbar, SIGNAL(firstDateChanged()), this, SLOT(firstDateChanged()));
	connect(m_d->m_navbar, SIGNAL(viewTypeChanged()), this, SLOT(viewTypeChanged()));
	m_d->m_navbar->setViewType(Calendar::View_Week);
	m_d->m_navbar->setDate(QDate::currentDate());

	// basic model
	setModel(new BasicCalendarModel(this));

	// start the hour timer
	m_d->m_timer.setInterval(REFRESH_INTERVAL * 1000);
	connect(&m_d->m_timer, SIGNAL(timeout()), this, SLOT(timeout()));
	m_d->m_timer.start();
}

void CalendarWidget::setModel(AbstractCalendarModel *model) {
	m_model = model;

	if (m_d->m_header)
		m_d->m_header->setModel(model);
	if (m_d->m_body)
		m_d->m_body->setModel(model);
}

void CalendarWidget::firstDateChanged() {
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

	if (qobject_cast<DayRangeBody*>(m_d->m_body))
		qobject_cast<DayRangeBody*>(m_d->m_body)->setGranularity(m_d->m_dayGranularity);

	m_d->m_scrollArea->setWidget(m_d->m_body);
	m_d->m_body->setFirstDate(m_d->m_navbar->firstDate());
	m_d->m_header->setMasterScrollArea(m_d->m_scrollArea);
	m_d->m_mainLayout->insertWidget(1, m_d->m_header);
	m_d->m_header->setFirstDate(m_d->m_navbar->firstDate());
	m_d->m_header->setModel(m_model);
	m_d->m_body->setModel(m_model);
}

void CalendarWidget::setViewType(Calendar::ViewType viewType) {
	m_d->m_navbar->setViewType(viewType);
}

void CalendarWidget::timeout() {
	m_d->m_body->refreshCurrentDateTimeStuff();
}

ViewType CalendarWidget::viewType() const {
	return m_d->m_navbar->viewType();
}

int CalendarWidget::dayGranularity() const {
	return m_d->m_dayGranularity;
}

void CalendarWidget::setDayGranularity(int value) {
	if (m_d->m_dayGranularity == value)
		return;

	m_d->m_dayGranularity = value;
	if (qobject_cast<DayRangeBody*>(m_d->m_body))
		qobject_cast<DayRangeBody*>(m_d->m_body)->setGranularity(m_d->m_dayGranularity);
}
