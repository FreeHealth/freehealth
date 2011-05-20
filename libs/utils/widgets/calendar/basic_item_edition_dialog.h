#ifndef BASIC_ITEM_EDITION_DIALOG_H
#define BASIC_ITEM_EDITION_DIALOG_H

#include "ui_basic_item_edition_dialog.h"
#include "calendar_item.h"

namespace Calendar {
	class BasicItemEditionDialog : public QDialog, private Ui::BasicItemEditionDialog
	{
		Q_OBJECT
	public:
		BasicItemEditionDialog(QWidget *parent = 0);

		const CalendarItem &item() const { return m_item; }

		void init(const CalendarItem &item);

	public slots:
		void accept();

	private:
		CalendarItem m_item;
	};
}

#endif
