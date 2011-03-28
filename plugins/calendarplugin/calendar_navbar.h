#ifndef CALENDAR_NAVBAR_H
#define CALENDAR_NAVBAR_H

#include <QWidget>
#include <QDate>

#include "common.h"

class QPushButton;
class QLabel;

namespace Calendar {
	class CalendarNavbar : public QWidget
	{
		Q_OBJECT
	public:
		CalendarNavbar(QWidget *parent = 0);

		void setViewType(ViewType viewType);
		ViewType viewType() const { return m_viewType; }

		const QDate &firstDate() const { return m_firstDate; }
		void setDate(const QDate &date);

	public slots:

	signals:
		void firstDateChanged();
		void viewTypeChanged();

	private slots:
		void todayPage();
		void previousPage();
		void nextPage();
		void dayMode();
		void weekMode();
		void monthMode();

	private:
		ViewType m_viewType;
		QDate m_firstDate;
		QPushButton *m_todayButton;
		QPushButton *m_previousPageButton;
		QPushButton *m_nextPageButton;
		QPushButton *m_dayButton;
		QPushButton *m_weekButton;
		QPushButton *m_monthButton;
		QLabel *m_dateLabel;

		void refreshInfos();
		QString getDateIntervalString();
	};
}

#endif
