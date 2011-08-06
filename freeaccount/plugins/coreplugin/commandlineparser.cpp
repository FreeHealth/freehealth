/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "commandlineparser.h"

#include <coreplugin/ipatient.h>

#include <utils/global.h>
#include <utils/log.h>

#include <QApplication>
#include <QStringList>
#include <QFile>
#include <QDir>
#include <QDomElement>
#include <QDomDocument>

#include <QDebug>

using namespace Core;
using namespace Internal;

namespace Core {
namespace Internal {

namespace Constants {

    const char *const XML_HEADER                  = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    const char *const XML_GENERAL_TAG             = "FreeAccount_In";
    const char *const XML_CONFIG_FILE             = "ConfigFile";
    const char *const XML_OUT_FILE                = "ExchangeOut";
    const char *const XML_DRUGS_DATABASE          = "DrugsDatabase";
    const char *const XML_EMR                     = "EMR";
    const char *const XML_PATIENT                 = "Patient";
    const char *const XML_PATIENT_IDENTITY        = "Identity";
    const char *const XML_PATIENT_CREAT           = "Creatinine";
    const char *const XML_PATIENT_CRCL            = "CreatinineClearance";
    const char *const XML_PATIENT_WEIGHT          = "Weight";
    const char *const XML_PATIENT_HEIGHT          = "Height";
    const char *const XML_INN_ALLERGIES           = "InnAllergies";
    const char *const XML_DRUGS_ALLERGIES         = "DrugsUidAllergies";
    const char *const XML_ATC_ALLERGIES           = "ATCAllergies";
    const char *const XML_INN_INTOLERANCES        = "InnIntolerances";
    const char *const XML_DRUGS_INTOLERANCES      = "DrugsUidIntolerances";
    const char *const XML_ATC_INTOLERANCES        = "ATCIntolerances";
    const char *const XML_ICD10                   = "ICD10";
    const char *const XML_UI                      = "Ui";

    const char *const XML_ATTRIB_UI_EDITMODE      = "editmode";
    const char *const XML_ATTRIB_UI_BLOCKDATAS    = "blockPatientDatas";

    const char *const XML_ATTRIB_VALUE            = "value";
    const char *const XML_ATTRIB_UNIT             = "unit";
    const char *const XML_ATTRIB_UID              = "uid";
    const char *const XML_ATTRIB_LASTNAMES        = "lastnames";
    const char *const XML_ATTRIB_NAME             = "name";
    const char *const XML_ATTRIB_FIRSTNAME        = "firstnames";
    const char *const XML_ATTRIB_VERSION          = "version";
    const char *const XML_ATTRIB_GENDER           = "gender";
    const char *const XML_ATTRIB_DATEOFBIRTH      = "dob";
    const char *const XML_ATTRIB_FORMAT           = "format";
}

inline static void warnXmlReadError(const QString &file, const QString &msg, const int line = 0, const int col = 0)
{
    Utils::Log::addError("CommandLine",
                         Trans::ConstantTranslations::tkTr(Trans::Constants::FILE_1_ISNOT_READABLE).arg(file) + " ; " +
                         Trans::ConstantTranslations::tkTr(Trans::Constants::ERROR_1_LINE_2_COLUMN_3)
                         .arg(msg).arg(line).arg(col),
                         __FILE__, __LINE__);
}

class CommandLinePrivate
{
public:
    CommandLinePrivate()
    {
        params.insert(CommandLine::CL_MedinTux,          "--medintux");
        params.insert(CommandLine::CL_Test,              "--test");
        params.insert(CommandLine::CL_ReceiptsCreator,   "--receipts-creator");
        params.insert(CommandLine::CL_EMR_Name,          "--emr-name");
        params.insert(CommandLine::CL_EMR_Uid,           "--emr-uid");
        params.insert(CommandLine::CL_SelectionOnly,     "--selection-only");
        params.insert(CommandLine::CL_ExchangeOutFile,      "--exchange-out");
        params.insert(CommandLine::CL_ExchangeOutFileFormat,"--exchange-format");
        params.insert(CommandLine::CL_ExchangeInFile,    "--exchange-in");
        params.insert(CommandLine::CL_PatientUid,        "--patientuid");
        params.insert(CommandLine::CL_PatientName,       "--patientname");
        params.insert(CommandLine::CL_PatientFirstname,  "--patientfirstname");
        params.insert(CommandLine::CL_PatientGender,     "--gender");
        params.insert(CommandLine::CL_DateOfBirth,       "--dateofbirth");
        params.insert(CommandLine::Chrono,               "--chrono");
        params.insert(CommandLine::ConfigFile,           "--config");
        params.insert(CommandLine::RunningUnderWine,     "--wine");
        params.insert(CommandLine::ClearUserDatabases,   "--clear-user-databases");
        params.insert(CommandLine::CL_BlockPatientDatas, "--blockpatientdatas");

        // insert default values
        value.insert(CommandLine::CL_ExchangeOutFileFormat, "html_xml");
    }

