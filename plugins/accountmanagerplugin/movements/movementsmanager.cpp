#include "movementsmanager.h"
#include "../../accountbaseplugin/constants.h"//<accountbaseplugin/constants.h>

using namespace AccountDB;
using namespace Constants;

movementsManager::movementsManager(){}

movementsManager::~movementsManager(){}

QHash<int,QVariant> movementsManager::getHashOfValues(int acMovId ,
                                            QString & userUid,
                                            int bankId ,
                                            int type ,
                                            QString & label ,
                                            QString & date ,
                                            QString & dateValue ,
                                            double valueCalculated ,
                                            QString & comment,
                                            int validity ,
                                            QString & trace ,
                                            int isValid ,
                                            QString & details){
    QHash<int,QVariant> hash;
    hash.insert(MOV_AV_MOVEMENT_ID,acMovId);
    hash.insert(MOV_USER_UID,userUid);
    hash.insert(MOV_ACCOUNT_ID,bankId);
    hash.insert(MOV_TYPE,type);
    hash.insert(MOV_LABEL,label);
    hash.insert(MOV_DATE,date);
    hash.insert(MOV_DATEOFVALUE,dateValue);
    hash.insert(MOV_AMOUNT,valueCalculated);
    hash.insert(MOV_COMMENT,comment);
    hash.insert(MOV_VALIDITY,validity);
    hash.insert(MOV_TRACE,trace);
    hash.insert(MOV_ISVALID,isValid);
    hash.insert(MOV_DETAILS,details);
    return hash;
}
