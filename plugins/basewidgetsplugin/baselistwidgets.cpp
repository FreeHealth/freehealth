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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       Guillaume DENRY <guillaume.denry@gmail.com>                       *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include <memory>
#include <vector>
#include "baselistwidgets.h"
#include "constants.h"

#include <formmanagerplugin/iformitem.h>

#include <listviewplugin/stringlistview.h>
#include <listviewplugin/stringlistmodel.h>

#include <utils/log.h>
#include <utils/global.h>

#include <translationutils/constants.h>
#include <translationutils/trans_datetime.h>
//#include <translationutils/trans_filepathxml.h>

#include <QStringList>
#include <QListWidget>
#include <QComboBox>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QSortFilterProxyModel>

using namespace BaseWidgets;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace {
static QString getPrintRoleSeparator(Form::FormItem *item)
{
    Q_ASSERT(item);
    if (!item)
        return QString::null;
    QString printSeparator = "<br/>";
    const QStringList &options = item->getOptions();
    if (options.contains("htmllinebreakseparator", Qt::CaseInsensitive))
        printSeparator = "<br/>";
    else if (options.contains("plaintextlinebreakseparator", Qt::CaseInsensitive))
        printSeparator = Utils::isRunningOnWin()?"\r\n":"\n";
    else if (options.contains("spaceseparator", Qt::CaseInsensitive))
        printSeparator = " ";
    else if (options.contains("semicolonseparator", Qt::CaseInsensitive))
        printSeparator = "; ";
    else if (options.contains("commaseparator", Qt::CaseInsensitive))
        printSeparator = ", ";
    else if (options.contains("htmlsemicolonseparator", Qt::CaseInsensitive))
        printSeparator = "&nbsp;; ";
    return printSeparator;
}
}
///////////////////////////////////////////////////////////////////////////////
/////////////////////////////   BaseList   ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
BaseList::BaseList(Form::FormItem *formItem, QWidget *parent, bool uniqueList) :
    Form::IFormWidget(formItem,parent), m_List(0)
{
    setObjectName("BaseList");
    // QtUi Loaded ?
    const QString &widget = formItem->spec()->value(Form::FormItemSpec::Spec_UiWidget).toString();
    if (!widget.isEmpty()) {
        // Find widget
        QListView *le = formItem->parentFormMain()->formWidget()->findChild<QListView*>(widget);
        if (le) {
            m_List = le;
        } else {
            LOG_ERROR("Using the QtUiLinkage, item not found in the ui: " + formItem->uuid());
            // To avoid segfaulting create a fake QListView
            m_List = new QListView(this);
        }
        // Find Label
        m_Label = Constants::findLabel(formItem);
    } else {
        // Prepare Widget Layout and label
        QBoxLayout * hb = getBoxLayout(OnLeft, m_FormItem->spec()->label(), this);
        hb->addWidget(m_Label);

        // Add List and manage size
        m_List = new QListView(this);
        m_List->setObjectName("List_" + m_FormItem->uuid());
        m_List->setUniformItemSizes(true);
        m_List->setAlternatingRowColors(true);
        m_List->setSizePolicy(QSizePolicy::Expanding , QSizePolicy::Expanding);
        hb->addWidget(m_List);
    }

    if (uniqueList)
        m_List->setSelectionMode(QAbstractItemView::SingleSelection);
    else
        m_List->setSelectionMode(QAbstractItemView::MultiSelection);

    const QStringList &possibles = m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Possible);
    m_Model = new QStringListModel(possibles, this);
    m_List->setModel(m_Model);
    m_List->setEditTriggers(QAbstractItemView::NoEditTriggers);

    if (formItem->getOptions().contains("resizeToContent", Qt::CaseInsensitive)) {
        int itemHeight = m_List->sizeHintForRow(0);
        m_List->setMinimumSize(m_List->minimumWidth(), itemHeight * (m_Model->rowCount() + 1));
        m_List->setMaximumSize(m_List->maximumWidth(), itemHeight * (m_Model->rowCount() + 1));
        m_List->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
    setFocusedWidget(m_List);

    // create FormItemData
    BaseListData *data = new BaseListData(m_FormItem);
    data->setBaseList(this);
    m_FormItem->setItemData(data);

    connect(m_List->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), data, SLOT(onValueChanged()));
}

