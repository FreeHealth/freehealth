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

CalendarWidget::CalendarWidget(QWidget *parent)
	: QScrollArea(parent),
	  m_view(0),
	  m_header(0),
	  m_model(0) {
	setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	setViewType(View_Week);
	setViewportMargins(0, 80, 0, 0);
	m_navbar = new CalendarNavbar(this);
	connect(m_navbar, SIGNAL(todayPage()), this, SLOT(todayPageRequested()));
	connect(m_navbar, SIGNAL(previousPage()), this, SLOT(previousPageRequested()));
	connect(m_navbar, SIGNAL(nextPage()), this, SLOT(nextPageRequested()));
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
	if (m_header)
		delete m_header;
	m_header = (WeekHeader *) m_view->createHeaderWidget(this);
	connect(m_view, SIGNAL(firstDateChanged()), this, SLOT(firstDateChanged()));
}

void CalendarWidget::resizeEvent(QResizeEvent *event) {
	widget()->resize(event->size().width(), widget()->height());

	QMargins margins = contentsMargins();

	// navigation bar
	m_navbar->resize(event->size().width(), m_navbar->sizeHint().height());
	m_navbar->move(margins.left(), margins.top());

	// top header
	m_header->resize(event->size().width(), 40 - margins.top() + 2);
	m_header->move(margins.left(), margins.top() + m_navbar->height());

	QScrollArea::resizeEvent(event);
}

void CalendarWidget::setModel(QAbstractItemModel *model) {
	// disconnect slots
	if (m_model){
		disconnect(m_model, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(dataChanged(const QModelIndex &, const QModelIndex &)));
		disconnect(m_model, SIGNAL(rowsAboutToBeInserted(const QModelIndex &, int, int)), this, SLOT(rowsAboutToBeInserted(const QModelIndex &, int , int)));
		disconnect(m_model, SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(rowsInserted(const QModelIndex &, int , int)));
		disconnect(m_model, SIGNAL(rowsAboutToBeRemoved(const QModelIndex &, int, int)), this, SLOT(rowsAboutToBeRemoved(const QModelIndex &, int , int)));
		disconnect(m_model, SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(rowsRemoved(const QModelIndex &, int , int)));
	}

	m_model = model;

	if (m_model) {
		// connect slots
		connect(m_model, SIGNAL(dataChanged(const QModelIndex &, const  QModelIndex &)), this, SLOT(dataChanged(const QModelIndex &, const QModelIndex &)));
		connect(m_model, SIGNAL(rowsAboutToBeInserted(const QModelIndex &, int, int)), this, SLOT(rowsAboutToBeInserted(const QModelIndex &, int , int)));
		connect(m_model, SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(rowsInserted(const QModelIndex &, int , int)));
		connect(m_model, SIGNAL(rowsAboutToBeRemoved(const QModelIndex &, int, int)), this, SLOT(rowsAboutToBeRemoved(const QModelIndex &, int , int)));
		connect(m_model, SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(rowsRemoved(const QModelIndex &, int , int)));
	}
}

void CalendarWidget::dataChanged(const QModelIndex &topLeft, const  QModelIndex &bottomRight) {
	// TODO
}

void CalendarWidget::rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end) {
	// TODO
}

void CalendarWidget::rowsAboutToBeInserted(const QModelIndex &parent, int start, int end) {
	// TODO
}

void CalendarWidget::rowsRemoved(const QModelIndex &parent, int start, int end) {
	// TODO
}

void CalendarWidget::rowsInserted(const QModelIndex &parent, int start, int end) {
	// TODO
}

void CalendarWidget::previousPageRequested() {
	m_view->previousPage();
}

void CalendarWidget::nextPageRequested() {
	m_view->nextPage();
}

void CalendarWidget::todayPageRequested() {
	m_view->todayPage();
}

void CalendarWidget::firstDateChanged() {
	m_header->setFirstDate(m_view->firstDate());
}

