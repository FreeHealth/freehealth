#ifndef  FINDRECEIPTSVALUES_H
#define  FINDRECEIPTSVALUES_H
#include "ui_findValuesGUI.h"

#include <accountplugin/account_exporter.h>
#include <accountplugin/receipts/receiptsmanager.h>
#include <accountplugin/receipts/xmlcategoriesparser.h>

#include <accountbaseplugin/medicalproceduremodel.h>
#include <accountbaseplugin/thesaurusmodel.h>
#include <accountbaseplugin/constants.h>

#include <QtGui>
#include <QtCore>

using namespace AccountDB;
namespace Ui{
  class findValueDialog;
}
class ACCOUNT_EXPORT findReceiptsValues:public QDialog
{
  Q_OBJECT
public:
    QHash<QString,QString> returnValuesHash();
    findReceiptsValues(QWidget * parent = 0);
    ~findReceiptsValues();
    QHash<QString,QString> getChoosenValues();
    void clear();
private:
    //QSqlTableModel * m_model; //  à modifier par m_mpmodel
    Ui::findValueDialog * ui;
    //receiptsManager * m_rbm;
    xmlCategoriesParser * m_xmlParser;
    MedicalProcedureModel * model;
    QStandardItemModel * itemModel;
    QHash<QString,QString> m_hashValuesChoosen;
    void initialize();
    void fillComboCategories();
    bool tableViewIsFull(QAbstractItemModel * model);
    void enableShowNextTable();
    
private slots:
    void fillListViewValues(const QString & comboItem);
    void chooseValue(const QModelIndex& index);
    void supprItemChoosen(QListWidgetItem * item);
    void on_lineEditFilter_textChanged(const QString & text);
    void showNext();
};


#endif
