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
 *  Main developer: Jerome Pinguet <jerome@jerome.cc                       *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef PROCESSPATIENTIMPORT_H
#define PROCESSPATIENTIMPORT_H

#include <patientimportplugin/patientimport_exporter.h>
#include <patientbaseplugin/patientmodel.h>
#include <patientbaseplugin/patientbase.h>
#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <QString>
#include <QFile>

namespace Patients {
class PatientModel;
} // namespace Patients

namespace PatientImport {
namespace Internal {
class ProcessPatientImportPrivate;
} // namespace Internal

class ProcessPatientImport : public QObject
{
    Q_OBJECT

public:
    ProcessPatientImport(QString filename, QString software);
    ~ProcessPatientImport();
    void parseGestcab();
    void modifyGestcab();
    void importGestcab();

private:
    Internal::ProcessPatientImportPrivate *d_ptr;
};


} // PatientImport

#endif // PROCESSPATIENTIMPORT_H
