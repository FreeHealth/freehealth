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
#include "agendaplugin.h"
#include "agendabase.h"
#include "constants.h"
#include "agendawidgetmanager.h"
#include "agendamode.h"
#include "calendaritemeditorpatientmapper.h"

// TEST
#include "eventeditorwidget.h"
#include <utils/randomizer.h>
#include <utils/log.h>
#include <QDir>
#include <QFileInfo>
#include <QProgressDialog>
#include <calendar/calendar_item.h>
#include <calendar/usercalendar.h>
#include <calendar/usercalendar_editor_widget.h>
#include <patientbaseplugin/patientbase.h>
#include <patientbaseplugin/constants_db.h>
// END TEST

#include <utils/log.h>
#include <calendar/calendar_theme.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/translators.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/dialogs/pluginaboutpage.h>

#include <QtCore/QtPlugin>
#include <QDialog>
#include <QGridLayout>
#include <QDebug>

using namespace Agenda;
using namespace Internal;

// TEST
static inline Patients::Internal::PatientBase *patientBase() {return Patients::Internal::PatientBase::instance();}
// END TEST

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}

AgendaPlugin::AgendaPlugin()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating AgendaPlugin";
}

AgendaPlugin::~AgendaPlugin()
{
}

bool AgendaPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "AgendaPlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("AgendaPlugin");

    return true;
}

void AgendaPlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "AgendaPlugin::extensionsInitialized";

    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));

    // Initialize database
    Internal::AgendaBase::instance();

    // Initialize ActionHandler and WidgetManager
    AgendaWidgetManager::instance();

//    testDatabase();

    // Inform Calendar lib of theme paths and icons
    Calendar::CalendarTheme *th = Calendar::CalendarTheme::instance();
    th->setPath(Calendar::CalendarTheme::SmallIconPath, settings()->path(Core::ISettings::SmallPixmapPath));
    th->setPath(Calendar::CalendarTheme::MediumIconPath, settings()->path(Core::ISettings::MediumPixmapPath));
    th->setPath(Calendar::CalendarTheme::BigIconPath, settings()->path(Core::ISettings::BigPixmapPath));
    th->setIconFileName(Calendar::CalendarTheme::NavigationBookmarks, Core::Constants::ICONAGENDA);
    th->setIconFileName(Calendar::CalendarTheme::NavigationCurrentDateView, Core::Constants::ICONEYES);
    th->setIconFileName(Calendar::CalendarTheme::NavigationViewMode, Constants::I_VIEWMODE);
    th->setIconFileName(Calendar::CalendarTheme::NavigationDayViewMode, Constants::I_VIEWMODE);
    th->setIconFileName(Calendar::CalendarTheme::NavigationWeekViewMode, Constants::I_VIEWMODE);
    th->setIconFileName(Calendar::CalendarTheme::NavigationMonthViewMode, Constants::I_VIEWMODE);
    th->setIconFileName(Calendar::CalendarTheme::NavigationNext, Core::Constants::ICONNEXT);
    th->setIconFileName(Calendar::CalendarTheme::NavigationPrevious, Core::Constants::ICONPREVIOUS);

    // Add Agenda's mode
    addAutoReleasedObject(new AgendaMode(this));

    // Add Agenda's Calendar::CalendarItem extended editing widgets
    addAutoReleasedObject(new CalendarItemEditorPatientMapper(this));
}

static int numberOfPatients()
{
    return patientBase()->count(Patients::Constants::Table_IDENT, Patients::Constants::IDENTITY_ID);
}

static QString patientUid(const int row)
{
    QSqlQuery query(patientBase()->database());
    QString req= patientBase()->select(Patients::Constants::Table_IDENT, Patients::Constants::IDENTITY_UID);
    req += QString(" LIMIT %1,%1").arg(row);
    if (query.exec(req)) {
        if (query.next()) {
            return query.value(0).toString();
        }
    } else {
        LOG_QUERY_ERROR_FOR("AgendaPlugin", query);
        return QString();
    }
    return QString();
}

