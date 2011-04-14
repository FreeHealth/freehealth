#ifndef CHOICEDIALOG_H
#define CHOICEDIALOG_H

#include <accountplugin/account_exporter.h>

#include <QDialog>
#include <QTimer>
#include <QHash>

namespace Ui{
    class ChoiceDialog;
}
class ACCOUNT_EXPORT choiceDialog : public QDialog
{
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
  void valueUpStop();
  void valueDownStop();
  void quickPlus();
  void quickLess();
private:
  Ui::ChoiceDialog * ui;
  QTimer * m_timerUp;
  QTimer * m_timerDown;
  double m_percent;
  int m_quickInt;
  QHash<int,QString> m_hashPercentages;
};

#endif
