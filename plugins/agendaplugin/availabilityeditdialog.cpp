/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "availabilityeditdialog.h"
#include "ui_availabilityeditdialog.h"

#include "dayavailability.h"

#include <translationutils/constanttranslations.h>
#include <translationutils/trans_agenda.h>
#include <translationutils/trans_current.h>

#include <utils/global.h>

#include <QDate>
#include <QPushButton>

using namespace Agenda;
using namespace Trans::ConstantTranslations;

AvailabilityEditDialog::AvailabilityEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AvailabilityCreatorDialog)
{
    ui->setupUi(this);
    for(int i=1; i < 8;++i)
        ui->dayCombo->addItem(QDate::longDayName(i));
    ui->dayCombo->addItem(tkTr(Trans::Constants::FROM_1_TO_2).arg(QDate::longDayName(1), QDate::longDayName(5)));

    connect(ui->startTime, SIGNAL(timeChanged(QTime)), this, SLOT(updateUi()));
    connect(ui->endTime, SIGNAL(timeChanged(QTime)), this, SLOT(updateUi()));

    updateUi();
}

AvailabilityEditDialog::~AvailabilityEditDialog()
{
    delete ui;
}

void AvailabilityEditDialog::on_startTime_timeChanged(const QTime &from)
{
    ui->endTime->setMinimumTime(from);
}

void AvailabilityEditDialog::updateUi()
{
    // disable Ok button if start time >= end time
    const bool okEnabled = ui->startTime->time() < ui->endTime->time();
    QPushButton *okButton = ui->buttonBox->button(QDialogButtonBox::Ok);
    okButton->setEnabled(okEnabled);
    okButton->setToolTip(okEnabled? "" : tr("The end start time cannot be before the end time."));

}

QList<DayAvailability> AvailabilityEditDialog::getAvailabilities() const
{
    QList<DayAvailability> toReturn;
    int day = ui->dayCombo->currentIndex()+1; // Mo, Tu, We, Th, Fr, Sa, Su, Mo-Fr
    if (day != -1 && day <= Qt::Sunday) {
        // One day only
        DayAvailability av;
        av.setWeekDay(day);
        av.addTimeRange(ui->startTime->time(), ui->endTime->time());
        toReturn << av;
    } else if (day == Qt::Sunday+1) {
        // From monday to friday
        for(int i = Qt::Monday; i <= Qt::Friday ; ++i) {
            DayAvailability av;
            av.setWeekDay(i);
            av.addTimeRange(ui->startTime->time(), ui->endTime->time());
            toReturn << av;
        }
    }
    return toReturn;
}

void AvailabilityEditDialog::setAvailability(const int dayOfWeek, const QTime & from, const QTime &to)
{
    if (!IN_RANGE(dayOfWeek, Qt::Monday, Qt::Sunday)) {
        ui->dayCombo->setCurrentIndex(-1);
        ui->startTime->setTime(QTime(0,0));
        ui->endTime->setTime(QTime(0,0));
    } else {
        ui->dayCombo->setCurrentIndex(dayOfWeek-1);
        ui->startTime->setTime(from);
        ui->endTime->setTime(to);
    }
    updateUi();
}

/*! \brief Sets the day of week in the dialog. */
void AvailabilityEditDialog::setDayOfWeek(const int dayOfWeek)
{
    if (IN_RANGE(dayOfWeek, Qt::Monday, Qt::Sunday))
        ui->dayCombo->setCurrentIndex(dayOfWeek - 1);
}

/*! \brief Disables changing of the availability day during edit.
 *
 * Disable the change of day when you edit an existing TimeRange.
 */
void AvailabilityEditDialog::disableDayChange()
{
    ui->dayCombo->setEnabled(false);
}
