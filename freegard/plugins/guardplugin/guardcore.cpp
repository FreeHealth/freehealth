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
 *  Main Developers:   Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
/**
 * \class Guard::GuardCore
 * Central object of the Guard plugin. Manages all singleton objects,
 * database control...
 */

#include "guardcore.h"
#include "database/guardbase.h"

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/isettings.h>

#include <extensionsystem/pluginmanager.h>
#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_filepathxml.h>

enum { RunDatabaseTests = false };

using namespace Guard;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::IUser *user() {return Core::ICore::instance()->user();}

GuardCore *GuardCore::_instance = 0;

/** Return the instance of the object. The object must be created firstly. */
GuardCore &GuardCore::instance()
{
    Q_ASSERT(_instance);
    if (!_instance)
        _instance = new GuardCore(qApp);
    return *_instance;
}

namespace Guard {
namespace Internal {
class GuardCorePrivate
{
public:
    GuardCorePrivate(GuardCore *parent) :
        _guardBase(0),
        q(parent)
    {
        Q_UNUSED(q);
    }

    ~GuardCorePrivate()
    {
    }

public:
    GuardBase *_guardBase;

private:
    GuardCore *q;
};
} // namespace Internal
} // namespace Guard

/** Construct the core of the Alert plugin. */
GuardCore::GuardCore(QObject *parent) :
    QObject(parent),
    d(new Internal::GuardCorePrivate(this))
{
    _instance = this;
    setObjectName("GuardCore");

    // Create all instance
    d->_guardBase = new Internal::GuardBase(this);
}

GuardCore::~GuardCore()
{
    if (d) {
        delete d;
        d = 0;
    }
}

/** Initialize the core. */
bool GuardCore::initialize()
{
    if (!d->_guardBase->initialize())
        return false;
//    if (RunDatabaseTests)
//        d->runDatabaseTests();
    return true;
}

/** Return the initiazation state of the accountancy database */
bool GuardCore::isDatabaseInitialized() const
{
    return d->_guardBase->isInitialized();
}

/**
 * Return the internal database. SHOULDN'T BE USED OUTSIDE THIS PLUGIN.
 * \internal
*/
Internal::GuardBase &GuardCore::guardBase() const
{
    return *d->_guardBase;
}
