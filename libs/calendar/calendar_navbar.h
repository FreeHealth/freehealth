#ifndef CALENDAR_NAVBAR_H
#define CALENDAR_NAVBAR_H

#include <QWidget>
#include <QDate>

#include "common.h"

QT_BEGIN_NAMESPACE
class QPushButton;
class QToolButton;
class QLabel;
class QComboBox;
QT_END_NAMESPACE

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

        public Q_SLOTS:
                void setDayGranularity(const int durationInMinutes);

        Q_SIGNALS:
		void firstDateChanged();
		void viewTypeChanged();
                void granularityChanged(int);

        private Q_SLOTS:
		void todayPage();
		void yesterdayPage();
		void tomorrowPage();
		void previousPage();
		void nextPage();
		void dayMode();
		void weekMode();
		void monthMode();
                void changeGranularity(const int index);

	private:
		ViewType m_viewType;
		QDate m_firstDate;
		QToolButton *m_todayButton;
		QPushButton *m_previousPageButton;
		QPushButton *m_nextPageButton;
		QPushButton *m_dayButton;
		QPushButton *m_weekButton;
		QPushButton *m_monthButton;
		QLabel *m_dateLabel;
                QComboBox *m_granularity;


		void refreshInfos();
		QString getDateIntervalString();
		QToolButton *createTodayButton();
	};
}

#endif
