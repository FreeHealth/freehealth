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
 *   Main Developers:                                                     *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "pack.h"
#include "datapackcore.h"

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>
#include <translationutils/trans_drugs.h>
#include <translationutils/trans_msgerror.h>

#include <QDomDocument>
#include <QDomElement>
#include <QDir>

using namespace DataPack;
using namespace Trans::ConstantTranslations;

static inline DataPack::DataPackCore &core() {return DataPack::DataPackCore::instance();}

namespace {
const char *const TAG_ROOT = "DataPack_Pack";
const char *const TAG_PACKDESCRIPTION = "PackDescription";
const char *const TAG_PACKDEPENDENCIES = "PackDependencies";
}

Pack::Pack() :
//    m_Sha1Checked(false), m_Md5Checked(false),
    m_type(-1)
{
}

Pack::~Pack()
{
}

bool Pack::isValid() const
{
    return (!uuid().isEmpty() &&
            !version().isEmpty() &&
            !m_descr.data(PackDescription::Label).toString().isEmpty());
}

QString Pack::uuid() const
{
    return m_descr.data(PackDescription::Uuid).toString();
}

QString Pack::version() const
{
    return m_descr.data(PackDescription::Version).toString();
}

QString Pack::name() const
{
    return m_descr.data(PackDescription::Label).toString();
}

QString Pack::vendor() const
{
    const QString &v = m_descr.data(PackDescription::Vendor).toString();
    if (v.isEmpty())
        return tkTr(Trans::Constants::THE_FREEMEDFORMS_COMMUNITY);
    return v;
}

/** Return the filename of the datapack content file (mainly the zip file to unzip and install) */
QString Pack::serverFileName() const
{
    if (m_descr.data(PackDescription::AbsFileName).toString().isEmpty())
        LOG_ERROR_FOR("Pack", "Pack does not have a defined server filename. Xml tag 'file' missing");
    return m_descr.data(PackDescription::AbsFileName).toString();
}

/** Return the server filename that contains the license terms */
QString Pack::serverLicenseFileName() const
{
//    return m_descr.data(PackDescription::LicenseFileName).toString();
    return QString();
}

/** Return the expected MD5 checksum of the datapack file content */
QString Pack::md5ControlChecksum() const
{
    return m_descr.data(PackDescription::Md5).toString();
}

/** Return the expected SHA1 checksum of the datapack file content */
QString Pack::sha1ControlChecksum() const
{
    return m_descr.data(PackDescription::Sha1).toString();
}

/** Not implemented */
bool Pack::isSha1Checked() const
{
    // TODO
    // Une fois le fichier télécharger, vérifie la sha1 locale avec celle donnée par le serveur
    return false;
}

/** Not implemented */
bool Pack::isMd5Checked() const
{
    // TODO
    // Une fois le fichier télécharger, vérifie la md5 locale avec celle donnée par le serveur
    return false;
}

/** Return the original file name of the pack XML config file. This file name is only valid on local servers. */
QString Pack::originalXmlConfigFileName() const
{
    return m_OriginalFileName;
}

/** Return the persistentCached file name of the pack XML config file. This file name is computed using the DataPack::DataPackCore::persistentCachePath(). */
QString Pack::persistentlyCachedXmlConfigFileName() const
{
    return core().persistentCachePath() + QDir::separator() + uuid() + QDir::separator() + "packconfig.xml";
}

/** Return the persistentCached file name of the zipped pack file. This file name is computed using the DataPack::DataPackCore::persistentCachePath(). */
QString Pack::persistentlyCachedZipFileName() const
{
    return core().persistentCachePath() + QDir::separator() + uuid() + QDir::separator() + QFileInfo(serverFileName()).fileName();
}

/**
 * Return the path where to unzip the pack zipped file.
 * This path is computed using the DataPack::DataPackCore::installPath().
 * If the DataPack::Pack::PackDescription::UnzipToPath contains path tags, these are
 * replaced using the DataPack::DataPackCore::replacePathTag().
 * \sa DataPack::DataPackCore::replacePathTag(), DataPack::DataPackCore::registerPathTag()
*/
QString Pack::unzipPackToPath() const
{
    QString zipPath = m_descr.data(PackDescription::UnzipToPath).toString();
    // contains a registered tag ?
    if (core().containsPathTag(zipPath))
        zipPath = core().replacePathTag(zipPath);
    else
        zipPath.prepend(core().installPath() + QDir::separator());
    return zipPath;
}

/**
 * Return the installed file name of the pack XML config file.
 * This file name is computed using the DataPack::DataPackCore::installPath().
*/
QString Pack::installedXmlConfigFileName() const
{
    return core().installPath() + QDir::separator() + "packconfig" + QDir::separator() + uuid() + QDir::separator() + "packconfig.xml";
}

/** Set installed files (usually after unzipping pack file). \sa DataPack::IPackManager::installDownloadedPack */
void Pack::setInstalledFiles(const QStringList &list)
{
    m_descr.setData(PackDescription::InstalledFiles, list.join("@@"));
}

/** Return installed files (set usually after unzipping pack file). \sa DataPack::IPackManager::installDownloadedPack */
QStringList Pack::installedFiles() const
{
    const QString &inst = m_descr.data(PackDescription::InstalledFiles).toString();
    QStringList draft = inst.split("@@");
    draft.removeAll("");
    QStringList list;
    foreach(QString s, draft) {
        s.prepend(unzipPackToPath() + QDir::separator());
        list << s;
    }
    return list;
}

