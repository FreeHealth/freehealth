#include <QPainter>

#include "calendar_item.h"

using namespace Calendar;

CalendarItem::CalendarItem(QWidget *parent) :
	QWidget(parent) {
}

void CalendarItem::paintEvent(QPaintEvent *event) {
	QPainter painter(this);
	painter.fillRect(rect(), Qt::blue);
}

