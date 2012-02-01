#include "basiclogindialog.h"
#include "ui_basiclogindialog.h"

using namespace Utils;

BasicLoginDialog::BasicLoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BasicLoginDialog)
{
    ui->setupUi(this);
    ui->loginWidget->focusLogin();
}

BasicLoginDialog::~BasicLoginDialog()
{
    delete ui;
}

void BasicLoginDialog::setToggleViewIcon(const QString &fullAbsPath)
{
    ui->loginWidget->setToggleViewIcon(fullAbsPath);
}

QString BasicLoginDialog::login() const
{
    return ui->loginWidget->login();
}

QString BasicLoginDialog::password() const
{
    return ui->loginWidget->password();
}
