#include "icdcodeselector.h"
#include "ui_icdcodeselector.h"

IcdCodeSelector::IcdCodeSelector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IcdCodeSelector)
{
    ui->setupUi(this);
}

IcdCodeSelector::~IcdCodeSelector()
{
    delete ui;
}

void IcdCodeSelector::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