    void parseCommandLine()
    {
        // Add default values
        value.insert(CommandLine::CL_Test, false);
        value.insert(CommandLine::CL_ReceiptsCreator, false);
        value.insert(CommandLine::CL_MedinTux, false);

        const QStringList &args = qApp->arguments();
        foreach(const QString &a, args) {
            QString k = a;
            if (k.contains(" "))
                k = k.left(k.indexOf(" "));
            if (a.contains("="))
                k = k.left(k.indexOf("="));
            switch (params.key(k,-1))
            {
            case CommandLine::CL_MedinTux :          value.insert(CommandLine::CL_MedinTux, true); break;
            case CommandLine::CL_Test :              value.insert(CommandLine::CL_Test, true); break;
            case CommandLine::CL_ReceiptsCreator :   value.insert(CommandLine::CL_ReceiptsCreator, true); break;
            case CommandLine::CL_EMR_Name :          value.insert(CommandLine::CL_EMR_Name, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::CL_EMR_Uid :           value.insert(CommandLine::CL_EMR_Uid, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::CL_SelectionOnly :     value.insert(CommandLine::CL_SelectionOnly, true); break;
            case CommandLine::CL_ExchangeOutFile :      value.insert(CommandLine::CL_ExchangeOutFile, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::CL_ExchangeOutFileFormat: value.insert(CommandLine::CL_ExchangeOutFileFormat, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::CL_ExchangeInFile :    value.insert(CommandLine::CL_ExchangeInFile, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::CL_PatientUid :        value.insert(CommandLine::CL_PatientUid, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::CL_PatientName :       value.insert(CommandLine::CL_PatientName, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::CL_PatientFirstname :  value.insert(CommandLine::CL_PatientFirstname, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::CL_PatientGender :     value.insert(CommandLine::CL_PatientGender, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::CL_DateOfBirth :       value.insert(CommandLine::CL_DateOfBirth, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::Chrono :            value.insert(CommandLine::Chrono, true); break;
            case CommandLine::ConfigFile :        value.insert(CommandLine::ConfigFile, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::RunningUnderWine:   value.insert(CommandLine::RunningUnderWine, true); break;
            case CommandLine::CL_BlockPatientDatas:  value.insert(CommandLine::CL_BlockPatientDatas, true); break;
            default : break;
        }
        }

        if (!value.value(CommandLine::CL_ExchangeInFile).isNull()) {
            QString file = value.value(CommandLine::CL_ExchangeInFile).toString();
            LOG_FOR("Core", QCoreApplication::translate("CommandLine", "Passing exchange in file : %1").arg(file));
            if (QDir::isRelativePath(file)) {
                file.prepend(qApp->applicationDirPath() + QDir::separator());
                file = QDir::cleanPath(file);
            }
            if (QFile::exists(file)) {
                readInFileXml(file);
            } else {
                LOG_ERROR_FOR("Core", QCoreApplication::translate("CommandLine", "Passing %1 as exchange in file, but file does not exists.").arg(file));
            }
        }
    }

    bool readInFileXml(const QString &file)
    {
        LOG_FOR("Core", QCoreApplication::translate("CommandLine", "Reading exchange in file : %1").arg(QFileInfo(file).absoluteFilePath()));
        // Read contents if necessary
        QString contents;
        contents = Utils::readTextFile(file, Utils::DontWarnUser);
        if (contents.isEmpty()) {
            Utils::Log::addError("CommandLine", QCoreApplication::translate("CommandLine", "In File %1 is empty.").arg(file),
                                 __FILE__, __LINE__);
            return false;
        }

        QDomDocument document;
        QString errorMsg;
        int errorLine, errorColumn;
        if (!document.setContent(contents, &errorMsg, &errorLine, &errorColumn)) {
            warnXmlReadError(file, errorMsg, errorLine, errorColumn);
            return false;
        }

        // Check root element
        QDomElement root = document.documentElement();
        if (root.tagName().compare(Internal::Constants::XML_GENERAL_TAG)!=0) {
            warnXmlReadError(file, QCoreApplication::translate("CommandLine", "Wrong root tag %1 %2.").arg(root.tagName()).arg(Constants::XML_GENERAL_TAG));
            return false;
        }
        if (!loadElement(root))
            return false;

//        foreach(int k, value.keys())
//            qWarning() << params.value(k) << value.value(k);

        return true;
    }

    bool loadElement(QDomElement &rootElement)
    {
//        <FreeAccount_In version="0.1.0">
//          <ConfigFile value="/path/to/the/requiered/configFile.ini"/>
//          <OutFile value="/path/to/the/fileToRead.exc" format="xml html html_xml"/>
//          <DrugsDatabase uid="FREEDIAMS_DRUGS_DATABASE_UID"/>
//          <EMR name="NameOfTheCallingEMR" uid="SessionUIDWillBeReturnedInOutputFile"/>
//          <Patient>
//          </Patient>
//          <Ui editmode="select-only or prescriber" blockPatientDatas="0/1"/>
//        </FreeDiams_In>
        QDomElement element = rootElement.firstChildElement();
        while (!element.isNull()) {
            if (element.tagName() == Internal::Constants::XML_CONFIG_FILE) {
                value.insert(CommandLine::ConfigFile, element.attribute(Internal::Constants::XML_ATTRIB_VALUE));
            } else if (element.tagName() == Internal::Constants::XML_OUT_FILE) {
                value.insert(CommandLine::CL_ExchangeOutFile, element.attribute(Internal::Constants::XML_ATTRIB_VALUE));
                value.insert(CommandLine::CL_ExchangeOutFileFormat, element.attribute(Internal::Constants::XML_ATTRIB_FORMAT));
            } else if (element.tagName() == Internal::Constants::XML_EMR) {
                value.insert(CommandLine::CL_EMR_Name, element.attribute(Internal::Constants::XML_ATTRIB_NAME));
                value.insert(CommandLine::CL_EMR_Uid, element.attribute(Internal::Constants::XML_ATTRIB_UID));
            } else if (element.tagName() == Internal::Constants::XML_UI) {
                if (element.attribute(Internal::Constants::XML_ATTRIB_UI_EDITMODE).compare("select-only",Qt::CaseInsensitive)==0)
                    value.insert(CommandLine::CL_SelectionOnly, true);
                else
                    value.insert(CommandLine::CL_SelectionOnly, false);
                value.insert(CommandLine::CL_BlockPatientDatas, element.attribute(Internal::Constants::XML_ATTRIB_UI_BLOCKDATAS));
            } else if (element.tagName() == Internal::Constants::XML_PATIENT) {
                readPatientDatas(element);
            }
            element = element.nextSiblingElement();
        }
        return true;
    }

    void readPatientDatas(const QDomElement &rootElement)
    {
//        <Patient>
//            <Identity lastnames="Name Of Patient" firstnames="James" uid="EMR Patient's UID" dob="yyyy/MM/dd" gender="M or F or H"/>
//            <Creatinin value="12" unit="mg/l or mmol/l"/>
//            <Weight value="70" unit="kg or pd" />
//            <Height value="170" unit="cm or "/>
//            <InnAllergies value="inn1;inn2;inn3"/>
//            <ATCAllergies value="ATC1;ATC2;ATC3"/>
//            <DrugsUidAllergies value="7655668;876769;656789"/>
//            <InnIntolerances value="inn1;inn2;inn3"/>
//            <ATCIntolerances value="ATC1;ATC2;ATC3"/>
//            <DrugsUidIntolerances value="7655668;876769;656789"/>
//            <ICD10 value="J11.0;A22;Z23"/>
//        </Patient>
        QDomElement element = rootElement.firstChildElement();
        while (!element.isNull()) {
            if (element.tagName() == Internal::Constants::XML_PATIENT_IDENTITY) {
                value.insert(CommandLine::CL_PatientName, element.attribute(Internal::Constants::XML_ATTRIB_LASTNAMES));
                value.insert(CommandLine::CL_PatientFirstname, element.attribute(Internal::Constants::XML_ATTRIB_FIRSTNAME));
                value.insert(CommandLine::CL_PatientUid, element.attribute(Internal::Constants::XML_ATTRIB_UID));
                value.insert(CommandLine::CL_PatientGender, element.attribute(Internal::Constants::XML_ATTRIB_GENDER));
                value.insert(CommandLine::CL_DateOfBirth, QDate::fromString(element.attribute(Internal::Constants::XML_ATTRIB_DATEOFBIRTH),"yyyy/MM/dd"));
            }
            element = element.nextSiblingElement();
        }
        // pass datas to patient object
    }

    void feedPatientDatas(Core::IPatient *patient)
    {
        patient->setData(patient->index(0, IPatient::Uid),            value.value(CommandLine::CL_PatientUid));
        patient->setData(patient->index(0, IPatient::DateOfBirth),    value.value(CommandLine::CL_DateOfBirth));
        patient->setData(patient->index(0, IPatient::BirthName),      value.value(CommandLine::CL_PatientName));
        patient->setData(patient->index(0, IPatient::Firstname),      value.value(CommandLine::CL_PatientFirstname));
        patient->setData(patient->index(0, IPatient::Gender),         value.value(CommandLine::CL_PatientGender));

//        QStringList tmp;
//        // Allergies
//        tmp = value.value(CommandLine::CL_AtcAllergies).toString().split(";");
//        tmp.removeAll("");
//        patient->setData(patient->index(0, IPatient::DrugsAtcAllergies), tmp);

//        tmp = value.value(CommandLine::CL_InnAllergies).toString().split(";");
//        tmp.removeAll("");
//        patient->setData(patient->index(0, IPatient::DrugsInnAllergies), tmp);

//        tmp = value.value(CommandLine::CL_DrugsAllergies).toString().split(";");
//        tmp.removeAll("");
//        patient->setData(patient->index(0, IPatient::DrugsUidAllergies), tmp);

//        // Intolerances
//        tmp = value.value(CommandLine::CL_AtcIntolerances).toString().split(";");
//        tmp.removeAll("");
//        patient->setData(patient->index(0, IPatient::DrugsAtcIntolerances), tmp);

//        tmp = value.value(CommandLine::CL_DrugsIntolerances).toString().split(";");
//        tmp.removeAll("");
//        patient->setData(patient->index(0, IPatient::DrugsUidIntolerances), tmp);

//        tmp = value.value(CommandLine::CL_InnIntolerances).toString().split(";");
//        tmp.removeAll("");
//        patient->setData(patient->index(0, IPatient::DrugsInnAtcIntolerances), tmp);

//        // Creatinin clearance
//        if (value.value(CommandLine::CL_CrCl).isValid()) {
//            patient->setData(patient->index(0, IPatient::CreatinClearance),     value.value(CommandLine::CL_CrCl));
//            patient->setData(patient->index(0, IPatient::CreatinClearanceUnit), value.value(CommandLine::CL_CrCl_Unit));
//        }
    }

public:
    QHash<int, QVariant> value;
    static QHash<int, QString> params;
};
}
}

QHash<int, QString> Internal::CommandLinePrivate::params;



CommandLine::CommandLine() :
        Core::ICommandLine(), d(new CommandLinePrivate)
{
    d->parseCommandLine();
}

CommandLine::~CommandLine()
{
    if (d)
        delete d;
    d=0;
}

void CommandLine::feedPatientDatas(Core::IPatient *patient)
{
    d->feedPatientDatas(patient);
}

QVariant CommandLine::value(int param, const QVariant &def) const
{
    return d->value.value(param,def);
}

QString CommandLine::paramName(int param) const
{
    if (d->params.keys().contains(param))
        return d->params.value(param);
    else
        return QString::number(param);
}
