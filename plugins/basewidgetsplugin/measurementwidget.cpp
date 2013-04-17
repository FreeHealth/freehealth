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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "measurementwidget.h"
#include "constants.h"

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_units.h>

#include <QComboBox>
#include <QDoubleSpinBox>

using namespace BaseWidgets;
using namespace Constants;
using namespace Trans::ConstantTranslations;

MeasurementWidget::MeasurementWidget(Form::FormItem *formItem, QWidget *parent) :
    Form::IFormWidget(formItem,parent),
    m_units(0),
    m_value(0),
    m_defaultUnitId(-1),
    m_isWeight(false),
    m_isLength(false)
{
    setObjectName("MeasurementWidget");
    bool uiBased = false;
    QLayout *hb = 0;
    const QString &layout = formItem->spec()->value(Form::FormItemSpec::Spec_UiInsertIntoLayout).toString();
    const QString &widgets = formItem->spec()->value(Form::FormItemSpec::Spec_UiWidget).toString();
    if (!layout.isEmpty()) {
        // QtUi Loaded using layout
        QLayout *lay = formItem->parentFormMain()->formWidget()->findChild<QLayout*>(layout);
        if (lay) {
            hb = lay;
        } else {
            LOG_ERROR("Using the QtUiLinkage, layout not found in the ui: " + formItem->uuid() + "; layout name: "+ layout);
            // To avoid segfaulting create a fake combo
            hb = new QHBoxLayout(this);
        }
        // Find Label
        m_Label = findLabel(formItem);

        // Add spin + combo to the layout
        m_units = new QComboBox(this);
        m_value = new QDoubleSpinBox(this);
        hb->addWidget(m_value);
        hb->addWidget(m_units);
        uiBased= true;
    } else if (!widgets.isEmpty()) {
        // Qt Ui using widgets naming
        if (widgets.count(";") != 1) {
            LOG_ERROR("Wrong widget naming. No ; found.");
        } else {
            QStringList w = widgets.split(";");
            QDoubleSpinBox *value = formItem->parentFormMain()->formWidget()->findChild<QDoubleSpinBox*>(w.at(0));
            if (value) {
                m_value = value;
            } else {
                LOG_ERROR("Using the QtUiLinkage, value spinbox not found in the ui: " + formItem->uuid() + "; widget name: "+ w.at(0));
            }
            QComboBox *units = formItem->parentFormMain()->formWidget()->findChild<QComboBox*>(w.at(1));
            if (units) {
                m_units = units;
            } else {
                LOG_ERROR("Using the QtUiLinkage, units combobox not found in the ui: " + formItem->uuid() + "; widget name: "+ w.at(1));
            }
            uiBased = true;
        }
        // Find Label
        m_Label = findLabel(formItem);
    } else {
        hb = getBoxLayout(OnLeft, m_FormItem->spec()->label(), this);
        hb->addWidget(m_Label);
        hb->setMargin(0);
        hb->setSpacing(0);
    }
    // Ensure that the widgets are available
    if (!hb && (!m_units || !m_value) && !uiBased) {
        LOG_ERROR("No layout, creating a default one. Form item: " + formItem->uuid());
        hb = new QHBoxLayout(this);
    }
    if (!m_value) {
        m_value = new QDoubleSpinBox(this);
        m_value->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        hb->addWidget(m_value);
    }
    if (!m_units) {
        // Add spin + combo to the layout
        m_units = new QComboBox(this);
        m_value->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        hb->addWidget(m_units);
    }
    m_units->setFocusPolicy(Qt::ClickFocus);

    const QStringList &options = formItem->getOptions();
    if (options.contains("Length", Qt::CaseInsensitive)) {
        populateWithLength();
    } else if (options.contains("Weight", Qt::CaseInsensitive)) {
        populateWithWeight();
    }

    setFocusedWidget(m_value);
    setLastTabWidget(m_units);
    setFocusProxy(m_value);

    // create item data
    MeasurementWidgetData *data = new MeasurementWidgetData(formItem);
    data->setMeasurementWidget(this);
    data->setDefaultUnitIndex(m_defaultUnitId);
    formItem->setItemData(data);

//    connect(m_Text->textEdit(), SIGNAL(textChanged()), data, SLOT(onValueChanged()));
    retranslate();
}

MeasurementWidget::~MeasurementWidget()
{
}

void MeasurementWidget::setTabOrder(bool consoleWarn)
{
    QWidget::setTabOrder(m_value, m_units);
    if (consoleWarn)
        qWarning() << "   Measurement: taborder" << m_value << m_units;
}

