/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main developers : Eric Maeker                                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class BaseWidgets::TreeViewFormItem
 */

#include "treeviewformitem.h"
#include "constants.h"

#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/iformitemspec.h>

#include <utils/log.h>
#include <utils/global.h>

#include <translationutils/constants.h>

#include <QTreeView>
#include <QDebug>

using namespace BaseWidgets;
using namespace Trans::ConstantTranslations;

/*! Constructor of the BaseWidgets::TreeViewFormItem class */
TreeViewFormItem::TreeViewFormItem(Form::FormItem *formItem, QWidget *parent)
    : Form::IFormWidget(formItem, parent)
{
    setObjectName("TreeViewFormItem");

    // QtUi Loaded ?
    const QString &widget = formItem->spec()->value(Form::FormItemSpec::Spec_UiWidget).toString();
    if (!widget.isEmpty()) {
        // Find widget
        QTreeView *le = formItem->parentFormMain()->formWidget()->findChild<QTreeView*>(widget);
        if (le) {
            m_Tree = le;
        } else {
            LOG_ERROR("Using the QtUiLinkage, item not found in the ui: " + formItem->uuid());
            // To avoid segfaulting create a fake combo
            m_Tree = new QTreeView(this);
        }
        // Find Label
        m_Label = Constants::findLabel(formItem);
    } else {
        // Prepare Widget Layout and label
        QBoxLayout * hb = getBoxLayout(OnLeft, m_FormItem->spec()->label(), this);
        hb->addWidget(m_Label);

        // Add List and manage size
        m_Tree = new QTreeView(this);
        m_Tree->setObjectName("Tree_" + m_FormItem->uuid());
//        m_Tree->setUniformItemSizes(true);
        m_Tree->setAlternatingRowColors(true);
        m_Tree->setSizePolicy(QSizePolicy::Expanding , QSizePolicy::Expanding);
    //        if (uniqueList)
    //            m_List->setSelectionMode(QAbstractItemView::SingleSelection);
    //        else
    //            m_List->setSelectionMode(QAbstractItemView::MultiSelection);
        hb->addWidget(m_Tree);
    }

    // Create the model using the extraData 'xmlModel', 'jsonModel'
    if (!m_FormItem->extraData().value("xmlmodel").isEmpty()) {
        // Read the XML model
    }
//    const QStringList &possibles = m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Possible);
//    m_Model = new QStringListModel(possibles, this);
//    m_Tree->setModel(m_Model);

    setFocusedWidget(m_Tree);

    // create FormItemData
    TreeViewFormItemData *data = new TreeViewFormItemData(m_FormItem, this);
    m_FormItem->setItemData(data);

    // connect(m_List->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), data, SLOT(onValueChanged()));
}

/*! Destructor of the BaseWidgets::TreeViewFormItem class */
TreeViewFormItem::~TreeViewFormItem()
{
}

void TreeViewFormItem::addWidgetToContainer(Form::IFormWidget *widget)
{
    Q_UNUSED(widget);
}

bool TreeViewFormItem::isContainer() const
{
    return false;
}

QString TreeViewFormItem::printableHtml(bool withValues) const
{
    Q_UNUSED(withValues);
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

void TreeViewFormItem::retranslate()
{
    if (m_Label)
        m_Label->setText(m_FormItem->spec()->label());
}

/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////   TreeViewFormItem   //////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
TreeViewFormItemData::TreeViewFormItemData(Form::FormItem *item, TreeViewFormItem *formWidget) :
    m_FormItem(item),
    m_Form(formWidget)
{
}

TreeViewFormItemData::~TreeViewFormItemData()
{
}

/** \brief Set the widget to the default value \sa FormItem::FormItemValue*/
void TreeViewFormItemData::clear()
{
    // Reset widget content to its default value
    // Using m_FormItem->valueReferences()->defaultValue().toString()
}

bool TreeViewFormItemData::isModified() const
{
    //    QStringList actual = storableData().toStringList();
    //    return actual != m_OriginalValue;
    return false;
}

void TreeViewFormItemData::setModified(bool modified)
{
    Q_UNUSED(modified);
    //    if (!modified)
    //        m_OriginalValue = storableData().toStringList();
}

void TreeViewFormItemData::setReadOnly(bool readOnly)
{
    Q_UNUSED(readOnly);
    // TODO: code this: readOnly feature
}

bool TreeViewFormItemData::isReadOnly() const
{
    // TODO: code this: readOnly feature
    return false;
}

bool TreeViewFormItemData::setData(const int ref, const QVariant &data, const int role)
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

QVariant TreeViewFormItemData::data(const int ref, const int role) const
{
    Q_UNUSED(ref);
    Q_UNUSED(role);
    // This member is mainly used by script and Core::IPatient to
    // access the value of the itemData.
    // Above an example:
    //    if (role==Qt::DisplayRole || role==Form::IFormItemData::PatientModelRole || role==Form::IFormItemData::PrintRole) {
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

void TreeViewFormItemData::setStorableData(const QVariant &data)
{
    Q_UNUSED(data);
    if (m_Form) {
        // Here you will get the exact value returned by storableData()
        // when an episode is activated. Use this value to populate your widget/itemData
    }
}

QVariant TreeViewFormItemData::storableData() const
{
    if (m_Form) {
        // Return the value you want to get back in setStorableData.
        // The returned value will be saved into the database 'as is'
        // And will be sent back in setStorableData (when an episode
        // will be activated)
    }
    return QVariant();
}

void TreeViewFormItemData::onValueChanged()
{
    // Execute the 'onValueChanged' item script
    Constants::executeOnValueChangedScript(m_FormItem);
    Q_EMIT dataChanged(0);
}

