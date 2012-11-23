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
 ***************************************************************************/
#include "agendaplugin.h"
#include "agendabase.h"
#include "agendacore.h"
#include "constants.h"
#include "agendawidgetmanager.h"
#include "agendacore.h"

// TEST
#include "appointment.h"
#include "calendaritemmodel.h"
#include "eventeditorwidget.h"
#include <utils/randomizer.h>
#include <utils/log.h>
#include <QDir>
#include <QFileInfo>
#include <QProgressDialog>
//#include <calendar/usercalendar.h>
#include <agendaplugin/usercalendar.h>
#include <calendar/common.h>
//#include <calendar/usercalendar_editor_widget.h>
#include <patientbaseplugin/patientbase.h>
#include <patientbaseplugin/constants_db.h>
// END TEST

#include <utils/log.h>
#include <calendar/calendar_theme.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/translators.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/icommandline.h>

#include <QtCore/QtPlugin>
#include <QDialog>
#include <QGridLayout>
#include <QProgressDialog>

#include <QDebug>

using namespace Agenda;
using namespace Internal;

// TEST
static inline Patients::Internal::PatientBase *patientBase() {return Patients::Internal::PatientBase::instance();}
// END TEST

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::ICommandLine *commandLine() {return Core::ICore::instance()->commandLine();}
static inline Agenda::Internal::AgendaBase &base() {return Agenda::AgendaCore::instance().agendaBase();}

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline void messageSplash(const QString &s) {theme()->messageSplashScreen(s); }

AgendaPlugin::AgendaPlugin() :
    m_Core(0)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating AgendaPlugin";
    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("agendaplugin");
    Core::ICore::instance()->translators()->addNewTranslator("calendar");

    // Create the core object
    m_Core = new AgendaCore(this);
}

AgendaPlugin::~AgendaPlugin()
{
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;
}

bool AgendaPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "AgendaPlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

    return true;
}

void AgendaPlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "AgendaPlugin::extensionsInitialized";

    messageSplash(tr("Initializing agenda plugin..."));

    m_Core->extensionsInitialized();

    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));

    // Initialize database
    QProgressDialog dlg(tr("Creating agenda base..."), tr("Please wait"), 0, 0);
    dlg.setWindowModality(Qt::WindowModal);
    dlg.setMinimumDuration(1000);
    dlg.show();
    dlg.setFocus();
    dlg.setValue(0);

    m_Core->initializeDatabase();

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

    if (commandLine()->value(Core::ICommandLine::CreateVirtuals).toBool()) {
        QList<Calendar::People> peoples;
        UserCalendar *u = 0;
        // McCoy calendar (Uhura as delegate)
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        peoples.append(Calendar::People(Calendar::People::PeopleUserDelegate, "", "0f148ea3de6e47b8bbf9c2cedea47511"));
        u = base().createVirtualUserCalendar("d1f29ad4a4ea4dabbe40ec888d153228", "McCoy @Enterprise",
                                          "Virtual calendar for the virtual user McCoy",
                                          15, 0, 0, 0, true, false, "", "mccoy.png", peoples);
        if (!u)
            return;
        createVirtualAppointments(u);

        // Phlox calendar (no delegates)
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        peoples.clear();
        u = base().createVirtualUserCalendar("b5caead635a246a2a87ce676e9d2ef4d", "Phlox @Enterprise",
                                          "Virtual calendar for the virtual user Phlox",
                                          10, 0, 0, 0, true, false, "", "phlox.png", peoples);
        if (u)
            createVirtualAppointments(u);
    }
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

//static QString patientName(const int row)
//{
//    QSqlQuery query(patientbase().database());
//    QString req= patientbase().select(Patients::Constants::Table_IDENT, Patients::Constants::IDENTITY_NAME);
//    req += QString(" LIMIT %1,%1").arg(row);
//    if (query.exec(req)) {
//        if (query.next()) {
//            return query.value(0).toString();
//        }
//    } else {
//        LOG_QUERY_ERROR_FOR("AgendaPlugin", query);
//        return QString();
//    }
//    return QString();
//}