void MeasurementWidget::populateWithLength()
{
    if (!m_units)
        return;
    // Take care: see MeasurementWidgetData::data code
    m_isLength = true;
    Form::FormItemValues *vals = m_FormItem->valueReferences();
    QString defaultVal = m_FormItem->extraData().value("default");
    int i = 0;
    QString uid = "centimeter";
    QString p = tkTr(Trans::Constants::CENTIMETER_S);
    vals->setValue(Form::FormItemValues::Value_Uuid, i, uid);
    vals->setValue(Form::FormItemValues::Value_Possible, i, p);
    m_units->addItem(p);
    if (defaultVal.compare(uid, Qt::CaseInsensitive)==0)
        m_defaultUnitId = i;
    ++i;
    uid = "meter";
    p = tkTr(Trans::Constants::METER_S);
    vals->setValue(Form::FormItemValues::Value_Uuid, i, uid);
    vals->setValue(Form::FormItemValues::Value_Possible, i, p);
    m_units->addItem(p);
    if (defaultVal.compare(uid, Qt::CaseInsensitive)==0)
        m_defaultUnitId = i;
    ++i;
    uid = "inch";
    p = tkTr(Trans::Constants::INCH_S);
    vals->setValue(Form::FormItemValues::Value_Uuid, i, uid);
    vals->setValue(Form::FormItemValues::Value_Possible, i, p);
    m_units->addItem(p);
    if (defaultVal.compare(uid, Qt::CaseInsensitive)==0)
        m_defaultUnitId = i;
    ++i;
    uid = "foot";
    p = tkTr(Trans::Constants::FOOT_S);
    vals->setValue(Form::FormItemValues::Value_Uuid, i, uid);
    vals->setValue(Form::FormItemValues::Value_Possible, i, p);
    m_units->addItem(p);
    if (defaultVal.compare(uid, Qt::CaseInsensitive)==0)
        m_defaultUnitId = i;
}

void MeasurementWidget::populateWithWeight()
{
    if (!m_units)
        return;
    // Take care: see MeasurementWidgetData::data code
    m_isWeight = true;
    Form::FormItemValues *vals = m_FormItem->valueReferences();
    QString defaultVal = m_FormItem->extraData().value("default");
    int i = 0;
    QString uid = "gram";
    QString p = tkTr(Trans::Constants::GRAM_S);
    vals->setValue(Form::FormItemValues::Value_Uuid, i, uid);
    vals->setValue(Form::FormItemValues::Value_Possible, i, p);
    m_units->addItem(p);
    if (defaultVal.compare(uid, Qt::CaseInsensitive)==0)
        m_defaultUnitId = i;
    ++i;
    uid = "kilogram";
    p = tkTr(Trans::Constants::KILOGRAM_S);
    vals->setValue(Form::FormItemValues::Value_Uuid, i, uid);
    vals->setValue(Form::FormItemValues::Value_Possible, i, p);
    m_units->addItem(p);
    if (defaultVal.compare(uid, Qt::CaseInsensitive)==0)
        m_defaultUnitId = i;
    ++i;
    uid = "ounce";
    p = tkTr(Trans::Constants::OUNCE_S);
    vals->setValue(Form::FormItemValues::Value_Uuid, i, uid);
    vals->setValue(Form::FormItemValues::Value_Possible, i, p);
    m_units->addItem(p);
    if (defaultVal.compare(uid, Qt::CaseInsensitive)==0)
        m_defaultUnitId = i;
    ++i;
    uid = "pound";
    p = tkTr(Trans::Constants::POUND_S);
    vals->setValue(Form::FormItemValues::Value_Uuid, i, uid);
    vals->setValue(Form::FormItemValues::Value_Possible, i, p);
    m_units->addItem(p);
    if (defaultVal.compare(uid, Qt::CaseInsensitive)==0)
        m_defaultUnitId = i;
}

QString MeasurementWidget::printableHtml(bool withValues) const
{
    if (m_FormItem->getOptions().contains(Constants::NOT_PRINTABLE, Qt::CaseInsensitive))
        return QString();

    if (withValues) {
        return QString("%1 %2").arg(m_value->value()).arg(m_units->currentText());
    } else if (m_FormItem->getOptions().contains(Constants::DONTPRINTEMPTYVALUES, Qt::CaseInsensitive)
               && m_value->value() == 0) {
        return QString();
    }
    return QString("--&nbsp;&nbsp;---");
}

void MeasurementWidget::retranslate()
{
    if (m_Label)
        m_Label->setText(m_FormItem->spec()->label());
    // TODO: retranslate units combobox
}

