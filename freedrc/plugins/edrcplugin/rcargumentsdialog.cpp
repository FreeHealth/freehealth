#include "rcargumentsdialog.h"
#include "edrcbase.h"
#include "edrccore.h"

#include "ui_rcargumentsdialog.h"

#include <utils/global.h>

static inline eDRC::EdrcCore &edrcCore() {return eDRC::EdrcCore::instance();}
static inline eDRC::Internal::DrcDatabase &edrcBase() {return eDRC::EdrcCore::instance().edrcBase();}

RcArgumentsDialog::RcArgumentsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RcArgumentsDialog)
{
    ui->setupUi(this);
    resize(500, 350);
    Utils::centerWidget(this, parent);
}

RcArgumentsDialog::~RcArgumentsDialog()
{
    delete ui;
}

void RcArgumentsDialog::setRcId(const int rcId)
{
    ui->rcLabel->setText(edrcBase().getRcLabel(rcId));
    ui->arguments->setHtml(edrcBase().getRcArguments(rcId, true));
}
