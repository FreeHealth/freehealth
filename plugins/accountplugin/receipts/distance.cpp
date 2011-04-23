#include "distance.h"
#include "constants.h"
#include "receiptsmanager.h"
#include "receiptsIO.h"
#include "ui_ChoiceDialog.h"
#include <QDebug>

using namespace ReceiptsConstants;
distance::distance(QWidget * parent):QDialog(parent),ui(new Ui::ChoiceDialog){
    ui->setupUi(this);
    setWindowTitle("Round trip.");
    m_percent = 100.00;
    m_percentValue = 100.00;
    receiptsManager manager;
    m_hashPercentages = manager.getPercentages();
    m_quickInt = m_hashPercentages.keys().last();
    ui->percentDoubleSpinBox->setRange(0.00,100.00);
    ui->percentDoubleSpinBox->setValue(100.00);
    ui->percentDoubleSpinBox->setSingleStep(0.10);
    ui->distanceDoubleSpinBox->setRange(0.00,100000.00);
    ui->distanceDoubleSpinBox->setSingleStep(0.10);
    m_timerUp = new QTimer(this);
    m_timerDown = new QTimer(this);
    connect(ui->okButton,SIGNAL(pressed()),this,SLOT(accept()));
    connect(ui->quitButton,SIGNAL(pressed()),this,SLOT(reject()));
    connect(ui->percentDoubleSpinBox,SIGNAL(valueChanged(double)),this,SLOT(value(double)));
    connect(ui->plusButton,SIGNAL(pressed()),this,SLOT(valueUp()));
    connect(ui->plusButton,SIGNAL(released()),this,SLOT(valueUpStop()));
    connect(ui->lessButton,SIGNAL(pressed()),this,SLOT(valueDown()));
    connect(ui->lessButton,SIGNAL(released()),this,SLOT(valueDownStop()));
    connect(ui->plusConstButton,SIGNAL(pressed()),this,SLOT(quickPlus()));
    connect(ui->lessConstButton,SIGNAL(pressed()),this,SLOT(quickLess()));
}

distance::~distance(){}

double distance::getDistanceNumber(const QString & data){
    qDebug() << __FILE__ << QString::number(__LINE__) << " data =" << data  ;
    receiptsEngine recIO;
    double dist = 0.00;
    double minDistance = recIO.getMinDistanceValue(data);
    qDebug() << __FILE__ << QString::number(__LINE__) << " minDistance =" << QString::number(minDistance) ;
    dist = ui->distanceDoubleSpinBox->value() - minDistance;
    return dist;
}

int distance::returnDistanceDialog(){
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

void distance::value(double val){
    m_percentValue = val;
}

void distance::valueUp(){
    connect(m_timerUp,SIGNAL(timeout()),ui->percentDoubleSpinBox,SLOT(stepUp()));
    m_timerUp->start(10);
}

void distance::valueDown(){
    connect(m_timerDown,SIGNAL(timeout()),ui->percentDoubleSpinBox,SLOT(stepDown()));
    m_timerDown->start(10);
}


void distance::valueUpStop(){
    m_timerUp->stop();
}

void distance::valueDownStop(){
    m_timerDown->stop();
}


void distance::quickPlus(){
    if(m_quickInt == m_hashPercentages.keys().last())
        return;
    else{
        m_quickInt++;
        m_percent = m_hashPercentages.value(m_quickInt).toDouble();
 	}
    ui->percentDoubleSpinBox->setValue(m_percent);
}

void distance::quickLess(){
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
