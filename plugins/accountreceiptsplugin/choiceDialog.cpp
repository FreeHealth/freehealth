#include "choiceDialog.h"
#include "receiptsmanager.h"
#include "ui_ChoiceDialog.h"
#include "constants.h"
#include <QRadioButton>


using namespace ReceiptsConstants;
choiceDialog::choiceDialog(QWidget * parent):QDialog(parent),ui(new Ui::ChoiceDialog){
    ui->setupUi(this);
    m_percent = 100.00;
    m_percentValue = 100.00;
    receiptsManager manager;
    m_hashPercentages = manager.getPercentages();
    m_quickInt = m_hashPercentages.keys().last();
    ui->percentDoubleSpinBox->setRange(0.00,100.00);
    ui->percentDoubleSpinBox->setValue(100.00);
    ui->percentDoubleSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_timer = new QTimer(this);
    connect(ui->okButton,SIGNAL(pressed()),this,SLOT(accept()));
    connect(ui->percentDoubleSpinBox,SIGNAL(valueChanged(double)),this,SLOT(value(double)));
    connect(ui->plusButton,SIGNAL(pressed()),this,SLOT(valueUp()));
    connect(ui->plusButton,SIGNAL(released()),this,SLOT(valueStop()));
    connect(ui->lessButton,SIGNAL(pressed()),this,SLOT(valueDown()));
    connect(ui->lessButton,SIGNAL(released()),this,SLOT(valueStop()));
    connect(ui->plusConstButton,SIGNAL(pressed()),this,SLOT(quickPlus()));
    connect(ui->lessConstButton,SIGNAL(pressed()),this,SLOT(quickLess()));
}
choiceDialog::~choiceDialog(){
    delete m_timer;
}
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

void choiceDialog::value(double val){
    m_percentValue = val;
}

void choiceDialog::valueUp(){
    connect(m_timer,SIGNAL(timeout()),this,SLOT(doubleSpinBoxUp()));
    m_timer->start(2);
}

void choiceDialog::valueDown(){
    connect(m_timer,SIGNAL(timeout()),this,SLOT(doubleSpinBoxDown()));
    m_timer->start(2);
}


void choiceDialog::valueStop(){
    m_timer->stop();
}

void choiceDialog::doubleSpinBoxUp(){
    double valuePlus = ui->percentDoubleSpinBox->value()+0.01;
    ui->percentDoubleSpinBox->setValue(valuePlus);
}

void choiceDialog::doubleSpinBoxDown(){
    double valueLess = ui->percentDoubleSpinBox->value()-0.01;
    ui->percentDoubleSpinBox->setValue(valueLess);
}

void choiceDialog::quickPlus(){
    if(m_quickInt == m_hashPercentages.keys().last())
        return;
    else{
        m_quickInt++;
        m_percent = m_hashPercentages.value(m_quickInt).toDouble();
 	}
    ui->percentDoubleSpinBox->setValue(m_percent);
}

void choiceDialog::quickLess(){
    if(m_quickInt == 1)
	    return;
  	/*else if(m_percent == 100){
            m_percent = list[0].toDouble();
 		}*/
    else{
        m_quickInt--;
        m_percent = m_hashPercentages.value(m_quickInt).toDouble();
 		}
    ui->percentDoubleSpinBox->setValue(m_percent);
}
