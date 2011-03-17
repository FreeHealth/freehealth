#ifndef DAY_VIEW_H
#define DAY_VIEW_H

#include <QGraphicsView>

class DayView : public QGraphicsView
{
	Q_OBJECT
public:
	DayView(QWidget *parent = 0) : QGraphicsView(parent) {}
};

#endif
