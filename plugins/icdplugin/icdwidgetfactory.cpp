/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "icdwidgetfactory.h"
#include "icdcentralwidget.h"

#include <formmanagerplugin/iformitem.h>


using namespace ICD;


IcdWidgetFactory::IcdWidgetFactory(QObject *parent) :
        IFormWidgetFactory(parent)
{
}

IcdWidgetFactory::~IcdWidgetFactory()
{
}

bool IcdWidgetFactory::initialize(const QStringList &arguments, QString *errorString)
{
    return true;
}

bool IcdWidgetFactory::extensionInitialized()
{
    return true;
}

bool IcdWidgetFactory::isInitialized() const
{
    return true;
}

QStringList IcdWidgetFactory::providedWidgets() const
{
    return QStringList() << "icd" << "icdcollection" << "icdselection";
}

Form::IFormWidget *IcdWidgetFactory::createWidget(const QString &name, Form::FormItem *object, QWidget *parent)
{
    return new IcdFormWidget(object, parent);
}


IcdFormWidget::IcdFormWidget(Form::FormItem *formItem, QWidget *parent)
        : Form::IFormWidget(formItem, parent), m_CentralWidget(0)
{
    // Prepare Widget Layout and label
    QBoxLayout * hb = getBoxLayout(Label_OnTop, m_FormItem->spec()->label(), this);
    hb->setSpacing(0);
    hb->setMargin(0);

    // Add QLabel
    hb->addWidget(m_Label);

    // Get options
//    if (formItem->extraDatas().value("options").contains(OPTION_WITHPRESCRIBING, Qt::CaseInsensitive)) {
//        m_WithPrescribing = true;
//    } else if (name=="drugselector") {
//        m_WithPrescribing = false;
//    } else if (name=="prescription" || name=="prescriptor") {
//        m_WithPrescribing = true;
//    }
    // create main widget
    m_CentralWidget = new IcdCentralWidget(this);

    // set selector/collection

    hb->addWidget(m_CentralWidget);

    // create formitemdata
    IcdFormData *datas = new IcdFormData(formItem);
    datas->setForm(this);
    formItem->setItemDatas(datas);
}

IcdFormWidget::~IcdFormWidget()
{
}

void IcdFormWidget::retranslate()
{
}

////////////////////////////////////////// ItemData /////////////////////////////////////////////
IcdFormData::IcdFormData(Form::FormItem *item) :
        m_FormItem(item), m_Form(0)
{}

IcdFormData::~IcdFormData()
{}

void IcdFormData::clear()
{
    m_Form->m_CentralWidget->clear();
    m_OriginalValue.clear();
}

bool IcdFormData::isModified() const
{
    return m_OriginalValue != storableData().toString();
}

bool IcdFormData::setData(const int ref, const QVariant &data, const int role)
{
//    qWarning() << "BaseCheckData::setData" << data << role;
//    if (role==Qt::EditRole || role==Qt::DisplayRole) {
//        if (data.canConvert(QVariant::Int))  { // Tristate
//            m_Check->setCheckState(Qt::CheckState(data.toInt()));
//        }
//    }
    return true;
}

QVariant IcdFormData::data(const int ref, const int role) const
{
    return QVariant();
//    return m_Check->checkState();
}

void IcdFormData::setStorableData(const QVariant &data)
{
    if (!data.isValid())
        return;
    m_Form->m_CentralWidget->readXmlCollection(data.toString());
    m_OriginalValue = data.toString();
}

QVariant IcdFormData::storableData() const
{
    return m_Form->m_CentralWidget->collectionToXml();
}
