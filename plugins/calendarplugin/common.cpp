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
}
