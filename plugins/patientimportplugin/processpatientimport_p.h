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
#ifndef PROCESSPATIENTIMPORT_P_H
#define PROCESSPATIENTIMPORT_P_H

#include "processpatientimport.h"
#include "patientimportconstants.h"
#include "patientimportplugin.h"
#include <iostream>
#include <QVector>
#include <QStringList>
#include <QTextStream>
#include <QUuid>
#include <QObject>

namespace PatientImport {
namespace Internal {

struct ProcessPatientImportPrivate {
 ProcessPatientImportPrivate(ProcessPatientImport *q): q_ptr(q) { }
 ProcessPatientImport *q_ptr;
 QString m_fileName;
 QString m_software;
 Patients::PatientModel *m_patientModel;
 QVector<QStringList> *m_import;
 QString m_uuid;
 QStringList m_duplicate;
 };

} // Internal
}
#endif // PROCESSPATIENTIMPORT_P_H
