/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "availabilityeditdialog.h"
#include "ui_availabilityeditdialog.h"

#include "dayavailability.h"

#include <translationutils/constants.h>
#include <translationutils/trans_agenda.h>
#include <translationutils/trans_current.h>

#include <QDate>

using namespace Agenda;
using namespace Trans::ConstantTranslations;

AvailabilityEditDialog::AvailabilityEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AvailabilityCreatorDialog)
{
    ui->setupUi(this);
    for(int i=1; i < 8;++i)
        ui->dayCombo->addItem(QDate::longDayName(i));
    ui->dayCombo->addItem(tkTr(Trans::Constants::FROM_1_TO_2).arg(QDate::longDayName(1)).arg(QDate::longDayName(5)));
}

AvailabilityEditDialog::~AvailabilityEditDialog()
{
    delete ui;
}

void AvailabilityEditDialog::on_startTime_timeChanged(const QTime &from)
{
    ui->endTime->setMinimumTime(from);
}

QList<DayAvailability> AvailabilityEditDialog::getAvailability() const
{
    QList<DayAvailability> toReturn;
    int id = ui->dayCombo->currentIndex();
    if (id < 7) {
        // One day only
        DayAvailability av;
        av.setWeekDay(id+1);
        av.addTimeRange(ui->startTime->time(), ui->endTime->time());
        toReturn << av;
    } else if (id == 7) {
        // From monday to friday
        for(int i=1; i < 6; ++i) {
            DayAvailability av;
            av.setWeekDay(i);
            av.addTimeRange(ui->startTime->time(), ui->endTime->time());
            toReturn << av;
        }
    }
    return toReturn;
}
