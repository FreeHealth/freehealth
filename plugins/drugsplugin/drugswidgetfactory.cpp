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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "drugswidgetfactory.h"

#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/drugsmodel.h>
#include <drugsbaseplugin/drugsio.h>

#include <drugsplugin/constants.h>
#include <drugsplugin/drugswidget/druginfo.h>
#include <drugsplugin/drugspreferences/mfDrugsPreferences.h>
#include <drugsplugin/drugswidget/drugscentralwidget.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/translators.h>
#include <coreplugin/ipatient.h>

#include <formmanagerplugin/iformitem.h>

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


namespace {
    const char* const OPTION_HIDESELECTOR     = "hideselector";
    const char* const OPTION_WITHPRESCRIBING  = "withprescribing";
    const char* const OPTION_WITHPRINTING     = "withprinting";

    const char * const  EXTRAS_KEY              = "option";
    const char * const  EXTRAS_KEY2             = "options";
}

using namespace DrugsWidget;
using namespace Internal;

static inline DrugsDB::Internal::DrugsBase *drugsBase() {return DrugsDB::Internal::DrugsBase::instance();}
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}

inline static QStringList getOptions(Form::FormItem *item)
{
    QStringList l;
    l = item->extraDatas().value(::EXTRAS_KEY).split(";", QString::SkipEmptyParts);
    l += item->extraDatas().value(::EXTRAS_KEY2).split(";", QString::SkipEmptyParts);
    return l;
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
     return QStringList() << "drugs" << "prescription" << "prescriptor" << "drugselector";  /** \todo add more type of plugs ? */
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
        m_PrescriptionModel(0)
{
    // Prepare Widget Layout and label
    QBoxLayout * hb = getBoxLayout(Label_OnTop, m_FormItem->spec()->label(), this );
    hb->setSpacing(0);
    hb->setMargin(0);

    // Add QLabel
    hb->addWidget(m_Label);
    // create main widget
    m_CentralWidget = new DrugsCentralWidget(this);
    m_CentralWidget->initialize(formItem->extraDatas().value("options").contains(OPTION_HIDESELECTOR, Qt::CaseInsensitive));
    m_PrescriptionModel = m_CentralWidget->currentDrugsModel();
    hb->addWidget(m_CentralWidget);

    // Manage options
    const QStringList &options = getOptions(formItem);
    if (options.contains(OPTION_WITHPRESCRIBING, Qt::CaseInsensitive)) {
        m_WithPrescribing = true;
    } else if (name.compare("drugselector",Qt::CaseInsensitive)==0) {
        m_WithPrescribing = false;
    } else if (name.compare("prescription", Qt::CaseInsensitive)==0 ||
               name.compare("prescriptor", Qt::CaseInsensitive)==0) {
        m_WithPrescribing = true;
    }
    m_PrescriptionModel->setSelectionOnlyMode(!m_WithPrescribing);

    if (options.contains("nointeractionchecking", Qt::CaseInsensitive)) {
        m_PrescriptionModel->setComputeDrugInteractions(false);
    }

    // create formitemdata
    DrugsWidgetData *datas = new DrugsWidgetData(formItem);
    datas->setDrugsPrescriptorWidget(this);
    formItem->setItemDatas(datas);
}

DrugsPrescriptorWidget::~DrugsPrescriptorWidget()
{
}

QString DrugsPrescriptorWidget::printableHtml(bool withValues) const
{
    QString html = DrugsDB::DrugsIO().prescriptionToHtml(m_PrescriptionModel);
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

void DrugsPrescriptorWidget::retranslate()
{
    m_Label->setText(m_FormItem->spec()->label());
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
    m_Widget->m_PrescriptionModel->clearDrugsList();
}

bool DrugsWidgetData::isModified() const
{
    return m_Widget->m_PrescriptionModel->isModified();
}

bool DrugsWidgetData::setData(const int ref, const QVariant &data, const int role)
{
    return true;
}

QVariant DrugsWidgetData::data(const int ref, const int role) const
{
//    qWarning() << "DrugsWidgetData::data(" << ref << role << ");" << Core::IPatient::DrugsInnAllergies;

    if (role != Form::IFormItemData::ID_ForPatientModel)
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
    }  // End switch

    return QVariant();
}

void DrugsWidgetData::setStorableData(const QVariant &data)
{
    if (!data.isValid())
        return;
    DrugsDB::DrugsIO io;
    io.prescriptionFromXml(m_Widget->m_PrescriptionModel, data.toString(), DrugsDB::DrugsIO::ReplacePrescription);
    m_Widget->m_PrescriptionModel->setModified(false);
}

QVariant DrugsWidgetData::storableData() const
{
    DrugsDB::DrugsIO io;
    return io.prescriptionToXml(m_Widget->m_PrescriptionModel);
}
