#ifndef WEEK_VIEW_H
#define WEEK_VIEW_H

#include <QGraphicsView>

class WeekView : public QGraphicsView
{
	Q_OBJECT
public:
	WeekView(QWidget *parent = 0) : QGraphicsView(parent) {}
};

#endif
