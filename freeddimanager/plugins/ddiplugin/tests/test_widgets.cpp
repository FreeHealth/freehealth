/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include <ddiplugin/ddiplugin.h>
#include <ddiplugin/constants.h>
#include <ddiplugin/atc/atcmode.h>
#include <ddiplugin/interactors/interactormode.h>
#include <ddiplugin/ddi/drugdruginteractionmode.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/modemanager/modemanager.h>

#include <utils/log.h>
#include <utils/randomizer.h>

#include <QString>
#include <QVariant>
#include <QTest>

using namespace DDI;
using namespace Internal;

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::ModeManager *modeManager() {return Core::ICore::instance()->modeManager();}

namespace {
static int loop = 100;
}

void DDIPlugin::test_atcWidget()
{
    // Get the mode
    modeManager()->activateMode(Constants::MODE_ATC);
    for(int i=0; i < loop; ++i)
        qApp->processEvents(QEventLoop::AllEvents);
    DDI::Internal::AtcMode *mode = qobject_cast<DDI::Internal::AtcMode*>(modeManager()->mode(Constants::MODE_ATC));
    Q_ASSERT(mode);
    mode->test_runWidgetTests();
}

void DDIPlugin::test_drugInteractorWidget()
{
    // Get the mode
    modeManager()->activateMode(Constants::MODE_INTERACTOR);
    for(int i=0; i < loop; ++i)
        qApp->processEvents(QEventLoop::AllEvents);
    DDI::Internal::InteractorMode *mode = qobject_cast<DDI::Internal::InteractorMode*>(modeManager()->mode(Constants::MODE_INTERACTOR));
    Q_ASSERT(mode);
    mode->test_runWidgetTests();
}

void DDIPlugin::test_drugDrugInteractionWidget()
{
    // Get the mode
    modeManager()->activateMode(Constants::MODE_DDI);
    for(int i=0; i < loop; ++i)
        qApp->processEvents(QEventLoop::AllEvents);
    DDI::Internal::DDIMode *mode = qobject_cast<DDI::Internal::DDIMode*>(modeManager()->mode(Constants::MODE_DDI));
    Q_ASSERT(mode);
    mode->test_runWidgetTests();
}

