#ifndef CHOICEDIALOG_H
#define CHOICEDIALOG_H
#include <QDialog>
#include <QTimer>


namespace Ui{
    class ChoiceDialog;
}
class choiceDialog : public QDialog{
  Q_OBJECT
  public:
  choiceDialog(QWidget *parent = 0);
  ~choiceDialog();
  int returnChoiceDialog();
  double m_percentValue;
  private slots:
  void value(double val);
  void valueUp();
  void valueDown();
  void valueStop();
  void doubleSpinBoxUp();
  void doubleSpinBoxDown();
  private :
  Ui::ChoiceDialog * ui;
  QTimer *m_timer;
};

#endif
