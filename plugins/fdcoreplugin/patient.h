/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
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
#ifndef FREEDIAMS_PATIENT_H
#define FREEDIAMS_PATIENT_H

#include <coreplugin/core_exporter.h>

#include <QVariant>

/**
 * \file patient.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.6
 * \date 16 Sept 2009
*/
namespace Core {
namespace Internal {
class PatientPrivate;
}

class CORE_EXPORT Patient
{
public:
    enum Reference {
        UID = 0,
        Name,
        Surname,
        Gender,
        DateOfBirth,
        Age,
        YearsOld,
        Weight,
        WeightUnit,
        Height,
        HeightUnit,
        IMC,
        Creatinine,
        CreatinineUnit,
        CreatinClearance,
        CreatinClearanceUnit,
        DrugsUidAllergies,     //  see tkSerializer --> string<->stringlist
        DrugsAtcAllergies,     //  see tkSerializer --> string<->stringlist
        DrugsInnAllergies,     //  see tkSerializer --> string<->stringlist
        DrugsInnAtcAllergies,     //  see tkSerializer --> string<->stringlist
        ICD10Deceases,      //  see tkSerializer --> string<->stringlist
    };

    Patient();
    ~Patient();

    void clear();
    bool has(const Reference ref ) const;

    QVariant value( Reference ref ) const;
    void setValue( Reference ref, const QVariant &value );

    QString toXml() const;
    bool fromXml(const QString &xml);

    void replaceTokens(QString &stringWillBeModified);

private:
    Internal::PatientPrivate *d;
};

}  // End Core

#endif // FREEDIAMS_PATIENT_H
