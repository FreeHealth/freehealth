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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "identitywidgetfactory.h"

#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>

#include <patientbaseplugin/patientmodel.h>
#include <patientbaseplugin/patientwidgetmanager.h>
#include <patientbaseplugin/patientselector.h>

#include <formmanagerplugin/iformitem.h>

#include <identityplugin/identityeditorwidget.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_datetime.h>

using namespace BaseWidgets;
using namespace Trans::ConstantTranslations;

static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}

//static inline QStringList getOptions(Form::FormItem *item)
//{
//    QStringList l;
//    l = item->extraData().value("options").split(";", QString::SkipEmptyParts);
//    l += item->extraData().value("option").split(";", QString::SkipEmptyParts);
//    return l;
//}

IdentityWidgetFactory::IdentityWidgetFactory(QObject *parent) :
    IFormWidgetFactory(parent)
{
}

IdentityWidgetFactory::~IdentityWidgetFactory()
{}

bool IdentityWidgetFactory::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);
    return true;
}

bool IdentityWidgetFactory::extensionInitialized()
{
    return true;
}

bool IdentityWidgetFactory::isInitialized() const
{
    return true;
}

QStringList IdentityWidgetFactory::providedWidgets() const
{
    return QStringList() << "identity" << "ident";
}

bool IdentityWidgetFactory::isContainer(const int idInStringList) const
{
    Q_UNUSED(idInStringList);
    return false;
}

Form::IFormWidget *IdentityWidgetFactory::createWidget(const QString &name, Form::FormItem *formItem, QWidget *parent)
{
    Q_UNUSED(name);
    return new IdentityFormWidget(formItem,parent);
}

IdentityFormWidget::IdentityFormWidget(Form::FormItem *formItem, QWidget *parent) :
    Form::IFormWidget(formItem,parent),
    m_ContainerLayout(0)
{
    setObjectName("IdentityFormWidget");
    // Create the central widget / layout
    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    // get options
    const QStringList &options = formItem->getOptions();
    m_Identity = new Identity::IdentityEditorWidget(this);
    m_Identity->initialize();

    Identity::IdentityEditorWidget::AvailableWidgets widgets = Identity::IdentityEditorWidget::FullIdentity;
    if (options.contains("with-photo", Qt::CaseInsensitive))
        widgets |= Identity::IdentityEditorWidget::Photo;
    if (options.contains("with-address", Qt::CaseInsensitive))
        widgets |= Identity::IdentityEditorWidget::FullAddress;
    if (options.contains("with-login", Qt::CaseInsensitive))
        widgets |= Identity::IdentityEditorWidget::FullLogin;
    m_Identity->setAvailableWidgets(widgets);

    if (options.contains("xml", Qt::CaseInsensitive))
        m_Identity->setXmlInOut(true);

    if (options.contains("readonly", Qt::CaseInsensitive))
        m_Identity->setReadOnly(true);

    // QtUi Loaded ?
    const QString &layout = formItem->spec()->value(Form::FormItemSpec::Spec_UiInsertIntoLayout).toString();
    if (!layout.isEmpty()) {
        // Find layout
        QLayout *lay = formItem->parentFormMain()->formWidget()->findChild<QLayout*>(layout);
        if (lay) {
            lay->addWidget(m_Identity);
            lay->setMargin(0);
            lay->setSpacing(0);
        } else {
            LOG_ERROR("Using the QtUiLinkage, layout not found in the ui: " + formItem->uuid());
        }
    } else {
        mainLayout->addWidget(m_Identity, 1, 0);
    }

    if (options.contains("compact", Qt::CaseInsensitive)) {
        m_Identity->layout()->setSpacing(0);
        m_Identity->layout()->setMargin(0);
    }

    setFocusedWidget(m_Identity);

    // create itemdata
    IdentityWidgetData *data = new IdentityWidgetData(m_FormItem);
    data->setIdentityFormWiget(this);
    m_FormItem->setItemData(data);
}

IdentityFormWidget::~IdentityFormWidget()
{
}

//void IdentityFormWidget::addWidgetToContainer(IFormWidget *widget)
//{
//    if (!widget)
//        return;
//    if (!m_ContainerLayout)
//        return;
//    col = (i % numberColumns);
//    row = (i / numberColumns);
//    m_ContainerLayout->addWidget(widget , row, col);
//    i++;
//}

void IdentityFormWidget::retranslate()
{
}

