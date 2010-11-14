#ifndef  FINDRECEIPTSVALUES_H
#define  FINDRECEIPTSVALUES_H
#include "ui_findValuesGUI.h"
#include "xmlcategoriesparser.h"
#include "receiptsmanager.h"
#include <accountbaseplugin/medicalproceduremodel.h>

#include <QtGui>
#include <QtCore>

using namespace AccountDB;
namespace Ui{
  class findValueDialog;
}
class findReceiptsValues:public QDialog{
  Q_OBJECT
  public :
    QHash<QString,QString> returnValuesHash();
    findReceiptsValues(QWidget * parent = 0);
    ~findReceiptsValues();
    QHash<QString,QString> getChoosenValues();
    void clear();
  private :
    //QSqlTableModel * m_model; //  à modifier par m_mpmodel
    Ui::findValueDialog * ui;
    receiptsManager * m_rbm;
    xmlCategoriesParser * m_xmlParser;
    MedicalProcedureModel * m_mpmodel;
    QHash<QString,QString> m_hashValuesChoosen;
    void initialize();
    void fillComboCategories();
    
  private slots :
    void fillListViewValues(const QString & comboItem);
    void chooseValue(const QModelIndex& index);
    void supprItemChoosen(QListWidgetItem * item);
};
#endif
