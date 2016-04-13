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
 *   Main developers : Eric Maeker
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef ZIPCODES_ZIPCORE_H
#define ZIPCODES_ZIPCORE_H

#include <zipcodesplugin/zipcodes_exporter.h>
#include <QObject>
#include <QSqlDatabase>

/**
 * \file zipcore.h
 * \author Eric Maeker
 * \version 0.8.0
 * \date 20 Dec 2012
*/

namespace DataPack {
class Pack;
}

namespace ZipCodes {
class ZipCountryCompleters;
namespace Internal {
class ZipCodesPlugin;
class ZipCorePrivate;
} // namespace Internal

class ZIPCODES_EXPORT ZipCore : public QObject
{
    Q_OBJECT
    friend class ZipCodes::Internal::ZipCodesPlugin;
    friend class ZipCodes::ZipCountryCompleters;

protected:
    explicit ZipCore(QObject *parent = 0);
    bool initialize();

public:
    static ZipCore &instance();
    ~ZipCore();

    bool isDatabaseAvailable() const;
    QSqlDatabase &database() const;

Q_SIGNALS:
    void databaseRefreshed();

public Q_SLOTS:

private Q_SLOTS:
    void packChanged(const DataPack::Pack &pack);

private:
    Internal::ZipCorePrivate *d;
    static ZipCore *_instance;
};

} // namespace ZipCodes

#endif  // ZIPCODES_ZIPCORE_H

