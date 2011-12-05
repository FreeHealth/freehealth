/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developpers :                                                    *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
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
    Pack();
    virtual ~Pack();

    const PackDescription &description() const {return m_descr;}
    const PackDependencies &dependencies() const {return m_depends;}

    QString serverFileName() const {return m_ServerFileName;}
    QString serverLicenceFileName() const {return m_ServerLicenceFileName;}
    QString md5ControlChecksum() const {return m_Md5;}
    QString sha1ControlChecksum() const {return m_Sha1;}

    QString downloadedFileName() const {return m_DownFileName;}

    bool isSha1Checked() const;
    bool isMd5Checked() const;

    void setServerFileName(const QString &sfn) {m_ServerFileName = sfn;}
    void setMd5ControlChecksum(const QString &md5) {m_Md5=md5;}
    void setSha1ControlChecksum(const QString &sha1) {m_Sha1=sha1;}

    void setDownloadedFileName(const QString &f) {m_DownFileName=f;}

    void fromXmlFile(const QString &absFileName);
    void fromXml(const QString &fullPackConfigXml);


private:
    QString m_ServerFileName, m_ServerLicenceFileName, m_Md5, m_Sha1, m_DownFileName;
    bool m_Sha1Checked, m_Md5Checked;
    PackDescription m_descr;
    PackDependencies m_depends;
};

}  // End namespace DataPack

#endif // DATAPACK_PACK_H
