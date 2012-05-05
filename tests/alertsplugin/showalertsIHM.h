#ifndef SHOWALERTSIHM_H
#define SHOWALERTSIHM_H

#include "ui_ShowAlertsWidget.h"
#include <QWidget>


class ShowAlerts : public QWidget, public Ui::ShowAlertsWidget
{
    Q_OBJECT
    public:
        ShowAlerts(QObject * parent = 0);
        ~ShowAlerts();
};


#endif

