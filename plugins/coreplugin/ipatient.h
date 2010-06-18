/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
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

#include <QVariant>
#include <QObject>

/**
 * \file ipatient.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.4.0
 * \date 18 June 2010
*/


namespace Core {

/**
  \brief Use this class to avoid any plugin dependencies (other than Core), when needing to access to patients datas.
*/

class CORE_EXPORT IPatient : public QObject
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
        Surname,
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
        DrugsAllergiesWithoutPrecision,
        DrugsUidAllergies,     //  see tkSerializer --> string<->stringlist
        DrugsAtcAllergies,     //  see tkSerializer --> string<->stringlist
        DrugsInnAllergies,     //  see tkSerializer --> string<->stringlist
        DrugsInnAtcAllergies,     //  see tkSerializer --> string<->stringlist
        DrugsChronicTherapeutics,
        ICD10Diseases,      //  see tkSerializer --> string<->stringlist

        // For debugging purpose
        PractitionnerLkID,
        NumberOfColumns
    };

    IPatient(QObject * parent = 0) : QObject(parent) {}
    virtual ~IPatient() {}

    virtual void clear() = 0;
    virtual bool has(const int ref) const = 0;

    virtual QVariant value(const int ref) const = 0;
    virtual bool setValue(const int ref, const QVariant &value) = 0;

    virtual QString toXml() const = 0;
    virtual bool fromXml(const QString &xml) = 0;

    void replaceTokens(QString &stringWillBeModified);

};

}  // End Core


#endif // IPATIENT_H
