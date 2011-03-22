#include <QRect>
#include <QPainter>
#include <QDate>

#include "week_view.h"

namespace Calendar {

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
//		pen.setColor(Qt::red);
//		painter.setPen(pen);
		QDate now = QDate::currentDate();
		for (int i = 0; i < 7; ++i) {
			QRect r(QPoint(60 + (i * containWidth) / 7, 0), QPoint(60 + ((i + 1) * containWidth) / 7 - 1, rect().height()));
			if (now.dayOfWeek() == i + 1){
				painter.fillRect(r, QColor(200,200,255));
				QPen pen = painter.pen();;
				pen.setColor(QColor(0, 0, 255));
				painter.setPen(pen);
				QFont font = painter.font();
				font.setBold(true);
				painter.setFont(font);
			}
			painter.drawText(r, Qt::AlignCenter | Qt::AlignTop, QDate::shortDayName(i + 1));
			painter.setPen(oldPen);
			painter.setFont(oldFont);
		}
	}

	WeekView::WeekView(QWidget *parent) :
		View(parent) {
		resize(10, 24 * m_hourHeight);
	}

	int WeekView::topHeaderHeight() const {
		return 40;
	}

	int WeekView::leftHeaderWidth() const {
		return 0;
	}

	void WeekView::paintTopHeader(QPainter *painter) {

	}

	void WeekView::paintLeftHeader(QPainter *painter) {

	}

	void WeekView::paintBody(QPainter *painter, const QRect &visibleRect) {
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

		// half-hours
		QPen oldPen = pen;
		pen.setDashPattern(QVector<qreal>() << 1 << 1);
		painter->setPen(pen);
		for (int i = 0; i < 24; ++i) {
			painter->drawLine(m_leftScaleWidth, i * m_hourHeight + m_hourHeight / 2,
							  visibleRect.width() - 1, i * m_hourHeight + m_hourHeight / 2);
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

	void WeekView::paintEvent(QPaintEvent *event) {
		QPainter painter(this);
		painter.fillRect(rect(), Qt::white);
		paintBody(&painter, rect());
	}

	QWidget *WeekView::createHeaderWidget(QWidget *parent) {
		QWidget *widget = new WeekHeader(parent);
		return widget;
	}

}

