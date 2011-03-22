#include <QPainter>
#include <QPaintEvent>
#include <QRect>
#include <QScrollBar>
#include <QPushButton>

#include "calendar_widget.h"
#include "day_view.h"
#include "week_view.h"
#include "month_view.h"

using namespace Calendar;

CalendarWidget::CalendarWidget(QWidget *parent) : QScrollArea(parent), m_view(0) {
	setViewType(View_Week);
	setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	setViewportMargins(0, 20, 0, 0);
}

void CalendarWidget::setViewType(ViewType value) {
	if (value == m_viewType)
		return;

	m_viewType = value;

	if (m_view)
		delete m_view;

	// TODO : refresh the calendar
	switch (m_viewType) {
	case View_Day:
		m_view = new DayView;
		break;
	case View_Week:
		m_view = new WeekView;
		break;
	case View_Month:
		m_view = new MonthView;
		break;
	default:
		Q_ASSERT(true); // should never happend :)
	}

	setWidget(m_view);
//	QWidget *headerW = m_view->createHeaderWidget(this);
}

void CalendarWidget::resizeEvent(QResizeEvent *event) {
	widget()->resize(event->size().width(), widget()->height());

	QScrollArea::resizeEvent(event);
}