void AgendaPlugin::createVirtualAppointments(UserCalendar *calendar)
{
//    QTime chr;
//    chr.start();

    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    QList<Appointment *> list;
    Utils::Randomizer r;
    r.setPathToFiles(settings()->path(Core::ISettings::BundleResourcesPath) + "/textfiles/");
    QDir pix(settings()->path(Core::ISettings::SmallPixmapPath));
    int maxStatus = Calendar::availableStatus().count() - 1;
    bool ok = true;
    int nbEvents = r.randomInt(50, 100);
    QDateTime start = QDateTime::currentDateTime();
    int maxDb = numberOfPatients();
    int defaultDuration = calendar->data(UserCalendar::DefaultDuration).toInt();
    int calendarId = calendar->data(Constants::Db_CalId).toInt();
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    for(int i = 0; i < nbEvents; ++i) {
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        Appointment *ev = 0;

        // add a duration to last start date
        start.setTime(start.addSecs(60*defaultDuration*r.randomInt(0, 5)).time());
        if (start.time().hour() >= 18) {
            start.setDate(start.addDays(1).date());
            start.setTime(QTime(8,0,0));
        } else {
        }
        QDateTime end = start.addSecs(60*defaultDuration);

        ev = new Appointment;
        ev->setData(Constants::Db_CalId, calendarId);
        ev->setData(Constants::Db_IsValid, 1);
        ev->setData(Constants::Db_IsVirtual, 1);
        ev->setData(Constants::Db_EvId, -1);
        ev->setData(Constants::Db_IsVirtual, 1);
        ev->setData(Constants::Db_XmlViewOptions, "XmlViewOptions");
        ev->setData(Constants::Db_XmlOptions, "XmlOptions");
        //    ev->setData(Constants::DbOnly_ComId, );
        ev->setData(Constants::Db_CatId, -1);
        //            ev->setData(Calendar::CalendarItem::PatientUid, r.getRandomString(45));
        ev->setData(CalendarItemModel::DateStart, start);
        ev->setData(CalendarItemModel::DateEnd, end);
        ev->setData(CalendarItemModel::Type, 1);
        ev->setData(CalendarItemModel::Status, r.randomInt(0, maxStatus));
        ev->setData(CalendarItemModel::LocationUid, "siteId");
        ev->setData(CalendarItemModel::IsPrivate, r.randomInt(0,1));
        ev->setData(CalendarItemModel::Password, "nopass");
        ev->setData(CalendarItemModel::IsBusy, r.randomInt(0,1));
        ev->setData(CalendarItemModel::IsAGroupEvent, r.randomInt(0,1));
        ev->setData(CalendarItemModel::Label, r.randomWords(r.randomInt(2, 15)));
        ev->setData(CalendarItemModel::Description, r.randomWords(r.randomInt(10, 500)));
        ev->setData(CalendarItemModel::Location, r.getRandomString(r.randomInt(1,145)));
        ev->setData(CalendarItemModel::IconPath, r.randomFile(pix, QStringList() << "*.png").fileName());

        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        // Add 1 to 3 patients
        for(int y = 0; y < r.randomInt(1, 3); ++y) {
            int zz = r.randomInt(0, maxDb);
            ev->addPeople(Calendar::People(Calendar::People::PeopleAttendee, "", patientUid(zz)));
        }
        list << ev;
    }
    if (!base().saveCalendarEvents(list))
        ok = false;

//    Utils::Log::logTimeElapsed(chr, "---", "creating virtual events");
    if (ok)
        qWarning() << nbEvents << "events successfully created and saved";
}

