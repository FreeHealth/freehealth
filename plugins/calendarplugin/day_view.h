#ifndef DAY_VIEW_H
#define DAY_VIEW_H

#include "view.h"

namespace Calendar {
	class DayView : public View
	{
		Q_OBJECT
	public:
		DayView(QWidget *parent = 0) : View(parent) {}

		virtual int topHeaderHeight() const;
		virtual int leftHeaderWidth() const;
	};
}

#endif
