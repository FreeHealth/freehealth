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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 * \class Core::IPatient
 * Abstract interface provided to access Patient's data.\n
 * Use this class to avoid any plugin dependencies (other than Core),
 * when needing editing access to the \b current \b editing \b patient data. \n
 * The Core::IPatient is a QAbstractListModel with only one row (the current patient).
 * The column represents the patient's values. This object can extract data
 * from the patient database and also inside the forms (if some items have defined
 * a patientdatarepresentation). \n
 * To remove a patient, just set the Core::IPatient::IsActive to \e false or \e 0.
 * \sa Core::ICore::setPatient(), Core::ICore::patient()
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
#include <translationutils/trans_current.h>
#include <translationutils/trans_menu.h>
#include <translationutils/trans_drugs.h>

#include <QMetaEnum>

using namespace Core;

static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline Core::ITokenPool *tokenPool() {return Core::ICore::instance()->padTools()->tokenPool();}

namespace {

const char * const NAMESPACE_DESCRIPTION = QT_TRANSLATE_NOOP("tkConstants", "This token namespace contains all patient's related tokens.\n"
                                                             "It does include the form's extracted token (when forms use 'patientDataRepresentation'),\n"
                                                             "but does not contains all other form items.");

class PatientToken : public Core::IToken
{
public:
    PatientToken(const QString &name, const int ref) :
        IToken(name),
        _ref(ref)
    {
    }

    QVariant testValue() const {return uid();}
    QVariant value() const
    {
        if (!patient())
            return QVariant();
        const QVariant &val = patient()->data(_ref);
        if (val.type() == QVariant::Date) {
            return QLocale().toString(val.toDate(), QLocale::ShortFormat);
        } else if (val.type() == QVariant::DateTime) {
            return QLocale().toString(val.toDateTime(), QLocale::ShortFormat);
        }
        return val;
    }

    int sortIndex() const {return _ref;}

private:
    int _ref;
};

}

IPatient::IPatient(QObject * parent) :
    QAbstractListModel(parent)
{
    setObjectName("Core::IPatient");
}

