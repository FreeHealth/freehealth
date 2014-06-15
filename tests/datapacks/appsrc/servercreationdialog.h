#ifndef SERVERCREATIONDIALOG_H
#define SERVERCREATIONDIALOG_H

#include <QDialog>

namespace Ui {
class ServerCreationDialog;
}

class ServerCreationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ServerCreationDialog(QWidget *parent = 0);
    ~ServerCreationDialog();

private:
    Ui::ServerCreationDialog *ui;
};

#endif // SERVERCREATIONDIALOG_H