BaseList::~BaseList()
{
}

QString BaseList::printableHtml(bool withValues) const
{
    if (m_FormItem->getOptions().contains(Constants::NOT_PRINTABLE))
        return QString();

    QString content;
    if (!withValues) {
        foreach(const QString &v, m_Model->stringList()) {
            content += "<li>" + v + "</li>";
        }
    } else {
        QModelIndexList indexes = m_List->selectionModel()->selectedIndexes();
        if (Constants::dontPrintEmptyValues(m_FormItem) && indexes.isEmpty())
            return QString();
        qSort(indexes);
        foreach(const QModelIndex &i, indexes) {
            content += "<li>" + i.data().toString() + "</li>";
        }
    }
    if (!content.isEmpty()) {
        content.prepend("<ul>");
        content.append("</ul>");
    }
    if (m_FormItem->getOptions().contains(Constants::DONTPRINTLABEL, Qt::CaseInsensitive)) {
        return content;
    } else {
        return QString("<table width=100% border=1 cellpadding=0 cellspacing=0  style=\"margin: 5px 0px 0px 0px\">"
                   "<thead>"
                   "<tr>"
                   "<td style=\"vertical-align: top; font-weight: bold; padding: 5px\">"
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
            .arg(m_FormItem->spec()->label()).arg(content);
    }
}

void BaseList::retranslate()
{
    if (m_Label)
        m_Label->setText(m_FormItem->spec()->label());
    if (m_List) {
        const QStringList &list = m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Possible);
        if (list.count() != m_Model->rowCount()) {
            Utils::warningMessageBox(
                    tr("Wrong form's translations"),
                    tr("You asked to change the language of the form to %1.\n"
                       "But this an error while reading translation of %2.\n"
                       "Number of items of the translation (%3) are wrong.")
                    .arg(QLocale().name(), m_FormItem->spec()->label()).arg(list.count()));
            return;
        }
        // keep selection
        QModelIndexList indexes = m_List->selectionModel()->selectedIndexes();
        m_Model->setStringList(list);
        foreach(const QModelIndex &i, indexes) {
            m_List->selectionModel()->select(i, QItemSelectionModel::Select);
        }
        m_List->setToolTip(m_FormItem->spec()->tooltip());
    }
}


///////////////////////////////////////////////////////////////////////////////
//////////////////////   BaseEditableStringList   /////////////////////////////
///////////////////////////////////////////////////////////////////////////////
BaseEditableStringList::BaseEditableStringList(Form::FormItem *formItem, QWidget *parent) :
    Form::IFormWidget(formItem,parent),
    m_StringListView(0)
{
    setObjectName("BaseEditableStringList");
    QLayout *layout = 0;
    // QtUi Loaded ?
    const QString &layoutName = formItem->spec()->value(Form::FormItemSpec::Spec_UiInsertIntoLayout).toString();
    if (!layoutName.isEmpty()) {
        // Find widget
        layout = formItem->parentFormMain()->formWidget()->findChild<QLayout*>(layoutName);
        if (!layout) {
            LOG_ERROR("QtUi: Layout not found: " + layoutName);
            layout = new QHBoxLayout(this);
            setLayout(layout);
        }
        m_Label = Constants::findLabel(formItem);
    } else {
        // Prepare Widget Layout and label
        QBoxLayout *hb = getBoxLayout(OnLeft, m_FormItem->spec()->label(), this);
        hb->addWidget(m_Label);
        layout = hb;
    }

    // Add List and manage size
    m_StringListView = new Views::StringListView(this);
    m_StringListView->setObjectName("StringListView_" + m_FormItem->uuid());
    m_StringListView->setAlternatingRowColors(true);
    m_StringListView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    Views::StringListModel *model = new Views::StringListModel(this, true, false);
    m_StringListView->setModel(model);
    // Get options
    if (m_FormItem->extraData().keys().contains("maxrow")) {
        model->setStringList(QStringList() << "sizeme");
        int max = m_FormItem->extraData().value("maxrow").toInt();
        m_StringListView->setMaximumRows(max);
        // resize the itemview
        int rowHeight = m_StringListView->itemView()->sizeHintForRow(0);
        m_StringListView->itemView()->setMaximumHeight(rowHeight * (max + 0.5));
        model->setStringList(QStringList());
    }

    layout->addWidget(m_StringListView);

    setFocusedWidget(m_StringListView);

    // create FormItemData
    BaseListData *data = new BaseListData(m_FormItem);
    data->setBaseStringListView(this);
    m_FormItem->setItemData(data);
}

