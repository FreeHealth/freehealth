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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FREEACCOUNT_PATIENT_H
#define FREEACCOUNT_PATIENT_H

#include <coreplugin/core_exporter.h>
#include <coreplugin/ipatient.h>

/**
 * \file patient.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.5.0
 * \date 04 Dec 2010
*/
namespace Core {
namespace Internal {
class PatientPrivate;
}

class CORE_EXPORT Patient : public IPatient
{
    Q_OBJECT
public:
    Patient(QObject *parent = 0);
    ~Patient();

    void clear();
    bool has(const int ref) const;

    QModelIndex currentPatientIndex() const {return QModelIndex();}

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant data(int column) const;

    /** \todo remove this and use setData instead **/
    bool setValue(int ref, const QVariant &value);

    QString toXml() const;
    bool fromXml(const QString &xml);

    Core::IPatientBar *patientBar() const {return 0;}
    void hidePatientBar() {}
    void showPatientBar() {}
    bool isPatientBarVisible() const {return false;}

private:
    Internal::PatientPrivate *d;
};

}  // End Core

#endif // FREEACCOUNT_PATIENT_H
