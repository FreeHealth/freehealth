/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef IPATIENT_H
#define IPATIENT_H

#include <coreplugin/core_exporter.h>

#include <QAbstractListModel>

#include <QDebug>

/**
 * \file ipatient.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.5.0
 * \date 23 Aug 2010
*/


namespace Core {

/**
  \brief Use this class to avoid any plugin dependencies (other than Core), when needing to access to the \e current \e patient datas.
*/

class CORE_EXPORT IPatient : public QAbstractListModel
{
        Q_OBJECT
public:
    enum DataRepresentation {
        Id = 0,

        // Identity
        Uid,
        IsVirtual,
        UsersUidList,
        GroupsUidList,
        FamilyUid,
        IconizedGender,
        Title,
        TitleIndex,
        FullName,
        BirthName,
        SecondName,
        Firstname,
        Gender,
        GenderIndex,
        DateOfBirth,
        Age,
        YearsOld,
        MaritalStatus,
        Profession,
        DateOfDeath,
        Photo,

        // Contact
        Street,
        ZipCode,
        City,
        Country,
        AddressNote,
        FullAddress,
        Mails,
        Tels,
        Faxes,

        // Biometrics
        Weight,
        WeightUnit,
        Height,
        HeightUnit,
        IMC,
        Creatinine,
        CreatinineUnit,
        CreatinClearance,
        CreatinClearanceUnit,

        // Drugs and diseases
        // Allergies
        DrugsAllergiesWithoutPrecision,
        DrugsUidAllergies,
        DrugsAtcAllergies,
        DrugsInnAllergies,
        // Intolerances
        DrugsIntolerancesWithoutPrecision,
        DrugsUidIntolerances,
        DrugsAtcIntolerances,
        DrugsInnIntolerances,
        DrugsInnAtcIntolerances,

        DrugsChronicTherapeutics,
        ICD10Diseases,

        // For debugging purpose
        PractitionnerLkID,
        NumberOfColumns
    };

    IPatient(QObject * parent = 0) : QAbstractListModel(parent) {}
    virtual ~IPatient() {}

    virtual void clear() = 0;
    virtual bool has(const int ref) const = 0;

    QString uuid() const {return data(Uid).toString();}

    int	columnCount(const QModelIndex & = QModelIndex()) const {return NumberOfColumns;}
    int	rowCount(const QModelIndex & = QModelIndex()) const {return 1;}

    virtual QModelIndex currentPatientIndex() const = 0;

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const = 0;
    virtual QVariant data(int column) const = 0;

    /** \todo remove this and use setData instead **/
    virtual bool setValue(int ref, const QVariant &value) = 0;

    virtual QString toXml() const = 0;
    virtual bool fromXml(const QString &xml) = 0;

    void replaceTokens(QString &stringWillBeModified);

    virtual void hidePatientBar() = 0;
    virtual void showPatientBar() = 0;
    virtual bool isPatientBarVisible() const = 0;

Q_SIGNALS:
    void currentPatientChanged();
    void patientCreated(const QString &uuid);
};

}  // End Core


#endif // IPATIENT_H
