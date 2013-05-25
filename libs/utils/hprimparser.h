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
 *   Main developers : Eric Maeker
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef UTILS_HPRIM_HPRIMPARSER_H
#define UTILS_HPRIM_HPRIMPARSER_H

#include <utils/global_exporter.h>
#include <QHash>
#include <QString>
#include <QList>
#include <QDate>

/**
 * \file hprimparser.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 23 May 2013
*/

namespace Utils {
namespace HPRIM {

class UTILS_EXPORT HprimHeader
{
public:
    enum {
        PatientId = 0,
        PatientName,
        PatientFirstName,
        PatientAddressFirstLine,
        PatientAddressSecondLine,
        PatientAddressZipCode,
        PatientAddressCity,
        PatientDateOfBirth,  // Format must be "dd/MM/yyyy"
        PatientSocialNumber,
        ExtraCode,
        DateOfExamination,
        SenderIdentity,
        ReceiverIdentity
    };
    HprimHeader();
    ~HprimHeader();

    bool isValid() const;
    bool isNull() const;

    bool setData(const int ref, const QString &value);
    QString data(const int ref) const;

    QString patientId() const {return data(PatientId);}
    QString patientName() const {return data(PatientName);}
    QString patientFirstName() const {return data(PatientFirstName);}
    QDate patientDateOfBirth() const {return QDate::fromString(data(PatientDateOfBirth), "dd/MM/yyyy");}
    QDate dateOfExamination() const {return QDate::fromString(data(DateOfExamination), "dd/MM/yyyy");}

    void setRawSource(const QString &fullContent);
    QString rawSource() const;

private:
    QHash<int, QString> _data;
    mutable QString _fullContent;
};

class UTILS_EXPORT HprimRawContent
{
public:
    HprimRawContent();
    ~HprimRawContent();

    bool isValid() const;
    bool isNull() const;

    void setRawSource(const QString &fullContent);
    QString rawSource() const;

private:
    mutable QString _fullContent;
};

class UTILS_EXPORT Hprim2Content
{
public:
    enum AvailableRawData {
        TypeOfSegment = 0,
        TestLabel,
        TestCode,
        TypeOfResult,
        TestResult1Value,
        TestResult1Units,
        TestResult1LowNorm,
        TestResult1HighNorm,
        TestResult1Abnormality,
        TestResult1Status,
        TestResult2Value,
        TestResult2Units,
        TestResult2LowNorm,
        TestResult2HighNorm,
        MaxRawData
    };

    enum Segment {
        Textual,
        Encoded
    };
    enum Result {
        Numeric,
        Code
    };
    enum Abnormality {
        Normal,
        Low,
        VeryLow,
        High,
        VeryHigh
    };
    enum Status {
        Validated,
        NonValidated,
        ModifiedCorrected
    };

    Hprim2Content();
    Hprim2Content(const HprimRawContent &rawContent);
    ~Hprim2Content();

    bool parseContent(const HprimRawContent &rawContent);

    bool isValid() const;
    bool isNull() const;

    int numberOfLines() const;
    QString rawData(int lineNumber, int rawDataIndex) const;

private:
    QHash<int, QStringList> _lines;
    bool _parseError;
};

class UTILS_EXPORT HprimMessage {
public:
    HprimMessage();
    ~HprimMessage();

    bool isValid() const;
    bool isNull() const;

    void setHeader(const HprimHeader &header);
    const HprimHeader &header() const;

    void setRawContent(const HprimRawContent &rawContent);
    const HprimRawContent &rawContent() const;

    QString toRawSource() const;

private:
    HprimHeader _header;
    HprimRawContent _rawContent;
};

UTILS_EXPORT HprimMessage &parseHprimRawSource(const QString &fullMessage);
UTILS_EXPORT HprimRawContent &createMessageRawContent(const QString &plainTextMessage);

} // namespace HPRIM
} // namespace Utils

#endif // UTILS_HPRIM_HPRIMPARSER_H
