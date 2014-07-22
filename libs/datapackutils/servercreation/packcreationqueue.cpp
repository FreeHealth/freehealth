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
#include "packcreationqueue.h"
#include <datapackutils/pack.h>
#include <datapackutils/servercontent.h>
#include <datapackutils/constants.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_msgerror.h>
#include <quazip/JlCompress.h>
#include <quazip/global.h>

#include <QString>
#include <QDir>
#include <QFile>
#include <QDomDocument>

enum {
    WarnRequestChecking = false,
    DebugEqualityOperator = false
};

using namespace DataPack;
using namespace Trans::ConstantTranslations;

/**
 * \class DataPack::PackCreationQueue
 * Keeps information about how to create a DataPack::Pack (description file, content,
 * server owner).
 * Information can be stored as an XML file. \n
 *
 * Pack stored information are:
 * - datapack XML description absolute/relative file path
 * - datapack server (which server owns the datapack)
 * - datapack content (zipped file, file or dir)
 *
 * DataPack::Pack creation members: \n
 * The DataPack::PackCreationQueue can help you to automatically create
 * a serie of packs. You can set multiple requests (see DataPack::RequestedPackCreation)
 * and manage the DataPack::Pack content creation with createZippedContent(). \n
 * If you want to create a full server please refer to DataPack::PackCreationModel
 * documentation.
 *
 * \warning: Pack content must be defined relatively to pack description file and can not start with \b ../
 * \note: All path are internally stored as absolute path
 *
 * XML architecture:
  \code
<DataPackCreationQueue>
    <datapack description="absOrRelativePathToDescriptionFile" server="serverOwnerUid">
      <content type="{file_zipped,file_unzipped,dir}">./relative/path/to/content</content>
      <content type="{file_zipped,file_unzipped,dir}">relative/path/to/content</content>
    </datapack>
</DataPackCreationQueue>
  \endcode
 */

namespace {
const char * const XML_ROOT_TAG = "PackCreationQueue";
const char * const XML_DATAPACK_TAG = "datapack";
const char * const XML_DATAPACK_CONTENT_TAG = "content";
const char * const XML_DATAPACK_DESCRIPTION_ATTRIB = "description";
const char * const XML_DATAPACK_SERVER_ATTRIB = "server";
const char * const XML_CONTENT_TYPE_ATTRIB = "type";

const char * const XML_TYPE_FILE_ZIPPED = "file_zipped";
const char * const XML_TYPE_FILE_UNZIPPED = "file_unzipped";
const char * const XML_TYPE_DIR = "dir";

static QString xmlContentType(const int type)
{
    switch (type)
    {
    case DataPack::RequestedPackCreation::ZippedFile: return ::XML_TYPE_FILE_ZIPPED;
    case DataPack::RequestedPackCreation::UnzippedFile: return ::XML_TYPE_FILE_UNZIPPED;
    case DataPack::RequestedPackCreation::DirContent: return ::XML_TYPE_DIR;
    }
    return QString::null;
}
} // anonymous namespace

/**
 * Ctor. Each object owns a unique uid. This uid is not shared between object.
 * If you create a copy of a queue, the copy will not have the same uid.
 */
PackCreationQueue::PackCreationQueue()
{
    _uid = Utils::createUid();
}

/** Dtor */
PackCreationQueue::~PackCreationQueue()
{}

/** Checks the validity of a requested Pack Creation */
bool PackCreationQueue::checkValidity(const RequestedPackCreation &request) const
{
    // Description file exists
    if (!QFileInfo(request.descriptionFilePath).exists()) {
        if (WarnRequestChecking)
            qDebug() << "File does not exists:" << request.descriptionFilePath;
        return false;
    }

    // All contents exists
    foreach(int key, request.content.uniqueKeys()) {
        foreach(const QString &path, request.content.values(key)) {
            if (!QFileInfo(path).exists()) {
                if (WarnRequestChecking)
                    qDebug() << "File does not exists:" << path;
                return false;
            }
        }
    }
    return true;
}

/** Returns \e true if queue is empty */
bool PackCreationQueue::isEmpty() const
{
    return _queue.isEmpty();
}

/** Add a Pack creation request in the queue */
bool PackCreationQueue::addToQueue(const RequestedPackCreation &request)
{
    // Check validity of the request
    if (!checkValidity(request))
        return false;
    // TODO: check duplicates
    _queue << request;
    return true;
}

