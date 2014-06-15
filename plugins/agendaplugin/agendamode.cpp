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
 *   Main Developers:                                                     *
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "agendamode.h"
#include "usercalendarviewer.h"
#include "agendabase.h"
#include "agendacore.h"
#include "usercalendarmodel.h"
#include "constants.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/iuser.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/modemanager/modemanager.h>
#include <coreplugin/actionmanager/actionmanager.h>

#include <utils/log.h>

using namespace Agenda;
using namespace Internal;

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::IUser *user() { return Core::ICore::instance()->user(); }
static inline Agenda::AgendaCore &agendaCore() {return Agenda::AgendaCore::instance();}
static inline Core::ModeManager *modeManager() { return Core::ICore::instance()->modeManager(); }
static inline Core::ActionManager *actionManager() { return Core::ICore::instance()->actionManager(); }

AgendaMode::AgendaMode(QObject *parent) :
    Core::IMode(parent),
    m_UserCalendarModel(0)
{
    setDisplayName(tr("Agenda"));
    setIcon(theme()->icon(Core::Constants::ICONAGENDA, Core::ITheme::BigIcon));
    setPriority(Core::Constants::P_MODE_AGENDA);
    setId(Core::Constants::MODE_AGENDA);
    setPatientBarVisibility(false);
    m_Viewer = new UserCalendarViewer;
    setWidget(m_Viewer);
    userChanged();

    Core::Command *cmd = actionManager()->command(Agenda::Constants::A_NEW_AGENDAEVENT);
    modeManager()->addAction(cmd->action(), Core::Constants::P_MODE_AGENDA);

//    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreInitialization()));
    connect(user(), SIGNAL(userChanged()), this, SLOT(userChanged()));
}

void AgendaMode::postCoreInitialization()
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << Q_FUNC_INFO;
}

void AgendaMode::userChanged()
{
    if (m_UserCalendarModel) {
        disconnect(m_UserCalendarModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(rowsChanged(QModelIndex,int,int)));
        disconnect(m_UserCalendarModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(rowsChanged(QModelIndex,int,int)));
    }
    m_UserCalendarModel = agendaCore().userCalendarModel();
    updateEnableState();
    connect(m_UserCalendarModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(rowsChanged(QModelIndex,int,int)));
    connect(m_UserCalendarModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(rowsChanged(QModelIndex,int,int)));
}

void AgendaMode::rowsChanged(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    updateEnableState();
    m_Viewer->recalculateComboAgendaIndex();
}

void AgendaMode::updateEnableState()
{
    if (!m_UserCalendarModel)
        return;
    // Disable the mode if there are no user calendar
    int nbCals = m_UserCalendarModel->rowCount();
    setEnabled(nbCals>0);
    m_Viewer->setEnabled(nbCals>0);

    // idem for action mode
    Core::Command *cmd = actionManager()->command(Agenda::Constants::A_NEW_AGENDAEVENT);
    cmd->action()->setEnabled(nbCals>0);
}
