#include "basic_item_edition_dialog.h"

using namespace Calendar;

BasicItemEditionDialog::BasicItemEditionDialog(QWidget *parent) :
	QDialog(parent) {
	setupUi(this);
}

void BasicItemEditionDialog::init(const CalendarItem &item) {
	m_item = item;

	lineEditTitle->setText(m_item.title());
	dateEditStart->setDate(item.beginning().date());
	timeEditStart->setTime(item.beginning().time());
	dateEditEnd->setDate(item.ending().date());
	timeEditEnd->setTime(item.ending().time());
	textEditDescription->setText(m_item.description());
}

void BasicItemEditionDialog::accept() {
	m_item.setTitle(lineEditTitle->text());
	m_item.setBeginning(QDateTime(dateEditStart->date(), timeEditStart->time()));
	m_item.setEnding(QDateTime(dateEditEnd->date(), timeEditEnd->time()));
	m_item.setDescription(textEditDescription->toPlainText());

	QDialog::accept();
}
