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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
#include "commandlineparser.h"
#include "drc_constants.h"

#include <coreplugin/ipatient.h>

#include <utils/global.h>
#include <utils/log.h>
#include <translationutils/constants.h>
#include <translationutils/trans_filepathxml.h>
#include <translationutils/trans_msgerror.h>

#include <QApplication>
#include <QStringList>
#include <QFile>
#include <QDir>
#include <QDomElement>
#include <QDomDocument>

#include <QDebug>

using namespace Core;
using namespace Internal;
using namespace Trans::ConstantTranslations;

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

static inline void warnXmlReadError(const QString &file, const QString &msg, const int line = 0, const int col = 0)
{
    LOG_ERROR_FOR("CommandLine",
                  Trans::ConstantTranslations::tkTr(Trans::Constants::FILE_1_ISNOT_READABLE).arg(file) + " ; " +
                  Trans::ConstantTranslations::tkTr(Trans::Constants::ERROR_1_LINE_2_COLUMN_3)
                  .arg(msg).arg(line).arg(col));
}

class CommandLinePrivate
{
public:
    CommandLinePrivate()
    {
        params.insert(Core::Constants::CL_MedinTux,          "--medintux");
        params.insert(Core::Constants::CL_Independant,       "--independant");
        params.insert(Core::Constants::CL_Movements,         "--movements");
        params.insert(Core::Constants::CL_PreferedReceipts,  "--prefered-receipts");
        params.insert(Core::Constants::CL_Test,              "--test");
        params.insert(Core::Constants::CL_ReceiptsCreator,   "--receipts-creator");
        params.insert(Core::Constants::CL_EMR_Name,          "--emr-name");
        params.insert(Core::Constants::CL_EMR_Uid,           "--emr-uid");
        params.insert(Core::Constants::CL_SelectionOnly,     "--selection-only");
        params.insert(Core::Constants::CL_ExchangeOutFile,      "--exchange-out");
        params.insert(Core::Constants::CL_ExchangeOutFileFormat,"--exchange-format");
        params.insert(Core::Constants::CL_ExchangeInFile,    "--exchange-in");
        params.insert(Core::Constants::CL_PatientUid,        "--patientuid");
        params.insert(Core::Constants::CL_PatientName,       "--patientname");
        params.insert(Core::Constants::CL_PatientFirstname,  "--patientfirstname");
        params.insert(Core::Constants::CL_PatientGender,     "--gender");
        params.insert(Core::Constants::CL_DateOfBirth,       "--dateofbirth");
        params.insert(Core::Constants::CL_BlockPatientDatas, "--blockpatientdatas");
        params.insert(Core::Constants::CL_DbHost, "--dbhost");
        params.insert(Core::Constants::CL_DbPort, "--dbport");
        params.insert(CommandLine::Chrono,               "--chrono");
        params.insert(CommandLine::ConfigFile,           "--config");
        params.insert(CommandLine::RunningUnderWine,     "--wine");
        params.insert(CommandLine::ClearUserDatabases,   "--clear-user-databases");
        params.insert(CommandLine::CreateVirtuals,       "--create-virtuals");
        params.insert(CommandLine::ResetUserPreferences, "--reset-user-preferences");
        params.insert(CommandLine::UserClearLogin,       "--user-clear-log");
        params.insert(CommandLine::UserClearPassword,    "--user-clear-password");

        // insert default values
        value.insert(Core::Constants::CL_ExchangeOutFileFormat, "html_xml");
        value.insert(CommandLine::UserClearLogin, QVariant());
        value.insert(CommandLine::UserClearPassword, QVariant());
    }

