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
 *  Main developers : Eric Maeker                                          *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 * \class DataPack::PackCreationModel
 * Read only tree model that manages DataPack::PackCreationQueue contents.\n
 * Queues represents a list of datapacks that should be used for during a
 * server creation.\n
 * You can add/remove queues/screening paths to the model or use the setData()
 * with a Qt::CheckStateRole to tag datapacks to use/not use for a server creation.
 *
 * Tree format:
 * You can define the tree model format to:
 * - ShowByQueue
 * - ShowByServer (default value)
 * If you set the format to ShowByQueue, the tree model is structured like this:
 * - [] Queue File Name
 *     - [] Server Name/Uid
 *         - [] DataPack Name
 *             - Uid
 *             - TypeOfContent
 *             - TotalSize
 *             - LicenseType
 *             - Dates: creation, last update
 *             - ...
 * If you set the format to ShowByServer, the tree model is structured like this:
 * - [] Server Name/Uid
 *     - [] DataPack Name
 *         - Uid
 *         - TypeOfContent
 *         - TotalSize
 *         - LicenseType
 *         - Dates: creation, last update
 *         - QueueFile: absPathToQueue
 *         - ...
 * By default, all items are checked.
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
#include <datapackutils/pack.h>
#include <datapackutils/constants.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>
#include <translationutils/trans_msgerror.h>
#include <translationutils/trans_filepathxml.h>

#include <QDir>

