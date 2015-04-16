/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developers:                                                       *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef GUARD_GUARDCORE_H
#define GUARD_GUARDCORE_H

#include <QObject>
#include <guardplugin/guard_exporter.h>

/**
 * \file guardcore.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 12 Oct 2014
*/

namespace Guard {
namespace Internal {
class GuardCorePrivate;
class GuardPlugin;
class GuardBase;
} // namespace Internal

class GUARD_EXPORT GuardCore : public QObject
{
    Q_OBJECT
    friend class Guard::Internal::GuardPlugin;

protected:
    GuardCore(QObject *parent = 0);
    bool initialize();

public:
    static GuardCore &instance();
    ~GuardCore();

    bool isDatabaseInitialized() const;

    Internal::GuardBase &guardBase() const;

protected:
    void postCoreInitialization();

private:
    static GuardCore *_instance;
    Internal::GuardCorePrivate *d;
};

}  // namespace Guard

#endif  // GUARD_GUARDCORE_H

