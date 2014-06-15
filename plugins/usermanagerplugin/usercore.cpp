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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class UserPlugin::UserCore
 * Central place for the UserPlugin.
 */

#include "usercore.h"
#include <usermanagerplugin/database/userbase.h>
#include <usermanagerplugin/usermodel.h>
#include <usermanagerplugin/coreusermodelwrapper.h>

#include <coreplugin/icore.h>
#include <coreplugin/translators.h>
#include <coreplugin/isettings.h>
#include <coreplugin/iuser.h>
#include <coreplugin/constants_tokensandsettings.h>

#include <translationutils/constants.h>

#include <QDebug>

using namespace UserPlugin;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::IUser *user() {return Core::ICore::instance()->user();}

namespace UserPlugin {
namespace Internal {
class UserCorePrivate
{
public:
    UserCorePrivate(UserCore */*parent*/) :
        _base(0),
        _model(0),
        _coreUserModelWrapper(0)  // , q(parent)
    {
    }

    ~UserCorePrivate()
    {
    }

public:
    UserBase *_base;
    UserModel *_model;
    CoreUserModelWrapper *_coreUserModelWrapper;

private:
//    UserCore *q;
};
} // namespace Internal
} // end namespace UserPlugin

// Singleton initialization
UserCore *UserCore::_instance = 0;

/**
 * Singleton access. This object creates its instance in the Ctor. So you should never
 * request the ctor more than once.
 */
UserPlugin::UserCore &UserPlugin::UserCore::instance() // static
{
    Q_ASSERT(_instance);
    return *_instance;
}

/*! Constructor of the UserPlugin::UserCore class */
UserCore::UserCore(QObject *parent) :
    QObject(parent),
    d(new UserCorePrivate(this))
{
    _instance = this;
    d->_base = new UserBase(this);
}

/*! Destructor of the UserPlugin::UserCore class */
UserCore::~UserCore()
{
    _instance = 0;
    if (d)
        delete d;
    d = 0;
}

/*!
 * Initializes the object with the default values.
 * Return true if initialization was completed.
 */
bool UserCore::initialize()
{
    // Already initialized?
    if (d->_model)
        return true;

    // Start initialization
    if (!d->_base->initialize())
        return false;

    // Create the Core::IUser models
    d->_model = new UserModel(this);
    d->_model->initialize();
    d->_coreUserModelWrapper = new CoreUserModelWrapper(this);
    d->_coreUserModelWrapper->initialize(d->_model);
    Core::ICore::instance()->setUser(d->_coreUserModelWrapper);
    connect(settings(), SIGNAL(userSettingsSynchronized()), d->_model, SLOT(updateUserPreferences()), Qt::UniqueConnection);

    return true;
}

/*!
 * Return true if the core is initialization is completed.
 */
bool UserCore::isInitialized() const
{
    return (d->_model!=0)
            && (d->_coreUserModelWrapper!=0)
            && (Core::ICore::instance()->user()!=0)
            && d->_base->isInitialized()
            ;
}

/**
 * If the first initialization gone wrong, you can force a new initialization of the user core
 * after the problem was corrected.
 */
bool UserCore::forceReInitialization()
{
    delete d->_model;
    d->_model = 0;
    delete d->_coreUserModelWrapper;
    d->_coreUserModelWrapper = 0;
    Core::ICore::instance()->setUser(0);
    d->_base->onCoreDatabaseServerChanged(); // force a re-initialization of the database
    return initialize();
}

/**
 * \internal
 * Post core opened - called by UserManagerPlugin object
 */
bool UserCore::postCoreInitialization()
{
    if (!user())
        return false;
    d->_model->checkUserPreferencesValidity();
    // be sure everyone is informed of the currently connected user
    d->_model->emitUserConnected();
    // and be sure that ui is translated in the correct language
    Core::ICore::instance()->translators()->changeLanguage(settings()->value(Core::Constants::S_PREFERREDLANGUAGE, user()->value(Core::IUser::LanguageISO).toString()).toString());
    return true;
}

/**
 * Only for internal use.\n
 * Return the Utils::Database for the UserPlugin plugin
 */
Internal::UserBase *UserCore::userBase() const
{
    return d->_base;
}

/**
 * Return the main user model (used in the Core::IUser).
 */
UserModel *UserCore::userModel() const
{
    return d->_model;
}