BaseEditableStringList::~BaseEditableStringList()
{
}

QString BaseEditableStringList::printableHtml(bool withValues) const
{
    if (m_FormItem->getOptions().contains(Constants::NOT_PRINTABLE, Qt::CaseInsensitive))
        return QString();

    if (withValues) {
        QString content;
        content += "<ul>";
        content += "<li> " + m_StringListView->getStringList().toStringList().join("</li><li>") + "</li>";
        content += "</ul>";
        return content;
    }
    return QString::null;
}

void BaseEditableStringList::retranslate()
{
    if (m_Label)
        m_Label->setText(m_FormItem->spec()->label());
}


//////////////////////////////////// ItemData /////////////////////////////////////////////
BaseListData::BaseListData(Form::FormItem *item) :
    m_FormItem(item),
    m_List(0),
    m_EditableList(0)
{
}

BaseListData::~BaseListData()
{
}

void BaseListData::setSelectedItems(const QString &s)
{
    QItemSelectionModel *selModel = 0;
    if (m_List)
        selModel = m_List->m_List->selectionModel();
    else if (m_EditableList && m_EditableList->m_StringListView)
        selModel = m_EditableList->m_StringListView->selectionModel();
    if (selModel)
        selModel->clearSelection();
    if (s.isEmpty())
        return;

    if (m_List) {
        const QStringList &uuids = m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Uuid);
        if (s.contains("`@`")) {
            // multilist
            foreach(const QString &i, s.split("`@`", QString::SkipEmptyParts)) {
                int row = uuids.lastIndexOf(i);
                QModelIndex idx = m_List->m_Model->index(row, 0);
                selModel->select(idx, QItemSelectionModel::Select);
            }
        } else {
            int row = uuids.lastIndexOf(s);
            QModelIndex idx = m_List->m_Model->index(row, 0);
            selModel->select(idx, QItemSelectionModel::Select);
        }
    } else if (m_EditableList) {
        m_EditableList->m_StringListView->setStringList(s.split("`@`", QString::SkipEmptyParts));
    }
    onValueChanged();
}

/** \brief Set the widget to the default value \sa FormItem::FormItemValue*/
void BaseListData::clear()
{
    setSelectedItems(m_FormItem->valueReferences()->defaultValue().toString());
}

bool BaseListData::isModified() const
{
    QStringList actual = storableData().toStringList();
    return actual != m_OriginalValue;
}

void BaseListData::setModified(bool modified)
{
    if (!modified)
        m_OriginalValue = storableData().toStringList();
}

void BaseListData::setReadOnly(bool readOnly)
{
    // TODO: improve this readonly feature
    if (m_List && m_List->m_List)
        m_List->m_List->setEnabled(!readOnly);
    if (m_EditableList && m_EditableList->m_StringListView)
        m_EditableList->m_StringListView->setEnabled(!readOnly);
}

bool BaseListData::isReadOnly() const
{
    if (m_List->m_List)
        return (!m_List->m_List->isEnabled());
    if (m_EditableList && m_EditableList->m_StringListView)
        return (!m_EditableList->m_StringListView->isEnabled());
    return false;
}

