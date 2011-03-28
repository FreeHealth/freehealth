#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

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
	layout->addStretch();
	layout->addWidget(m_dayButton = new QPushButton(tr("Day")));
	layout->addWidget(m_weekButton = new QPushButton(tr("Week")));
	layout->addWidget(m_monthButton = new QPushButton(tr("Month")));

	// signal/slot connections
	connect(m_todayButton, SIGNAL(clicked()), this, SIGNAL(todayPage()));
	connect(m_previousPageButton, SIGNAL(clicked()), this, SIGNAL(previousPage()));
	connect(m_nextPageButton, SIGNAL(clicked()), this, SIGNAL(nextPage()));
}
