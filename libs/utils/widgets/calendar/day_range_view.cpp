#include <QRect>
#include <QPainter>
#include <QDate>
#include <QPixmapCache>
#include <QScrollArea>
#include <QMouseEvent>

#include "calendar_item_widget.h"
#include "common.h"
#include "abstract_calendar_model.h"
#include "calendar_widget.h"
#include "day_range_view.h"

using namespace Calendar;

CalendarItemNode::~CalendarItemNode() {
	// destroy recursively all the structure (siblings and children)
	if (m_right)
		delete m_right;
	if (m_next)
		delete m_next;
}

CalendarItemNode *CalendarItemNode::mostBottomNode() {
	CalendarItemNode *node = this;
	while (node->next())
		node = node->next();
	return node;
}

CalendarItemNode *CalendarItemNode::getNextCollidingNode(const CalendarItem &item) {
	CalendarItemNode *node = mostBottomNode();
	if (node->item().overlap(item))
		return node;

	if (node->right())
		return node->right()->getNextCollidingNode(item);

	if (node->colliding())
		return node->colliding()->getNextCollidingNode(item);

	return 0;
}

void CalendarItemNode::store(const CalendarItem &item) {
	CalendarItemNode *current = mostBottomNode();
	if (current->item().overlap(item)) {
		if (current->right())
			current->right()->store(item);
		else {
			if (current->colliding()) {
				if (current->colliding()->item().overlap(item)) {
					if (current->m_index + 1 >= current->colliding()->m_index) { // we reached the insertion count of node before the colliding one
						current->colliding()->store(item);
					} else // insert it
						current->m_right = new CalendarItemNode(item, current->m_colliding, current->m_index + 1);
				} else
					current->m_right = new CalendarItemNode(item, current->colliding()->getNextCollidingNode(item), current->m_index + 1);
			} else
				current->m_right = new CalendarItemNode(item, 0, current->m_index + 1);
		}
		return;
	}

	// non overlapping => will be the next item
	current->m_next = new CalendarItemNode(item, current->getNextCollidingNode(item), current->m_index);
}

int CalendarItemNode::computeMaxCount() {
	m_maxCount = 1 + (m_right ? m_right->computeMaxCount() : 0);
	return qMax(m_maxCount, m_next ? m_next->computeMaxCount() : 0);
}

int CalendarItemNode::computeMaxCountBeforeColliding() {
	m_maxCountBeforeColliding = 1;

	if (m_right && m_right->m_colliding == m_colliding)
		m_maxCountBeforeColliding += m_right->computeMaxCountBeforeColliding();

	if (m_next) {
		int nextMaxCountBeforeColliding = m_next->computeMaxCountBeforeColliding();
		if (m_next->m_colliding == m_colliding)
			return qMax(m_maxCountBeforeColliding, nextMaxCountBeforeColliding);
	}
	return m_maxCountBeforeColliding;
}

void CalendarItemNode::prepareForWidthsComputing() {
	computeMaxCount();
	computeMaxCountBeforeColliding();
}

void CalendarItemNode::computeWidths(int left, int width, QList<CalendarItemNode*> &list) {
	m_left = left;
	list << this;

	int collidingWidth = -1;

	// compute colliding width
	if (m_colliding)
		collidingWidth = (m_colliding->m_left - left) / m_maxCountBeforeColliding;
	m_width = width / m_maxCount;
	if (collidingWidth != -1 && collidingWidth < m_width)
		m_width = collidingWidth;
	if (m_right)
		m_right->computeWidths(m_left + m_width, width - m_width, list);
	if (m_next)
		m_next->computeWidths(m_left, width, list);
}

// -------------------------------------

int DayRangeView::m_leftScaleWidth = 60;
int DayRangeView::m_hourHeight = 40;