void AgendaPlugin::testDatabase()
{
    qWarning() << "\n\n\nxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx AGENDA BASE TEST";
    // Try to get calendar(s) for the current user
    QList<Agenda::UserCalendar *> cals = base().getUserCalendars();
    qWarning() << "Number of user calendars" << cals.count();

    Utils::Randomizer r;
    r.setPathToFiles(settings()->path(Core::ISettings::BundleResourcesPath) + "/textfiles/");
    QDir pix(settings()->path(Core::ISettings::SmallPixmapPath));

    Agenda::UserCalendar *ucal = 0;
    if (cals.count() == 0) {
        for(int i=0; i < 5; ++i) {
            ucal = new Agenda::UserCalendar();
            // Create a calendar for the current user
            ucal->setData(Constants::Db_IsValid, 1);
            ucal->setData(Agenda::UserCalendar::Password, r.getRandomString(r.randomInt(0,10)));
            ucal->setData(Agenda::UserCalendar::Label, r.randomWords(r.randomInt(2,5)));
            ucal->setData(Agenda::UserCalendar::IsPrivate, r.randomInt(0,1));
            ucal->setData(Agenda::UserCalendar::IsDefault, 0);
            ucal->setData(Agenda::UserCalendar::DefaultDuration, r.randomInt(10,120));
            ucal->setData(Agenda::UserCalendar::Description, r.randomWords(r.randomInt(5,50)));
            ucal->setData(Agenda::UserCalendar::AbsPathIcon, r.randomFile(pix, QStringList() << "*.png").fileName());
            // Create availabilities
            for(int i = 1; i < 6; ++i) {
                QTime from = r.randomTime(6, 12);
                QTime to = r.randomTime(17, 20);
                Agenda::DayAvailability av;
                av.setWeekDay(i);
                av.addTimeRange(from, to);
                ucal->addAvailabilities(av);
            }
            if (base().saveUserCalendar(ucal))
                qWarning() << "user calendar successfully saved to database";
            cals << ucal;
        }
        // one must be the default
        ucal = cals.at(2);
        ucal->setData(Agenda::UserCalendar::IsDefault, 1);
        base().saveUserCalendar(ucal);
    }
    ucal = cals.at(0);

//    qWarning() << ucal->data(Constants::Db_CalId) << ucal->availabilities().count();

    // Test UserCalendar Widget Editor
//    QDialog dlg;
//    QGridLayout lay(&dlg);
//    Agenda::UserCalendarEditorWidget w(&dlg);
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
    QList<Appointment *> list = base().getCalendarEvents(q);
    qWarning() << "Retreived" << list.count() << "events from the database for user" << ucal->data(Agenda::UserCalendar::UserOwnerUid).toString() << "dateRange" << q.dateStart().toString(Qt::ISODate)<< q.dateEnd().toString(Qt::ISODate) << "in" << chrono.elapsed() << "ms";

    qWarning() << "PatientBase count" << numberOfPatients();

    Appointment *ev = 0;
    int maxStatus = Calendar::availableStatus().count() - 1;
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

            ev = new Appointment;
            ev->setData(Constants::Db_CalId, ucal->data(Constants::Db_CalId));
            ev->setData(Constants::Db_IsValid, 1);
            ev->setData(Constants::Db_EvId, -1);
            ev->setData(Constants::Db_IsVirtual, 1);
            ev->setData(Constants::Db_XmlViewOptions, "XmlViewOptions");
            ev->setData(Constants::Db_XmlOptions, "XmlOptions");
            //    ev->setData(Constants::DbOnly_ComId, );
            ev->setData(Constants::Db_CatId, -1);
//            ev->setData(Calendar::CalendarItem::PatientUid, r.getRandomString(45));
            ev->setData(CalendarItemModel::DateStart, start);
            ev->setData(CalendarItemModel::DateEnd, end);
            ev->setData(CalendarItemModel::Type, 1);
            ev->setData(CalendarItemModel::Status, r.randomInt(0, maxStatus));
            ev->setData(CalendarItemModel::LocationUid, "siteId");
            ev->setData(CalendarItemModel::IsPrivate, r.randomInt(0,1));
            ev->setData(CalendarItemModel::Password, "nopass");
            ev->setData(CalendarItemModel::IsBusy, r.randomInt(0,1));
            ev->setData(CalendarItemModel::IsAGroupEvent, r.randomInt(0,1));
            ev->setData(CalendarItemModel::Label, r.randomWords(r.randomInt(2, 15)));
            ev->setData(CalendarItemModel::Description, r.randomWords(r.randomInt(10, 500)));
            ev->setData(CalendarItemModel::Location, r.getRandomString(r.randomInt(1,145)));
            ev->setData(CalendarItemModel::IconPath, r.randomFile(pix, QStringList() << "*.png").fileName());

            // Add 1 to 3 patients
            for(int y = 0; y < r.randomInt(1, 3); ++y) {
                int zz = r.randomInt(0, maxDb);
                ev->addPeople(Calendar::People(Calendar::People::PeopleAttendee, "", patientUid(zz)));
            }

            if (!base().saveCalendarEvent(ev))
                ok = false;
            list << ev;
        }
        if (ok)
            qWarning() << nbEvents << "events successfully created and saved in" << chrono.elapsed() << "ms";
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
    qWarning() << "Next available dates (15min)" << base().nextAvailableTime(QDateTime::currentDateTime(), 15, *ucal, 5);

    qDeleteAll(list);
    list.clear();

    qWarning() << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx AGENDA BASE TEST END\n\n\n";
}

ExtensionSystem::IPlugin::ShutdownFlag AgendaPlugin::aboutToShutdown()
{
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    // Remove preferences pages to plugins manager object pool
    if (m_Core)
        delete m_Core;
    m_Core = 0;
    return SynchronousShutdown;
}

Q_EXPORT_PLUGIN(AgendaPlugin)
