#ifndef CALENDAR_WIDGET_H
#define CALENDAR_WIDGET_H

#include <QGraphicsView>

class CalendarWidget : public QGraphicsView
{
	Q_OBJECT
public:
	CalendarWidget(QWidget *parent = 0) : QGraphicsView(parent) {}
};

#endif
