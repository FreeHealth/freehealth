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
#ifndef DDIMANAGER_DRUGSDBPLUGIN_H
#define DDIMANAGER_DRUGSDBPLUGIN_H

#include <extensionsystem/iplugin.h>

QT_BEGIN_NAMESPACE
class QAction;
QT_END_NAMESPACE

/**
 * \file drugsbdplugin.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 11 Janv 2014
*/

namespace DrugsDb {
namespace Internal {
class IDrugDatabase;
class DrugsDbMode;

class DrugsDbPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.freemedforms.FreeDDIManager.DrugsDbPlugin" FILE "DrugsDb.json")

public:
    DrugsDbPlugin();
    ~DrugsDbPlugin();

    bool initialize(const QStringList &arguments, QString *errorMessage = 0);
    void extensionsInitialized();

    ExtensionSystem::IPlugin::ShutdownFlag aboutToShutdown();


//#ifdef WITH_TESTS
//private Q_SLOTS:
//    void initTestCase();
//    void cleanTestCase();
//#endif

private:
    DrugsDbMode *_mode;
    QList<IDrugDatabase*> _databases;
};

} // namespace Internal
} // namespace DrugsDb

#endif // DDIMANAGER_DRUGSDBPLUGIN_H
