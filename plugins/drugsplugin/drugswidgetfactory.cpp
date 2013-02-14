/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
 *   eric.maeker@gmail.com                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "drugswidgetfactory.h"

#include <drugsbaseplugin/drugbasecore.h>
#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/drugsmodel.h>
#include <drugsbaseplugin/drugsio.h>

#include <drugsplugin/constants.h>
#include <drugsplugin/drugswidget/druginfo.h>
#include <drugsplugin/drugswidget/drugscentralwidget.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ipatient.h>

#include <formmanagerplugin/iformitem.h>

#include <translationutils/constants.h>

#include <QStringList>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>
#include <QTextBrowser>
#include <QFileDialog>
#include <QPrinter>
#include <QTextDocument>
#include <QTextEdit>
#include <QGridLayout>
#include <QModelIndex>
#include <QSpacerItem>
#include <QPushButton>

namespace {
    const char* const OPTION_HIDESELECTOR     = "hideselector";
    const char* const OPTION_WITHPRESCRIBING  = "withprescribing";
    const char* const OPTION_WITHPRINTING     = "withprinting";

    const char * const  DONTPRINTEMPTYVALUES = "DontPrintEmptyValues";
    const char * const  ADDCHRONICTHERAPY = "AddChronicTherapyButton";
}

using namespace DrugsWidget;
using namespace Internal;

static inline DrugsDB::DrugsBase &drugsBase() {return DrugsDB::DrugBaseCore::instance().drugsBase();}
static inline DrugsDB::DrugsIO &drugsIo() {return DrugsDB::DrugBaseCore::instance().drugsIo();}
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}

static inline bool dontPrintEmptyValues(Form::FormItem *item)
{
    return item->getOptions().contains(::DONTPRINTEMPTYVALUES, Qt::CaseInsensitive);
}

static inline bool addChronicButton(Form::FormItem *item)
{
    return item->getOptions().contains(::ADDCHRONICTHERAPY, Qt::CaseInsensitive);
}

//--------------------------------------------------------------------------------------------------------
//------------------------------------ mfDrugsWidget plugin interface ------------------------------------
//--------------------------------------------------------------------------------------------------------
DrugsWidgetsFactory::DrugsWidgetsFactory(QObject *parent) :
        Form::IFormWidgetFactory(parent)
{
}

DrugsWidgetsFactory::~DrugsWidgetsFactory()
{
}

bool DrugsWidgetsFactory::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);
    return true;
}

bool DrugsWidgetsFactory::extensionInitialized()
{
    return true;
}


QStringList DrugsWidgetsFactory::providedWidgets() const
{
    // TODO: add more type of plugs ?
     return QStringList() << "drugs" << "prescription" << "prescriptor" << "drugselector";
}

bool DrugsWidgetsFactory::isContainer(const int) const
{
    return false;
}

Form::IFormWidget *DrugsWidgetsFactory::createWidget(const QString &name, Form::FormItem *formItem, QWidget *parent)
{
    return new DrugsPrescriptorWidget(name, formItem, parent);
}

//--------------------------------------------------------------------------------------------------------
//--------------------------------- DrugsPrescriptorWidget implementation --------------------------------
//--------------------------------------------------------------------------------------------------------
DrugsPrescriptorWidget::DrugsPrescriptorWidget(const QString &name, Form::FormItem *formItem, QWidget *parent) :
    Form::IFormWidget(formItem,parent),
    m_PrescriptionModel(0),
    m_AddChronic(0)
{
    // Prepare Widget Layout and label
    QWidget *labelWidget = new QWidget(this);
    QBoxLayout *labelBox = getBoxLayout(Label_OnLeft, m_FormItem->spec()->label(), labelWidget);
    labelBox->setSpacing(0);
    labelBox->setMargin(0);
    labelBox->addWidget(m_Label);

    // Label always on top...
    QVBoxLayout *hb = new QVBoxLayout(this);
    hb->setSpacing(0);
    hb->setMargin(0);
    hb->addWidget(labelWidget);

    // create main widget
    m_CentralWidget = new DrugsCentralWidget(this);
    m_CentralWidget->initialize(formItem->extraData().value("options").contains(OPTION_HIDESELECTOR, Qt::CaseInsensitive));
    m_PrescriptionModel = m_CentralWidget->currentDrugsModel();

    // Manage options
    const QStringList &options = formItem->getOptions();
    if (options.contains(OPTION_WITHPRESCRIBING, Qt::CaseInsensitive)) {
        m_WithPrescribing = true;
    } else if (name.compare("drugselector",Qt::CaseInsensitive)==0) {
        m_WithPrescribing = false;
    } else if (name.compare("prescription", Qt::CaseInsensitive)==0 ||
               name.compare("prescriptor", Qt::CaseInsensitive)==0) {
        m_WithPrescribing = true;
    }
    m_PrescriptionModel->setSelectionOnlyMode(!m_WithPrescribing);

    if (addChronicButton(formItem)) {
        labelBox->addSpacerItem(new QSpacerItem(20,1, QSizePolicy::Expanding, QSizePolicy::Minimum));
        m_AddChronic = new QPushButton(QCoreApplication::translate(Constants::DRUGCONSTANTS_TR_CONTEXT, Constants::ADDLONGTERMTHERAPEUTICS_TEXT), this);
        labelBox->addWidget(m_AddChronic);
        connect(m_AddChronic, SIGNAL(clicked()), this, SLOT(addChronicTherapeutics()));
    }
    hb->addWidget(m_CentralWidget);

    if (options.contains("nointeractionchecking", Qt::CaseInsensitive)) {
        m_PrescriptionModel->setComputeDrugInteractions(false);
    }

    // create formitemdata
    DrugsWidgetData *drugsWidgetData = new DrugsWidgetData(formItem);
    drugsWidgetData->setDrugsPrescriptorWidget(this);
    formItem->setItemData(drugsWidgetData);
}

