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
 *   Main Developers:                                                     *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef LIB_DATAPACK_PACKDESCRIPTION_H
#define LIB_DATAPACK_PACKDESCRIPTION_H

#include <datapackutils/datapack_exporter.h>
#include <utils/genericdescription.h>

QT_BEGIN_NAMESPACE
class QTreeWidget;
QT_END_NAMESPACE

/**
 * \file packdescription.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.2
 * \date 25 Nov 2011
*/

namespace DataPack {

class DATAPACK_EXPORT PackDependencyData
{
public:
    enum DataRepresentation {
        Type = 0,
        Uuid,
        PackName,
        PackVersion
    };

    enum TypeOfDependence {
        Depends = 0,
        Recommends,
        Suggests,
        Requires,
        Conflicts,
        Breaks,
        Provides
    };

    PackDependencyData();
    ~PackDependencyData();

    void setType(int type) {m_type=type;}
    void setName(const QString &name) {m_name=name;}
    void setVersion(const QString &version) {m_version=version;}
    void setUuid(const QString &uuid) {m_uid=uuid;}

    int type() const {return m_type;}
    const QString &name() const {return m_name;}
    const QString &version() const {return m_version;}
    const QString &uuid() const {return m_uid;}

    bool operator<(const PackDependencyData &to);

    static QString typeName(int typeReference);
    static int typeFromName(const QString &name);

private:
    int m_type;
    QString m_version, m_name, m_uid;
};

class DATAPACK_EXPORT PackDependencies
{
public:
    PackDependencies();
    ~PackDependencies();

    int count() const {return dependencies.count();}
    const PackDependencyData &at(const int index) const;

    bool fromDomElement(const QDomElement &root);
    bool toDomElement(QDomElement *root, QDomDocument *doc) const;
    QString toXml() const;

    // TODO: operator==();

public:
    QList<PackDependencyData> dependencies;
    PackDependencyData null;
};

class DATAPACK_EXPORT PackDescription : public Utils::GenericDescription
{
public:
    enum ExtraNonTranslated {
        Size = NonTranslatableExtraData + 1,
        UnzipToPath,
        Md5,
        Sha1,
        DataType,
        InstalledFiles
    };

    PackDescription();

    bool isFreeContent() const;
    bool setData(const int ref, const QVariant &value, const QString &lang = QString::null);

    QString toXml() const;

};

}  // End namespace DataPack

#endif // LIB_DATAPACK_PACKDESCRIPTION_H
