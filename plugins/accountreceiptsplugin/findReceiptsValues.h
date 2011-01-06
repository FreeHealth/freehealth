#ifndef  FINDRECEIPTSVALUES_H
#define  FINDRECEIPTSVALUES_H

#include <QtGui>
#include <QtCore>


class xmlCategoriesParser;
class receiptsManager;


namespace AccountDB {
class MedicalProcedureModel;
}  // End namespace Account

namespace Ui{
  class findValueDialog;
}

class findReceiptsValues:public QDialog
{
    Q_OBJECT

public:
    findReceiptsValues(QWidget * parent = 0);
    ~findReceiptsValues();

    QHash<QString,QString> returnValuesHash();
    QHash<QString,QString> getChoosenValues();
    void clear();

private:
    void initialize();
    void fillComboCategories();

private Q_SLOTS:
    void fillListViewValues(const QString & comboItem);
    void chooseValue(const QModelIndex& index);
    void supprItemChoosen(QListWidgetItem * item);


private:
    //QSqlTableModel * m_model; //  à modifier par m_mpmodel
    Ui::findValueDialog *ui;
    receiptsManager *m_rbm;
    xmlCategoriesParser *m_xmlParser;
    AccountDB::MedicalProcedureModel *m_mpmodel;
    QHash<QString,QString> m_hashValuesChoosen;
};


#endif
