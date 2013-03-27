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
 *  Main Developers:                                                       *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *       Pierre-Marie Desombre <pm.desombre@gmail.com>                     *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 * \class Alert::AlertItemTimingEditorWidget
 * A timing editor widget (start, end, cycling info...) for alert items.
 */

#include "alertitemtimingeditorwidget.h"
#include "alertitem.h"
#include "ui_alertitemtimingeditorwidget.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <translationutils/constants.h>
#include <translationutils/trans_datetime.h>

#include <QDebug>

using namespace Alert;
using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}

AlertItemTimingEditorWidget::AlertItemTimingEditorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Internal::Ui::AlertItemTimingEditorWidget),
    _periodicalCycling(false)
{
    ui->setupUi(this);
    layout()->setMargin(0);

    // set up icons
    ui->startPeriodSelector->setIconSize(QSize(16,16));
    ui->startPeriodSelector->setIcon(theme()->icon(Core::Constants::ICONAGENDA_NEW));
    ui->endPeriodSelector->setIconSize(QSize(16,16));
    ui->endPeriodSelector->setIcon(theme()->icon(Core::Constants::ICONAGENDA_NEW));
    ui->startPeriodSelector->setStartPeriodsAt(Trans::Constants::Time::Days);
    ui->endPeriodSelector->setStartPeriodsAt(Trans::Constants::Time::Days);

    // set up dateedits
    ui->startDate->setDisplayFormat(QLocale().dateFormat());
    ui->endDate->setDisplayFormat(QLocale().dateFormat());

    // populate combos
    ui->cycleCombo->addItem(tr("Not cycling"));
    ui->cycleCombo->addItem(tr("Cycle every"));
    ui->cyclingEvery->addItems(Trans::ConstantTranslations::periods());

    // manage date mofication (start < end)
    connect(ui->startDate, SIGNAL(editingFinished()), this, SLOT(checkDates()));
    connect(ui->endDate, SIGNAL(editingFinished()), this, SLOT(checkDates()));
    connect(ui->cycleCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(cycleComboChanged(int)));
    // periods
    connect(ui->startPeriodSelector, SIGNAL(periodSelected(int,int)), this, SLOT(startPeriodSelected(int,int)));
    connect(ui->endPeriodSelector, SIGNAL(periodSelected(int,int)), this, SLOT(endPeriodSelected(int,int)));
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
        // TODO: add tooltip "alert in x years x months x days x hours x mins"
        if (time.expiration().isValid()) {
            ui->endDate->setDateTime(time.expiration());
        } else {
            ui->neverExpires->setChecked(false);
        }
        // TODO: add tooltip "alert in x years x months x days x hours x mins"
        if (time.isCycling())
            ui->cycleCombo->setCurrentIndex(1);
        else
            ui->cycleCombo->setCurrentIndex(0);
        ui->cycles->setValue(time.numberOfCycles());
        cyclingToUi(time);
    } else {
        ui->startDate->setDateTime(QDateTime(QDate::currentDate(), QTime(0,0,0)));
        ui->endDate->setDateTime(QDateTime(QDate::currentDate().addYears(1), QTime(23,59,0)));
        ui->cycleCombo->setCurrentIndex(0);
    }
}

static QDateTime getDateTimeFromPeriod(QDate from, const int amount, const int period)
{
    QDateTime start = QDateTime(from, QTime::currentTime());
    switch (period) {
    case Trans::Constants::Time::Seconds: start = start.addSecs(amount); break;
    case Trans::Constants::Time::Minutes: start = start.addSecs(amount*60); break;
    case Trans::Constants::Time::Hours: start = start.addSecs(amount*60*24); break;
    case Trans::Constants::Time::Days: start = start.addDays(amount);break;
    case Trans::Constants::Time::Weeks: start = start.addDays(amount*7);break;
    case Trans::Constants::Time::Months: start = start.addMonths(amount);break;
    case Trans::Constants::Time::Quarter: start = start.addMonths(amount*3);break;
    case Trans::Constants::Time::Year: start = start.addYears(amount);break;
    case Trans::Constants::Time::Decade: start = start.addYears(amount*10);break;
    }
    return start;
}

/** Submit the timing to the first Alert::AlertTiming of the \e item. */
bool AlertItemTimingEditorWidget::submit(AlertItem &item)
{
    if (item.timings().count() == 0) {
        AlertTiming time;
        item.addTiming(time);
    }
    AlertTiming &time = item.timingAt(0);
    time.setStart(QDateTime(ui->startDate->date(), QTime(0,0,0)));
    if (ui->neverExpires->isChecked())
        time.setExpiration(QDateTime());
    else
        time.setExpiration(QDateTime(ui->endDate->date(), QTime(23,59,59)));
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

void AlertItemTimingEditorWidget::startPeriodSelected(int period, int value)
{
    QDateTime start = getDateTimeFromPeriod(ui->startDate->date(), value, period);
    ui->startDate->setDate(start.date());
}

void AlertItemTimingEditorWidget::endPeriodSelected(int period, int value)
{
    QDateTime start = getDateTimeFromPeriod(ui->endDate->date(), value, period);
    ui->endDate->setDate(start.date());
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
    timing.setNumberOfCycles(ui->cycles->value());
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