DrugsPrescriptorWidget::~DrugsPrescriptorWidget()
{
}

QString DrugsPrescriptorWidget::printableHtml(bool withValues) const
{
    if (withValues && dontPrintEmptyValues(m_FormItem) && m_PrescriptionModel->rowCount()==0) {
        return QString();
    }
    QString html = drugsIo().prescriptionToHtml(m_PrescriptionModel);
    int begin = html.indexOf("<body");
    begin = html.indexOf(">", begin) + 1;
    int end = html.indexOf("</body>");
    html = html.mid(begin, end-begin);
    begin = html.indexOf("<a href");
    if (begin!=-1) {
        // remove the link
        end = html.indexOf(">", begin) + 1;
        html = html.left(begin) + html.mid(end);
        html = html.remove("</a>");
    }
    return QString("<table width=100% border=1 cellspacing=0 style=\"margin: 1em 0em 1em 0em\">"
                   "<thead>"
                   "<tr>"
                   "<td style=\"vertical-align: top; font-weight: 600; padding: 5px\">"
                   "%1"
                   "</td>"
                   "</tr>"
                   "</thead>"
                   "<tbody>"
                   "<tr>"
                   "<td style=\"vertical-align: top; padding: 1px; margin: 0px\">"
                   "%2"
                   "</td>"
                   "</tr>"
                   "</tbody>"
                   "</table>")
            .arg(m_FormItem->spec()->label()).arg(html);
}

void DrugsPrescriptorWidget::addChronicTherapeutics()
{
    const QString &chronic = patient()->data(Core::IPatient::DrugsChronicTherapeutics).toString();
    if (!chronic.isEmpty()) {
        drugsIo().prescriptionFromXml(m_PrescriptionModel, chronic, DrugsDB::DrugsIO::AppendPrescription);
    }
}

void DrugsPrescriptorWidget::retranslate()
{
    m_Label->setText(m_FormItem->spec()->label());
    if (m_AddChronic)
        m_AddChronic->setText(QCoreApplication::translate(Constants::DRUGCONSTANTS_TR_CONTEXT, Constants::ADDLONGTERMTHERAPEUTICS_TEXT));
}

////////////////////////////////////////// ItemData /////////////////////////////////////////////
DrugsWidgetData::DrugsWidgetData(Form::FormItem *item) :
        Form::IFormItemData(), m_FormItem(item), m_Widget(0)
{}

DrugsWidgetData::~DrugsWidgetData()
{}

void DrugsWidgetData::setDrugsPrescriptorWidget(DrugsPrescriptorWidget *widget)
{
    m_Widget = widget;
    clear();
}

void DrugsWidgetData::clear()
{
    m_Widget->m_CentralWidget->clear();
}

bool DrugsWidgetData::isModified() const
{
    return m_Widget->m_PrescriptionModel->isModified();
}

void DrugsWidgetData::setModified(bool modified)
{
    m_Widget->m_PrescriptionModel->setModified(modified);
}

bool DrugsWidgetData::setData(const int ref, const QVariant &data, const int role)
{
    Q_UNUSED(ref);
    Q_UNUSED(data);
    Q_UNUSED(role);
    return true;
}

QVariant DrugsWidgetData::data(const int ref, const int role) const
{
//    qWarning() << "DrugsWidgetData::data(" << ref << role << ");" << Core::IPatient::DrugsInnAllergies;

    if (role != Form::IFormItemData::PatientModelRole)
        return QVariant();

    DrugsDB::DrugsModel *model = m_Widget->m_PrescriptionModel;
    int rowCount = model->rowCount();

//    QList<int> refs;
//    if (ref==Core::IPatient::DrugsAllergiesWithoutPrecision)

    switch (ref) {
    case Core::IPatient::DrugsAtcAllergies:
    case Core::IPatient::DrugsAtcIntolerances:
    {
        // setting inn codes
        QStringList atcs;
        for(int i=0; i < rowCount; ++i) {
            atcs.append(model->index(i, DrugsDB::Constants::Drug::InnCodes).data().toStringList());
        }
        atcs.removeAll("");
        atcs.removeDuplicates();
        return atcs;
        break;
    }
    case Core::IPatient::DrugsInnAllergies:
    case Core::IPatient::DrugsInnIntolerances:
    {
        QStringList inns;
        for(int i=0; i < rowCount; ++i) {
            inns.append(model->index(i, DrugsDB::Constants::Drug::Inns).data().toStringList());
        }
        inns.removeAll("");
        inns.removeDuplicates();
        //            qWarning() << inns;
        return inns;
        break;
    }
    case Core::IPatient::DrugsChronicTherapeutics:
    {
        return drugsIo().prescriptionToXml(model);
    }

    }  // End switch

    return QVariant();
}

void DrugsWidgetData::setStorableData(const QVariant &data)
{
    m_Widget->m_CentralWidget->clear();
    m_Widget->m_PrescriptionModel->setModified(false);
    if (!data.isValid())
        return;
    drugsIo().prescriptionFromXml(m_Widget->m_PrescriptionModel, data.toString(), DrugsDB::DrugsIO::ReplacePrescription);
    m_Widget->m_PrescriptionModel->setModified(false);
}

QVariant DrugsWidgetData::storableData() const
{
    return drugsIo().prescriptionToXml(m_Widget->m_PrescriptionModel);
}
