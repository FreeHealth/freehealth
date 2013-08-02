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
 *   Main Developers:                                                     *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef SERVERIDENTIFICATION_H
#define SERVERIDENTIFICATION_H

#include <datapackutils/datapack_exporter.h>

#include <QString>

/**
 * \file packdescription.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.2
 * \date 25 Nov 2011
*/

namespace DataPack {

class DATAPACK_EXPORT ServerIdentification
{
public:
    ServerIdentification();

    void setLogin(const QString &login) {m_login=login;}
    void setUserName(const QString &user) {m_userName=user;}
    void setPrivateKey(const QString &key) {m_privateKey=key;}

    QString login() const {return m_login;}
    QString userName() const {return m_userName;}
    QString privateKey() const {return m_privateKey;}

private:
    QString m_login;
    QString m_userName;
    QString m_privateKey;

};

}  // End namespace DataPack

#endif // SERVERIDENTIFICATION_H
