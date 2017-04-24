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
  \class Core::IUser
  Is a model data for the currently logged user.
  Use this class to avoid any plugin dependencies (other than Core),
  when needing access to user's data.
  \a Core::ICore::user()
*/

#include "iuser.h"
#include "constants_tokensandsettings.h"

#include <utils/global.h>
#include <utils/log.h>
#include <translationutils/constants.h>
#include <translationutils/trans_patient.h>
#include <translationutils/trans_user.h>
#include <translationutils/trans_current.h>
#include <translationutils/trans_menu.h>

#include <coreplugin/icore.h>
#include <coreplugin/ipadtools.h>

using namespace Core;

static inline Core::IUser *user() {return Core::ICore::instance()->user();}
static inline Core::ITokenPool *tokenPool() {return Core::ICore::instance()->padTools()->tokenPool();}

namespace {

const char * const NAMESPACE_DESCRIPTION = QT_TRANSLATE_NOOP("tkConstants", "This token namespace contains all user's related tokens.");

class UserToken : public Core::IToken
{
public:
    UserToken(const QString &name, const int ref) :
        IToken(name),
        _ref(ref)
    {
    }

    QVariant testValue() const {return uid();}
    QVariant value() const {if (user()) return user()->value(_ref); return QVariant();}

    int sortIndex() const {return _ref;}

private:
    int _ref;
};
}

IUser::IUser(QObject *parent) : QObject(parent)
{
    setObjectName("Core::IUser");
}

IUser::~IUser()
{}

void IUser::registerUserTokens() const
{
#ifndef WITH_PAD
    return;
#else
    // Create and register namespaces
    TokenNamespace userNs(Constants::TOKEN_USER_NAMESPACE);
    userNs.setUntranslatedHumanReadableName(Trans::Constants::USER);
    userNs.setUntranslatedHelpText(::NAMESPACE_DESCRIPTION);
    userNs.setUntranslatedTooltip(::NAMESPACE_DESCRIPTION);

    TokenNamespace userIdentNs("Identity");
    userIdentNs.setUntranslatedHumanReadableName(Trans::Constants::IDENTITY_TEXT);

    TokenNamespace userAgeNs("Age");
    userAgeNs.setUntranslatedHumanReadableName(Trans::Constants::AGE);

    TokenNamespace userContactNs("Contact");
    userContactNs.setUntranslatedHumanReadableName(Trans::Constants::CONTACT);

    TokenNamespace userProfNs("Professional");
    userProfNs.setUntranslatedHumanReadableName(Trans::Constants::PROFESSIONAL);

    userNs.addChild(userIdentNs);
    userNs.addChild(userAgeNs);
    userNs.addChild(userContactNs);
    userNs.addChild(userProfNs);
    if (tokenPool())
        tokenPool()->registerNamespace(userNs);

    // Create tokens
    Core::IToken *t;
    QVector<Core::IToken *> _tokens;
    t = new UserToken(Constants::TOKEN_USERUSUALNAME, UsualName);
    t->setUntranslatedHumanReadableName(Trans::Constants::USUALNAME);
    _tokens << t;

    t = new UserToken(Constants::TOKEN_USEROTHERNAMES, OtherNames);
    t->setUntranslatedHumanReadableName(Trans::Constants::OTHERNAMES);
    _tokens << t;

    t = new UserToken(Constants::TOKEN_USERFIRSTNAME, Firstname);
    t->setUntranslatedHumanReadableName(Trans::Constants::FIRSTNAME);
    _tokens << t;

    t = new UserToken(Constants::TOKEN_USERFULLNAME, FullName);
    t->setUntranslatedHumanReadableName(Trans::Constants::FULLNAME);
    _tokens << t;

    t = new UserToken(Constants::TOKEN_USERDATEOFBIRTH, DateOfBirth);
    t->setUntranslatedHumanReadableName(Trans::Constants::DATE_OF_BIRTH);
    _tokens << t;

    //    t = new UserToken(Constants::TOKEN_USERYEARSOLD, YearsOld);
//    t->setUntranslatedHumanReadableName(Trans::Constants::);
//    _tokens << t;

//    t = new UserToken(Constants::TOKEN_USERAGE, Age);
//    t->setUntranslatedHumanReadableName(Trans::Constants::);
//    _tokens << t;

    t = new UserToken(Constants::TOKEN_USERTITLE, Title);
    t->setUntranslatedHumanReadableName(Trans::Constants::TITLE);
    _tokens << t;

    t = new UserToken(Constants::TOKEN_USERSPECIALITIES, Specialities);
    t->setUntranslatedHumanReadableName(Trans::Constants::SPECIALTIES);
    _tokens << t;

    t = new UserToken(Constants::TOKEN_USERIDENTIFIER_OLD, ProfessionalIdentifiers);
    t->setUntranslatedHumanReadableName(Trans::Constants::IDENTIFIERS_OLD);
    _tokens << t;

    t = new UserToken(Constants::TOKEN_USERIDENTIFIER, ProfessionalIdentifiers);
    t->setUntranslatedHumanReadableName(Trans::Constants::IDENTIFIERS);
    _tokens << t;

    t = new UserToken(Constants::TOKEN_USERQUALIFICATIONS, Qualifications);
    t->setUntranslatedHumanReadableName(Trans::Constants::QUALIFICATIONS);
    _tokens << t;

    t = new UserToken(Constants::TOKEN_USERFULLCONTACT_HTML, FullHtmlContact);
    t->setUntranslatedHumanReadableName(Trans::Constants::FULLCONTACT_HTML);
    _tokens << t;

    t = new UserToken(Constants::TOKEN_USERFULLADDRESS_HTML, FullHtmlAddress);
    t->setUntranslatedHumanReadableName(Trans::Constants::FULLADDRESS_HTML);
    _tokens << t;

    t = new UserToken(Constants::TOKEN_USERFULLADDRESS, FullAddress);
    t->setUntranslatedHumanReadableName(Trans::Constants::FULLADDRESS);
    _tokens << t;

    t = new UserToken(Constants::TOKEN_USERSTREET, Street);
    t->setUntranslatedHumanReadableName(Trans::Constants::STREET);
    _tokens << t;

    t = new UserToken(Constants::TOKEN_USERZIP, Zipcode);
    t->setUntranslatedHumanReadableName(Trans::Constants::ZIPCODE);
    _tokens << t;

    t = new UserToken(Constants::TOKEN_USERCITY, City);
    t->setUntranslatedHumanReadableName(Trans::Constants::CITY);
    _tokens << t;

    t = new UserToken(Constants::TOKEN_USERSTATE, StateProvince);
    t->setUntranslatedHumanReadableName(Trans::Constants::STATE);
    _tokens << t;

    t = new UserToken(Constants::TOKEN_USERCOUNTRY, Country);
    t->setUntranslatedHumanReadableName(Trans::Constants::COUNTRY);
    _tokens << t;

    t = new UserToken(Constants::TOKEN_USERMAIL, Mail);
    t->setUntranslatedHumanReadableName(Trans::Constants::MAIL);
    _tokens << t;

    t = new UserToken(Constants::TOKEN_USERTEL1, Tel1);
    t->setUntranslatedHumanReadableName(Trans::Constants::TELS);
    _tokens << t;

    t = new UserToken(Constants::TOKEN_USERTEL2, Tel2);
    t->setUntranslatedHumanReadableName(Trans::Constants::TELS);
    _tokens << t;

    t = new UserToken(Constants::TOKEN_USERTEL3, Tel3);
    t->setUntranslatedHumanReadableName(Trans::Constants::TELS);
    _tokens << t;

    t = new UserToken(Constants::TOKEN_USERFAX, Fax);
    t->setUntranslatedHumanReadableName(Trans::Constants::FAX);
    _tokens << t;

    if (tokenPool()) {
        LOG("Registering Core::IUser tokens");
        tokenPool()->addTokens(_tokens);
    } else {
        LOG_ERROR("PadTools object is not available, can not register the Core::IUser tokens");
    }
#endif
}

