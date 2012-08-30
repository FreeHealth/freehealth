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
#ifndef PATIENTDATA_H
#define PATIENTDATA_H

#include <QVariant>
#include <QDateTime>


/**
 * \file patientdata.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.0.1
 * \date 16 Nov 2009
*/

class PatientData
{
public:
    PatientData();
    ~PatientData();

    // inalterable data --> use a model
    QString uuid() const;
    QString name() const;
    QString firstname() const;
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
        P_Firstname,
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
