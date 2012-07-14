#ifndef IPREVENTION_H
#define IPREVENTION_H

#include <QObject>
#include <QVector>
#include <QVariant>

namespace PreventionEngine {

class IPreventionEngine : public QObject {
    Q_OBJECT
    public :
      enum Parenthood {
          PARENT = 0,
          CHILD,
          Parenthood_MaxParam
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
      enum Types_Of_Items {
          PRIMARY_PREVENTION_ITEM = 0,
          SECONDARY_PREVENTION_ITEM,
          Types_Of_Items_MaxParam
          };
      enum Icons {//see enum in treeviewdelegate
          PREVENT_OK = 0,
          PREVENT_WARNING,
          PREVENT_PAST,
          Icons_MaxParam
          };
      enum Uuid_List {
          PATIENT = 0,
          USER
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
        IPreventionEngine (const QVector<QVariant> &data = QVector<QVariant>(),QObject * parent = 0) {}
        virtual ~IPreventionEngine() {}
        
        static IPreventionEngine * preventionEnginePtr();
        
        virtual QObject *child(int number) =0;
        virtual int childCount() const =0;
        virtual int columnCount() const =0;
        virtual QVariant data(int column) const =0;
        virtual QVector<QVariant> datas(int columns) =0;
        virtual bool insertChildren(int position, int count, int columns) =0;
        virtual bool insertColumns(int position, int columns) =0;
        virtual QObject * parent() =0;
        virtual bool removeChildren(int position, int count) =0;
        virtual bool removeColumns(int position, int columns) =0;
        virtual int childNumber() const =0;
        virtual bool setData(int column, const QVariant&) =0;
       
};

}

#endif