void IPatient::registerPatientTokens()
{
#ifndef WITH_PAD
    return;
#else
    // Create and register namespaces
    TokenNamespace patientNs("Patient");
    patientNs.setUntranslatedHumanReadableName(Trans::Constants::PATIENT);
    patientNs.setUntranslatedHelpText(::NAMESPACE_DESCRIPTION);
    patientNs.setUntranslatedTooltip(::NAMESPACE_DESCRIPTION);

    TokenNamespace patientIdentNs("Identity");
    patientIdentNs.setUntranslatedHumanReadableName(Trans::Constants::IDENTITY_TEXT);

    TokenNamespace patientAgeNs("Age");
    patientAgeNs.setUntranslatedHumanReadableName(Trans::Constants::AGE);

    TokenNamespace patientAddressNs("Address");
    patientAddressNs.setUntranslatedHumanReadableName(Trans::Constants::ADDRESS);

    TokenNamespace patientContactNs("Contact");
    patientContactNs.setUntranslatedHumanReadableName(Trans::Constants::CONTACT);

    TokenNamespace patientMetricsNs("Metrics");
    patientMetricsNs.setUntranslatedHumanReadableName(Trans::Constants::METRICS);

    TokenNamespace patientBioNs("Biology");
    patientBioNs.setUntranslatedHumanReadableName(Trans::Constants::BIOLOGY);

    TokenNamespace patientInsuranceNs("Insurance");
    patientInsuranceNs.setUntranslatedHumanReadableName(Trans::Constants::INSURANCE);

    TokenNamespace patientDrugsNs("Drugs");
    patientInsuranceNs.setUntranslatedHumanReadableName(Trans::Constants::DRUGS);

    TokenNamespace patientDrugsLongTermNs("ChronicTherapeutics");
    patientDrugsLongTermNs.setUntranslatedHumanReadableName(Trans::Constants::LONG_TERM_THERAPY);
    patientDrugsNs.addChild(patientDrugsLongTermNs);

    TokenNamespace patientDrugsAllergiesNs("Allergies");
    patientDrugsAllergiesNs.setUntranslatedHumanReadableName(Trans::Constants::DRUGS_ALLERGIES);
    patientDrugsNs.addChild(patientDrugsAllergiesNs);

    TokenNamespace patientDrugsIntolerancesNs("Intolerances");
    patientDrugsIntolerancesNs.setUntranslatedHumanReadableName(Trans::Constants::DRUGS_INTOLERANCES);
    patientDrugsNs.addChild(patientDrugsIntolerancesNs);

    patientNs.addChild(patientIdentNs);
    patientNs.addChild(patientAgeNs);
    patientNs.addChild(patientAddressNs);
    patientNs.addChild(patientContactNs);
    patientNs.addChild(patientMetricsNs);
    patientNs.addChild(patientBioNs);
    patientNs.addChild(patientInsuranceNs);
    patientNs.addChild(patientDrugsNs);
    if (tokenPool())
        tokenPool()->registerNamespace(patientNs);

    // Create tokens
    Core::IToken *t;
    QVector<Core::IToken *> _tokens;

    // Identity
    t = new PatientToken(Constants::TOKEN_PATIENTUSUALNAME, UsualName);
    t->setUntranslatedHumanReadableName(Trans::Constants::USUALNAME);
    _tokens << t;
    t = new PatientToken(Constants::TOKEN_PATIENTOTHERNAMES, OtherNames);
    t->setUntranslatedHumanReadableName(Trans::Constants::OTHERNAMES);
    _tokens << t;

    t = new PatientToken(Constants::TOKEN_PATIENTFIRSTNAME, Firstname);
    t->setUntranslatedHumanReadableName(Trans::Constants::FIRSTNAME);
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
    t = new PatientToken(Constants::TOKEN_PATIENTDATEOFDEATH, DateOfDeath);
    t->setUntranslatedHumanReadableName(Trans::Constants::DATE_OF_DEATH);
    _tokens << t;
    t = new PatientToken(Constants::TOKEN_PATIENTISPEDIATRIC, IsPediatricToken);
    t->setUntranslatedHumanReadableName(Trans::Constants::IS_PEDIATRIC);
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
    t = new PatientToken(Constants::TOKEN_PATIENTADDRESS_STATE, StateProvince);
    t->setUntranslatedHumanReadableName(Trans::Constants::STATE);
    _tokens << t;
    t = new PatientToken(Constants::TOKEN_PATIENTADDRESS_COUNTRY, Country);
    t->setUntranslatedHumanReadableName(Trans::Constants::COUNTRY);
    _tokens << t;
    t = new PatientToken(Constants::TOKEN_PATIENTCONTACT_EMAIL, Email);
    t->setUntranslatedHumanReadableName(Trans::Constants::EMAIL);
    _tokens << t;
    t = new PatientToken(Constants::TOKEN_PATIENTCONTACT_TELS, Tels);
    t->setUntranslatedHumanReadableName(Trans::Constants::TELS);
    _tokens << t;
    t = new PatientToken(Constants::TOKEN_PATIENTCONTACT_FAX, Faxes);
    t->setUntranslatedHumanReadableName(Trans::Constants::FAX);
    _tokens << t;
    t = new PatientToken(Constants::TOKEN_PATIENTCONTACT_MOBILEPHONE, MobilePhone);
    t->setUntranslatedHumanReadableName(Trans::Constants::MOBILEPHONE);
    _tokens << t;

    t = new PatientToken(Constants::TOKEN_WEIGHTINGRAMS, WeightInGrams);
    t->setUntranslatedHumanReadableName(Trans::Constants::WEIGHT);
    _tokens << t;

    t = new PatientToken(Constants::TOKEN_HEIGHTINCENTIMETERS, HeightInCentimeters);
    t->setUntranslatedHumanReadableName(Trans::Constants::HEIGHT);
    _tokens << t;

    t = new PatientToken(Constants::TOKEN_CLCR, CreatinClearance);
    t->setUntranslatedHumanReadableName(Trans::Constants::CREATININ_CLEARANCE);
    _tokens << t;

    t = new PatientToken(Constants::TOKEN_CLCR_UNIT, CreatinClearanceUnit);
//    t->setUntranslatedHumanReadableName(Trans::Constants::);
    _tokens << t;

    t = new PatientToken(Constants::TOKEN_PATIENTSOCIALNUMBER, SocialNumber);
    t->setUntranslatedHumanReadableName(Trans::Constants::SOCIAL_NUMBER);
    _tokens << t;
    t = new PatientToken(Constants::TOKEN_PATIENTSOCIALNUMBER2, SocialNumber2);
    t->setUntranslatedHumanReadableName(Trans::Constants::SOCIAL_NUMBER);
    _tokens << t;
    t = new PatientToken(Constants::TOKEN_PATIENTSOCIALNUMBER3, SocialNumber3);
    t->setUntranslatedHumanReadableName(Trans::Constants::SOCIAL_NUMBER);
    _tokens << t;
    t = new PatientToken(Constants::TOKEN_PATIENTSOCIALNUMBER4, SocialNumber4);
    t->setUntranslatedHumanReadableName(Trans::Constants::SOCIAL_NUMBER);
    _tokens << t;
//    t = new PatientToken(Constants::TOKEN_PATIENTSOCIALNUMBERS, SocialNumbers);
//    t->setUntranslatedHumanReadableName(Trans::Constants::SOCIAL_NUMBERS);
//    _tokens << t;
    t = new PatientToken(Constants::TOKEN_PATIENTSOCIALNUMBEROWNERNAME, SocialNumberOwnerFullName);
    t->setUntranslatedHumanReadableName(Trans::Constants::SOCIAL_NUMBER_OWNER_NAME);
    _tokens << t;
    t = new PatientToken(Constants::TOKEN_PATIENTSOCIALNUMBEROWNERADDRESS, SocialNumberOwnerFullAddress);
    t->setUntranslatedHumanReadableName(Trans::Constants::SOCIAL_NUMBER_OWNER_ADDRESS);
    _tokens << t;

    t = new PatientToken(Constants::TOKEN_DRUGS_CHRONIC_XML, DrugsChronicTherapeutics);
    t->setUntranslatedHumanReadableName(Trans::Constants::LONG_TERM_THERAPY);
    _tokens << t;
    t = new PatientToken(Constants::TOKEN_DRUGS_CHRONIC_HTML, DrugsChronicTherapeuticsHtml);
    t->setUntranslatedHumanReadableName(Trans::Constants::LONG_TERM_THERAPY);
    _tokens << t;

    if (tokenPool()) {
        LOG("Registering Core::IPatient tokens");
        tokenPool()->addTokens(_tokens);
    } else {
        LOG_ERROR("PadTools object is not available, can not register the Core::IPatient tokens");
    }
#endif

}

