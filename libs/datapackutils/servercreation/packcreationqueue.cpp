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

/** --- XML Architecture ---
  <DataPackCreationQueue>
    <datapack description="" server="">
      <content type=""></content>
      <content type=""></content>
      <content type=""></content>
    </datapack>
  </DataPackCreationQueue>
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
}

PackCreationQueue::PackCreationQueue()
{
    _uid = Utils::createUid();
}

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

/**
 * \fn const QList<RequestedPackCreation> &PackCreationQueue::queue() const
 * Returns the current queue of the object
 */

/**
 * Prepare the zipped file \e absZipeFileName with all the content of the datapack. \n
 * \warning the zipped file must not already exists, otherwise will return \e false
 */
bool PackCreationQueue::createZippedContent(const RequestedPackCreation &request, const QString &absZipFileName)
{
    if (absZipFileName.isEmpty())
        return false;
    if (QFileInfo(absZipFileName).exists())
        return false;
    if (!checkValidity(request))
        return false;

    // Move all content into a tmp path
    // Unzip if needed
    // Then zip the tmp path

    QString tmpPath = QDir::tempPath() + Utils::createUid();
    if (!QDir().mkpath(tmpPath))
        return false;
    int n = 0;

    // Dir contents
    foreach(const QString &path, request.content.values(RequestedPackCreation::DirContent)) {
        // Copy dir content into the tmp path
        if (!Utils::copyDir(path, tmpPath)) {
            // Clean tmp path
            Utils::removeDirRecursively(tmpPath);
            return false;
        }
        ++n;
    }

    // Unzipped files
    foreach(const QString &path, request.content.values(RequestedPackCreation::UnzippedFile)) {
        // Copy file into the tmp path
        QString dest = tmpPath + path; // problem with relative path because we processed them
        if (!QFile::copy(path, dest)) {
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
    foreach(const QString &path, request.content.values(RequestedPackCreation::ZippedFile)) {
        // Copy file into the tmp path
        if (!QuaZipTools::unzipFile(path, tmpPath))
            LOG_ERROR_FOR("PackCreationQueue", "Unable to unzip file: " + path);
        ++n;
    }

    // Decompress the create dir with all its content
    if (!JlCompress::compressDir(absZipFileName, tmpPath, true)) {
        LOG_ERROR_FOR("PackCreationQueue", "Unable to compress dir: "+tmpPath);
        // Clean tmpPath
        Utils::removeDirRecursively(tmpPath);
        return false;
    }

    // Clean tmpPath
    Utils::removeDirRecursively(tmpPath);

    return true;
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
    return true;
}

/**
 * Save the queue to an XML file \e absPath.
 * Path of the datapack contents (file and dirs) can
 * stored as absolute path or relative path to the \e absFile.
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
                    QString rpath = QDir(QFileInfo(absFile).absolutePath()).relativeFilePath(path);
                    QDomText text = doc.createTextNode(rpath);
                    contentElement.appendChild(text);
                } else {
                    QDomText text = doc.createTextNode(path);
                    contentElement.appendChild(text);
                }
            }
        }
    }

    QString xml = QString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                          "%1").arg(doc.toString(2));
    return Utils::saveStringToFile(xml, absFile, Utils::Overwrite, Utils::DontWarnUser);
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
