#ifndef RECEIPTSENGINE_H
#define RECEIPTSENGINE_H

#include <accountbaseplugin/accountmodel.h>

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

private :
   AccountModel *m_mpmodel;
   QSqlDatabase  m_db;
};


#endif
