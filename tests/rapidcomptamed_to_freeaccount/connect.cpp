#include "connect.h"

ConnectionDialog::ConnectionDialog(QObject * parent)
{
    setupUi(this);    
}

ConnectionDialog::~ConnectionDialog(){}

QString ConnectionDialog::host()
{
    return hostEdit->text();
}

QString ConnectionDialog::login()
{
    return logEdit->text();
}

QString ConnectionDialog::password()
{
    return passEdit->text();
}



