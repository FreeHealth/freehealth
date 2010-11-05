#ifndef  FINDRECEIPTSVALUES_H
#define  FINDRECEIPTSVALUES_H
#include "ui_findValuesGUI.h"
#include <QtGui>
#include <QtCore>

namespace Ui{
  class findValueDialog;
}
class findReceiptsValues:public QDialog{
  Q_OBJECT
  public :
    QHash<QString,QString> returnValuesHash();

      findReceiptsValues(QWidget * parent = 0);
    ~findReceiptsValues();
  private :
    Ui::findValueDialog * ui;
    
  private slots :
  
};
#endif
