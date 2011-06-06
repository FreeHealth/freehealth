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
//	painter.fillRect(rect(), Qt::red);

	painter.setRenderHint(QPainter::Antialiasing);
	painter.setPen(Qt::NoPen);
	QBrush brush = painter.brush();
	brush.setStyle(Qt::SolidPattern);
	brush.setColor(QColor(0, 150, 0, 255));
	painter.setBrush(brush);

	painter.drawRoundedRect(rect(), 4, 4);
	painter.setPen(Qt::white);
	if (model()) {
		CalendarItem item = model()->getItemByUid(uid());
		painter.drawText(rect().adjusted(2, 0, -2, 0), Qt::AlignVCenter | Qt::AlignLeft, item.title().isEmpty() ? tr("(untitled)") : item.title());
	}

/*	// title
	QPixmap titlePixmap(size().width(), 20);
	titlePixmap.fill(Qt::transparent);
	QPainter titlePainter(&titlePixmap);
	titlePainter.setRenderHint(QPainter::Antialiasing);
	titlePainter.setPen(Qt::NoPen);
	QBrush brush = titlePainter.brush();
	brush.setStyle(Qt::SolidPattern);
	brush.setColor(QColor(0, 100, 0, m_inMotion ? 200 : 255));
	titlePainter.setBrush(brush);
	titlePainter.drawRoundedRect(QRect(0, 0, width(), 20), 5, 5);

	// text
	QRect r = rect();
	r.setHeight(15);
	r.adjust(2, 0, 0, 0);
	QPen fontPen;
	fontPen.setColor(Qt::white);
	titlePainter.setPen(fontPen);
	QFont font = titlePainter.font();
	font.setPixelSize(10);
	font.setBold(true);
	titlePainter.setFont(font);
	titlePainter.drawText(r, Qt::AlignLeft | Qt::AlignVCenter, QString("%1 - %2").arg(beginDateTime().time().toString("hh:mm")).arg(endDateTime().time().toString("hh:mm")));

	// body
	QPixmap bodyPixmap(size().width(), rect().height());
	bodyPixmap.fill(Qt::transparent);
	QPainter bodyPainter(&bodyPixmap);
	bodyPainter.setRenderHint(QPainter::Antialiasing);
	bodyPainter.setPen(QColor(0, 100, 0, m_inMotion ? 200 : 255));
	brush = bodyPainter.brush();
	brush.setStyle(Qt::SolidPattern);
	brush.setColor(QColor(0, 200, 0, m_inMotion ? 200 : 255));
	bodyPainter.setBrush(brush);
	bodyPainter.drawRoundedRect(QRect(0, 0, width(), height()), 5, 5);
	if (model()) {
		CalendarItem item = model()->getItemByUid(uid());
		if (item.isValid()) {
			bodyPainter.setPen(Qt::white);
			bodyPainter.drawText(QRect(2, 20, width() - 3, height()), Qt::TextWordWrap | Qt::AlignLeft, item.title().isEmpty() ? tr("(untitled)") : item.title());
		}
	}

	// main
	QPainter painter(this);
	painter.drawPixmap(QPoint(0, 0), titlePixmap, QRect(0, 0, width(), 15));
	painter.drawPixmap(QPoint(0, 15), bodyPixmap, QRect(0, 15, width(), rect().height() - 15));*/
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
