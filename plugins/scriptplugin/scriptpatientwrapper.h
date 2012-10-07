/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef SCRIPTPATIENTWRAPPER_H
#define SCRIPTPATIENTWRAPPER_H

#include <QObject>
#include <QDate>
#include <QVariant>
#include <QString>

/**
 * \file scriptpatientwrapper.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.7.6
 * \date 01 Jun 2012
*/

namespace Script {
namespace Internal {

class ScriptPatientWrapper : public QObject  //, public QScriptClass
{
    Q_OBJECT
    Q_PROPERTY(bool     isActive    READ isActive())
    Q_PROPERTY(QString  fullName    READ fullName())
    Q_PROPERTY(QDate    dateOfBirth READ dateOfBirth())
    Q_PROPERTY(int      yearsOld    READ yearsOld())
    Q_PROPERTY(bool     isMale      READ isMale())
    Q_PROPERTY(bool     isFemale    READ isFemale())
    Q_PROPERTY(double   weight      READ weight())
    Q_PROPERTY(QString  weightUnit  READ weightUnit())
    Q_PROPERTY(double   height      READ height())
    Q_PROPERTY(QString  heightUnit  READ heightUnit())

    Q_PROPERTY(double   creatinine              READ creatinine())
    Q_PROPERTY(QString  creatinineUnit          READ creatinineUnit())
    Q_PROPERTY(double   clearanceCreatinine     READ clearanceCreatinine())
    Q_PROPERTY(QString  clearanceCreatinineUnit READ clearanceCreatinineUnit())

public:
    ScriptPatientWrapper(QObject *parent);

public Q_SLOTS:
    bool isActive() const;
    QString fullName() const;
    QDate dateOfBirth() const;
    int yearsOld() const;
    bool isMale() const;
    bool isFemale() const;
    double weight() const;
    QString weightUnit() const;
    double height() const;
    QString heightUnit() const;
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
