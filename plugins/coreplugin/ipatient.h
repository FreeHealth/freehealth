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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef IPATIENT_H
#define IPATIENT_H

#include <coreplugin/core_exporter.h>

#include <QAbstractListModel>
#include <QHash>
#include <QString>

#include <QDebug>

/**
 * \file ipatient.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.7.6
 * \date 28 Jun 2012
*/

namespace Core {
class IToken;

class CORE_EXPORT IPatient : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(PatientDataRepresentation)

public:
    enum PatientDataRepresentation {
        Id = 0,

        // Identity
        Uid,
        IsVirtual,
        UsersUidList,
        GroupsUidList,
        FamilyUid,
        Title,
        TitleIndex,
        FullName,
        BirthName,
        SecondName,           // 10
        Firstname,
        Gender,
        GenderIndex,
        IconizedGender,
        GenderPixmap,
        DateOfBirth,
        Age,
        YearsOld,
        MaritalStatus,
        Profession,           // 20
        ProfessionSite,
        DateOfDeath,
        Photo_32x32,
        Photo_64x64,

        // Contact
        Street,
        ZipCode,
        City,
        Country,
        AddressNote,
        FullAddress,
        Mails,             // 30
        Tels,
        MobilePhone,
        Faxes,
        ProfessionalTels,
        ProfessionalSite,

        // Social numbers
        SocialNumber,
        SocialNumber2,
        SocialNumber3,
        SocialNumber4,

        // Biometrics
        Weight,
        WeightUnit,
        Height,
        HeightUnit,         // 40
        BMI,
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

    IPatient(QObject * parent = 0);
    virtual ~IPatient();

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

    void registerPatientTokens();

    void replaceTokens(QString &stringWillBeModified);

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
