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
#ifndef DDIMANAGER_DDIPLUGIN_H
#define DDIMANAGER_DDIPLUGIN_H

#include <extensionsystem/iplugin.h>

QT_BEGIN_NAMESPACE
class QAction;
QT_END_NAMESPACE

/**
 * \file ddiplugin.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 01 Janv 2014
*/

namespace DDI {
class DDICore;
namespace Internal {
class AtcMode;
class InteractorMode;
class DDIMode;
class ServerManagerMode;
class ComponentAtcMode;

class DDIPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.freemedforms.FreeDDIManager.DDIManagerPlugin" FILE "DDIManager.json")

public:
    DDIPlugin();
    ~DDIPlugin();

    bool initialize(const QStringList &arguments, QString *errorMessage = 0);
    void extensionsInitialized();

    ExtensionSystem::IPlugin::ShutdownFlag aboutToShutdown();

    void createActions();

private Q_SLOTS:
    bool changeLocalDatabase();

#ifdef WITH_TESTS
private Q_SLOTS:
    void initTestCase();
    void test_atcWidget();
    void test_drugInteractor();
    void test_drugInteractorWidget();
    void test_drugDrugInteractionWidget();
    void cleanTestCase();
#endif

private:
    DDICore *_ddiCore;
    AtcMode *_atcMode;
    InteractorMode *_interactorMode;
    DDIMode *_ddiMode;
    ServerManagerMode *_serverMode;
    ComponentAtcMode *_componentAtcMode;
    QAction *aChangeLocalDb;
};

} // namespace Internal
} // namespace DDI

#endif // DDIMANAGER_DDIPLUGIN_H
