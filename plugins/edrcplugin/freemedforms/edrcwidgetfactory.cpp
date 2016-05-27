/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2016 by Eric MAEKER, MD                                     **
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "edrcwidgetfactory.h"
#include <edrcplugin/widgets/crlistviewer.h>
#include <edrcplugin/models/crtreemodel.h>
#include <edrcplugin/consultresult.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ipatient.h>

#include <formmanagerplugin/iformitem.h>

#include <utils/global.h>
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
const char* const OPTION_WITHPRINTING     = "withprinting";
const char* const DONTPRINTEMPTYVALUES    = "DontPrintEmptyValues";
}

using namespace eDRC;
using namespace Internal;

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}

static inline bool dontPrintEmptyValues(Form::FormItem *item)
{
    return item->getOptions().contains(::DONTPRINTEMPTYVALUES, Qt::CaseInsensitive);
}

//---------------------------------------------------------------------------------------
//-------------------------  EdrcWidgetFactory  -----------------------------------------
//---------------------------------------------------------------------------------------
EdrcWidgetFactory::EdrcWidgetFactory(QObject *parent) :
        Form::IFormWidgetFactory(parent)
{
}

EdrcWidgetFactory::~EdrcWidgetFactory()
{
}

bool EdrcWidgetFactory::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);
    return true;
}

bool EdrcWidgetFactory::extensionInitialized()
{
    return true;
}


QStringList EdrcWidgetFactory::providedWidgets() const
{
     return QStringList() << "edrc";
}

bool EdrcWidgetFactory::isContainer(const int) const
{
    return false;
}

Form::IFormWidget *EdrcWidgetFactory::createWidget(const QString &name, Form::FormItem *formItem, QWidget *parent)
{
    return new EdrcWidget(name, formItem, parent);
}

//---------------------------------------------------------------------------------------
//--------------------------   EdrcWidgetData implementation   --------------------------
//---------------------------------------------------------------------------------------
EdrcWidget::EdrcWidget(const QString &name, Form::FormItem *formItem, QWidget *parent) :
    Form::IFormWidget(formItem,parent),
    m_CentralWidget(0),
    m_CrTreeModel(0)
{
    Q_UNUSED(name); // TODO: check why name is not used
    // Prepare Widget Layout and label
    QWidget *labelWidget = new QWidget(this);
    QBoxLayout *labelBox = getBoxLayout(OnLeft, m_FormItem->spec()->label(), labelWidget);
    labelBox->setSpacing(0);
    labelBox->setMargin(0);
    labelBox->addWidget(m_Label);

    // Label always on top...
    QVBoxLayout *hb = new QVBoxLayout(this);
    hb->setSpacing(0);
    hb->setMargin(0);
    hb->addWidget(labelWidget);

    // create main widget
    m_CentralWidget = new CrListViewer(this);
    m_CentralWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_CrTreeModel = new CrTreeModel(this);
    m_CentralWidget->setConsultResultTreeModel(m_CrTreeModel);

    hb->addWidget(m_CentralWidget);

    // Manage options
//    const QStringList &options = formItem->getOptions();
//    if (options.contains(OPTION_WITHPRESCRIBING, Qt::CaseInsensitive)) {
//        m_WithPrescribing = true;
//    } else if (name.compare("drugselector",Qt::CaseInsensitive)==0) {
//        m_WithPrescribing = false;
//    } else if (name.compare("prescription", Qt::CaseInsensitive)==0 ||
//               name.compare("prescriptor", Qt::CaseInsensitive)==0) {
//        m_WithPrescribing = true;
//    }
//    m_PrescriptionModel->setSelectionOnlyMode(!m_WithPrescribing);

    // create formitemdata
    EdrcWidgetData *data = new EdrcWidgetData(formItem);
    data->setEdrcWidget(this);
    formItem->setItemData(data);
}

EdrcWidget::~EdrcWidget()
{
}

QString EdrcWidget::printableHtml(bool withValues) const
{
    if (withValues && dontPrintEmptyValues(m_FormItem) && m_CrTreeModel->rowCount()==0) {
        return QString();
    }
    QString html;
//    = Utils::htmlBodyContent(prescriptionPrinter().prescriptionToHtml(m_PrescriptionModel));
    html = Utils::htmlRemoveLinkTags(html);
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

void EdrcWidget::retranslate()
{
    m_Label->setText(m_FormItem->spec()->label());
}

////////////////////////////////////////// ItemData /////////////////////////////////////////////
EdrcWidgetData::EdrcWidgetData(Form::FormItem *item) :
        Form::IFormItemData(), m_FormItem(item), m_Widget(0)
{}

EdrcWidgetData::~EdrcWidgetData()
{}

void EdrcWidgetData::setEdrcWidget(EdrcWidget *widget)
{
    m_Widget = widget;
    clear();
}

void EdrcWidgetData::clear()
{
    m_Widget->m_CentralWidget->clear();
}

bool EdrcWidgetData::isModified() const
{
    // TODO: here
    return true;
//    return m_Widget->m_CrTreeModel->isModified();
}

void EdrcWidgetData::setModified(bool modified)
{
    Q_UNUSED(modified);
    // TODO: here
    return;
//    m_Widget->m_PrescriptionModel->setModified(modified);
}

void EdrcWidgetData::setReadOnly(bool readOnly)
{
    Q_UNUSED(readOnly);
    // TODO: here
    return;
//    m_Widget->m_CentralWidget->setEnabled(!readOnly);
}

bool EdrcWidgetData::isReadOnly() const
{
    // TODO: here
    return false;
//    return (!m_Widget->m_CentralWidget->isEnabled());
}

bool EdrcWidgetData::setData(const int ref, const QVariant &data, const int role)
{
    Q_UNUSED(ref);
    Q_UNUSED(data);
    Q_UNUSED(role);
    return true;
}

QVariant EdrcWidgetData::data(const int ref, const int role) const
{
    Q_UNUSED(ref); // TODO: check why ref is not used
//    qWarning() << "EdrcWidgetData::data(" << ref << role << ");" << Core::IPatient::DrugsInnAllergies;

    if (role == Form::IFormItemData::PrintRole) {
        // TODO
        return QVariant();
    }

    if (role != Form::IFormItemData::PatientModelRole)
        return QVariant();

    return QVariant();
}

void EdrcWidgetData::setStorableData(const QVariant &data)
{
    m_Widget->m_CrTreeModel->setCrList(ConsultResult::fromXml(data.toString()));
//    m_Widget->m_CentralWidget->clear();
//    m_Widget->m_PrescriptionModel->setModified(false);
//    if (!data.isValid())
//        return;
//    drugsIo().prescriptionFromXml(m_Widget->m_PrescriptionModel, data.toString(), DrugsDB::DrugsIO::ReplacePrescription);
//    m_Widget->m_PrescriptionModel->setModified(false);
}

QVariant EdrcWidgetData::storableData() const
{
    return ConsultResult::listToXml(m_Widget->m_CrTreeModel->consultResultList());
}
