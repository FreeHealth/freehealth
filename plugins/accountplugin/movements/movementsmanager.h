#ifndef MOVEMENTSMANAGER_H
#define MOVEMENTSMANAGER_H

#include <accountplugin/account_exporter.h>

#include <QObject>
#include <QVariant>
#include <QHash>

class ACCOUNT_EXPORT MovementsManager : public QObject
{
    Q_OBJECT
public:
    MovementsManager();
    ~MovementsManager();

    QHash<int,QVariant> getHashOfValues(int acMovId,
                                        QString &userUid,
                                        int bankId,
                                        int type,
                                        QString &label,
                                        QString &date,
                                        QString &dateValue,
                                        double valueCalculated,
                                        QString &comment,
                                        int validity,
                                        QString &trace,
                                        int isValid,
                                        QString &details);

    double getCalculatedValue(double value,double percentage);
};


#endif

