#include "ledgerviewer.h"
#include "ui_ledgerviewer.h"
#include "ledgerIO.h"


LedgerViewer::LedgerViewer(QWidget * parent): QWidget(parent),ui(new Ui::LedgerViewerWidget){
    ui->setupUi(this);
}

LedgerViewer::~LedgerViewer(){}
