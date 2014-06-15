/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/

/**
 * \class Agenda::UserCalendarViewer
 * Is a Agenda::UserCalendar widget viewer for FreeMedForms used in the Agenda mode.
 *
 * This widget presents:
 * - a left bar with some usercalendar information, plus automated next availabilities calculation
 * - a central Calendar::CalendarViewer for users to edit appointments
 */

#include "usercalendarviewer.h"
#include "agendabase.h"
#include "constants.h"
#include "calendaritemmodel.h"
#include "usercalendar.h"
#include "usercalendarmodel.h"
#include "agendacore.h"
#include "dayavailability.h"

#include <coreplugin/modemanager/modemanager.h>
#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/actionmanager/actionmanager.h>

#include <patientbaseplugin/patientcore.h>

#include <calendar/modelanditem/basic_item_edition_dialog.h>
#include <utils/log.h>
#include <translationutils/constants.h>
#include <translationutils/trans_agenda.h>
#include <translationutils/trans_datetime.h>

#include "ui_usercalendarviewer.h"

#include <QStandardItemModel>
#include <QStandardItem>
#include <QHash>
#include <QMenu>

using namespace Agenda;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::IUser *user() {return Core::ICore::instance()->user();}
static inline Agenda::Internal::AgendaBase &base() {return Agenda::AgendaCore::instance().agendaBase();}
static inline Agenda::AgendaCore &agendaCore() {return Agenda::AgendaCore::instance();}
static inline Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}
static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Patients::PatientCore *patientCore() {return Patients::PatientCore::instance();}

namespace {
    const int S_NUMBEROFAVAILABILITIESTOSHOW = 10;
}

namespace Agenda {
namespace Internal {
class UserCalendarViewerPrivate
{
public:
    UserCalendarViewerPrivate(UserCalendarViewer *parent) :
        ui(new Ui::UserCalendarViewer),
        m_CalendarItemModel(0),
        m_ItemContextMenu(0),
        m_UserCalendarModel(agendaCore().userCalendarModel(user()->uuid())),
        aToday(0), aTomorrow(0), aNextWeek(0), aNextMonth(0),
        aSwitchToPatient(0), aEditItem(0), aPrintItem(0), aDeleteItem(0),
//        aRefreshView(0),
        m_AvailModel(0),
        q(parent)
    {
    }

    ~UserCalendarViewerPrivate()
    {
        delete ui;
//        if (m_CalendarItemModel) {
//            delete m_CalendarItemModel;
//            m_CalendarItemModel = 0;
//        }
//        if (m_UserCalendarModel) {
//            delete m_UserCalendarModel;
//            m_UserCalendarModel = 0;
//        }
    }

    void createActions()
    {
        aToday = new QAction(q);
        aTomorrow = new QAction(q);
        aNextWeek = new QAction(q);
        aNextMonth = new QAction(q);
//        aRefreshView = new QAction(q);
        aToday->setIcon(theme()->icon(Core::Constants::ICONDATE));
        aTomorrow->setIcon(theme()->icon(Core::Constants::ICONDATE));
        aNextWeek->setIcon(theme()->icon(Core::Constants::ICONDATE));
        aNextMonth->setIcon(theme()->icon(Core::Constants::ICONDATE));
//        aRefreshView->setIcon(theme()->icon(Core::Constants::ICONSOFTWAREUPDATEAVAILABLE));
        ui->startDateSelector->addAction(aToday);
        ui->startDateSelector->addAction(aTomorrow);
        ui->startDateSelector->addAction(aNextWeek);
        ui->startDateSelector->addAction(aNextMonth);
        ui->startDateSelector->setDefaultAction(aToday);
        QObject::connect(ui->startDateSelector, SIGNAL(triggered(QAction*)), q, SLOT(quickDateSelection(QAction*)));
    }

