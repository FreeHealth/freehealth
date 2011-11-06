/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/

/**
  \class Agenda::UserCalendarViewer
  Is a Agenda::UserCalendar widget viewer for FreeMedForms used in the Agenda mode.
  This widget presents:
  - a left bar with some usercalendar informations, plus automated next availabilities calculation
  - a central Calendar::CalendarViewer for users to edit appointements
*/

#include "usercalendarviewer.h"
#include "agendabase.h"
#include "constants.h"
#include "calendaritemmodel.h"
#include "usercalendar.h"
#include "usercalendarmodel.h"
#include "agendacore.h"

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/actionmanager/actionmanager.h>

#include <calendar/basic_item_edition_dialog.h>

#include <translationutils/constanttranslations.h>

#include "ui_usercalendarviewer.h"

#include <QStandardItemModel>

using namespace Agenda;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::IUser *user() {return Core::ICore::instance()->user();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline Agenda::Internal::AgendaBase *base() {return Agenda::Internal::AgendaBase::instance();}
static inline Agenda::AgendaCore *agendaCore() {return Agenda::AgendaCore::instance();}
inline static Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}
inline static Core::ITheme *theme() {return Core::ICore::instance()->theme();}

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
        m_UserCalendarModel(agendaCore()->userCalendarModel(user()->uuid())),
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

    void populateCalendarWithCurrentWeek(Agenda::UserCalendar *calendar)
    {
        ui->calendarViewer->setModel(m_CalendarItemModel);
    }


public:
    Ui::UserCalendarViewer *ui;
    CalendarItemModel *m_CalendarItemModel;
    UserCalendarModel *m_UserCalendarModel;
    QHash<QString, int> m_UidToListIndex;
    bool scrollOnShow;

private:
    UserCalendarViewer *q;
};

}  // End namespace Internal
}  // End namespace Agenda


UserCalendarViewer::UserCalendarViewer(QWidget *parent) :
    QWidget(parent),
    d(new UserCalendarViewerPrivate(this))
{
    d->ui->setupUi(this);
    this->layout()->setMargin(0);
    d->ui->refreshAvailabilities->setIcon(theme()->icon(Core::Constants::ICONSOFTWAREUPDATEAVAILABLE));
    d->ui->calendarViewer->setDate(QDate::currentDate());
    d->ui->calendarViewer->setDayScaleHourDivider(2);
    d->ui->calendarViewer->setDayGranularity(5);
    d->ui->calendarViewer->setHourHeight(4*20); // 20pixels per minutes

    // populate the availabilities duration selector combo (every five minutes)
    for(int i = 1; i < 19; ++i) {
        d->ui->availDurationCombo->addItem(QString("%1 %2").arg(i*5).arg(tkTr(Trans::Constants::MINUTES)));
    }
    connect(d->ui->availDurationCombo, SIGNAL(activated(int)), this, SLOT(recalculateAvailabilitiesWithDurationIndex(int)));

    Agenda::UserCalendar *cal = d->m_UserCalendarModel->defaultUserCalendar();
    if (cal) {
        if (cal->isValid()) {
            d->ui->description->setHtml(cal->data(Agenda::UserCalendar::Description).toString());
            d->ui->durationLabel->setText(cal->data(Agenda::UserCalendar::DefaultDuration).toString());
        }
    }

    int width = size().width();
    int third = width/3;
    d->ui->splitter->setSizes(QList<int>() << third << width-third);

    connect(user(), SIGNAL(userChanged()), this, SLOT(userChanged()));
    connect(d->ui->availButton, SIGNAL(triggered(QAction*)), this, SLOT(newEventAtAvailabity(QAction*)));
    connect(d->ui->availableAgendasCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(on_availableAgendasCombo_activated(int)));
    connect(d->ui->refreshAvailabilities, SIGNAL(clicked()), this, SLOT(refreshAvailabilities()));
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
}

void UserCalendarViewer::newEvent()
{
    newEventAtAvailabity(0);
}

/** When the user select an availability in the toolButton, this slot is activated. Open a EventEditor dialog and save the item in the CalendarItemModel if the dialog is accepted. */
void UserCalendarViewer::newEventAtAvailabity(QAction *action)
{
    Calendar::BasicItemEditionDialog dlg(d->m_CalendarItemModel, this);
    QDateTime start = QDateTime::currentDateTime();
    if (!action) {
        if (d->ui->availButton->actions().count() > 0) {
            action = d->ui->availButton->actions().at(0);
        }
    }
    if (action) {
        start = action->data().toDateTime();
    }

    Calendar::CalendarItem item = d->m_CalendarItemModel->insertItem(start, start.addSecs((d->ui->availDurationCombo->currentIndex()+1)*5*60));
    dlg.init(item);
    if (dlg.exec() != QDialog::Accepted) {
        d->m_CalendarItemModel->removeItem(item.uid());
    }
    recalculateAvailabilitiesWithDurationIndex((d->ui->availDurationCombo->currentIndex()+1)*5);
}

