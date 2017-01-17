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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developer: Eric Maeker <eric.maeker@gmail.com>                  *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef EDRCPLUGIN_INTERNAL_IPLUGIN_H
#define EDRCPLUGIN_INTERNAL_IPLUGIN_H

#include <extensionsystem/iplugin.h>

/**
 * \file edrccore.h
 * \author Eric Maeker
 * \version 0.9.2
 * \date 22 Sept 2013
*/

namespace eDRC {
class EdrcCore;
namespace Internal {
class EdrcPreferencesPage;
#ifdef FREEHEALTH
class EdrcWidgetFactory;
#endif

class EdrcPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "io.freehealth.ehr.ToolsPlugin"  FILE "eDRC.json")

public:
    EdrcPlugin();
    ~EdrcPlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

private Q_SLOTS:
    void postCoreInitialization();

#ifdef WITH_TESTS
private Q_SLOTS:
    void initTestCase();
    void testConsultResultObject();
    void testConsultResultXml();
    void testValidatorTestOne();
    void testValidatorTestTwo();
    void testValidatorTestThree();
    void testCrToHtml();
    void cleanupTestCase();
#endif

private:
    EdrcCore *_core;
    EdrcPreferencesPage *_pref;
#ifdef FREEHEALTH
    EdrcWidgetFactory *_factory;
#endif
};

} // namespace Internal
} // namespace eDRC

#endif // EDRCPLUGIN_INTERNAL_IPLUGIN_H

