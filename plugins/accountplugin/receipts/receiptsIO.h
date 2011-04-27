#ifndef RECEIPTSENGINE_H
#define RECEIPTSENGINE_H

#include <accountplugin/account_exporter.h>
#include <accountbaseplugin/accountmodel.h>
#include <accountbaseplugin/medicalproceduremodel.h>
#include <accountbaseplugin/thesaurusmodel.h>
#include <accountbaseplugin/distancerulesmodel.h>
#include <accountbaseplugin/workingplacesmodel.h>
#include <accountbaseplugin/insurancemodel.h>

#include <QHash>
#include <QSqlDatabase>
#include <QSqlTableModel>


using namespace AccountDB;
class ACCOUNT_EXPORT receiptsEngine : public QObject
{
    Q_OBJECT
public:
    receiptsEngine();
    ~receiptsEngine();
    bool insertIntoAccount(const QHash<int,QVariant> &hashValues, const QString &userUuid);
    QHash<QString,QVariant> getNamesAndValuesFromMP();
    bool insertInThesaurus(const QString &listOfValuesStr, const QString &userUuid);
    bool deleteFromThesaurus(QString & data );
    bool addBoolTrue(QString & data);
    double getMinDistanceValue(const QString & data);
    QHash<int,QVariant> getListOfPreferedValues(QString & userUuid,int choice);

private:
    enum typeOfChoice {
            CASH = 0,
            CHECK,
            VISA,
            BANKING,
            OTHER,
            DUE
        };
   AccountModel *m_mpmodel;
   QSqlDatabase  m_db;
};


#endif