bool BaseListData::setData(const int ref, const QVariant &data, const int role)
{
    Q_UNUSED(ref);
    if (role!=Qt::EditRole) {
        setSelectedItems(data.toStringList().join("`@`"));
    }
    return true;
}

QVariant BaseListData::data(const int ref, const int role) const
{
    if (role==Qt::DisplayRole
            || role==Form::IFormItemData::PatientModelRole
            || role==Form::IFormItemData::PrintRole
            || role==Form::IFormItemData::CalculationsRole)  {

        // Get print separator
        QString printSeparator;
        if (role==Form::IFormItemData::PrintRole) {
            printSeparator = getPrintRoleSeparator(m_FormItem);
        }

        if (m_List) {
            QStringList selected;
            QItemSelectionModel *selModel = m_List->m_List->selectionModel();
            if (!selModel->hasSelection())
                return QVariant();
            if (ref==Form::IFormItemData::ID_CurrentUuid) {
                const QStringList &uuids = m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Uuid);
                foreach(const QModelIndex &idx, selModel->selectedIndexes()) {
                    selected.append(uuids.at(idx.row()));
                }
            } else {
                foreach(const QModelIndex &idx, selModel->selectedIndexes()) {
                    selected.append(idx.data().toString());
                }
            }
            if (role==Form::IFormItemData::PrintRole)
                return selected.join(printSeparator);
            return selected;
        } else if (m_EditableList) {
            if (role==Form::IFormItemData::PrintRole)
                return  m_EditableList->m_StringListView->getStringList().toStringList().join(printSeparator);
            return m_EditableList->m_StringListView->getStringList();
        }
    }
    // TODO: code something like this:
//    else if (role==Qt::CheckStateRole) {
//        if (m_EditableList) {
//            return m_EditableList->m_StringListView->getCheckedStringList();
//        }
//    }
    return QVariant();
}

void BaseListData::setStorableData(const QVariant &data)
{
    setSelectedItems(data.toString());
    m_OriginalValue = data.toStringList();
    qSort(m_OriginalValue);
}

/** Storable data of a List is the uuids of the selected items sorted in alphabetic order. */
QVariant BaseListData::storableData() const
{
    if (m_List) {
        QItemSelectionModel *selModel = m_List->m_List->selectionModel();

        if (!selModel->hasSelection())
            return QVariant();

        QStringList selected;
        const QStringList &uuids = m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Uuid);
        foreach(const QModelIndex &idx, selModel->selectedIndexes()) {
            selected.append(uuids.at(idx.row()));
        }
        qSort(selected);
        return selected.join("`@`");
    } else if (m_EditableList) {
        return m_EditableList->m_StringListView->getStringList().toStringList().join("`@`");
    }
    return QVariant();
}

void BaseListData::onValueChanged()
{
    Constants::executeOnValueChangedScript(m_FormItem);
    Q_EMIT dataChanged(0);
}

