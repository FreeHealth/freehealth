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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/

/**
  \class Agenda::UserAgendasViewer
  \brief Calendar::UserCalendar widget viewer for FreeMedForms.
  This widget presents:
  - a left bar with some usercalendar informations, plus automated next availabilities calculation
  - a central Calendar::CalendarViewer
*/

#include "useragendasviewer.h"
#include "agendabase.h"
#include "constants.h"
#include "calendaritemmodel.h"

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/ipatient.h>

#include <calendar/usercalendar.h>
#include <calendar/basic_item_edition_dialog.h>

#include <translationutils/constanttranslations.h>

#include "ui_useragendasviewer.h"

#include <QStandardItemModel>

using namespace Agenda;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::IUser *user() {return Core::ICore::instance()->user();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline Agenda::Internal::AgendaBase *base() {return Agenda::Internal::AgendaBase::instance();}

namespace Agenda {
namespace Internal {
class UserAgendasViewerPrivate
{
public:
    UserAgendasViewerPrivate(UserAgendasViewer *parent) :
            ui(new Ui::UserAgendasViewer),
            m_Model(new Agenda::CalendarItemModel(parent)),
            q(parent)
    {
    }

    ~UserAgendasViewerPrivate()
    {
        delete ui;
        if (m_Model) {
            delete m_Model;
            m_Model = 0;
        }
    }

    void populateCalendarWithCurrentWeek(Calendar::UserCalendar *calendar)
    {
        ui->calendarViewer->setModel(m_Model);
    }


public:
    Ui::UserAgendasViewer *ui;
    CalendarItemModel *m_Model;
    QHash<QString, int> m_UidToListIndex;

private:
    UserAgendasViewer *q;
};

}  // End namespace Internal
}  // End namespace Agenda


UserAgendasViewer::UserAgendasViewer(QWidget *parent) :
    QWidget(parent),
    d(new UserAgendasViewerPrivate(this))
{
    d->ui->setupUi(this);
    d->ui->calendarViewer->setDate(QDate::currentDate().addDays(3));
    d->ui->calendarViewer->setModel(d->m_Model);

    Calendar::UserCalendar cal = d->m_Model->defaultUserCalendar();
    // populate the availabilities duration selector combo (every five minutes)
    for(int i = 1; i < 19; ++i) {
        d->ui->availDurationCombo->addItem(QString("%1 %2").arg(i*5).arg(tkTr(Trans::Constants::MINUTES)));
    }
    connect(d->ui->availDurationCombo, SIGNAL(activated(int)), this, SLOT(recalculateAvailabilitiesWithDurationIndex(int)));

    if (cal.isValid()) {
        d->ui->description->setHtml(cal.data(Calendar::UserCalendar::Description).toString());
        d->ui->durationLabel->setText(cal.data(Calendar::UserCalendar::DefaultDuration).toString());
    }

    int width = size().width();
    int third = width/3;
    d->ui->splitter->setSizes(QList<int>() << third << width-third);

    connect(user(), SIGNAL(userChanged()), this, SLOT(userChanged()));
    connect(d->ui->availButton, SIGNAL(triggered(QAction*)), this, SLOT(newEventAtAvailabity(QAction*)));
    userChanged();
}

UserAgendasViewer::~UserAgendasViewer()
{
    if (d) {
        delete d;
        d = 0;
    }
}

/** When the user select an availability in the toolButton, this slot is activated. Open a EventEditor dialog and save the item in the CalendarItemModel if the dialog is accepted. */
void UserAgendasViewer::newEventAtAvailabity(QAction *action)
{
    Calendar::BasicItemEditionDialog dlg(d->m_Model, this);
    Calendar::CalendarItem item;
    QDateTime start = action->data().toDateTime();
    item.setBeginning(start);
    item.setEnding(start.addSecs((d->ui->availDurationCombo->currentIndex()+1)*5*60));
    dlg.init(item);
    if (dlg.exec() == QDialog::Accepted) {
        d->m_Model->addCalendarItem(dlg.item());
    }
}

void UserAgendasViewer::recalculateAvailabilitiesWithDurationIndex(const int index)
{
    /** \todo Create a tree model for the nextAvailableDates */

    for(int i = 1; i < d->ui->availButton->actions().count(); ++i)
        d->ui->availButton->removeAction(d->ui->availButton->actions().at(i));

    Calendar::UserCalendar cal = d->m_Model->defaultUserCalendar();
    // Next available dates
    QList<QDateTime> dates = base()->nextAvailableTime(QDateTime::currentDateTime(), index*5, cal, 5);
    d->ui->nextAvailCombo->clear();
    for(int i = 0; i < dates.count(); ++i) {
        QAction *a = new QAction(d->ui->availButton);
        a->setData(dates.at(i));
        a->setText(dates.at(i).toString(QLocale().dateTimeFormat(QLocale::ShortFormat)));
        a->setToolTip(a->text());
        d->ui->availButton->addAction(a);
        d->ui->nextAvailCombo->addItem(dates.at(i).toString(QLocale().dateTimeFormat(QLocale::ShortFormat)));
    }
}

void UserAgendasViewer::on_availableAgendasCombo_activated(const int index)
{
//    d->populateCalendarWithCurrentWeek(d->m_UserCals.at(index));
}

void UserAgendasViewer::userChanged()
{
    Calendar::UserCalendar cal = d->m_Model->defaultUserCalendar();

    d->ui->userNameLabel->setText(user()->value(Core::IUser::FullName).toString());
    // model is automatically updated and reseted but the userCalendar combo model
    d->ui->availableAgendasCombo->setModel(d->m_Model->userCalendarComboModel(this));
    d->ui->availableAgendasCombo->setCurrentIndex(d->m_Model->defaultUserCalendarComboModelIndex());

    // Add availabilities
    d->ui->availabilitiesView->setModel(cal.availabilitiesModel(this));
    d->ui->availabilitiesView->expandAll();

    // Next available dates
    int duration = cal.data(Calendar::UserCalendar::DefaultDuration).toInt();
    if (duration%5)
        duration = (duration/5);
    else
        duration = (duration/5 - 1);
    d->ui->availDurationCombo->setCurrentIndex(duration);
    recalculateAvailabilitiesWithDurationIndex(duration);
}

void UserAgendasViewer::changeEvent(QEvent *e)
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
