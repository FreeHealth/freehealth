#ifndef CALENDAR_NAVBAR_H
#define CALENDAR_NAVBAR_H

#include <QWidget>

class QPushButton;

namespace Calendar {
	class CalendarNavbar : public QWidget
	{
		Q_OBJECT
	public:
		CalendarNavbar(QWidget *parent = 0);

	private:
		QPushButton *m_todayButton;
		QPushButton *m_previousPageButton;
		QPushButton *m_nextPageButton;
		QPushButton *m_dayButton;
		QPushButton *m_weekButton;
		QPushButton *m_monthButton;
	};
}

#endif
