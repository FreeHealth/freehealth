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
 *   Main developers : Eric Maeker
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DDIMANAGER_DDIPLUGIN_DDICORE_H
#define DDIMANAGER_DDIPLUGIN_DDICORE_H

#include <ddiplugin/ddi_exporter.h>
#include <QObject>

/**
 * \file ddicore.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 09 Oct 2013
*/

namespace DDI {
class AtcTableModel;
namespace Internal {
class DDIPlugin;
class DDICorePrivate;
class DDIDatabase;
}  // namespace Internal

class DDI_EXPORT DDICore : public QObject
{
    Q_OBJECT
    friend class DDI::Internal::DDIPlugin;

protected:
    explicit DDICore(QObject *parent = 0);
    bool initialize();

public:
    static DDICore *instance();
    ~DDICore();

    AtcTableModel *atcTableModel() const;
    Internal::DDIDatabase &database() const;

private:
    static DDICore *_instance;
    Internal::DDICorePrivate *d;
};

} // namespace DDI

#endif  // DDIMANAGER_DDIPLUGIN_DDICORE_H

