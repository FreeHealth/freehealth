#include <QPainter>
#include <QPaintEvent>
#include <QRect>
#include <QPushButton>
#include <QVBoxLayout>

#include "calendar_widget.h"
#include "day_view.h"
#include "week_view.h"
#include "month_view.h"

using namespace Calendar;

CalendarWidget::CalendarWidget(QWidget *parent)
	: QWidget(parent),
	  m_view(0),
	  m_header(0),
	  m_model(0) {
	m_mainLayout = new QVBoxLayout(this);
	m_mainLayout->setContentsMargins(0, 0, 0, 0);
	m_mainLayout->setSpacing(0);

	// scroll area
	m_scrollArea = new QScrollArea;
	m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	m_scrollArea->setWidgetResizable(true);
	m_scrollArea->setFrameShape(QFrame::NoFrame);

	// navigation bar
	m_navbar = new CalendarNavbar(this);
	connect(m_navbar, SIGNAL(firstDateChanged()), this, SLOT(firstDateChanged()));
	connect(m_navbar, SIGNAL(viewTypeChanged()), this, SLOT(viewTypeChanged()));
	m_navbar->setViewType(Calendar::View_Week);
	m_navbar->setDate(QDate::currentDate());
	m_mainLayout->insertWidget(0, m_navbar);

	m_mainLayout->addWidget(m_scrollArea);
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
	Q_UNUSED(topLeft);
	Q_UNUSED(bottomRight);
	// TODO
}

void CalendarWidget::rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end) {
	Q_UNUSED(parent);
	Q_UNUSED(start);
	Q_UNUSED(end);
	// TODO
}

void CalendarWidget::rowsAboutToBeInserted(const QModelIndex &parent, int start, int end) {
	Q_UNUSED(parent);
	Q_UNUSED(start);
	Q_UNUSED(end);
	// TODO
}

void CalendarWidget::rowsRemoved(const QModelIndex &parent, int start, int end) {
	Q_UNUSED(parent);
	Q_UNUSED(start);
	Q_UNUSED(end);
	// TODO
}

void CalendarWidget::rowsInserted(const QModelIndex &parent, int start, int end) {
	Q_UNUSED(parent);
	Q_UNUSED(start);
	Q_UNUSED(end);
	// TODO
}

void CalendarWidget::firstDateChanged() {
	m_header->setFirstDate(m_navbar->firstDate());
	m_view->setFirstDate(m_navbar->firstDate());
}

void CalendarWidget::viewTypeChanged() {
	Calendar::ViewType viewType = m_navbar->viewType();
	switch (viewType) {
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

	m_scrollArea->setWidget(m_view);
	m_view->setFirstDate(m_navbar->firstDate());
	if (m_header)
		delete m_header;
	m_header = (WeekHeader *) m_view->createHeaderWidget();
	m_header->setScrollArea(m_scrollArea);
	m_header->setFirstDate(m_navbar->firstDate());
	m_mainLayout->insertWidget(1, m_header);
}

void CalendarWidget::setViewType(Calendar::ViewType viewType) {
	m_navbar->setViewType(viewType);
}
