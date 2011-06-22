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
	if (item.endingType() == Date_Date)
		dateEditEnd->setDate(item.ending().date().addDays(-1));
	else
		dateEditEnd->setDate(item.ending().date());
	timeEditEnd->setTime(item.ending().time());
	checkBoxAllDay->setChecked(item.beginningType() == Date_Date &&
							   item.endingType() == Date_Date);
	textEditDescription->setText(m_item.description());
}

void BasicItemEditionDialog::accept() {
	m_item.setTitle(lineEditTitle->text());
	m_item.setBeginningType(checkBoxAllDay->isChecked() ? Date_Date : Date_DateTime);
	m_item.setEndingType(checkBoxAllDay->isChecked() ? Date_Date : Date_DateTime);
	if (checkBoxAllDay->isChecked()) {
		m_item.setBeginning(QDateTime(dateEditStart->date(), QTime(0, 0)));
		m_item.setEnding(QDateTime(dateEditEnd->date().addDays(1), QTime(0, 0)));
	} else {
		m_item.setBeginning(QDateTime(dateEditStart->date(), timeEditStart->time()));
		m_item.setEnding(QDateTime(dateEditEnd->date(), timeEditEnd->time()));
	}
	m_item.setDescription(textEditDescription->toPlainText());

	QDialog::accept();
}

void BasicItemEditionDialog::on_checkBoxAllDay_toggled(bool checked) {
	timeEditStart->setVisible(!checked);
	timeEditEnd->setVisible(!checked);
}
