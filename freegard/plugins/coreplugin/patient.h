/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FREEGUARD_PATIENT_H
#define FREEGUARD_PATIENT_H

#include <coreplugin/core_exporter.h>
#include <coreplugin/ipatient.h>

/**
 * \file patient.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 10 Oct 2014
*/
namespace Core {
class CORE_EXPORT Patient : public IPatient
{
    Q_OBJECT
public:
    Patient(QObject * = 0) {}
    ~Patient() {}

    void clear() {}
    bool has(const int) const {return false;}

    QModelIndex currentPatientIndex() const {return QModelIndex();}

    bool setData(const QModelIndex &, const QVariant &, int = Qt::EditRole) {return false;}
    bool setValue(int , const QVariant &) {return false;}

    QVariant data(const QModelIndex &, int = Qt::DisplayRole) const {return false;}
    QVariant data(int ) const {return QVariant();}

    QString toXml() const {return QString::null;}
    bool fromXml(const QString &) {return false;}

    Core::IPatientBar *patientBar() const {return 0;}
    void hidePatientBar() {}
    void showPatientBar() {}
    bool isPatientBarVisible() const {return false;}

};

}  // End Core

#endif // FREEGUARD_PATIENT_H