/** Add a PackCreationQueue to this object */
PackCreationQueue& PackCreationQueue::operator+=(const PackCreationQueue& add)
{
    foreach(const RequestedPackCreation &request, add.queue()) {
        // TODO: manage error when adding requests
        addToQueue(request);
    }
    return *this;
}
/**
 * \fn const QList<RequestedPackCreation> &PackCreationQueue::queue() const
 * Returns the current queue of the object
 */

/**
 * Returns \e true if the queue contains a reference to the
 * DataPack::Pack description XML file. Note that the \e absPath
 * \b must \b be \b an \b absolute \b path.
 */
bool PackCreationQueue::containsPackDescriptionFile(const QString &absPath) const
{
    foreach(const RequestedPackCreation &request, _queue) {
        if (request.descriptionFilePath == absPath)
            return true;
    }
    return false;
}

/**
 * Prepare the zipped file \e absZipFileName with all
 * the content of the datapack. \n
 * \warning the zipped file must not already exists,
 * otherwise will return \e false
 * \warning: Pack content must be defined relatively to pack description file and can not start with \b ../
 */
bool PackCreationQueue::createZippedContent(const RequestedPackCreation &request, const QString &absZipFileName) const
{
    if (absZipFileName.isEmpty()) {
        LOG_ERROR_FOR("PackCreationQueue", "Empty file name");
        return false;
    }
    if (QFileInfo(absZipFileName).exists()) {
        LOG_ERROR_FOR("PackCreationQueue", "Zip file already exists");
        return false;
    }
    if (!checkValidity(request)) {
        LOG_ERROR_FOR("PackCreationQueue", "Invalid request");
        return false;
    }

    // Move all content into a tmp path
    // Unzip if needed
    // Then zip the tmp path

    QString tmpPath = QString("%1/%2").arg(QDir::tempPath()).arg(Utils::createUid());
    if (!QDir().mkpath(tmpPath)) {
        LOG_ERROR_FOR("PackCreationQueue", QString("Unable to create path: %1").arg(tmpPath));
        return false;
    }
    int n = 0;

    // Copy Dir contents to tmp path
    foreach(const QString &path, request.content.values(RequestedPackCreation::DirContent)) {
        // Get relative path to pack description file of the content file
        if (!request.isRelativePathFromDescriptionPathValid(path)) {
            LOG_ERROR_FOR("PackCreationQueue", QString("Content file outside pack description dir tree: %1 / description: %2")
                          .arg(path)
                          .arg(request.descriptionFilePath));
            // Clean tmp path
            Utils::removeDirRecursively(tmpPath);
            return false;
        }

        // Copy dir content into the tmp path
        QString dest = QString("%1/%2").arg(tmpPath).arg(request.relativePathFromDescriptionPath(path));
        if (!Utils::copyDir(path, dest)) {
            LOG_ERROR_FOR("PackCreationQueue", QString("Unable to copy file: %1 to %2").arg(path).arg(dest));
            // Clean tmp path
            Utils::removeDirRecursively(tmpPath);
            return false;
        }
        ++n;
    }

    // Copy unzipped files to tmp path
    foreach(const QString &path, request.content.values(RequestedPackCreation::UnzippedFile)) {
        // Get relative path to pack description file of the content file
        if (!request.isRelativePathFromDescriptionPathValid(path)) {
            LOG_ERROR_FOR("PackCreationQueue", QString("Content file outside pack description dir tree: %1 / description: %2")
                          .arg(path)
                          .arg(request.descriptionFilePath));
            // Clean tmp path
            Utils::removeDirRecursively(tmpPath);
            return false;
        }

        // Copy file into the tmp path
        QString dest = QString("%1/%2").arg(tmpPath).arg(request.relativePathFromDescriptionPath(path));
        if (!QDir().mkpath(QFileInfo(dest).absolutePath())) {
            LOG_ERROR_FOR("PackCreationQueue", QString("Unable to create path: %1").arg(path));
            // Clean tmp path
            Utils::removeDirRecursively(tmpPath);
            return false;
        }

        if (!QFile::copy(path, dest)) {
            LOG_ERROR_FOR("PackCreationQueue", QString("Unable to copy file: %1 to %2").arg(path).arg(dest));
            // Clean tmp path
            Utils::removeDirRecursively(tmpPath);
            return false;
        }
        ++n;
    }

    // Zipped file (one specific case: request only contains one zipped file -> copy this file and return)
    if (n == 0) {
        if (request.content.values(RequestedPackCreation::ZippedFile).count() == 1) {
            // Just use the zipped file and return true
            return QFile(request.content.value(RequestedPackCreation::ZippedFile)).copy(absZipFileName);
        }
    }

    // Zipped file -> unzip everything into tmp dir
    foreach(const QString &path, request.content.values(RequestedPackCreation::ZippedFile)) {
        // Copy file into the tmp path
        if (!QuaZipTools::unzipFile(path, tmpPath))
            LOG_ERROR_FOR("PackCreationQueue", "Unable to unzip file: " + path);
        ++n;
    }

    // Compress the tmp dir with all its content
    if (!JlCompress::compressDir(absZipFileName, tmpPath, true)) {
        LOG_ERROR_FOR("PackCreationQueue", "Unable to compress dir: "+tmpPath);
        // Clean tmpPath
        Utils::removeDirRecursively(tmpPath);
        return false;
    }

    // Clean tmpPath
    if (!Utils::removeDirRecursively(tmpPath))
        LOG_ERROR_FOR("PackCreationQueue", QString("Unable to clean temp path: %1").arg(tmpPath));

    return true;
}

