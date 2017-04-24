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
#ifndef SCRIPTUSERWRAPPER_H
#define SCRIPTUSERWRAPPER_H

#include <QObject>
#include <QDate>
#include <QVariant>
#include <QString>
#include <QStringList>

/**
 * \file scriptuserwrapper.h
 * \author Eric Maeker
 * \version 0.8.2
 * \date 30 Dec 2012
*/

namespace Script {
namespace Internal {

class ScriptUserWrapper : public QObject  //, public QScriptClass
{
    Q_OBJECT
    Q_PROPERTY(bool     isActive    READ isActive())
    Q_PROPERTY(QString  usualName   READ usualName())
    Q_PROPERTY(QString  otherNames  READ otherNames())
    Q_PROPERTY(QString  firstName   READ firstName())
    Q_PROPERTY(QString  fullName    READ fullName())

    Q_PROPERTY(QString  street      READ street())
    Q_PROPERTY(QString  city        READ city())
    Q_PROPERTY(QString  zipcode     READ zipcode())
    Q_PROPERTY(QString  state       READ state())
    Q_PROPERTY(QString  country     READ country())
    Q_PROPERTY(QString  fullAddress READ fullAddress())

    Q_PROPERTY(QDate    dateOfBirth READ dateOfBirth())
    Q_PROPERTY(bool     isMale      READ isMale())
    Q_PROPERTY(bool     isFemale    READ isFemale())

    Q_PROPERTY(QStringList specialties READ specialties())
    Q_PROPERTY(QStringList qualifications READ qualifications())
    Q_PROPERTY(QStringList identifiers READ identifiers())

public:
    ScriptUserWrapper(QObject *parent);

public Q_SLOTS:
    bool isActive() const;

    QString fullName() const;
    QString usualName() const;
    QString otherNames() const;
    QString firstName() const;

    QString street() const;
    QString city() const;
    QString zipcode() const;
    QString state() const;
    QString country() const;
    QString fullAddress() const;

    QDate dateOfBirth() const;
    bool isMale() const;
    bool isFemale() const;

    QStringList specialties() const;
    QStringList qualifications() const;
    QStringList identifiers() const;

};

}  // namespace Internal
}  // namespace Script

#endif // SCRIPTUSERWRAPPER_H
