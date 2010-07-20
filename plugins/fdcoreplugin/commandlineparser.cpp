#include "commandlineparser.h"

#include <fdcoreplugin/patient.h>

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

namespace Core {
namespace Internal {

namespace Constants {

    const char *const XML_HEADER                  = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    const char *const XML_GENERAL_TAG             = "FreeDiams_In";
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
    const char *const XML_ICD10                   = "ICD10";
    const char *const XML_UI                      = "Ui";

    const char *const XML_ATTRIB_UI_EDITMODE      = "editmode";
    const char *const XML_ATTRIB_UI_BLOCKDATAS    = "blockPatientDatas";

    const char *const XML_ATTRIB_VALUE            = "value";
    const char *const XML_ATTRIB_UNIT             = "unit";
    const char *const XML_ATTRIB_UID              = "uid";
    const char *const XML_ATTRIB_NAME             = "name";
    const char *const XML_ATTRIB_SURNAME          = "surname";
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
                         .arg(msg).arg(line).arg(col));
}

class CommandLinePrivate
{
public:
    CommandLinePrivate() 
    {
        params.insert(CommandLine::CL_MedinTux,          "--medintux");
        params.insert(CommandLine::CL_EMR_Name,          "--emr-name");
        params.insert(CommandLine::CL_EMR_Uid,           "--emr-uid");
        params.insert(CommandLine::CL_SelectionOnly,     "--selection-only");
        params.insert(CommandLine::CL_DrugsDatabaseUid,  "--drugsdb-uid");
        params.insert(CommandLine::CL_ExchangeFile,      "--exchange-out");
        params.insert(CommandLine::CL_ExchangeFileFormat,"--exchange-format");
        params.insert(CommandLine::CL_ExchangeInFile,    "--exchange-in");
        params.insert(CommandLine::CL_PatientUid,        "--patientuid");
        params.insert(CommandLine::CL_PatientName,       "--patientname");
        params.insert(CommandLine::CL_PatientSurname,    "--patientsurname");
        params.insert(CommandLine::CL_PatientGender,     "--gender");
        params.insert(CommandLine::CL_DateOfBirth,       "--dateofbirth");
        params.insert(CommandLine::CL_Weight,            "--weight");
        params.insert(CommandLine::CL_Weight_Unit,       "--weight-unit");
        params.insert(CommandLine::CL_Height,            "--height");
        params.insert(CommandLine::CL_Height_Unit,       "--height-unit");
        params.insert(CommandLine::CL_CrCl,              "--crcl");
        params.insert(CommandLine::CL_CrCl_Unit,         "--crcl-unit");
        params.insert(CommandLine::CL_Chrono,            "--chrono");
        params.insert(CommandLine::CL_Creatinine,        "--creatinine");
        params.insert(CommandLine::CL_Creatinine_Unit,   "--creatinine-unit");
        params.insert(CommandLine::CL_DrugsAllergies,    "--notdrugs");
        params.insert(CommandLine::CL_InnAllergies,      "--drugs-uid-allergies");
        params.insert(CommandLine::CL_AtcAllergies,      "--atc-allergies");
        params.insert(CommandLine::CL_ICD10Diseases,     "--inn-allergies");
        params.insert(CommandLine::CL_TransmitDosage,    "--transmit-dosage");
        params.insert(CommandLine::CL_ConfigFile,        "--config");
        params.insert(CommandLine::CL_RunningUnderWine,  "--wine");
        params.insert(CommandLine::CL_BlockPatientDatas, "--blockpatientdatas");

        // insert default values
        value.insert(CommandLine::CL_ExchangeFileFormat, "html_xml");
    }

