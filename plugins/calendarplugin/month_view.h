#ifndef MONTH_VIEW_H
#define MONTH_VIEW_H

#include <QGraphicsView>

class MonthView : public QGraphicsView
{
	Q_OBJECT
public:
	MonthView(QWidget *parent = 0) : QGraphicsView(parent) {}
};

#endif
