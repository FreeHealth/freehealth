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
		 * The component computes the correct days interval to display this date
		 */
		void setDate(const QDate &date);

		const QDate &firstDate() const { return m_firstDate; }

		virtual void previousPage() {}
		virtual void nextPage() {}
		void todayPage();

	signals:
		void firstDateChanged();

	protected:
		QDate m_firstDate;

		/** this function computes the first interval date used by the view */
		virtual QDate getFirstDateByRandomDate(const QDate &randomDate) = 0;
	};
}

#endif
