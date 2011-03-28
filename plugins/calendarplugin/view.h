#ifndef VIEW_H
#define VIEW_H

#include <QWidget>
#include <QDate>

class QPainter;
class QRect;

namespace Calendar {
	/**
	 * \brief a view is an abstract class which defines common things about calendar views (headers, body, etc)
	 */
	class View : public QWidget
	{
		Q_OBJECT
	public:
		View(QWidget *parent = 0) : QWidget(parent) {}

		/** returns the top header height for the view */
		virtual int topHeaderHeight() const = 0;
		/** returns the left height width for the view */
		virtual int leftHeaderWidth() const = 0;

		virtual void paintBody(QPainter *painter, const QRect &visibleRect) {};

		virtual QWidget *createHeaderWidget(QWidget *parent = 0) { return 0; }

		/** sets a date for the view
		 */
		void setFirstDate(const QDate &firstDate);

		const QDate &firstDate() const { return m_firstDate; }

	protected:
		QDate m_firstDate;
	};
}

#endif