///////////////////////////////////////////////////////////////////////////////
////////////////////////////   BaseCombo   ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
BaseCombo::BaseCombo(Form::FormItem *formItem, QWidget *parent) :
    Form::IFormWidget(formItem,parent), m_Combo(0), m_Sort(false),
    m_Default(false)
{
    setObjectName("BaseCombo");

    // create FormItemData
    BaseComboData *data = new BaseComboData(m_FormItem);
    data->setBaseCombo(this);

    // QtUi Loaded ?
    const QString &widget = formItem->spec()->value(Form::FormItemSpec::Spec_UiWidget).toString();
    if (!widget.isEmpty()) {
        // Find widget
        m_Combo = formItem->parentFormMain()->formWidget()->findChild<QComboBox*>(widget);
        if (!m_Combo) {
            LOG_ERROR("Using the QtUiLinkage, item not found in the ui: " + formItem->uuid());
            // To avoid segfaulting create a fake combo
            m_Combo = new QComboBox(this);
        }
        // Find label
        m_Label = Constants::findLabel(formItem);
    } else {
        // Prepare Widget Layout and label
        QBoxLayout *hb = getBoxLayout(OnLeft, m_FormItem->spec()->label(), this);
        hb->addWidget(m_Label);

        // Add List and manage size
        m_Combo = new QComboBox(this);
        m_Combo->setObjectName("Combo_" + m_FormItem->uuid());
        hb->addWidget(m_Combo);
    }

    // Manage options
    data->m_DefaultValue= m_FormItem->extraData().value("default");
    if (!data->m_DefaultValue.isEmpty())
        m_Default = true;

    if (m_FormItem->getOptions().contains("PopulateWithPeriods", Qt::CaseInsensitive)) {
        data->populateWithPeriods();
     } else {
        data->setVectors();
        // define default index

        if (m_Default) {
            data->m_DefaultIndex = data->comboIndexFromUuid(data->m_DefaultValue);
        } else {
            data->m_DefaultIndex=-1;
        }
    }

    //set model


    // sort?
    m_Sort= m_FormItem->getOptions().contains("Sort", Qt::CaseInsensitive);
    if (m_Sort) {
        data->m_Proxy = new QSortFilterProxyModel(m_Combo);
        data->setModel();
        data->m_Proxy->setSourceModel(data->m_Model);
        m_Combo->setModel(data->m_Proxy);
        m_Combo->model()->sort(0);
        int idx = data->comboIndexFromUuid(data->m_DefaultValue);
        data->setDefaultIndex(idx);
    } else {
        data->setModel();
        m_Combo->setModel(data->m_Model);
        int idx = m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Uuid).indexOf(data->m_DefaultValue);
        data->setDefaultIndex(idx);
    }
    setFocusedWidget(m_Combo);

    data->clear();
    m_FormItem->setItemData(data);

    connect(m_Combo, SIGNAL(currentIndexChanged(int)), data, SLOT(onValueChanged()));
}

BaseCombo::~BaseCombo()
{
}

QString BaseCombo::printableHtml(bool withValues) const
{
    if (m_FormItem->getOptions().contains(Constants::NOT_PRINTABLE))
        return QString();

    QString content;
    if (!withValues) {
        for(int i = 0; i < m_Combo->count(); ++i) {
            content += "<li>" + m_Combo->itemData(i).toString() + "</li>";
        }
    } else {
        if (m_Combo->currentIndex()==-1)
            return QString();
        content += "<li>" + m_Combo->currentText() + "</li>";
    }
    if (!content.isEmpty()) {
        content.prepend("<ul>");
        content.append("</ul>");
    }
    return content;
}

void BaseCombo::retranslate()
{
    if (m_Label)
        m_Label->setText(m_FormItem->spec()->label());
    if (m_Combo) {
        QStringList list = m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Possible);
        if (list.count() != m_Combo->count()) {
            Utils::warningMessageBox(
                    tr("Wrong form's translations"),
                    tr("You asked to change the language of the form to %1.\n"
                       "But this an error while reading translation of %2.\n"
                       "Number of items of the translation (%3) are wrong.")
                    .arg(QLocale().name(), m_FormItem->spec()->label()).arg(list.count()));
            return;
        }
        //sort?
        if (m_Sort)
            list.sort();
        // refresh combo items
        int id = m_Combo->currentIndex();
        m_Combo->clear();
        m_Combo->addItems(list);
        m_Combo->setCurrentIndex(id);
        m_Combo->setToolTip(m_FormItem->spec()->tooltip());
    }
}

////////////////////////////////////////// ItemData /////////////////////////////////////////////
BaseComboData::BaseComboData(Form::FormItem *item) :
    m_FormItem(item),
    m_BaseCombo(0),
    m_OriginalValue(-1),
    m_DefaultIndex(-1),
    m_Model(0)
{
    m_Model = new QStandardItemModel(this);
    m_Model->setColumnCount(2);
}

BaseComboData::~BaseComboData()
{
}

