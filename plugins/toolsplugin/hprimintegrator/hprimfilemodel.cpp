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
 *  along with this program (COPYING file).                   *
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
 * Inherits QFileSystemModel. The model is automatically connected to the
 * HPRIM path extracted from the user's settings. \n
 * It presents HPRIM files with the following data:
 * - related patient name
 * - related patient date of birth
 */

#include "hprimfilemodel.h"
#include "constants.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/hprimparser.h>
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

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
//static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
//static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}

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
    QString xmlFileIndexAbsFilePath(const QString &absPath)
    {
        return QString("%1/%2").arg(absPath).arg(::XML_INDEX_FILENAME);
    }

    // Simply checks if the index exists in the path
    bool xmlIndexExists(const QString &absPath)
    {
        return QFileInfo(xmlFileIndexAbsFilePath(absPath)).exists();
    }

    // Read the content of the xml index file & populate object vars.
    bool readXmlIndex(const QString &absPath)
    {
        if (!xmlIndexExists(absPath))
            return false;
        QDomDocument doc;
        QString error;
        int line = 0, col = 0;
        if (!doc.setContent(Utils::readTextFile(xmlFileIndexAbsFilePath(absPath), Utils::DontWarnUser), &error, &line, &col)) {
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
                                       xmlFileIndexAbsFilePath(absPath), Utils::Overwrite, Utils::DontWarnUser);
    }

    // Read all dir file content and cache patient names
    void readPatientName(const QString &absPath)
    {
        _fileNameToPatientName.clear();
        _fileNameToPatientDob.clear();
        foreach(const QFileInfo &info, Utils::getFiles(QDir(absPath))) {
            QString content = readFileContent(info.absoluteFilePath());
            // CR only
            if (content.contains("\r") && !content.contains("\n"))
                content = content.replace("\r", "\n");
            // Try to read HPRIM header
            Utils::HPRIM::HprimMessage msg = Utils::HPRIM::parseHprimRawSource(content);
            if (msg.isValid() && msg.header().isValid()) {
                _fileNameToPatientName.insert(info.absoluteFilePath(), QString("%1 %2").arg(msg.header().patientName()).arg(msg.header().patientFirstName()));
                _fileNameToPatientDob.insert(info.absoluteFilePath(), QLocale().toString(msg.header().patientDateOfBirth(), QLocale::LongFormat));
            }
        }
    }

    // Read the content of the currently selected file
    QString readFileContent(const QString absPath)
    {
        QString encoding;
        switch (settings()->value(Constants::S_DEFAULT_FILE_ENCODING).toInt()) {
        case Constants::AutoDetect:
            if (Utils::isRunningOnLinux())
                encoding = "UTF-8";
            else if (Utils::isRunningOnMac())
                encoding = "MacRoman";
            else if (Utils::isRunningOnWin())
                encoding = "ISO-8859-1";
            break;
        case Constants::ForceUtf8: encoding = "UTF-8"; break;
        case Constants::ForceMacRoman: encoding = "MacRoman"; break;
        case Constants::ForceIso8859_1: encoding = "ISO-8859-1"; break;
        }
        QString content = Utils::readTextFile(absPath);
        // Correct wrong encoding
        return Utils::correctTextAccentEncoding(content);
    }

    QString patientName(const QModelIndex &fileSystemModel)
    {
        return _fileNameToPatientName.value(_fileModel->fileInfo(fileSystemModel).absoluteFilePath());
    }

    QString patientDob(const QModelIndex &fileSystemModel)
    {
        return _fileNameToPatientDob.value(_fileModel->fileInfo(fileSystemModel).absoluteFilePath());
    }

public:
    QFileSystemModel *_fileModel;
    QList<File> _files;
    QPersistentModelIndex _rootPathIndex;
    QHash<QString, QString> _fileNameToPatientName, _fileNameToPatientDob;
private:
    HprimFileModel *q;
};
} // namespace Internal
} // end namespace Tools

// Default path of the plugin
static inline QString defaultPath()
{
    return QString("%1/%2")
            .arg(settings()->path(Core::ISettings::UserDocumentsPath))
            .arg("HPRIM/Received");
}

