/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developer: Eric Maeker <eric.maeker@gmail.com>                  *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef IDENTITY_H
#define IDENTITY_H

#include "identity_exporter.h"
#include "identitypreferences.h"

#include <extensionsystem/iplugin.h>

/**
 * \file identityplugin.h
 * \author Eric Maeker
 * \version 0.8.2
 * \date 18 Dec 2012
*/

namespace Identity {
namespace Internal {

class IdentityPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.freemedforms.FreeMedForms.IdentityPlugin" FILE "Identity.json")

public:
    IdentityPlugin();
    ~IdentityPlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    //    ShutdownFlag aboutToShutdown();

private Q_SLOTS:
    void postCoreInitialization();
    void coreAboutToClose();
    //    void triggerAction();

private:
//    IdentityPreferencesPage *m_prefPage;
};

} // namespace Internal
} // namespace Identity

#endif // IDENTITY_H

