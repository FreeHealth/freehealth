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
 *   Main Developers:                                                     *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DATAPACK_PACK_H
#define DATAPACK_PACK_H

#include <datapackutils/datapack_exporter.h>
#include <datapackutils/packdescription.h>

#include <QString>

namespace DataPack {

class DATAPACK_EXPORT Pack
{
public:
    enum DataType {
        NoType = 0,
        FormSubset,
        SubForms,
        DrugsWithoutInteractions,
        DrugsWithInteractions,
        ICD,
        ZipCodes,
        UserDocuments,
        Accountancy,
        AlertPacks,
        Binaries,
        UnknownType
    };

    Pack();
    virtual ~Pack();

    bool isValid() const;

    const PackDescription &description() const {return m_descr;}
    const PackDependencies &dependencies() const {return m_depends;}

    QString uuid() const;
    QString version() const;
    QString name() const;
    QString vendor() const;
    QString serverFileName() const;
    QString serverLicenseFileName() const;

    // Path && file name
    QString originalXmlConfigFileName() const;
    QString persistentlyCachedXmlConfigFileName() const;
    QString persistentlyCachedZipFileName() const;
    QString unzipPackToPath() const;

    QString installedXmlConfigFileName() const;

    void setInstalledFiles(const QStringList &list);
    QStringList installedFiles() const;

    DataType dataType() const;
    QString dataTypeName() const;

    // Checksums
    QString md5ControlChecksum() const;
    QString sha1ControlChecksum() const;
    bool isSha1Checked() const;
    bool isMd5Checked() const;

    void fromXmlFile(const QString &absFileName);
    QString toXml() const;

    bool operator==(const Pack &other) const;
    bool operator!=(const Pack &other) const {return (!operator==(other));}

private:
    void fromXml(const QString &fullPackConfigXml);

private:
    QString m_OriginalFileName;
//    bool m_Sha1Checked, m_Md5Checked;
    PackDescription m_descr;
    PackDependencies m_depends;
    mutable int m_type;
};


}  // End namespace DataPack

DATAPACK_EXPORT QDebug operator<<(QDebug dbg, const DataPack::Pack &c);

#endif // DATAPACK_PACK_H
