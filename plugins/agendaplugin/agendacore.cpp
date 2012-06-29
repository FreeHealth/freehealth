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
/**
  \class Agenda::AgendaCore
  Core manager of the Agenda plugin.
*/
#include "agendacore.h"
#include "agendabase.h"
#include "usercalendarmodel.h"
#include "usercalendarpageforuserviewer.h"
#include "usercalendarwizardcreatorpage.h"
#include "calendaritemeditorpatientmapper.h"
#include "agendamode.h"
#include "calendaritemmodel.h"

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>

#include <utils/log.h>
#include <extensionsystem/pluginmanager.h>

#include <QHash>

#include <QDebug>

using namespace Agenda;
using namespace Internal;

static inline Core::IUser *user() {return Core::ICore::instance()->user();}
static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}

namespace Agenda {
namespace Internal {
class AgendaCorePrivate
{
public:
    AgendaCorePrivate() :
        m_Initialized(false),
        m_UserViewerPage(0),
        m_UserCreatorPage(0),
        m_CalItemMapper(0),
        m_AgendaMode(0),
        m_AgendaBase(0)
    {}

    ~AgendaCorePrivate()
    {
        if (m_UserViewerPage) {
            delete m_UserViewerPage;
            m_UserViewerPage = 0;
        }
        if (m_UserCreatorPage) {
            delete m_UserCreatorPage;
            m_UserCreatorPage = 0;
        }
        if (m_CalItemMapper) {
            delete m_CalItemMapper;
            m_CalItemMapper = 0;
        }
        if (m_AgendaMode) {
            delete m_AgendaMode;
            m_AgendaMode = 0;
        }
    }

public:
    bool m_Initialized;
    // TODO: use QPointers instead of pure pointers
    QHash<QString, UserCalendarModel *> m_UCalModels;
    QHash<QString, CalendarItemModel *> m_CalItemModel;
    Internal::UserCalendarPageForUserViewer *m_UserViewerPage;
    Internal::UserCalendarWizardCreatorPage *m_UserCreatorPage;
    Internal::CalendarItemEditorPatientMapper *m_CalItemMapper;
    Internal::AgendaMode *m_AgendaMode;
    Internal::AgendaBase *m_AgendaBase;
};
}  // End namespace Internal
}  // End namespace Agenda

AgendaCore *AgendaCore::m_Instance = 0;

AgendaCore &AgendaCore::instance()
{
    Q_ASSERT(m_Instance);
    return *m_Instance;
}

AgendaCore::AgendaCore(QObject *parent) :
    QObject(parent),
    d(new Internal::AgendaCorePrivate)
{
    m_Instance = this;

    // Add Agenda's UserViewer editor page
    pluginManager()->addObject(d->m_UserViewerPage = new Internal::UserCalendarPageForUserViewer(this));

    // Add the UserCreator wizard page
    pluginManager()->addObject(d->m_UserCreatorPage = new Internal::UserCalendarWizardCreatorPage(this));

    // Create the base pointer (does not initialize the database)
    d->m_AgendaBase = new Internal::AgendaBase(this);
}

AgendaCore::~AgendaCore()
{
    // remove object from plugin manager pool
    pluginManager()->removeObject(d->m_UserViewerPage);
    pluginManager()->removeObject(d->m_UserCreatorPage);
    pluginManager()->removeObject(d->m_CalItemMapper);
    pluginManager()->removeObject(d->m_AgendaMode);
    delete d;
}

bool AgendaCore::initializeDatabase()
{
    return d->m_AgendaBase->initialize();
}

void AgendaCore::extensionsInitialized()
{
    connect(user(), SIGNAL(userChanged()), this, SLOT(postCoreInitialization()));
}

/** Direct access to agenda base. Please DONT use this function outside the agenda plugin. */
Internal::AgendaBase &AgendaCore::agendaBase() const
{
    Q_ASSERT(d->m_AgendaBase);
    return *d->m_AgendaBase;
}

/** Create or get the Agenda::UserCalendarModel for the user \e userUid. The return pointer \b MUST \b NOT be deleted. */
UserCalendarModel *AgendaCore::userCalendarModel(const QString &userUid)
{
    QString uid = userUid;
    if (userUid.isEmpty()) {
        uid = user()->uuid();
    }
    UserCalendarModel *model = 0;
    if (d->m_UCalModels.keys().contains(uid)) {
        model = d->m_UCalModels.value(uid);
    } else {
        model = new UserCalendarModel(uid, this);
        d->m_UCalModels.insert(uid, model);
    }
    return model;
}

CalendarItemModel *AgendaCore::calendarItemModel(const QVariant &calendarUid)
{
    Q_ASSERT(!calendarUid.isNull());
    Q_ASSERT(calendarUid.isValid());
    if (calendarUid.isNull() || !calendarUid.isValid())
        return 0;

    CalendarItemModel *model = 0;
    if (d->m_CalItemModel.keys().contains(calendarUid.toString())) {
        model = d->m_CalItemModel.value(calendarUid.toString());
    } else {
        model = new CalendarItemModel(calendarUid, this);
        d->m_CalItemModel.insert(calendarUid.toString(), model);
    }
    return model;
}

void AgendaCore::postCoreInitialization()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << Q_FUNC_INFO;
    if (d->m_Initialized)
        return;
    if (!user())
        return;
    if (user()->uuid().isEmpty())
        return;

    initializeDatabase();

    // Add Agenda's Calendar::CalendarItem extended editing widgets
    pluginManager()->addObject(d->m_CalItemMapper = new Internal::CalendarItemEditorPatientMapper(this));

    // Add Agenda's mode
    pluginManager()->addObject(d->m_AgendaMode = new Internal::AgendaMode(this));

    d->m_Initialized = true;
    disconnect(user(), SIGNAL(userChanged()), this, SLOT(postCoreInitialization()));
}
