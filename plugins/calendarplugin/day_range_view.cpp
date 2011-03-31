#include <QRect>
#include <QPainter>
#include <QDate>
#include <QPixmapCache>
#include <QScrollArea>

#include "day_range_view.h"
#include "calendar_item.h"
#include "common.h"

using namespace Calendar;

int DayRangeView::m_leftScaleWidth = 60;
int DayRangeView::m_hourHeight = 40;

QSize DayRangeHeader::sizeHint() const {
	return QSize(0, 40);
}

void DayRangeHeader::paintEvent(QPaintEvent *) {
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
		QRect r(QPoint(60 + (i * containWidth) / 7, 0), QPoint(60 + ((i + 1) * containWidth) / 7, rect().height()));
		if (date == now){
			painter.fillRect(r, QColor(200,200,255));
			QPen pen = painter.pen();;
			pen.setColor(QColor(0, 0, 255));
			painter.setPen(pen);
		}
		r.adjust(0, 2, 0, 0);  // +2 is a vertical correction to not be stucked to the top line
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

DayRangeView::DayRangeView(QWidget *parent, int rangeWidth) :
	View(parent),
	m_hourWidget(0),
	m_rangeWidth(rangeWidth) {
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	setFirstDate(Calendar::getFirstDateByRandomDate(Calendar::View_Week, QDate::currentDate()));

	CalendarItem *item = new CalendarItem(this);
	QDate now = QDate::currentDate();
	item->setBeginDateTime(QDateTime(now, QTime(7, 0, 0)));
	item->setEndDateTime(QDateTime(now, QTime(8, 0, 0)));

	item = new CalendarItem(this);
	item->setBeginDateTime(QDateTime(now, QTime(10, 0, 0)));
	item->setEndDateTime(QDateTime(now, QTime(15, 45, 0)));

	item = new CalendarItem(this);
	item->setBeginDateTime(QDateTime(now.addDays(1), QTime(2, 0, 0)));
	item->setEndDateTime(QDateTime(now.addDays(1), QTime(13, 15, 0)));
}

QSize DayRangeView::sizeHint() const {
	return QSize(0, 24 * m_hourHeight);
}

int DayRangeView::topHeaderHeight() const {
	return 40;
}

int DayRangeView::leftHeaderWidth() const {
	return 0;
}

void DayRangeView::paintBody(QPainter *painter, const QRect &visibleRect) {
	painter->fillRect(visibleRect, Qt::white);
	QPen pen = painter->pen();
	pen.setColor(QColor(200, 200, 200));
	pen.setCapStyle(Qt::FlatCap);
	painter->setPen(pen);
	int containWidth = visibleRect.width() - m_leftScaleWidth;

	// draw current day?
	QDate now = QDate::currentDate();
	if (now >= m_firstDate && now < m_firstDate.addDays(m_rangeWidth)){
		int day = now.dayOfWeek() - m_firstDate.dayOfWeek();
		painter->fillRect(m_leftScaleWidth + (day * containWidth) / m_rangeWidth, 0,
						  ((day + 1) * containWidth) / m_rangeWidth - (day * containWidth) / m_rangeWidth, visibleRect.height(),
						  QColor(255, 255, 200));
	}

	// vertical lines
	for (int i = 0; i < m_rangeWidth; ++i) {
		painter->drawLine(m_leftScaleWidth + (i * containWidth) / m_rangeWidth, 0,
						  m_leftScaleWidth + (i * containWidth) / m_rangeWidth, visibleRect.height());
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
	if (now >= m_firstDate && now < m_firstDate.addDays(m_rangeWidth)) {
		if (!m_hourWidget)
			m_hourWidget = new HourWidget(this);

		int day = now.dayOfWeek() - m_firstDate.dayOfWeek();

		// move and resize
		m_hourWidget->resize(((day + 1) * containWidth) / m_rangeWidth - (day * containWidth) / m_rangeWidth, m_hourWidget->sizeHint().height());

		// compute
		QTime nowTime = QTime::currentTime();
		int y = (rect().height() * nowTime.hour()) / 24;
		int minY = (((rect().height() * (nowTime.hour() + 1)) / 24 - (rect().height() * nowTime.hour()) / 24) * nowTime.minute()) / 60;

		m_hourWidget->move(m_leftScaleWidth + (day * containWidth) / m_rangeWidth, y + minY);
		m_hourWidget->raise();
		m_hourWidget->show();

	} else if (m_hourWidget) {
		delete m_hourWidget;
		m_hourWidget = 0;
	}
}

ViewHeader *DayRangeView::createHeaderWidget(QWidget *parent) {
	DayRangeHeader *widget = new DayRangeHeader(parent);
	widget->setFirstDate(m_firstDate);
	return widget;
}

void DayRangeView::refreshItemSizeAndPosition(CalendarItem *item) {
	// TODO if item is over many days, explodes it in several times intervals
	QRect rect = getTimeIntervalRect(item->beginDateTime().date().dayOfWeek(), item->beginDateTime().time(), item->endDateTime().time());
	item->move(rect.x(), rect.y());
	item->resize(rect.width() - 8, rect.height());
}

QRect DayRangeView::getTimeIntervalRect(int day, const QTime &begin, const QTime &end) const {
	int containWidth = rect().width() - m_leftScaleWidth;

	day--; // convert 1 -> 7 to 0 -> 6 for drawing reasons

	int seconds = begin.secsTo(end);
	int top = (QTime(0, 0, 0).secsTo(begin) * m_hourHeight) / 3600;
	int height = (seconds * m_hourHeight) / 3600;

	// vertical lines
	return QRect(m_leftScaleWidth + (day * containWidth) / m_rangeWidth,
				 top,
				 ((day + 1) * containWidth) / m_rangeWidth - (day * containWidth) / m_rangeWidth,
				 height);
}

void DayRangeView::setRangeWidth(int width) {
	if (width == m_rangeWidth)
		return;

	m_rangeWidth = width;
	forceUpdate();
}
