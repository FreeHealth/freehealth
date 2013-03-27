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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef XMLFORMNAME_H
#define XMLFORMNAME_H

#include <QString>
#include <QMultiHash>

/**
 * \file xmlformcontentreader.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.7.5
 * \date 22 Jun 2012
*/


namespace XmlForms {
namespace Internal {

struct XmlFormName {
    XmlFormName();
    XmlFormName(const QString &uid);

    bool operator==(const XmlFormName &other) const;

    bool isValid, isAvailableFromDatabase;
    QString uid, absFileName, absPath, modeName;
    QString fileVersion, databaseVersion;
    QMultiHash<int, QString> databaseAvailableContents;
    int dbId;
};

QDebug operator<<(QDebug dbg, const XmlFormName &c);

}
}


#endif // XMLFORMNAME_H
