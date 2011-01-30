#ifndef RECEIPTSENGINE_H
#define RECEIPTSENGINE_H

#include <accountbaseplugin/accountmodel.h>
#include <accountbaseplugin/medicalproceduremodel.h>
#include <accountbaseplugin/thesaurusmodel.h>
#include <QHash>
#include <QSqlDatabase>
#include <QSqlTableModel>


using namespace AccountDB;
class receiptsEngine : public QObject {    
    Q_OBJECT
public :
    receiptsEngine();
    ~receiptsEngine();
    bool insertIntoAccount(QHash<int,QVariant> & hashValues);
    QHash<QString,QVariant> getNamesAndValuesFromMP();
    bool insertInThesaurus(QString & listOfValuesStr,QString & userUuid);

private :
   AccountModel *m_mpmodel;
   QSqlDatabase  m_db;
};


#endif
