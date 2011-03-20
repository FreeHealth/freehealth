#include <QPainter>
#include <QPaintEvent>
#include <QRect>
#include <QScrollBar>

#include "calendar_widget.h"
#include "day_view.h"
#include "week_view.h"
#include "month_view.h"

using namespace Calendar;

CalendarWidget::CalendarWidget(QWidget *parent) : QAbstractScrollArea(parent), m_view(0) {
	setViewType(View_Week);
}

void CalendarWidget::paintEvent(QPaintEvent *event) {
	QPainter painter(viewport());
//	painter.setRenderHint(QPainter::Antialiasing);
	QRect r = viewport()->rect();

	painter.translate(QPoint(-horizontalScrollBar()->value(), -verticalScrollBar()->value()));
	m_view->paintBody(&painter, r);
}

void CalendarWidget::resizeEvent(QResizeEvent *event) {
	computeGeometries();
}

void CalendarWidget::computeGeometries() {
	setViewportMargins(m_view->leftHeaderWidth(), m_view->topHeaderHeight(), 0, 0);
	verticalScrollBar()->setRange(0, 100);
//	horizontalScrollBar()->setRange(0, 100);
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

	computeGeometries();
}