int BaseComboData::selectedItem(const QString &s)
{
    m_BaseCombo->m_Combo->setCurrentIndex(-1);
    int row;
    if (s.isEmpty())
        return -1;
    if (!m_BaseCombo->m_Sort) {
        const QStringList &uuids = m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Uuid);
        row = uuids.lastIndexOf(s);
    } else {
        row = comboIndexFromUuid(s);
    }
    m_BaseCombo->m_Combo->setCurrentIndex(row);
    return row;
}

void BaseComboData::setDefaultIndex(int index)
{
    if (!m_DefaultIndex==-1)
        return;
    m_DefaultIndex = index;
}

int BaseComboData::defaultIndex() const
{
    return m_DefaultIndex;
}

/** \brief Set the widget to the default value \sa FormItem::FormItemValue*/
void BaseComboData::clear()
{
    m_OriginalValue = -1;
    if (m_BaseCombo->m_Combo)
        m_BaseCombo->m_Combo->setCurrentIndex(m_DefaultIndex);
}

bool BaseComboData::isModified() const
{
    return m_OriginalValue != m_BaseCombo->m_Combo->currentIndex();
}

void BaseComboData::setModified(bool modified)
{
    if (!modified)
        m_OriginalValue = m_BaseCombo->m_Combo->currentIndex();
}

void BaseComboData::setReadOnly(bool readOnly)
{
    m_BaseCombo->m_Combo->setEnabled(!readOnly);
}

bool BaseComboData::isReadOnly() const
{
    return m_BaseCombo->m_Combo->isEnabled();
}

bool BaseComboData::setData(const int ref, const QVariant &data, const int role)
{
    if (role!=Qt::EditRole)
        return false;
    if (ref==Form::IFormItemData::ID_CurrentUuid) {
        //int id = parentItem()->valueReferences()->values(Form::FormItemValues::Value_Uuid).indexOf(data.toString());
        QModelIndex start = m_Model->index(0, 1, QModelIndex());
        QModelIndexList list = m_Model->match(start,
                                       Qt::DisplayRole,
                                       data,
                                       1,
                                       Qt::MatchExactly);
        QModelIndex index = list.takeFirst();
        int id = index.row();
        m_BaseCombo->m_Combo->setCurrentIndex(id);
        onValueChanged();
    }
    return true;
}

/*!
    \brief return data of a the combo FormItem

    Return data for each role. CalculationsRole return numerical values. Used by
    scriptwrappers.cpp
    \sa FormItemScriptWrapper::currentValue()
*/
QVariant BaseComboData::data(const int ref, const int role) const
{
    if (role==Form::IFormItemData::CalculationsRole) {
        QString data = m_BaseCombo->m_Combo->currentText();
        QModelIndex start = m_Model->index(0, 0, QModelIndex());
        QModelIndexList list = m_Model->match(start,
                                       Qt::DisplayRole,
                                       data,
                                       1,
                                       Qt::MatchExactly);
        if (!list.empty()) {
        QModelIndex index = list.takeFirst();
        int idx = index.row();
        const QStringList &vals = parentItem()->valueReferences()->values(Form::FormItemValues::Value_Numerical);
            if (idx < vals.count() && idx >= 0) {
                return vals.at(idx);
            }
        }
    }

    if (ref==Form::IFormItemData::ID_CurrentUuid) {
        int id = m_BaseCombo->m_Combo->currentIndex();
        if (id>=0)
            return parentItem()->valueReferences()->values(Form::FormItemValues::Value_Uuid).at(id);

    }
    if (role==Qt::DisplayRole
            || role==Form::IFormItemData::PatientModelRole
            || role==Form::IFormItemData::PrintRole)  {
        return m_BaseCombo->m_Combo->currentText();
    }

    return QVariant();
}

void BaseComboData::setStorableData(const QVariant &data)
{
    m_OriginalValue = selectedItem(data.toString());
}

QVariant BaseComboData::storableData() const
{
    int uuidrow = m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Possible).indexOf(m_BaseCombo->m_Combo->currentText());
    if (uuidrow < 0 || uuidrow >= m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Uuid).count())
        return QVariant();
    return m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Uuid).at(uuidrow);
}

