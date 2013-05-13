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
#ifndef DRUGSBASE_INTERNAL_PRESCRIPTIONPRINTER_H
#define DRUGSBASE_INTERNAL_PRESCRIPTIONPRINTER_H

#include <QObject>
#include <QString>

/**
 * \file prescriptionprinter.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 13 May 2013
*/

namespace DrugsDB {
class DrugsModel;
namespace Internal {
class PrescriptionPrinterPrivate;
class PrescriptionPrinterJobPrivate;

class PrescriptionPrinterJob
{
public:
    enum OutputType {
        FullHtml = 0,
        Simplified,
        DrugsNameOnly
    };

    enum DrugLine {
        HtmlOrderedList = 0,    // uses <ol><li></li></ol>
        HtmlDiv                 // uses <div style="prescriptionLine">...</div>
    };

    PrescriptionPrinterJob();
    ~PrescriptionPrinterJob();
    void readSettings();

    QString uuid() const;

    void setVersion(OutputType type);
    OutputType outputType() const;

    bool sortBeforePrinting() const;
    bool addLineBreakBetweenEachDrugs() const;
    bool printDuplicates() const;

    bool addPatientBiometrics() const;
    QString patientBiometricsToHtml() const;

    void addXmlExtraData(const QString &xmlExtraData);
    QString xmlExtraData() const;

    void setDrugsModel(DrugsModel *drugsModel);
    DrugsModel *drugsModel() const;

private:
    PrescriptionPrinterJobPrivate *d;
};

class PrescriptionPrinter : public QObject
{
    Q_OBJECT
    
public:
    explicit PrescriptionPrinter(QObject *parent = 0);
    ~PrescriptionPrinter();
    bool initialize();
    
    QString prescriptionToHtml(DrugsModel *model);
    QString prescriptionToHtml(const PrescriptionPrinterJob &job);
    bool print(const PrescriptionPrinterJob &job);
    
public Q_SLOTS:
    
private:
    PrescriptionPrinterPrivate *d;
};

} // namespace Internal
} // namespace DrugsDB

#endif // DRUGSBASE_INTERNAL_PRESCRIPTIONPRINTER_H

