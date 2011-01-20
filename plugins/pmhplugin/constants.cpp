#include "constants.h"

#include <QStringList>
#include <QCoreApplication>

namespace PMH {
namespace Constants {

QString typeToString(int type)
{
    if (type == ChronicDisease)
        return QCoreApplication::translate(MH_CONSTANTS_TR_CONTEXT, "Chronic disease");
    if (type == ChronicDiseaseWithoutAcuteEpisodes)
        return QCoreApplication::translate(MH_CONSTANTS_TR_CONTEXT, "Chronic disease without acute episode");
    if (type == AcuteDisease)
        return QCoreApplication::translate(MH_CONSTANTS_TR_CONTEXT, "Acute disease");
    return QString();
}

QString statusToString(int state)
{
    if (state == IsActive)
        return QCoreApplication::translate(MH_CONSTANTS_TR_CONTEXT, "Active");
    if (state == IsInRemission)
        return QCoreApplication::translate(MH_CONSTANTS_TR_CONTEXT, "In remission");
    if (state == IsQuiescent)
        return QCoreApplication::translate(MH_CONSTANTS_TR_CONTEXT, "Quiescent");
    if (state == IsCured)
        return QCoreApplication::translate(MH_CONSTANTS_TR_CONTEXT, "Cured");
    return QString();
}


}
}