    void populateCalendarWithCurrentWeek(Agenda::UserCalendar *calendar)
    {
        Q_UNUSED(calendar);
        ui->calendarViewer->setModel(m_CalendarItemModel);
    }

    void createCalendarItemContextMenu()
    {
        if (!m_ItemContextMenu) {
            m_ItemContextMenu = new QMenu(q);
            aSwitchToPatient = new QAction(q);
            aEditItem = new QAction(q);
            aPrintItem = new QAction(q);
            aDeleteItem = new QAction(q);

            m_ItemContextMenu->addAction(aSwitchToPatient);
            m_ItemContextMenu->addAction(aEditItem);
            // TODO: re add the print appointement action
//            m_ItemContextMenu->addAction(aPrintItem);
            m_ItemContextMenu->addAction(aDeleteItem);

            ui->calendarViewer->setContextMenuForItems(m_ItemContextMenu);
            QObject::connect(aSwitchToPatient, SIGNAL(triggered()), q, SLOT(onSwitchToPatientClicked()));
            QObject::connect(aEditItem, SIGNAL(triggered()), q, SLOT(onEditAppointmentClicked()));
            QObject::connect(aPrintItem, SIGNAL(triggered()), q, SLOT(onPrintAppointmentClicked()));
            QObject::connect(aDeleteItem, SIGNAL(triggered()), q, SLOT(onDeleteAppointmentClicked()));
        }
    }

public:
    Ui::UserCalendarViewer *ui;
    CalendarItemModel *m_CalendarItemModel;
    QMenu *m_ItemContextMenu;
    UserCalendarModel *m_UserCalendarModel;
    QHash<QString, int> m_UidToListIndex;
    bool scrollOnShow;

    QAction *aToday, *aTomorrow, *aNextWeek, *aNextMonth;
    QAction *aSwitchToPatient, *aEditItem, *aPrintItem, *aDeleteItem;
//    QAction *aRefreshView;
    QStandardItemModel *m_AvailModel;

private:
    UserCalendarViewer *q;
};
}  // End namespace Internal
}  // End namespace Agenda

UserCalendarViewer::UserCalendarViewer(QWidget *parent) :
    QWidget(parent),
    d(new UserCalendarViewerPrivate(this))
{
    setObjectName("UserCalendarViewer");
    d->ui->setupUi(this);
    this->layout()->setMargin(0);
    d->ui->startDate->setDate(QDate::currentDate());
    d->ui->startDate->setDisplayFormat(tkTr(Trans::Constants::DATEFORMAT_FOR_EDITOR));
    d->createActions();

    d->ui->refreshAvailabilities->setIcon(theme()->icon(Core::Constants::ICONSOFTWAREUPDATEAVAILABLE));
    d->ui->refreshAvailabilities->setToolTip(tr("Refresh Availabilities"));
    d->ui->calendarViewer->setDate(QDate::currentDate());
    d->ui->calendarViewer->setDayScaleHourDivider(2);
    d->ui->calendarViewer->setDayGranularity(5);
    d->ui->calendarViewer->setHourHeight(4*20); // 20pixels per minutes
    d->createCalendarItemContextMenu();

    // populate the availabilities duration selector combo (every five minutes)
    for(int i = 1; i < 19; ++i) {
        d->ui->availDurationCombo->addItem(QString("%1 %2").arg(i*5).arg(tkTr(Trans::Constants::MINUTES)));
    }
    connect(d->ui->availDurationCombo, SIGNAL(activated(int)), this, SLOT(recalculateAvailabilitiesWithDurationIndex(int)));

    Agenda::UserCalendar *cal = d->m_UserCalendarModel->defaultUserCalendar();
    if (cal && cal->isValid()) {
        d->ui->description->setHtml(cal->data(Agenda::UserCalendar::Description).toString());
        resetDefaultDuration();
    }
    d->ui->defaultDurationButton->setText("80");
    d->ui->defaultDurationButton->setIcon(theme()->icon(Constants::I_RESET_TO_DEFAULT));

    int width = size().width();
    int third = width/3;
    d->ui->splitter->setSizes(QList<int>() << third << width-third);

    connect(user(), SIGNAL(userChanged()), this, SLOT(userChanged()));
    connect(d->ui->availabilitiesView, SIGNAL(activated(QModelIndex)), this, SLOT(newEventAtAvailabity(QModelIndex)));
    connect(d->ui->availableAgendasCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(on_availableAgendasCombo_activated(int)));
    connect(d->ui->refreshAvailabilities, SIGNAL(clicked()), this, SLOT(refreshAvailabilities()));
    connect(d->ui->defaultDurationButton, SIGNAL(clicked()), this, SLOT(resetDefaultDuration()));
    connect(d->ui->startDate, SIGNAL(dateChanged(QDate)), this, SLOT(onStartDateChanged(QDate)));
    userChanged();

    // Connect menu actions
    Core::Command *cmd = actionManager()->command(Constants::A_NEW_AGENDAEVENT);
    connect(cmd->action(),SIGNAL(triggered()), this, SLOT(newEvent()));
}