////////////////////////////////////////// ItemData /////////////////////////////////////////////
MeasurementWidgetData::MeasurementWidgetData(Form::FormItem *item) :
    m_FormItem(item),
    m_Measurement(0),
    m_defaultUnitId(-1)
{}

MeasurementWidgetData::~MeasurementWidgetData()
{}

void MeasurementWidgetData::clear()
{
//    WARN_FUNC;
    m_OriginalValue = -1;
    m_Measurement->m_units->setCurrentIndex(-1);
    m_Measurement->m_units->setCurrentIndex(m_defaultUnitId);
    m_Measurement->m_value->setValue(0.0);
}

void MeasurementWidgetData::setSelectedUnit(const QString &uuid)
{
    const QStringList &uuids = m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Uuid);
    int row = uuids.lastIndexOf(uuid);
    m_Measurement->m_units->setCurrentIndex(row);
}

QString MeasurementWidgetData::selectedUnitUuid() const
{
    const QStringList &uuids = m_FormItem->valueReferences()->values(Form::FormItemValues::Value_Uuid);
    if (IN_RANGE_STRICT_MAX(m_Measurement->m_units->currentIndex(), 0, uuids.count()))
            return uuids.at(m_Measurement->m_units->currentIndex());
    return "null";
}

bool MeasurementWidgetData::isModified() const
{
    return m_OriginalValue != storableData().toString();
}

void MeasurementWidgetData::setModified(bool modified)
{
    if (!modified)
        m_OriginalValue = storableData().toString();
}

void MeasurementWidgetData::setReadOnly(bool readOnly)
{
    m_Measurement->m_units->setEnabled(!readOnly);
    m_Measurement->m_value->setEnabled(!readOnly);
}

bool MeasurementWidgetData::isReadOnly() const
{
    return (!m_Measurement->m_units->isEnabled());
}

bool MeasurementWidgetData::setData(const int ref, const QVariant &data, const int role)
{
    Q_UNUSED(ref);
    Q_UNUSED(data);
    Q_UNUSED(role);
    // TODO: code this
//    qWarning() << "MeasurementWidgetData::setData" << data << role;
//    if (role==Qt::EditRole) {
//        m_Editor->textEdit()->setHtml(data.toString());
//        onValueChanged();
//    }
    return true;
}

QVariant MeasurementWidgetData::data(const int ref, const int role) const
{
    Q_UNUSED(ref);
    Q_UNUSED(role);
//    qWarning() << "MeasurementWidgetData::data" << ref << role << m_Measurement->m_value->value() << m_Measurement->m_units->currentText();

    // Manage specific measurements (weight & length)
    if (role == Form::IFormItemData::PatientModelRole) {
        if (m_Measurement->m_isWeight) {
            // return in grams
            double val = m_Measurement->m_value->value();
            switch (m_Measurement->m_units->currentIndex()) {
            case 0: return val;
            case 1: return val*100.;
            case 2: return val*28.3495231; // 1 ounce = 28.3495231 grams
            case 3: return val*453.59237;  // 1 pound = 453.59237 grams
            }
            return val;
        } else if (m_Measurement->m_isLength) {
            // return in centimeters
            double val = m_Measurement->m_value->value();
            switch (m_Measurement->m_units->currentIndex()) {
            case 0: return val;
            case 1: return val*100.;
            case 2: return val*2.54;   // 1 inch = 2.54 centimeters
            case 3: return val*30.48;  // 1 foot = 30.48 centimeters
            }
            return val;
        }
    }
    // return value + unit in a string
    return QString("%1 %2")
            .arg(QString::number(m_Measurement->m_value->value(), 'f', 2))
            .arg(m_Measurement->m_units->currentText());
}

// Storable data are serialized like this: value;;unit
void MeasurementWidgetData::setStorableData(const QVariant &data)
{
//    qWarning() << "MeasurementWidgetData::setStorableData" << data;
    clear();
    if (data.isNull() || data.toString().simplified().isEmpty()) {
        m_OriginalValue.clear();
    } else {
        m_OriginalValue = data.toString();
        if (m_OriginalValue.contains(";;")) {
            const QStringList &val = m_OriginalValue.split(";;");
            m_Measurement->m_value->setValue(val.at(0).toDouble());
            setSelectedUnit(val.at(1));
        }
    }
}

QVariant MeasurementWidgetData::storableData() const
{
    return QString("%1;;%2")
            .arg(m_Measurement->m_value->value())
            .arg(selectedUnitUuid());
}

void MeasurementWidgetData::onValueChanged()
{
    // TODO: code this
//    executeOnValueChangedScript(m_FormItem);
    Q_EMIT dataChanged(0);
}


