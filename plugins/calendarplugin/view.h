#ifndef VIEW_H
#define VIEW_H

class QPainter;
class QRect;

namespace Calendar {
	/**
	 * \brief a view is an abstract class which defines common things about calendar views (headers, body, etc)
	 */
	class View
	{
	public:
		/** returns the top header height for the view */
		virtual int topHeaderHeight() const = 0;
		/** returns the left height width for the view */
		virtual int leftHeaderWidth() const = 0;

		virtual void paintTopHeader(QPainter *painter) {};
		virtual void paintLeftHeader(QPainter *painter) {};
		virtual void paintBody(QPainter *painter, const QRect &visibleRect) {};
	};
}

#endif
