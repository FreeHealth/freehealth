/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 * \class Tools::Internal::HprimFileModel
 */

#include "hprimfilemodel.h"

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_filepathxml.h>
#include <translationutils/trans_msgerror.h>

#include <QDomDocument>
#include <QDateTime>
#include <QFileInfo>

#include <QDebug>

using namespace Tools;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace {
const char * const XML_INDEX_FILENAME = "fileindex.xml";

const char * const XML_ROOT_TAG = "FileIndexer";
const char * const XML_TAG_FILE = "File";
const char * const XML_TAG_READER = "Reader";

const char * const XML_ATTRIB_NAME = "name";
const char * const XML_ATTRIB_UID = "uid";
const char * const XML_ATTRIB_READDATETIME = "readDate";
const char * const XML_ATTRIB_SHA1 = "sha1";
const char * const XML_ATTRIB_ISINTEGRATED = "integrated";
const char * const XML_ATTRIB_INTEGRATIONDATETIME = "integrationDateTime";
}

namespace Tools {
namespace Internal {
struct UserReader {
    QString uid, fullName;
    QDateTime readDateTime;
};

struct File {
    File() : integratedIntoPatientFile(false) {}

    QString absFilePath, sha1;
    int integratedIntoPatientFile;
    QDateTime integrationDateTime;
    QList<UserReader> readers;
};


class HprimFileModelPrivate
{
public:
    HprimFileModelPrivate(HprimFileModel *parent) :
        _fileModel(0),
        q(parent)
    {
    }
    
    ~HprimFileModelPrivate()
    {
    }
    
    // Returns the absFilePath of the xml index for the specified path
    QString xmlFileIndexAbsFiePath(const QString &absPath)
    {
        return QString("%1/%2").arg(absPath).arg(::XML_INDEX_FILENAME);
    }

    // Simply checks if the index exists in the path
    bool xmlIndexExists(const QString &absPath)
    {
        return QFileInfo(xmlFileIndexAbsFiePath(absPath)).exists();
    }

    // Read the content of the xml index file & populate object vars.
    bool readXmlIndex(const QString &absPath)
    {
        if (!xmlIndexExists(absPath))
            return false;
        QDomDocument doc;
        QString error;
        int line = 0, col = 0;
        if (!doc.setContent(Utils::readTextFile(xmlFileIndexAbsFiePath(absPath), Utils::DontWarnUser), &error, &line, &col)) {
            LOG_ERROR_FOR(q, tkTr(Trans::Constants::ERROR_1_LINE_2_COLUMN_3).arg(error).arg(line).arg(col));
            return false;
        }
        QDomElement root = doc.documentElement();
        root = root.firstChildElement(::XML_ROOT_TAG);
        QDomElement element = root.firstChildElement(::XML_TAG_FILE);
        while (!element.isNull()) {
            File file;
            file.absFilePath = element.attribute(::XML_ATTRIB_NAME);
            file.sha1 = element.attribute(::XML_ATTRIB_SHA1);
            file.integratedIntoPatientFile = element.attribute(::XML_ATTRIB_ISINTEGRATED).toInt();
            file.integrationDateTime = QDateTime::fromString(element.attribute(::XML_ATTRIB_INTEGRATIONDATETIME), Qt::ISODate);
            QDomElement reader = element.firstChildElement(::XML_TAG_READER);
            while (!element.isNull()) {
                UserReader r;
                r.uid = reader.attribute(::XML_ATTRIB_UID);
                r.fullName = reader.attribute(::XML_ATTRIB_NAME);
                r.readDateTime = QDateTime::fromString(reader.attribute(::XML_ATTRIB_READDATETIME), Qt::ISODate);
                file.readers.append(r);
                reader = reader.nextSiblingElement(::XML_TAG_READER);
            }
            _files.append(file);
            element = element.nextSiblingElement(::XML_TAG_FILE);
        }
        return true;
    }

    // Save the file list into the xml index file
    bool saveXmlIndex(const QString &absPath)
    {
        QDomDocument doc("FreeMedForms");
        QDomComment comment = doc.createComment(
                    "This file is generated by FreeMedForms. It contains the file index of the\n"
                    "current path. Please do not edit by-hand.");
        doc.appendChild(comment);
        QDomElement root = doc.createElement(::XML_ROOT_TAG);
        doc.appendChild(root);
        foreach(const File &file, _files) {
            QDomElement e = doc.createElement(::XML_TAG_FILE);
            root.appendChild(e);
            e.setAttribute(::XML_ATTRIB_NAME, file.absFilePath);
            e.setAttribute(::XML_ATTRIB_SHA1, file.sha1);
            e.setAttribute(::XML_ATTRIB_ISINTEGRATED, int(file.integratedIntoPatientFile));
            e.setAttribute(::XML_ATTRIB_INTEGRATIONDATETIME, file.integrationDateTime.toString(Qt::ISODate));
            foreach(const UserReader &reader, file.readers) {
                QDomElement u = doc.createElement(::XML_TAG_READER);
                e.appendChild(u);
                u.setAttribute(::XML_ATTRIB_UID, reader.uid);
                u.setAttribute(::XML_ATTRIB_NAME, reader.fullName);
                u.setAttribute(::XML_ATTRIB_READDATETIME, reader.readDateTime.toString(Qt::ISODate));
            }
        }
        return Utils::saveStringToFile(QString("<?xml version='1.0' encoding='UTF-8'?>\n" + doc.toString(2)),
                                       xmlFileIndexAbsFiePath(absPath), Utils::Overwrite, Utils::DontWarnUser);
    }

public:
    QFileSystemModel *_fileModel;
    QList<File> _files;

private:
    HprimFileModel *q;
};
} // namespace Internal
} // end namespace Tools


/*! Constructor of the Tools::Internal::HprimFileModel class */
HprimFileModel::HprimFileModel(QObject *parent) :
    QAbstractTableModel(parent),
    d(new HprimFileModelPrivate(this))
{
}

/*! Destructor of the Tools::Internal::HprimFileModel class */
HprimFileModel::~HprimFileModel()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool HprimFileModel::initialize(QFileSystemModel *model)
{
    d->_fileModel = model;
    connect(d->_fileModel, SIGNAL(directoryLoaded(QString)), this, SLOT(_onDirectoryLoaded(QString)));
//    connect(d->_fileModel, SIGNAL(rootPathChanged(QString)), this, SLOT(_onRootPathChanged(QString)));
    // Utils::linkSignalsFromFirstModelToSecondModel(d->_fileModel, this, true);
    return true;
}

void HprimFileModel::_onDirectoryLoaded(const QString &absPath)
{
    // Check for the internal XML content file
    if (!d->xmlIndexExists(absPath))
        return;

    // If xml index exists read it
    d->readXmlIndex(absPath);

    // Populate the QFileSystemModel with extra data

}
