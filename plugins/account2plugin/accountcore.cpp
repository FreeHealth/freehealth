/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  The FreeAccount plugins are free, open source FreeMedForms' plugins.   *
 *                                                                         *
 *  (C) 2013-now by Christian A Reiter and Eric Maeker                     *
 *  (C) 2010-2012 by Pierre-Marie Desombre and Eric Maeker                 *
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
 *  Main Developers :  Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors :                                                         *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
/**
 * \class Account2::AccountCore
 * Central object of the Account2 plugin. Manages all singleton objects,
 * database control...
 */

#include "accountcore.h"
#include "database/accountbase.h"
#include "database/accountbasequery.h"
#include "database/accountbaseresult.h"

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/isettings.h>

#include <extensionsystem/pluginmanager.h>
#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_filepathxml.h>

#include <datapackutils/datapackcore.h>
#include <datapackutils/ipackmanager.h>
#include <datapackutils/pack.h>

enum { RunDatabaseTests = false };

using namespace Account2;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline Core::IUser *user() {return Core::ICore::instance()->user();}

// Manage Datapacks
static inline DataPack::DataPackCore &dataPackCore() { return DataPack::DataPackCore::instance(); }
static inline DataPack::IPackManager *packManager() { return dataPackCore().packManager(); }

AccountCore *AccountCore::_instance = 0;

/** Return the instance of the object */
AccountCore *AccountCore::instance()
{
    Q_ASSERT(_instance);
    if (!_instance)
        _instance = new AccountCore(qApp);
    return _instance;
}

namespace Account2 {
namespace Internal {
class AccountCorePrivate
{
public:
    AccountCorePrivate(AccountCore *parent) :
        _accountBase(0),
        q(parent)
    {}

    ~AccountCorePrivate()
    {
    }

    void runDatabaseTests()
    {
        qWarning() << "RUN TESTS";
        _accountBase->createVirtuals(10);

        // Query
        AccountBaseQuery query;
        query.setDateRange(QDateTime::currentDateTime().addDays(-2), QDateTime::currentDateTime());
        query.setIncludeInvalidObjects(true);
        query.setRetrieveObjects(AccountBaseQuery::Fee | AccountBaseQuery::Payment);

        query.addUser("%");
        query.addPatient("%");

        AccountBaseResult result = _accountBase->query(query);
        // Warn Fees
        foreach(const Fee &fee, result.fees())
            qWarning() << fee;
        // Warn Payments
        foreach(const Payment &pay, result.payments())
            qWarning() << pay;
        // Warn Bankings
        foreach(const Banking &bkg, result.bankings())
            qWarning() << bkg;
    }

public:
    AccountBase *_accountBase;

private:
    AccountCore *q;
};
} // namespace Internal
} // namespace Account2

/** Construct the core of the Alert plugin. */
AccountCore::AccountCore(QObject *parent) :
    QObject(parent),
    d(new Internal::AccountCorePrivate(this))
{
    _instance = this;
    setObjectName("AccountCore");

    // Create all instance
    d->_accountBase = new Internal::AccountBase(this);

    connect(packManager(), SIGNAL(packInstalled(DataPack::Pack)), this, SLOT(packInstalled(DataPack::Pack)));
    connect(packManager(), SIGNAL(packRemoved(DataPack::Pack)), this, SLOT(packRemoved(DataPack::Pack)));
//    connect(packManager(), SIGNAL(packUpdated(DataPack::Pack)), this, SLOT(packChanged(DataPack::Pack)));
}

AccountCore::~AccountCore()
{
    if (d) {
        delete d;
        d = 0;
    }
}

/** Initialize the core. */
bool AccountCore::initialize()
{
    if (!d->_accountBase->initialize())
        return false;
    if (RunDatabaseTests)
        d->runDatabaseTests();
    return true;
}

void AccountCore::packInstalled(const DataPack::Pack &pack)
{
//    if (pack.dataType() == DataPack::Pack::AlertPacks) {
//        // register the alertpack
//        if (!registerAlertPack(pack.unzipPackToPath())) {
//            LOG_ERROR(tr("Unable to register AlertPack. Path: %1").arg(pack.unzipPackToPath()));
//            return;
//        }
//        // force a new alert checking
//        checkAllAlerts();
//    }
}

void AccountCore::packRemoved(const DataPack::Pack &pack)
{
//    if (pack.dataType() == DataPack::Pack::AlertPacks) {
//        if (!removeAlertPack(pack.uuid()))
//            LOG_ERROR("Unable to remove AlertPack " + pack.uuid());
//    }
}
