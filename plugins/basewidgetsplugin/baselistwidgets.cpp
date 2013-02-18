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
 *       Guillaume DENRY <guillaume.denry@gmail.com>                       *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
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

using namespace BaseWidgets;
using namespace Internal;
using namespace Trans::ConstantTranslations;

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
            // To avoid segfaulting create a fake combo
            m_List = new QListView(this);
        }
        // Find Label
        m_Label = Constants::findLabel(formItem);
    } else {
        // Prepare Widget Layout and label
        QBoxLayout * hb = getBoxLayout(Label_OnLeft, m_FormItem->spec()->label(), this);
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
    setFocusableWidget(m_List);

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
    return content;
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
        QBoxLayout *hb = getBoxLayout(Label_OnLeft, m_FormItem->spec()->label(), this);
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

    setFocusableWidget(m_StringListView);

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
    if (m_FormItem->getOptions().contains(Constants::NOT_PRINTABLE))
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


////////////////////////////////////////// ItemData /////////////////////////////////////////////
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
    if (role==Qt::DisplayRole || role==Form::IFormItemData::PatientModelRole || role==Form::IFormItemData::PrintRole)  {
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
            return selected;
        } else if (m_EditableList) {
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
    Form::IFormWidget(formItem,parent), m_Combo(0)
{
    setObjectName("BaseCombo");
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
        QBoxLayout *hb = getBoxLayout(Label_OnLeft, m_FormItem->spec()->label(), this);
        hb->addWidget(m_Label);

        // Add List and manage size
        m_Combo = new QComboBox(this);
        m_Combo->setObjectName("Combo_" + m_FormItem->uuid());
        hb->addWidget(m_Combo);
    }
    m_Combo->addItems(m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Possible));

    setFocusableWidget(m_Combo);

    // Manage options
    int defaultId = -1;
    if (m_FormItem->getOptions().contains("PopulateWithPeriods")) {
        Form::FormItemValues *vals = m_FormItem->valueReferences();
        QString defaultVal = m_FormItem->extraData().value("default");
        int i = 0;
        QString uid = "second";
        QString p = tkTr(Trans::Constants::SECOND_S);
        vals->setValue(Form::FormItemValues::Value_Uuid, i, uid);
        vals->setValue(Form::FormItemValues::Value_Possible, i, p);
        m_Combo->addItem(p);
        if (defaultVal.compare(uid, Qt::CaseInsensitive)==0)
            defaultId = i;
        ++i;
        uid = "minute";
        p = tkTr(Trans::Constants::MINUTE_S);
        vals->setValue(Form::FormItemValues::Value_Uuid, i, uid);
        vals->setValue(Form::FormItemValues::Value_Possible, i, p);
        m_Combo->addItem(p);
        if (defaultVal.compare(uid, Qt::CaseInsensitive)==0)
            defaultId = i;
        ++i;
        uid = "hour";
        p = tkTr(Trans::Constants::HOUR_S);
        vals->setValue(Form::FormItemValues::Value_Uuid, i, uid);
        vals->setValue(Form::FormItemValues::Value_Possible, i, p);
        m_Combo->addItem(p);
        if (defaultVal.compare(uid, Qt::CaseInsensitive)==0)
            defaultId = i;
        ++i;
        uid = "day";
        p = tkTr(Trans::Constants::DAY_S);
        vals->setValue(Form::FormItemValues::Value_Uuid, i, uid);
        vals->setValue(Form::FormItemValues::Value_Possible, i, p);
        m_Combo->addItem(p);
        if (defaultVal.compare(uid, Qt::CaseInsensitive)==0)
            defaultId = i;
        ++i;
        uid = "week";
        p = tkTr(Trans::Constants::WEEK_S);
        vals->setValue(Form::FormItemValues::Value_Uuid, i, uid);
        vals->setValue(Form::FormItemValues::Value_Possible, i, p);
        m_Combo->addItem(p);
        if (defaultVal.compare(uid, Qt::CaseInsensitive)==0)
            defaultId = i;
        ++i;
        uid = "month";
        p = tkTr(Trans::Constants::MONTH_S);
        vals->setValue(Form::FormItemValues::Value_Uuid, i, uid);
        vals->setValue(Form::FormItemValues::Value_Possible, i, p);
        m_Combo->addItem(p);
        if (defaultVal.compare(uid, Qt::CaseInsensitive)==0)
            defaultId = i;
        ++i;
        uid = "quarter";
        p = tkTr(Trans::Constants::QUARTER_S);
        vals->setValue(Form::FormItemValues::Value_Uuid, i, uid);
        vals->setValue(Form::FormItemValues::Value_Possible, i, p);
        m_Combo->addItem(p);
        if (defaultVal.compare(uid, Qt::CaseInsensitive)==0)
            defaultId = i;
        ++i;
        uid = "year";
        p = tkTr(Trans::Constants::YEAR_S);
        vals->setValue(Form::FormItemValues::Value_Uuid, i, uid);
        vals->setValue(Form::FormItemValues::Value_Possible, i, p);
        m_Combo->addItem(p);
        if (defaultVal.compare(uid, Qt::CaseInsensitive)==0)
            defaultId = i;
        ++i;
        uid = "decade";
        p = tkTr(Trans::Constants::DECADE_S);
        vals->setValue(Form::FormItemValues::Value_Uuid, i, uid);
        vals->setValue(Form::FormItemValues::Value_Possible, i, p);
        m_Combo->addItem(p);
        if (defaultVal.compare(uid, Qt::CaseInsensitive)==0)
            defaultId = i;
    }

    // create FormItemData
    BaseComboData *data = new BaseComboData(m_FormItem);
    data->setBaseCombo(this);
    data->setDefaultIndex(defaultId);
    m_FormItem->setItemData(data);
    data->clear();

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
        const QStringList &list = m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Possible);
        if (list.count() != m_Combo->count()) {
            Utils::warningMessageBox(
                    tr("Wrong form's translations"),
                    tr("You asked to change the language of the form to %1.\n"
                       "But this an error while reading translation of %2.\n"
                       "Number of items of the translation (%3) are wrong.")
                    .arg(QLocale().name(), m_FormItem->spec()->label()).arg(list.count()));
            return;
        }
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
    m_Combo(0),
    m_OriginalValue(-1),
    m_DefaultIndex(-1)
{
}

