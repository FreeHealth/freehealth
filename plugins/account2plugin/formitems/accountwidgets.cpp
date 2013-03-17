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
 *   Main developers : Eric Maeker                                *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class Account::Internal::AccountWidgetFactory
 * \brief The Accountancy formitem widget factory.
 */

/*!
 * \class Account::Internal::FeeFormWidget
 * \brief The Accountancy formitem widget Fee manager.
 * Should be presented like this
 * \code
 * +----------------------------+
 * | Search [                  ]|
 * +----------------------------+
 * |                            |
 * |       FEE SELECTION        |
 * |         TREEVIEW           |
 * | Tree because of categories |
 * |                            |
 * +----------------------------+
 * |                            |
 * |       SELECTED FEES        |
 * |         LISTVIEW           |
 * |                            |
 * +----------------------------+
 * | [ BUTTONS ] [ BUTTONS ]    |
 * +----------------------------+
 * \endcode
 * Buttons should include:
 * - remove fee
 * - print fees
 * - create a payment (creates a payment for all the selected fees)
 *
 * Views:
 * Selection view should include:
 * - Fee label
 * - Medical Procedure label
 * - Amount
 * Selected fees view should include [+][-] buttons when mouse hover items to ease the
 * fee adding/removal process.
 *
 * What is saved in the episodeBase and how to link episode/fees/payments?
 * - An xml fee collection/payment (shouldn't we use QString UUID instead of simple ID integers)
 * - Nothing and we use episode date/fee/payment date, patient uuid to link objects?
 */


#include "accountwidgets.h"
#include "constants.h"

#include <account2plugin/accountcore.h>

#include <formmanagerplugin/iformitem.h>

#include <utils/log.h>
#include <utils/global.h>

#include <translationutils/constants.h>

#include <QDebug>

using namespace Account2;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Account2::AccountCore *accountCore() {return Account2::AccountCore::instance();}

static const QStringList widgetsName =
        QStringList() << "fee";

AccountWidgetFactory::AccountWidgetFactory(QObject *parent)
{
}

AccountWidgetFactory::~AccountWidgetFactory()
{}

bool AccountWidgetFactory::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);
    return accountCore()->isDatabaseInitialized();
}

bool AccountWidgetFactory::extensionInitialized()
{
    return true;
}

bool AccountWidgetFactory::isInitialized() const
{
    return true;
}

bool AccountWidgetFactory::isContainer(const int) const
{
    return false;
}

QStringList AccountWidgetFactory::providedWidgets() const
{
    return widgetsName;
}

Form::IFormWidget *AccountWidgetFactory::createWidget(const QString &name, Form::FormItem *object, QWidget *parent)
{
    int id = widgetsName.indexOf(name);
    if (id == -1)
        return 0;
    switch (id)
    {
    case 0 : return new FeeFormWidget(formItem, parent);
    }
    return 0;
}


/*! Constructor of the Account::Internal::FeeFormWidget class */
FeeFormWidget::FeeFormWidget(Form::FormItem *linkedObject, QWidget *parent)
    : Form::IFormWidget(linkedObject, parent)
{
    setObjectName("FeeFormWidget");
    
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
    
    //    setFocusableWidget(m_List);
    
    //    // create FormItemData
    //    FeeFormWidgetData *data = new FeeFormWidgetData(m_FormItem);
    //    data->setBaseList(this);
    //    m_FormItem->setItemData(data);
    
    //    connect(m_List->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), data, SLOT(onValueChanged()));
}

/*! Destructor of the Account::Internal::FeeFormWidget class */
FeeFormWidget::~FeeFormWidget()
{
}

void FeeFormWidget::addWidgetToContainer(Form::IFormWidget *widget)
{
    Q_UNUSED(widget);
}

bool FeeFormWidget::isContainer() const
{
    return false;
}

QString FeeFormWidget::printableHtml(bool withValues = true) const
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

void FeeFormWidget::retranslate()
{
    if (m_Label)
        m_Label->setText(m_FormItem->spec()->label());
}

/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////   FeeFormWidget   //////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
FeeFormWidgetData::FeeFormWidgetData(Form::FormItem *item, FeeFormWidget *formWidget) :
    m_FormItem(item),
    m_Form(formWidget)
{
}

FeeFormWidgetData::~FeeFormWidgetData()
{
}

/** \brief Set the widget to the default value \sa FormItem::FormItemValue*/
void FeeFormWidgetData::clear()
{
    // Reset widget content to its default value
    // Using m_FormItem->valueReferences()->defaultValue().toString()
}

bool FeeFormWidgetData::isModified() const
{
    //    QStringList actual = storableData().toStringList();
    //    return actual != m_OriginalValue;
    return false;
}

void FeeFormWidgetData::setModified(bool modified)
{
    //    if (!modified)
    //        m_OriginalValue = storableData().toStringList();
}

bool FeeFormWidgetData::setData(const int ref, const QVariant &data, const int role)
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

QVariant FeeFormWidgetData::data(const int ref, const int role) const
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

void FeeFormWidgetData::setStorableData(const QVariant &data)
{
    if (m_Form) {
        // Here you will get the exact value returned by storableData()
        // when an episode is activated. Use this value to populate your widget/itemData
    }
}

/** Storable data of a List is the uuids of the selected items sorted in alphabetic order. */
QVariant FeeFormWidgetData::storableData() const
{
    if (m_Form) {
        // Return the value you want to get back in setStorableData.
        // The returned value will be saved into the database 'as is'
        // And will be sent back in setStorableData (when an episode
        // will be activated)
    }
    return QVariant();
}

void FeeFormWidgetData::onValueChanged()
{
    // Execute the 'onValueChanged' item script
    Constants::executeOnValueChangedScript(m_FormItem);
    Q_EMIT dataChanged(0);
}

