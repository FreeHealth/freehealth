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
 *  Main developers : Eric Maeker                                          *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 * \class DataPack::PackCreationModel
 * Read only tree model that manages DataPack::PackCreationQueue contents.\n
 * Queues represents datapack in use for a datapack server creation.\n
 * You can add/remove queues/screening paths to the model or use the setData()
 * with a Qt::CheckStateRole to tag datapacks to use/not use for a server creation.
 */

// Tree Model:
// Screening_AbsPath
//      DataPack
//          ServerUid
//          Name
//          Type
//          Size
//          ...
//      DataPack
// ...

#include "packcreationmodel.h"
#include "packcreationqueue.h"

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>

using namespace DataPack;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace DataPack {
namespace Internal {
class PackCreationModelPrivate {
public:
    PackCreationModelPrivate(PackCreationModel *parent) :
        q(parent)
    {}

    ~PackCreationModelPrivate()
    {}

    void initialize()
    {
        q->clear();
        // Create the root item
        QStandardItem *root = new QStandardItem(tkTr(Trans::Constants::NO_ITEM));
        q->invisibleRootItem()->appendRow(root);
        // Set the number of columns
        q->setColumnCount(1);
    }

//    // Return cached item corresponding to the serverUid \e uid
//    QStandardItem *serverItem(const QString &uid)
//    {
//        return _serversUidToItem.value(uid, 0);
//    }

//    // Cache the corresponding item for serverUid \e uid
//    void addServerItem(const QString &uid, QStandardItem *item)
//    {
//        _serversUidToItem.insert(uid, item);
//    }

    // Return cached item corresponding to the screeningAbsolutePath \e path
    QStandardItem *absPathItem(const QString &path)
    {
        return _screeningPathToItem.value(path, 0);
    }

    // Cache the corresponding item for screeningAbsolutePath \e path
    void addAbsPathItem(const QString &path, QStandardItem *item)
    {
        _screeningPathToItem.insert(path, item);
    }

    bool isPackCreationQueueAlreadyInserted(const PackCreationQueue &queue)
    {
        return _insertedPackCreationQueueUids.contains(queue.uid());
    }

    void insertPackCreationQueueInCache(const PackCreationQueue &queue)
    {
        _insertedPackCreationQueueUids.append(queue.uid());
    }

    // Insert a queue content to the model
    bool packCreationQueueToItem(const PackCreationQueue &queue)
    {
        // Already inserted?
        if (isPackCreationQueueAlreadyInserted(queue))
            return true;
        insertPackCreationQueueInCache(queue);

        // Get the path item or create it & cache it
        foreach(const RequestedPackCreation &request, queue.queue()) {
            QStandardItem *server = new QStandardItem(QString("%1: %2")
                                                    .arg(tkTr(Trans::Constants::SERVER))
                                                    .arg(request.serverUid));
            q->invisibleRootItem()->appendRow(server);
        }

        // Read all packdescription xmlfiles and related create items
        return true;
    }

public:
    QHash<QString, QStandardItem *> _serversUidToItem, _screeningPathToItem;
    QStringList _insertedPackCreationQueueUids;

private:
    PackCreationModel *q;
};
} // namespace Internal
} // namespace DataPack

PackCreationModel::PackCreationModel(QObject *parent) :
    QStandardItemModel(parent),
    d(new Internal::PackCreationModelPrivate(this))
{
    d->initialize();

}

PackCreationModel::~PackCreationModel()
{
    if (d)
        delete d;
    d = 0;
}

QVariant PackCreationModel::data(const QModelIndex &index, int role) const
{
    return QStandardItemModel::data(index, role);
}

bool PackCreationModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    // TODO: manage checkstaterole
    return false;
}

//QVariant PackCreationModel::headerData(int section, Qt::Orientation orientation, int role) const
//{
//    return QVariant();
//}

bool PackCreationModel::addPackCreationQueue(const PackCreationQueue &queue)
{
    return d->packCreationQueueToItem(queue);
}

bool PackCreationModel::addScreeningPath(const QString &screeningAbsPath)
{
    // Path already screened in model?
    // Find all packcreationqueue-xmlfiles (recursively)
    // Add all packcreationqueue to the model
    return true;
}
