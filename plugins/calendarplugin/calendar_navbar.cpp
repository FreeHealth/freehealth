#include <QHBoxLayout>
#include <QPushButton>

#include "calendar_navbar.h"

using namespace Calendar;

CalendarNavbar::CalendarNavbar(QWidget *parent) :
	QWidget(parent) {
	setAutoFillBackground(true);
	QPalette palette = this->palette();
	palette.setColor(QPalette::Window, QColor(180, 180, 255));
	this->setPalette(palette);

	QHBoxLayout *layout = new QHBoxLayout(this);
	m_todayButton = new QPushButton(tr("Today"));
	layout->addWidget(m_todayButton);
	m_previousPageButton = new QPushButton("<<");
	layout->addWidget(m_previousPageButton);
	m_nextPageButton = new QPushButton(">>");
	layout->addWidget(m_nextPageButton);
	layout->addStretch();
	layout->addWidget(m_dayButton = new QPushButton(tr("Day")));
	layout->addWidget(m_weekButton = new QPushButton(tr("Week")));
	layout->addWidget(m_monthButton = new QPushButton(tr("Month")));
}
