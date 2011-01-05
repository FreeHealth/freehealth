#include "choiceDialog.h"
#include "ui_ChoiceDialog.h"
#include "constants.h"
#include <QRadioButton>


using namespace ReceiptsConstants;
choiceDialog::choiceDialog(QWidget * parent):QDialog(parent),ui(new Ui::ChoiceDialog){
    ui->setupUi(this);
    connect(ui->okButton,SIGNAL(pressed()),this,SLOT(accept()));
}
choiceDialog::~choiceDialog(){}
int choiceDialog::returnChoiceDialog(){
    int ret = 0;
        if (ui->cashButton->isChecked())
    {
    	ret = Cash;
        }
        if (ui->checkButton->isChecked())
    {
    	ret = Check;
        }
        if (ui->visaButton->isChecked())
    {
    	ret = Visa;
        }
        if (ui->bankingButton->isChecked())
    {
    	ret = Banking;
        }
        if (ui->otherButton->isChecked())
    {
    	ret = Other;
        }
        if (ui->dueButton->isChecked())
    {
    	ret = Due;
        }
    return ret;
    
}