IPatient::~IPatient()
{
//    qDeleteAll(_tokens);
//    _tokens.clear();
}

/*! \deprecated */
void IPatient::replaceTokens(QString &stringWillBeModified)
{
#ifdef WITH_PAD
    Q_UNUSED(stringWillBeModified);
    // TODO: manage PADTOOLS here
#else
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_PATIENTUSUALNAME,  data(IPatient::UsualName).toString());
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_PATIENTOTHERNAMES, data(IPatient::OtherNames).toString());
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_PATIENTFIRSTNAME,  data(IPatient::Firstname).toString());
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_PATIENTFULLNAME,   data(IPatient::FullName).toString());
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_PATIENTYEARSOLD,   data(IPatient::YearsOld).toString() );
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_PATIENTAGE,        data(IPatient::Age).toString() );
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_WEIGHTINGRAMS,     data(IPatient::WeightInGrams).toString() );
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_HEIGHTINCENTIMETERS, data(IPatient::HeightInCentimeters).toString() );
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_PATIENTDATEOFBIRTH,data(IPatient::DateOfBirth).toString() );
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_PATIENTDATEOFDEATH,data(IPatient::DateOfDeath).toString() );
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_CLCR,              data(IPatient::CreatinClearance).toString() );
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_CLCR_UNIT,         data(IPatient::CreatinClearanceUnit).toString() );
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_PATIENTTITLE,      data(IPatient::Title).toString() );

    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_PATIENTSOCIALNUMBER,  data(IPatient::SocialNumber).toString() );
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_PATIENTSOCIALNUMBER2, data(IPatient::SocialNumber2).toString() );
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_PATIENTSOCIALNUMBER3, data(IPatient::SocialNumber3).toString() );
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_PATIENTSOCIALNUMBER4, data(IPatient::SocialNumber4).toString() );
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_PATIENTSOCIALNUMBERS, data(IPatient::SocialNumber).toString() + " " + data(IPatient::SocialNumber2).toString() + " " + data(IPatient::SocialNumber3).toString() + " " + data(IPatient::SocialNumber4).toString());
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_PATIENTSOCIALNUMBEROWNERNAME, data(IPatient::SocialNumberOwnerFullName).toString());
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_PATIENTSOCIALNUMBEROWNERADDRESS, data(IPatient::SocialNumberOwnerFullAddress).toString());


    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_PATIENTFULLADDRESS, data(IPatient::FullAddress).toString());
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_PATIENTTELS, data(IPatient::Tels).toString());
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_PATIENTFAXES, data(IPatient::Faxes).toString());
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_PATIENTMAILS, data(IPatient::Email).toString());
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_PATIENTMOBILEPHONE, data(IPatient::MobilePhone).toString());
#endif
}
// END

/** Return the readable name of the Core::IPatient::PatientDataRepresentation enumerator index. */
QString IPatient::enumToString(PatientDataRepresentation data)
{
    int index = metaObject()->indexOfEnumerator("PatientDataRepresentation");
    QMetaEnum metaEnum = metaObject()->enumerator(index);
    return metaEnum.valueToKey(data);
}
