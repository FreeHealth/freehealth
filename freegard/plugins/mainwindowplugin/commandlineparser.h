#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H

#include <QVariant>
#include <QHash>
#include <QString>
#include <QApplication>
#include <QDebug>

namespace Core {
class CommandLine 
{
public:
    enum Param {
        CL_MedinTux = 0,
        CL_ExchangeFile,
        CL_PatientName,
        CL_DateOfBirth,
        CL_Weight,
        CL_Size,
        CL_ClCr,
        CL_Creatinin,
        CL_DrugsAllergies,
        CL_ICD10Diseases,
        CL_Chrono,
        CL_TransmitDosage,
        CL_ConfigFile,
        CL_RunningUnderWine,
        CL_MaxParam
    };

    CommandLine()
    {
        ref.insert(CL_MedinTux, "--medintux");
        ref.insert(CL_ExchangeFile, "--exchange");
        ref.insert(CL_PatientName, "--patientname");
        ref.insert(CL_DateOfBirth, "--dateofbirth");
        ref.insert(CL_Weight, "--weight");
        ref.insert(CL_Size, "--size");
        ref.insert(CL_ClCr, "--clcr");
        ref.insert(CL_Chrono, "--chrono");
        ref.insert(CL_Creatinin, "--creatinin");
        ref.insert(CL_DrugsAllergies, "--notdrugs");
        ref.insert(CL_ICD10Diseases, "--icd");
        ref.insert(CL_TransmitDosage, "--transmit-dosage");
        ref.insert(CL_ConfigFile, "--config");
        ref.insert(CL_RunningUnderWine, "--wine");
        const QStringList &args = qApp->arguments();
        qWarning() << "args" << args;
        foreach(const QString &a, args) {
            QString k;
            if (a.contains(" "))
                k = a.left(a.indexOf(" "));
            if (a.contains("="))
                k = a.left(a.indexOf("="));
            switch (ref.key(k,-1))
            {
                case CL_MedinTux : params.insert(CL_MedinTux, true); break;
                case CL_ExchangeFile : params.insert(CL_ExchangeFile, a.mid(a.indexOf("=")+1).remove("\"")); break;
                case CL_DateOfBirth : params.insert(CL_DateOfBirth, a.mid(a.indexOf("=")+1).remove("\"")); break;
                case CL_Weight : params.insert(CL_Weight, a.mid(a.indexOf("=")+1).remove("\"")); break;
                case CL_Size : params.insert(CL_Size, a.mid(a.indexOf("=")+1).remove("\"")); break;
                case CL_ClCr : params.insert(CL_ClCr, a.mid(a.indexOf("=")+1).remove("\"")); break;
                case CL_Creatinin : params.insert(CL_Creatinin, a.mid(a.indexOf("=")+1).remove("\"")); break;
                case CL_Chrono : params.insert(CL_Chrono, true); break;
                case CL_TransmitDosage : params.insert(CL_TransmitDosage, true); break;
                case CL_ConfigFile : params.insert(CL_ConfigFile, a.mid(a.indexOf("=")+1).remove("\"")); break;
                case CL_RunningUnderWine : params.insert(CL_RunningUnderWine, true); break;
                    /** \todo Add ICD10, Drugs Allergies */
                default : break;
            }
        }
    }

    QVariant value(int param, const QVariant &def = QVariant()) const
    {
        return params.value(param,def);
    }

    QString paramName(int param) const
    {
        if (ref.keys().contains(param))
            return ref.value(param);
        else
            return QString::number(param);
    }

private:
    QHash<int,QVariant> params;
    QHash<int, QString> ref;
};

}

#endif // COMMANDLINEPARSER_H
