/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  The FreeAccount plugins are free, open source FreeMedForms' plugins.   *
 *  (C) 2010-2011 by Pierre-Marie Desombre, MD <pm.desombre@medsyn.fr>     *
 *  and Eric Maeker, MD <eric.maeker@gmail.com>                            *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developpers : Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors :                                                         *
 *      NAME <MAIL@ADRESS>                                                 *
 ***************************************************************************/
#include "choiceDialog.h"
#include "receiptsmanager.h"
#include "ui_ChoiceDialog.h"
#include "constants.h"
#include <QRadioButton>


using namespace ReceiptsConstants;
choiceDialog::choiceDialog(QWidget * parent):QDialog(parent),ui(new Ui::ChoiceDialog){
    ui->setupUi(this);
    ui->distanceDoubleSpinBox->hide();
    ui->distanceGroupBox->hide();
    m_percent = 100.00;
    m_percentValue = 100.00;
    receiptsManager manager;
    m_hashPercentages = manager.getPercentages();
    m_quickInt = m_hashPercentages.keys().last();
    ui->percentDoubleSpinBox->setRange(0.00,100.00);
    ui->percentDoubleSpinBox->setValue(100.00);
    ui->percentDoubleSpinBox->setSingleStep(0.10);
    ui->percentDoubleSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
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
choiceDialog::~choiceDialog(){
    delete m_timerUp;
    delete m_timerDown;
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
    connect(m_timerUp,SIGNAL(timeout()),ui->percentDoubleSpinBox,SLOT(stepUp()));
    m_timerUp->start(10);
}

void choiceDialog::valueDown(){
    connect(m_timerDown,SIGNAL(timeout()),ui->percentDoubleSpinBox,SLOT(stepDown()));
    m_timerDown->start(10);
}


void choiceDialog::valueUpStop(){
    m_timerUp->stop();
}

void choiceDialog::valueDownStop(){
    m_timerDown->stop();
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

double choiceDialog::returnPercentValue(){
    return m_percent;
}
