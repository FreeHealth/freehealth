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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef PORT_RECOVERABLEDATA_H
#define PORT_RECOVERABLEDATA_H

#include <portplugin/port_exporter.h>

namespace Port {

class PORT_EXPORT RecoverableData
{
public:
    enum PatientIdentityData {
        PatientIdentityNoData = 0x00000000,
        PatientName         = 0x00000001,
        PatientFirstName    = 0x00000002,
        PatientSecondName   = 0x00000004,
        PatientDateOfBirth  = 0x00000100,
        PatientTitle        = 0x00000200,
        PatientGender       = 0x00000400,
        PatientDateOfDeath  = 0x00000800,
        PatientSocialNumber = 0x00010000,
        PatientIsOwnerOfSocialNumber = 0x00020000,
        PatientSocialNumberOwner     = 0x00040000,
        AllIdentityData     = 0xFFFFFFFF
    };
    Q_DECLARE_FLAGS(PatientIdentitiesData, PatientIdentityData)

    enum PatientContactData {
        PatientContactNoData = 0x00000000,
        PatientAdress       = 0x00000001,
        PatientCity         = 0x00000002,
        PatientZipCode      = 0x00000004,
        PatientCountry      = 0x00000008,
        PatientFirstTel     = 0x00000100,
        PatientSecondTel    = 0x00000200,
        PatientThirdTel     = 0x00000400,
        PatientMobilePhone  = 0x00000800,
        PatientEMail        = 0x00001000,
        PatientFax          = 0x00002000,
        AllContactData      = 0xFFFFFFFF
    };
    Q_DECLARE_FLAGS(PatientContactsData, PatientContactData)

    enum PatientBiometricData {
        PatientBiometricNoData = 0x00000000,
        ArterialTension     = 0x00000001,
        Weight              = 0x00000002,
        WeightUnit          = 0x00000004,
        Height              = 0x00000008,
        HeightUnit          = 0x00000010,
        AllBiometricData    = 0xFFFFFFFF
    };
    Q_DECLARE_FLAGS(PatientBiometricsData, PatientBiometricData)

    enum PmhData {
        PmhNoData           = 0x00000000,
        PmhTextLabel        = 0x00000001,
        PmhStartDate        = 0x00000002,
        PmhEndDate          = 0x00000004,
        PmhConfidenceIndex  = 0x00000008,
        PmhICD10Code        = 0x00000010,
        PmhCategory         = 0x00000020,
        PmhContact          = 0x00000040,
        AllPmhData          = 0xFFFFFFFF
    };
    Q_DECLARE_FLAGS(PMHxData, PmhData)

    enum DrugAllergyData {
        DrugAllergy_NoData          = 0x00000000,
        DrugAllergy_DrugBrandName   = 0x00000001,
        DrugAllergy_ATC_Code        = 0x00000002,
        DrugAllergy_UserComment     = 0x00000004,
        DrugAllergy_DrugName        = 0x00000008,
        AllDrugAllergyData          = 0xFFFFFFFF
    };
    Q_DECLARE_FLAGS(DrugAllergiesData, DrugAllergyData)

    enum ChronicTherapyData {
        ChronicTherapy_NoData               = 0x00000000,
        ChronicTherapy_AllDrugBrandName     = 0x00000001,
        ChronicTherapy_DrugPosology         = 0x00000002,
        ChronicTherapy_DrugNote             = 0x00000004,
        ChronicTherapy_LastPrescriptionDate = 0x00000008,
        AllChronicTherapyData               = 0xFFFFFFFF
    };
    Q_DECLARE_FLAGS(ChronicTherapiesData, ChronicTherapyData)

    enum PrescriptionData {
        Prescription_NoData                 = 0x00000000,
        Prescriptions_AllDrugBrandName      = 0x000000001,
        Prescriptions_DrugPosology          = 0x000000002,
        Prescriptions_DrugNote              = 0x000000004,
        Prescriptions_Date                  = 0x000000008,
        AllPrescriptionsData                = 0xFFFFFFFFF
    };
    Q_DECLARE_FLAGS(PrescriptionsData, PrescriptionData)

    enum FormData {
        Forms_NoData                = 0x00000000,
        Form_EpisodeDate            = 0x000000001,
        Form_EpisodeLabel           = 0x000000002,
        Form_EpisodeUserCreator     = 0x000000004,
        Form_Content                = 0x000000008,
        AllFormsData                = 0xFFFFFFFFF
    };
    Q_DECLARE_FLAGS(FormsData, FormData)