void UserCalendarViewer::refreshAvailabilities()
{
    recalculateAvailabilitiesWithDurationIndex(d->ui->availDurationCombo->currentIndex());
}

void UserCalendarViewer::recalculateAvailabilitiesWithDurationIndex(const int index)
{
    /** \todo Create a tree model for the nextAvailableDates */

    for(int i = d->ui->availButton->actions().count()-1; i > -1 ; --i)
        d->ui->availButton->removeAction(d->ui->availButton->actions().at(i));

    if (index<0)
        return;

    QList<QDateTime> dates;
    Agenda::UserCalendar *cal = d->m_UserCalendarModel->defaultUserCalendar();
    if (cal) {
        // Next available dates
        dates = base()->nextAvailableTime(QDateTime::currentDateTime(), (index+1)*5, *cal, S_NUMBEROFAVAILABILITIESTOSHOW);
    }
//    d->ui->nextAvailCombo->clear();
    for(int i = 0; i < dates.count(); ++i) {
        QAction *a = new QAction(d->ui->availButton);
        a->setData(dates.at(i));
        a->setText(dates.at(i).toString(QLocale().dateTimeFormat(QLocale::ShortFormat)));
        a->setToolTip(a->text());
        d->ui->availButton->addAction(a);
//        d->ui->nextAvailCombo->addItem(dates.at(i).toString(QLocale().dateTimeFormat(QLocale::ShortFormat)));
    }
    if (d->ui->availButton->actions().count() > 0) {
        d->ui->availButton->setDefaultAction(d->ui->availButton->actions().at(0));
    }
}

void UserCalendarViewer::on_availableAgendasCombo_activated(const int index)
{
    if (index >= 0 && index < d->m_UserCalendarModel->rowCount()) {
        QModelIndex calIndex = d->m_UserCalendarModel->index(index, UserCalendarModel::Uid);
        QVariant calUid = calIndex.data();
        if (calUid.isNull())
            return;
        if (!calUid.isValid())
            return;
        d->m_CalendarItemModel = agendaCore()->calendarItemModel(calUid);
        d->ui->calendarViewer->setModel(d->m_CalendarItemModel);

        // Add availabilities
        d->ui->availabilitiesView->setModel(d->m_UserCalendarModel->availabilityModel(calIndex, this));
        d->ui->availabilitiesView->expandAll();

        // Reset the Calendar View properties
        int defaultDuration = d->m_UserCalendarModel->index(index, UserCalendarModel::DefaultDuration).data().toInt();
        d->ui->calendarViewer->setDayScaleHourDivider(defaultDuration/60);
        d->ui->calendarViewer->setDayItemDefaultDuration(defaultDuration);

        d->ui->durationLabel->setText(QString::number(defaultDuration) + " " + tkTr(Trans::Constants::MINUTES));
        d->ui->description->setHtml(d->m_UserCalendarModel->index(index, UserCalendarModel::Description).data().toString());
    }
//    d->populateCalendarWithCurrentWeek(d->m_UserCals.at(index));
}

void UserCalendarViewer::userChanged()
{
    // Update ui
    d->ui->userNameLabel->setText(user()->value(Core::IUser::FullName).toString());
    // model is automatically updated and reseted but the userCalendar combo model
    d->m_UserCalendarModel = agendaCore()->userCalendarModel();
    d->ui->availableAgendasCombo->setModel(d->m_UserCalendarModel);
    d->ui->availableAgendasCombo->setModelColumn(UserCalendarModel::ExtraLabel);
    QModelIndex calIndex = d->m_UserCalendarModel->defaultUserCalendarModelIndex();
    d->ui->availableAgendasCombo->setCurrentIndex(calIndex.row());

    // Add availabilities
    d->ui->availabilitiesView->setModel(d->m_UserCalendarModel->availabilityModel(calIndex, this));
    d->ui->availabilitiesView->expandAll();

    // Next available dates
    Agenda::UserCalendar *cal = d->m_UserCalendarModel->defaultUserCalendar();
    int duration = 5;
    if (cal) {
        duration = cal->data(Agenda::UserCalendar::DefaultDuration).toInt();
    }
    if (duration%5)
        duration = (duration/5);
    else
        duration = (duration/5 - 1);
    d->ui->availDurationCombo->setCurrentIndex(duration);
    recalculateAvailabilitiesWithDurationIndex(duration);

    if (cal) {
        d->m_CalendarItemModel = agendaCore()->calendarItemModel(cal->uid());
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
        d->ui->calendarViewer->scrollToTime(QTime(17,0,0));
        d->scrollOnShow = false;
    } else {
        d->scrollOnShow = true;
    }
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
