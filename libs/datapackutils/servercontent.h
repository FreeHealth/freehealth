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
 *   Main Developers:                                                     *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef SERVERCONTENTS_H
#define SERVERCONTENTS_H

#include <datapackutils/datapack_exporter.h>

#include <QUrl>
#include <QStringList>

class QDomElement;
class QDomDocument;

/**
 * \file servercontent.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.2
 * \date 02 Dec 2011
*/

namespace DataPack {

class DATAPACK_EXPORT ServerContent
{
public:
    ServerContent();

    void clear();
    bool isEmpty() const;

    const QStringList &packDescriptionFileNames() const {return m_PackFileNames;}
    bool addPackRelativeFileName(const QString &fileName);

    bool fromXml(const QString &xml);
    bool fromDomElement(const QDomElement &root);
    bool toXml(QDomElement *root, QDomDocument *doc) const;
    QString toXml() const;

private:
    QStringList m_PackFileNames;
};

}  // End namespace DataPack

#endif // SERVERCONTENTS_H
