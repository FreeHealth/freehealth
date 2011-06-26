#ifndef COMMON_H
#define COMMON_H

#include <QPair>

class QDate;
class QDateTime;
class QTime;

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

	/** compute an intersection value (int) between a range of QDateTime and a day range
	 * @param beginning The lower bound of the QDateTime range
	 * @param ending The upper bound of the QDateTime range
	 * @param firstDay The lower bound of the day range
	 * @param lastDay The upper bound of the QDateTime range
	 * @return -1 if the range is entirely before first day
	 * @return 0 if the range intersects [firstDay, lastDay]
	 * @return 1 if the range is entirely after lastDay
	 */
	int intersectsDays(const QDateTime &beginning, const QDateTime &ending,
					   const QDate &firstDay, const QDate &lastDay);

	/** returns the first and last day intersecting a given range
	 * Take account of 00:00AM for ending datetime
	 * \param beginning A QDateTime representing the lower bound of a QDateTime range
	 * \param ending A QDateTime representing the upper bound of a QDateTime range
	 * \return {QPair<QDate,QDate>} The first and last day intersecting the given range
	 */
	QPair<QDate, QDate> getIntersectDayRange(const QDateTime &beginning, const QDateTime &ending);

	/** returns the top and the height of an item, taking account of a minimum item height */
	QPair<int, int> getItemTopAndHeight(const QTime &begin, const QTime &end, int hourHeight, int minimumItemHeight);
}

#endif
