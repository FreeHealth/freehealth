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
#include <utils/global.h>

#include "../../autotest.h"

#include <QDebug>
#include <QTest>
#include <QDir>

/**
 * Run unit test on
 * Utils::isDebugWithoutInstallCompilation();
 * Utils::isReleaseCompilation();
 * Utils::isRunningOnMac();
 * Utils::isRunningOnWin();
 * Utils::isRunningOnLinux();
 * Utils::isRunningOnFreebsd();
 * Utils::isLinuxIntegratedCompilation();
 * Utils::applicationPluginsPath(const QString &binaryName, const QString &libraryBaseName)
 * These function are fundamental for the Core::ISettings object and are depend to the qmake step
 */
class tst_OsConfig : public QObject
{
    Q_OBJECT
public:

private slots:
    void initTestCase()
    {
    }

    void testOsConfig()
    {
#if defined(Q_OS_MAC) || defined(Q_OS_MAC64) || defined(Q_OS_MACX)
        QVERIFY(Utils::isRunningOnMac() == true);
        QVERIFY(Utils::isRunningOnWin() == false);
        QVERIFY(Utils::isRunningOnLinux() == false);
        QVERIFY(Utils::isRunningOnFreebsd() == false);
#endif

#if defined(Q_OS_WIN32)
        QVERIFY(Utils::isRunningOnMac() == false);
        QVERIFY(Utils::isRunningOnWin() == true);
        QVERIFY(Utils::isRunningOnLinux() == false);
        QVERIFY(Utils::isRunningOnFreebsd() == false);
#endif

#if defined(Q_OS_LINUX)
        QVERIFY(Utils::isRunningOnMac() == false);
        QVERIFY(Utils::isRunningOnWin() == false);
        QVERIFY(Utils::isRunningOnLinux() == true);
        QVERIFY(Utils::isRunningOnFreebsd() == false);
#endif

#if defined(Q_OS_FREEBSD)
        QVERIFY(Utils::isRunningOnMac() == false);
        QVERIFY(Utils::isRunningOnWin() == false);
        QVERIFY(Utils::isRunningOnLinux() == false);
        QVERIFY(Utils::isRunningOnFreebsd() == true);
#endif
    }

    void testCompilationFlags()
    {
#ifdef DEBUG_WITHOUT_INSTALL
        QVERIFY(Utils::isDebugWithoutInstallCompilation() == true);
        QVERIFY(Utils::isReleaseCompilation() == false);
#endif

#ifndef DEBUG_WITHOUT_INSTALL
        QVERIFY(Utils::isDebugWithoutInstallCompilation() == false);
#endif

#ifdef LINUX_INTEGRATED
        QVERIFY(Utils::isLinuxIntegratedCompilation() == true);
        QVERIFY(Utils::isRunningOnMac() == false);
        QVERIFY(Utils::isRunningOnWin() == false);
#endif

#ifdef DEBUG
        QVERIFY(Utils::isReleaseCompilation() == false);
#endif
#ifdef RELEASE
        QVERIFY(Utils::isReleaseCompilation() == true);
        QVERIFY(Utils::isDebugWithoutInstallCompilation() == false);
#endif
    }

    void testApplicationLibPath()
    {
        QStringList libs = Utils::applicationPluginsPath("freemedforms", "lib64");
        // TODO: how to simulate a release/debug compilation and different OS?
#ifdef DEBUG_WITHOUT_INSTALL
        QString test = qApp->applicationDirPath();
        if (Utils::isRunningOnMac())
            test += "/../../../plugins";
        else if (Utils::isRunningOnLinux())
            test += "/plugins";
        else if (Utils::isRunningOnWin())
            test += "/plugins";
        test = QDir::cleanPath(test);
        QVERIFY(libs.contains(test, Qt::CaseInsensitive) == true);
#endif
#ifdef RELEASE
        if (Utils::isRunningOnMac())
            test += "/../plugins";
//        else if (Utils::isRunningOnLinux())
//            QVERIFY(libs.contains(qApp->applicationDirPath() + "/plugins", Qt::CaseInsensitive) == true);
        else if (Utils::isRunningOnWin())
            test += "/plugins";
        test = QDir::cleanPath(test);
        QVERIFY(libs.contains(test, Qt::CaseInsensitive) == true);
#endif
    }

    void cleanupTestCase()
    {}
};

DECLARE_TEST(tst_OsConfig)
#include "os_config.moc"
