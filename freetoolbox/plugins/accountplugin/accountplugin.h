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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FREETOOLBOX_ACCOUNTPLUGIN_H
#define FREETOOLBOX_ACCOUNTPLUGIN_H

#include <extensionsystem/iplugin.h>

/**
 * \file accountplugin.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.1.0
 * \date 23 May 2012
*/

namespace Account {
namespace Internal {

class AccountPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.freemedforms.FreeToolBox.AccountPlugin" FILE "Account.json")

public:
    AccountPlugin();
    ~AccountPlugin();

    bool initialize(const QStringList &arguments, QString *errorMessage = 0);
    void extensionsInitialized();

    ExtensionSystem::IPlugin::ShutdownFlag aboutToShutdown();

};

} // namespace Internal
} // namespace Account

#endif // FREETOOLBOX_ACCOUNTPLUGIN_H
