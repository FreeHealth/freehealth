/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main developers : Eric Maeker
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef PATIENTS_PATIENTCORE_H
#define PATIENTS_PATIENTCORE_H

#include <patientbaseplugin/patientbase_exporter.h>
#include <QObject>

/**
 * \file PatientCore.h
 * \author Eric Maeker
 * \version 0.8.0
 * \date 04 Dec 2012
*/

namespace Patients {
class PatientModel;
namespace Internal {
class PatientBase;
class PatientBasePlugin;
class BasicSqlPatientModel;
class PatientCorePrivate;
}  // namespace Internal

class PATIENT_EXPORT PatientCore : public QObject
{
    Q_OBJECT
    friend class Patients::Internal::PatientBasePlugin;

protected:
    explicit PatientCore(QObject *parent = 0);
    bool initialize();
    
public:
    static PatientCore *instance() {return _instance;}
    ~PatientCore();

    Internal::PatientBase *patientBase() const;
    Internal::BasicSqlPatientModel *basicSqlPatientModel() const;

Q_SIGNALS:
    
public Q_SLOTS:
    
private:
    Internal::PatientCorePrivate *d;
    static PatientCore *_instance;
};

} // namespace Patients

#endif  // PATIENTS_PATIENTCORE_H

