/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main developers : %AuthorName%                                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
@if "%Doxygen%" == "true"
/*!
@if "%Internal%" == "true"
 * \class %PluginNamespace:c%::Internal::%ClassName:c%
@else
 * \class %PluginNamespace:c%::%ClassName:c%
@endif
 * \brief short description of class
 *
 * Long description of class
 * \sa %PluginNamespace:c%::
 */
@endif

#include "%ClassName:l%.h"
#include "constants.h"

#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/iformitemspec.h>

#include <utils/log.h>
#include <utils/global.h>

@if "%Translations%" == "true"
#include <translationutils/constants.h>

@endif
#include <QDebug>

using namespace %PluginNamespace:c%;
@if "%Internal%" == "true"
using namespace Internal;
@endif
@if "%Translations%" == "true"
using namespace Trans::ConstantTranslations;
@endif

@if "%Doxygen%" == "true"
@if "%Internal%" == "true"
/*! Constructor of the %PluginNamespace%::Internal::%ClassName:c% class */
@else
/*! Constructor of the %PluginNamespace%::%ClassName:c% class */
@endif
@endif
%ClassName:c%::%ClassName:c%(Form::FormItem *formItem, QWidget *parent)
  : Form::IFormWidget(formItem, parent)
{
    setObjectName("%ClassName:c%");

    // If you are creating a BaseWidget, edit the basewidgets.cpp
    // Add your type in the Type_Enum and at the same Index of the list
    // add the name of your widget in the static const QStringList widgetsName.
    // Then edit the BaseWidgetsFactory::createWidget(...)
    // Add your case in the switch, for example:
    // case ::Type_%ClassName:c% : return new %ClassName:c%(formItem,parent);



//    // QtUi Loaded ?
//    const QString &widget = formItem->spec()->value(Form::FormItemSpec::Spec_UiWidget).toString();
//    if (!widget.isEmpty()) {
//        // Find widget
//        QListView *le = formItem->parentFormMain()->formWidget()->findChild<QListView*>(widget);
//        if (le) {
//            m_List = le;
//        } else {
//            LOG_ERROR("Using the QtUiLinkage, item not found in the ui: " + formItem->uuid());
//            // To avoid segfaulting create a fake combo
//            m_List = new QListView(this);
//        }
//        // Find Label
//        m_Label = Constants::findLabel(formItem);
//    } else {
//        // Prepare Widget Layout and label
//        QBoxLayout * hb = getBoxLayout(Label_OnLeft, m_FormItem->spec()->label(), this);
//        hb->addWidget(m_Label);

//        // Add List and manage size
//        m_List = new QListView(this);
//        m_List->setObjectName("List_" + m_FormItem->uuid());
//        m_List->setUniformItemSizes(true);
//        m_List->setAlternatingRowColors(true);
//        m_List->setSizePolicy(QSizePolicy::Expanding , QSizePolicy::Expanding);
//        if (uniqueList)
//            m_List->setSelectionMode(QAbstractItemView::SingleSelection);
//        else
//            m_List->setSelectionMode(QAbstractItemView::MultiSelection);

//        const QStringList &possibles = m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Possible);
//        m_Model = new QStringListModel(possibles, this);
//        m_List->setModel(m_Model);

//        hb->addWidget(m_List);
//    }

//    setFocusedWidget(m_List);

//    // create FormItemData
//    %ClassName:c%Data *data = new %ClassName:c%Data(m_FormItem, %ClassName:c%);
//    m_FormItem->setItemData(data);

    // Connect the widget content modification to the itemdata SLOT(onValueChanged())
//    connect(..., SIGNAL(...), data, SLOT(onValueChanged()));
}

@if "%Doxygen%" == "true"
@if "%Internal%" == "true"
/*! Destructor of the %PluginNamespace%::Internal::%ClassName:c% class */
@else
/*! Destructor of the %PluginNamespace%::%ClassName:c% class */
@endif
@endif
%ClassName:c%::~%ClassName:c%()
{
}

void %ClassName:c%::addWidgetToContainer(Form::IFormWidget *widget)
{
    Q_UNUSED(widget);
}

bool %ClassName:c%::isContainer() const
{
    return false;
}

