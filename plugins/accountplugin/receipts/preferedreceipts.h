#ifndef PREFEREDRECEIPTS_H
#define PREFEREDRECEIPTS_H

#include <accountbaseplugin/accountmodel.h>
#include <accountbaseplugin/constants.h>
#include <accountplugin/account_exporter.h>

#include <QWidget>
#include <QHash>

namespace Ui{
    class PreferedReceiptsWidget;
}

class ACCOUNT_EXPORT PreferedReceipts: public QWidget {
    Q_OBJECT
    public :
        PreferedReceipts(QWidget * parent);
        ~PreferedReceipts();
    private :
        void insertPreferedValuesIntoAccount();
        Ui::PreferedReceiptsWidget *ui;
        int m_typeOfChoice;
};

#endif

