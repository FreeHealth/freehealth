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
#ifndef FORM_FORMEXPORTER_H
#define FORM_FORMEXPORTER_H

#include <formmanagerplugin/formmanager_exporter.h>
#include <QObject>
#include <QString>
#include <QStringList>

/**
 * \file formexporter.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 15 Apr 2013
*/

namespace Form {

class FormExporterJob
{
public:
    enum ExportGroupmentType {
        FormOrderedExportation,
        DateOrderedExportation
    };

    FormExporterJob() : _allPatients(false), _groupType(FormOrderedExportation) {}
    ~FormExporterJob() {}

    void setExportAllPatients(bool exportAll) {_allPatients=exportAll;}
    bool exportAllPatients() const {return _allPatients;}

    void setPatientUids(const QStringList &uuids) {_patientUids = uuids;}
    void setPatientUid(const QString &uid) {_patientUids.clear(); _patientUids<<uid;}
    const QStringList &patientUids() const {return _patientUids;}

    void setExportGroupmentType(ExportGroupmentType type) {_groupType=type;}
    ExportGroupmentType exportGroupmentType() const {return _groupType;}

private:
    bool _allPatients;
    QStringList _patientUids;
    ExportGroupmentType _groupType;
};

namespace Internal {
class FormExporterPrivate;
} // namespace Internal

class FORM_EXPORT FormExporter : public QObject
{
    Q_OBJECT
    
public:
    explicit FormExporter(QObject *parent = 0);
    ~FormExporter();
    bool initialize();
    
    void setPatientUuid(const QString &uuid);
    void setUseCurrentPatientForms(bool useCurrent);
    void setUseAllAvailableForms(bool useAll);

Q_SIGNALS:
    
public Q_SLOTS:
    void startExportation(const FormExporterJob &job);

private:
    Internal::FormExporterPrivate *d;
};

} // namespace Form

#endif  // FORM_FORMEXPORTER_H

