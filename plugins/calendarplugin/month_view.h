#ifndef MONTH_VIEW_H
#define MONTH_VIEW_H

#include "view.h"

namespace Calendar {
	class MonthView : public View
	{
	public:
		virtual int topHeaderHeight() const;
		virtual int leftHeaderWidth() const;
	};
}

#endif
