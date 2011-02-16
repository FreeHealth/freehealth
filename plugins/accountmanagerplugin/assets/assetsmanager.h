#ifndef ASSETSMANAGER_H
#define ASSETSMANAGER_H
#include <QHash>
#include <QtCore>

class assetsManager : public QObject {
    Q_OBJECT
    public :
        assetsManager();
        ~assetsManager();
        QHash<int,QVariant> getHashOfValues(       QString & userUid,
                                                   int accountId,
                                                   QString & label,
                                                   QString & date,
                                                   double duration,
                                                   int mode,
                                                   double value,
                                                   double taxActs,
                                                   double residualValue,
                                                   QVariant & result,
                                                   QVariant & movement,
                                                   QString & comments,
                                                   QVariant & trace);
        QHash<int,QString> getHashForModeComboBox();
        double getResidualValue(QString & dateBegin , double value,int mode , double duration);
    private :
        double linearCalc(double value , double  duration , QString & date);
        double decreasingCalc(double  val , double  duration , QString & date);
};

#endif

