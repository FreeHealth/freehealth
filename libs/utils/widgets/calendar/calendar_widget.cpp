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
	ViewWidget *m_header;
	ViewWidget *m_body;
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
//	m_navbar = new CalendarNavbar(calendar);
	m_mainLayout->addWidget(m_navbar = new CalendarNavbar(calendar));
//	m_mainLayout->insertWidget(0, m_navbar);

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

ViewType CalendarWidget::viewType() const{
	return m_d->m_navbar->viewType();
}