UserCalendarViewer::~UserCalendarViewer()
{
    if (d) {
        delete d;
        d = 0;
    }
}

void UserCalendarViewer::recalculateComboAgendaIndex()
{
    d->ui->availableAgendasCombo->setCurrentIndex(d->m_UserCalendarModel->defaultUserCalendarModelIndex().row());
    on_availableAgendasCombo_activated(d->m_UserCalendarModel->defaultUserCalendarModelIndex().row());
}

void UserCalendarViewer::newEvent()
{
    newEventAtAvailabity(QModelIndex());
}

/** When the user selects an availability in the toolButton, this slot is activated.
Opens an EventEditor dialog and saves the item in the CalendarItemModel if the dialog is accepted. */
void UserCalendarViewer::newEventAtAvailabity(const QModelIndex &index)
{
    QModelIndex idx = index;
    if (!index.isValid()) {
        // use the first available date
        idx = d->m_AvailModel->index(0,0);
        idx = d->m_AvailModel->index(0,0, idx);
    }
    Calendar::BasicItemEditorDialog dlg(d->m_CalendarItemModel, this);
    QDateTime start = d->m_AvailModel->itemFromIndex(idx)->data().toDateTime();
    if (start.isNull())
        return;
//    if (!action) {
//        if (d->ui->availButton->actions().count() > 0) {
//            action = d->ui->availButton->actions().at(0);
//        }
//    }
//    if (action) {
//        start = action->data().toDateTime();
//    }

    Calendar::CalendarItem item = d->m_CalendarItemModel->insertItem(start, start.addSecs((d->ui->availDurationCombo->currentIndex()+1)*5*60));
    dlg.init(item);
    if (dlg.exec() != QDialog::Accepted) {
        d->m_CalendarItemModel->removeItem(item.uid());
    }
    recalculateAvailabilitiesWithDurationIndex(d->ui->availDurationCombo->currentIndex());
}

void UserCalendarViewer::refreshAvailabilities()
{
    recalculateAvailabilitiesWithDurationIndex(d->ui->availDurationCombo->currentIndex());
}

/** Resets the duration in the ComboBox to the default duration set in the Agenda config. */
void UserCalendarViewer::resetDefaultDuration()
{
    int currentCalendarIndex = d->ui->availableAgendasCombo->currentIndex();

    QModelIndex index = d->m_UserCalendarModel->index(currentCalendarIndex, UserCalendarModel::DefaultDuration);
    updateCalendarData(index,QModelIndex());
}

