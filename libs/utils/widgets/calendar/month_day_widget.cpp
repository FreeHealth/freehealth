#include <QVBoxLayout>
#include <QLabel>

#include "month_day_widget.h"

using namespace Calendar;

MonthDayWidget::MonthDayWidget(AbstractCalendarModel *model, const QDate &day, QWidget *parent)
	: QWidget(parent),
	  m_model(model),
	  m_day(day) {

	m_items = m_model->getItemsBetween(m_day, m_day);
	qSort(m_items.begin(), m_items.end(), calendarItemLessThan);
}

void MonthDayWidget::resizeEvent(QResizeEvent *event) {
	QWidget::resizeEvent(event);

	refreshItems();
}

void MonthDayWidget::refreshItems() {
	// delete all
	QList<QWidget*> list;
	foreach (QObject *obj, children()) {
		QWidget *widget = qobject_cast<QWidget*>(obj);
		if (widget)
			list << widget;
	}
	qDeleteAll(list);

	if (!m_items.count())
		return;

	QFont itemFont = QLabel().font(); // the font is a QLabel font classical => todo : change it and choose a better one
	QFont linkFont = itemFont;
	linkFont.setUnderline(true);
	int itemHeight = QFontMetrics(itemFont).height();
	int visibleItemsCount = this->height() / itemHeight;
	int top = 0;
	int count = visibleItemsCount < m_items.count() ? visibleItemsCount - 1 : m_items.count();

	for (int i = 0; i < count; i++) {
		const CalendarItem &item = m_items[i];
		QLabel *label = new QLabel(QString("<b>%1</b> %2").arg(item.beginning().time().toString("hh:mm")).arg(item.title().isEmpty() ? "(untitled)" : item.title()), this);
		label->setFont(itemFont);
		label->move(0, top);
		label->show();
		top += itemHeight;
	}

	if (count != m_items.count()) { // add a link
		QLabel *label = new QLabel(tr("see others"), this);
		label->setFont(linkFont);
		QPalette palette = label->palette();
		palette.setColor(QPalette::WindowText, Qt::darkBlue);
		label->setPalette(palette);
		label->setCursor(Qt::PointingHandCursor);
		label->move(0, top);
		label->show();
/*		painter.drawText(dayRect.adjusted(2, top, -2, 0), Qt::AlignLeft, "more elements");*/
	}

/*
	foreach (const CalendarItem &item, m_items) {
		QLabel *label = new QLabel(QString("<b>%1</b> %2").arg(item.beginning().time().toString("hh:mm")).arg(item.title().isEmpty() ? "(untitled)" : item.title()), this);
		label->move(0, top);
		top += QFontMetrics(label->font()).height();
		label->show();
	}
*/
/*	if (count != items.count()) { // add a link
		painter.drawText(dayRect.adjusted(2, top, -2, 0), Qt::AlignLeft, "more elements");
		}*/
}
