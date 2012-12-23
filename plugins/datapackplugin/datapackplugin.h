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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DATAPACK_PLUGIN_H
#define DATAPACK_PLUGIN_H

#include <extensionsystem/iplugin.h>

#include <QtCore/QObject>

/**
 * \file datapackplugin.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 25 July 2012
*/

namespace DataPackPlugin {
namespace Internal {
class DataPackPreferencePage;

class DataPackPluginIPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.freemedforms.FreeMedForms.DataPackPlugin" FILE "DataPackPlugin.json")

public:
    DataPackPluginIPlugin();
    ~DataPackPluginIPlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

private Q_SLOTS:
    void postCoreInitialization();
    void coreAboutToClose();
    void togglePackManager();

private:
    DataPackPreferencePage *m_prefPage;
};

}  // End namespace Internal
}  // End namespace DataPackPlugin

#endif  // End DATAPACK_PLUGIN_H
