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
 *   Main developers : Eric Maeker
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef USERPLUGIN_INTERNAL_COREUSERMODELWRAPPER_H
#define USERPLUGIN_INTERNAL_COREUSERMODELWRAPPER_H

#include <coreplugin/iuser.h>

/**
 * \file coreusermodelwrapper.h
 * \author Eric Maeker
 * \version 0.8.0
 * \date 17 Dec 2012
*/

namespace UserPlugin {
class UserModel;
namespace Internal {
class CoreUserModelWrapperPrivate;

class CoreUserModelWrapper : public Core::IUser
{
    Q_OBJECT
public:
    explicit CoreUserModelWrapper(QObject *parent = 0);
    ~CoreUserModelWrapper();
    bool initialize(UserModel *model);

    // IUser interface
    void clear() {}
    bool has(const int ref) const;
    bool hasCurrentUser() const;

    QVariant value(const int ref) const;
    bool setValue(const int ref, const QVariant &value);

    QString toXml() const {return QString();}
    bool fromXml(const QString &) {return true;}

    bool saveChanges();

    QString fullNameOfUser(const QVariant &uid);

private Q_SLOTS:
    void newUserConnected(const QString &uid);

private:
    Internal::CoreUserModelWrapperPrivate *d;
};

} // namespace Internal
} // namespace UserPlugin

#endif // USERPLUGIN_INTERNAL_COREUSERMODELWRAPPER_H

