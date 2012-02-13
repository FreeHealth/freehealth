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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "drugbasecore.h"
#include <drugsbaseplugin/constants.h>
#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/interactionmanager.h>

#include <coreplugin/icore.h>

#include <utils/log.h>
#include <utils/global.h>
#include <datapackutils/datapackcore.h>
#include <datapackutils/ipackmanager.h>
#include <datapackutils/pack.h>

using namespace DrugsDB;
using namespace DrugsDB::Internal;
//using namespace Trans::ConstantTranslations;

static inline DataPack::DataPackCore &dataPackCore() { return DataPack::DataPackCore::instance(); }
static inline DataPack::IPackManager *packManager() { return dataPackCore().packManager(); }

namespace DrugsDB {
namespace Internal {
class DrugBaseCorePrivate
{
public:
    DrugBaseCorePrivate(DrugBaseCore *base) :
        q(base),
        m_DrugsBase(0),
        m_ProtocolBase(0),
        m_InteractionManager(0)
    {
    }

    ~DrugBaseCorePrivate()
    {
    }

private:
    DrugBaseCore *q;

public:
    DrugsBase *m_DrugsBase;
    ProtocolBase *m_ProtocolBase;
    InteractionManager *m_InteractionManager;
};
}  // End Internal
}  // End DrugsDB


DrugBaseCore *DrugBaseCore::m_Instance = 0;

/** \brief Returns the unique instance of DrugsDB::DrugBaseCore. If it does not exists, it is created */
DrugBaseCore &DrugBaseCore::instance(QObject *parent)
{
    if (!m_Instance) {
        m_Instance = new DrugBaseCore(parent);
        m_Instance->init();
    }
    return *m_Instance;
}

DrugBaseCore::DrugBaseCore(QObject *parent) :
    QObject(parent),
    d(new Internal::DrugBaseCorePrivate(this))
{
    connect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
    connect(packManager(), SIGNAL(packInstalled(DataPack::Pack)), this, SLOT(packChanged(DataPack::Pack)));
    connect(packManager(), SIGNAL(packRemoved(DataPack::Pack)), this, SLOT(packChanged(DataPack::Pack)));
//    connect(packManager(), SIGNAL(packUpdated(DataPack::Pack)), this, SLOT(packChanged(DataPack::Pack)));
}

DrugBaseCore::~DrugBaseCore()
{
    if (d) {
        delete d;
        d = 0;
    }
}

bool DrugBaseCore::init()
{
    d->m_DrugsBase = new DrugsBase(this);
    d->m_DrugsBase->init();
//    d->m_ProtocolBase
    d->m_InteractionManager = new InteractionManager(this);
    /** \todo code here */
}

DrugsBase &DrugBaseCore::drugsBase() const
{
    Q_ASSERT(d->m_DrugsBase);
    return *d->m_DrugsBase;
}

ProtocolBase &DrugBaseCore::protocolBase() const
{
    Q_ASSERT(d->m_ProtocolBase);
    return *d->m_ProtocolBase;
}

InteractionManager &DrugBaseCore::interactionManager() const
{
    Q_ASSERT(d->m_InteractionManager);
    return *d->m_InteractionManager;
}

void DrugBaseCore::onCoreDatabaseServerChanged()
{
    Q_ASSERT(d->m_DrugsBase);
    d->m_DrugsBase->onCoreDatabaseServerChanged();
}

void DrugBaseCore::packChanged(const DataPack::Pack &pack)
{
    WARN_FUNC << pack.uuid() << pack.version() << pack.dataType();
    if (pack.dataType() == DataPack::Pack::DrugsWithInteractions ||
            pack.dataType() == DataPack::Pack::DrugsWithoutInteractions) {
        // Refresh drugsbase
        qWarning() << "XXXXXXXXXXXXXX REFRESH DRUG DATABASE" << pack.uuid() << pack.version();
        d->m_DrugsBase->datapackChanged();
    }
}