    void parseCommandLine()
    {
        LOG_FOR("CommandLine", "Parsing command line");
        // Add default values
        value.insert(Core::Constants::CL_Test, false);
        value.insert(Core::Constants::CL_ReceiptsCreator, false);
        value.insert(Core::Constants::CL_MedinTux, false);

        QStringList args = qApp->arguments();
        foreach(const QString &a, args) {
            LOG_FOR("CommandLine", a);
            QString k = a;
            if (k.contains(" "))
                k = k.left(k.indexOf(" "));
            if (a.contains("="))
                k = k.left(k.indexOf("="));
            switch (params.key(k,-1))
            {
            case Core::Constants::CL_MedinTux :          value.insert(Core::Constants::CL_MedinTux, true); break;
            case Core::Constants::CL_Independant :       value.insert(Core::Constants::CL_Independant, true); break;
            case Core::Constants::CL_Movements :         value.insert(Core::Constants::CL_Movements, true); break;
            case Core::Constants::CL_PreferedReceipts :  value.insert(Core::Constants::CL_PreferedReceipts, true); break;
            case Core::Constants::CL_Test :              value.insert(Core::Constants::CL_Test, true); break;
            case Core::Constants::CL_ReceiptsCreator :   value.insert(Core::Constants::CL_ReceiptsCreator, true); break;
            case Core::Constants::CL_EMR_Name :          value.insert(Core::Constants::CL_EMR_Name, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case Core::Constants::CL_EMR_Uid :           value.insert(Core::Constants::CL_EMR_Uid, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case Core::Constants::CL_SelectionOnly :     value.insert(Core::Constants::CL_SelectionOnly, true); break;
            case Core::Constants::CL_ExchangeOutFile :      value.insert(Core::Constants::CL_ExchangeOutFile, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case Core::Constants::CL_ExchangeOutFileFormat: value.insert(Core::Constants::CL_ExchangeOutFileFormat, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case Core::Constants::CL_ExchangeInFile :    value.insert(Core::Constants::CL_ExchangeInFile, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case Core::Constants::CL_PatientUid :        value.insert(Core::Constants::CL_PatientUid, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case Core::Constants::CL_PatientName :       value.insert(Core::Constants::CL_PatientName, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case Core::Constants::CL_PatientFirstname :  value.insert(Core::Constants::CL_PatientFirstname, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case Core::Constants::CL_PatientGender :     value.insert(Core::Constants::CL_PatientGender, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case Core::Constants::CL_DateOfBirth :       value.insert(Core::Constants::CL_DateOfBirth, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case Core::Constants::CL_DbHost :       value.insert(Core::Constants::CL_DbHost, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case Core::Constants::CL_DbPort :       value.insert(Core::Constants::CL_DbPort, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::Chrono :            value.insert(CommandLine::Chrono, true); break;
            case CommandLine::ConfigFile :        value.insert(CommandLine::ConfigFile, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::RunningUnderWine:   value.insert(CommandLine::RunningUnderWine, true); break;
            case CommandLine::ClearUserDatabases:   value.insert(CommandLine::ClearUserDatabases, true); break;
            case Core::Constants::CL_BlockPatientDatas:  value.insert(Core::Constants::CL_BlockPatientDatas, true); break;
            case CommandLine::UserClearLogin:        value.insert(CommandLine::UserClearLogin, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::UserClearPassword:        value.insert(CommandLine::UserClearPassword, a.mid(a.indexOf("=")+1).remove("\"")); break;
            default : break;
        }
        }

        if (!value.value(Core::Constants::CL_ExchangeInFile).isNull()) {
            QString file = value.value(Core::Constants::CL_ExchangeInFile).toString();
            LOG_FOR("Core", QCoreApplication::translate("CommandLine", "Passing exchange in file : %1").arg(file));
            if (QDir::isRelativePath(file)) {
                file.prepend(qApp->applicationDirPath() + QDir::separator());
                file = QDir::cleanPath(file);
            }
            if (QFile::exists(file)) {
                readInFileXml(file);
            } else {
                LOG_ERROR_FOR("Core", tkTr(Trans::Constants::FILE_1_DOESNOT_EXISTS).arg(file));
            }
        }

        if (Utils::isReleaseCompilation()) {
            value.insert(CommandLine::ClearUserDatabases, false);
        }        
    }

    bool readInFileXml(const QString &file)
    {
        LOG_FOR("Core", tkTr(Trans::Constants::OPENING_FILE_1).arg(QFileInfo(file).absoluteFilePath()));
        // Read contents if necessary
        QString contents;
        contents = Utils::readTextFile(file, Utils::DontWarnUser);
        if (contents.isEmpty()) {
            LOG_ERROR_FOR("CommandLine", tkTr(Trans::Constants::FILE_1_ISEMPTY).arg(file));
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
            warnXmlReadError(file, tkTr(Trans::Constants::XML_WRONG_ROOT_TAG_1_2).arg(root.tagName()).arg(Constants::XML_GENERAL_TAG));
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
                value.insert(Core::Constants::CL_ExchangeOutFile, element.attribute(Internal::Constants::XML_ATTRIB_VALUE));
                value.insert(Core::Constants::CL_ExchangeOutFileFormat, element.attribute(Internal::Constants::XML_ATTRIB_FORMAT));
            } else if (element.tagName() == Internal::Constants::XML_EMR) {
                value.insert(Core::Constants::CL_EMR_Name, element.attribute(Internal::Constants::XML_ATTRIB_NAME));
                value.insert(Core::Constants::CL_EMR_Uid, element.attribute(Internal::Constants::XML_ATTRIB_UID));
            } else if (element.tagName() == Internal::Constants::XML_UI) {
                if (element.attribute(Internal::Constants::XML_ATTRIB_UI_EDITMODE).compare("select-only",Qt::CaseInsensitive)==0)
                    value.insert(Core::Constants::CL_SelectionOnly, true);
                else
                    value.insert(Core::Constants::CL_SelectionOnly, false);
                value.insert(Core::Constants::CL_BlockPatientDatas, element.attribute(Internal::Constants::XML_ATTRIB_UI_BLOCKDATAS));
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
                value.insert(Core::Constants::CL_PatientName, element.attribute(Internal::Constants::XML_ATTRIB_LASTNAMES));
                value.insert(Core::Constants::CL_PatientFirstname, element.attribute(Internal::Constants::XML_ATTRIB_FIRSTNAME));
                value.insert(Core::Constants::CL_PatientUid, element.attribute(Internal::Constants::XML_ATTRIB_UID));
                value.insert(Core::Constants::CL_PatientGender, element.attribute(Internal::Constants::XML_ATTRIB_GENDER));
                value.insert(Core::Constants::CL_DateOfBirth, QDate::fromString(element.attribute(Internal::Constants::XML_ATTRIB_DATEOFBIRTH),"yyyy/MM/dd"));
            }
            element = element.nextSiblingElement();
        }
        // pass datas to patient object
    }

    void feedPatientDatas(Core::IPatient *patient)
    {
        patient->setData(patient->index(0, IPatient::Uid),            value.value(Core::Constants::CL_PatientUid));
        patient->setData(patient->index(0, IPatient::DateOfBirth),    value.value(Core::Constants::CL_DateOfBirth));
        patient->setData(patient->index(0, IPatient::UsualName),      value.value(Core::Constants::CL_PatientName));
        patient->setData(patient->index(0, IPatient::Firstname),      value.value(Core::Constants::CL_PatientFirstname));
        patient->setData(patient->index(0, IPatient::Gender),         value.value(Core::Constants::CL_PatientGender));
    }

public:
    QHash<int, QVariant> value;
    QHash<int, QString> params;
};
}
}

CommandLine::CommandLine(QObject *parent) :
    Core::ICommandLine(parent),
    d(new CommandLinePrivate)
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
    if (d->params.keys().contains(param)) {
        return d->params.value(param);
    } else {
        // If a value is recorded return stringyfied param
        if (d->value.value(param).isValid())
            return QString::number(param);
    }
    return QString::null;
}
