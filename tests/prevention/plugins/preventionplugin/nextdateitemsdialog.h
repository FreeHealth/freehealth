#ifndef NEXTDATEITEMS_H
#define NEXTDATEITEMS_H

#include "ui_nextdateitemsdialog.h"

#include <QSqlTableModel>

class NextDateDialog : public QDialog, public Ui::NextDateItemsDialog
{
    Q_OBJECT
    public:
        NextDateDialog(const QObject * parent , QStringList & listOfItems, QSqlTableModel * nextDateModel);
        ~NextDateDialog();
        
        QStringList getListForChosenItem();
    private:
        QSqlTableModel * m_model;
};


#endif

