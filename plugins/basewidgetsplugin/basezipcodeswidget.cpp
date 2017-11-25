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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "basedatecompleterwidget.h"
#include "constants.h"

#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/iformitemspec.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/widgets/moderndateeditor.h>

#include <coreplugin/icore.h>
#include <coreplugin/iscriptmanager.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

//#include <translationutils/constants.h>
//#include <translationutils/trans_menu.h>
//#include <translationutils/trans_filepathxml.h>

#include <QUiLoader>
#include <QBuffer>

using namespace BaseWidgets;
using namespace Internal;

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}

BaseDateCompleterWidget::BaseDateCompleterWidget(Form::FormItem *formItem, QWidget *parent) :
    Form::IFormWidget(formItem, parent),
    _dateEdit(0)
{
    setObjectName("BaseDateCompleterWidget_" + m_FormItem->uuid());

    // Create the detailsWidget
    _dateEdit = new Utils::ModernDateEditor(this);
    _dateEdit->setDateIcon(theme()->iconFullPath(Core::Constants::ICONDATE));
    _dateEdit->setClearIcon(theme()->iconFullPath(Core::Constants::ICONCLEARLINEEDIT));

    // Create the global layout
    const QString &uiLayout = formItem->spec()->value(Form::FormItemSpec::Spec_UiInsertIntoLayout).toString();
    if (!uiLayout.isEmpty()) {
        QLayout *lay = formItem->parentFormMain()->formWidget()->findChild<QLayout*>(uiLayout);
        if (lay) {
            lay->addWidget(_dateEdit);
        } else {
            LOG_ERROR("Using the QtUiLinkage, layout not found in the ui: " + formItem->uuid());
        }
    } else {
        QVBoxLayout *layout = new QVBoxLayout(this);
        setLayout(layout);
        //    layout->setMargin(0);
        //    layout->setSpacing(0);
        layout->addWidget(_dateEdit);
    }

    // create itemdata
//    BaseDateCompleterData *data = new BaseDateCompleterData(formItem);
//    data->setDateEditor(this);
//    formItem->setItemData(data);

    retranslate();

}

BaseDateCompleterWidget::~BaseDateCompleterWidget()
{
}

void BaseDateCompleterWidget::addWidgetToContainer(Form::IFormWidget *widget)
{
    Q_UNUSED(widget);
}

bool BaseDateCompleterWidget::isContainer() const
{
    return false;
}

// Printing
QString BaseDateCompleterWidget::printableHtml(bool withValues) const
{
    if (m_FormItem->getOptions().contains(Constants::NOT_PRINTABLE))
        return QString();

    if (!withValues) {
        return QString("<table width=100% border=1 cellpadding=0 cellspacing=0  style=\"margin: 0px\">"
                       "<tbody>"
                       "<tr>"
                       "<td style=\"vertical-align: top; padding-left:2em; padding-top:5px; padding-bottom: 5px; padding-right:2em\">"
                       "%1"
                       "</td>"
                       "<td style=\"vertical-align: top;\" width=50%>"
                       "&nbsp;"
                       "</td>"
                       "</tr>"
                       "</tbody>"
                       "</table>")
                .arg(m_FormItem->spec()->label());
    } else {
        if (Constants::dontPrintEmptyValues(m_FormItem) && _dateEdit->date().isNull())
            return QString();
        return QString("<table width=100% border=1 cellpadding=0 cellspacing=0  style=\"margin: 0px\">"
                       "<tbody>"
                       "<tr>"
                       "<td style=\"vertical-align: top; padding-left:2em; padding-top:5px; padding-bottom: 5px; padding-right:2em\">"
                       "%1"
                       "</td>"
                       "<td style=\"vertical-align: top;\">"
                       "%2"
                       "</td>"
                       "</tr>"
                       "</tbody>"
                       "</table>")
                .arg(m_FormItem->spec()->label())
                .arg(QLocale().toString(_dateEdit->date(), Constants::getDateFormat(m_FormItem)).replace(" ","&nbsp;"));
    }
}

void BaseDateCompleterWidget::retranslate()
{
    Q_ASSERT(_dateEdit);
    if (m_Label)
        m_Label->setText(m_FormItem->spec()->label());
    if (_dateEdit)
        _dateEdit->setToolTip(m_FormItem->spec()->tooltip());
}

BaseDateCompleterData::BaseDateCompleterData(Form::FormItem *item) :
    m_FormItem(item), m_Date(0)
{
}

BaseDateCompleterData::~BaseDateCompleterData()
{
}

void BaseDateCompleterData::setDate(const QString &s)
{
    m_Date->_dateEdit->clear();
    m_Date->_dateEdit->setDate(QDate::fromString(s, Qt::ISODate));
    onValueChanged();
}

/** \brief Set the widget to the default value \sa FormItem::FormItemValue*/
void BaseDateCompleterData::clear()
{
    m_OriginalValue = m_FormItem->valueReferences()->defaultValue().toString();
    setDate(m_OriginalValue);
}

bool BaseDateCompleterData::isModified() const
{
    return m_OriginalValue != m_Date->_dateEdit->date().toString(Qt::ISODate);
}

void BaseDateCompleterData::setModified(bool modified)
{
    if (!modified)
        m_OriginalValue = m_Date->_dateEdit->date().toString(Qt::ISODate);
}

bool BaseDateCompleterData::setData(const int ref, const QVariant &data, const int role)
{
    Q_UNUSED(ref);
    if (role==Qt::EditRole) {
        if (data.canConvert<QDate>()) {
            m_Date->_dateEdit->setDate(data.toDate());
            onValueChanged();
        } else if (data.canConvert<QDateTime>()) {
            m_Date->_dateEdit->setDate(data.toDateTime().date());
            onValueChanged();
        }
    }
    return true;
}

QVariant BaseDateCompleterData::data(const int ref, const int role) const
{
    Q_UNUSED(ref);
    Q_UNUSED(role);
    return m_Date->_dateEdit->date();
}

void BaseDateCompleterData::setStorableData(const QVariant &data)
{
    setDate(data.toString());
    m_OriginalValue = data.toString();
}

QVariant BaseDateCompleterData::storableData() const
{
    return m_Date->_dateEdit->date().toString(Qt::ISODate);
}

void BaseDateCompleterData::onValueChanged()
{
    Constants::executeOnValueChangedScript(m_FormItem);
    Q_EMIT dataChanged(0);
}
