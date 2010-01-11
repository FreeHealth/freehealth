/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
 *   eric.maeker@free.fr                                                   *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "mfDrugsCentralWidget.h"

// include drugs widgets headers
#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/drugsmodel.h>
#include <drugsbaseplugin/drugsio.h>

#include <drugsplugin/drugswidget/mfDrugSelector.h>
#include <drugsplugin/drugswidget/mfPrescriptionViewer.h>
#include <drugsplugin/dosagedialog/mfDosageCreatorDialog.h>
#include <drugsplugin/dosagedialog/mfDosageDialog.h>
#include <drugsplugin/drugswidgetmanager.h>
#include <drugsplugin/constants.h>

#include <utils/global.h>
#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/constants.h>
#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/uniqueidmanager.h>
#include <coreplugin/contextmanager/contextmanager.h>

#include <printerplugin/printer.h>

#include <templatesplugin/templatescreationdialog.h>

#ifdef FREEDIAMS
#  include <fdcoreplugin/patient.h>
#endif

#include "ui_mfDrugsCentralWidget.h"

using namespace DrugsWidget;

inline static DrugsDB::DrugsModel *drugModel() { return DrugsWidget::DrugsWidgetManager::instance()->currentDrugsModel(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

/** \brief Constructor */
DrugsCentralWidget::DrugsCentralWidget(QWidget *parent) :
    QWidget(parent), m_CurrentDrugModel(0)
{
    // create instance of DrugsManager
    DrugsWidgetManager::instance();
}

/** \brief Initialize the widget after the ui was setted */
bool DrugsCentralWidget::initialize()
{
    m_ui = new DrugsWidget::Internal::Ui::DrugsCentralWidget();
    m_ui->setupUi(this);

    // create context
    m_Context = new Internal::DrugsContext(this);
    m_Context->setContext( QList<int>() << Core::ICore::instance()->uniqueIDManager()->uniqueIdentifier(Constants::C_DRUGS_PLUGINS));
    Core::ICore::instance()->contextManager()->addContextObject(m_Context);

    // create model view for selected drugs list
    m_CurrentDrugModel = new DrugsDB::DrugsModel(this);
    m_ui->m_PrescriptionView->initialize();
    m_ui->m_PrescriptionView->setModel(m_CurrentDrugModel);
    m_ui->m_PrescriptionView->setModelColumn( DrugsDB::Constants::Drug::FullPrescription );

    m_ui->m_DrugSelector->initialize();

    m_ui->m_DrugSelector->setFocus();

    DrugsWidgetManager::instance()->setCurrentView(this);

    changeFontTo( QFont(settings()->value(Constants::S_VIEWFONT).toString(), settings()->value(Constants::S_VIEWFONTSIZE).toInt()) );

    return true;
}

QListView *DrugsCentralWidget::prescriptionListView()
{
    return m_ui->m_PrescriptionView->listview();
}

PrescriptionViewer *DrugsCentralWidget::prescriptionView()
{
    return m_ui->m_PrescriptionView;
}

DrugsDB::DrugsModel *DrugsCentralWidget::currentDrugsModel() const
{
    return m_CurrentDrugModel;
}

void DrugsCentralWidget::setCurrentSearchMethod(int method)
{
    m_ui->m_DrugSelector->setSearchMethod(method);
}

/** \brief Creates connections */
void DrugsCentralWidget::createConnections()
{
    connect(m_ui->m_DrugSelector, SIGNAL(drugSelected(int)), this, SLOT( selector_drugSelected(const int) ) );
    connect(prescriptionListView(), SIGNAL(activated(const QModelIndex &)),
            m_ui->m_PrescriptionView, SLOT(showDosageDialog(const QModelIndex&)) );
}

void DrugsCentralWidget::disconnect()
{
    prescriptionListView()->disconnect( prescriptionListView(), SIGNAL(activated(const QModelIndex &)),
             m_ui->m_PrescriptionView, SLOT(showDosageDialog(const QModelIndex&)) );
    m_ui->m_DrugSelector->disconnect(m_ui->m_DrugSelector, SIGNAL(drugSelected(int)),
                                     this, SLOT( selector_drugSelected(const int) ) );
}

void DrugsCentralWidget::focusInEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    m_ui->m_DrugSelector->setFocus();
}

/**
  \brief Slot called when is selected from the drugSelector.
  Verify that the drug isn't already prescribed (if it is warn user and stop). \n
  Add the drug to the mfDrugsModel and open the mfDosageCreatorDialog\n
*/
void DrugsCentralWidget::selector_drugSelected( const int CIS )
{
    // if exists dosage for that drug show the dosageSelector widget
    // else show the dosage creator widget
    if (m_CurrentDrugModel->containsDrug(CIS)) {
        Utils::warningMessageBox(tr("Can not add this drug to your prescription."),
                                    tr("Prescription can not contains twice the sample pharmaceutical drug.\n"
                                       "Drug %1 is already in your prescription")
                                    .arg(m_CurrentDrugModel->drugData(CIS, DrugsDB::Constants::Drug::Denomination).toString()),
                                    tr("If you want to change the dosage of this drug please double-click on it in the prescription box."));
        return;
    }
//    int drugPrescriptionRow = m_CurrentDrugModel->addDrug(CIS);
    m_CurrentDrugModel->addDrug(CIS);
    Internal::DosageCreatorDialog dlg(this, m_CurrentDrugModel->dosageModel(CIS));
    if (dlg.exec()==QDialog::Rejected) {
        m_CurrentDrugModel->removeLastInsertedDrug();
    }
    m_ui->m_PrescriptionView->listview()->update();
}

/** \brief Change the font of the viewing widget */
void DrugsCentralWidget::changeFontTo( const QFont &font )
{
    m_ui->m_DrugSelector->setFont(font);
    m_ui->m_PrescriptionView->listview()->setFont(font);
}

bool DrugsCentralWidget::printPrescription()
{
    Print::Printer p(this);
    if (!p.askForPrinter(this))
        return false;
#ifdef FREEDIAMS
    QString header = settings()->value( Constants::S_USERHEADER ).toString();
    Core::ICore::instance()->patient()->replaceTokens(header);
    Utils::replaceToken(header, Core::Constants::TOKEN_DATE, QDate::currentDate().toString( QLocale().dateFormat() ) );
    QString footer = settings()->value( Constants::S_USERFOOTER ).toString();
    footer.replace("</body>",QString("<br /><span style=\"align:left;font-size:6pt;color:black;\">%1</span></p></body>")
                   .arg(tr("Made with FreeDiams.")));
    Utils::replaceToken(footer, Core::Constants::TOKEN_NUMBEROFDRUGS, QString::number(drugModel()->rowCount()) );
    p.addHtmlWatermark( settings()->value( Constants::S_WATERMARK_HTML ).toString(),
                        Print::Printer::Presence(settings()->value( Constants::S_WATERMARKPRESENCE ).toInt()),
                        Qt::AlignmentFlag(settings()->value( Constants::S_WATERMARKALIGNEMENT ).toInt()));
#else
    /** \todo FreeMedForms prescription printing */
    QString header = "Work in progress";
//    diCore::patient()->replaceTokens(header);
//    Utils::replaceToken(header, Core::Constants::TOKEN_DATE, QDate::currentDate().toString( QLocale().dateFormat() ) );
    QString footer = "Work in progress";
    footer.replace("</body>",QString("<br /><span style=\"align:left;font-size:6pt;color:black;\">%1</span></p></body>")
                   .arg(tr("Made with FreeMedForms.")));
#endif
    p.setHeader(header);
    p.setFooter(footer);
    p.printWithDuplicata(settings()->value(Constants::S_PRINTDUPLICATAS).toBool());
    return p.print( DrugsDB::DrugsIO::instance()->prescriptionToHtml(m_CurrentDrugModel) );
}

bool DrugsCentralWidget::createTemplate()
{
    // get the template content
    QString content = DrugsDB::DrugsIO::prescriptionToXml(m_CurrentDrugModel);
//    qWarning() << content;
    // create a new template with it
    Templates::TemplatesCreationDialog dlg(this);
    dlg.setTemplateContent(content);
    dlg.setTemplateSummary(DrugsDB::DrugsIO::prescriptionToHtml(m_CurrentDrugModel));
    dlg.setTemplateMimeTypes(m_CurrentDrugModel->mimeTypes());
    dlg.exec();
    return true;
}
