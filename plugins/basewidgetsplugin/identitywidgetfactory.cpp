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
#include "identitywidgetfactory.h"

#include <patientbaseplugin/identitywidget.h>
#include <patientbaseplugin/patientmodel.h>
#include <patientbaseplugin/patientwidgetmanager.h>
#include <patientbaseplugin/patientselector.h>

#include <formmanagerplugin/iformitem.h>

#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>

#include <utils/log.h>
#include <translationutils/constants.h>
#include <translationutils/trans_datetime.h>

using namespace BaseWidgets;
using namespace Trans::ConstantTranslations;

static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}

inline static QStringList getOptions(Form::FormItem *item)
{
    QStringList l;
    l = item->extraData().value("options").split(";", QString::SkipEmptyParts);
    l += item->extraData().value("option").split(";", QString::SkipEmptyParts);
    return l;
}

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


inline static int getNumberOfColumns(Form::FormItem *item, int defaultValue = 1)
{
    if (!item->extraData().value("column").isEmpty())
        return item->extraData().value("column").toInt();
    else
        return defaultValue;
}

IdentityFormWidget::IdentityFormWidget(Form::FormItem *formItem, QWidget *parent) :
    Form::IFormWidget(formItem,parent), m_ContainerLayout(0)
{
    setObjectName("IdentityFormWidget");
    // Create the central widget / layout
    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);
//    QWidget *mainWidget = new QWidget;
//    mainLayout->addWidget(mainWidget);
//    mainLayout->addStretch();

    // get options
    const QStringList &options = formItem->getOptions();
    Patients::IdentityWidget::EditMode mode;
    if (options.contains("readonly", Qt::CaseInsensitive))
        mode = Patients::IdentityWidget::ReadOnlyMode;
    else
        mode = Patients::IdentityWidget::ReadWriteMode;

    m_Identity = new Patients::IdentityWidget(this, mode);
    m_Identity->setCurrentPatientModel(Patients::PatientModel::activeModel());

    // QtUi Loaded ?
    const QString &layout = formItem->spec()->value(Form::FormItemSpec::Spec_UiInsertIntoLayout).toString();
    if (!layout.isEmpty()) {
        // Find layout
        QLayout *lay = qFindChild<QLayout*>(formItem->parentFormMain()->formWidget(), layout);
        if (lay) {
            lay->addWidget(m_Identity);
        } else {
            LOG_ERROR("Using the QtUiLinkage, layout not found in the ui: " + formItem->uuid());
        }
    } else {
//        m_ContainerLayout = new QGridLayout(mainWidget);
//        m_ContainerLayout->setMargin(0);
//        m_ContainerLayout->setSpacing(0);
//        m_ContainerLayout->addWidget(m_Identity, 1, 0,  1, numberColumns);
        mainLayout->addWidget(m_Identity, 1, 0);
    }

    if (options.contains("compact", Qt::CaseInsensitive)) {
        m_Identity->layout()->setSpacing(0);
        m_Identity->layout()->setMargin(0);
    }

    connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(onCurrentPatientChanged()));
    onCurrentPatientChanged();

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

void IdentityFormWidget::onCurrentPatientChanged()
{
    m_Identity->setCurrentIndex(patient()->currentPatientIndex());
}

void IdentityFormWidget::retranslate()
{
}

QString IdentityFormWidget::printableHtml(bool withValues) const
{
    if (m_FormItem->getOptions().contains("notprintable"))
        return QString();

    if (withValues) {
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
    } else {
        // TODO: code here: empty identity HTML mask
        return QString();
    }
    return QString();
}

bool IdentityWidgetData::isModified() const
{
    return m_Widget->m_Identity->isModified();
}

QVariant IdentityWidgetData::storableData() const
{
    m_Widget->m_Identity->submit(); return QVariant();
}
