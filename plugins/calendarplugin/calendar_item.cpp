#include <QPainter>

#include "calendar_item.h"

using namespace Calendar;

CalendarItem::CalendarItem(QWidget *parent) :
	QWidget(parent) {
}

void CalendarItem::paintEvent(QPaintEvent *event) {
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	QPen pen = painter.pen();
	pen.setColor(Qt::blue);
	pen.setCapStyle(Qt::FlatCap);
	painter.setPen(pen);
	QBrush brush = painter.brush();
	brush.setColor(QColor(200, 200, 255));
	brush.setStyle(Qt::SolidPattern);
	painter.setBrush(brush);
	painter.drawRoundedRect(rect().adjusted(0, 0, 0, 0), 5, 5);
}