void UserCalendarViewer::quickDateSelection(QAction *a)
{
    if (a==d->aToday)
        d->ui->startDate->setDate(QDate::currentDate());
    if (a==d->aTomorrow)
        d->ui->startDate->setDate(QDate::currentDate().addDays(1));
    if (a==d->aNextWeek)
        d->ui->startDate->setDate(QDate::currentDate().addDays(8 - QDate::currentDate().dayOfWeek()));
    if (a==d->aNextMonth)
        d->ui->startDate->setDate(QDate(QDate::currentDate().year(), QDate::currentDate().month(), 1).addMonths(1));
    onStartDateChanged(d->ui->startDate->date());
}

void UserCalendarViewer::onStartDateChanged(const QDate &start)
{
    Q_UNUSED(start);
    recalculateAvailabilitiesWithDurationIndex(d->ui->availDurationCombo->currentIndex());
}

void UserCalendarViewer::recalculateAvailabilitiesWithDurationIndex(const int index)
{
    // TODO: Create a tree model for the nextAvailableDates

//    for(int i = d->ui->availButton->actions().count()-1; i > -1 ; --i)
//        d->ui->availButton->removeAction(d->ui->availButton->actions().at(i));

    if (index<0)
        return;

    // get currently selected calendar
    int calId = d->ui->availableAgendasCombo->currentIndex();
    Agenda::UserCalendar *cal = 0;
    if (calId > 0)
        cal = d->m_UserCalendarModel->userCalendarAt(calId);
    else
        cal = d->m_UserCalendarModel->defaultUserCalendar();

    // Get the next available dates
    QList<QDateTime> dates;
    if (cal) {
        dates = base().nextAvailableTime(QDateTime(d->ui->startDate->date(), QTime(0,0)), (index+1)*5, *cal, S_NUMBEROFAVAILABILITIESTOSHOW);
    }

    // Create a simple QStandardItemModel
    if (!d->m_AvailModel)
        d->m_AvailModel = new QStandardItemModel(this);
    d->m_AvailModel->clear();
    d->ui->availabilitiesView->setModel(d->m_AvailModel);

    QHash<QString, QStandardItem *> main;
    QFont bold;
    bold.setBold(true);
    for(int i = 0; i < dates.count(); ++i) {
        QDate dt = dates.at(i).date();
        QStandardItem *parent = main.value(dt.toString(Qt::ISODate), 0);
        if (!parent) {
            parent = new QStandardItem((QLocale().toString(dt, QLocale::LongFormat)));
            parent->setFont(bold);
            d->m_AvailModel->invisibleRootItem()->appendRow(parent);
            main.insert(dt.toString(Qt::ISODate), parent);
        }
        QStandardItem *item = new QStandardItem(dates.at(i).time().toString("HH:mm"));
        item->setData(dates.at(i));
        parent->appendRow(item);
    }
    d->ui->availabilitiesView->expandAll();


//    for(int i = 0; i < dates.count(); ++i) {
//        QAction *a = new QAction(d->ui->availButton);
//        a->setData(dates.at(i));
//        a->setText(dates.at(i).toString(QLocale().dateTimeFormat(QLocale::ShortFormat)));
//        a->setToolTip(a->text());
//        d->ui->availButton->addAction(a);
////        d->ui->nextAvailCombo->addItem(dates.at(i).toString(QLocale().dateTimeFormat(QLocale::ShortFormat)));
//    }
//    if (d->ui->availButton->actions().count() > 0) {
//        d->ui->availButton->setDefaultAction(d->ui->availButton->actions().at(0));
//    }
}

/** Clear the view. */
void UserCalendarViewer::clear()
{
    d->ui->calendarViewer->setModel(0);
    d->ui->availabilitiesView->setModel(0);
    d->ui->availDurationCombo->setCurrentIndex(-1);
    d->ui->defaultDurationButton->setToolTip("");
    d->ui->description->setHtml("");
}

