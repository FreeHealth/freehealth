/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
/**
  \class Core::IUser
  Is a model data for the currently logged user.
  \a Core::ICore::user()
*/

#include "iuser.h"
#include "constants_tokensandsettings.h"

#include <utils/global.h>

using namespace Core;

void IUser::replaceTokens(QString &stringWillBeModified)
{
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_USERNAME,       value(IUser::Name).toString());
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_USERFIRSTNAME,  value(IUser::Firstname).toString());
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_USERSECONDNAME, value(IUser::SecondName).toString());
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_USERFULLNAME,   value(IUser::FullName).toString());
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_USERTITLE,      value(IUser::Title).toString() );
//    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_USERAGE,        value(IUser::Age).toString() );
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_USERSPECIALITIES, value(IUser::Specialities).toStringList().join("<br />") );
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_USERIDENTIFIANTS, value(IUser::PractitionerId).toStringList().join("<br />") );
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_USERQUALIFICATIONS, value(IUser::Qualifications).toStringList().join("<br />") );

    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_USERADRESS,   value(IUser::Adress).toString());
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_USERMAIL,   value(IUser::Mail).toString());
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_USERTEL1,   value(IUser::Tel1).toString());
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_USERTEL2,   value(IUser::Tel2).toString());
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_USERTEL3,   value(IUser::Tel3).toString());
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_USERFAX,   value(IUser::Fax).toString());


    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_USERFULLCONTACT,   value(IUser::FullHtmlContact).toString());
    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_USERFULLADRESS,   value(IUser::FullHtmlAdress).toString());

//    Utils::replaceToken(stringWillBeModified, Constants::TOKEN_USERDATEOFBIRTH, value(IUser::DateOfBirth).toString() );
    /** \todo improve manage of token mail, tels, faxes */
}

