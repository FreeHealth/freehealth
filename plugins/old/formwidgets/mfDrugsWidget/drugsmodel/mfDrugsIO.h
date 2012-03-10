/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@gmail.com                                                   *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef MFDRUGSIO_H
#define MFDRUGSIO_H

class mfDrugsIOPrivate;

#include <QObject>
#include <QHash>
#include <QString>

/**
 * \file mfDrugsIO.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.0.3
 * \date 13 July 2009
*/

class mfDrugsIO : public QObject
{
    Q_OBJECT
public:
    enum Loader {
        AppendPrescription,
        ReplacePrescription
    };

    static mfDrugsIO *instance(QObject *parent=0);
    ~mfDrugsIO();

    bool startsDosageTransmission();
    static bool isSendingDosage();
    static bool  prescriptionFromXml(const QString &xml, Loader loader = ReplacePrescription);

    static bool loadPrescription(const QString &fileName, QHash<QString,QString> &extraDatas, Loader loader = ReplacePrescription);
    static bool loadPrescription(const QString &fileName, Loader loader = ReplacePrescription);
    static bool loadPrescription(const QString &fileName, QString &xmlExtraDatas, Loader loader = ReplacePrescription);

    static bool savePrescription(const QHash<QString,QString> &extraDatas, const QString &toFileName = QString::null);
    static bool savePrescription(const QString &extraDatas, const QString &toFileName = QString::null);

    static QString prescriptionToXml();
    static QString prescriptionToHtml();

private:
    mfDrugsIO(QObject *parent);

private Q_SLOTS:
    void dosageTransmissionDone();

Q_SIGNALS:
    void transmissionDone();

private:
    static mfDrugsIO *m_Instance;
    mfDrugsIOPrivate *d;
};

#endif // MFDRUGSIO_H
