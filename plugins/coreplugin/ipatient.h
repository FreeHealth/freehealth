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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef CORE_IPATIENT_H
#define CORE_IPATIENT_H

#include <coreplugin/core_exporter.h>

#include <QAbstractListModel>
#include <QHash>
#include <QString>

#include <QDebug>

/**
 * \file ipatient.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.4
 * \date 18 Apr 2013
*/

namespace Core {
class IToken;
class IPatientBar;

class CORE_EXPORT IPatient : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(PatientDataRepresentation)

public:
    enum PatientDataRepresentation {
        Id = 0,

        // Identity
        Uid,
        IsActive,
        IsVirtual,
        UsersUidList,
        GroupsUidList,
        FamilyUid,
        Title,
        TitleIndex,
        FullName,
        UsualName,            // 10
        OtherNames,
        Firstname,
        Gender,
        GenderIndex,
        IconizedGender,
        GenderPixmap,
        DateOfBirth,
        Age,
        YearsOld,
        MaritalStatus,        // 20
        Profession,
        ProfessionSite,
        DateOfDeath,
        Photo_32x32,
        Photo_64x64,

        // Contact
        Street,
        ZipCode,
        City,
        Country,
        StateProvince,       // 30
        AddressNote,
        FullAddress,
        Email,
        Tels,
        MobilePhone,
        Faxes,
        WorkPhone,
        ProfessionalSite,

        // Social numbers
        SocialNumber,
        SocialNumber2,      // 40
        SocialNumber3,
        SocialNumber4,
        SocialNumberOwnerFullName,      // If different from the patient
        SocialNumberOwnerFullAddress,   // If different from the patient

        // pediatrics
        IsPediatricToken, // returns space character if true, returns empty string if false
        IsPediatric, // returns true if patient age is equal or superior to pediatric
                     // age limit, false otherwise
        // female
        IsPregnant,
        LastMenstrualPeriod,

        // Biometrics
        SystolicBloodPressure,
        DiastolicBloodPressure,
        WeightInGrams,
        HeightInCentimeters,
        BMI,
        Creatinine,
        CreatinineUnit,
        CreatinClearance,
        CreatinClearanceUnit,

        // Drugs and diseases
        // Allergies
        DrugsAllergiesWithoutPrecision,
        DrugsAllergiesWithoutPrecisionHtml,
        DrugsUidAllergies,
        DrugsAtcAllergies,
        DrugsInnAllergies,
        // Intolerances
        DrugsIntolerancesWithoutPrecision,
        DrugsIntolerancesWithoutPrecisionHtml,
        DrugsUidIntolerances,
        DrugsAtcIntolerances,
        DrugsInnIntolerances,
        DrugsInnAtcIntolerances,

        DrugsChronicTherapeutics,
        DrugsChronicTherapeuticsHtml,
        ICD10Diseases,

        // For debugging purpose
        PractitionerUuid,
        NumberOfColumns
    };

    IPatient(QObject * parent = 0);
    virtual ~IPatient();

    virtual void clear() = 0;
    virtual bool has(const int ref) const = 0;

    /** Return the current active patient UUID */
    QString uuid() const {return data(Uid).toString();}

    /** Set the current active patient to \e uuid */
    virtual void setCurrentPatientUid(const QString &uid) {Q_UNUSED(uid);}

    int	columnCount(const QModelIndex & = QModelIndex()) const {return NumberOfColumns;}
    int	rowCount(const QModelIndex & = QModelIndex()) const {return 1;}

    virtual QModelIndex currentPatientIndex() const = 0;

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const = 0;
    virtual QVariant data(int column) const = 0;

    /** \todo remove this and use setData instead **/
    virtual bool setValue(int ref, const QVariant &value) = 0;

    virtual QString toXml() const = 0;
    virtual bool fromXml(const QString &xml) = 0;

    void registerPatientTokens();

    void replaceTokens(QString &stringWillBeModified);

    virtual Core::IPatientBar *patientBar() const = 0;
    virtual void hidePatientBar() = 0;
    virtual void showPatientBar() = 0;
    virtual bool isPatientBarVisible() const = 0;

    virtual QHash<QString, QString> fullPatientName(const QString &uuid) const {Q_UNUSED(uuid); return QHash<QString, QString>();}
    virtual QHash<QString, QString> fullPatientName(const QStringList &uuids) const {Q_UNUSED(uuids); return QHash<QString, QString>();}

    QString enumToString(PatientDataRepresentation data);

Q_SIGNALS:
    void currentPatientChanged();
    void patientCreated(const QString &uuid);

//private:
//    QVector<Core::IToken *> _tokens;
};

}  // End Core


#endif // IPATIENT_H
