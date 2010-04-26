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
 ***************************************************************************/
#ifndef PATIENTDATA_H
#define PATIENTDATA_H

#include <QVariant>
#include <QDateTime>


/**
 * \file patientdata.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.1
 * \date 16 Nov 2009
*/

class PatientData
{
public:
    PatientData();
    ~PatientData();

    // inalterable datas --> use a model
    QString uuid() const;
    QString name() const;
    QString surname() const;
    QString secondName() const;
    QDate dateOfBirth() const;

    // episodes
//    QList<PatientRecord *> records() const;

    // ask for a specific record
//    PatientRecord *getRecordsFor(const int formItemId, const QDateTime &approxDateTime);

};


class RecordValues;

class PatientRecord
{
public:
    enum PatientReferences {
        P_Name = 0,
        P_Surname,
        P_SecondName,
        P_DateOfBirth
    };

    PatientRecord();
    ~PatientRecord();

    void setValue(const int formItemId, RecordValues *value);
    RecordValues *value(const int formItemId);

    // Link to Users
    QStringList userOwners() const;
    bool ownsToUser(const QString &uuid);

    // lock this record (can be edited only once a time)
    bool isLocked() const;
    void setLocked(const bool state);

    // Dates
    QDateTime creationDate() const;
    void setCreationDate(const QDateTime &date);
    QDateTime lastModificationDate() const;
    void setLastModificationDate(const QDateTime &date = QDateTime::currentDateTime());

    // Link to UI
    int linkedFormItemId() const;

    QString trace() const;



};



class RecordValues
{
public:
    RecordValues();
    ~RecordValues();

//    Core::IFormItemData *formItemData() const;
};

#endif // PATIENTDATA_H
