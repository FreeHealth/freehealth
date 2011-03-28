#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QDate>

#include "calendar_navbar.h"

using namespace Calendar;

CalendarNavbar::CalendarNavbar(QWidget *parent) :
	QWidget(parent) {
	setAutoFillBackground(true);
	QPalette palette = this->palette();
	palette.setColor(QPalette::Window, QColor(180, 180, 255));
	this->setPalette(palette);

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->addWidget(m_todayButton = new QPushButton(tr("Today")));
	layout->addWidget(m_previousPageButton = new QPushButton("<<"));
	layout->addWidget(m_nextPageButton = new QPushButton(">>"));
	layout->addWidget(m_dateLabel = new QLabel);
	QFont font = m_dateLabel->font();
	font.setBold(true);
	m_dateLabel->setFont(font);
	layout->addStretch();
	layout->addWidget(m_dayButton = new QPushButton(tr("Day")));
	layout->addWidget(m_weekButton = new QPushButton(tr("Week")));
	layout->addWidget(m_monthButton = new QPushButton(tr("Month")));

	// signal/slot connections
	connect(m_todayButton, SIGNAL(clicked()), this, SLOT(todayPage()));
	connect(m_previousPageButton, SIGNAL(clicked()), this, SLOT(previousPage()));
	connect(m_nextPageButton, SIGNAL(clicked()), this, SLOT(nextPage()));
	connect(m_dayButton, SIGNAL(clicked()), this, SLOT(dayMode()));
	connect(m_weekButton, SIGNAL(clicked()), this, SLOT(weekMode()));
	connect(m_monthButton, SIGNAL(clicked()), this, SLOT(monthMode()));
}

void CalendarNavbar::setViewType(ViewType viewType) {
	if (viewType == m_viewType)
		return;

	m_viewType = viewType;

	refreshInfos();

	emit viewTypeChanged();
}

void CalendarNavbar::setDate(const QDate &date) {
	QDate firstDate = getFirstDateByRandomDate(m_viewType, date);
	if (firstDate == m_firstDate)
		return;

	m_firstDate = firstDate;
	refreshInfos();
	emit firstDateChanged();
}

void CalendarNavbar::refreshInfos() {
	// TODO (refresh label, etc...)
	switch (m_viewType){
	case View_Day:
		m_dateLabel->setText(m_firstDate.toString());
		break;
	case View_Week:
		m_dateLabel->setText(getDateIntervalString());
		break;
	case View_Month:
		m_dateLabel->setText(m_firstDate.toString("MMMM yyyy"));
		break;
	default:; // should never happends
	}
}

void CalendarNavbar::todayPage() {
	setDate(QDate::currentDate());
}

void CalendarNavbar::previousPage() {
	switch (m_viewType) {
	case View_Day:
		setDate(m_firstDate.addDays(-1));
		break;
	case View_Week:
		setDate(m_firstDate.addDays(-7));
		break;
	case View_Month:
		// TODO
		setDate(m_firstDate); // WRONG
		break;
	default: // should never happend
		break;
	}
}

void CalendarNavbar::nextPage() {
	switch (m_viewType) {
	case View_Day:
		setDate(m_firstDate.addDays(1));
		break;
	case View_Week:
		setDate(m_firstDate.addDays(7));
		break;
	case View_Month:
		// TODO
		setDate(m_firstDate); // WRONG
		break;
	default: // should never happend
		break;
	}
}

void CalendarNavbar::dayMode() {
	setViewType(Calendar::View_Day);
}

void CalendarNavbar::weekMode() {
	setViewType(Calendar::View_Week);
}

void CalendarNavbar::monthMode() {
	setViewType(Calendar::View_Month);
}

QString CalendarNavbar::getDateIntervalString() {
	QDate lastDate;
	switch (m_viewType) {
	case View_Day:
		return m_firstDate.toString();
	case View_Week:
		lastDate = m_firstDate.addDays(6);
		if (m_firstDate.month() != lastDate.month() && m_firstDate.year() != lastDate.year())
			return QString("%1 - %2").arg(m_firstDate.toString(tr("d MMM yyyy"))).arg(lastDate.toString(tr("d MMM yyyy")));
		else if (m_firstDate.month() != lastDate.month())
			return QString("%1 - %2").arg(m_firstDate.toString(tr("d MMM"))).arg(lastDate.toString(tr("d MMM yyyy")));
		else
			return QString("%1 - %2").arg(m_firstDate.toString(tr("d"))).arg(lastDate.toString(tr("d MMM yyyy")));
	case View_Month:
//			return randomDate.addDays(- randomDate.day() + 1);
	default: // should never happend
		return "";
	}
}
