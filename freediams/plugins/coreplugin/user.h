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
#ifndef USER_H
#define USER_H

#include <coreplugin/core_exporter.h>
#include <coreplugin/iuser.h>

namespace Core {
namespace Internal {

class User : public Core::IUser
{
    Q_OBJECT
public:
    User(QObject *parent);
    ~User();

    void clear();
    bool has(const int ref) const;
    virtual bool hasCurrentUser() const {return true;}

    QString fullNameOfUser(const QVariant &uid) {Q_UNUSED(uid); return QString();}

    QVariant value(const int ref) const;
    bool setValue(const int ref, const QVariant &value);

    bool saveChanges() {return false;}

    QString toXml() const ;
    bool fromXml(const QString &xml);

};

}
}

#endif // USER_H