QString %ClassName:c%::printableHtml(bool withValues) const
{
//    if (m_FormItem->getOptions().contains(Constants::NOT_PRINTABLE))
//        return QString();
//    QString content;
//    if (!withValues) {
//        foreach(const QString &v, m_Model->stringList()) {
//            content += "<li>" + v + "</li>";
//        }
//    } else {
//        QModelIndexList indexes = m_List->selectionModel()->selectedIndexes();
//        if (Constants::dontPrintEmptyValues(m_FormItem) && indexes.isEmpty())
//            return QString();
//        qSort(indexes);
//        foreach(const QModelIndex &i, indexes) {
//            content += "<li>" + i.data().toString() + "</li>";
//        }
//    }
//    if (!content.isEmpty()) {
//        content.prepend("<ul>");
//        content.append("</ul>");
//    }
//    return content;
    return QString::null;
}

void %ClassName:c%::retranslate()
{
    if (m_Label)
        m_Label->setText(m_FormItem->spec()->label());
}

/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////   %ClassName:c%   //////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
%ClassName:c%Data::%ClassName:c%Data(Form::FormItem *item, %ClassName:c% *formWidget) :
    m_FormItem(item),
    m_Form(formWidget)
{
}

%ClassName:c%Data::~%ClassName:c%Data()
{
}

/** \brief Set the widget to the default value \sa FormItem::FormItemValue*/
void %ClassName:c%Data::clear()
{
    // Reset widget content to its default value
    // Using m_FormItem->valueReferences()->defaultValue().toString()
}

bool %ClassName:c%Data::isModified() const
{
//    QStringList actual = storableData().toStringList();
//    return actual != m_OriginalValue;
    return false;
}

void %ClassName:c%Data::setModified(bool modified)
{
//    if (!modified)
//        m_OriginalValue = storableData().toStringList();
}

void %ClassName:c%Data::setReadOnly(bool readOnly)
{}

bool %ClassName:c%Data::isReadOnly() const
{}

bool %ClassName:c%Data::setData(const int ref, const QVariant &data, const int role)
{
    Q_UNUSED(ref);
    Q_UNUSED(data);
    Q_UNUSED(role);
    // This member is mainly used by script and Core::IPatient to
    // set the value of the itemData.
//    if (role!=Qt::EditRole) {
//        setSelectedItems(data.toStringList().join("`@`"));
//    }
    return true;
}

QVariant %ClassName:c%Data::data(const int ref, const int role) const
{
    // This member is mainly used by script and Core::IPatient to
    // access the value of the itemData.
    // Above an example:
//    if (role==Qt::DisplayRole || role==Form::IFormItemData::PatientModelRole) {
//        if (m_List) {
//            QStringList selected;
//            QItemSelectionModel *selModel = m_List->m_List->selectionModel();
//            if (!selModel->hasSelection())
//                return QVariant();
//            if (ref==Form::IFormItemData::ID_CurrentUuid) {
//                const QStringList &uuids = m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Uuid);
//                foreach(const QModelIndex &idx, selModel->selectedIndexes()) {
//                    selected.append(uuids.at(idx.row()));
//                }
//            } else {
//                foreach(const QModelIndex &idx, selModel->selectedIndexes()) {
//                    selected.append(idx.data().toString());
//                }
//            }
//            return selected;
//        } else if (m_EditableList) {
//            return m_EditableList->m_StringListView->getStringList();
//        }
//    }
    return QVariant();
}

void %ClassName:c%Data::setStorableData(const QVariant &data)
{
    if (m_Form) {
        // Here you will get the exact value returned by storableData()
        // when an episode is activated. Use this value to populate your widget/itemData
    }
}

QVariant %ClassName:c%Data::storableData() const
{
    if (m_Form) {
        // Return the value you want to get back in setStorableData.
        // The returned value will be saved into the database 'as is'
        // And will be sent back in setStorableData (when an episode
        // will be activated)
    }
    return QVariant();
}

void %ClassName:c%Data::onValueChanged()
{
    // Execute the 'onValueChanged' item script
    Constants::executeOnValueChangedScript(m_FormItem);
    Q_EMIT dataChanged(0);
}
