#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QtGlobal>
#include <QString>

namespace PMH {
namespace Constants {

    // Translate context
    const char * const  MH_CONSTANTS_TR_CONTEXT = "PMHx";

    // Database
    const char * const  DB_NAME            = "pmh";
    const char * const  DB_FILENAME        = "pmh.db";
    const char * const  DB_ACTUALVERSION   = "0.1";

    // Tables
    enum Tables {
      Table_MASTER = 0,
      Table_EPISODE,
      Table_ICD,
      Table_CATEGORIES,
      Table_CATEGORY_LABEL,
      Table_VERSION
    };

    // Fields
    enum MasterFields {
        MASTER_ID = 0,
        MASTER_PATIENT_UID,
        MASTER_USER_UID,
        MASTER_EPISODE_ID,
        MASTER_CONTACTS_ID,
        MASTER_LABEL,
        MASTER_TYPE,
        MASTER_STATE,
        MASTER_MaxParam
    };

    enum EpisodeFields {
        EPISODE_ID = 0,
        EPISODE_MASTER_ID,
        EPISODE_DATE_START,
        EPISODE_DATE_END,
        EPISODE_CONF_INDEX,
        EPISODE_ICD_CODES,
        EPISODE_LABEL,
        EPISODE_MaxParam
    };

    enum IcdFields {
        ICD_ID = 0,
        ICD_CONTENT,
        ICD_MaxParam
    };

    enum PmhCatFields {
        CATEGORY_ID = 0, CATEGORY_PARENT, CATEGORY_LABEL_ID, CATEGORY_ISRISKFACTOR,
        CATEGORY_ISCHONICDISEASE,CATEGORY_THEMEDICON,
        CATEGORY_MaxParam
    };

    enum PmhCatLabelFields {
        CATEGORYLABEL_ID = 0, CATEGORYLABEL_LANG, CATEGORYLABEL_VALUE,
        CATEGORYLABEL_MaxParam
    };

    enum VersionFields {
        VERSION_TEXT = 0
    };


    // Types of Medical History
    enum MHType {
        ChronicDisease                      = 0x0001,
        ChronicDiseaseWithoutAcuteEpisodes  = 0x0002,
        AcuteDisease                        = 0x0004,
        RiskFactor                          = 0x0100,
    };

    enum MHStatus {
        IsActive                            = 0x01,
        IsInRemission                       = 0x02,
        IsQuiescent                         = 0x04,
        IsCured                             = 0x08
    };

    QString typeToString(int type);
    QString statusToString(int state);

}  // End namespace Constants
}  // End namespace History


#endif // CONSTANTS_H