using namespace DataPack;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace DataPack {
namespace Internal {
class PackCreationModelPrivate {
public:
    PackCreationModelPrivate(PackCreationModel *parent) :
        _format(PackCreationModel::ShowByQueue),
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

    // Clear the model contents and all cache but screenedPath
    void clearModelAndCache()
    {
        q->clear();
        _serversUidToItem.clear();
        // _screeningPathToItem.clear();
        _insertedPackCreationQueueUids.clear();
        _packDescriptionFilesIncluded.clear();
        _packItems.clear();
        _queues.clear();
    }

    // Return cached item corresponding to the serverUid \e uid
    QStandardItem *serverItem(const QString &uid)
    {
        return _serversUidToItem.value(uid, 0);
    }

    // Cache the corresponding item for serverUid \e uid
    void addServerItem(const QString &uid, QStandardItem *item)
    {
        _serversUidToItem.insert(uid, item);
    }

//    // Return cached item corresponding to the screeningAbsolutePath \e path
//    QStandardItem *absPathItem(const QString &path)
//    {
//        return _screeningPathToItem.value(path, 0);
//    }

//    // Cache the corresponding item for screeningAbsolutePath \e path
//    void addAbsPathItem(const QString &path, QStandardItem *item)
//    {
//        _screeningPathToItem.insert(path, item);
//    }

    // Checks if a queue is already included in the model
    bool isPackCreationQueueAlreadyInserted(const PackCreationQueue &queue)
    {
        return _insertedPackCreationQueueUids.keys().contains(queue.uid());
    }

    // Insert a queue to the model
    void insertPackCreationQueueInCache(const PackCreationQueue &queue, QStandardItem *item)
    {
        _insertedPackCreationQueueUids.insert(queue.uid(), item);
    }

    // Returns the root tree item containing the whole datapack information
    QStandardItem *packToItem(const QString &absPathToDescriptionFile, const PackCreationQueue &queue)
    {
        // Read datapack description file
        DataPack::Pack pack;
        pack.fromXmlFile(absPathToDescriptionFile);

        // Create datapack items
        QStandardItem *packItem = new QStandardItem(pack.name());
        QFont bold;
        bold.setBold(true);
        packItem->setFont(bold);
        packItem->setCheckable(true);
        packItem->setCheckState(Qt::Checked);

        QStandardItem *item = 0;
        item = new QStandardItem(tkTr(Trans::Constants::_1_COLON_2)
                                 .arg(tkTr(Trans::Constants::TYPE))
                                 .arg(pack.dataTypeName()));
        item->setToolTip(item->text());
        packItem->appendRow(item);
        item = new QStandardItem(tkTr(Trans::Constants::_1_COLON_2)
                                 .arg(tkTr(Trans::Constants::LICENSE))
                                 .arg(pack.description().data(PackDescription::LicenseName).toString()));
        item->setToolTip(item->text());
        packItem->appendRow(item);
        item = new QStandardItem(tkTr(Trans::Constants::_1_COLON_2)
                                 .arg(tkTr(Trans::Constants::VERSION))
                                 .arg(pack.version()));
        item->setToolTip(item->text());
        packItem->appendRow(item);
        item = new QStandardItem(tkTr(Trans::Constants::_1_COLON_2)
                                 .arg(tkTr(Trans::Constants::VENDOR))
                                 .arg(pack.vendor()));
        item->setToolTip(item->text());
        packItem->appendRow(item);
        item = new QStandardItem(tkTr(Trans::Constants::_1_COLON_2)
                                 .arg(tkTr(Trans::Constants::CREATION_DATE_TIME))
                                 .arg(QLocale().toString(pack.description().data(PackDescription::CreationDate).toDate())));
        item->setToolTip(item->text());
        packItem->appendRow(item);
        item = new QStandardItem(tkTr(Trans::Constants::_1_COLON_2)
                                 .arg(tkTr(Trans::Constants::LAST_MODIFICATION))
                                 .arg(QLocale().toString(pack.description().data(PackDescription::LastModificationDate).toDate())));
        item->setToolTip(item->text());
        packItem->appendRow(item);

        if (_format == PackCreationModel::ShowByServer) {
            item = new QStandardItem(tkTr(Trans::Constants::_1_COLON_2)
                                     .arg(tkTr(Trans::Constants::QUEUE))
                                     .arg(queue.sourceAbsolutePathFile()));
            item->setToolTip(item->text());
            packItem->appendRow(item);
        }

        _packItems.insert(pack.originalXmlConfigFileName(), packItem);

        return packItem;
    }

    // Insert a queue content to the model
    bool packCreationQueueToItem(const PackCreationQueue &queue)
    {
        // Already inserted?
        if (isPackCreationQueueAlreadyInserted(queue))
            return true;

        QFont bold;
        bold.setBold(true);

        // Create the root item for this queue
        QStandardItem *rootItem = 0;
        if (_format == PackCreationModel::ShowByServer) {
            // nothing to do
        } else if (_format == PackCreationModel::ShowByQueue) {
            rootItem = new QStandardItem(tkTr(Trans::Constants::_1_COLON_2)
                                                .arg(tkTr(Trans::Constants::QUEUE))
                                                .arg(queue.sourceAbsolutePathFile()));
            rootItem->setToolTip(queue.sourceAbsolutePathFile());
            rootItem->setFont(bold);
            rootItem->setCheckable(true);
            rootItem->setCheckState(Qt::Checked);
            q->invisibleRootItem()->appendRow(rootItem);
            insertPackCreationQueueInCache(queue, rootItem);
        } else {
            LOG_ERROR_FOR(q, "Format not supported");
            return false;
        }

        // Get the content of the queue and create the tree for each pack
        QHash<QString, QStandardItem *> serversUidToItem;
        foreach(const RequestedPackCreation &request, queue.queue()) {
            if (_packDescriptionFilesIncluded.contains(request.descriptionFilePath))
                continue;
            _packDescriptionFilesIncluded.append(request.descriptionFilePath);

            // Get the serverUid root item
            QStandardItem *server = 0;
            if (_format == PackCreationModel::ShowByQueue) {
                server = serversUidToItem.value(request.serverUid, 0);
            } else if (_format == PackCreationModel::ShowByServer) {
                server = serverItem(request.serverUid);
            }

            // If the server is not available yet: create it
            if (!server) {
                server = new QStandardItem(tkTr(Trans::Constants::_1_COLON_2)
                                           .arg(tkTr(Trans::Constants::SERVER))
                                           .arg(request.serverUid));
                if (_format == PackCreationModel::ShowByQueue) {
                    // In this format
                    // append the server item in the cache and the queueItem
                    serversUidToItem.insert(request.serverUid, server);
                    server->setCheckable(true);
                    server->setCheckState(Qt::Checked);
                    server->setFont(bold);
                    rootItem->appendRow(server);
                } else if (_format == PackCreationModel::ShowByServer) {
                    // In this format
                    // append the server item in the central cache and the rootItem
                    addServerItem(request.serverUid, server);
                    rootItem = server;
                    rootItem->setFont(bold);
                    rootItem->setCheckable(true);
                    rootItem->setCheckState(Qt::Checked);
                    q->invisibleRootItem()->appendRow(rootItem);
                }
            }

            // Include datapack to the server item
            server->appendRow(packToItem(request.descriptionFilePath, queue));
        }

        return true;
    }

    // Find all PackCreationQueue XML files inside a directory
    // and create model branches
    bool screenPath(const QString &absPath)
    {
        // Path already screened in model?
        if (_screenedAbsPath.contains(absPath))
            return true;
        _screenedAbsPath.append(absPath);

        // Find all packcreationqueue-xmlfiles (recursively)
        QFileInfoList files = Utils::getFiles(QDir(absPath), QString(Constants::PACKCREATIONQUEUE_DEFAULT_FILENAME));

        // Add all packcreationqueue to the model
        foreach(const QFileInfo &info, files) {
            // Read the Queue file
            PackCreationQueue queue;
            if (!queue.fromXmlFile(info.absoluteFilePath())) {
                LOG_ERROR_FOR(q, tkTr(Trans::Constants::FILE_1_ISNOT_READABLE).arg(info.absoluteFilePath()));
                continue;
            }

            // Create the queue branch
            if (!packCreationQueueToItem(queue)) {
                LOG_ERROR_FOR(q, QString("Unable to create the queue branch: %1").arg(info.absoluteFilePath()));
                continue;
            }
            _queues << queue;
        }
        return true;
    }

public:
    QHash<QString, QStandardItem *> _serversUidToItem; //, _screeningPathToItem;
    QHash<QString, QStandardItem *> _insertedPackCreationQueueUids;  // Key: queue.uid()
    QHash<QString, QStandardItem *> _packItems; // Key: absPathDescriptionFile
    QList<PackCreationQueue> _queues;

    QStringList _screenedAbsPath;
    QStringList _packDescriptionFilesIncluded;
    PackCreationModel::Format _format;

private:
    PackCreationModel *q;
};
} // namespace Internal
} // namespace DataPack

