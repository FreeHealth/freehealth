/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@gmail.com                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "servermanagermode.h"
#include <ddiplugin/server/servermanagerwidget.h>
#include <ddiplugin/constants.h>

// TEST
#include <ddiplugin/server/serverupdatemanager.h>
#include <QLabel>
#include <QDebug>
// END TEST


#include <coreplugin/icore.h>
#include <coreplugin/theme.h>
#include <coreplugin/modemanager/modemanager.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>

#include <translationutils/constants.h>
#include <translationutils/trans_current.h>

using namespace DDI;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ModeManager *modeManager() { return Core::ICore::instance()->modeManager(); }
static inline Core::ActionManager *actionManager() { return Core::ICore::instance()->actionManager(); }
static inline Core::ITheme *theme() { return Core::ICore::instance()->theme(); }

ServerManagerMode::ServerManagerMode(QObject *parent) :
    Core::IMode(parent),
    _widget(0)
{
    setDisplayName(tkTr(Trans::Constants::SERVER));
    setIcon(theme()->icon(Constants::ICON_SERVERMODE, Core::ITheme::BigIcon));
    setPriority(1000);
    setId(Constants::MODE_SERVERMANAGER);
    setPatientBarVisibility(false);

//    // Add the new patient action in the mode manager
//    Core::Command *cmd = actionManager()->command(Core::Constants::A_PATIENT_NEW);
//    modeManager()->addAction(cmd->action(), Core::Constants::P_MODE_PATIENT_SEARCH);

    // create the mode widget
    _widget = new ServerManagerWidget;
    _widget->initialize();

//    ServerUpdateManager m;
//    QLabel *l = new QLabel;

//    qWarning() << "--------------";

//    QString t = QString("New ATC: %1\n").arg(m.numberDatabaseOfUpdates(ServerUpdate::New, ServerUpdate::Atc));
//    t += QString("New Interactor: %1\n").arg(m.numberDatabaseOfUpdates(ServerUpdate::New, ServerUpdate::Interactor));
//    t += QString("New DDI: %1\n").arg(m.numberDatabaseOfUpdates(ServerUpdate::New, ServerUpdate::DrugDrugInteraction));
//    t += QString("Update ATC: %1\n").arg(m.numberDatabaseOfUpdates(ServerUpdate::Update, ServerUpdate::Atc));
//    t += QString("Update Interactor: %1\n").arg(m.numberDatabaseOfUpdates(ServerUpdate::Update, ServerUpdate::Interactor));
//    t += QString("Update DDI: %1\n").arg(m.numberDatabaseOfUpdates(ServerUpdate::Update, ServerUpdate::DrugDrugInteraction));

//    qWarning() << "--------------";

//    l->setText(t);
    setWidget(_widget);
}

ServerManagerMode::~ServerManagerMode()
{
//    if (_widget)
//        delete _widget;
}

void ServerManagerMode::postCoreInitialization()
{
}
