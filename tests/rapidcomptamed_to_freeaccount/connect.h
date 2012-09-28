#ifndef CONNECT_H
#define CONNECT_H

#include "ui_connexiondialog.h"
#include <QtSql>

class ConnectionDialog:public QDialog,public Ui::ConnectionDialog
{
    Q_OBJECT
    public:
        ConnectionDialog(QObject * parent = 0);
        ~ConnectionDialog();
    	QString host();
    	QString login();
    	QString password();   

};

#endif


