#ifndef MONTH_VIEW_H
#define MONTH_VIEW_H

#include "view.h"

namespace Calendar {
	class MonthView : public View
	{
		Q_OBJECT
	public:
		MonthView(QWidget *parent = 0) : View(parent) {}

		virtual int topHeaderHeight() const;
		virtual int leftHeaderWidth() const;

		virtual QWidget *createHeaderWidget(QWidget *parent = 0);
	};
}

#endif