void UserCalendarViewer::on_availableAgendasCombo_activated(const int index)
{
    clear();
    if (index >= 0 && index < d->m_UserCalendarModel->rowCount()) {
        QModelIndex calIndex = d->m_UserCalendarModel->index(index, UserCalendarModel::Uid);
        QVariant calUid = calIndex.data();
        if (calUid.isNull())
            return;
        if (!calUid.isValid())
            return;
        d->m_CalendarItemModel = agendaCore().calendarItemModel(calUid);
        d->ui->calendarViewer->setModel(d->m_CalendarItemModel);

        // Add availabilities
//        d->ui->availabilitiesView->setModel(d->m_UserCalendarModel->availabilityModel(calIndex, this));

        // Reset the Calendar View properties
        int defaultDuration = d->m_UserCalendarModel->index(index, UserCalendarModel::DefaultDuration).data().toInt();
        d->ui->calendarViewer->setDayScaleHourDivider(defaultDuration/60);
        d->ui->calendarViewer->setDayItemDefaultDuration(defaultDuration);

        //: default agenda duration time, %1 = time, %2 = minutes (fixed)
        d->ui->defaultDurationButton->setToolTip(tr("Set back to default: %1 %2").arg(
                                                     QString::number(defaultDuration),
                                                     tkTr(Trans::Constants::MINUTES)));
        resetDefaultDuration();
        d->ui->description->setHtml(d->m_UserCalendarModel->index(index, UserCalendarModel::Description).data().toString());
    }
//    d->populateCalendarWithCurrentWeek(d->m_UserCals.at(index));
}

void UserCalendarViewer::userChanged()
{
    // Update ui
    if (d->m_UserCalendarModel) {
        disconnect(d->m_UserCalendarModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(updateCalendarData(QModelIndex,QModelIndex)));
    }
    // model is automatically updated and resetted but the userCalendar combo model
    d->m_UserCalendarModel = agendaCore().userCalendarModel();
    connect(d->m_UserCalendarModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(updateCalendarData(QModelIndex,QModelIndex)));
    d->ui->availableAgendasCombo->setModel(d->m_UserCalendarModel);
    d->ui->availableAgendasCombo->setModelColumn(UserCalendarModel::ExtraLabel);
    QModelIndex calIndex = d->m_UserCalendarModel->defaultUserCalendarModelIndex();
    d->ui->availableAgendasCombo->setCurrentIndex(calIndex.row());

    // Add availabilities
    d->ui->availabilitiesView->setModel(d->m_UserCalendarModel->availabilityModel(calIndex, this));
    d->ui->availabilitiesView->expandAll();

    // Next available dates
    resetDefaultDuration();

    Agenda::UserCalendar *cal = d->m_UserCalendarModel->defaultUserCalendar();
    if (cal) {
        d->m_CalendarItemModel = agendaCore().calendarItemModel(cal->uid());
    } else {
        d->m_CalendarItemModel = 0;
    }
    d->ui->calendarViewer->setModel(d->m_CalendarItemModel);

    // Activate calendar
    on_availableAgendasCombo_activated(calIndex.row());

    // Manage menu action
    // Connect menu actions
    if (d->m_UserCalendarModel->rowCount() == 0) {
        Core::Command *cmd = actionManager()->command(Constants::A_NEW_AGENDAEVENT);
        cmd->action()->setEnabled(false);
    }

    // Scroll the view to now
    if (isVisible()) {
        d->ui->calendarViewer->scrollToTime(QTime::currentTime());
        d->scrollOnShow = false;
    } else {
        d->scrollOnShow = true;
    }
}

