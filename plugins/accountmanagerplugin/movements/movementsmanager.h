#ifndef MOVEMENTSMANAGER_H
#define MOVEMENTSMANAGER_H
#include <QObject>
#include <QtCore>

class movementsManager : public QObject {
    Q_OBJECT
    public :
        movementsManager();
        ~movementsManager();
        QHash<int,QVariant> getHashOfValues(int acMovId ,
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
                                            QString & details);
         double getCalculatedValue(double value,double percentage);
};


#endif

