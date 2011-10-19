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
 *   Main Developpers :                                                    *
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "agendamode.h"
#include "usercalendarviewer.h"
#include "agendabase.h"
#include "agendacore.h"
#include "usercalendarmodel.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/iuser.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>

using namespace Agenda;
using namespace Internal;

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline Core::IPatient *patient() { return Core::ICore::instance()->patient(); }
static inline Core::IUser *user() { return Core::ICore::instance()->user(); }
static inline Agenda::Internal::AgendaBase *base() {return Agenda::Internal::AgendaBase::instance();}
static inline Agenda::AgendaCore *agendaCore() {return Agenda::AgendaCore::instance();}

AgendaMode::AgendaMode(QObject *parent) :
    Core::BaseMode(parent),
    m_UserCalendarModel(0)
{
    setName(tr("Agenda"));
    setIcon(theme()->icon(Core::Constants::ICONAGENDA, Core::ITheme::BigIcon));
    setPriority(Core::Constants::P_MODE_AGENDA);
    setUniqueModeName(Core::Constants::MODE_AGENDA);
    setPatientBarVisibility(false);
//    const QList<int> &context;
//    setContext();
//    m_Stack = new QStackedWidget;
//    setWidget(m_Stack);
    m_Viewer = new UserCalendarViewer;
    setWidget(m_Viewer);
    userChanged();

//    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreInitialization()));
    connect(user(), SIGNAL(userChanged()), this, SLOT(userChanged()));
}

QString AgendaMode::name() const
{
    return tr("Agenda");
}

void AgendaMode::postCoreInitialization()
{
}

void AgendaMode::userChanged()
{
    if (m_UserCalendarModel) {
        disconnect(m_UserCalendarModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(rowsChanged(QModelIndex,int,int)));
        disconnect(m_UserCalendarModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(rowsChanged(QModelIndex,int,int)));
    }
    m_UserCalendarModel = agendaCore()->userCalendarModel();
    int nbCals = m_UserCalendarModel->rowCount();
    m_Viewer->setEnabled(nbCals>0);
    connect(m_UserCalendarModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(rowsChanged(QModelIndex,int,int)));
    connect(m_UserCalendarModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(rowsChanged(QModelIndex,int,int)));
}

void AgendaMode::rowsChanged(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    int nbCals = m_UserCalendarModel->rowCount();
    m_Viewer->setEnabled((nbCals>0));
    m_Viewer->recalculateComboAgendaIndex();
}
