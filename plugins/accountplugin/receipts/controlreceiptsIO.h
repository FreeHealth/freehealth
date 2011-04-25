/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       Pierre-Marie Desombre <pm.desombre@gmail.com>                     *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
 
#ifndef CONTROLRECEIPTSIO_H
#define CONTROLRECEIPTSIO_H

#include <accountbaseplugin/accountmodel.h>
#include <accountbaseplugin/constants.h>
#include <accountplugin/account_exporter.h>

#include <QWidget>
#include <QMap>

namespace Ui{
    class ControlReceiptsWidget;
}

using namespace AccountDB;
class ACCOUNT_EXPORT ControlReceipts : public QWidget
{
    Q_OBJECT
    public :
        ControlReceipts(QWidget * parent);
        ~ControlReceipts();
        void resizeControlReceipts(QWidget * parent);
    private slots :
        void search();
        void deleteLine();
        void printDues();
        void print();
    
    private:
        void coloringDoubles();
        void refresh();
        void refreshFilter(const QString & filter);
        QString textOfSums(AccountModel * model);
        Ui::ControlReceiptsWidget * ui;
        AccountModel * m_accountModel;
        QString m_userUuid;
        QMap<QString,QString> m_mapCombo;
        QString m_typeOfMoney;
};

#endif

