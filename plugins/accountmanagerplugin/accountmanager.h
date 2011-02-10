#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H
#include <QWidget>

namespace Ui{
    class AccountManagerWidget;
}

class accountManager : public QWidget {
    Q_OBJECT
    public :
        accountManager(QWidget * parent);
        ~accountManager();
    private :
        Ui::AccountManagerWidget * ui;
};


#endif

