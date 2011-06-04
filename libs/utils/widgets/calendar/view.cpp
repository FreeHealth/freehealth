#include <QPainter>
#include <QPixmapCache>
#include <QResizeEvent>
#include <QScrollArea>

#include "view.h"

using namespace Calendar;

void ViewWidget::setFirstDate(const QDate &firstDate) {
	if (m_firstDate == firstDate)
		return;

	m_firstDate = firstDate;
	emit firstDateChanged();

	resetItemWidgets();
	m_refreshGrid = true;
	update();
}

QPixmap ViewWidget::generatePixmap() {
	QPixmap pixmap(width(), height());
	QPainter painter(&pixmap);
	paintBody(&painter, rect());
	return pixmap;
}

void ViewWidget::paintEvent(QPaintEvent *) {
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

void ViewWidget::resizeEvent(QResizeEvent *event) {
	m_refreshGrid = true;

	refreshItemsSizesAndPositions();

	QWidget::resizeEvent(event);
}

void ViewWidget::refreshItemsSizesAndPositions() {
	foreach (QObject *object, children()) {
		CalendarItemWidget *widget = qobject_cast<CalendarItemWidget*>(object);
		if (widget)
			refreshItemSizeAndPosition(widget);
	}
}

void ViewWidget::refreshCurrentDateTimeStuff() {
	m_refreshGrid = true;
	update();
}

void ViewWidget::setModel(AbstractCalendarModel *model) {
	// disconnect slots
	if (m_model){
		disconnect(m_model, SIGNAL(itemInserted(const CalendarItem &)), this, SLOT(itemInserted(const CalendarItem &)));
		disconnect(m_model, SIGNAL(itemModified(const CalendarItem &, const CalendarItem &)), this, SLOT(itemModified(const CalendarItem &, const CalendarItem &)));
		disconnect(m_model, SIGNAL(itemRemoved(const CalendarItem &)), this, SLOT(itemRemoved(const CalendarItem &)));
	}

	m_model = model;

	if (m_model) {
		// connect slots
		connect(m_model, SIGNAL(itemInserted(const CalendarItem &)), this, SLOT(itemInserted(const CalendarItem &)));
		connect(m_model, SIGNAL(itemModified(const CalendarItem &, const CalendarItem &)), this, SLOT(itemModified(const CalendarItem &, const CalendarItem &)));
		connect(m_model, SIGNAL(itemRemoved(const CalendarItem &)), this, SLOT(itemRemoved(const CalendarItem &)));
	}

	resetItemWidgets();
}

CalendarItemWidget *ViewWidget::getWidgetByUid(const QString &uid) const {
	foreach (QObject *object, children()) {
		CalendarItemWidget *widget = qobject_cast<CalendarItemWidget*>(object);
		if (widget && widget->uid() == uid)
			return widget;
	}
        return 0;
}

QList<CalendarItemWidget*> ViewWidget::getWidgetsByDate(const QDate &dayDate) const {
	QList<CalendarItemWidget*> list;
	foreach (QObject *obj, children()) {
		CalendarItemWidget *widget = qobject_cast<CalendarItemWidget*>(obj);
		if (widget && widget->beginDateTime().date() == dayDate)
			list << widget;
	}
	return list;
}

void ViewWidget::deleteAllWidgets() {
	QList<CalendarItemWidget*> list;
	foreach (QObject *obj, children()) {
		CalendarItemWidget *widget = qobject_cast<CalendarItemWidget*>(obj);
		if (widget)
			list << widget;
	}
	qDeleteAll(list);
}

void ViewWidget::setMasterScrollArea(QScrollArea *scrollArea) {
	masterScrollArea = scrollArea;
}
