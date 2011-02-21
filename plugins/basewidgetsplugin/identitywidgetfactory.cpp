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
#include "identitywidgetfactory.h"

#include <patientbaseplugin/identitywidget.h>
#include <patientbaseplugin/patientmodel.h>
#include <patientbaseplugin/patientwidgetmanager.h>
#include <patientbaseplugin/patientselector.h>

#include <formmanagerplugin/iformitem.h>

#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>

#include <translationutils/constanttranslations.h>

using namespace BaseWidgets;
using namespace Trans::ConstantTranslations;

static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}

IdentityWidgetFactory::IdentityWidgetFactory(QObject *parent) :
        IFormWidgetFactory(parent)
{
}

IdentityWidgetFactory::~IdentityWidgetFactory()
{}

bool IdentityWidgetFactory::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);
    return true;
}

bool IdentityWidgetFactory::extensionInitialized()
{
    return true;
}

bool IdentityWidgetFactory::isInitialized() const
{
    return true;
}

QStringList IdentityWidgetFactory::providedWidgets() const
{
    return QStringList() << "identity" << "ident";
}

bool IdentityWidgetFactory::isContainer(const int idInStringList) const
{
    Q_UNUSED(idInStringList);
    return true;
}

Form::IFormWidget *IdentityWidgetFactory::createWidget(const QString &name, Form::FormItem *formItem, QWidget *parent)
{
    return new IdentityFormWidget(formItem,parent);
}


inline static int getNumberOfColumns(Form::FormItem *item, int defaultValue = 1)
{
    if (!item->extraDatas().value("column").isEmpty())
        return item->extraDatas().value("column").toInt();
    else
        return defaultValue;
}

IdentityFormWidget::IdentityFormWidget(Form::FormItem *formItem, QWidget *parent)
        : Form::IFormWidget(formItem,parent), m_ContainerLayout(0)
{
    setObjectName("IdentityFormWidget");
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);
    QWidget *mainWidget = new QWidget;
    mainLayout->addWidget(mainWidget);
    mainLayout->addStretch();

    m_ContainerLayout = new QGridLayout(mainWidget);
    m_ContainerLayout->setMargin(0);
    m_ContainerLayout->setSpacing(0);
//    IFormWidget::createLabel(tkTr(Trans::Constants::IDENTITY_TEXT), Qt::AlignCenter);
//    m_Label->setFrameStyle(FormLabelFrame);
//    QFont font = m_Label->font();
//    font.setBold(true);
//    m_Label->setFont(font);
//    QSizePolicy sizePolicy = m_Label->sizePolicy();
//    sizePolicy.setVerticalPolicy(QSizePolicy::Fixed);
//    m_Label->setSizePolicy(sizePolicy);

    // Retrieve the number of columns for the gridlayout (lays in extraDatas() of linked FormItem)
    numberColumns = getNumberOfColumns(m_FormItem);
//    if (isCompactView(m_FormItem)) {
//        mainLayout->setMargin(0);
//        mainLayout->setSpacing(2);
//        m_ContainerLayout->setMargin(0);
//        m_ContainerLayout->setSpacing(2);
//    }
//    m_ContainerLayout->addWidget(m_Label, 0, 0,  1, numberColumns);
    if (formItem->extraDatas().value("option", QString()).compare("readonly", Qt::CaseInsensitive) == 0)
        m_Identity = new Patients::IdentityWidget(this);
    else
        m_Identity = new Patients::IdentityWidget(this, Patients::IdentityWidget::ReadWriteMode);
    m_Identity->setCurrentPatientModel(Patients::PatientModel::activeModel());
    m_ContainerLayout->addWidget(m_Identity, 1, 0,  1, numberColumns);
    i = numberColumns;
    row = 1;
    col = 0;

    connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(onCurrentPatientChanged()));

    // create itemdata
    IdentityWidgetData *datas = new IdentityWidgetData(m_FormItem);
    datas->setIdentityFormWiget(this);
    m_FormItem->setItemDatas(datas);
}

IdentityFormWidget::~IdentityFormWidget()
{
}

void IdentityFormWidget::addWidgetToContainer(IFormWidget *widget)
{
    if (!widget)
        return;
    if (!m_ContainerLayout)
        return;
    col = (i % numberColumns);
    row = (i / numberColumns);
    m_ContainerLayout->addWidget(widget , row, col);
    i++;
}

void IdentityFormWidget::onCurrentPatientChanged()
{
    m_Identity->setCurrentIndex(patient()->currentPatientIndex());
}

void IdentityFormWidget::retranslate()
{
    /** \todo iformitem --> one spec per language ? */
    //     m_Label->setText(m_FormItem->spec()->label());
}



bool IdentityWidgetData::isModified() const
{
    return m_Widget->m_Identity->isModified();
}

QVariant IdentityWidgetData::storableData() const
{
    m_Widget->m_Identity->submit(); return QVariant();
}
