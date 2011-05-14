#include <QPainter>
#include <QMouseEvent>

#include "day_item_widget.h"

using namespace Calendar;

DayItemWidget::DayItemWidget(QWidget *parent, const QString &uid) : CalendarItemWidget(parent, uid) {
	m_inMotion = uid.isEmpty();
	setMouseTracking(true);
}

void DayItemWidget::paintEvent(QPaintEvent *) {
	// title
	QPixmap titlePixmap(size().width(), 20);
	titlePixmap.fill(Qt::transparent);
	QPainter titlePainter(&titlePixmap);
	titlePainter.setRenderHint(QPainter::Antialiasing);
	titlePainter.setPen(Qt::NoPen);
	QBrush brush = titlePainter.brush();
	brush.setStyle(Qt::SolidPattern);
	brush.setColor(QColor(0, 100, 0, m_inMotion ? 200 : 255));
	titlePainter.setBrush(brush);
	titlePainter.drawRoundedRect(QRect(0, 0, width(), 20), 5, 5);

	// text
	QRect r = rect();
	r.setHeight(15);
	r.adjust(2, 0, 0, 0);
	QPen fontPen;
	fontPen.setColor(Qt::white);
	titlePainter.setPen(fontPen);
	QFont font = titlePainter.font();
	font.setPixelSize(10);
	font.setBold(true);
	titlePainter.setFont(font);
	titlePainter.drawText(r, Qt::AlignLeft | Qt::AlignVCenter, QString("%1 - %2").arg(beginDateTime().time().toString("hh:mm")).arg(endDateTime().time().toString("hh:mm")));

	// body
	QPixmap bodyPixmap(size().width(), rect().height());
	bodyPixmap.fill(Qt::transparent);
	QPainter bodyPainter(&bodyPixmap);
	bodyPainter.setRenderHint(QPainter::Antialiasing);
	bodyPainter.setPen(QColor(0, 100, 0, m_inMotion ? 200 : 255));
	brush = bodyPainter.brush();
	brush.setStyle(Qt::SolidPattern);
	brush.setColor(QColor(0, 200, 0, m_inMotion ? 200 : 255));
	bodyPainter.setBrush(brush);
	bodyPainter.drawRoundedRect(QRect(0, 0, width(), height()), 5, 5);

	// main
	QPainter painter(this);
	painter.drawPixmap(QPoint(0, 0), titlePixmap, QRect(0, 0, width(), 15));
	painter.drawPixmap(QPoint(0, 15), bodyPixmap, QRect(0, 15, width(), rect().height() - 15));
}

void DayItemWidget::setInMotion(bool value) {
	if (m_inMotion == value)
		return;

	m_inMotion = value;
}

void DayItemWidget::mouseMoveEvent(QMouseEvent *event) {
	if (event->pos().y() >= height() - 5 && event->pos().y() < height())
		setCursor(Qt::SizeVerCursor);
	else
		setCursor(Qt::ArrowCursor);

	QWidget::mouseMoveEvent(event);
}