/**
 * If the Queue was created using a XML file (fromXmlFile()), returns
 * the absolute file path of the XML source file, otherwise
 * returns an empty QString
 */
QString PackCreationQueue::sourceAbsolutePathFile() const
{
    return _sourceAbsPath;
}

/**
 * Read a creation queue from an XML file.
 * \warning The current queue is cleared.
 */
bool PackCreationQueue::fromXmlFile(const QString &absFile)
{
    if (absFile.isEmpty() || !QFile(absFile).exists())
        return false;

    QString content = Utils::readTextFile(absFile, Utils::DontWarnUser);
    if (content.isEmpty())
        return false;

    QDomDocument doc;
    int line = 0;
    int col = 0;
    QString error;
    if (!doc.setContent(content, &error, &line, &col)) {
        LOG_ERROR_FOR("PackCreationQueue", tkTr(Trans::Constants::ERROR_1_LINE_2_COLUMN_3).arg(error).arg(line).arg(col));
        return false;
    }

    // Read XML
    QDomElement root = doc.documentElement();
    if (root.tagName().compare(::XML_ROOT_TAG, Qt::CaseInsensitive) != 0) {
        LOG_ERROR_FOR("PackCreationQueue", "Wrong root tag: " + root.tagName() + "; awaiting " + ::XML_ROOT_TAG);
        return false;
    }

    QDomElement packElement = root.firstChildElement(::XML_DATAPACK_TAG);
    while (!packElement.isNull()) {
        RequestedPackCreation request;

        // Get the Pack description file
        QString descrFile = packElement.attribute(::XML_DATAPACK_DESCRIPTION_ATTRIB);
        QFileInfo packDescrFile(descrFile);
        if (packDescrFile.isRelative())
            packDescrFile.setFile(QString("%1/%2").arg(QFileInfo(absFile).absolutePath()).arg(descrFile));
        if (!packDescrFile.exists()) {
            LOG_ERROR_FOR("PackCreationQueue", "Pack does not exists: " + packDescrFile.absoluteFilePath());
            packElement = packElement.nextSiblingElement(::XML_DATAPACK_TAG);
            continue;
        }
        request.descriptionFilePath = packDescrFile.absoluteFilePath();

        // Get server
        request.serverUid = packElement.attribute(::XML_DATAPACK_SERVER_ATTRIB);

        // Get all contents
        QDomElement content = packElement.firstChildElement(::XML_DATAPACK_CONTENT_TAG);
        while (!content.isNull()) {
            const QString &type = content.attribute(::XML_CONTENT_TYPE_ATTRIB);
            // Get path
            QString path = content.text();
            if (QDir(path).isRelative()) {
                path = QDir::cleanPath(QString("%1/%2")
                        .arg(QFileInfo(absFile).absolutePath())
                        .arg(path)
                                       );
            }

            // Get type
            if (type.compare(::XML_TYPE_DIR, Qt::CaseInsensitive) == 0) {
                request.content.insertMulti(RequestedPackCreation::DirContent, path);
            } else if (type.compare(::XML_TYPE_FILE_ZIPPED, Qt::CaseInsensitive) == 0) {
                request.content.insertMulti(RequestedPackCreation::ZippedFile, path);
            } else if (type.compare(::XML_TYPE_FILE_UNZIPPED, Qt::CaseInsensitive) == 0) {
                request.content.insertMulti(RequestedPackCreation::UnzippedFile, path);
            }

            // Next Pack content
            content = content.nextSiblingElement(::XML_DATAPACK_CONTENT_TAG);
        }

        // Next requested Pack sibling
        addToQueue(request);
        packElement = packElement.nextSiblingElement(::XML_DATAPACK_TAG);
    }
    _sourceAbsPath = QDir::cleanPath(absFile);
    return true;
}

