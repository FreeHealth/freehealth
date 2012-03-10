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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
  \class Core::IPatient
  Abstract interface provided to access Patient's datas.
  \sa Core::ICore::setPatient(), Core::ICore::patient()
*/

#include "ipatient.h"
#include "constants_tokensandsettings.h"

#include <utils/global.h>

using namespace Core;

void IPatient::replaceTokens(QString &stringWillBeModified)
{
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_PATIENTNAME,       data(IPatient::BirthName).toString());
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_PATIENTFIRSTNAME,  data(IPatient::Firstname).toString());
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_PATIENTSECONDNAME, data(IPatient::SecondName).toString());
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_PATIENTFULLNAME,   data(IPatient::FullName).toString());
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_PATIENTYEARSOLD,   data(IPatient::YearsOld).toString() );
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_PATIENTAGE,        data(IPatient::Age).toString() );
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_WEIGHT,            data(IPatient::Weight).toString() );
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_WEIGHT_UNIT,       data(IPatient::WeightUnit).toString() );
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_HEIGHT,            data(IPatient::Height).toString() );
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_HEIGHT_UNIT,       data(IPatient::HeightUnit).toString() );
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_PATIENTDATEOFBIRTH,data(IPatient::DateOfBirth).toString() );
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_CLCR,              data(IPatient::CreatinClearance).toString() );
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_CLCR_UNIT,         data(IPatient::CreatinClearanceUnit).toString() );
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_PATIENTTITLE,      data(IPatient::Title).toString() );
}