/** Ctor */
PackCreationModel::PackCreationModel(QObject *parent) :
    QStandardItemModel(parent),
    d(new Internal::PackCreationModelPrivate(this))
{
    d->initialize();

}

/** Dtor */
PackCreationModel::~PackCreationModel()
{
    if (d)
        delete d;
    d = 0;
}

/**
 * Clear the model content, you should use the QStandardItemModel::clear()
 * to  clear this model or you expose your code to segfaulting.
 */
void PackCreationModel::clearPackModel()
{
    d->clearModelAndCache();
}

/**
 * Set the tree model format. By default, set to ShowByQueue
 * \sa Format
 */
void PackCreationModel::setFormat(Format format)
{
    d->_format = format;
    // Recreate the whole tree
    d->clearModelAndCache();
    foreach(const QString &path, d->_screenedAbsPath)
        d->screenPath(path);
}

/**
 * Get read acces to the model contents. Also manages Qt::CheckStateRole
 */
QVariant PackCreationModel::data(const QModelIndex &index, int role) const
{
    return QStandardItemModel::data(index, role);
}

/**
 * Apply same checkstate to all children of the branch
 */
bool PackCreationModel::setCheckStateRoleToItemAndChildren(const QModelIndex &parent, const QVariant &value)
{
    if (!itemFromIndex(parent)->isCheckable())
        return false;

    if (!QStandardItemModel::setData(parent, value, Qt::CheckStateRole))
        return false;

    for(int i=0; i < rowCount(parent); ++i) {
        QModelIndex child = this->index(i, 0, parent);
        if (!setCheckStateRoleToItemAndChildren(child, value))
            continue;
    }
    return true;
}

/**
 * Apply same checkstate if Qt::Checked to all parents of the index
 */
bool PackCreationModel::setCheckedToAllParents(const QModelIndex &index)
{
    if (!index.parent().isValid())
        return false;

    if (!itemFromIndex(index)->isCheckable())
        return false;

    if (QStandardItemModel::setData(index.parent(), Qt::Checked, Qt::CheckStateRole))
        return setCheckedToAllParents(index.parent());

    return true;
}

/**
 * Only manages Qt::CheckStateRole.
 */
bool PackCreationModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::CheckStateRole) {
        bool children = setCheckStateRoleToItemAndChildren(index, value);
        bool parents = true;
        if (value.toInt() == Qt::Checked) {
            parents = setCheckedToAllParents(index);
        }
        return children & parents;
    }
    return false;
}

//QVariant PackCreationModel::headerData(int section, Qt::Orientation orientation, int role) const
//{
//    return QVariant();
//}

/** Add a queue content using a DataPack::PackCreationQueue \e queue */
bool PackCreationModel::addPackCreationQueue(const PackCreationQueue &queue)
{
    return d->packCreationQueueToItem(queue);
}

/** Add a queue XML file */
bool PackCreationModel::addScreeningPath(const QString &screeningAbsPath)
{
    return d->screenPath(screeningAbsPath);
}

/** Returns the number of requested Packs found in all Queues */
int PackCreationModel::totalNumberOfPacksFound() const
{
    return d->_packItems.count();
}

/**
 * Returns all checked pack description file absolute path.\n
 * If you want to create packs, please be sure to create the pack
 * content using the DataPack::PackCreationQueue::createZippedContent()
 * before.
 */
QStringList PackCreationModel::getCheckedPacks() const
{
    QStringList list;
    QHashIterator<QString, QStandardItem *> i(d->_packItems);
    while (i.hasNext()) {
        i.next();
        if (i.value()->checkState() == Qt::Checked)
            list << i.key();
    }
    return list;
}

/**
 * Creates a PackCreationQueue with all the selected Packs.
 * \sa PackServerCreator::addPackCreationQueue()
 * \sa PackCreationQueue::createZippedContent()
 */
PackCreationQueue &PackCreationModel::generateQueueForServerCreation() const
{
    PackCreationQueue *internalQueue = new PackCreationQueue;
    foreach(const QString &packDescPath, getCheckedPacks()) {
        // Find the related Request inside all queues of the model
        foreach(const PackCreationQueue &queue, d->_queues) {
            foreach(const RequestedPackCreation &request, queue.queue()) {
                if (request.descriptionFilePath == packDescPath) {
                    if (!internalQueue->addToQueue(request)) {
                        LOG_ERROR("unable to add request to queue");
                    }
                    break;
                }
            }
        }
    }
    return *internalQueue;
}
