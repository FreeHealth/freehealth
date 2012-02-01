#include "basiclogindialog.h"
#include "ui_basiclogindialog.h"

using namespace Utils;

BasicLoginDialog::BasicLoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BasicLoginDialog)
{
    ui->setupUi(this);
    ui->loginWidget->focusLogin();
    ui->loginWidget->togglePasswordEcho(false);
    adjustSize();
}

BasicLoginDialog::~BasicLoginDialog()
{
    delete ui;
}

void BasicLoginDialog::setTitle(const QString &title)
{
    ui->title->setText(title);
    this->setWindowTitle(title);
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
