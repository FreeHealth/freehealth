#include <QRect>
#include <QPainter>
#include <QDate>
#include <QPixmapCache>
#include <QScrollArea>

#include "week_view.h"
#include "calendar_item.h"
#include "common.h"

using namespace Calendar;

int WeekView::m_leftScaleWidth = 60;
int WeekView::m_hourHeight = 40;

QSize WeekHeader::sizeHint() const {
	return QSize(0, 40);
}

void WeekHeader::paintEvent(QPaintEvent *) {
	// fill all in light blue
	QPainter painter(this);
	painter.fillRect(rect(), QColor(220, 220, 255));

	// bottom line
	QPen pen = painter.pen();
	pen.setColor(QColor(200, 200, 255));
	painter.setPen(pen);
	painter.drawLine(0, rect().bottom(), rect().right(), rect().bottom());

	// text
	pen.setColor(QColor(150, 150, 255));
	painter.setPen(pen);

	// vertical lines
	int containWidth = m_scrollArea->viewport()->width() - 60;
	QPen oldPen = painter.pen();
	QFont oldFont = painter.font();
	QDate date = firstDate();
	QDate now = QDate::currentDate();
	for (int i = 0; i < 7; ++i) {
		QRect r(QPoint(60 + (i * containWidth) / 7, 0), QPoint(60 + ((i + 1) * containWidth) / 7, rect().height())); // +2 is a vertical correction to not be stucked to the top line
		if (date == now){
			painter.fillRect(r, QColor(200,200,255));
			QPen pen = painter.pen();;
			pen.setColor(QColor(0, 0, 255));
			painter.setPen(pen);
		}
		r.adjust(0, 2, 0, 0);
		painter.drawText(r, Qt::AlignHCenter | Qt::AlignTop, date.toString("ddd d/M").toLower());
		painter.setPen(oldPen);
		painter.setFont(oldFont);
		date = date.addDays(1);
	}
}

/////////////////////////////////////////////////////////////////

void HourWidget::paintEvent(QPaintEvent *) {
	QPainter painter(this);
	painter.fillRect(rect(), QColor(255, 150, 150));
}

/////////////////////////////////////////////////////////////////

WeekView::WeekView(QWidget *parent) :
	View(parent),
	m_hourWidget(0) {
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	setFirstDate(Calendar::getFirstDateByRandomDate(Calendar::View_Week, QDate::currentDate()));

	CalendarItem *item = new CalendarItem(this);
	item->resize(100, m_hourHeight);
	item->move(m_leftScaleWidth + 1, 0);
}

QSize WeekView::sizeHint() const {
	return QSize(0, 24 * m_hourHeight);
}

int WeekView::topHeaderHeight() const {
	return 40;
}

int WeekView::leftHeaderWidth() const {
	return 0;
}

void WeekView::paintBody(QPainter *painter, const QRect &visibleRect) {
	painter->fillRect(visibleRect, Qt::white);
	QPen pen = painter->pen();
	pen.setColor(QColor(200, 200, 200));
	pen.setCapStyle(Qt::FlatCap);
	painter->setPen(pen);
	int containWidth = visibleRect.width() - m_leftScaleWidth;

	// draw current day?
	QDate now = QDate::currentDate();
	if (now >= m_firstDate && now <= m_firstDate.addDays(7))
		painter->fillRect(m_leftScaleWidth + ((now.dayOfWeek() - 1) * containWidth) / 7, 0,
						  (now.dayOfWeek() * containWidth) / 7 - ((now.dayOfWeek() - 1) * containWidth) / 7, visibleRect.height(),
						  QColor(255, 255, 200));

	// vertical lines
	for (int i = 0; i < 7; ++i) {
		painter->drawLine(m_leftScaleWidth + (i * containWidth) / 7, 0,
						  m_leftScaleWidth + (i * containWidth) / 7, visibleRect.height());
	}

	// hours horizontal lines
	for (int i = 0; i < 24; ++i) {
		painter->drawLine(0, (i + 1) * m_hourHeight,
						  visibleRect.width() - 1, (i + 1) * m_hourHeight);
	}

	// half-hours : optimization : just draw the first dashed line and copy it with drawPixmap because dashed lines are SLOOOW with X11
	QPen oldPen = pen;
	QPixmap dashPixmap(visibleRect.width(), 1);
	QPainter dashPainter(&dashPixmap);
	dashPainter.fillRect(QRect(0, 0, visibleRect.width(), 1), Qt::white);
	QPen dashPen = dashPainter.pen();
	dashPen.setColor(QColor(200, 200, 200));
	dashPen.setCapStyle(Qt::FlatCap);
	dashPen.setDashPattern(QVector<qreal>() << 1 << 1);
	dashPainter.setPen(dashPen);
	dashPainter.drawLine(0, 0, visibleRect.width(), 0);

	pen.setDashPattern(QVector<qreal>() << 1 << 1);
	painter->setPen(pen);
	for (int i = 0; i < 24; ++i) {
		painter->drawPixmap(m_leftScaleWidth, i * m_hourHeight + m_hourHeight / 2,
							visibleRect.width(), 1, dashPixmap);
	}

	painter->setPen(oldPen);

	pen = painter->pen();
	pen.setColor(QColor(120, 120, 120));
	painter->setPen(pen);
	for (int i = 0; i < 24; ++i) {
		QRect scaleRect(QPoint(0, i * m_hourHeight + 1),
						QPoint(m_leftScaleWidth - 3, (i + 1) * m_hourHeight - 1));
		painter->drawText(scaleRect, Qt::AlignRight, QString("%1:00").arg(i, 2, 10, QChar('0')));
	}

	// hour widget
	if (now >= m_firstDate && now <= m_firstDate.addDays(7)) {
		if (!m_hourWidget)
			m_hourWidget = new HourWidget(this);
		// move and resize
		m_hourWidget->resize((now.dayOfWeek() * containWidth) / 7 - ((now.dayOfWeek() - 1) * containWidth) / 7, m_hourWidget->sizeHint().height());

		// compute
		QTime nowTime = QTime::currentTime();
		int y = (rect().height() * nowTime.hour()) / 24;
		int minY = (((rect().height() * (nowTime.hour() + 1)) / 24 - (rect().height() * nowTime.hour()) / 24) * nowTime.minute()) / 60;

		m_hourWidget->move(m_leftScaleWidth + ((now.dayOfWeek() - 1) * containWidth) / 7, y + minY);
		m_hourWidget->show();

	} else if (m_hourWidget) {
		delete m_hourWidget;
		m_hourWidget = 0;
	}
}

QWidget *WeekView::createHeaderWidget(QWidget *parent) {
	WeekHeader *widget = new WeekHeader(parent);
	widget->setFirstDate(m_firstDate);
	return widget;
}
