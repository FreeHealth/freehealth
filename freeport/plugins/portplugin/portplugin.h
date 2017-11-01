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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FREEPORT_PORTPLUGIN_H
#define FREEPORT_PORTPLUGIN_H

#include <extensionsystem/iplugin.h>

#include <QObject>

/**
 * \file portplugin.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.10.0
 * \date 14 May 2014
*/

namespace Port {
namespace Internal {

class PortPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.freemedforms.FreePort.PortPlugin" FILE "PortPlugin.json")

public:
    PortPlugin();
    ~PortPlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
};

} // namespace Internal
} // namespace Port

#endif // End FREEPORT_PORTPLUGIN_H
