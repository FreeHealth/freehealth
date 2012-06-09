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
 *   Main Developpers:                                                     *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *       Pierre-Marie Desombre <pm.desombre@gmail.com>                     *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "alertitemtimingeditorwidget.h"
#include "alertitem.h"
#include "ui_alertitemtimingeditorwidget.h"

#include <translationutils/constants.h>
#include <translationutils/trans_datetime.h>

#include <QDebug>

using namespace Alert;
using namespace Trans::ConstantTranslations;

AlertItemTimingEditorWidget::AlertItemTimingEditorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Internal::Ui::AlertItemTimingEditorWidget),
    _periodicalCycling(false)
{
    ui->setupUi(this);
    layout()->setMargin(0);

    // set up dateedits
    ui->startDate->setDisplayFormat(tkTr(Trans::Constants::DATEFORMAT_FOR_EDITOR));
    ui->endDate->setDisplayFormat(tkTr(Trans::Constants::DATEFORMAT_FOR_EDITOR));
    ui->startTime->setDisplayFormat(tkTr(Trans::Constants::TIMEFORMAT_FOR_EDITOR));
    ui->endTime->setDisplayFormat(tkTr(Trans::Constants::TIMEFORMAT_FOR_EDITOR));
    ui->cycleCombo->addItem(tr("Not cycling"));
    ui->cycleCombo->addItem(tr("Cycle every"));
    ui->cyclingEvery->addItems(Trans::ConstantTranslations::periods());

    // manage date mofication (start < end)
    connect(ui->startDate, SIGNAL(editingFinished()), this, SLOT(checkDates()));
    connect(ui->endDate, SIGNAL(editingFinished()), this, SLOT(checkDates()));
//    connect(ui->cycleCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(cycleComboChanged(int)));
}

AlertItemTimingEditorWidget::~AlertItemTimingEditorWidget()
{
    delete ui;
}

void AlertItemTimingEditorWidget::clear()
{
    ui->cycleCombo->setCurrentIndex(-1);
    ui->cyclingEvery->setCurrentIndex(-1);
    ui->cycleDelayNumber->setValue(0);
    ui->cycles->setValue(0);
    ui->startDate->clear();
    ui->endDate->clear();
}

void AlertItemTimingEditorWidget::setAlertItem(const AlertItem &item)
{
    // Timings
    if (item.timings().count() > 0) {
        const AlertTiming &time = item.timingAt(0);
        ui->startDate->setDateTime(time.start());
        ui->endDate->setDateTime(time.end());
        if (time.isCycling())
            ui->cycleCombo->setCurrentIndex(1);
        else
            ui->cycleCombo->setCurrentIndex(0);
        ui->cycles->setValue(time.numberOfCycles());
        cyclingToUi(time);
    }
}

/** Submit the timing to the first Alert::AlertTiming of the \e item. */
bool AlertItemTimingEditorWidget::submit(AlertItem &item)
{
    if (item.timings().count() == 0) {
        AlertTiming time;
        item.addTiming(time);
    }
    const AlertTiming &time = item.timingAt(0);
    time.setStart(QDateTime(ui->startDate->date(), ui->startTime->time()));
    time.setEnd(QDateTime(ui->endDate->date(), ui->endTime->time()));
    if (ui->cycleCombo->currentIndex()==1) {
        cyclingFromUi(time);
    } else {
        time.setCycling(false);
    }
    return true;
}

void AlertItemTimingEditorWidget::cycleComboChanged(int index)
{
    if (index==1) {
        ui->cyclingEvery->show();
        ui->cycles->show();
        ui->cycleLabel->show();
        ui->cycleDelayNumber->show();
    } else {
        ui->cyclingEvery->hide();
        ui->cycles->hide();
        ui->cycleLabel->hide();
        ui->cycleDelayNumber->hide();
    }
}

void AlertItemTimingEditorWidget::checkDates()
{
    // startDate < endDate
    // TODO: add a small visual effect to alert user of the automated modification
    QDateEdit *de = qobject_cast<QDateEdit*>(sender());
    if (de==ui->startDate) {
        if (ui->startDate->date() > ui->endDate->date()) {
            ui->endDate->setDate(ui->startDate->date().addMonths(1));
        }
    } else {
        if (ui->startDate->date() > ui->endDate->date()) {
            ui->startDate->setDate(ui->endDate->date().addMonths(-1));
        }
    }
}

void AlertItemTimingEditorWidget::cyclingToUi(const AlertTiming &timing)
{
    _periodicalCycling = false;

    int period, mod;
    timing.cyclingDelayPeriodModulo(&period, &mod);
    switch (period) {
    case 0: // hours
        ui->cyclingEvery->setCurrentIndex(Trans::Constants::Time::Hours);
        break;
    case 1: // days
        ui->cyclingEvery->setCurrentIndex(Trans::Constants::Time::Days);
        break;
    case 2: // weeks
        ui->cyclingEvery->setCurrentIndex(Trans::Constants::Time::Weeks);
        break;
    case 3: // months
        ui->cyclingEvery->setCurrentIndex(Trans::Constants::Time::Months);
        break;
    case 4: // years
        ui->cyclingEvery->setCurrentIndex(Trans::Constants::Time::Year);
        break;
    case 5: // decades
        ui->cyclingEvery->setCurrentIndex(Trans::Constants::Time::Decade);
        break;
    case -1: // minutes
        // TODO: improve this -> 1442543521341 minutes is not really user friendly
        ui->cyclingEvery->setCurrentIndex(Trans::Constants::Time::Minutes);
        break;
    }
    ui->cycleDelayNumber->setValue(mod);
}

void AlertItemTimingEditorWidget::cyclingFromUi(Alert::AlertTiming &timing)
{
    time.numberOfCycles(ui->cycles->value());
    switch (ui->cyclingEvery->currentIndex()) {
    case Trans::Constants::Time::Seconds:
        timing.setCycling(true);
        timing.setCyclingDelayInMinutes(ui->cycleDelayNumber->value() / 60);
        break;
    case Trans::Constants::Time::Minutes:
        timing.setCycling(true);
        timing.setCyclingDelayInMinutes(ui->cycleDelayNumber->value());
        break;
    case Trans::Constants::Time::Hours:
        timing.setCycling(true);
        timing.setCyclingDelayInHours(ui->cycleDelayNumber->value());
        break;
    case Trans::Constants::Time::Days:
        timing.setCycling(true);
        timing.setCyclingDelayInDays(ui->cycleDelayNumber->value());
        break;
    case Trans::Constants::Time::Weeks:
        timing.setCycling(true);
        timing.setCyclingDelayInWeeks(ui->cycleDelayNumber->value());
        break;
    case Trans::Constants::Time::Months:
        timing.setCycling(true);
        timing.setCyclingDelayInMonth(ui->cycleDelayNumber->value());
        break;
    case Trans::Constants::Time::Quarter:
        timing.setCycling(true);
        timing.setCyclingDelayInMonth(ui->cycleDelayNumber->value() * 3);
        break;
    case Trans::Constants::Time::Year:
        timing.setCycling(true);
        timing.setCyclingDelayInYears(ui->cycleDelayNumber->value());
        break;
    case Trans::Constants::Time::Decade:
        timing.setCycling(true);
        timing.setCyclingDelayInDecades(ui->cycleDelayNumber->value());
        break;
    }
}

