#include <QPainter>
#include <QPixmapCache>
#include <QResizeEvent>
#include <QScrollArea>

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

void ViewHeader::setScrollArea(QScrollArea *scrollArea) {
	m_scrollArea = scrollArea;
}


///////////////////////////////

void View::setFirstDate(const QDate &firstDate) {
	if (m_firstDate == firstDate)
		return;

	m_firstDate = firstDate;
	resetItemWidgets();
	m_refreshGrid = true;
	update();
}

QPixmap View::generatePixmap() {
	QPixmap pixmap(width(), height());
	QPainter painter(&pixmap);
	paintBody(&painter, rect());
	return pixmap;
}

void View::paintEvent(QPaintEvent *) {
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

	refreshItemsSizesAndPositions();

	QWidget::resizeEvent(event);
}

void View::refreshItemsSizesAndPositions() {
	foreach (QObject *object, children()) {
		CalendarItemWidget *widget = qobject_cast<CalendarItemWidget*>(object);
		if (widget)
			refreshItemSizeAndPosition(widget);
	}
}

void View::refreshCurrentDateTimeStuff() {
	m_refreshGrid = true;
	update();
}

void View::setModel(AbstractCalendarModel *model) {
	// disconnect slots
	if (m_model){
		disconnect(m_model, SIGNAL(itemInserted(const CalendarItem &)), this, SLOT(itemInserted(const CalendarItem &)));
		disconnect(m_model, SIGNAL(itemModified(const CalendarItem &, const CalendarItem &)), this, SLOT(itemModified(const CalendarItem &, const CalendarItem &)));
	}

	m_model = model;

	if (m_model) {
		// connect slots
		connect(m_model, SIGNAL(itemInserted(const CalendarItem &)), this, SLOT(itemInserted(const CalendarItem &)));
		connect(m_model, SIGNAL(itemModified(const CalendarItem &, const CalendarItem &)), this, SLOT(itemModified(const CalendarItem &, const CalendarItem &)));
	}

	resetItemWidgets();
}

CalendarItemWidget *View::getWidgetByUid(const QString &uid) const {
	foreach (QObject *object, children()) {
		CalendarItemWidget *widget = qobject_cast<CalendarItemWidget*>(object);
		if (widget && widget->uid() == uid)
			return widget;
	}
        return 0;
}

QList<CalendarItemWidget*> View::getWidgetsByDate(const QDate &dayDate) const {
	QList<CalendarItemWidget*> list;
	foreach (QObject *obj, children()) {
		CalendarItemWidget *widget = qobject_cast<CalendarItemWidget*>(obj);
		if (widget && widget->beginDateTime().date() == dayDate)
			list << widget;
	}
	return list;
}

void View::deleteAllWidgets() {
	QList<CalendarItemWidget*> list;
	foreach (QObject *obj, children()) {
		CalendarItemWidget *widget = qobject_cast<CalendarItemWidget*>(obj);
		if (widget)
			list << widget;
	}
	qDeleteAll(list);
}
