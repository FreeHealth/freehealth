#include <QPainter>

#include "calendar_item.h"

using namespace Calendar;

CalendarItem::CalendarItem(QWidget *parent, bool temp) :
	QWidget(parent), m_temp(temp) {
}

void CalendarItem::paintEvent(QPaintEvent *event) {
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	QPen pen = painter.pen();
	pen.setColor(Qt::green);
	pen.setCapStyle(Qt::FlatCap);
	painter.setPen(pen);
	QBrush brush = painter.brush();
	brush.setColor(QColor(0, 150, 0, m_temp ? 200 : 255));
	brush.setStyle(Qt::SolidPattern);
	painter.setBrush(brush);
	painter.drawRoundedRect(rect().adjusted(0, 0, 0, 0), 5, 5);
}

void CalendarItem::setBeginDateTime(const QDateTime &dateTime) {
	m_beginDateTime = dateTime;
}

void CalendarItem::setEndDateTime(const QDateTime &dateTime) {
	m_endDateTime = dateTime;
}
