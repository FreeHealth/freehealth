#include "fancytreeview.h"
#include "ui_fancytreeview.h"

FancyTreeView::FancyTreeView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FancyTreeView)
{
    ui->setupUi(this);
}

FancyTreeView::~FancyTreeView()
{
    delete ui;
}

void FancyTreeView::changeEvent(QEvent *e)
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
