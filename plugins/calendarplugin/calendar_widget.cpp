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
	setViewType(View_Week);
	setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	setViewportMargins(0, 40, 0, 0);
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
	m_header = m_view->createHeaderWidget(this);
}

void CalendarWidget::resizeEvent(QResizeEvent *event) {
	widget()->resize(event->size().width(), widget()->height());

	QMargins margins = contentsMargins();
	m_header->resize(event->size().width(), 40 - margins.top() + 2);
	m_header->move(margins.left(), margins.top());

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

void CalendarWidget::dataChanged(const QModelIndex & topLeft, const  QModelIndex & bottomRight) {
	// TODO
}

void CalendarWidget::rowsAboutToBeRemoved(const QModelIndex & parent, int start, int end) {
	// TODO
}

void CalendarWidget::rowsAboutToBeInserted(const QModelIndex & parent, int start, int end) {
	// TODO
}

void CalendarWidget::rowsRemoved(const QModelIndex & parent, int start, int end) {
	// TODO
}

void CalendarWidget::rowsInserted(const QModelIndex & parent, int start, int end) {
	// TODO
}

