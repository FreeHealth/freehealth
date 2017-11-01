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
/***************************************************************************
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef USERMODELWRAPPER_H
#define USERMODELWRAPPER_H

#include <coreplugin/iuser.h>

/**
 * \file usermodelwrapper.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 27 Apr 2012
*/

namespace MainWin {
namespace Internal {

class UserModelWrapper : public Core::IUser
{
    Q_OBJECT
public:
    UserModelWrapper(QObject *parent = 0);
    ~UserModelWrapper();

    void init();

    // IUser interface
    void clear() {}
    bool has(const int ref) const {return (ref>=0 && ref<Core::IUser::NumberOfColumns);}
    virtual bool hasCurrentUser() const {return true;}

    virtual QVariant value(const int ref) const;
    virtual bool setValue(const int ref, const QVariant &value);

    virtual QString toXml() const {return QString();}
    virtual bool fromXml(const QString &xml)  {Q_UNUSED(xml); return true;}

    virtual bool saveChanges() {return true;}

    virtual QString fullNameOfUser(const QVariant &uid) {Q_UNUSED(uid);return QString::null;}

};

}  // End namespace Internal
}  // End namespace MainWin

#endif // USERMODELWRAPPER_H
