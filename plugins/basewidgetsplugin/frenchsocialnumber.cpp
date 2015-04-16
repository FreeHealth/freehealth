/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main developers : Eric MAEKER, MD <eric.maeker@gmail.com>             *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "frenchsocialnumber.h"
#include "constants.h"
#include "frenchsocialnumberwidget.h"

#include <formmanagerplugin/iformitem.h>

#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>

#include <utils/log.h>

#include <QEvent>
#include <QKeyEvent>
#include <QRegExpValidator>

#include <QDebug>

using namespace BaseWidgets;
using namespace Internal;

static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////   FrenchSocialNumberFormWidget   //////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
FrenchSocialNumberFormWidget::FrenchSocialNumberFormWidget(Form::FormItem *formItem, QWidget *parent) :
    Form::IFormWidget(formItem,parent), m_NSS(0)
{
    setObjectName("FrenchSocialNumber_" + m_FormItem->uuid());
    // Create NSS widget
    m_NSS = new FrenchSocialNumberWidget(this);
    m_NSS->setObjectName("FrenchSocialNumber_" + m_FormItem->uuid());

    // QtUi Loaded ?
    const QString &layout = formItem->spec()->value(Form::FormItemSpec::Spec_UiInsertIntoLayout).toString();
    if (!layout.isEmpty()) {
        // Find layout
        QLayout *lay = formItem->parentFormMain()->formWidget()->findChild<QLayout*>(layout);
        if (lay) {
            lay->addWidget(m_NSS);
        } else {
            LOG_ERROR("Using the QtUiLinkage, layout not found in the ui: " + formItem->uuid());
        }
        m_Label = Constants::findLabel(formItem);
    } else {
        QHBoxLayout *hb = new QHBoxLayout(this);
        // Add QLabel
        createLabel(m_FormItem->spec()->label(), Qt::AlignJustify);
        hb->addWidget(m_Label);

        hb->addWidget(m_NSS);
    }
    retranslate();

    setFocusedWidget(m_NSS);

    // create itemdata
    m_ItemData = new FrenchSocialNumberFormData(formItem);
    m_ItemData->setWidget(m_NSS);
    formItem->setItemData(m_ItemData);
}

FrenchSocialNumberFormWidget::~FrenchSocialNumberFormWidget()
{
}

QString FrenchSocialNumberFormWidget::printableHtml(bool withValues) const
{
    if (m_FormItem->getOptions().contains("notprintable"))
        return QString();

    QString content;
    if (!withValues) {
        content += QString("%1:&nbsp;%2")
                .arg(m_FormItem->spec()->label())
                .arg(m_NSS->emptyHtmlMask());
    } else {
        content += QString("%1:&nbsp;%2")
                .arg(m_FormItem->spec()->label())
                .arg(m_NSS->toHtml());
    }
    return content;
}

void FrenchSocialNumberFormWidget::retranslate()
{
    if (m_Label)
        m_Label->setText(m_FormItem->spec()->label());
}

/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////   FrenchSocialNumberFormData   ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
FrenchSocialNumberFormData::FrenchSocialNumberFormData(Form::FormItem *item) :
    m_FormItem(item), m_Widget(0)
{}

FrenchSocialNumberFormData::~FrenchSocialNumberFormData()
{}

void FrenchSocialNumberFormData::clear()
{
//    WARN_FUNC;
    m_Widget->clear();
}

void FrenchSocialNumberFormData::populateWithPatientData()
{
//    WARN_FUNC;
    if (!m_Widget->numberWithControlKey().isEmpty())
        return;
    if (patient()->uuid().isEmpty()) {
        m_Widget->clear();
        return;
    }

    QString fullNumber;
    fullNumber.fill(' ', 13);

//    qWarning() << "NSS populateWithPatientData";
    // Add patient default values
    if (patient()->data(Core::IPatient::Gender).toString()=="M") {
        fullNumber[0] = '1';
    } else if (patient()->data(Core::IPatient::Gender).toString()=="F") {
        fullNumber[0] = '2';
    }

    // Birth date
    QModelIndex idx = patient()->index(patient()->currentPatientIndex().row(), Core::IPatient::DateOfBirth);
    QDate birth = patient()->data(idx, Qt::EditRole).toDate();
    if (birth.isValid()) {
        // year
        fullNumber = fullNumber.left(1) + QString::number(birth.year()).right(2) + fullNumber.mid(3);
        // month
        QString month = QString::number(birth.month());
        if (month.size()==1)
            month.prepend("0");
        fullNumber = fullNumber.left(3) + month + fullNumber.mid(5);
    }

    fullNumber = fullNumber.simplified();
    m_Widget->setNumberWithoutControlKey(fullNumber);
//    qWarning() << "    AUTO" << fullNumber;
}

bool FrenchSocialNumberFormData::isModified() const
{
//    WARN_FUNC;
    return m_OriginalValue != m_Widget->numberWithControlKey();
}

void FrenchSocialNumberFormData::setModified(bool modified)
{
//    WARN_FUNC << modified;
    if (!modified)
        m_OriginalValue = m_Widget->numberWithControlKey();
}

void FrenchSocialNumberFormData::setReadOnly(bool readOnly)
{
//    WARN_FUNC << readOnly;
    m_Widget->setEnabled(!readOnly);
}

bool FrenchSocialNumberFormData::isReadOnly() const
{
    WARN_FUNC;
    return (!m_Widget->isEnabled());
}

bool FrenchSocialNumberFormData::setData(const int ref, const QVariant &data, const int role)
{
    Q_UNUSED(ref);
    Q_UNUSED(data);
    Q_UNUSED(role);
//    qWarning() << "FrenchSocialNumberFormData::setData" << data << role << ref;
//    if (role==Qt::EditRole || role==Qt::DisplayRole) {
//        if (data.canConvert(QVariant::Int))  { // Tristate
//            m_Check->setCheckState(Qt::CheckState(data.toInt()));
//        }
//    }
    return true;
}

QVariant FrenchSocialNumberFormData::data(const int ref, const int role) const
{
//    WARN_FUNC << this << m_Widget->numberWithControlKey();
    Q_UNUSED(ref);
    Q_UNUSED(role);
    return m_Widget->numberWithControlKey();
}

void FrenchSocialNumberFormData::setStorableData(const QVariant &data)
{
//    WARN_FUNC << data;
    if (!data.isValid())
        return;
    m_OriginalValue = data.toString();
    if (m_OriginalValue.simplified().isEmpty()
            && !patient()->uuid().isEmpty())
        populateWithPatientData();
    else
        m_Widget->setNumberWithControlKey(m_OriginalValue);
//    qWarning() << "    " << m_Widget->numberWithControlKey();
}

QVariant FrenchSocialNumberFormData::storableData() const
{
//    WARN_FUNC << m_Widget->numberWithControlKey();
    return m_Widget->numberWithControlKey();
}
