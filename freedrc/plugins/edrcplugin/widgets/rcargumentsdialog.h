#ifndef RCARGUMENTSDIALOG_H
#define RCARGUMENTSDIALOG_H

#include <QDialog>

namespace Ui {
class RcArgumentsDialog;
}

class RcArgumentsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit RcArgumentsDialog(QWidget *parent = 0);
    ~RcArgumentsDialog();
    void setRcId(const int rcId);

private:
    Ui::RcArgumentsDialog *ui;
};

#endif // RCARGUMENTSDIALOG_H
