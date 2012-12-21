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
 *   Main developers: Eric MAEKER, <eric.maeker@gmail.com>                 *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class ZipCodes::ZipCore
 * Contains central ZipcCodes plugin instances and data.
 */

#include "zipcore.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <datapackutils/datapackcore.h>
#include <datapackutils/ipackmanager.h>
#include <datapackutils/pack.h>

#include <utils/log.h>
#include <translationutils/constants.h>
#include <translationutils/trans_database.h>

#include <QFileInfo>
#include <QSqlDatabase>
#include <QDir>

#include <QDebug>

using namespace ZipCodes;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline DataPack::DataPackCore &dataPackCore() { return DataPack::DataPackCore::instance(); }
static inline DataPack::IPackManager *packManager() { return dataPackCore().packManager(); }

namespace ZipCodes {
namespace Internal {
class ZipCorePrivate
{
public:
    ZipCorePrivate(ZipCore *parent) :
        _dbAvailable(false),
        q(parent)
    {
    }
    
    ~ZipCorePrivate()
    {
    }
    
    // Find the database to use. In priority order:
    // - User datapack
    // - Application installed datapack
    QString databasePath()
    {
        QString dbRelPath = "/zipcodes/zipcodes.db";
        QString tmp;
        tmp = settings()->dataPackInstallPath() + dbRelPath;
        if (QFileInfo(tmp).exists())
            return settings()->dataPackInstallPath();
        tmp = settings()->dataPackApplicationInstalledPath() + dbRelPath;
        return settings()->dataPackApplicationInstalledPath();
    }

    QString databaseFileName()
    {
        return databasePath() + QDir::separator() + "zipcodes" + QDir::separator() + "zipcodes.db";
    }

    // Populates the _dbAvailable and create/open _db
    void checkDatabase()
    {
        _db = QSqlDatabase();
        _dbAvailable = false;
        if (QSqlDatabase::connectionNames().contains("ZIPS")) {
            _db = QSqlDatabase::database("ZIPS");
            _dbAvailable = true;
        } else {
            LOG_FOR(q, QString("Trying to open ZipCode database from %1").arg(databaseFileName()));
            _db = QSqlDatabase::addDatabase("QSQLITE", "ZIPS");
            if (QFileInfo(databaseFileName()).exists()) {
                _db.setDatabaseName(databaseFileName());
                _dbAvailable = true;
            } else {
                _dbAvailable = false;
            }
        }
        if (_dbAvailable) {
            if (!_db.open()) {
                LOG_ERROR_FOR(q, "Unable to open Zip database");
                _dbAvailable = false;
            } else {
                LOG_FOR(q, tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg("zipcodes").arg("sqlite"));
            }
        }
    }

public:
    QSqlDatabase _db;
    bool _dbAvailable;
    
private:
    ZipCore *q;
};
} // namespace Internal
} // end namespace ZipCodes

// Singleton initialization
ZipCore *ZipCore::_instance = 0;

/**
 * Singleton access. This object creates its instance in the Ctor. So you should never
 * request the ctor more than once.
 */
ZipCodes::ZipCore &ZipCodes::ZipCore::instance() // static
{
    Q_ASSERT(_instance);
    return *_instance;
}

/*! Constructor of the ZipCodes::ZipCore class */
ZipCore::ZipCore(QObject *parent) :
    QObject(parent),
    d(new Internal::ZipCorePrivate(this))
{
    _instance = this;
    setObjectName("ZipCore");
}

/*! Destructor of the ZipCodes::ZipCore class */
ZipCore::~ZipCore()
{
    _instance = 0;
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool ZipCore::initialize()
{
    d->checkDatabase();

    // Manage datapacks
    connect(packManager(), SIGNAL(packInstalled(DataPack::Pack)), this, SLOT(packChanged(DataPack::Pack)));
    connect(packManager(), SIGNAL(packRemoved(DataPack::Pack)), this, SLOT(packChanged(DataPack::Pack)));
//    connect(packManager(), SIGNAL(packUpdated(DataPack::Pack)), this, SLOT(packChanged(DataPack::Pack)));
    return true;
}

/** Return the (un)initialized database */
QSqlDatabase &ZipCore::database() const
{
    return d->_db;
}

/** Returns true if a zipcodes database was found */
bool ZipCore::isDatabaseAvailable() const
{
    return d->_dbAvailable;
}

/** Refresh database if a Zip code data pack is installed/removed */
void ZipCore::packChanged(const DataPack::Pack &pack)
{
    if (pack.dataType() == DataPack::Pack::ZipCodes) {
        QSqlDatabase::removeDatabase("ZIPS");
        d->checkDatabase();
        Q_EMIT databaseRefreshed();
    }
}

