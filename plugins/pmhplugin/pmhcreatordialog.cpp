#include "pmhcreatordialog.h"
#include "pmhbase.h"
#include "pmhdata.h"
#include "pmhcategorymodel.h"
#include "constants.h"

#include <coreplugin/dialogs/helpdialog.h>

#include "ui_pmhcreatordialog.h"

using namespace PMH;

PmhCreatorDialog::PmhCreatorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PmhCreatorDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("Past Medical History Creator"));
    ui->pmhViewer->createNewPmh();
}

PmhCreatorDialog::~PmhCreatorDialog()
{
    delete ui;
}

void PmhCreatorDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    switch (ui->buttonBox->standardButton(button)) {
    case QDialogButtonBox::Save:
        {
            Internal::PmhData *pmh = ui->pmhViewer->pmhData();
            // Feed category model with this new PmhData
            break;
        }
    case QDialogButtonBox::Cancel: break;
    case QDialogButtonBox::Help: break;
    }
}

void PmhCreatorDialog::helpRequested()
{
    Core::HelpDialog::showPage(Constants::H_PMH_CREATOR_PAGE);
}

void PmhCreatorDialog::changeEvent(QEvent *e)
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
