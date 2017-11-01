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
#ifndef FREEDDIMANAGER_DATAPACKPLUGIN_IPLUGIN_H
#define FREEDDIMANAGER_DATAPACKPLUGIN_IPLUGIN_H

#include <extensionsystem/iplugin.h>

/**
 * \file datapackplugin.h
 * \author Eric Maeker
 * \version 0.10.9
 * \date 25 May 2014
*/

namespace DataPackPlugin {
namespace Internal {
class DataPackMode;

class DataPackIPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.freemedforms.FreeDDIManager.DataPackPlugin" FILE "DataPackPlugin.json")

public:
    DataPackIPlugin();
    ~DataPackIPlugin();

    bool initialize(const QStringList &arguments, QString *errorMessage = 0);
    void extensionsInitialized();

    ExtensionSystem::IPlugin::ShutdownFlag aboutToShutdown();

private:
    DataPackMode *_datapackMode;
};

} // namespace Internal
} // namespace DataPack

#endif // FREEDDIMANAGER_DATAPACKPLUGIN_IPLUGIN_H
