#include "mfMainCentralWidget.h"

mfMainCentralWidget::mfMainCentralWidget(QWidget *parent) :
    QWidget(parent){
    setupUi(this);
}

void mfMainCentralWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}
