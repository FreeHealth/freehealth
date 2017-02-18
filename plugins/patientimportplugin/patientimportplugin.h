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
 *   Main Developer: Jerome Pinguet <jerome@jerome.cc>                     *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef PATIENTIMPORT_IPLUGIN_H
#define PATIENTIMPORT_IPLUGIN_H

#include <extensionsystem/iplugin.h>
#include <QStringList>

namespace PatientImport {
namespace Internal {

class PatientimportPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "io.freehealth.ehr.patientimportPlugin" FILE "patientimport.json")

public:
    PatientimportPlugin();
    ~PatientimportPlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

    static QStringList softwareList();

public slots:
    void openPatientImportDialog();

private Q_SLOTS:
    void postCoreInitialization();
    void coreAboutToClose();

    // Declare plugin unit tests here
//#ifdef WITH_TESTS
//private Q_SLOTS:
//    void initTestCase();
//    void test_your_plugin_unit_test(); // see file tests/test_patientimport.cpp
//    void cleanTestCase();
//#endif

};

} // namespace Internal
} // namespace PatientImport

#endif // PATIENTIMPORT_IPLUGIN_H
