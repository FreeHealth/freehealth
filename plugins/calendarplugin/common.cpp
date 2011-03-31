#include <QDate>

#include "common.h"

namespace Calendar {
	QDate getFirstDateByRandomDate(ViewType viewType, const QDate &randomDate) {
		switch (viewType) {
		case View_Day:
			return randomDate;
		case View_Week:
			return randomDate.addDays(-randomDate.dayOfWeek() + 1);
		case View_Month:
			return randomDate.addDays(- randomDate.day() + 1);
		default: // should never happend
			return QDate();
		}
	}

	QPair<QDate, QDate> getBoundingMonthDaysInterval(int year, int month) {
		QDate firstDayOfMonth(year, month, 1);

		// get the very first day of the grid (not necessarily the first day of the month)
		QDate veryFirstDay = firstDayOfMonth.addDays(-firstDayOfMonth.dayOfWeek() + 1);

		// get the very last day of the grid (not necessarily the last day of the month)
		QDate veryLastDay = firstDayOfMonth.addDays(firstDayOfMonth.daysInMonth() - 1);
		veryLastDay = veryLastDay.addDays(7 - veryLastDay.dayOfWeek());

		return QPair<QDate,QDate>(veryFirstDay, veryLastDay);
	}

	QPair<QDate, QDate> getBoundingMonthDaysInterval(const QDate &date) {
		return getBoundingMonthDaysInterval(date.year(), date.month());
	}
}
