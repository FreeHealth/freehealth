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
 *   Main Developers :                                                    *
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/

/**
  \class Calendar::ItemEditorWidget
  Basic editing widget of Calendar::CalendarItem. This widget can be extended with the
  usage of Calendar::ICalendarItemDataWidget objects.
*/

#include "item_editor_widget.h"
#include "calendar_item.h"
#include "abstract_calendar_model.h"
#include <calendar/icalendaritemdatawidget.h>
#include <calendar/common.h>

#include <translationutils/constants.h>
#include <translationutils/trans_datetime.h>
#include <utils/log.h>

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
            m_Model(0),
            ui(new Internal::Ui::ItemEditorWidget),
            m_UserCalsModel(0),
            m_ShowingExtra(true),
            q(parent)
        {
        }

        ~ItemEditorWidgetPrivate() {}

        void populateDurationCombo()
        {
            ui->durationCombo->clear();
            for(int i = 0; i < (120/durationDivider); ++i) {
                ui->durationCombo->addItem(QString::number(i*5) + " " + tkTr(Trans::Constants::MINUTES));
            }
        }

        void populateStatusCombo()
        {
            ui->statusCombo->addItems(availableStatus());
        }

        void setEventToUi()
        {
            QDateTime start = m_Item.beginning();
            QDateTime end = m_Item.ending();
            int durationInMinutes = start.secsTo(end) / 60;
            ui->durationCombo->setCurrentIndex(durationInMinutes / durationDivider);
            ui->location->setText(m_Item.data(CalendarItem::Location).toString());
            ui->startDate->setDate(start.date());
            ui->endDate->setDate(end.date());
            ui->startTime->setTime(start.time());
            ui->endTime->setTime(end.time());
            ui->busyCheck->setChecked(m_Item.data(CalendarItem::IsBusy).toBool());
            ui->privateCheck->setChecked(m_Item.data(CalendarItem::IsPrivate).toBool());
            ui->password->setText(m_Item.data(CalendarItem::Password).toString());
            ui->eventLabel->setText(m_Item.data(CalendarItem::Label).toString());
            ui->fullInfo->setText(m_Item.data(CalendarItem::Description).toString());
//            ui->iconLabel->setPixmap(theme()->icon(m_Item.data(CalendarItem::ThemedIcon).toString()).pixmap(16, 16));

//            if (m_Item.model()) {
//			ui->calendarCombo->setModel(m_Item.model()->userCalendarComboModel(q));
//            } else {
//                ui->calendarCombo->setModel(m_Model->userCalendarComboModel(q));
//            }

            ui->statusCombo->setCurrentIndex(m_Item.data(CalendarItem::Status).toInt());
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
            m_Item.setData(CalendarItem::Status, ui->statusCombo->currentIndex());
//            m_Item.setData(CalendarItem::ThemedIcon, QString());
        }

    public:
        AbstractCalendarModel *m_Model;
        Ui::ItemEditorWidget *ui;
        Calendar::CalendarItem m_Item;
        QList<UserCalendar *> m_UserCals;
        QStandardItemModel *m_UserCalsModel;
        QVector<ICalendarItemDataWidget *> m_AddedWidgets;
        bool m_ShowingExtra;

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
    d->ui->startDate->setDisplayFormat(tkTr(Trans::Constants::DATEFORMAT_FOR_EDITOR));
    d->ui->endDate->setDisplayFormat(tkTr(Trans::Constants::DATEFORMAT_FOR_EDITOR));
    d->ui->tabWidget->setCurrentIndex(0);

    d->populateDurationCombo();
    d->populateStatusCombo();
    connect(d->ui->durationCombo, SIGNAL(activated(int)), this, SLOT(changeDuration(int)));

    // hide extra information
    toogleExtraInformation();

    adjustSize();
}

ItemEditorWidget::~ItemEditorWidget()
{
    delete d;
}

/** Clear the widget of its data. All changes will be lost. You must redefine the Calendar::CalendarItem to edit. \sa setCalendarEvent() */
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

    // clear addedWidgets
    foreach(ICalendarItemDataWidget * widget, d->m_AddedWidgets) {
        widget->clear();
    }
}

