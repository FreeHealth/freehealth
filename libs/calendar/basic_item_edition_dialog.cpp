/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developpers :                                                    *
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "basic_item_edition_dialog.h"
#include "calendar_item.h"

#include "ui_basic_item_edition_dialog.h"

using namespace Calendar;

BasicItemEditionDialog::BasicItemEditionDialog(AbstractCalendarModel *model, QWidget *parent) :
        QDialog(parent),
        m_Model(model),
        ui(new Internal::Ui::BasicItemEditionDialog)
{
    Q_ASSERT(model);
    ui->setupUi(this);
    ui->viewer->setModel(model);
}

BasicItemEditionDialog::~BasicItemEditionDialog()
{
    delete ui;
}

void BasicItemEditionDialog::init(const CalendarItem &item)
{
    ui->viewer->submit();
    ui->viewer->setCalendarEvent(item);
}

Calendar::CalendarItem BasicItemEditionDialog::item() const
{
    return ui->viewer->calendarEvent();
}

//	lineEditTitle->setText(m_item.title());
//	dateEditStart->setDate(item.beginning().date());
//	timeEditStart->setTime(item.beginning().time());
//	if (item.endingType() == Date_Date)
//		dateEditEnd->setDate(item.ending().date().addDays(-1));
//	else
//		dateEditEnd->setDate(item.ending().date());
//	timeEditEnd->setTime(item.ending().time());
//	checkBoxAllDay->setChecked(item.beginningType() == Date_Date &&
//							   item.endingType() == Date_Date);
//	textEditDescription->setText(m_item.description());
//}

//void BasicItemEditionDialog::accept() {
//	m_item.setTitle(lineEditTitle->text());
//	m_item.setBeginningType(checkBoxAllDay->isChecked() ? Date_Date : Date_DateTime);
//	m_item.setEndingType(checkBoxAllDay->isChecked() ? Date_Date : Date_DateTime);
//	if (checkBoxAllDay->isChecked()) {
//		m_item.setBeginning(QDateTime(dateEditStart->date(), QTime(0, 0)));
//		m_item.setEnding(QDateTime(dateEditEnd->date().addDays(1), QTime(0, 0)));
//	} else {
//		m_item.setBeginning(QDateTime(dateEditStart->date(), timeEditStart->time()));
//		m_item.setEnding(QDateTime(dateEditEnd->date(), timeEditEnd->time()));
//	}
//	m_item.setDescription(textEditDescription->toPlainText());

//	QDialog::accept();
//}

//void BasicItemEditionDialog::on_checkBoxAllDay_toggled(bool checked) {
//	timeEditStart->setVisible(!checked);
//	timeEditEnd->setVisible(!checked);
//}