/** Return the DataPack::Pack::DataType of the package according to its description. */
Pack::DataType Pack::dataType() const
{
    if (m_type!=-1)
        return Pack::DataType(m_type);
    const QString &type = m_descr.data(PackDescription::DataType).toString();
    if (type.compare("FormsFullSet", Qt::CaseInsensitive)==0)
        m_type = Pack::FormSubset;
    else if (type.compare("SubForms", Qt::CaseInsensitive)==0)
        m_type = Pack::SubForms;
    else if (type.compare("DrugsWithInteractions", Qt::CaseInsensitive)==0)
        m_type = Pack::DrugsWithInteractions;
    else if (type.compare("DrugsWithoutInteractions", Qt::CaseInsensitive)==0)
        m_type = Pack::DrugsWithoutInteractions;
    else if (type.compare("icd", Qt::CaseInsensitive)==0)
        m_type = Pack::ICD;
    else if (type.compare("ZipCodes", Qt::CaseInsensitive)==0)
        m_type = Pack::ZipCodes;
    else if (type.compare("UserDocuments", Qt::CaseInsensitive)==0)
        m_type = Pack::UserDocuments;
    else if (type.compare("Accountancy", Qt::CaseInsensitive)==0)
        m_type = Pack::Accountancy;
    else if (type.compare("Account", Qt::CaseInsensitive)==0)
        m_type = Pack::Accountancy;
    else if (type.compare("AlertPack", Qt::CaseInsensitive)==0
             || type.compare("AlertPacks", Qt::CaseInsensitive)==0)
        m_type = Pack::AlertPacks;
    else if (type.compare("Binaries", Qt::CaseInsensitive)==0)
        m_type = Pack::Binaries;
    else
        m_type = Pack::UnknownType;
    return Pack::DataType(m_type);
}

/** Return the DataPack::Pack::DataType name of the package according to its description. */
QString Pack::dataTypeName() const
{
    Pack::DataType type = dataType();
    switch (type) {
    case Pack::FormSubset: return tkTr(Trans::Constants::FORMS);
    case Pack::SubForms: return tkTr(Trans::Constants::SUBFORMS);
    case Pack::DrugsWithInteractions: return tkTr(Trans::Constants::DRUGS_WITH_INTERACTIONS);
    case Pack::DrugsWithoutInteractions: return tkTr(Trans::Constants::DRUGS_WITHOUT_INTERACTIONS);
    case Pack::ICD: return tkTr(Trans::Constants::ICD10);
    case Pack::ZipCodes: return tkTr(Trans::Constants::ZIP_CODES);
    case Pack::UserDocuments: return tkTr(Trans::Constants::USER_DOCUMENTS);
    case Pack::AlertPacks: return tkTr(Trans::Constants::ALERT_PACKS);
    case Pack::Binaries: return tkTr(Trans::Constants::BINARY_PACKS);
    default: return tkTr(Trans::Constants::UNKNOWN);
    }
    return tkTr(Trans::Constants::UNKNOWN);
}

/**
 * Reads the XML configuration file of the pack and
 * create the DataPack::PackDescription and the DataPack::PackDependencies
 * related to this pack.
*/
void Pack::fromXmlFile(const QString &absFileName)
{
    m_OriginalFileName = absFileName;
//    qWarning() << "PackFromXml" << absFileName;
    fromXml(Utils::readTextFile(absFileName, Utils::DontWarnUser));
}

/**
 * Reads the XML configuration content of the pack and
 * create the DataPack::PackDescription and the DataPack::PackDependencies
 * related to this pack.
*/
void Pack::fromXml(const QString &fullPackConfigXml)
{
    QDomDocument doc;
    QString error;
    int line, col;
    if (!doc.setContent(fullPackConfigXml, &error, &line, &col)) {
        LOG_ERROR_FOR("DataPack::Pack", QString("Wrong XML: (%1:%2) %3").arg(line).arg(col).arg(error));
        return;
    }
    QDomElement root = doc.firstChildElement(::TAG_ROOT);
    QDomElement descr = root.firstChildElement(::TAG_PACKDESCRIPTION);
    QDomElement dep = root.firstChildElement(::TAG_PACKDEPENDENCIES);
    m_descr.fromDomElement(descr);
    m_depends.fromDomElement(dep);
}

/** Return the XML code corresponding to this pack */
QString Pack::toXml() const
{
    QDomDocument doc("FreeMedForms");
    QDomElement element = doc.createElement(::TAG_ROOT);
    doc.appendChild(element);
    if (!m_descr.toDomElement(&element, &doc))
        LOG_ERROR_FOR("Pack", "Unable to write PackDescription XML content to QDomDocument");
    if (!m_depends.toDomElement(&element, &doc))
        LOG_ERROR_FOR("Pack", "Unable to write PackDependencies XML content to QDomDocument");
    return QString("<?xml version='1.0' encoding='UTF-8'?>\n" + doc.toString(2));
}

/** Check equality between two Pack */
bool Pack::operator==(const Pack &other) const
{
    return (this->uuid()==other.uuid() &&
            this->version()==other.version() &&
            this->vendor()==other.vendor() &&
            this->name()==other.name());
}

QDebug operator<<(QDebug dbg, const DataPack::Pack &p)
{
    dbg.nospace() << "Pack("
                  << p.uuid()
                  << ", "
                  << p.version()
                  << ", "
                  << p.serverFileName()
                  << ")";
    return dbg.space();
}