/** Define the Calendar::AbstractCalendarModel to use for the current edition. */
void ItemEditorWidget::setModel(AbstractCalendarModel *model)
{
    Q_ASSERT(model);
    d->m_Model = model;
}

/** Define the Calendar::CalendarItem to use for the current edition. */
void ItemEditorWidget::setCalendarItem(const CalendarItem &item)
{
    d->m_Item = item;
    // Clear ui
    clear();
    // Populate ui
    d->setEventToUi();

    // set to addedWidgets
    foreach(ICalendarItemDataWidget *widget, d->m_AddedWidgets) {
        widget->setCalendarItem(item);
    }
}

/** Return the modification done on the Calendar::CalendarItem. You must call submit() before if you want to get the update version of the Calendar::CalendarItem. \sa submit(), setCalendarEvent() */
Calendar::CalendarItem ItemEditorWidget::calendarEvent() const
{
    return d->m_Item;
}

/** Hide/show the extra information. */
void ItemEditorWidget::toogleExtraInformation()
{
    d->m_ShowingExtra = !d->m_ShowingExtra;
    d->ui->tabWidget->setVisible(d->m_ShowingExtra);
    adjustSize();
}

/** Return true is the view is showing the extra information. */
bool ItemEditorWidget::isShowingExtraInformation() const
{
    return d->m_ShowingExtra;
}

/** Define the Calendar::UserCalendar to use for the current edition. */
//void ItemEditorWidget::setAvailableUserCalendar(const QList<UserCalendar *> &userCals)
//{
//    d->m_UserCals = userCals;

//    // create the model
//    if (d->m_UserCalsModel) {
//        delete d->m_UserCalsModel;
//        d->m_UserCalsModel = 0;
//    }
//    d->m_UserCalsModel = new QStandardItemModel(this);
//    QStandardItem *root = d->m_UserCalsModel->invisibleRootItem();
//    int defaultRow = -1;
//    for(int i = 0; i < userCals.count(); ++i) {
//        root->appendRow(new QStandardItem(userCals.at(i)->data(Calendar::UserCalendar::Label).toString()));
//        if (userCals.at(i)->data(UserCalendar::IsDefault).toBool()) {
//            defaultRow = i;
//        }
//    }
//    d->ui->calendarCombo->setModel(d->m_UserCalsModel);
//    d->ui->calendarCombo->setCurrentIndex(defaultRow);
//}

/**
  Add specific widgets to the editor using the Calendar::ICalendarItemDataWidget interface.
  You must set the Calendar::CalendarItem AFTER you have added ALL your Calendar::ICalendarItemDataWidget.
*/
void ItemEditorWidget::addCalendarDataWidget(Calendar::ICalendarItemDataWidget *dataWidget)
{
    dataWidget->setCalendarItemModel(d->m_Model);
    d->m_AddedWidgets << dataWidget;
    QWidget *widget = dataWidget->createWidget(this);
    switch (dataWidget->insertionPlace()) {
    case ICalendarItemDataWidget::Beginning: d->ui->beginningLayout->addWidget(widget); break;
    case ICalendarItemDataWidget::Ending: d->ui->endingLayout->addWidget(widget); break;
    case ICalendarItemDataWidget::AfterDateTime: d->ui->afterDateTimeLayout->addWidget(widget); break;
    case ICalendarItemDataWidget::AfterDescription: d->ui->afterDescriptionLayout->addWidget(widget); break;
    case ICalendarItemDataWidget::AfterGeneralInformation: d->ui->afterGeneralInformationLayout->addWidget(widget); break;
    case ICalendarItemDataWidget::BeforeDateTime: d->ui->beforeDateTimeLayout->addWidget(widget); break;
    case ICalendarItemDataWidget::BeforeDescrition: d->ui->beforeDescriptionLayout->addWidget(widget); break;
    }
}

/** Submit UI changes to the internal Agenda::CalendarItem \sa calendarEvent(), setCalendarEvent() */
void ItemEditorWidget::submit()
{
    // add data in addedWidgets to the model
    foreach(ICalendarItemDataWidget *widget, d->m_AddedWidgets) {
        widget->submitChangesToCalendarItem(d->m_Item);
    }
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
