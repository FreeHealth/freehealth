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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 * \class DrugsDB::DrugBaseCore
 * Central place of the DrugsBase plugin. \n
 * Available objects:
 *   DrugsDB::DrugsBase \sa drugsBase()
 *   DrugsDB::ProtocolsBase \sa protocolsBase()
 *   DrugsDB::InteractionManager \sa interactionManager()
 *   DrugsDB::VersionUpdater \sa versionUpdater()
 *   DrugsDB::DrugsIO \sa drugsIo()
 * Manages Drug DataPack installation, removal
 */

#include "drugbasecore.h"
#include <drugsbaseplugin/constants.h>
#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/protocolsbase.h>
#include <drugsbaseplugin/interactionmanager.h>
#include <drugsbaseplugin/versionupdater.h>
#include <drugsbaseplugin/drugsio.h>
#include <drugsbaseplugin/prescriptionprinter.h>

#include <coreplugin/icore.h>

#include <utils/log.h>
#include <utils/global.h>
#include <datapackutils/datapackcore.h>
#include <datapackutils/ipackmanager.h>
#include <datapackutils/pack.h>

using namespace DrugsDB;
using namespace Internal;
//using namespace Trans::ConstantTranslations;

static inline DataPack::DataPackCore &dataPackCore() { return DataPack::DataPackCore::instance(); }
static inline DataPack::IPackManager *packManager() { return dataPackCore().packManager(); }

namespace DrugsDB {
namespace Internal {
class DrugBaseCorePrivate
{
public:
    DrugBaseCorePrivate(DrugBaseCore */*base*/) :
//        q(base),
        m_DrugsBase(0),
        m_ProtocolsBase(0),
        m_InteractionManager(0),
        m_VersionUpdater(0),
        _drugsIo(0),
        _prescriptionPrinter(0)
    {
    }

    ~DrugBaseCorePrivate()
    {
        if (m_VersionUpdater)
            delete m_VersionUpdater;
        m_VersionUpdater = 0;
        if (_prescriptionPrinter)
            delete _prescriptionPrinter;
        _prescriptionPrinter = 0;
    }

private:
//    DrugBaseCore *q;

public:
    DrugsBase *m_DrugsBase;
    ProtocolsBase *m_ProtocolsBase;
    InteractionManager *m_InteractionManager;
    VersionUpdater *m_VersionUpdater;
    DrugsIO *_drugsIo;
    PrescriptionPrinter *_prescriptionPrinter;
};
}  // End Internal
}  // End DrugsDB


DrugBaseCore *DrugBaseCore::m_Instance = 0;

/** \brief Returns the unique instance of DrugsDB::DrugBaseCore. If it does not exist, it is created */
DrugBaseCore &DrugBaseCore::instance()
{
    Q_ASSERT(m_Instance);
    return *m_Instance;
}

DrugBaseCore::DrugBaseCore(QObject *parent) :
    QObject(parent),
    d(new Internal::DrugBaseCorePrivate(this))
{
    m_Instance = this;
    d->m_DrugsBase = new DrugsBase(this);
    d->m_ProtocolsBase = new ProtocolsBase(this);
    d->m_VersionUpdater = new VersionUpdater;
    d->_drugsIo = new DrugsIO(this);
    d->_prescriptionPrinter = new PrescriptionPrinter();

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

bool DrugBaseCore::initialize()
{
    d->m_DrugsBase->initialize();
    d->m_ProtocolsBase->initialize();
    d->m_InteractionManager = new InteractionManager(this);
    d->_drugsIo->initialize();
    d->_prescriptionPrinter->initialize();
    connect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
    return true;
}

void DrugBaseCore::postCoreInitialization()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << Q_FUNC_INFO;
    initialize();
}

DrugsBase &DrugBaseCore::drugsBase() const
{
    Q_ASSERT(d->m_DrugsBase);
    return *d->m_DrugsBase;
}

ProtocolsBase &DrugBaseCore::protocolsBase() const
{
    Q_ASSERT(d->m_ProtocolsBase);
    return *d->m_ProtocolsBase;
}

InteractionManager &DrugBaseCore::interactionManager() const
{
    Q_ASSERT(d->m_InteractionManager);
    return *d->m_InteractionManager;
}

VersionUpdater &DrugBaseCore::versionUpdater() const
{
    Q_ASSERT(d->m_VersionUpdater);
    return *d->m_VersionUpdater;
}

DrugsIO &DrugBaseCore::drugsIo() const
{
    return *d->_drugsIo;
}

PrescriptionPrinter &DrugBaseCore::prescriptionPrinter() const
{
    return *d->_prescriptionPrinter;
}

void DrugBaseCore::onCoreDatabaseServerChanged()
{
    Q_ASSERT(d->m_DrugsBase);
    d->m_DrugsBase->onCoreDatabaseServerChanged();
    d->m_ProtocolsBase->onCoreDatabaseServerChanged();
}

void DrugBaseCore::packChanged(const DataPack::Pack &pack)
{
    if (pack.dataType() == DataPack::Pack::DrugsWithInteractions ||
            pack.dataType() == DataPack::Pack::DrugsWithoutInteractions) {
        d->m_DrugsBase->datapackChanged();
    }
}
