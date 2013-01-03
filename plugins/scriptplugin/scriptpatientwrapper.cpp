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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "scriptpatientwrapper.h"

#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>

using namespace Script;
using namespace Internal;

static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}

ScriptPatientWrapper::ScriptPatientWrapper(QObject *parent) :
    QObject(parent)
{
}

bool ScriptPatientWrapper::isActive() const
{
    return (!patient()->uuid().isEmpty());
}
QString ScriptPatientWrapper::fullName() const
{
    return patient()->data(Core::IPatient::FullName).toString();
}

QString ScriptPatientWrapper::birthName() const
{
    return patient()->data(Core::IPatient::BirthName).toString();
}

QString ScriptPatientWrapper::secondName() const
{
    return patient()->data(Core::IPatient::SecondName).toString();
}

QString ScriptPatientWrapper::firstName() const
{
    return patient()->data(Core::IPatient::Firstname).toString();
}

QString ScriptPatientWrapper::street() const
{
    return patient()->data(Core::IPatient::Street).toString();
}

QString ScriptPatientWrapper::city() const
{
    return patient()->data(Core::IPatient::City).toString();
}

QString ScriptPatientWrapper::zipcode() const
{
    return patient()->data(Core::IPatient::ZipCode).toString();
}

QString ScriptPatientWrapper::state() const
{
    return patient()->data(Core::IPatient::StateProvince).toString();
}

QString ScriptPatientWrapper::country() const
{
    return patient()->data(Core::IPatient::Country).toString();
}

QDate ScriptPatientWrapper::dateOfBirth() const
{
    return patient()->data(Core::IPatient::DateOfBirth).toDate();
}
int ScriptPatientWrapper::yearsOld() const
{
    return patient()->data(Core::IPatient::YearsOld).toInt();
}
bool ScriptPatientWrapper::isMale() const
{
    return (patient()->data(Core::IPatient::GenderIndex).toInt()==0);
}
bool ScriptPatientWrapper::isFemale() const
{
    return patient()->data(Core::IPatient::GenderIndex).toInt()==1;
}
double ScriptPatientWrapper::weight() const
{
    return patient()->data(Core::IPatient::Weight).toDouble();
}
QString ScriptPatientWrapper::weightUnit() const
{
    return patient()->data(Core::IPatient::WeightUnit).toString();
}
double ScriptPatientWrapper::height() const
{
    return patient()->data(Core::IPatient::Height).toDouble();
}
QString ScriptPatientWrapper::heightUnit() const
{
    return patient()->data(Core::IPatient::HeightUnit).toString();
}
double ScriptPatientWrapper::bmi() const
{
    return patient()->data(Core::IPatient::BMI).toDouble();
}
double ScriptPatientWrapper::creatinine() const
{
    return patient()->data(Core::IPatient::Creatinine).toDouble();
}
QString ScriptPatientWrapper::creatinineUnit() const
{
    return patient()->data(Core::IPatient::CreatinineUnit).toString();
}
double ScriptPatientWrapper::clearanceCreatinine() const
{
    return patient()->data(Core::IPatient::CreatinClearance).toDouble();
}
QString ScriptPatientWrapper::clearanceCreatinineUnit() const
{
    return patient()->data(Core::IPatient::CreatinClearanceUnit).toString();
}

