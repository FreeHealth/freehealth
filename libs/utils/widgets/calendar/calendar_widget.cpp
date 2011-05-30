#include <QPainter>
#include <QPaintEvent>
#include <QRect>
#include <QPushButton>
#include <QVBoxLayout>

#include "day_range_view.h"
#include "month_view.h"
#include "basic_calendar_model.h"
#include "calendar_widget.h"
#include "calendar_navbar.h"

using namespace Calendar;

#define REFRESH_INTERVAL 60 // in seconds

struct Calendar::CalendarWidgetPrivate
{
	CalendarWidgetPrivate(CalendarWidget *calendar);
	QVBoxLayout *m_mainLayout;
	QScrollArea *m_scrollArea;
	CalendarNavbar *m_navbar;
	View *m_view;
	ViewHeader *m_header;
	QTimer m_timer; // used to refresh every date/time stuffs
};

CalendarWidgetPrivate::CalendarWidgetPrivate(CalendarWidget *calendar) :
	m_view(0),
	m_header(0) {
	m_mainLayout = new QVBoxLayout(calendar);
	m_mainLayout->setContentsMargins(0, 0, 0, 0);
	m_mainLayout->setSpacing(0);

	// scroll area
	m_scrollArea = new QScrollArea;
	m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	m_scrollArea->setWidgetResizable(true);
	m_scrollArea->setFrameShape(QFrame::NoFrame);

	// navigation bar
	m_navbar = new CalendarNavbar(calendar);
	m_mainLayout->insertWidget(0, m_navbar);

	m_mainLayout->addWidget(m_scrollArea);
}

// -----------------------------

CalendarWidget::CalendarWidget(QWidget *parent)
	: QWidget(parent),
	  m_d(new CalendarWidgetPrivate(this)),
	  m_model(0) {

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
	// disconnect slots
	if (m_model){
	}

	m_model = model;

	if (m_model) {
		// connect slots
	}
	if (m_d->m_view)
		m_d->m_view->setModel(model);
	if (m_d->m_header)
		m_d->m_header->setModel(model);
}

void CalendarWidget::firstDateChanged() {
	m_d->m_header->setFirstDate(m_d->m_navbar->firstDate());
	m_d->m_view->setFirstDate(m_d->m_navbar->firstDate());
}

void CalendarWidget::viewTypeChanged() {
	Calendar::ViewType viewType = m_d->m_navbar->viewType();
	switch (viewType) {
	case View_Day:
		m_d->m_view = new DayRangeView(0, 1);
		break;
	case View_Week:
		m_d->m_view = new DayRangeView;
		break;
	case View_Month:
		m_d->m_view = new MonthView;
		break;
	default:
		Q_ASSERT(true); // should never happend :)
	}

	m_d->m_scrollArea->setWidget(m_d->m_view);
	m_d->m_view->setFirstDate(m_d->m_navbar->firstDate());
	if (m_d->m_header)
		delete m_d->m_header;
	m_d->m_header = m_d->m_view->createHeaderWidget();
	m_d->m_header->setScrollArea(m_d->m_scrollArea);
	m_d->m_header->setFirstDate(m_d->m_navbar->firstDate());
	m_d->m_mainLayout->insertWidget(1, m_d->m_header);
	m_d->m_view->setModel(m_model);
	m_d->m_view->setModel(m_model);
}

void CalendarWidget::setViewType(Calendar::ViewType viewType) {
	m_d->m_navbar->setViewType(viewType);
}

void CalendarWidget::timeout() {
	m_d->m_view->refreshCurrentDateTimeStuff();
}

ViewType CalendarWidget::viewType() const{
	return m_d->m_navbar->viewType();
}