static QString patientName(const int row)
{
    QSqlQuery query(patientBase()->database());
    QString req= patientBase()->select(Patients::Constants::Table_IDENT, Patients::Constants::IDENTITY_NAME);
    req += QString(" LIMIT %1,%1").arg(row);
    if (query.exec(req)) {
        if (query.next()) {
            return query.value(0).toString();
        }
    } else {
        LOG_QUERY_ERROR_FOR("AgendaPlugin", query);
        return QString();
    }
    return QString();
}

void AgendaPlugin::testDatabase()
{
    qWarning() << "\n\n\nxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx AGENDA BASE TEST";
    Internal::AgendaBase *base = Internal::AgendaBase::instance();

    // Try to get calendar(s) for the current user
    QList<Calendar::UserCalendar *> cals = base->getUserCalendars();
    qWarning() << "Number of user calendars" << cals.count();

    Utils::Randomizer r;
    r.setPathToFiles(settings()->path(Core::ISettings::BundleResourcesPath) + "/textfiles/");
    QDir pix(settings()->path(Core::ISettings::SmallPixmapPath));

    Calendar::UserCalendar *ucal = 0;
    if (cals.count() == 0) {
        for(int i=0; i < 5; ++i) {
            ucal = new Calendar::UserCalendar();
            // Create a calendar for the current user
            ucal->setData(Constants::Db_IsValid, 1);
            ucal->setData(Calendar::UserCalendar::Password, r.getRandomString(r.randomInt(0,10)));
            ucal->setData(Calendar::UserCalendar::Label, r.randomWords(r.randomInt(2,5)));
            ucal->setData(Calendar::UserCalendar::IsPrivate, r.randomInt(0,1));
            ucal->setData(Calendar::UserCalendar::IsDefault, 0);
            ucal->setData(Calendar::UserCalendar::DefaultDuration, r.randomInt(10,120));
            ucal->setData(Calendar::UserCalendar::Description, r.randomWords(r.randomInt(5,50)));
            ucal->setData(Calendar::UserCalendar::AbsPathIcon, r.randomFile(pix, QStringList() << "*.png").fileName());
            // Create availabilities
            for(int i = 1; i < 6; ++i) {
                QTime from = r.randomTime(6, 12);
                QTime to = r.randomTime(17, 20);
                Calendar::DayAvailability av;
                av.setWeekDay(i);
                av.addTimeRange(from, to);
                ucal->addAvailabilities(av);
            }
            if (base->saveUserCalendar(ucal))
                qWarning() << "user calendar correctly saved to database";
            cals << ucal;
        }
        // one must be the default
        ucal = cals.at(2);
        ucal->setData(Calendar::UserCalendar::IsDefault, 1);
        base->saveUserCalendar(ucal);
    }
    ucal = cals.at(0);

//    qWarning() << ucal->data(Constants::Db_CalId) << ucal->availabilities().count();

    // Test UserCalendar Widget Editor
//    QDialog dlg;
//    QGridLayout lay(&dlg);
//    Calendar::UserCalendarEditorWidget w(&dlg);
//    w.setUserCalendar(*ucal);
//    lay.addWidget(&w);
//    dlg.setLayout(&lay);
//    dlg.exec();

    // Create events in the calendar
    // Try to get events now
    QTime chrono;
    chrono.start();
    CalendarEventQuery q;
    q.setDateRangeForCurrentWeek();
    q.setCalendarId(ucal->data(Constants::Db_CalId).toInt());
    QList<Calendar::CalendarItem *> list = base->getCalendarEvents(q);
    qWarning() << "Retreived" << list.count() << "events from the database for user" << ucal->data(Calendar::UserCalendar::UserOwnerUid).toString() << "dateRange" << q.dateStart().toString(Qt::ISODate)<< q.dateEnd().toString(Qt::ISODate) << "in" << chrono.elapsed() << "ms";

    qWarning() << "PatientBase count" << numberOfPatients() << "Uid of patient 5" << patientUid(5);

    Calendar::CalendarItem *ev = 0;
    if (list.count()==0) {
        chrono.restart();
        bool ok = true;
        int nbEvents = r.randomInt(1000, 2000);
        QDateTime start = QDateTime::currentDateTime();

        QProgressDialog prog;
        prog.setLabelText(QString("Creating %1 events (1 month ~= 800 events").arg(nbEvents));
        prog.setRange(0, nbEvents);
        prog.show();
        int maxDb = numberOfPatients();

        for(int i = 0; i < nbEvents; ++i) {
            if ((i % 100)==0) {
                prog.setValue(i);
                qApp->processEvents();
            }

            if (start.time().hour() >= 18) {
                start.setDate(start.addDays(1).date());
                start.setTime(QTime(8,0,0));
            } else {
                start.setTime(start.addSecs(60*15).time());
            }
            QDateTime end = start.addSecs(60*15);
            ucal = cals.at(r.randomInt(0, cals.count()-1));

            Calendar::CalendarItem *ev = new Calendar::CalendarItem;
            ev->setData(Constants::Db_CalId, ucal->data(Constants::Db_CalId));
            ev->setData(Constants::Db_IsValid, 1);
            ev->setData(Constants::Db_EvId, -1);
            ev->setData(Constants::Db_XmlViewOptions, "XmlViewOptions");
            ev->setData(Constants::Db_XmlOptions, "XmlOptions");
            //    ev->setData(Constants::DbOnly_ComId, );
            ev->setData(Constants::Db_CatId, -1);
//            ev->setData(Calendar::CalendarItem::PatientUid, r.getRandomString(45));
            ev->setData(Calendar::CalendarItem::DateStart, start);
            ev->setData(Calendar::CalendarItem::DateEnd, end);
            ev->setData(Calendar::CalendarItem::Type, 1);
            ev->setData(Calendar::CalendarItem::Status, 2);
            ev->setData(Calendar::CalendarItem::LocationUid, "siteId");
            ev->setData(Calendar::CalendarItem::IsPrivate, r.randomInt(0,1));
            ev->setData(Calendar::CalendarItem::Password, "nopass");
            ev->setData(Calendar::CalendarItem::IsBusy, r.randomInt(0,1));
            ev->setData(Calendar::CalendarItem::IsAGroupEvent, r.randomInt(0,1));
            ev->setData(Calendar::CalendarItem::Label, r.randomWords(r.randomInt(2, 15)));
            ev->setData(Calendar::CalendarItem::Description, r.randomWords(r.randomInt(10, 500)));
            ev->setData(Calendar::CalendarItem::Location, r.getRandomString(r.randomInt(1,145)));
            ev->setData(Calendar::CalendarItem::IconPath, r.randomFile(pix, QStringList() << "*.png").fileName());

            // Add 1 to 3 patients
            for(int y = 0; y < r.randomInt(1, 3); ++y) {
                int zz = r.randomInt(0, maxDb);
                ev->addPeople(Calendar::CalendarItem::PeopleAttendee, patientName(zz), patientUid(zz));
            }

            if (!base->saveCalendarEvent(ev))
                ok = false;
            list << ev;
        }
        if (ok)
            qWarning() << nbEvents << "events correctly created and saved in" << chrono.elapsed() << "ms";
    }
    ev = list.at(0);

    // Test event dialog
//    QDialog dlg;
//    QGridLayout lay(&dlg);
//    EventEditorWidget w(&dlg);
//    w.setCalendarEvent(ev);
//    w.setAvailableUserCalendar(cals);
//    lay.addWidget(&w);
//    dlg.setLayout(&lay);
//    dlg.exec();

    // Test nextAvailableDate
    qWarning() << "Next available dates (15min)" << base->nextAvailableTime(QDateTime::currentDateTime(), 15, *ucal, 5);

    qDeleteAll(list);
    list.clear();

    qWarning() << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx AGENDA BASE TEST END\n\n\n";
}

Q_EXPORT_PLUGIN(AgendaPlugin)
