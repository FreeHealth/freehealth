#ifndef COMMON_H
#define COMMON_H

class QDate;

namespace Calendar {
	enum ViewType {
		View_Day,
		View_Week,
		View_Month
	};

	/** returns the first date of the view in function of a random date */
	QDate getFirstDateByRandomDate(ViewType viewType, const QDate &randomDate);
}

#endif
