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
#ifndef DRUGSIO_H
#define DRUGSIO_H

#include <drugsbaseplugin/drugsbase_exporter.h>

#include <QObject>
#include <QHash>
#include <QString>

/**
 * \file drugsio.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.2.1
 * \date 25 Oct 2009
*/
namespace DrugsDB {
class DrugsModel;
namespace Internal {
class DrugsData;
class DrugsIOPrivate;
}

class DRUGSBASE_EXPORT DrugsIO : public QObject
{
    Q_OBJECT
public:
    enum Loader {
        AppendPrescription,
        ReplacePrescription
    };
    enum HtmlVersion {
        MedinTuxVersion = 0,
        NormalVersion,
        SimpleVersion,
        DrugsOnlyVersion
    };

    static DrugsIO *instance(QObject *parent=0);
    ~DrugsIO();

    bool startsDosageTransmission();
    static bool isSendingDosage();
    static bool prescriptionFromXml(DrugsDB::DrugsModel *model, const QString &xml, Loader loader = ReplacePrescription);

    static bool loadPrescription(DrugsDB::DrugsModel *model, const QString &fileName, QHash<QString,QString> &extraDatas, Loader loader = ReplacePrescription);
    static bool loadPrescription(DrugsDB::DrugsModel *model, const QString &fileName, Loader loader = ReplacePrescription);
    static bool loadPrescription(DrugsDB::DrugsModel *model, const QString &fileName, QString &xmlExtraDatas, Loader loader = ReplacePrescription);

    static bool savePrescription(DrugsDB::DrugsModel *model, const QHash<QString,QString> &extraDatas, const QString &toFileName = QString::null);
    static bool savePrescription(DrugsDB::DrugsModel *model, const QString &extraDatas, const QString &toFileName = QString::null);

    static QString prescriptionToXml(DrugsDB::DrugsModel *model);
    static QString prescriptionToHtml(DrugsDB::DrugsModel *model, int version = MedinTuxVersion);

    static bool printPrescription(DrugsDB::DrugsModel *model); //,
//                                  const QString &header, const QString &footer,
//                                  const QString &watermark, const int watermarkPresence,
//                                  const Qt::AlignmentFlag watermarkAlign, bool withDuplicata);

    static QStringList prescriptionMimeTypes();

private:
    DrugsIO(QObject *parent);

private Q_SLOTS:
    void dosageTransmissionDone();

Q_SIGNALS:
    void transmissionDone();

private:
    static DrugsIO *m_Instance;
    Internal::DrugsIOPrivate *d;
};


}  // End DrugsDB

#endif // DRUGSIO_H
