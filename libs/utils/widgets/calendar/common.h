#ifndef COMMON_H
#define COMMON_H

#include <QPair>

class QDate;

namespace Calendar {
	enum ViewType {
		View_Day,
		View_Week,
		View_Month
	};

	enum DateType {
		Date_DateTime,
		Date_Date
	};

	/** returns the first date of the view in function of a random date */
	QDate getFirstDateByRandomDate(ViewType viewType, const QDate &randomDate);

	/** returns the bounding days interval for a month. WARNING: days are not necessarily inside the month. */
	QPair<QDate, QDate> getBoundingMonthDaysInterval(int year, int month);

	/** returns the bounding days interval for a month. WARNING: days are not necessarily inside the month.
	 * Same function as the previous one but with a QDate instead of a year+month.
	 */
	QPair<QDate, QDate> getBoundingMonthDaysInterval(const QDate &date);
}

#endif
