#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace PreventionConstants {
          enum Prevention_Items {
          ID_PREVENTION = 0,
          ITEM,
          TYPE_OF_ITEM,
          PARENT_ITEM,
          PARENT_OR_CHILD,
          ICON,
          DATE_DONE,
          DATE_NEXT,
          ABSTRACT,
          ID_ITEM,
          PATIENT_UID,
          USER_UID,          
          RESULT,
          Prevention_Items_MaxParam
          };
      enum Headers {
          ITEM_H = 0,
          TYPE_OF_ITEM_H,
          PARENT_ITEM_H,
          PARENT_OR_CHILD_H,
          ICON_H,
          DATE_DONE_H,
          DATE_NEXT_H,
          ABSTRACT_H,
          ID_ITEM_H,
          PATIENT_UID_H,
          USER_UID_H,          
          RESULT_H,
          Headers_MaxParam
          };
        enum NextDateItems {
            ID_NEXTDATE = 0,
            ND_ITEM,
            ND_YEAR,
            ND_MONTH,
            ND_DAY,
            ND_HOURS,
            ND_MIN,
            ND_USER_UID,
            NextDateItems_MaxParam
            };


};
#endif

