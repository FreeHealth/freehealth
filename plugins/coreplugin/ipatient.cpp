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
  Abstract interface provided to access Patient's data.\n
  Use this class to avoid any plugin dependencies (other than Core),
  when needing editing access to the \b current \b editing \b patient data.
  \sa Core::ICore::setPatient(), Core::ICore::patient()
*/


#include "ipatient.h"
#include "constants_tokensandsettings.h"

#include <coreplugin/icore.h>
#include <coreplugin/ipadtools.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_patient.h>
#include <translationutils/trans_user.h>

using namespace Core;

static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline Core::ITokenPool *tokenPool() {return Core::ICore::instance()->padTools()->tokenPool();}

namespace {
class PatientToken : public Core::IToken
{
public:
    PatientToken(const QString &name, const int ref) :
        IToken(name),
        _ref(ref)
    {
    }

    QVariant testValue() const {return fullName();}
    QVariant value() const {return patient()->data(_ref);}

private:
    int _ref;
};

}

IPatient::IPatient(QObject * parent) :
    QAbstractListModel(parent)
{
}

void IPatient::registerPatientTokens()
{
#ifndef WITH_PAD
    return;
#else

    // Create and register namespaces
    // Create tokens
    // For 0.8.0 ONLY
    Core::IToken *t;
    QVector<Core::IToken *> _tokens;

    // Identity
    t = new PatientToken(Constants::TOKEN_PATIENTNAME, BirthName);
    t->setUntranslatedHumanReadableName(Trans::Constants::BIRTHNAME);
    _tokens << t;
    t = new PatientToken(Constants::TOKEN_PATIENTFIRSTNAME, Firstname);
    t->setUntranslatedHumanReadableName(Trans::Constants::FIRSTNAME);
    _tokens << t;
    t = new PatientToken(Constants::TOKEN_PATIENTSECONDNAME, SecondName);
    t->setUntranslatedHumanReadableName(Trans::Constants::SECONDNAME);
    _tokens << t;
    t = new PatientToken(Constants::TOKEN_PATIENTFULLNAME, FullName);
    t->setUntranslatedHumanReadableName(Trans::Constants::FULLNAME);
    _tokens << t;
    t = new PatientToken(Constants::TOKEN_PATIENTTITLE, Title);
    t->setUntranslatedHumanReadableName(Trans::Constants::TITLE);
    _tokens << t;

    // Dates && age
    t = new PatientToken(Constants::TOKEN_PATIENTDATEOFBIRTH, DateOfBirth);
    t->setUntranslatedHumanReadableName(Trans::Constants::DATE_OF_BIRTH);
    _tokens << t;
    t = new PatientToken(Constants::TOKEN_PATIENTDATEOFBIRTH, DateOfDeath);
    t->setUntranslatedHumanReadableName(Trans::Constants::DATE_OF_DEATH);
    _tokens << t;
    t = new PatientToken(Constants::TOKEN_PATIENTYEARSOLD, YearsOld);
    t->setUntranslatedHumanReadableName(Trans::Constants::AGE_IN_YEARS);
    _tokens << t;
    t = new PatientToken(Constants::TOKEN_PATIENTAGE, Age);
    t->setUntranslatedHumanReadableName(Trans::Constants::AGE);
    _tokens << t;

    // Address & contact
    t = new PatientToken(Constants::TOKEN_PATIENTFULLADDRESS, FullAddress);
    t->setUntranslatedHumanReadableName(Trans::Constants::FULLADDRESS);
    _tokens << t;
    t = new PatientToken(Constants::TOKEN_PATIENTADDRESS_STREET, Street);
    t->setUntranslatedHumanReadableName(Trans::Constants::STREET);
    _tokens << t;
    t = new PatientToken(Constants::TOKEN_PATIENTADDRESS_CITY, City);
    t->setUntranslatedHumanReadableName(Trans::Constants::CITY);
    _tokens << t;
    t = new PatientToken(Constants::TOKEN_PATIENTADDRESS_ZIP, ZipCode);
    t->setUntranslatedHumanReadableName(Trans::Constants::ZIPCODE);
    _tokens << t;
    t = new PatientToken(Constants::TOKEN_PATIENTADDRESS_COUNTRY, Country);
    t->setUntranslatedHumanReadableName(Trans::Constants::COUNTRY);
    _tokens << t;
    t = new PatientToken(Constants::TOKEN_PATIENTCONTACT_EMAIL, Mails);
    t->setUntranslatedHumanReadableName(Trans::Constants::EMAIL);
    _tokens << t;
    t = new PatientToken(Constants::TOKEN_PATIENTCONTACT_TELS, Tels);
    t->setUntranslatedHumanReadableName(Trans::Constants::TELS);
    _tokens << t;
    t = new PatientToken(Constants::TOKEN_PATIENTCONTACT_FAX, Faxes);
    t->setUntranslatedHumanReadableName(Trans::Constants::FAX);
    _tokens << t;
    // TODO: add mobile phone (form patient data representation, patientmodel, tokens)
//    t = new PatientToken(Constants::TOKEN_PATIENTCONTACT_MOBILEPHONE, MobilePhone);
//    t->setUntranslatedHumanReadableName(Trans::Constants::MOBILEPHONE);
//    _tokens << t;

    t = new PatientToken(Constants::TOKEN_WEIGHT, Weight);
    t->setUntranslatedHumanReadableName(Trans::Constants::WEIGHT);
    _tokens << t;

    t = new PatientToken(Constants::TOKEN_WEIGHT_UNIT, WeightUnit);
//    t->setUntranslatedHumanReadableName(Trans::Constants::);
    _tokens << t;

    t = new PatientToken(Constants::TOKEN_HEIGHT, Height);
    t->setUntranslatedHumanReadableName(Trans::Constants::HEIGHT);
    _tokens << t;

    t = new PatientToken(Constants::TOKEN_HEIGHT_UNIT, HeightUnit);
//    t->setUntranslatedHumanReadableName(Trans::Constants::);
    _tokens << t;


    t = new PatientToken(Constants::TOKEN_CLCR, CreatinClearance);
    t->setUntranslatedHumanReadableName(Trans::Constants::CREATININ_CLEARANCE);
    _tokens << t;

    t = new PatientToken(Constants::TOKEN_CLCR_UNIT, CreatinClearanceUnit);
//    t->setUntranslatedHumanReadableName(Trans::Constants::);
    _tokens << t;

    if (tokenPool()) {
        LOG("Registering Core::IPatient tokens");
        tokenPool()->addTokens(_tokens);
    } else {
        LOG_ERROR("PadTools object is not available, can not register the Core::IPatient tokens");
    }
    // END

#endif

}

IPatient::~IPatient()
{
//    qDeleteAll(_tokens);
//    _tokens.clear();
}

// OBSOLETE
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
// END
