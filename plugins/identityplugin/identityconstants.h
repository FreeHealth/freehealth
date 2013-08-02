/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developer: Eric Maeker <eric.maeker@gmail.com>                  *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef IDENTITYCONSTANTS_H
#define IDENTITYCONSTANTS_H

namespace Identity {
namespace Constants {

const char * const ACTION_ID = "Identity.Action";
const char * const MENU_ID = "Identity.Menu";

const char* const PASSWORD_SUMMARY_NOLOGIN_NOPASSWORD = QT_TRANSLATE_NOOP("UserPlugin", "Please choose login and password");
const char* const PASSWORD_SUMMARY_LOGIN_NOPASSWORD   = QT_TRANSLATE_NOOP("UserPlugin", "Login set, but password not");
const char* const PASSWORD_SUMMARY_LOGIN_PASSWORD     = QT_TRANSLATE_NOOP("UserPlugin", "Login and password are set");

const char* const UNCRYPTED_PASSWORD_AVAILABLE        = QT_TRANSLATE_NOOP("UserPlugin", "Uncrypted password available");
const char* const CRYPTED_PASSWORD_AVAILABLE          = QT_TRANSLATE_NOOP("UserPlugin", "Crypted password set");

} // namespace Identity
} // namespace Constants

#endif // IDENTITYCONSTANTS_H