/**
 * Save the queue to an XML file \e absPath.
 * Path of the datapack contents (file and dirs) can
 * stored as absolute path or relative path to the \e absFile. \n
 * This member also define the result of sourceAbsolutePathFile().
 */
bool PackCreationQueue::saveToXmlFile(const QString &absFile, bool useRelativePath) const
{
    QDomDocument doc("FreeMedForms");
    QDomElement root = doc.createElement(::XML_ROOT_TAG);
    doc.appendChild(root);

    // Append all packs
    foreach(const RequestedPackCreation &request, _queue) {
        QDomElement requestElement = doc.createElement(::XML_DATAPACK_TAG);
        root.appendChild(requestElement);
        if (useRelativePath) {
            QString rpath = QDir(QFileInfo(absFile).absolutePath()).relativeFilePath(request.descriptionFilePath);
            requestElement.setAttribute(::XML_DATAPACK_DESCRIPTION_ATTRIB, rpath);
        } else {
            requestElement.setAttribute(::XML_DATAPACK_DESCRIPTION_ATTRIB, request.descriptionFilePath);
        }
        requestElement.setAttribute(::XML_DATAPACK_SERVER_ATTRIB, request.serverUid);
        // Append all contents
        foreach(int key, request.content.uniqueKeys()) {
            foreach(const QString &path, request.content.values(key)) {
                QDomElement contentElement = doc.createElement(::XML_DATAPACK_CONTENT_TAG);
                requestElement.appendChild(contentElement);
                contentElement.setAttribute(::XML_CONTENT_TYPE_ATTRIB, xmlContentType(key));
                if (useRelativePath) {
                    QString rpath = request.relativePathFromDescriptionPath(path);
                    QDomText text = doc.createTextNode(rpath);
                    contentElement.appendChild(text);
                } else {
                    QDomText text = doc.createTextNode(path);
                    contentElement.appendChild(text);
                }
            }
        }
    }

    // Get XML content
    QString xml = QString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                          "%1").arg(doc.toString(2));

    // Save file and store the source file absolute path of this object
    if (Utils::saveStringToFile(xml, absFile, Utils::Overwrite, Utils::DontWarnUser)) {
        _sourceAbsPath = absFile;
        return true;
    }

    return false;
}

/** Returns a relative path of \e absPath from the pack description file path */
QString RequestedPackCreation::relativePathFromDescriptionPath(const QString &absPath) const
{
    return QDir(QFileInfo(this->descriptionFilePath).absolutePath()).relativeFilePath(absPath);

}

/**
 * Returns \e true if the relative path of \e absPath from the pack description file path
 * is valid (inside the pack description dir tree).
*/
bool RequestedPackCreation::isRelativePathFromDescriptionPathValid(const QString &absPath) const
{
    return (!relativePathFromDescriptionPath(absPath).contains("../"));
}

/** Checks equality between two RequestedPackCreation */
bool RequestedPackCreation::operator==(const RequestedPackCreation &other) const
{
    if (this->serverUid != other.serverUid) {
        if (DebugEqualityOperator)
            LOG_FOR("RequestedPackCreation", "serverUid mismatch");
        return false;
    }
    if (this->descriptionFilePath != other.descriptionFilePath) {
        if (DebugEqualityOperator)
            LOG_FOR("RequestedPackCreation", "descriptionFilePath mismatch");
        return false;
    }
    if (this->content != other.content) {
        if (DebugEqualityOperator)
            LOG_FOR("RequestedPackCreation", "content mismatch");
        return false;
    }
    return true;
}

/**
 * Checks equality between two PackCreationQueue. As all Queue must have a different
 * UUID, this operator does not take the uid into account but only compares the
 * queue content.
*/
bool PackCreationQueue::operator==(const PackCreationQueue &other) const
{
    if (this->_queue.count() != other._queue.count()) {
        if (DebugEqualityOperator)
            LOG_FOR("PackCreationQueue", "Pack count mismatch");
        return false;
    }
    foreach(const RequestedPackCreation &request, _queue) {
        if (!other._queue.contains(request)) {
            if (DebugEqualityOperator)
                LOG_FOR("PackCreationQueue", "request mismatch");
            return false;
        }
    }
    if (DebugEqualityOperator)
        LOG_FOR("PackCreationQueue", "operator==() returns true");
    return true;
}
