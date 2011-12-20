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

    QString uuid() const;
    QString version() const;
    QString name() const;
    QString serverFileName() const;
    QString serverLicenceFileName() const;
    QString md5ControlChecksum() const;
    QString sha1ControlChecksum() const;

    QString localFileName() const {return m_LocalFileName;}

    bool isSha1Checked() const;
    bool isMd5Checked() const;

    void setLocalFileName(const QString &f) {m_LocalFileName=f;}

    void fromXmlFile(const QString &absFileName);
    void fromXml(const QString &fullPackConfigXml);

private:
    QString m_LocalFileName;
    bool m_Sha1Checked, m_Md5Checked;
    PackDescription m_descr;
    PackDependencies m_depends;
};

}  // End namespace DataPack

#endif // DATAPACK_PACK_H
