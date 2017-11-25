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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
#include "user.h"

#include <coreplugin/isettings.h>
#include <coreplugin/icore.h>
#include <coreplugin/constants_tokensandsettings.h>

#include <drugsbaseplugin/constants.h>

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}

namespace Core {
namespace Constants {

    const char * const FREEDIAMS_DEFAULT_USER_UUID = "freediams.default.user";

}
}

using namespace Core;
using namespace Internal;

User::User(QObject *parent) :
        Core::IUser(parent)
{
}

User::~User()
{}

void User::clear()
{
}

bool User::has(const int ref) const
{
    if (ref >= 0 && ref < NumberOfColumns)
        return true;
    return false;
}


QVariant User::value(const int ref) const
{
    if (!has(ref))
        return QVariant();

    switch (ref) {
    case Id : return -1;
    case Uuid : return Constants::FREEDIAMS_DEFAULT_USER_UUID;;
    case Validity: return true;
//    case Login:
//    case Password:
//    case LastLogin:
//    case Name:
//    case SecondName:
//    case Firstname:
//    case Mail:
//    case Language:
//    case Locker:
//    case LanguageIndex:

        // ORDER CAN CHANGE
//    case FullName:
//    case GenderIndex:
//    case TitleIndex:
//    case Gender:
//    case Title:
//    case Adress:
//    case Zipcode:
//    case City:
//    case Country:
//    case Tel1:
//    case Tel2:
//    case Tel3:
//    case Fax:
//    case PractitionerId:
//    case Specialities:
//    case Qualifications:
//    case Preferences:

//    case GenericHeader:
//    case GenericFooter:
//    case GenericWatermark:

//    case AdministrativeHeader:
//    case AdministrativeFooter:
//    case AdministrativeWatermark:

    case PrescriptionHeader: return settings()->value(DrugsDB::Constants::S_USERHEADER);
    case PrescriptionFooter: return settings()->value(DrugsDB::Constants::S_USERFOOTER);
    case PrescriptionWatermark: return settings()->value(DrugsDB::Constants::S_WATERMARK_HTML);
//    case PrescriptionHeaderPresence: return ;
//    case PrescriptionFooterPresence:
    case PrescriptionWatermarkPresence: return settings()->value(DrugsDB::Constants::S_WATERMARKPRESENCE);
    case PrescriptionWatermarkAlignement: return settings()->value(DrugsDB::Constants::S_WATERMARKALIGNEMENT);

//    case ManagerRights:
    case DrugsRights: return IUser::AllRights;
    case MedicalRights: return IUser::AllRights;
//    case ParamedicalRights:
//    case AdministrativeRights:

//    case DecryptedLogin:
//    case LoginHistory:
    case IsModified: settings()->sync(); return false;
//    case Warn:
//    case WarnText:
    default: return QVariant();
    }

    return QVariant();
}

bool User::setValue(const int ref, const QVariant &value)
{
    // TODO: code this ??? FREEDIAMS::User::setValue(const int ref, const QVariant &value)*/
    Q_UNUSED(ref);
    Q_UNUSED(value);
    return true;
}

QString User::toXml() const
{
    return QString();
}

bool User::fromXml(const QString &xml)
{
    Q_UNUSED(xml);
    return true;
}


