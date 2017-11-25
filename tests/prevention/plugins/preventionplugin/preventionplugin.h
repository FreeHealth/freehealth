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
#ifndef PREVENTION_COREPLUGIN_H
#define PREVENTION_COREPLUGIN_H

#include <extensionsystem/iplugin.h>

/**
 * \file preventionplugin.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 03 Mar 2012
*/

namespace Prevention {
    class ApplicationGeneralPreferencesPage;
}


namespace Prevention {
namespace Internal {

class PreventionPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
public:
    PreventionPlugin();
    ~PreventionPlugin();

    bool initialize(const QStringList &arguments, QString *errorMessage = 0);
    void extensionsInitialized();

public Q_SLOTS:
    void postCoreOpened();

private:
    ApplicationGeneralPreferencesPage *prefPage;
};

} // namespace Internal
} // namespace Prevention

#endif // Prevention_COREPLUGIN_H
