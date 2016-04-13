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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developers:                                                       *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "../formmanagerplugin.h"
#include "../formcore.h"
#include "../formmanager.h"
#include "../episodebase.h"
#include "../episodemanager.h"
#include "../patientformitemdatawrapper.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/randomizer.h>

#include <QTest>
#include <QSignalSpy>

using namespace Form;
using namespace Internal;

static inline Form::FormCore &formCore() {return Form::FormCore::instance();}
static inline Form::Internal::EpisodeBase *episodeBase() {return Form::Internal::EpisodeBase::instance();}

void FormManagerPlugin::initTestCase()
{
}

void FormManagerPlugin::test_formcore_initialization()
{
    QCOMPARE(formCore().isInitialized(), true);
    QCOMPARE(formCore().formManager().isInitialized(), true);
    QCOMPARE(formCore().episodeManager().isInitialized(), true);
    QCOMPARE(formCore().patientFormItemDataWrapper().isInitialized(), true);
    QCOMPARE(episodeBase()->isInitialized(), true);
}

void FormManagerPlugin::cleanupTestCase()
{
}
