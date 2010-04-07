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
    const char *const XML_GENERAL_TAG             = "FreeAccount";
    const char *const XML_CONFIG_FILE             = "ConfigFile";
    const char *const XML_INFILE                  = "InFile";
    const char *const XML_EMR                     = "EMR";
    const char *const XML_PATIENT                 = "Patient";
    const char *const XML_PATIENT_IDENTITY        = "Identity";
    const char *const XML_UI                      = "Ui";

    const char *const XML_ATTRIB_UI_EDITMODE      = "editmode";
    const char *const XML_ATTRIB_UI_BLOCKDATAS    = "blockPatientDatas";

    const char *const XML_ATTRIB_VALUE            = "value";
    const char *const XML_ATTRIB_UNIT             = "unit";
    const char *const XML_ATTRIB_UID              = "uid";
    const char *const XML_ATTRIB_NAME             = "name";
    const char *const XML_ATTRIB_SURNAME          = "surname";
    const char *const XML_ATTRIB_VERSION          = "version";
    const char *const XML_ATTRIB_DATEOFBIRTH      = "dob";
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
        params.insert(CommandLine::CL_Test,              "--test");
        params.insert(CommandLine::CL_MedinTux,          "--medintux");
        params.insert(CommandLine::CL_EMR_Name,          "--emr-name");
        params.insert(CommandLine::CL_EMR_Uid,           "--emr-uid");
        params.insert(CommandLine::CL_SelectionOnly,     "--selection-only");
        params.insert(CommandLine::CL_ExchangeFile,      "--exchange");
        params.insert(CommandLine::CL_ExchangeInFile,    "--exchange-in");
        params.insert(CommandLine::CL_PatientUid,        "--patientuid");
        params.insert(CommandLine::CL_PatientName,       "--patientname");
        params.insert(CommandLine::CL_PatientSurname,    "--patientsurname");
        params.insert(CommandLine::CL_DateOfBirth,       "--dateofbirth");
        params.insert(CommandLine::CL_Chrono,            "--chrono");
        params.insert(CommandLine::CL_ConfigFile,        "--config");
        params.insert(CommandLine::CL_RunningUnderWine,  "--wine");
        params.insert(CommandLine::CL_BlockPatientDatas, "--blockpatientdatas");
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
            case CommandLine::CL_Test :            value.insert(CommandLine::CL_Test, true); break;
            case CommandLine::CL_MedinTux :        value.insert(CommandLine::CL_MedinTux, true); break;
            case CommandLine::CL_EMR_Name :        value.insert(CommandLine::CL_EMR_Name, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::CL_EMR_Uid :         value.insert(CommandLine::CL_EMR_Uid, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::CL_SelectionOnly :   value.insert(CommandLine::CL_SelectionOnly, true); break;
            case CommandLine::CL_ExchangeFile :    value.insert(CommandLine::CL_ExchangeFile, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::CL_ExchangeInFile :  value.insert(CommandLine::CL_ExchangeInFile, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::CL_PatientUid :      value.insert(CommandLine::CL_PatientUid, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::CL_PatientName :     value.insert(CommandLine::CL_PatientName, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::CL_DateOfBirth :     value.insert(CommandLine::CL_DateOfBirth, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::CL_Chrono :          value.insert(CommandLine::CL_Chrono, true); break;
            case CommandLine::CL_ConfigFile :      value.insert(CommandLine::CL_ConfigFile, a.mid(a.indexOf("=")+1).remove("\"")); break;
            case CommandLine::CL_RunningUnderWine: value.insert(CommandLine::CL_RunningUnderWine, true); break;
            case CommandLine::CL_BlockPatientDatas:value.insert(CommandLine::CL_BlockPatientDatas, true); break;
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
//          <InFile value="/path/to/the/fileToRead.exc"/>
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
            } else if (element.tagName() == Internal::Constants::XML_EMR) {
                value.insert(CommandLine::CL_EMR_Name, element.attribute(Internal::Constants::XML_ATTRIB_NAME));
                value.insert(CommandLine::CL_EMR_Uid, element.attribute(Internal::Constants::XML_ATTRIB_UID));
            } else if (element.tagName() == Internal::Constants::XML_UI) {
                if (element.attribute(Internal::Constants::XML_ATTRIB_UI_EDITMODE) == "select-only")
                    value.insert(CommandLine::CL_SelectionOnly, true);
                else
                    value.insert(CommandLine::CL_SelectionOnly, false);
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
                value.insert(CommandLine::CL_DateOfBirth, QDate::fromString(element.attribute(Internal::Constants::XML_ATTRIB_DATEOFBIRTH),"yyyy/MM/dd"));
            }
            element = element.nextSiblingElement();
        }
        // pass datas to patient object
    }

    void feedPatientDatas(Core::Patient *patient)
    {
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
