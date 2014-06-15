#include "servercreationdialog.h"
#include "ui_servercreationdialog.h"

ServerCreationDialog::ServerCreationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ServerCreationDialog)
{
    ui->setupUi(this);
}

ServerCreationDialog::~ServerCreationDialog()
{
    delete ui;
}