QSize DayRangeHeader::sizeHint() const {
	return QSize(0, 40);
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

	// text
	pen.setColor(QColor(150, 150, 255));
	painter.setPen(pen);

	// vertical lines
	int containWidth = m_scrollArea->viewport()->width() - 60;
	QPen oldPen = painter.pen();
	QFont oldFont = painter.font();
	QDate date = firstDate();
	QDate now = QDate::currentDate();
	for (int i = 0; i < m_rangeWidth; ++i) {
		QRect r(QPoint(60 + (i * containWidth) / m_rangeWidth, 0), QPoint(60 + ((i + 1) * containWidth) / m_rangeWidth, rect().height()));
		if (date == now){
			painter.fillRect(r, QColor(200,200,255));
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
	m_rangeWidth(rangeWidth),
	m_pressItemWidget(0),
	m_mouseMode(MouseMode_None) {
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	setFirstDate(Calendar::getFirstDateByRandomDate(Calendar::View_Week, QDate::currentDate()));
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
	DayRangeHeader *widget = new DayRangeHeader(parent, m_rangeWidth);
	widget->setFirstDate(m_firstDate);
	return widget;
}

void DayRangeView::refreshItemsSizesAndPositions() {
	for (int i = 0; i < m_rangeWidth; i++)
		refreshDayWidgets(m_firstDate.addDays(i));
}

QRect DayRangeView::getTimeIntervalRect(int day, const QTime &begin, const QTime &end) const {
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

QPair<int, int> DayRangeView::getBand(const QDate &date) const {
	int containWidth = rect().width() - m_leftScaleWidth;
	QPair<int, int> band;

	int day = date.dayOfWeek() - 1;
	band.first = m_leftScaleWidth + (day * containWidth) / m_rangeWidth;
	band.second = ((day + 1) * containWidth) / m_rangeWidth - (day * containWidth) / m_rangeWidth - 8;
	return band;
}

QPair<int, int> DayRangeView::getItemVerticalData(const QTime &begin, const QTime &end) const {
	int seconds = end < begin ? begin.secsTo(QTime(23, 59)) + 1 : begin.secsTo(end);
	int top = (QTime(0, 0).secsTo(begin) * m_hourHeight) / 3600;
	int height = (seconds * m_hourHeight) / 3600;

	// vertical lines
	return QPair<int, int>(top, height);
}

void DayRangeView::setRangeWidth(int width) {
	if (width == m_rangeWidth)
		return;

	m_rangeWidth = width;
	forceUpdate();
}

QDateTime DayRangeView::getDateTime(const QPoint &pos) const {
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
	return QDateTime(m_firstDate.addDays(day), QTime(hour, minutes));
}

void DayRangeView::mousePressEvent(QMouseEvent *event) {
	if (event->pos().x() < m_leftScaleWidth)
		return;
	m_pressDateTime = getDateTime(event->pos());
	m_previousDateTime = m_pressDateTime;
	m_pressPos = event->pos();

	// item under mouse?
	m_pressItemWidget = qobject_cast<CalendarItemWidget*>(childAt(event->pos()));
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

void DayRangeView::mouseMoveEvent(QMouseEvent *event) {
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
				m_pressItemWidget = new CalendarItemWidget(this);
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

void DayRangeView::mouseReleaseEvent(QMouseEvent *) {
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
		newItem = m_pressItem;
		newItem.setBeginning(m_pressItemWidget->beginDateTime());
		newItem.setEnding(m_pressItemWidget->endDateTime());
		model()->setItemByUid(m_pressItem.uid(), newItem);
		break;
	default:;
	}
	m_pressDateTime = QDateTime();
	m_pressItem = CalendarItem();
	m_pressItemWidget = 0;
	m_mouseMode = MouseMode_None;
}

void DayRangeView::itemInserted(const CalendarItem &item) {
	// refresh the entire day band
	refreshDayWidgets(item.beginning().date());
}

void DayRangeView::itemModified(const CalendarItem &oldItem, const CalendarItem &newItem) {
	QList<QDate> daysToRefresh;
	if (!oldItem.intersects(m_firstDate, m_firstDate.addDays(m_rangeWidth - 1))) { // collect all old item days
		daysToRefresh << oldItem.beginning().date();
		if (daysToRefresh.indexOf(oldItem.ending().date()) < 0)
			daysToRefresh << oldItem.ending().date();
	}
	if (!newItem.intersects(m_firstDate, m_firstDate.addDays(m_rangeWidth - 1))) { // collect all new item days
		if (daysToRefresh.indexOf(newItem.beginning().date()) < 0)
			daysToRefresh << newItem.beginning().date();
		if (daysToRefresh.indexOf(newItem.ending().date()) < 0)
			daysToRefresh << newItem.ending().date();
	}

	foreach (const QDate &date, daysToRefresh)
		refreshDayWidgets(date);
}

void DayRangeView::resetItemWidgets() {
	for (int i = 0; i < m_rangeWidth; i++)
		refreshDayWidgets(m_firstDate.addDays(i));
}

// at first compare with begin dates. If they're equals, compare by end dates.
bool calendarItemLessThan(const Calendar::CalendarItem &item1, const Calendar::CalendarItem &item2)
{
	if (item1.beginning() < item2.beginning())
		return true;
	else if (item1.beginning() > item2.beginning())
		return false;
	else if (item1.ending() > item2.ending())
		return true;
	else
		return false;
}

void DayRangeView::refreshDayWidgets(const QDate &dayDate) {
	if (dayDate < m_firstDate || dayDate >= m_firstDate.addDays(m_rangeWidth)) // day is out of range
		return;

	// at first remove all day widgets
	qDeleteAll(getWidgetsByDate(dayDate));

	if (!model())
		return;

	// re-create them
	QList<CalendarItem> items = model()->getItemsBetween(dayDate, dayDate);

	if (!items.count())
		return;

	QPair<int, int> band = getBand(dayDate);

	// sorting and create the tree
	qSort(items.begin(), items.end(), calendarItemLessThan);

	CalendarItemNode node(items[0]);

	for (int i = 1; i < items.count(); i++)
		node.store(items[i]);

	node.prepareForWidthsComputing();
	QList<CalendarItemNode*> nodes;
	node.computeWidths(band.first, band.second, nodes);

	foreach (CalendarItemNode *node, nodes) {
		CalendarItemWidget *widget = new CalendarItemWidget(this, node->item().uid());
		QPair<int, int> verticalData = getItemVerticalData(node->item().beginning().time(), node->item().ending().time());
		widget->setBeginDateTime(node->item().beginning());
		widget->setEndDateTime(node->item().ending());
		widget->move(node->left(), verticalData.first);
		widget->resize(node->width(), verticalData.second);
		widget->show();
	}
}

QList<CalendarItemWidget*> DayRangeView::getWidgetsByDate(const QDate &dayDate) const {
	QList<CalendarItemWidget*> list;
	foreach (QObject *obj, children()) {
		CalendarItemWidget *widget = qobject_cast<CalendarItemWidget*>(obj);
		if (widget && widget->beginDateTime().date() == dayDate)
			list << widget;
	}
	return list;
}
