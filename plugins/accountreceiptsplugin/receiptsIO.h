#ifndef RECEIPTSENGINE_H
#define RECEIPTSENGINE_H

#include <accountbaseplugin/accountmodel.h>

#include <QHash>
#include <QSqlDatabase>

using namespace AccountDB;
class receiptsEngine : public QObject {    
    Q_OBJECT
public :
    receiptsEngine();
    ~receiptsEngine();
    bool insertIntoAccount(const QHash<QString,QString> &hashOfValues, const QHash<int,QString> &hashOfParams);

private :
   AccountModel *m_mpmodel;
   QSqlDatabase  m_db;
};


#endif
