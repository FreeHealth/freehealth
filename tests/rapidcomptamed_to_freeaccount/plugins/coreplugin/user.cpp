#include "user.h"

User::User(QObject *parent) :
        Core::IUser(parent)
{
}

User::~User()
{
}

void User::clear()
{
}

bool User::has(const int ref) const
{
}

QVariant User::value(const int ref) const
{
    switch (ref) {
    case Name:
    case SecondName:
    case Firstname:
    case Mail:
    case Language:
    case Locker:
    case LanguageIndex:
    case FullName:
    case GenderIndex:
    case TitleIndex:
    case Gender:
    case Title:
    case Adress:
    case Zipcode:
    case City:
    case Country:
    case Tel1:
    case Tel2:
    case Tel3:
    case Fax:
    case PractitionerId:
    case Specialities:
    case Qualifications:
    case Preferences:

    case GenericHeader:
    case GenericFooter:
    case GenericWatermark:
    case GenericHeaderPresence:
    case GenericFooterPresence:
    case GenericWatermarkPresence:
    case GenericWatermarkAlignement:
    default: return QVariant();
    }
}

bool User::setValue(const int ref, const QVariant &value)
{
}

QString User::toXml() const
{
    return QString();
}

bool User::fromXml(const QString &xml)
{
    Q_UNUSED(xml);
    return false;
}
