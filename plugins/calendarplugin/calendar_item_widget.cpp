#include <QPainter>
#include <QBitmap>

#include "calendar_item_widget.h"

using namespace Calendar;

CalendarItemWidget::CalendarItemWidget(QWidget *parent, const QString &uid) :
	QWidget(parent), m_uid(uid) {
	m_inMotion = m_uid.isEmpty();
}

void CalendarItemWidget::paintEvent(QPaintEvent *) {
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
	titlePainter.drawText(r, Qt::AlignLeft | Qt::AlignVCenter, QString("%1 - %2").arg(m_beginDateTime.time().toString("hh:mm")).arg(m_endDateTime.time().toString("hh:mm")));

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

void CalendarItemWidget::setInMotion(bool value) {
	if (m_inMotion == value)
		return;

	m_inMotion = value;
}