    void parseCommandLine()
    {
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
            case CommandLine::CL_EMR_Name :          value.insert(CommandLine::CL_EMR_Name, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::CL_EMR_Uid :           value.insert(CommandLine::CL_EMR_Uid, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::CL_SelectionOnly :     value.insert(CommandLine::CL_SelectionOnly, true); break;
            case CommandLine::CL_ExchangeFile :      value.insert(CommandLine::CL_ExchangeFile, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::CL_ExchangeFileFormat: value.insert(CommandLine::CL_ExchangeFileFormat, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::CL_ExchangeInFile :    value.insert(CommandLine::CL_ExchangeInFile, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::CL_PatientUid :        value.insert(CommandLine::CL_PatientUid, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::CL_PatientName :       value.insert(CommandLine::CL_PatientName, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::CL_PatientSurname :    value.insert(CommandLine::CL_PatientSurname, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::CL_PatientGender :     value.insert(CommandLine::CL_PatientGender, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::CL_DateOfBirth :       value.insert(CommandLine::CL_DateOfBirth, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::CL_Weight :            value.insert(CommandLine::CL_Weight, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::CL_Weight_Unit :       value.insert(CommandLine::CL_Weight_Unit, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::CL_Height :            value.insert(CommandLine::CL_Height, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::CL_Height_Unit :       value.insert(CommandLine::CL_Height_Unit, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::CL_CrCl :              value.insert(CommandLine::CL_CrCl, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::CL_CrCl_Unit :         value.insert(CommandLine::CL_CrCl_Unit, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::CL_Creatinine :        value.insert(CommandLine::CL_Creatinine, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::CL_Creatinine_Unit :   value.insert(CommandLine::CL_Creatinine_Unit, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::CL_Chrono :            value.insert(CommandLine::CL_Chrono, true); break;
            case CommandLine::CL_TransmitDosage :    value.insert(CommandLine::CL_TransmitDosage, true); break;
            case CommandLine::CL_ConfigFile :        value.insert(CommandLine::CL_ConfigFile, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::CL_RunningUnderWine:   value.insert(CommandLine::CL_RunningUnderWine, true); break;
            case CommandLine::CL_BlockPatientDatas:  value.insert(CommandLine::CL_BlockPatientDatas, true); break;
                /** \todo icd10 and drugs allergies */
                //                case CL_DrugsAllergies : value.insert(CL_DrugsAllergies, true); break;
                //                case CL_ICD10Diseases : value.insert(CL_ICD10Diseases, true); break;
            default : break;
        }
        }

        if (!value.value(CommandLine::CL_ExchangeInFile).isNull()) {
            QString file = value.value(CommandLine::CL_ExchangeInFile).toString();
            if (QDir::isRelativePath(file)) {
                file.prepend(qApp->applicationDirPath() + QDir::separator());
                file = QDir::cleanPath(file);
            }
            if (QFile::exists(file)) {
                readInFileXml(file);
            }
        }
    }

    bool readInFileXml(const QString &file)
    {
        // Read contents if necessary
        QString contents;
        contents = Utils::readTextFile(file, Utils::DontWarnUser);
        if (contents.isEmpty()) {
            Utils::Log::addError("CommandLine", QCoreApplication::translate("CommandLine", "In File %1 is empty.").arg(file));
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
//        <FreeDiams_In version="0.4.0">
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
                value.insert(CommandLine::CL_ConfigFile, element.attribute(Internal::Constants::XML_ATTRIB_VALUE));
            } else if (element.tagName() == Internal::Constants::XML_OUT_FILE) {
                value.insert(CommandLine::CL_ExchangeFile, element.attribute(Internal::Constants::XML_ATTRIB_VALUE));
                value.insert(CommandLine::CL_ExchangeFileFormat, element.attribute(Internal::Constants::XML_ATTRIB_FORMAT));
            } else if (element.tagName() == Internal::Constants::XML_DRUGS_DATABASE) {
                value.insert(CommandLine::CL_DrugsDatabaseUid, element.attribute(Internal::Constants::XML_ATTRIB_UID));
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
//            <Identity name="Name Of Patient" surname="James" uid="EMR Patient's UID" dob="yyyy/MM/dd" gender="M or F or H"/>
//            <Creatinin value="12" unit="mg/l or mmol/l"/>
//            <Weight value="70" unit="kg or pd" />
//            <Height value="170" unit="cm or "/>
//            <InnAllergies value="inn1;inn2;inn3"/>
//            <ATCAllergies value="ATC1;ATC2;ATC3"/>
//            <DrugsUidAllergies value="7655668;876769;656789"/>
//            <ICD10 value="J11.0;A22;Z23"/>
//        </Patient>
        QDomElement element = rootElement.firstChildElement();
        while (!element.isNull()) {
            if (element.tagName() == Internal::Constants::XML_PATIENT_IDENTITY) {
                value.insert(CommandLine::CL_PatientName, element.attribute(Internal::Constants::XML_ATTRIB_NAME));
                value.insert(CommandLine::CL_PatientSurname, element.attribute(Internal::Constants::XML_ATTRIB_SURNAME));
                value.insert(CommandLine::CL_PatientUid, element.attribute(Internal::Constants::XML_ATTRIB_UID));
                value.insert(CommandLine::CL_PatientGender, element.attribute(Internal::Constants::XML_ATTRIB_GENDER));
                value.insert(CommandLine::CL_DateOfBirth, QDate::fromString(element.attribute(Internal::Constants::XML_ATTRIB_DATEOFBIRTH),"yyyy/MM/dd"));
            } else if (element.tagName() == Internal::Constants::XML_PATIENT_HEIGHT) {
                value.insert(CommandLine::CL_Height, element.attribute(Internal::Constants::XML_ATTRIB_VALUE));
                value.insert(CommandLine::CL_Height_Unit, element.attribute(Internal::Constants::XML_ATTRIB_UNIT));
            } else if (element.tagName() == Internal::Constants::XML_PATIENT_WEIGHT) {
                value.insert(CommandLine::CL_Weight, element.attribute(Internal::Constants::XML_ATTRIB_VALUE));
                value.insert(CommandLine::CL_Weight_Unit, element.attribute(Internal::Constants::XML_ATTRIB_UNIT));
            } else if (element.tagName() == Internal::Constants::XML_PATIENT_CREAT) {
                value.insert(CommandLine::CL_Creatinine, element.attribute(Internal::Constants::XML_ATTRIB_VALUE));
                value.insert(CommandLine::CL_Creatinine_Unit, element.attribute(Internal::Constants::XML_ATTRIB_UNIT));
            } else if (element.tagName() == Internal::Constants::XML_PATIENT_CRCL) {
                value.insert(CommandLine::CL_CrCl, element.attribute(Internal::Constants::XML_ATTRIB_VALUE));
                value.insert(CommandLine::CL_CrCl_Unit, element.attribute(Internal::Constants::XML_ATTRIB_UNIT));
            } else if (element.tagName() == Internal::Constants::XML_ATC_ALLERGIES) {
                value.insert(CommandLine::CL_AtcAllergies, element.attribute(Internal::Constants::XML_ATTRIB_VALUE));
            } else if (element.tagName() == Internal::Constants::XML_INN_ALLERGIES) {
                value.insert(CommandLine::CL_InnAllergies, element.attribute(Internal::Constants::XML_ATTRIB_VALUE));
            } else if (element.tagName() == Internal::Constants::XML_DRUGS_ALLERGIES) {
                value.insert(CommandLine::CL_DrugsAllergies, element.attribute(Internal::Constants::XML_ATTRIB_VALUE));
            }
            element = element.nextSiblingElement();
        }
        // pass datas to patient object
    }

    void feedPatientDatas(Core::Patient *patient)
    {
        patient->setValue(IPatient::Uid,            value.value(CommandLine::CL_PatientUid));
        patient->setValue(IPatient::DateOfBirth,    value.value(CommandLine::CL_DateOfBirth));
        patient->setValue(IPatient::Height,         value.value(CommandLine::CL_Height));
        patient->setValue(IPatient::HeightUnit,     value.value(CommandLine::CL_Height_Unit));
        patient->setValue(IPatient::Weight,         value.value(CommandLine::CL_Weight));
        patient->setValue(IPatient::WeightUnit,     value.value(CommandLine::CL_Weight_Unit));
        patient->setValue(IPatient::Creatinine,     value.value(CommandLine::CL_Creatinine));
        patient->setValue(IPatient::CreatinineUnit, value.value(CommandLine::CL_Creatinine_Unit));
        patient->setValue(IPatient::BirthName,      value.value(CommandLine::CL_PatientName));
        patient->setValue(IPatient::Surname,        value.value(CommandLine::CL_PatientSurname));
        patient->setValue(IPatient::Gender,         value.value(CommandLine::CL_PatientGender));

        QStringList tmp = value.value(CommandLine::CL_AtcAllergies).toString().split(";");
        tmp.removeAll("");
        patient->setValue(IPatient::DrugsAtcAllergies, tmp);

        tmp = value.value(CommandLine::CL_InnAllergies).toString().split(";");
        tmp.removeAll("");
        patient->setValue(IPatient::DrugsInnAllergies, tmp);

        tmp = value.value(CommandLine::CL_DrugsAllergies).toString().split(";");
        tmp.removeAll("");
        patient->setValue(IPatient::DrugsUidAllergies, tmp);

        if (value.value(CommandLine::CL_CrCl).isValid()) {
            patient->setValue(IPatient::CreatinClearance,     value.value(CommandLine::CL_CrCl));
            patient->setValue(IPatient::CreatinClearanceUnit, value.value(CommandLine::CL_CrCl_Unit));
        }
    }

public:
    QHash<int, QVariant> value;
    static QHash<int, QString> params;
};
}
}

QHash<int, QString> Internal::CommandLinePrivate::params;



CommandLine::CommandLine() :
        ICommandLine(), d(new Internal::CommandLinePrivate)
{
    d->parseCommandLine();
}

CommandLine::~CommandLine()
{
    if (d)
        delete d;
    d=0;
}

void CommandLine::feedPatientDatas(Core::Patient *patient)
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
