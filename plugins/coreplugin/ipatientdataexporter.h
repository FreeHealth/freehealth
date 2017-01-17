/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main developers: Eric MAEKER, <eric.maeker@gmail.com>                 *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef CORE_IPATIENTDATAEXPORTER_H
#define CORE_IPATIENTDATAEXPORTER_H

#include <coreplugin/core_exporter.h>
#include <QObject>
#include <QString>
#include <QStringList>

namespace Core {

class CORE_EXPORT PatientDataExporterJob
{
public:
    enum ExportGroupmentType {
        FormOrderedExportation,
        DateOrderedExportation
    };

    enum InformationPriority {
        High    = 0x0001,
        Medium  = 0x0002,
        Low     = 0x0004,
        All     = High | Medium | Low,
        Default = All
    };
    Q_DECLARE_FLAGS(InformationPriorities, InformationPriority)

    enum OutputFormat {
        Html,
        Pdf,
        Xml
    };

    enum FileExportation {
        MultipleFiles,
        SingleFile
    };

    PatientDataExporterJob() :
        _allPatients(false),
        _groupType(FormOrderedExportation),
        _outputFormat(Html),
        _priorities(Default)
    {}

    ~PatientDataExporterJob() {}

    /** If set, export all patient from the patient database */
    void setExportAllPatients(bool exportAll) {_allPatients=exportAll;}
    bool exportAllPatients() const {return _allPatients;}

    /** Define the patient UUID to use for the extraction */
    void setPatientUids(const QStringList &uuids) {_patientUids = uuids;}
    void setPatientUid(const QString &uid) {_patientUids.clear(); _patientUids<<uid;}
    const QStringList &patientUids() const {return _patientUids;}

    /** Define the exportation groupment type (eg: group by forms, group by date...) */
    void setExportGroupmentType(ExportGroupmentType type) {_groupType=type;}
    const ExportGroupmentType &exportGroupmentType() const {return _groupType;}

    /** Define the exportation information priorities. By default, set to Core::InformationPriority::Default */
    void setInformationPriorities(InformationPriorities priorities) {_priorities=priorities;}
    const InformationPriorities &informationPriorities() const {return _priorities;}

    /** Define the file format to use for the general output file */
    void setOutputFormat(OutputFormat format) {_outputFormat=format;}
    const OutputFormat &outputFormat() const {return _outputFormat;}

    /** Define the absolute path of the extraction */
    void setOutputAbsolutePath(const QString &path) {_absPath=path;}
    const QString &outputAbsolutePath() const {return _absPath;}

private:
    bool _allPatients;
    QStringList _patientUids;
    ExportGroupmentType _groupType;
    OutputFormat _outputFormat;
    QString _absPath;
    InformationPriorities _priorities;
};

class CORE_EXPORT PatientDataExtraction // : public QObject
{
public:
    PatientDataExtraction() {}
    ~PatientDataExtraction() {}

    // Files
    void setMasterAbsoluteFilePath(const QString &filepath) {_masterFile=filepath;}
    QString masterAbsoluteFilePath() const {return _masterFile;}

    void addSecondaryAbsoluteFilePath(const QString &filepath) {_secondaryFiles << filepath;}
    void setSecondaryAbsoluteFilePath(const QStringList &files) {_secondaryFiles = files;}
    void clearSecondaryFiles() {_secondaryFiles.clear();}
    QStringList secondaryFiles() const {return _secondaryFiles;}

    // Errors
    bool hasError() const {return (!_errors.isEmpty());}
    void addErrorMessage(const QString &error) {_errors << error;}
    const QStringList &errorMessages() const {return _errors;}

private:
    QString _masterFile;
    QStringList _secondaryFiles, _errors;
};

class CORE_EXPORT IPatientDataExporter : public QObject
{
    Q_OBJECT
public:
    enum ExporterType {
        IdentityExporter    = 0x000001,
        PmhxExporter        = 0x000002,
        FormsExporter       = 0x000004,
        VaccineExporter     = 0x000008,
        DocumentExporter    = 0x000010
    };
    Q_DECLARE_FLAGS(ExporterTypes, ExporterType)

    explicit IPatientDataExporter(QObject *parent = 0) : QObject(parent) {}
    virtual ~IPatientDataExporter() {}

    void setExporterTypes(const ExporterTypes &types) {_types=types;}
    ExporterTypes exporterTypes() const {return _types;}

    virtual bool isBusy() const = 0;

public Q_SLOTS:
    // Currently can not be asynchronous because of form management
    virtual Core::PatientDataExtraction *startExportationJob(const Core::PatientDataExporterJob &job) = 0;

Q_SIGNALS:
    void extractionProgressValueChanged(int value);
    void extractionProgressRangeChanged(int min, int max);
    void extractionProgressMessageChanged(const QString &message);

private:
    ExporterTypes _types;
};

} // namespace Core

Q_DECLARE_OPERATORS_FOR_FLAGS(Core::IPatientDataExporter::ExporterTypes)
Q_DECLARE_OPERATORS_FOR_FLAGS(Core::PatientDataExporterJob::InformationPriorities)

#endif // CORE_IPATIENTDATAEXPORTER_H