QString IdentityFormWidget::printableHtml(bool withValues) const
{
    if (m_FormItem->getOptions().contains("notprintable"))
        return QString();

    if (withValues) {
        if (!m_FormItem->spec()->value(Form::FormItemSpec::Spec_HtmlPrintMask).toString().isEmpty()) {
            QString mask = m_FormItem->spec()->value(Form::FormItemSpec::Spec_HtmlPrintMask).toString();
            // TODO: manage PadTools
            QHash<QString, QVariant> tokens;
            tokens.insert("USUALNAME", m_Identity->currentUsualName());
            tokens.insert("OTHERNAMES", m_Identity->currentOtherNames());
            tokens.insert("FIRSTNAME", m_Identity->currentFirstName());
            tokens.insert("TITLE", m_Identity->currentTitle());
            tokens.insert("GENDER", m_Identity->currentGender());
            tokens.insert("DATEOFBIRTH", m_Identity->currentDateOfBirth());
            tokens.insert("LANGUAGE", m_Identity->currentLanguage());
            tokens.insert("STREET", m_Identity->currentStreet());
            tokens.insert("CITY", m_Identity->currentCity());
            tokens.insert("STATE", m_Identity->currentStateProvince());
            tokens.insert("COUNTRY", m_Identity->currentCountryName());
            tokens.insert("COUNTRYISO", m_Identity->currentCountryIso());
            tokens.insert("ZIP", m_Identity->currentZipCode());
            Utils::replaceTokens(mask, tokens);
            return mask;
        } else {
            QString n = patient()->data(Core::IPatient::FullName).toString();
            n = n.simplified();
            QString age;
            // For pediatrics show full age
            // For adults show simplified age
            if (patient()->data(Core::IPatient::YearsOld).toInt() <= 15) {
                age = patient()->data(Core::IPatient::Age).toString();
            } else {
                age = patient()->data(Core::IPatient::YearsOld).toString() + " " + tkTr(Trans::Constants::YEARS);
            }
            QModelIndex idx = patient()->index(patient()->currentPatientIndex().row(), Core::IPatient::DateOfBirth);
            QDate dob = patient()->data(idx, Qt::EditRole).toDate();
            return QString("<table width=100% border=1 cellpadding=0 cellspacing=0>"
                           "<thead>"
                           "<tr>"
                           "<td style=\"vertical-align: top; padding: 5px\">"
                           "<span style=\"font-weight: 600\">%1</span> (%2; %3) - %4"
                           "</td>"
                           "</tr>"
                           "</thead>"
                           "<tbody>"
                           "<tr>"
                           "<td style=\"vertical-align: top; padding-left:2em; padding-top:5px; padding-bottom: 5px; padding-right:2em\">"
                           "%5"
                           "</td>"
                           "</tr>"
                           "</tbody>"
                           "</table>")
                    .arg(n)
                    .arg(QLocale().toString(dob, QLocale().dateFormat(QLocale::LongFormat)))
                    .arg(age)
                    .arg(patient()->data(Core::IPatient::Gender).toString())
                    .arg(patient()->data(Core::IPatient::FullAddress).toString())
                    ;
        }
    } else {
        // TODO: code here: empty identity HTML mask
        return QString();
    }
    return QString();
}

void IdentityWidgetData::clear()
{
    if (m_Widget->m_Identity->isXmlInOut())
        m_Widget->m_Identity->clear();
}

bool IdentityWidgetData::isModified() const
{
    return m_Widget->m_Identity->isModified();
}

void IdentityWidgetData::setModified(bool modified)
{
    Q_UNUSED(modified);
    // TODO: code here: HOW ??
}

void IdentityWidgetData::setReadOnly(bool readOnly)
{
    m_Widget->m_Identity->setReadOnly(readOnly);
}

bool IdentityWidgetData::isReadOnly() const
{
    return m_Widget->m_Identity->isReadOnly();
}

QVariant IdentityWidgetData::data(const int ref, const int role) const
{
    if (role==Form::IFormItemData::PrintRole) {
        return m_Widget->printableHtml(true);
    }
    Q_UNUSED(ref);
    return QVariant();
}

void IdentityWidgetData::setStorableData(const QVariant &value)
{
    if (m_Widget->m_Identity->isXmlInOut()) {
        m_Widget->m_Identity->fromXml(value.toString());
    }
}

QVariant IdentityWidgetData::storableData() const
{
    if (m_Widget->m_Identity->isXmlInOut()) {
        return m_Widget->m_Identity->toXml();
    } else {
        m_Widget->m_Identity->submit();
    }
    return QVariant();
}
