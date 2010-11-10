/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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

    static QString prescriptionToXml(DrugsDB::DrugsModel *model, const QString &xmlExtraData = QString::null);
    static QString prescriptionToHtml(DrugsDB::DrugsModel *model, const QString &xmlExtraDatas = QString::null, int version = MedinTuxVersion);

    static bool printPrescription(DrugsDB::DrugsModel *model); //,
//                                  const QString &header, const QString &footer,
//                                  const QString &watermark, const int watermarkPresence,
//                                  const Qt::AlignmentFlag watermarkAlign, bool withDuplicata);
    static void prescriptionPreview(DrugsDB::DrugsModel *model);

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
