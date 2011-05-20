#include "calendar_item_widget.h"

using namespace Calendar;

CalendarItemWidget::CalendarItemWidget(QWidget *parent, const QString &uid, AbstractCalendarModel *model) :
	QWidget(parent), m_uid(uid), m_model(model) {
}

void CalendarItemWidget::setBeginDateTime(const QDateTime &dateTime) {
	if (m_beginDateTime == dateTime)
		return;

	m_beginDateTime = dateTime;
}

void CalendarItemWidget::setEndDateTime(const QDateTime &dateTime) {
	if (m_endDateTime == dateTime)
		return;

	m_endDateTime = dateTime;
}
