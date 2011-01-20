#include "pmhviewer.h"
#include "ui_pmhviewer.h"

using namespace PMH;
using namespace Internal;

PmhViewer::PmhViewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PmhViewer)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentWidget(ui->categoryTab);

    // Insert ICD widget inside icdTab
    //
}

PmhViewer::~PmhViewer()
{
    delete ui;
}

void PmhViewer::changeEvent(QEvent *e)
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
