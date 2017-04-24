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
#include "scriptuserwrapper.h"

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>

using namespace Script;
using namespace Internal;

static inline Core::IUser *user() {return Core::ICore::instance()->user();}

ScriptUserWrapper::ScriptUserWrapper(QObject *parent) :
    QObject(parent)
{
}

bool ScriptUserWrapper::isActive() const
{
    return (!user()->uuid().isEmpty());
}
QString ScriptUserWrapper::fullName() const
{
    return user()->value(Core::IUser::FullName).toString();
}

QString ScriptUserWrapper::usualName() const
{
    return user()->value(Core::IUser::UsualName).toString();
}

QString ScriptUserWrapper::otherNames() const
{
    return user()->value(Core::IUser::OtherNames).toString();
}

QString ScriptUserWrapper::firstName() const
{
    return user()->value(Core::IUser::Firstname).toString();
}

QString ScriptUserWrapper::street() const
{
    return user()->value(Core::IUser::Street).toString();
}

QString ScriptUserWrapper::city() const
{
    return user()->value(Core::IUser::City).toString();
}

QString ScriptUserWrapper::zipcode() const
{
    return user()->value(Core::IUser::Zipcode).toString();
}

QString ScriptUserWrapper::state() const
{
    return user()->value(Core::IUser::StateProvince).toString();
}

QString ScriptUserWrapper::country() const
{
    return user()->value(Core::IUser::Country).toString();
}

QString ScriptUserWrapper::fullAddress() const
{
    return user()->value(Core::IUser::FullAddress).toString();
}

QDate ScriptUserWrapper::dateOfBirth() const
{
    return user()->value(Core::IUser::DateOfBirth).toDate();
}

bool ScriptUserWrapper::isMale() const
{
    return (user()->value(Core::IUser::GenderIndex).toInt()==0);
}

bool ScriptUserWrapper::isFemale() const
{
    return user()->value(Core::IUser::GenderIndex).toInt()==1;
}

QStringList ScriptUserWrapper::specialties() const
{
    return user()->value(Core::IUser::Specialities).toStringList();
}

QStringList ScriptUserWrapper::qualifications() const
{
    return user()->value(Core::IUser::Qualifications).toStringList();
}

QStringList ScriptUserWrapper::identifiers() const
{
    return user()->value(Core::IUser::ProfessionalIdentifiers).toStringList();
}
