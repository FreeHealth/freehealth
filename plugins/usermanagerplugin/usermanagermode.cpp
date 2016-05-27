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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "usermanagermode.h"
#include "widgets/usermanager_p.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/iuser.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/modemanager/modemanager.h>

#include <extensionsystem/pluginmanager.h>

#include <translationutils/constanttranslations.h>

#include <QGridLayout>

#include <QDebug>

using namespace UserPlugin;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline ExtensionSystem::PluginManager *pluginManager() { return ExtensionSystem::PluginManager::instance(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::IUser *user()  { return Core::ICore::instance()->user(); }

UserManagerMode::UserManagerMode(QObject *parent) :
    Core::IMode(parent), inPool(false)
{
    setDisplayName(tkTr(Trans::Constants::USERS));
    setIcon(theme()->icon(Core::Constants::ICONUSERMANAGER, Core::ITheme::MediumIcon));
    setPriority(Core::Constants::P_MODE_USERMANAGER);
    setId(Core::Constants::MODE_USERMANAGER);
    setPatientBarVisibility(false);

    UserManagerWidget *w = new UserManagerWidget;
    w->initialize();

    setWidget(w);
    connect(user(), SIGNAL(userChanged()), this, SLOT(onUserChanged()));
    onUserChanged();
}

UserManagerMode::~UserManagerMode()
{
    if (inPool)
        pluginManager()->removeObject(this);
}

QString UserManagerMode::name() const
{
    return tkTr(Trans::Constants::USERS);
}

void UserManagerMode::onUserChanged()
{
    Core::IUser::UserRights r = Core::IUser::UserRights(user()->value(Core::IUser::ManagerRights).toInt());
    if (r & Core::IUser::AllRights) {
        if (!inPool)
            pluginManager()->addObject(this);
        inPool = true;
    } else {
        if (inPool) {
            pluginManager()->removeObject(this);
        }
        inPool = false;
    }
}
