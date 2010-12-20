#ifndef CHOICEDIALOG_H
#define CHOICEDIALOG_H
#include <QDialog>

namespace Ui{
    class ChoiceDialog;
}
class choiceDialog : public QDialog{
  Q_OBJECT
  public:
  choiceDialog(QWidget *parent = 0);
  ~choiceDialog();
  int returnChoiceDialog();
  private :
  Ui::ChoiceDialog * ui;
};

#endif
