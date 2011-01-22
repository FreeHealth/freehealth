#ifndef CHOICEDIALOG_H
#define CHOICEDIALOG_H
#include <QDialog>
#include <QTimer>
#include <QHash>

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
  void quickPlus();
  void quickLess();
  private :
  Ui::ChoiceDialog * ui;
  QTimer *m_timer;
  double m_percent;
  int m_quickInt;
  QHash<int,QString> m_hashPercentages;
};

#endif
