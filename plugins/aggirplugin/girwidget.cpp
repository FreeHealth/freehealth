/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "girwidget.h"
#include "ui_girwidget.h"

#include <formmanagerplugin/iformitem.h>
#include <medicalutils/aggir/girscore.h>

#include <QDebug>

using namespace Gir;
using namespace Gir::Internal;

GirWidgetFactory::GirWidgetFactory(QObject *parent) :
        IFormWidgetFactory(parent)
{
}

Form::IFormWidget *GirWidgetFactory::createWidget(const QString &name, Form::FormItem *object, QWidget *parent)
{
    if ((name.compare("aggir",Qt::CaseInsensitive)==0) ||
        (name.compare("gir",Qt::CaseInsensitive)==0)) {
        return new GirWidget(object, parent);
    }
    return 0;
}

//--------------------------------------------------------------------------------------------------------
//------------------------------------------------ GirUi -------------------------------------------------
//--------------------------------------------------------------------------------------------------------
GirUi::GirUi(QWidget *parent) :
    QWidget(parent),
    m_ui(0)
{
    m_ui = new Ui::GirWidget();
    m_ui->setupUi(this);
    // connect all QButtonGroup to Gir string preparation
    foreach(QButtonGroup *gr, this->findChildren<QButtonGroup*>()) {
        connect(gr, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(updateGirString(QAbstractButton*)));
    }
    m_GirString.fill('_',13);
    m_Gir = 0;
}

GirUi::~GirUi()
{
    if (m_ui) delete m_ui;
    m_ui=0;
}

inline static void allRadioOn(const QString &s, QWidget *parent)
{
    foreach(QRadioButton *r, parent->findChildren<QRadioButton*>()) {
        if (r->text().compare(s,Qt::CaseInsensitive)==0)
            r->setChecked(true);
    }
}

void GirUi::on_aButton_clicked()
{
    allRadioOn("A",this);
    m_GirString.fill('A',13);
    updateGirString(0);
}

void GirUi::on_bButton_clicked()
{
    allRadioOn("B",this);
    m_GirString.fill('B',13);
    updateGirString(0);
}

void GirUi::on_cButton_clicked()
{
    allRadioOn("C",this);
    m_GirString.fill('C',13);
    updateGirString(0);
}

inline static int calculateGirScore(const QString &s)
{
    MedicalUtils::AGGIR::GirScore score;
    score.setValues(s[0], s[1], s.mid(2,2), s.mid(4,3), s.mid(7,2), s.mid(9,2),s[11], s[12]);
    return score.resultingGir();
}

void GirUi::updateGirString(QAbstractButton *radio)
{
    QButtonGroup *p = 0;
    foreach(QButtonGroup *gr, this->findChildren<QButtonGroup*>()) {
        if (gr->buttons().contains(radio)) {
            p = gr;
            break;
        }
    }
    // all QButtonGroup are name like GR_X where X is the id in the Gir string to fill
    if (p) {
        int id = p->objectName().mid(3).toInt();
        if (id < m_GirString.count())
            m_GirString[id]=radio->text()[0];
    }
    if (!m_GirString.contains("_")) {
        m_Gir = calculateGirScore(m_GirString);
        m_ui->girBar->setValue(7-m_Gir);
        m_ui->girLineEdit->setText(QString::number(m_Gir));
    }
}


//--------------------------------------------------------------------------------------------------------
//---------------------------------------------- GirWidget -----------------------------------------------
//--------------------------------------------------------------------------------------------------------
GirWidget::GirWidget(Form::FormItem *formItem, QWidget *parent) :
    Form::IFormWidget(formItem,parent)
{
    QBoxLayout * hb = getBoxLayout(Label_OnTop, m_FormItem->spec()->label(), this);
    m_Label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    hb->addWidget(m_Label);
    hb->addWidget(new GirUi(this));
    adjustSize();
}

GirWidget::~GirWidget()
{
}

void GirWidget::setValue(const QVariant &)
{
}

QVariant GirWidget::value() const
{
    return QVariant();
}
