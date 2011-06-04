#include <QRect>
#include <QPainter>
#include <QDate>
#include <QPixmapCache>
#include <QScrollArea>
#include <QMouseEvent>
#include <QMenu>

#include "day_item_widget.h"
#include "common.h"
#include "abstract_calendar_model.h"
#include "calendar_widget.h"
#include "basic_item_edition_dialog.h"
#include "day_item_node.h"
#include "day_range_view.h"

using namespace Calendar;

int DayRangeBody::m_leftScaleWidth = 60;
int DayRangeBody::m_hourHeight = 40;

DayRangeHeader::DayRangeHeader(QWidget *parent, int rangeWidth) : ViewWidget(parent), m_rangeWidth(rangeWidth) {
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

QList<CalendarItem> DayRangeHeader::getItems() const {
	// optimization : do not compute items every time...
	QList<CalendarItem> items = model()->getItemsBetween(firstDate(), firstDate().addDays(m_rangeWidth - 1));
	// filter only day items
	for (int i = items.count() - 1; i >= 0; i--) {
		const CalendarItem &item = items[i];
		if (item.beginningType() == Date_DateTime && item.endingType() == Date_DateTime)
			items.removeAt(i);
	}
	return items;
}

QSize DayRangeHeader::sizeHint() const {
	// optimization : do not compute items every time...
	QList<CalendarItem> items = getItems();
	qSort(items.begin(), items.end(), calendarItemLessThan);
	QDate first = firstDate();
	QDate last = first.addDays(m_rangeWidth - 1);
	QMap<QDate, int> dayStacks;
	int maxWidth = 0;
	foreach (const CalendarItem &item, items) {
		int width = 0;
		// get width first
		for (QDate date = first; date <= last; date = date.addDays(1))
			if (!item.intersects(date, date)) {
				if (dayStacks[date] > width)
					width = dayStacks[date];
			}
		// refresh all stacks
		int dayIndex = 0;
		int firstIndex = -1;
//		int lastIndex;
		for (QDate date = first; date <= last; date = date.addDays(1)) {
			if (!item.intersects(date, date)) {
				if (firstIndex == -1)
					firstIndex = dayIndex;
				dayStacks.insert(date, width + 1);
//				lastIndex = dayIndex;
			}
			dayIndex++;
		}
		if (width + 1> maxWidth)
			maxWidth = width + 1;
	}

	int fontHeight = QFontMetrics(QFont()).height();

	return QSize(0, fontHeight + 4 + (maxWidth ? (maxWidth + 1) * (fontHeight + 5) : fontHeight + 4 + 5));
}

void DayRangeHeader::setRangeWidth(int width) {
	if (width == m_rangeWidth)
		return;

	m_rangeWidth = width;
	update();
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

	pen.setColor(QColor(150, 150, 255));
	painter.setPen(pen);

	// vertical lines
	int containWidth = (masterScrollArea ? masterScrollArea->viewport()->width() : width()) - 60;
	QPen oldPen = painter.pen();
	QDate date = firstDate();
	QDate now = QDate::currentDate();
	int fontHeight = QFontMetrics(QFont()).height();
	for (int i = 0; i < m_rangeWidth; ++i) {
		QRect br(QPoint(60 + (i * containWidth) / m_rangeWidth, 0), QPoint(60 + ((i + 1) * containWidth) / m_rangeWidth, rect().height() - 2));
		painter.fillRect(br, Qt::white);

		// vertical lines
		if (i > 0) {
			QPen pen = painter.pen();
			pen.setColor(QColor(200, 200, 200));
			pen.setCapStyle(Qt::FlatCap);
			painter.setPen(pen);
			painter.drawLine(60 + (i * containWidth) / m_rangeWidth, 0,
							 60 + (i * containWidth) / m_rangeWidth, rect().height());
		}
		br = QRect(QPoint(60 + (i * containWidth) / m_rangeWidth, rect().height() - 5), QPoint(60 + ((i + 1) * containWidth) / m_rangeWidth, rect().height() - 2));
		painter.fillRect(br, QColor(220, 220, 255));

		QRect r(QPoint(60 + (i * containWidth) / m_rangeWidth, 0), QPoint(60 + ((i + 1) * containWidth) / m_rangeWidth, fontHeight + 4));
		if (date == now){
			painter.fillRect(r, QColor(200,200,255));
			QPen pen = painter.pen();;
			pen.setColor(QColor(0, 0, 255));
			painter.setPen(pen);
		} else {
			painter.fillRect(r, QColor(220, 220, 255));
			QPen pen = painter.pen();;
			pen.setColor(QColor(0, 0, 255));
			painter.setPen(pen);
		}
		r.adjust(0, 2, 0, 0);  // +2 is a vertical correction to not be stucked to the top line
		if (m_rangeWidth == 1)
			painter.drawText(r, Qt::AlignHCenter | Qt::AlignTop, date.toString("dddd d/M").toLower());
		else
			painter.drawText(r, Qt::AlignHCenter | Qt::AlignTop, date.toString("ddd d/M").toLower());
		painter.setPen(oldPen);
		date = date.addDays(1);
	}
	int scaleHeight = fontHeight + 5;

	// items
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setPen(Qt::NoPen);
	QBrush brush = painter.brush();
	brush.setStyle(Qt::SolidPattern);
	brush.setColor(QColor(0, 150, 0, 255));
	painter.setBrush(brush);

	QList<CalendarItem> items = getItems();
	qSort(items.begin(), items.end(), calendarItemLessThan);
	QDate first = firstDate();
	QDate last = first.addDays(m_rangeWidth - 1);
	QMap<QDate, int> dayStacks;
	foreach (const CalendarItem &item, items) {
		int top = 0;
		// get top first
		for (QDate date = first; date <= last; date = date.addDays(1))
			if (!item.intersects(date, date)) {
				if (dayStacks[date] > top)
					top = dayStacks[date];
			}
		// refresh all stacks
		int dayIndex = 0;
		int firstIndex = -1;
		int lastIndex;
		for (QDate date = first; date <= last; date = date.addDays(1)) {
			if (!item.intersects(date, date)) {
				if (firstIndex == -1)
					firstIndex = dayIndex;
				dayStacks.insert(date, top + 1);
				lastIndex = dayIndex;
			}
			dayIndex++;
		}
		// paint
		QRect r(QPoint(60 + (firstIndex * containWidth) / m_rangeWidth + 1, scaleHeight + top * (fontHeight + 5)), QPoint(60 + ((lastIndex + 1) * containWidth) / m_rangeWidth - 2, scaleHeight + (top + 1) * (fontHeight + 5) - 2));
		painter.drawRoundedRect(r, 4, 4);
		painter.setPen(Qt::white);
		painter.drawText(r.adjusted(2, 0, -2, 0), Qt::AlignVCenter | Qt::AlignLeft, item.title().isEmpty() ? tr("(untitled)") : item.title());
	}
}

/////////////////////////////////////////////////////////////////

HourWidget::HourWidget(QWidget *parent) : QWidget(parent) {
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void HourWidget::paintEvent(QPaintEvent *) {
	QPainter painter(this);
	painter.fillRect(rect(), QColor(255, 150, 150));
}

/////////////////////////////////////////////////////////////////

DayRangeBody::DayRangeBody(QWidget *parent, int rangeWidth) :
	ViewWidget(parent),
	m_hourWidget(0),
	m_rangeWidth(rangeWidth),
	m_pressItemWidget(0),
	m_mouseMode(MouseMode_None) {
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	setFirstDate(Calendar::getFirstDateByRandomDate(Calendar::View_Week, QDate::currentDate()));
}

QSize DayRangeBody::sizeHint() const {
	return QSize(0, 24 * m_hourHeight);
}

void DayRangeBody::paintBody(QPainter *painter, const QRect &visibleRect) {
	painter->fillRect(visibleRect, Qt::white);
	QPen pen = painter->pen();
	pen.setColor(QColor(200, 200, 200));
	pen.setCapStyle(Qt::FlatCap);
	painter->setPen(pen);
	int containWidth = visibleRect.width() - m_leftScaleWidth;

	// draw current day?
	QDate now = QDate::currentDate();
	if (now >= firstDate() && now < firstDate().addDays(m_rangeWidth)){
		int day = now.dayOfWeek() - firstDate().dayOfWeek();
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
	if (now >= firstDate() && now < firstDate().addDays(m_rangeWidth)) {
		if (!m_hourWidget)
			m_hourWidget = new HourWidget(this);

		int day = now.dayOfWeek() - firstDate().dayOfWeek();

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

void DayRangeBody::refreshItemsSizesAndPositions() {
	for (int i = 0; i < m_rangeWidth; i++)
		refreshDayWidgets(firstDate().addDays(i));
}

QRect DayRangeBody::getTimeIntervalRect(int day, const QTime &begin, const QTime &end) const {
	int containWidth = rect().width() - m_leftScaleWidth;

	day--; // convert 1 -> 7 to 0 -> 6 for drawing reasons

	int seconds = end < begin ? begin.secsTo(QTime(23, 59)) + 1 : begin.secsTo(end);
	int top = (QTime(0, 0).secsTo(begin) * m_hourHeight) / 3600;
	int height = (seconds * m_hourHeight) / 3600;

	// vertical lines
	return QRect(m_leftScaleWidth + (day * containWidth) / m_rangeWidth,
				 top,
				 ((day + 1) * containWidth) / m_rangeWidth - (day * containWidth) / m_rangeWidth,
				 height);
}

QPair<int, int> DayRangeBody::getBand(const QDate &date) const {
	int containWidth = rect().width() - m_leftScaleWidth;
	QPair<int, int> band;

	int day = firstDate().daysTo(date);
	band.first = m_leftScaleWidth + (day * containWidth) / m_rangeWidth;
	band.second = ((day + 1) * containWidth) / m_rangeWidth - (day * containWidth) / m_rangeWidth - 8;
	return band;
}

QPair<int, int> DayRangeBody::getItemVerticalData(const QTime &begin, const QTime &end) const {
	int seconds = end < begin ? begin.secsTo(QTime(23, 59)) + 1 : begin.secsTo(end);
	int top = (QTime(0, 0).secsTo(begin) * m_hourHeight) / 3600;
	int height = (seconds * m_hourHeight) / 3600;

	// vertical lines
	return QPair<int, int>(top, height);
}

void DayRangeBody::setRangeWidth(int width) {
	if (width == m_rangeWidth)
		return;

	m_rangeWidth = width;
	forceUpdate();
}

QDateTime DayRangeBody::getDateTime(const QPoint &pos) const {
	// get day and time
	int containWidth = rect().width() - m_leftScaleWidth;
	int x = pos.x();
	int y = pos.y();
	int day = 0;
	for (int i = 0; i < m_rangeWidth; ++i) {
		if (x >= (i * containWidth) / m_rangeWidth + m_leftScaleWidth && x < ((i + 1) * containWidth) / m_rangeWidth + m_leftScaleWidth){
			break;
		}
		day++;
	}
	int hour = y / m_hourHeight;
	int remain = y - hour * m_hourHeight;
	int minutes = (remain * 60) / m_hourHeight;
	if (minutes < 15)
		minutes = 0;
	else if (minutes < 45)
		minutes = 30;
	else {
		minutes = 0;
		hour++;
	}
	return QDateTime(firstDate().addDays(day), QTime(hour, minutes));
}

void DayRangeBody::mousePressEvent(QMouseEvent *event) {
	if (event->pos().x() < m_leftScaleWidth)
		return;
	m_pressDateTime = getDateTime(event->pos());
	m_previousDateTime = m_pressDateTime;
	m_pressPos = event->pos();

	// item under mouse?
	m_pressItemWidget = qobject_cast<DayItemWidget*>(childAt(event->pos()));
	if (m_pressItemWidget) {
		m_pressItem = model()->getItemByUid(m_pressItemWidget->uid());
		QPoint pos = m_pressItemWidget->mapFromParent(event->pos());
		if (pos.y() >= m_pressItemWidget->height() - 5 && pos.y() < m_pressItemWidget->height())
			m_mouseMode = MouseMode_Resize;
		else
			m_mouseMode = MouseMode_Move;
	} else {
		m_mouseMode = MouseMode_Creation;
	}
}

void DayRangeBody::mouseMoveEvent(QMouseEvent *event) {
	QDateTime dateTime = getDateTime(event->pos());
	QRect rect;
	int seconds, limits;
	QDateTime beginning, ending;

	if (m_previousDateTime == dateTime)
		return;

	m_previousDateTime = dateTime;

	switch (m_mouseMode) {
	case MouseMode_Creation:
		if (dateTime != m_pressDateTime) {
			if (!m_pressItemWidget) {
				m_pressItemWidget = new DayItemWidget(this);
				m_pressItemWidget->setBeginDateTime(m_pressDateTime);
				m_pressItemWidget->show();
			}

			if (event->pos().y() > m_pressPos.y()) {
				rect = getTimeIntervalRect(m_pressDateTime.date().dayOfWeek(), m_pressDateTime.time(), dateTime.time());
				m_pressItemWidget->setBeginDateTime(m_pressDateTime);
				m_pressItemWidget->setEndDateTime(dateTime);
			}
			else {
				rect = getTimeIntervalRect(m_pressDateTime.date().dayOfWeek(), dateTime.time(), m_pressDateTime.time());
				m_pressItemWidget->setBeginDateTime(dateTime);
				m_pressItemWidget->setEndDateTime(m_pressDateTime);
			}

			m_pressItemWidget->move(rect.x(), rect.y());
			m_pressItemWidget->resize(rect.width(), rect.height());
		}
		break;
	case MouseMode_Move:
	case MouseMode_Resize:
		m_pressItemWidget->setInMotion(true);
		seconds = m_pressDateTime.time().secsTo(dateTime.time()); // seconds to add
		if (event->pos().y() > m_pressPos.y()) {
			QDateTime l = m_pressItem.ending().addDays(1);
			l.setTime(QTime(0, 0));
			limits = m_pressItem.ending().secsTo(l);
			if (seconds > limits)
				seconds = limits;
		} else {
			QDateTime l = m_pressItem.beginning();
			l.setTime(QTime(0, 0));
			limits = m_pressItem.beginning().secsTo(l);
			if (seconds < limits)
				seconds = limits;
		}

		if (m_mouseMode == MouseMode_Move) {
			beginning = m_pressItem.beginning().addSecs(seconds);
			beginning.setDate(dateTime.date());
		} else
			beginning = m_pressItem.beginning();

		ending = m_pressItem.ending().addSecs(seconds);
		if (m_mouseMode == MouseMode_Move) {
			ending.setDate(dateTime.date());
			m_pressItemWidget->setBeginDateTime(beginning);
		} else {
			if (ending <= beginning)
				ending = beginning.addSecs(1800);
		}
		m_pressItemWidget->setEndDateTime(ending);
		rect = getTimeIntervalRect(beginning.date().dayOfWeek(), beginning.time(), ending.time());
		m_pressItemWidget->move(rect.x(), rect.y());
		m_pressItemWidget->resize(rect.width(), rect.height());
		break;
	default:;
	}
}

void DayRangeBody::mouseReleaseEvent(QMouseEvent *event) {
	QDateTime beginning, ending;
	CalendarItem newItem;

	switch (m_mouseMode) {
	case MouseMode_Creation:
		if (!m_pressItemWidget) {
			// an hour by default
			beginning = m_pressDateTime;
			ending = m_pressDateTime.addSecs(3600);
		} else {
			beginning = m_pressItemWidget->beginDateTime();
			ending = m_pressItemWidget->endDateTime();
			beginning.setDate(m_pressDateTime.date());
			ending.setDate(m_pressDateTime.date());
			delete m_pressItemWidget;
		}
		if (model()){
			model()->insertItem(beginning, ending);
		}
		break;
	case MouseMode_Move:
	case MouseMode_Resize:
		if (!m_pressItemWidget->inMotion()) {
			// display a menu
			QMenu menu;
			QAction *modifyAction = menu.addAction(tr("modify"));
			connect(modifyAction, SIGNAL(triggered()), this, SLOT(modifyPressItem()));
			QAction *removeAction = menu.addAction(tr("remove"));
			connect(removeAction, SIGNAL(triggered()), this, SLOT(removePressItem()));
			menu.exec(event->globalPos());
		} else {
			newItem = m_pressItem;
			newItem.setBeginning(m_pressItemWidget->beginDateTime());
			newItem.setEnding(m_pressItemWidget->endDateTime());
			model()->setItemByUid(m_pressItem.uid(), newItem);
		}
		break;
	default:;
	}
	m_pressDateTime = QDateTime();
	m_pressItemWidget = 0;
	m_mouseMode = MouseMode_None;
}

void DayRangeBody::mouseDoubleClickEvent(QMouseEvent *event) {
	BasicItemEditionDialog dialog(this);
	dialog.init(m_pressItem);
	if (dialog.exec() == QDialog::Accepted)
		model()->setItemByUid(m_pressItem.uid(), dialog.item());
}

void DayRangeBody::itemInserted(const CalendarItem &item) {
	// refresh the entire day band
	refreshDayWidgets(item.beginning().date());
}

void DayRangeBody::itemRemoved(const CalendarItem &removedItem) {
	// refresh the involved bands
	refreshDayWidgets(removedItem.beginning().date());
}

void DayRangeBody::itemModified(const CalendarItem &oldItem, const CalendarItem &newItem) {
	QList<QDate> daysToRefresh;
	if (!oldItem.intersects(firstDate(), firstDate().addDays(m_rangeWidth - 1))) { // collect all old item days
		daysToRefresh << oldItem.beginning().date();
		if (daysToRefresh.indexOf(oldItem.ending().date()) < 0)
			daysToRefresh << oldItem.ending().date();
	}
	if (!newItem.intersects(firstDate(), firstDate().addDays(m_rangeWidth - 1))) { // collect all new item days
		if (daysToRefresh.indexOf(newItem.beginning().date()) < 0)
			daysToRefresh << newItem.beginning().date();
		if (daysToRefresh.indexOf(newItem.ending().date()) < 0)
			daysToRefresh << newItem.ending().date();
	}

	foreach (const QDate &date, daysToRefresh)
		refreshDayWidgets(date);
}

void DayRangeBody::resetItemWidgets() {
	deleteAllWidgets();
	for (int i = 0; i < m_rangeWidth; i++)
		refreshDayWidgets(firstDate().addDays(i));
}

void DayRangeBody::refreshDayWidgets(const QDate &dayDate) {
	if (dayDate < firstDate() || dayDate >= firstDate().addDays(m_rangeWidth)) // day is out of range
		return;

	// at first remove all day widgets
	qDeleteAll(getWidgetsByDate(dayDate));

	if (!model())
		return;

	// re-create them
	QList<CalendarItem> items = model()->getItemsBetween(dayDate, dayDate);
	// filter by date-time types
	for (int i = items.count() - 1; i >= 0; i--) {
		const CalendarItem &item = items[i];
		if (item.beginningType() == Date_Date || item.endingType() == Date_Date)
			items.removeAt(i);
	}
	if (!items.count())
		return;

	// sorting and create the tree
	qSort(items.begin(), items.end(), calendarItemLessThan);

	DayItemNode node(items[0]);

	for (int i = 1; i < items.count(); i++)
		node.store(items[i]);

	node.prepareForWidthsComputing();
	QList<DayItemNode*> nodes;
	QPair<int, int> band = getBand(dayDate);
	node.computeWidths(band.first, band.second, nodes);

	foreach (DayItemNode *node, nodes) {
		DayItemWidget *widget = new DayItemWidget(this, node->item().uid(), model());
		QPair<int, int> verticalData = getItemVerticalData(node->item().beginning().time(), node->item().ending().time());
		widget->setBeginDateTime(node->item().beginning());
		widget->setEndDateTime(node->item().ending());
		widget->move(node->left(), verticalData.first);
		widget->resize(node->width(), verticalData.second);
		widget->show();
	}
}

void DayRangeBody::modifyPressItem() {
	BasicItemEditionDialog dialog(this);
	dialog.init(m_pressItem);
	if (dialog.exec() == QDialog::Accepted)
		model()->setItemByUid(m_pressItem.uid(), dialog.item());
}

void DayRangeBody::removePressItem() {
	model()->removeItem(m_pressItem.uid());
}