void UserCalendarViewer::updateCalendarData(const QModelIndex &top, const QModelIndex &bottom)
{
    Q_UNUSED(bottom);
    if (top.column()==UserCalendarModel::DefaultDuration) {
        // get default duration of given calendar
        int defaultDuration = d->m_UserCalendarModel->index(top.row(),
                                                            UserCalendarModel::DefaultDuration, top.parent()).data().toInt();
        d->ui->calendarViewer->setDayScaleHourDivider(defaultDuration/60);
        d->ui->calendarViewer->setDayItemDefaultDuration(defaultDuration);
        //: default agenda duration time (in minutes)
        d->ui->defaultDurationButton->setToolTip(tr("Set back to default: ") +
                                                 QString::number(defaultDuration) + " " + tkTr(Trans::Constants::MINUTES));
        if (defaultDuration % 5)
            defaultDuration = (defaultDuration/5);
        else
            defaultDuration = (defaultDuration/5 - 1);
        d->ui->availDurationCombo->setCurrentIndex(defaultDuration);
        recalculateAvailabilitiesWithDurationIndex(defaultDuration);
    }
}

/** Reacts to the user selected action in the contextual menu */
void UserCalendarViewer::onSwitchToPatientClicked()
{
    // Get the clicked calendar item
    Calendar::CalendarItem item = d->ui->calendarViewer->getContextualCalendarItem();

    // Find the first patient of the appointement
    QList<Calendar::People> people = d->m_CalendarItemModel->peopleList(item);
    foreach(const Calendar::People &guest, people) {
        if (guest.type == Calendar::People::PeopleAttendee) {
            if (!patientCore()->setCurrentPatientUuid(guest.uid)) {
                LOG_ERROR("Unable to set current patient");
            }
            break;
        }
    }
}

/** Reacts to the user selected action in the contextual menu */
void UserCalendarViewer::onEditAppointmentClicked()
{
    // Get the clicked calendar item
    Calendar::CalendarItem item = d->ui->calendarViewer->getContextualCalendarItem();

    // Starts the editor dialog
    Calendar::BasicItemEditorDialog dialog(d->m_CalendarItemModel, this);
    dialog.init(item);
    dialog.exec();
}

/** Reacts to the user selected action in the contextual menu */
void UserCalendarViewer::onPrintAppointmentClicked()
{
    // TODO
}

/** Reacts to the user selected action in the contextual menu */
void UserCalendarViewer::onDeleteAppointmentClicked()
{
    // Get the clicked calendar item
    Calendar::CalendarItem item = d->ui->calendarViewer->getContextualCalendarItem();
    d->m_CalendarItemModel->removeItem(item.uid());
}

bool UserCalendarViewer::event(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
    {
        d->ui->retranslateUi(this);
        // populate the availabilities duration selector combo (every five minutes)
        int idx = d->ui->availDurationCombo->currentIndex();
        d->ui->availDurationCombo->clear();
        for(int i = 1; i < 19; ++i) {
            d->ui->availDurationCombo->addItem(QString("%1 %2").arg(i*5).arg(tkTr(Trans::Constants::MINUTES)));
        }
        d->ui->availDurationCombo->setCurrentIndex(idx);
        d->aToday->setText(tkTr(Trans::Constants::TODAY));
        d->aToday->setToolTip(d->aToday->text());
        d->aTomorrow->setText(tkTr(Trans::Constants::TOMORROW));
        d->aTomorrow->setToolTip(d->aTomorrow->text());
        d->aNextWeek->setText(tr("Beginning of next week"));
        d->aNextWeek->setToolTip(d->aNextWeek->text());
        d->aNextMonth->setText(tr("Beginning of next month"));
        d->aNextMonth->setToolTip(d->aNextMonth->text());

        if (d->aSwitchToPatient)
            d->aSwitchToPatient->setText(tr("Switch to patient"));
        if (d->aEditItem)
            d->aEditItem->setText(tr("Edit appointment"));
        if (d->aPrintItem)
                d->aPrintItem->setText(tr("Print appointment"));
        if (d->aDeleteItem)
                d->aDeleteItem->setText(tr("Delete appointment"));

        break;
    }
    case QEvent::Show:
        if (d->scrollOnShow) {
            d->ui->calendarViewer->scrollToTime(QTime::currentTime());
            d->scrollOnShow = false;
        }
    default:
        break;
    }
    return QWidget::event(e);
}
