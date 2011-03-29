#include <QRect>
#include <QPainter>
#include <QDate>
#include <QPixmapCache>

#include "week_view.h"
#include "calendar_item.h"
#include "common.h"

using namespace Calendar;

int WeekView::m_leftScaleWidth = 60;
int WeekView::m_hourHeight = 40;

void WeekHeader::paintEvent(QPaintEvent *event) {
	QPainter painter(this);
	painter.fillRect(rect(), QColor(220, 220, 255));
	QPen pen = painter.pen();
	pen.setColor(QColor(150, 150, 255));
	painter.setPen(pen);
	QRect r = rect();
	r.adjust(0, 0, -1, 1);
	painter.drawLine(0, r.bottom(), r.right(), r.bottom());

	// text
	// vertical lines
	int containWidth = rect().width() - 60;
	QPen oldPen = painter.pen();
	QFont oldFont = painter.font();
	QDate date = m_firstDate;
	QDate now = QDate::currentDate();
	for (int i = 0; i < 7; ++i) {
		QRect r(QPoint(60 + (i * containWidth) / 7, 0), QPoint(60 + ((i + 1) * containWidth) / 7 - 1, rect().height()));
		if (date == now){
			painter.fillRect(r, QColor(200,200,255));
			QPen pen = painter.pen();;
			pen.setColor(QColor(0, 0, 255));
			painter.setPen(pen);
		}
		painter.drawText(r, Qt::AlignCenter | Qt::AlignTop, date.toString("ddd d/M").toLower());
		painter.setPen(oldPen);
		painter.setFont(oldFont);
		date = date.addDays(1);
	}
}

void WeekHeader::setFirstDate(const QDate &date) {
	if (date == m_firstDate)
		return;

	m_firstDate = date;
	update();
}

/////////////////////////////////////////////////////////////////

WeekView::WeekView(QWidget *parent) :
	View(parent) {
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
}

QWidget *WeekView::createHeaderWidget(QWidget *parent) {
	WeekHeader *widget = new WeekHeader(parent);
	widget->setFirstDate(m_firstDate);
	return widget;
}