void IUser::replaceTokens(QString &stringWillBeModified)
{
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_USERUSUALNAME,  value(IUser::UsualName).toString());
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_USEROTHERNAMES, value(IUser::OtherNames).toString());
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_USERFIRSTNAME,  value(IUser::Firstname).toString());
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_USERFULLNAME,   value(IUser::FullName).toString());
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_USERTITLE,      value(IUser::Title).toString() );
//    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_USERAGE,        value(IUser::Age).toString() );
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_USERSPECIALITIES, value(IUser::Specialities).toStringList().join("<br />") );
    // TOKEN_USERIDENTIFIER_OLD corresponds to User.Professional.Identifiants
    // We need to keep it until users modify their forms and documents
    // to support the new token User.Professional.Identifiers
    // "identifiers" is the correct English word, "identifiants" is a French word.
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_USERIDENTIFIER_OLD, value(IUser::ProfessionalIdentifiers).toStringList().join("<br />") );
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_USERIDENTIFIER, value(IUser::ProfessionalIdentifiers).toStringList().join("<br />") );
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_USERQUALIFICATIONS, value(IUser::Qualifications).toStringList().join("<br />") );

    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_USERFULLADDRESS,   value(IUser::FullAddress).toString());
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_USERMAIL,   value(IUser::Mail).toString());
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_USERTEL1,   value(IUser::Tel1).toString());
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_USERTEL2,   value(IUser::Tel2).toString());
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_USERTEL3,   value(IUser::Tel3).toString());
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_USERFAX,   value(IUser::Fax).toString());


    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_USERFULLCONTACT,   value(IUser::FullHtmlContact).toString());
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_USERFULLADDRESS,   value(IUser::FullHtmlAddress).toString());

//    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_USERDATEOFBIRTH, value(IUser::DateOfBirth).toString() );
    // TODO: improve manage of token mail, tels, faxes
}

bool IUser::isAdministrator() const
{
    // get currently connected user's ManagerRights
    QVariant currentUserRights = Core::ICore::instance()->user()->value(Core::IUser::ManagerRights);

    // If currently connected user has AllRight for ManagerRights, it is an administrative account
    return(Core::IUser::UserRight(currentUserRights.toInt()) == Core::IUser::AllRights);
}
