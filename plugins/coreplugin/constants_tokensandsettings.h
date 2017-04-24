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
 ***************************************************************************/
#ifndef CORE_CONSTANTS_TOKENS_SETTINGS_H
#define CORE_CONSTANTS_TOKENS_SETTINGS_H

/**
 * \file constants_tokensandsettings.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 11 Jan 2014
*/

namespace Core {
namespace Constants {

// TEXT COMPLETION tokens

// For Documents
const char * const  TOKEN_DOCUMENTTITLE     = "DOCUMENTTITLE";

// For Patients
#ifndef WITH_PAD
// OLD TOKENS (before v0.8.0)
const char * const  TOKEN_PATIENTAGE        = "PATIENTAGE";
const char * const  TOKEN_PATIENTYEARSOLD   = "PATIENTYEARSOLD";
const char * const  TOKEN_PATIENTFULLNAME   = "PATIENTFULLNAME";
const char * const  TOKEN_PATIENTUSUALNAME  = "PATIENTUSUALNAME";
const char * const  TOKEN_PATIENTOTHERNAMES = "PATIENTOTHERNAMES";
const char * const  TOKEN_PATIENTFIRSTNAME  = "PATIENTFIRSTNAME";
const char * const  TOKEN_PATIENTTITLE      = "PATIENTTITLE";
const char * const  TOKEN_PATIENTFULLADDRESS= "PATIENTFULLADDRESS";
const char * const  TOKEN_PATIENTDATEOFBIRTH= "PATIENTDATEOFBIRTH";
const char * const  TOKEN_PATIENTDATEOFDEATH= "PATIENTDATEOFDEATH";
const char * const  TOKEN_WEIGHTINGRAMS     = "WEIGHTINGRAMS";
const char * const  TOKEN_HEIGHTINCENTIMETERS = "HEIGHTINCENTIMETERS";
const char * const  TOKEN_CLCR              = "CLCR";
const char * const  TOKEN_CLCR_UNIT         = "CLCR_UNIT";
const char * const  TOKEN_DATE              = "DATE";
const char * const  TOKEN_NUMBEROFDRUGS     = "NBDRUGS";
const char * const  TOKEN_PATIENTSOCIALNUMBER = "SOCIALNUMBER";
const char * const  TOKEN_PATIENTSOCIALNUMBER2 = "SOCIALNUMBER2";
const char * const  TOKEN_PATIENTSOCIALNUMBER3 = "SOCIALNUMBER3";
const char * const  TOKEN_PATIENTSOCIALNUMBER4 = "SOCIALNUMBER4";
const char * const  TOKEN_PATIENTSOCIALNUMBERS = "SOCIALNUMBERS";
const char * const  TOKEN_PATIENTSOCIALNUMBEROWNERNAME = "SOCIALNUMBEROWNERNAME";
const char * const  TOKEN_PATIENTSOCIALNUMBEROWNERADDRESS = "SOCIALNUMBEROWNERADDRESS";

const char * const  TOKEN_PATIENTTELS = "PATIENTTELS";
const char * const  TOKEN_PATIENTFAXES = "PATIENTFAXES";
const char * const  TOKEN_PATIENTMAILS = "PATIENTMAILS";
const char * const  TOKEN_PATIENTMOBILEPHONE = "PATIENTMOBILEPHONE";
// END
#else
// NEW TOKENS (>= v0.8.0)
const char * const  TOKEN_PATIENT_NAMESPACE          = "Patient";

const char * const  TOKEN_PATIENTAGE                 = "Patient.Age.FullAge";
const char * const  TOKEN_PATIENTYEARSOLD            = "Patient.Age.YearsOld";
const char * const  TOKEN_PATIENTDATEOFBIRTH         = "Patient.Age.DateOfBirth";
const char * const  TOKEN_PATIENTDATEOFDEATH         = "Patient.Age.DateOfDeath";
const char * const  TOKEN_PATIENTISPEDIATRIC         = "Patient.Age.IsPediatric";
const char * const  TOKEN_PATIENTFULLNAME            = "Patient.Identity.FullName";
const char * const  TOKEN_PATIENTUSUALNAME           = "Patient.Identity.UsualName";
const char * const  TOKEN_PATIENTOTHERNAMES          = "Patient.Identity.OtherNames";
const char * const  TOKEN_PATIENTFIRSTNAME           = "Patient.Identity.FirstName";
const char * const  TOKEN_PATIENTTITLE               = "Patient.Identity.Title";
const char * const  TOKEN_PATIENTFULLADDRESS         = "Patient.Address.FullAddress";
const char * const  TOKEN_PATIENTADDRESS_STREET      = "Patient.Address.Street";
const char * const  TOKEN_PATIENTADDRESS_CITY        = "Patient.Address.City";
const char * const  TOKEN_PATIENTADDRESS_ZIP         = "Patient.Address.Zip";
const char * const  TOKEN_PATIENTADDRESS_STATE       = "Patient.Address.State";
const char * const  TOKEN_PATIENTADDRESS_COUNTRY     = "Patient.Address.Country";
const char * const  TOKEN_PATIENTCONTACT_EMAIL       = "Patient.Contact.Email";
const char * const  TOKEN_PATIENTCONTACT_TELS        = "Patient.Contact.Tels";
const char * const  TOKEN_PATIENTCONTACT_FAX         = "Patient.Contact.Fax";
const char * const  TOKEN_PATIENTCONTACT_MOBILEPHONE = "Patient.Contact.MobilePhone";

const char * const  TOKEN_PATIENTSOCIALNUMBER        = "Patient.Insurance.SocialNumber";
const char * const  TOKEN_PATIENTSOCIALNUMBER2       = "Patient.Insurance.SocialNumber2";
const char * const  TOKEN_PATIENTSOCIALNUMBER3       = "Patient.Insurance.SocialNumber3";
const char * const  TOKEN_PATIENTSOCIALNUMBER4       = "Patient.Insurance.SocialNumber4";
const char * const  TOKEN_PATIENTSOCIALNUMBERS       = "Patient.Insurance.SocialNumbers";
const char * const  TOKEN_PATIENTSOCIALNUMBEROWNERNAME = "Patient.Insurance.SocialNumberOwnerName";
const char * const  TOKEN_PATIENTSOCIALNUMBEROWNERADDRESS = "Patient.Insurance.SocialNumberOwnerAddress";

const char * const  TOKEN_WEIGHTINGRAMS     = "Patient.Metrics.WeightInGrams";
const char * const  TOKEN_HEIGHTINCENTIMETERS = "Patient.Metrics.HeightInCentimeters";
const char * const  TOKEN_CLCRINMLMIN         = "Patient.Biology.CreatininClearanceInMlMin";
const char * const  TOKEN_CLCR              = "Patient.Biology.CreatininClearance";
const char * const  TOKEN_CLCR_UNIT         = "Patient.Biology.CreatininClearanceUnit";

const char * const  TOKEN_DRUGS_CHRONIC_XML  = "Patient.Drugs.ChronicTherapeutics.Xml";
const char * const  TOKEN_DRUGS_CHRONIC_HTML = "Patient.Drugs.ChronicTherapeutics.Html";

const char * const  TOKEN_DATE              = "DATE";
const char * const  TOKEN_NUMBEROFDRUGS     = "NBDRUGS";
// END
#endif

// For Users
#ifndef WITH_PAD
// OLD TOKENS (before v0.8.0)
const char * const  TOKEN_USERAGE            = "USERAGE";
const char * const  TOKEN_USERYEARSOLD       = "USERYEARSOLD";
const char * const  TOKEN_USERFULLNAME       = "USERFULLNAME";
const char * const  TOKEN_USERTITLE          = "USERTITLE";
const char * const  TOKEN_USERUSUALNAME      = "USERNAME";
const char * const  TOKEN_USEROTHERNAMES     = "USEROTHERNAMES";
const char * const  TOKEN_USERFIRSTNAME      = "USERFIRSTNAME";
const char * const  TOKEN_USERSPECIALITIES   = "USERSPECIALITIES";
const char * const  TOKEN_USERIDENTIFIER_OLD = "USERIDENTIFIANTS";
const char * const  TOKEN_USERIDENTIFIER     = "USERIDENTIFIERS";
const char * const  TOKEN_USERQUALIFICATIONS = "USERQUALIFICATIONS";
const char * const  TOKEN_USERSTREET         = "USERADDRESS";
const char * const  TOKEN_USERMAIL           = "USERMAIL";
const char * const  TOKEN_USERTEL1           = "USERTEL1";
const char * const  TOKEN_USERTEL2           = "USERTEL2";
const char * const  TOKEN_USERTEL3           = "USERTEL3";
const char * const  TOKEN_USERFAX            = "USERFAX";
const char * const  TOKEN_USERFULLADDRESS    = "USERFULLADDRESS";
const char * const  TOKEN_USERFULLCONTACT    = "USERFULLCONTACT";
// END
#else
const char * const  TOKEN_USER_NAMESPACE     = "User";

const char * const  TOKEN_USERDATEOFBIRTH    = "User.Age.DateOfBirth";
const char * const  TOKEN_USERAGE            = "User.Age.FullAge";
const char * const  TOKEN_USERYEARSOLD       = "User.Age.YearsOld";
const char * const  TOKEN_USERFULLNAME       = "User.Identity.FullName";
const char * const  TOKEN_USERTITLE          = "User.Identity.Title";
const char * const  TOKEN_USERUSUALNAME      = "User.Identity.UsualName";
const char * const  TOKEN_USEROTHERNAMES     = "User.Identity.OtherNames";
const char * const  TOKEN_USERFIRSTNAME      = "User.Identity.FirstName";
const char * const  TOKEN_USERSPECIALITIES   = "User.Professional.Specialties";
const char * const  TOKEN_USERIDENTIFIER_OLD = "User.Professional.Identifiants";
const char * const  TOKEN_USERIDENTIFIER     = "User.Professional.Identifiers";
const char * const  TOKEN_USERQUALIFICATIONS = "User.Professional.Qualifications";
const char * const  TOKEN_USERMAIL           = "User.Contact.Mail";
const char * const  TOKEN_USERTEL1           = "User.Contact.Tel1";
const char * const  TOKEN_USERTEL2           = "User.Contact.Tel2";
const char * const  TOKEN_USERTEL3           = "User.Contact.Tel3";
const char * const  TOKEN_USERFAX            = "User.Contact.Fax";
const char * const  TOKEN_USERFULLADDRESS    = "User.Contact.FullAddress";
const char * const  TOKEN_USERFULLCONTACT    = "User.Contact.FullContact";
const char * const  TOKEN_USERFULLADDRESS_HTML  = "User.Contact.FullAddress.Html";
const char * const  TOKEN_USERFULLCONTACT_HTML  = "User.Contact.FullContact.Html";
const char * const  TOKEN_USERSTREET         = "User.Contact.Street";
const char * const  TOKEN_USERZIP            = "User.Contact.Zipcode";
const char * const  TOKEN_USERCITY           = "User.Contact.City";
const char * const  TOKEN_USERSTATE          = "User.Contact.State";
const char * const  TOKEN_USERCOUNTRY        = "User.Contact.Country";

#endif

// For Prescriptions
#ifdef WITH_PAD
const char * const  TOKEN_PRESC_DRUGNAME             = "Prescription.Drug.brandname";
const char * const  TOKEN_PRESC_Q_FULL               = "Prescription.Protocol.Quantity.Full";
const char * const  TOKEN_PRESC_Q_FROM               = "Prescription.Protocol.Quantity.From";
const char * const  TOKEN_PRESC_Q_TO                 = "Prescription.Protocol.Quantity.to";
const char * const  TOKEN_PRESC_Q_SCHEME             = "Prescription.Protocol.Quantity.Scheme";
const char * const  TOKEN_PRESC_MEAL                 = "Prescription.Protocol.Meal";
const char * const  TOKEN_PRESC_PERIOD_FULL          = "Prescription.Protocol.Period.Full";
const char * const  TOKEN_PRESC_PERIOD_VALUE         = "Prescription.Protocol.Period.Value";
const char * const  TOKEN_PRESC_PERIOD_SCHEME        = "Prescription.Protocol.Period.Scheme";
const char * const  TOKEN_PRESC_D_FULL               = "Prescription.Protocol.Duration.Full";
const char * const  TOKEN_PRESC_D_FROM               = "Prescription.Protocol.Duration.From";
const char * const  TOKEN_PRESC_D_TO                 = "Prescription.Protocol.Duration.To";
const char * const  TOKEN_PRESC_D_SCHEME             = "Prescription.Protocol.Duration.Scheme";
const char * const  TOKEN_PRESC_ROUTE                = "Prescription.Protocol.Route";
const char * const  TOKEN_PRESC_DISTRIB_DAILYSCHEME  = "Prescription.Protocol.DailyScheme.Distributed";
const char * const  TOKEN_PRESC_REPEATED_DAILYSCHEME = "Prescription.Protocol.DailyScheme.Repeated";
const char * const  TOKEN_PRESC_MININTERVAL_FULL     = "Prescription.Protocol.Quantity.MinInterval.Full";
const char * const  TOKEN_PRESC_MININTERVAL_VALUE    = "Prescription.Protocol.Quantity.MinInterval.Value";
const char * const  TOKEN_PRESC_MININTERVAL_SCHEME   = "Prescription.Protocol.Quantity.MinInterval.Scheme";
const char * const  TOKEN_PRESC_REFILL               = "Prescription.Protocol.Refill";
const char * const  TOKEN_PRESC_NOTE                 = "Prescription.Protocol.Note";
#endif

// Some path tags
const char * const  TAG_APPLICATION_RESOURCES_PATH          = "__appResources__";
const char * const  TAG_APPLICATION_THEME_PATH              = "__theme__";
const char * const  TAG_APPLICATION_COMPLETEFORMS_PATH      = "__completeForms__";
const char * const  TAG_APPLICATION_SUBFORMS_PATH           = "__subForms__";
const char * const  TAG_APPLICATION_USER_COMPLETEFORMS_PATH = "__userCompleteForms__";
const char * const  TAG_APPLICATION_USER_SUBFORMS_PATH      = "__userSubForms__";
const char * const  TAG_DATAPACK_COMPLETEFORMS_PATH         = "__datapackCompleteForms__";
const char * const  TAG_DATAPACK_SUBFORMS_PATH              = "__datapackSubForms__";
const char * const  TAG_USER_DOCUMENT_PATH                  = "__userDocuments__";
const char * const  TAG_USER_RESOURCES_PATH                 = "__userResources__";

// Settings keys
const char * const  S_RECENTFILES_GROUP    = "RecentFiles";
const char * const  S_RECENTFILES_KEY      = "File";
const char * const  S_RECENTFILES_MAX_KEY  = "Max";
const char * const  S_SAVEINTERVAL         = "Core/SaveInterval";
const char * const  S_OPENLAST             = "Core/OpenLastOpenedFile";
const char * const  S_SHOWHELPTEXT         = "Core/ShowFormHelpText";
const char * const  S_LASTLOGIN            = "Login/userLogin";
const char * const  S_LASTPASSWORD         = "Login/userPassword";
const char * const  S_DATEFORMAT           = "Dates/Format";
const char * const  S_PREFERREDLANGUAGE    = "Core/preferredLanguage";

const char * const  S_DEF_PATIENTFORMS_FILENAME  = "gp_basic1";

const char * const  S_USE_EXTERNAL_DATABASE  = "ExternalDatabase/UseIt";
const char * const  S_EXTERNAL_DATABASE_LOG  = "ExternalDatabase/Log";
const char * const  S_EXTERNAL_DATABASE_PASS = "ExternalDatabase/Pass";
const char * const  S_EXTERNAL_DATABASE_HOST = "ExternalDatabase/Host";
const char * const  S_EXTERNAL_DATABASE_PORT = "ExternalDatabase/Port";

const char * const  S_ALWAYS_SAVE_WITHOUT_PROMPTING = "Core/SaveInDatabaseWithoutPromptingUser";
const char * const  S_ALLOW_VIRTUAL_DATA            = "Core/AllowAndUseVirtuals";

const char * const  S_PROXY = "Core/Proxy";

const char * const  S_PATIENTCHANGEONCREATION = "Patients/SelectOnCreation";

// Printer correction
const char * const S_PRINTERCORRECTION_HORIZ_MM = "Printer/Correction/horiz_mm";
const char * const S_PRINTERCORRECTION_VERTIC_MM = "Printer/Correction/vertic_mm";
const char * const S_PRINT_DIRECTION = "Printer/Correction/direction";
// Settings Values
const char * const S_TOPTOBOTTOM = "topToBottom";
const char * const S_BOTTOMTOTOP = "bottomToTop";

}  // end Constants
} // end Core

#endif // CORE_CONSTANTS_TOKENS_SETTINGS_H