    enum UserData {
        User_NoData         = 0x00000000,
        User_FullName       = 0x00000001,
        User_Title          = 0x00000002,
        User_Gender         = 0x00000004,
        User_Profession     = 0x00000008,
        User_Rights         = 0x00000010,
        User_Adress         = 0x00000020,
        User_City           = 0x00000040,
        User_ZipCode        = 0x00000080,
        User_Country        = 0x00000100,
        User_Tel1           = 0x00000200,
        User_Tel2           = 0x00000400,
        User_Tel3           = 0x00000800,
        User_MobilePhone    = 0x00001000,
        User_Fax            = 0x00002000,
        User_Mail           = 0x00004000,
        AllUserData         = 0xFFFFFFFF
    };
    Q_DECLARE_FLAGS(UsersData, UserData)

    enum AlertData {
        Alert_NoData            = 0x00000000,
        Alert_Label             = 0x00000001,
        Alert_Priority          = 0x00000002,
        Alert_FreeMedFormsType  = 0x00000004,
        Alert_StartDate         = 0x00000008,
        Alert_Cycling           = 0x00000010,
        Alert_ExpirationDate    = 0x00000020,
        AllAlertData            = 0xFFFFFFFF
    };
    Q_DECLARE_FLAGS(AlertsData, AlertData)

    // TODO: Lab results, scan docs, mails, e-mails, correspondants, vaccines...

    RecoverableData();

    void setPatientIdentity(PatientIdentitiesData data) {_patientIdentity = data;}
    void setPatientContact(PatientContactsData data) {_patientContact = data;}
    void setPatientBiometrics(PatientBiometricsData data) {_patientBiometrics = data;}
    void setPmhData(PMHxData data) {_pmhData = data;}
    void setDrugAllergy(DrugAllergiesData data) {_drugAllergy = data;}
    void setChronicTherapy(ChronicTherapiesData data) {_chronicTherapy = data;}
    void setPrescription(PrescriptionsData data) {_prescription = data;}
    void setForm(FormsData data) {_form = data;}
    void setUser(UsersData data) {_user = data;}
    void setAlert(AlertsData data) {_alert = data;}

    PatientIdentitiesData patientIdentity() const {return _patientIdentity;}
    PatientContactsData patientContact() const {return _patientContact;}
    PatientBiometricsData patientBiometrics() const {return _patientBiometrics;}
    PMHxData pmhData() const {return _pmhData;}
    DrugAllergiesData drugAllergy() const {return _drugAllergy;}
    ChronicTherapiesData chronicTherapy() const {return _chronicTherapy;}
    PrescriptionsData prescription() const {return _prescription;}
    FormsData form() const {return _form;}
    UsersData user() const {return _user;}
    AlertsData alert() const {return _alert;}

private:
    PatientIdentitiesData _patientIdentity;
    PatientContactsData _patientContact;
    PatientBiometricsData _patientBiometrics;
    PMHxData _pmhData;
    DrugAllergiesData _drugAllergy;
    ChronicTherapiesData _chronicTherapy;
    PrescriptionsData _prescription;
    FormsData _form;
    UsersData _user;
    AlertsData _alert;
};

} // namespace Port

Q_DECLARE_OPERATORS_FOR_FLAGS(Port::RecoverableData::PatientIdentitiesData)
Q_DECLARE_OPERATORS_FOR_FLAGS(Port::RecoverableData::PatientContactsData)
Q_DECLARE_OPERATORS_FOR_FLAGS(Port::RecoverableData::PatientBiometricsData)
Q_DECLARE_OPERATORS_FOR_FLAGS(Port::RecoverableData::PMHxData)
Q_DECLARE_OPERATORS_FOR_FLAGS(Port::RecoverableData::DrugAllergiesData)
Q_DECLARE_OPERATORS_FOR_FLAGS(Port::RecoverableData::ChronicTherapiesData)
Q_DECLARE_OPERATORS_FOR_FLAGS(Port::RecoverableData::PrescriptionsData)
Q_DECLARE_OPERATORS_FOR_FLAGS(Port::RecoverableData::FormsData)
Q_DECLARE_OPERATORS_FOR_FLAGS(Port::RecoverableData::UsersData)
Q_DECLARE_OPERATORS_FOR_FLAGS(Port::RecoverableData::AlertsData)

#endif // PORT_RECOVERABLEDATA_H
