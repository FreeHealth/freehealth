#ifndef USERMANAGERMAINWINDOW_H
#define USERMANAGERMAINWINDOW_H

#include <tkUserManager.h>

#include <QObject>

class UserManagerMainWindow : public tkUserManager
{
    Q_OBJECT
public:
    UserManagerMainWindow(QWidget *parent = 0);
    bool initialize();

public Q_SLOTS:
    bool aboutApplication();
    bool applicationHelp();
    bool debugDialog();
};

#endif // USERMANAGERMAINWINDOW_H
