#include <QPainter>
#include <QMouseEvent>

#include "abstract_calendar_model.h"
#include "day_widget.h"

using namespace Calendar;

DayWidget::DayWidget(QWidget *parent, const QString &uid, AbstractCalendarModel *model)
	: CalendarItemWidget(parent, uid, model), m_aboveWidget(0) {
	m_titleFont = getTitleFont();
	m_inMotion = uid.isEmpty();
	setMouseTracking(true);
//	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

QSize DayWidget::sizeHint() const {
	return QSize(0, QFontMetrics(m_titleFont).height() + 4);
}

QFont DayWidget::getTitleFont() {
	return QFont();
}

QSize DayWidget::staticSizeHint() {
	return QSize(0, QFontMetrics(getTitleFont()).height() + 4);
}

void DayWidget::paintEvent(QPaintEvent *) {
        QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setPen(Qt::NoPen);
	QBrush brush = painter.brush();
	brush.setStyle(Qt::SolidPattern);
	brush.setColor(QColor(0, 150, 0, m_inMotion ? 200 : 255));
	painter.setBrush(brush);

	painter.drawRoundedRect(rect(), 4, 4);
	painter.setPen(Qt::white);
	if (model()) {
		CalendarItem item = model()->getItemByUid(uid());
		painter.drawText(rect().adjusted(2, 0, -2, 0), Qt::AlignVCenter | Qt::AlignLeft, item.title().isEmpty() ? tr("(untitled)") : item.title());
	}
}

void DayWidget::setInMotion(bool value) {
	if (m_inMotion == value)
		return;

	m_inMotion = value;

	if (m_inMotion) { // record the above widget and put itself in the stack top
		QWidget *parent = parentWidget();
		m_aboveWidget = 0;
		for (int index = parent->children().indexOf(this) + 1; index < parent->children().count(); index++) {
			QWidget *widget = qobject_cast<QWidget*>(parent->children()[index]);
			if (widget) {
				m_aboveWidget = widget;
				break;
			}
		}
		raise();
	} else { // restore the widget in the stack
		if (m_aboveWidget)
			stackUnder(m_aboveWidget);
	}
}