/*! Constructor of the Tools::Internal::HprimFileModel class */
HprimFileModel::HprimFileModel(QObject *parent) :
    QSortFilterProxyModel(parent),
    d(new HprimFileModelPrivate(this))
{
    setDynamicSortFilter(true);

    // FIXME: using full path in settings is not crossplatform:
    // Create a patient db under macos, set HPRIM preferences
    //      default is /Users/name/freehealth/Documents/Hprim
    // Go to Linux with the same database -> Error because under Linux
    //      default is /home/name/freehealth/Documents/Hprim
    // The fix -> considere usage of relatives path inside the
    //      Core::ISettings::UserDocumentsPath

    QString path = settings()->value(Constants::S_PATH_TO_SCAN).toString();

    // No path defined in settings -> use default path
    if (path.isEmpty() || (path == qApp->applicationDirPath()))
        path = defaultPath();

    // Ensure that the HPRIM path to scan exists
    if (!QDir(path).exists()) {
        if (!QDir().mkpath(path)) {
            // Path does not exist && can not be created -> Fallback to default path
            path = defaultPath();
            if (!QDir(path).exists()) {
                // Path does not exist && can not be created -> Error
                if (!QDir().mkpath(path)) {
                    LOG_ERROR(tkTr(Trans::Constants::PATH_1_CANNOT_BE_CREATED).arg(path));
                    path.clear();
                }
            }
        }
    }

    // Create the FileSystemModel
    if (!path.isEmpty()) {
        d->_fileModel = new QFileSystemModel(this);
        d->_fileModel->setReadOnly(true);
        d->_fileModel->setResolveSymlinks(false);
        d->_fileModel->setFilter(QDir::Files);
        d->_rootPathIndex = d->_fileModel->setRootPath(path);
        setSourceModel(d->_fileModel);
        connect(d->_fileModel, SIGNAL(directoryLoaded(QString)), this, SLOT(_onDirectoryLoaded(QString)));
    }
}

/*! Destructor of the Tools::Internal::HprimFileModel class */
HprimFileModel::~HprimFileModel()
{
    if (d)
        delete d;
    d = 0;
}

/** Define the path to scan for HPRIM files */
bool HprimFileModel::setRootPath(const QString &path)
{
    d->_rootPathIndex = d->_fileModel->setRootPath(path);
    return d->_rootPathIndex.isValid();
}

/** Returns the scanned root path index */
QModelIndex HprimFileModel::fileRootPath() const
{
    return mapFromSource(d->_rootPathIndex);
}

QVariant HprimFileModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    QModelIndex source = mapToSource(index);
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case PatientName: return d->patientName(source);
        case PatientDateOfBirth: return d->patientDob(source);
        case FileName:
        {
            source = d->_fileModel->index(source.row(), 0, source.parent());
            return d->_fileModel->fileInfo(source).fileName();
        }
        case FileDate: return QLocale().toString(d->_fileModel->lastModified(source), QLocale::ShortFormat);
        }

    }
    return QVariant();
    // return sourceModel()->data(source, role);
}

QVariant HprimFileModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            switch (section) {
            case PatientName: return tr("Patient name");
            case PatientDateOfBirth: return tr("Patient date of birth");
            case FileName: return tr("File name");
            case FileDate: return tr("Date");
            }
        } else {
            return section+1;
        }
    }
    return QVariant();
}

/** Returns the QFileInfo corresponding to the QModelIndex \e index of the model */
QFileInfo HprimFileModel::fileInfo(const QModelIndex &index) const
{
    QModelIndex source = mapToSource(index);
    return d->_fileModel->fileInfo(source);
}

/** Returns the content of the \e index corresponding file */
QString HprimFileModel::fileContent(const QModelIndex &index) const
{
    QModelIndex source = mapToSource(index);
    source = d->_fileModel->index(source.row(), 0, source.parent());
    return d->readFileContent(d->_fileModel->fileInfo(source).absoluteFilePath());
}

Qt::ItemFlags HprimFileModel::flags(const QModelIndex &) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void HprimFileModel::_onDirectoryLoaded(const QString &absPath)
{
    // Populate the QFileSystemModel with extra data
    d->readPatientName(absPath);
}
