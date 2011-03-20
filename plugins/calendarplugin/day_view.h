#ifndef DAY_VIEW_H
#define DAY_VIEW_H

#include "view.h"

namespace Calendar {
	class DayView : public View
	{
	public:
		virtual int topHeaderHeight() const;
		virtual int leftHeaderWidth() const;
	};
}

#endif
