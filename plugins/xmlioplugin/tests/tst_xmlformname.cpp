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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developers:                                                       *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "../xmlioplugin.h"
#include "../xmlformname.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_tokensandsettings.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/randomizer.h>
#include <extensionsystem/pluginmanager.h>

#include <QTest>

using namespace XmlForms;
using namespace Internal;

static inline ExtensionSystem::PluginManager *pluginManager() { return ExtensionSystem::PluginManager::instance(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

void XmlFormIOPlugin::initTestCase()
{
}

void XmlFormIOPlugin::test_XmlFormName()
{
    QStringList modes;
    modes << "central" << "pmhx" << "drugs" << "identity" << "other";
    foreach(const QString &mode, modes) {
        QString file = settings()->path(Core::ISettings::UserCompleteFormsPath);
        file += QString("/MySet/unit-test1/%1.xml").arg(mode);
        XmlFormName userCompleteForm(file);
        QVERIFY(userCompleteForm.absFileName == file);
        QVERIFY(userCompleteForm.absPath == QFileInfo(file).absolutePath());
        QVERIFY(userCompleteForm.modeName == mode);
        QVERIFY(userCompleteForm.uid.isEmpty() == false);
        QVERIFY(userCompleteForm.uid == QString("%1/MySet/unit-test1").arg(Core::Constants::TAG_APPLICATION_USER_COMPLETEFORMS_PATH));

        file = settings()->path(Core::ISettings::UserSubFormsPath);
        file += QString("/MySet/unit-test1/%1.xml").arg(mode);
        XmlFormName userSubForm(file);
        QVERIFY(userSubForm.absFileName == file);
        QVERIFY(userSubForm.absPath == QFileInfo(file).absolutePath());
        QVERIFY(userSubForm.modeName == mode);
        QVERIFY(userSubForm.uid.isEmpty() == false);
        QVERIFY(userSubForm.uid == QString("%1/MySet/unit-test1").arg(Core::Constants::TAG_APPLICATION_USER_SUBFORMS_PATH));

        file = settings()->path(Core::ISettings::CompleteFormsPath);
        file += QString("/Category/unit-test1/%1.xml").arg(mode);
        XmlFormName completeForm(file);
        QVERIFY(completeForm.absFileName == file);
        QVERIFY(completeForm.absPath == QFileInfo(file).absolutePath());
        QVERIFY(completeForm.modeName == mode);
        QVERIFY(completeForm.uid.isEmpty() == false);
        QVERIFY(completeForm.uid == QString("%1/Category/unit-test1").arg(Core::Constants::TAG_APPLICATION_COMPLETEFORMS_PATH));

        file = settings()->path(Core::ISettings::SubFormsPath);
        file += QString("/Category/unit-test1/%1.xml").arg(mode);
        XmlFormName subForm(file);
        QVERIFY(subForm.absFileName == file);
        QVERIFY(subForm.absPath == QFileInfo(file).absolutePath());
        QVERIFY(subForm.modeName == mode);
        QVERIFY(subForm.uid.isEmpty() == false);
        QVERIFY(subForm.uid == QString("%1/Category/unit-test1").arg(Core::Constants::TAG_APPLICATION_SUBFORMS_PATH));
    }
}

void XmlFormIOPlugin::cleanupTestCase()
{
}
