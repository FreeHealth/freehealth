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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef SCRIPTPATIENTWRAPPER_H
#define SCRIPTPATIENTWRAPPER_H

#include <QObject>
#include <QDate>
#include <QVariant>
#include <QString>
#include <QStringList>

/**
 * \file scriptpatientwrapper.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 23 May 2013
*/

namespace Script {
namespace Internal {

class ScriptPatientWrapper : public QObject  //, public QScriptClass
{
    Q_OBJECT
    Q_PROPERTY(bool     isActive    READ isActive())
    Q_PROPERTY(QString  usualName   READ usualName())
    Q_PROPERTY(QString  otherNames  READ otherNames())
    Q_PROPERTY(QString  firstName   READ firstName())
    Q_PROPERTY(QString  fullName    READ fullName())

    Q_PROPERTY(QString  street     READ street())
    Q_PROPERTY(QString  city       READ city())
    Q_PROPERTY(QString  zipcode    READ zipcode())
    Q_PROPERTY(QString  state      READ state())
    Q_PROPERTY(QString  country    READ country())

    Q_PROPERTY(QDate    dateOfBirth READ dateOfBirth())
    Q_PROPERTY(int      yearsOld    READ yearsOld())
    Q_PROPERTY(bool     isPediatric READ isPediatric())
    Q_PROPERTY(bool     isMale      READ isMale())
    Q_PROPERTY(bool     isFemale    READ isFemale())

    Q_PROPERTY(QStringList  socialNumbers    READ socialNumbers())
    Q_PROPERTY(QString  socialNumber1    READ socialNumber1())
    Q_PROPERTY(QString  socialNumber2    READ socialNumber2())
    Q_PROPERTY(QString  socialNumber3    READ socialNumber3())
    Q_PROPERTY(QString  socialNumber4    READ socialNumber4())
    Q_PROPERTY(QString  socialNumberOwnerName    READ socialNumberOwnerName())
    Q_PROPERTY(QString  socialNumberOwnerAddress    READ socialNumberOwnerAddress())

    Q_PROPERTY(double   weightInGrams READ weightInGrams())
    Q_PROPERTY(double   heightInCentimeters      READ heightInCentimeters())

    Q_PROPERTY(double   creatinine              READ creatinine())
    Q_PROPERTY(QString  creatinineUnit          READ creatinineUnit())
    Q_PROPERTY(double   clearanceCreatinine     READ clearanceCreatinine())
    Q_PROPERTY(QString  clearanceCreatinineUnit READ clearanceCreatinineUnit())

public:
    ScriptPatientWrapper(QObject *parent);

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

    QDate dateOfBirth() const;
    int yearsOld() const;
    bool isPediatric() const;
    bool isMale() const;
    bool isFemale() const;

    QStringList socialNumbers() const;
    QString socialNumber1() const;
    QString socialNumber2() const;
    QString socialNumber3() const;
    QString socialNumber4() const;
    QString socialNumberOwnerName() const;
    QString socialNumberOwnerAddress() const;

    double weightInGrams() const;
    double heightInCentimeters() const;
    double bmi() const;
    double imc() const {return bmi();}
    double creatinine() const;
    QString creatinineUnit() const;
    double clearanceCreatinine() const;
    QString clearanceCreatinineUnit() const;
};


}  // namespace Internal
}  //  End namespace Script

#endif // SCRIPTPATIENTWRAPPER_H
