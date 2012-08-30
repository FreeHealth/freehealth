/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "icdwidgetfactory.h"
#include "icdcentralwidget.h"

#include <icdplugin/icdio.h>
#include <icdplugin/icdcollectionmodel.h>

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
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);
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
    Q_UNUSED(name);
    return new IcdFormWidget(object, parent);
}


IcdFormWidget::IcdFormWidget(Form::FormItem *formItem, QWidget *parent) :
    Form::IFormWidget(formItem, parent), m_CentralWidget(0)
{
    // Prepare Widget Layout and label
    QBoxLayout * hb = getBoxLayout(Label_OnTop, m_FormItem->spec()->label(), this);
    hb->setSpacing(0);
    hb->setMargin(0);
    hb->addWidget(m_Label);

    // create main widget
    m_CentralWidget = new IcdCentralWidget(this);

    // set selector/collection
    hb->addWidget(m_CentralWidget);

    // create formitemdata
    IcdFormData *formData = new IcdFormData(formItem);
    formData->setForm(this);
    formItem->setItemData(formData);
}

IcdFormWidget::~IcdFormWidget()
{
}

QString IcdFormWidget::printableHtml(bool withValues) const
{
    if (withValues) {
        if (m_FormItem->getOptions().contains("DontPrintEmptyValues")) {
            if (m_CentralWidget->icdCollectionModel()->isEmpty())
                return QString();
        }
        IcdIO io;
        QString toPrint = io.icdCollectionToHtml(m_CentralWidget->icdCollectionModel());
        return QString("<table width=100% border=1 cellpadding=0 cellspacing=0  style=\"margin: 1em 0em 1em 0em\">"
                       "<thead>"
                       "<tr>"
                       "<td style=\"vertical-align: top; font-weight: 600; padding: 5px\">"
                       "%1"
                       "</td>"
                       "</tr>"
                       "</thead>"
                       "<tbody>"
                       "<tr>"
                       "<td style=\"vertical-align: top; padding-left:2em; padding-top:5px; padding-bottom: 5px; padding-right:2em\">"
                       "%2"
                       "</td>"
                       "</tr>"
                       "</tbody>"
                       "</table>")
                .arg(m_FormItem->spec()->label()).arg(toPrint);
    } else {
        return QString("<table width=100% border=1 cellpadding=0 cellspacing=0  style=\"margin: 1em 0em 1em 0em\">"
                       "<thead>"
                       "<tr>"
                       "<td style=\"vertical-align: top; font-weight: 600; padding: 5px\">"
                       "%1"
                       "</td>"
                       "</tr>"
                       "</thead>"
                       "<tbody>"
                       "<tr>"
                       "<td style=\"vertical-align: top; padding-left:2em; padding-top:5px; padding-bottom: 5px; padding-right:2em\">"
                       "&nbsp;<br />&nbsp;<br />&nbsp;<br />&nbsp;<br />&nbsp;<br />"
                       "&nbsp;<br />&nbsp;<br />&nbsp;<br />&nbsp;<br />&nbsp;<br />"
                       "</td>"
                       "</tr>"
                       "</tbody>"
                       "</table>")
                .arg(m_FormItem->spec()->label());
    }
    return QString();
}


void IcdFormWidget::retranslate()
{
    m_Label->setText(m_FormItem->spec()->label());
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
    Q_UNUSED(ref);
    Q_UNUSED(role);
    Q_UNUSED(data);
    // TODO: code here
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
    Q_UNUSED(ref);
    Q_UNUSED(role);
    // TODO: code here
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
