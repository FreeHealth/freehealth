#ifndef PMHCREATORDIALOG_H
#define PMHCREATORDIALOG_H

#include <QDialog>
class QAbstractButton;


namespace PMH {
namespace Ui {
    class PmhCreatorDialog;
}

class PmhCreatorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PmhCreatorDialog(QWidget *parent = 0);
    ~PmhCreatorDialog();

private Q_SLOTS:
    void on_buttonBox_clicked(QAbstractButton *button);
    void helpRequested();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::PmhCreatorDialog *ui;
};

}

#endif // PMHCREATORDIALOG_H
