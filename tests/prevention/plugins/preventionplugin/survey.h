#ifndef SURVEY_H
#define SURVEY_H

#include "preventIO.h"
#include <QTimer>
#include <QVariant>
#include <QHash>


class Survey : public QObject
{
    Q_OBJECT
    public:
        Survey(QObject * parent = 0);
        ~Survey();
        QHash<int,QVariant> getHashIconDate();
    signals :
        void iconsReset(const QHash<int,QVariant> & hash);
    private Q_SLOTS:
        void resetIcons();
    private:
        void setTimer();
        QTimer * m_timer;
        PreventIO *m_io;
        QHash<int,QVariant> m_hashIconIdItem;
    
};


#endif

