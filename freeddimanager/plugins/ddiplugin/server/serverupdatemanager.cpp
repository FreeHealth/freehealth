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
 *   Main developers: Eric MAEKER, <eric.maeker@gmail.com>                 *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class DDI::ServerUpdateManager
 */

#include "serverupdatemanager.h"
#include <ddiplugin/constants.h>
#include <ddiplugin/ddicore.h>
#include <ddiplugin/database/ddidatabase.h>

#include <translationutils/constants.h>

#include <QDateTime>

#include <QDebug>

using namespace DDI;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline DDI::DDICore *ddiCore()  { return DDI::DDICore::instance(); }
static inline DDI::Internal::DDIDatabase &ddiBase()  { return DDI::DDICore::instance()->database(); }

namespace DDI {
namespace Internal {
class ServerUpdateManagerPrivate
{
public:
    ServerUpdateManagerPrivate(ServerUpdateManager *parent) :
        q(parent)
    {
        Q_UNUSED(q);
    }
    
    ~ServerUpdateManagerPrivate()
    {
    }

    // Wiki:
    // {{~ServerDate~}} -> current server date
    void createWikiContent(ServerUpdate::UpdateType type, ServerUpdate::UpdateSubType subType)
    {
        // TODO: code here
        Q_UNUSED(type);
        Q_UNUSED(subType);
    }
    
    // Returns the last date of server update extracted from the database,
    // or, if the user never sent update to the server, the database date of release
    QDateTime lastServerUpdateDateTime()
    {
        // Get latest server update (last time the user sent his data to the FreeMedForms server)
        QDateTime lastUpdate = ddiBase().dateOfLastServerUpdate();
        if (!lastUpdate.isValid())
            lastUpdate = ddiBase().dateOfRelease();
        return lastUpdate;
    }

public:
    QVector<ServerUpdate> _updates;
    
private:
    ServerUpdateManager *q;
};
} // namespace Internal
} // end namespace DDI


/*! Constructor of the DDI::ServerUpdateManager class */
ServerUpdateManager::ServerUpdateManager(QObject *parent) :
    QObject(parent),
    d(new ServerUpdateManagerPrivate(this))
{
}

/*! Destructor of the DDI::ServerUpdateManager class */
ServerUpdateManager::~ServerUpdateManager()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool ServerUpdateManager::initialize()
{
    return true;
}

void ServerUpdateManager::prepareUpdateToSend(ServerUpdate::UpdateType type, ServerUpdate::UpdateSubType subType)
{
    // TODO: code here
    Q_UNUSED(type);
    Q_UNUSED(subType);
}

void ServerUpdateManager::sendUpdates()
{}

/** Return the number of available update in the local database */
int ServerUpdateManager::numberDatabaseOfUpdates(ServerUpdate::UpdateType type, ServerUpdate::UpdateSubType subType) const
{
    QHash<int, QString> where;
    int table = -1;
    int field = 0;
    if (type == ServerUpdate::New) {
        switch (subType) {
        case ServerUpdate::Atc:
        {
            table = Constants::Table_ATC;
            where.insert(Constants::ATC_ISVALID, "=1");
            where.insert(Constants::ATC_DATECREATE, Constants::SQL_ISNOTNULL);
            where.insert(Constants::ATC_DATECREATE, QString("> '%1'").arg(d->lastServerUpdateDateTime().toString(Qt::ISODate)));
            break;
        }
        case ServerUpdate::Interactor:
        {
            table = Constants::Table_INTERACTORS;
            where.insert(Constants::INTERACTOR_ISVALID, "=1");
            where.insert(Constants::INTERACTOR_ISREVIEWED, "=1");
            where.insert(Constants::INTERACTOR_DATECREATE, Constants::SQL_ISNOTNULL);
            where.insert(Constants::INTERACTOR_DATECREATE, QString("> '%1'").arg(d->lastServerUpdateDateTime().toString(Qt::ISODate)));
            break;
        }
        case ServerUpdate::DrugDrugInteraction:
        {
            table = Constants::Table_DDI;
            where.insert(Constants::DDI_ISVALID, "=1");
            where.insert(Constants::DDI_ISREVIEWED, "=1");
            where.insert(Constants::DDI_DATECREATE, Constants::SQL_ISNOTNULL);
            where.insert(Constants::DDI_DATECREATE, QString("> '%1'").arg(d->lastServerUpdateDateTime().toString(Qt::ISODate)));
            break;
        }
        } // switch subType

    } else if (type == ServerUpdate::Update) {
        switch (subType) {
        case ServerUpdate::Atc:
        {
            table = Constants::Table_ATC;
            where.insert(Constants::ATC_ISVALID, "=1");
            where.insert(Constants::ATC_DATEUPDATE, Constants::SQL_ISNOTNULL);
            where.insert(Constants::ATC_DATEUPDATE, QString("> '%1'").arg(d->lastServerUpdateDateTime().toString(Qt::ISODate)));
            where.insert(Constants::ATC_DATECREATE, QString("< '%1'").arg(d->lastServerUpdateDateTime().toString(Qt::ISODate)));
            break;
        }
        case ServerUpdate::Interactor:
        {
            table = Constants::Table_INTERACTORS;
            where.insert(Constants::INTERACTOR_ISVALID, "=1");
            where.insert(Constants::INTERACTOR_ISREVIEWED, "=1");
            where.insert(Constants::INTERACTOR_DATEUPDATE, Constants::SQL_ISNOTNULL);
            where.insert(Constants::INTERACTOR_DATEUPDATE, QString("> '%1'").arg(d->lastServerUpdateDateTime().toString(Qt::ISODate)));
            where.insert(Constants::INTERACTOR_DATECREATE, QString("< '%1'").arg(d->lastServerUpdateDateTime().toString(Qt::ISODate)));
            break;
        }
        case ServerUpdate::DrugDrugInteraction:
        {
            table = Constants::Table_DDI;
            where.insert(Constants::DDI_ISVALID, "=1");
            where.insert(Constants::DDI_ISREVIEWED, "=1");
            where.insert(Constants::DDI_DATEUPDATE, Constants::SQL_ISNOTNULL);
            where.insert(Constants::DDI_DATEUPDATE, QString("> '%1'").arg(d->lastServerUpdateDateTime().toString(Qt::ISODate)));
            where.insert(Constants::DDI_DATECREATE, QString("< '%1'").arg(d->lastServerUpdateDateTime().toString(Qt::ISODate)));
            break;
        }
        } // switch subType
    }

    if (table != -1)
        return ddiBase().count(table, field, ddiBase().getWhereClause(table, where));

    return 0;
}