BaseComboData::~BaseComboData()
{
}

int BaseComboData::selectedItem(const QString &s)
{
    m_Combo->m_Combo->setCurrentIndex(-1);
    if (s.isEmpty())
        return -1;

    const QStringList &uuids = m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Uuid);
    int row = uuids.lastIndexOf(s);
    m_Combo->m_Combo->setCurrentIndex(row);
    return row;
}

void BaseComboData::setDefaultIndex(int index)
{
    m_DefaultIndex = index;
}

int BaseComboData::defaultIndex() const
{
    if (m_DefaultIndex != -1)
        return m_DefaultIndex;

    const QStringList &uuids = m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Uuid);
    return uuids.lastIndexOf(m_FormItem->valueReferences()->defaultValue().toString());
}

/** \brief Set the widget to the default value \sa FormItem::FormItemValue*/
void BaseComboData::clear()
{
    m_OriginalValue = -1;
    m_Combo->m_Combo->setCurrentIndex(-1);
    m_Combo->m_Combo->setCurrentIndex(defaultIndex());
}

bool BaseComboData::isModified() const
{
    return m_OriginalValue != m_Combo->m_Combo->currentIndex();
}

void BaseComboData::setModified(bool modified)
{
    if (!modified)
        m_OriginalValue = m_Combo->m_Combo->currentIndex();
}

bool BaseComboData::setData(const int ref, const QVariant &data, const int role)
{
    if (role!=Qt::EditRole)
        return false;
    if (ref==Form::IFormItemData::ID_CurrentUuid) {
        int id = parentItem()->valueReferences()->values(Form::FormItemValues::Value_Uuid).indexOf(data.toString());
        m_Combo->m_Combo->setCurrentIndex(id);
        onValueChanged();
    }
    return true;
}

QVariant BaseComboData::data(const int ref, const int role) const
{
    int id = m_Combo->m_Combo->currentIndex();
    if (ref==Form::IFormItemData::ID_CurrentUuid) {
        if (id>=0)
            return parentItem()->valueReferences()->values(Form::FormItemValues::Value_Uuid).at(id);

    }
    if (role==Qt::DisplayRole || role==Form::IFormItemData::PatientModelRole || role==Form::IFormItemData::PrintRole) {
        return m_Combo->m_Combo->currentText();
    }
    if (role==Form::IFormItemData::CalculationsRole) {
        const QStringList &vals = parentItem()->valueReferences()->values(Form::FormItemValues::Value_Numerical);
        if (id < vals.count() && id >= 0)
            return vals.at(id);
    }
    return QVariant();
}

void BaseComboData::setStorableData(const QVariant &data)
{
    m_OriginalValue = selectedItem(data.toString());
}

QVariant BaseComboData::storableData() const
{
    int row = m_Combo->m_Combo->currentIndex();
    if (row < 0 || row >= m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Uuid).count())
        return QVariant();
    return m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Uuid).at(row);
}

void BaseComboData::onValueChanged()
{
//    WARN_FUNC;
    Constants::executeOnValueChangedScript(m_FormItem);
    Q_EMIT dataChanged(Form::IFormItemData::ID_CurrentUuid);
}
