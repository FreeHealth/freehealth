/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
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
 * \author Eric MAEKER <eric.maeker@free.fr>
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
        DrugsUidAllergies,     //  see tkSerializer --> string<->stringlist
        DrugsAtcAllergies,     //  see tkSerializer --> string<->stringlist
        DrugsInnAllergies,     //  see tkSerializer --> string<->stringlist
        // Intolerances
        DrugsIntolerancesWithoutPrecision,
        DrugsUidIntolerances,     //  see tkSerializer --> string<->stringlist
        DrugsAtcIntolerances,     //  see tkSerializer --> string<->stringlist
        DrugsInnIntolerances,     //  see tkSerializer --> string<->stringlist
        DrugsInnAtcIntolerances,     //  see tkSerializer --> string<->stringlist

        DrugsChronicTherapeutics,
        ICD10Diseases,      //  see tkSerializer --> string<->stringlist

        // For debugging purpose
        PractitionnerLkID,
        NumberOfColumns
    };

    IPatient(QObject * parent = 0) : QAbstractListModel(parent) {}
    virtual ~IPatient() {}

    virtual void clear() = 0;
    virtual bool has(const int ref) const = 0;

    int	columnCount(const QModelIndex & = QModelIndex()) const {return NumberOfColumns;}
    int	rowCount(const QModelIndex & = QModelIndex()) const {return 1;}

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const = 0;
    virtual QVariant data(int column) const {return data(index(0, column));}

    virtual QString toXml() const = 0;
    virtual bool fromXml(const QString &xml) = 0;

    void replaceTokens(QString &stringWillBeModified);

Q_SIGNALS:
    void currentPatientChanged();
};

}  // End Core


#endif // IPATIENT_H
