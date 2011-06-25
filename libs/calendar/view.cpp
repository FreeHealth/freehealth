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
	if (!masterScrollArea) {
		m_refreshGrid = true;
		refreshItemsSizesAndPositions();
	}

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
		disconnect(m_model, SIGNAL(itemInserted(const Calendar::CalendarItem &)), this, SLOT(itemInserted(const Calendar::CalendarItem &)));
		disconnect(m_model, SIGNAL(itemModified(const Calendar::CalendarItem &, const Calendar::CalendarItem &)), this, SLOT(itemModified(const Calendar::CalendarItem &, const Calendar::CalendarItem &)));
		disconnect(m_model, SIGNAL(itemRemoved(const Calendar::CalendarItem &)), this, SLOT(itemRemoved(const Calendar::CalendarItem &)));
		disconnect(m_model, SIGNAL(reset()), this, SLOT(reset()));
	}

	m_model = model;

	if (m_model) {
		// connect slots
		connect(m_model, SIGNAL(itemInserted(const Calendar::CalendarItem &)), this, SLOT(itemInserted(const Calendar::CalendarItem &)));
		connect(m_model, SIGNAL(itemModified(const Calendar::CalendarItem &, const Calendar::CalendarItem &)), this, SLOT(itemModified(const Calendar::CalendarItem &, const Calendar::CalendarItem &)));
		connect(m_model, SIGNAL(itemRemoved(const Calendar::CalendarItem &)), this, SLOT(itemRemoved(const Calendar::CalendarItem &)));
		connect(m_model, SIGNAL(reset()), this, SLOT(reset()));
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
	if (masterScrollArea) {
		masterScrollArea->viewport()->installEventFilter(this);
	}
}

bool ViewWidget::eventFilter(QObject *obj, QEvent *event) {
	bool r = QWidget::eventFilter(obj, event);
	if (event->type() == QEvent::Resize) {
//		QWidget *w = qobject_cast<QWidget*>(obj);
/*		QScrollArea *w = qobject_cast<QScrollArea*>(obj);
		qDebug("resize: %d, %d", w->width(), w->viewport()->width());*/
		m_refreshGrid = true;
		refreshItemsSizesAndPositions();
	}
	return r;
}

void ViewWidget::reset() {
	resetItemWidgets();
	update();
}
