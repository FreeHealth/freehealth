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

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setSpacing(0);
}

void MonthDayWidget::resizeEvent(QResizeEvent *event) {
	QWidget::resizeEvent(event);

	refreshItems();
}

void MonthDayWidget::refreshItems() {
	// delete all
	QLayoutItem *child;
	while ((child = layout()->takeAt(0)) != 0) {
		if (child->widget())
			delete child->widget();
		delete child;
	}

	if (!m_items.count())
		return;

	QVBoxLayout *vlayout = qobject_cast<QVBoxLayout*>(layout());

	foreach (const CalendarItem &item, m_items) {
		vlayout->addWidget(new QLabel(QString("<b>%1</b> %2").arg(item.beginning().time().toString("hh:mm")).arg(item.title().isEmpty() ? "(untitled)" : item.title())));
	}
	vlayout->addStretch();

/*	if (count != items.count()) { // add a link
		painter.drawText(dayRect.adjusted(2, top, -2, 0), Qt::AlignLeft, "more elements");
		}*/

}
