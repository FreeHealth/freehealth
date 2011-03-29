#include <QPainter>
#include <QPixmapCache>
#include <QResizeEvent>

#include "view.h"

using namespace Calendar;

ViewHeader::ViewHeader(QWidget *parent) : QWidget(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void ViewHeader::setFirstDate(const QDate &date) {
	if (date == m_firstDate)
		return;

	m_firstDate = date;
	update();
}

void ViewHeader::resizeEvent(QResizeEvent *event) {
	emit resized(event->size());
}

///////////////////////////////

void View::setFirstDate(const QDate &firstDate) {
	if (m_firstDate == firstDate)
		return;

	m_firstDate = firstDate;
	m_refreshGrid = true;
	update();
}

QPixmap View::generatePixmap() {
	QPixmap pixmap(width(), height());
	QPainter painter(&pixmap);
	paintBody(&painter, rect());
	return pixmap;
}

void View::paintEvent(QPaintEvent *event) {
	QPixmap pixmap;
	QString key = "grid";

	if (!QPixmapCache::find(key, pixmap) || m_refreshGrid) {
		pixmap = generatePixmap();
		QPixmapCache::insert(key, pixmap);
		m_refreshGrid = false;
	}
	QPainter painter(this);
	painter.drawPixmap(0, 0, pixmap);
}

void View::resizeEvent(QResizeEvent *event) {
	m_refreshGrid = true;

	QWidget::resizeEvent(event);
}
