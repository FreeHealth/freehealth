#ifndef DISTANCE_H
#define DISTANCE_H
#include <QDialog>
#include <QTimer>
#include <QHash>

namespace Ui{
    class ChoiceDialog;
}

class distance:public QDialog{
    Q_OBJECT
    public :
        distance(QWidget * parent);
        ~distance();
        double getDistanceNumber();
        int returnDistanceDialog();
        double m_percentValue;
    private slots :
        void value(double val);
        void valueUp();
        void valueDown();
        void valueUpStop();
        void valueDownStop();
        void quickPlus();
        void quickLess();
    private :
        Ui::ChoiceDialog * ui;
        QTimer * m_timerUp;
        QTimer * m_timerDown;
        double m_percent;
        int m_quickInt;
        QHash<int,QString> m_hashPercentages;
};

#endif

