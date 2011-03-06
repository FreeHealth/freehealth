#include "receipts.h"
#include <QApplication>

int main(int argc,char * argv[]){
  QApplication a(argc,argv);
    QString name = "desombre";
    QString firstname = "pierre-marie";
    QString uid = "12345";
    QString birthday = "1961-02-06";
  ReceiptsGUI * r = ReceiptsGUI::getInstance();
  r -> getPatientDatas(name,firstname,uid,birthday);
  r->initialize();
  return a.exec();

}
