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
#include "item_editor_widget.h"
#include "calendar_item.h"
#include "usercalendar.h"

#include <translationutils/constanttranslations.h>

#include "ui_item_editor_widget.h"

#include <QIcon>
#include <QStandardItemModel>
#include <QDebug>

using namespace Calendar;
using namespace Trans::ConstantTranslations;

namespace {
    const int durationDivider = 5; // define a duration each 5 minutes
}

namespace Calendar {
namespace Internal {

    class ItemEditorWidgetPrivate
    {
    public:
        ItemEditorWidgetPrivate(ItemEditorWidget *parent) :
                ui(new Internal::Ui::ItemEditorWidget),
                m_UserCalsModel(0),
                q(parent)
        {
        }

        ~ItemEditorWidgetPrivate() {}

        void populateDurationCombo()
        {
            ui->durationCombo->clear();
            for(int i = 0; i < (60/durationDivider); ++i) {
                ui->durationCombo->addItem(QString::number(i*5) + " " + tkTr(Trans::Constants::MINUTES));
            }
        }

        void setEventToUi()
        {
//            ui->typeCombo->setCurrentIndex();
            QDateTime start = m_Item.data(CalendarItem::DateStart).toDateTime();
            QDateTime end = m_Item.data(CalendarItem::DateEnd).toDateTime();
            int durationInMinutes = start.secsTo(end) / 60;
            ui->durationCombo->setCurrentIndex(durationInMinutes / durationDivider);
            ui->location->setText(m_Item.data(CalendarItem::Location).toString());
            ui->startDate->setDate(start.date());
            ui->endDate->setDate(end.date());
            ui->startTime->setTime(start.time());
            ui->endTime->setTime(end.time());
//            ui->durationCombo->setCurrentIndex(-1);
            ui->busyCheck->setChecked(m_Item.data(CalendarItem::IsBusy).toBool());
            ui->privateCheck->setChecked(m_Item.data(CalendarItem::IsPrivate).toBool());
            ui->password->setText(m_Item.data(CalendarItem::Password).toString());
            ui->eventLabel->setText(m_Item.data(CalendarItem::Label).toString());
            ui->fullInfo->setText(m_Item.data(CalendarItem::Description).toString());
//            ui->iconLabel->setPixmap(theme()->icon(m_Item.data(CalendarItem::ThemedIcon).toString()).pixmap(16, 16));
        }

        void submit()
        {
            if (m_Item.isNull())
                return;
            m_Item.setData(CalendarItem::DateStart, QDateTime(ui->startDate->date(), ui->startTime->time()));
            m_Item.setData(CalendarItem::DateEnd, QDateTime(ui->endDate->date(), ui->endTime->time()));
            m_Item.setData(CalendarItem::Location, ui->location->text());
            m_Item.setData(CalendarItem::IsBusy, ui->busyCheck->isChecked());
            m_Item.setData(CalendarItem::IsPrivate, ui->privateCheck->isChecked());
            m_Item.setData(CalendarItem::Password, ui->password->text());
            m_Item.setData(CalendarItem::Label, ui->eventLabel->text());
            m_Item.setData(CalendarItem::Description,ui->fullInfo->toHtml());
//            m_Item.setData(CalendarItem::ThemedIcon, QString());

            // get user calendar
//            if (ui->calendarCombo->currentIndex() <= m_UserCals.count()) {
//                UserCalendar *ucal = m_UserCals.at(ui->calendarCombo->currentIndex());
//                m_Item.setData(CalendarItem::DbOnly_CalId, ucal->calendarId());
//            }
        }

    public:
        Ui::ItemEditorWidget *ui;
        Calendar::CalendarItem m_Item;
        QList<UserCalendar *> m_UserCals;
        QStandardItemModel *m_UserCalsModel;

    private:
        ItemEditorWidget *q;
    };
}
}


ItemEditorWidget::ItemEditorWidget(QWidget *parent) :
    QWidget(parent),
    d(new Internal::ItemEditorWidgetPrivate(this))
{
    d->ui->setupUi(this);
    d->populateDurationCombo();
    connect(d->ui->durationCombo, SIGNAL(activated(int)), this, SLOT(changeDuration(int)));
}

ItemEditorWidget::~ItemEditorWidget()
{
    delete d;
}

void ItemEditorWidget::clear()
{
    d->ui->typeCombo->setCurrentIndex(-1);
    d->ui->location->clear();
    d->ui->startDate->setDate(QDate::currentDate());
    d->ui->endDate->setDate(QDate::currentDate());
    d->ui->startTime->setTime(QTime::currentTime());
    d->ui->endTime->setTime(QTime::currentTime());
    d->ui->durationCombo->setCurrentIndex(-1);
    d->ui->busyCheck->setChecked(false);
    d->ui->privateCheck->setChecked(false);
    d->ui->password->clear();
    d->ui->eventLabel->clear();
    d->ui->fullInfo->clear();
    d->ui->iconLabel->clear();
}

void ItemEditorWidget::setCalendarEvent(const CalendarItem &item)
{
    d->m_Item = item;
    // Clear ui
    clear();
    // Populate ui
    d->setEventToUi();
}

Calendar::CalendarItem ItemEditorWidget::calendarEvent() const
{
    return d->m_Item;
}

void ItemEditorWidget::setAvailableUserCalendar(const QList<UserCalendar *> &userCals)
{
    d->m_UserCals = userCals;

    // create the model
    if (d->m_UserCalsModel) {
        delete d->m_UserCalsModel;
        d->m_UserCalsModel = 0;
    }
    d->m_UserCalsModel = new QStandardItemModel(this);
    QStandardItem *root = d->m_UserCalsModel->invisibleRootItem();
    int defaultRow = -1;
    for(int i = 0; i < userCals.count(); ++i) {
        root->appendRow(userCals.at(i)->toStandardItem());
        if (userCals.at(i)->data(UserCalendar::IsDefault).toBool()) {
            defaultRow = i;
        }
    }
    d->ui->calendarCombo->setModel(d->m_UserCalsModel);
    d->ui->calendarCombo->setCurrentIndex(defaultRow);
}

/** Submit UI changes to the internal Agenda::CalendarItem \sa calendarEvent(), setCalendarEvent() */
void ItemEditorWidget::submit()
{
    d->submit();
}

void ItemEditorWidget::on_selectIconButton_clicked()
{}

void ItemEditorWidget::changeDuration(const int comboIndex)
{
    QTime end = d->ui->startTime->time();
    end = end.addSecs(comboIndex * durationDivider * 60);
    d->ui->endTime->setTime(end);
}

void ItemEditorWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        d->ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