void BaseComboData::onValueChanged()
{
//    WARN_FUNC;
    Constants::executeOnValueChangedScript(m_FormItem);
    Q_EMIT dataChanged(Form::IFormItemData::ID_CurrentUuid);
}

int BaseComboData::comboIndexFromUuid(const QString s)
{
    QModelIndex start = m_BaseCombo->m_Combo->model()->index(0,1,QModelIndex());
    QModelIndexList list = m_BaseCombo->m_Combo->model()->match(start,
                                   Qt::DisplayRole,
                                   s,
                                   1,
                                   Qt::MatchExactly);
    if (!list.isEmpty()) {
        QModelIndex index = list.takeFirst();
        return index.row();
    }
    return -1;
}

void BaseComboData::setVectors()
{
    if (!m_v_Possibles.empty())
        m_v_Possibles.clear();
    if (!m_v_Uuids.empty())
        m_v_Uuids.clear();

    const QStringList possibles = parentItem()->valueReferences()->values(Form::FormItemValues::Value_Possible);
    const QStringList uuids = parentItem()->valueReferences()->values(Form::FormItemValues::Value_Uuid);

    if (!possibles.count()==uuids.count()) {
        Utils::warningMessageBox(
                    tr("Possibles - Uuids mismatch"),
                    tr("There are %1 different possibles values.\n"
                       "There are %2 different uuids values.\n"
                       "Please correct your form and try again.")
                    .arg(possibles.count()).arg(uuids.count()));
        return;
    }

    QString p;
    foreach (p, possibles) {
        m_v_Possibles.push_back(std::unique_ptr<QStandardItem> (new QStandardItem(p)));
    }

    QString u;
    foreach (u, uuids) {
        m_v_Uuids.push_back(std::unique_ptr<QStandardItem> (new QStandardItem(u)));
    }
}

void BaseComboData::setModel()
{
    if (m_Model)
        m_Model->clear();

    unsigned int i;
    for (i = 0; i < m_v_Possibles.size(); i++) {
            m_Model->setItem(i,0,m_v_Possibles.at(i).get());
    }
    for (i = 0; i < m_v_Uuids.size(); i++) {
            m_Model->setItem(i,1,m_v_Uuids.at(i).get());
    }
}

void BaseComboData::populateWithPeriods()
{
    Form::FormItemValues *vals = m_FormItem->valueReferences();
    std::vector<std::unique_ptr<QStandardItem>> v;

    QStringList period_uids = QStringList();
    period_uids << "second" << "minute" << "hour" << "day" << "week" << "month"
               << "quarter" << "year" << "decade";

    QStringList period_constants = QStringList();
    period_constants << tkTr(Trans::Constants::SECOND_S)
                     << tkTr(Trans::Constants::MINUTE_S)
                     << tkTr(Trans::Constants::HOUR_S)
                     << tkTr(Trans::Constants::DAY_S)
                     << tkTr(Trans::Constants::WEEK_S)
                     << tkTr(Trans::Constants::MONTH_S)
                     << tkTr(Trans::Constants::QUARTER_S)
                     << tkTr(Trans::Constants::YEAR_S)
                     << tkTr(Trans::Constants::DECADE_S);

    for (int i = 0; i < period_uids.count(); i++)
    {
        QString uid = period_uids.at(i);
        QString p = period_constants.at(i);
        vals->setValue(Form::FormItemValues::Value_Uuid, i, uid);
        vals->setValue(Form::FormItemValues::Value_Possible, i, p);
        m_v_Possibles.push_back(std::unique_ptr<QStandardItem> (new QStandardItem(p)));
        m_v_Uuids.push_back(std::unique_ptr<QStandardItem> (new QStandardItem(uid)));
        if (m_DefaultValue.compare(uid, Qt::CaseInsensitive)==0)
            setDefaultIndex(i);
    }
}
