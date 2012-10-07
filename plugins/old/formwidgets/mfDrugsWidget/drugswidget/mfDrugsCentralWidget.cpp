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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "mfDrugsCentralWidget.h"

// include drugs widgets headers
#include <drugsdatabase/mfDrugsBase.h>
#include <drugsmodel/mfDrugsModel.h>
#include <drugsmodel/mfDrugsIO.h>
#include <drugswidget/mfDrugSelector.h>
#include <drugswidget/mfPrescriptionViewer.h>
#include <dosagedialog/mfDosageCreatorDialog.h>
#include <dosagedialog/mfDosageDialog.h>
#include <mfDrugsManager.h>

// include toolkit headers
#include <tkGlobal.h>
#include <tkLog.h>
#include <tkSettings.h>
#include <tkTheme.h>
#include <tkConstantTranslations.h>
#include <tkContextManager.h>
#include <tkPrinter.h>

#ifdef DRUGS_INTERACTIONS_STANDALONE
#    include <diCore.h>
#endif


/** \brief Constructor */
mfDrugsCentralWidget::mfDrugsCentralWidget(QWidget *parent) :
    QWidget(parent), m_CurrentDrugModel(0)
{
    // create instance of mfDrugsManager
    mfDrugsManager::instance();
}

/** \brief Initialize the widget after the ui was setted */
bool mfDrugsCentralWidget::initialize()
{
    setupUi(this);

    // create context
    m_Context = new mfDrugsContext(this);
    m_Context->setContext( QList<int>() << tkUID->uniqueIdentifier(mfDrugsConstants::C_DRUGS_PLUGINS) );
    tkContextManager::instance()->addContextObject(m_Context);

    // create model view for selected drugs list
    m_CurrentDrugModel = new mfDrugsModel(this);
    m_PrescriptionView->initialize();
    m_PrescriptionView->setModel( m_CurrentDrugModel );
    m_PrescriptionView->setModelColumn( Drug::FullPrescription );

    m_DrugSelector->initialize();

    m_DrugSelector->setFocus();
    return true;
}

QListView *mfDrugsCentralWidget::prescriptionListView()
{
    return m_PrescriptionView->listview();
}

mfPrescriptionViewer *mfDrugsCentralWidget::prescriptionView()
{
    return m_PrescriptionView;
}

mfDrugsModel *mfDrugsCentralWidget::currentDrugsModel() const
{
    return m_CurrentDrugModel;
}

void mfDrugsCentralWidget::setCurrentSearchMethod(int method)
{
    m_DrugSelector->setSearchMethod(method);
}

/** \brief Creates connections */
void mfDrugsCentralWidget::createConnections()
{
    connect(m_DrugSelector, SIGNAL(drugSelected(int)), this, SLOT( selector_drugSelected(const int) ) );
    connect( prescriptionListView(), SIGNAL(activated(const QModelIndex &)),
             m_PrescriptionView, SLOT(showDosageDialog(const QModelIndex&)) );
}

void mfDrugsCentralWidget::disconnect()
{
    prescriptionListView()->disconnect( prescriptionListView(), SIGNAL(activated(const QModelIndex &)),
             m_PrescriptionView, SLOT(showDosageDialog(const QModelIndex&)) );
    m_DrugSelector->disconnect(m_DrugSelector, SIGNAL(drugSelected(int)), this, SLOT( selector_drugSelected(const int) ) );
}

void mfDrugsCentralWidget::focusInEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    m_DrugSelector->setFocus();
}

/**
  \brief Slot called when is selected from the drugSelector.
  Verify that the drug isn't already prescribed (if it is warn user and stop). \n
  Add the drug to the mfDrugsModel and open the mfDosageCreatorDialog\n
*/
void mfDrugsCentralWidget::selector_drugSelected( const int CIS )
{
    // if exists dosage for that drug show the dosageSelector widget
    // else show the dosage creator widget
    if (m_CurrentDrugModel->containsDrug(CIS)) {
        tkGlobal::warningMessageBox(tr("Can not add this drug to your prescription."),
                                    tr("Prescription can not contains twice the sample pharmaceutical drug.\n"
                                       "Drug %1 is already in your prescription").arg(m_CurrentDrugModel->drugData(CIS,Drug::Denomination).toString()),
                                    tr("If you want to change the dosage of this drug please double-click on it in the prescription box."));
        return;
    }
    int drugPrescriptionRow = m_CurrentDrugModel->addDrug(CIS);
    mfDosageCreatorDialog dlg(this, m_CurrentDrugModel->dosageModel(CIS));
    if (dlg.exec()==QDialog::Rejected) {
        m_CurrentDrugModel->removeLastInsertedDrug();
    }
    m_PrescriptionView->listview()->update();
}

/** \brief Change the font of the viewing widget */
void mfDrugsCentralWidget::changeFontTo( const QFont &font )
{
    m_DrugSelector->setFont(font);
    m_PrescriptionView->listview()->setFont(font);
}

bool mfDrugsCentralWidget::printPrescription()
{
    tkPrinter p(this);
    if (!p.askForPrinter(this))
        return false;
#ifdef DRUGS_INTERACTIONS_STANDALONE
    QString header = diCore::settings()->value( MFDRUGS_SETTING_USERHEADER ).toString();
    diCore::patient()->replaceTokens(header);
    tkGlobal::replaceToken(header, TOKEN_DATE, QDate::currentDate().toString( QLocale().dateFormat() ) );
    QString footer = diCore::settings()->value( MFDRUGS_SETTING_USERFOOTER ).toString();
    footer.replace("</body>",QString("<br /><span style=\"align:left;font-size:6pt;color:black;\">%1</span></p></body>")
                   .arg(tr("Made with FreeDiams.")));
    p.addHtmlWatermark( diCore::settings()->value( MFDRUGS_SETTING_WATERMARK_HTML ).toString(),
                        tkPrinter::Presence(diCore::settings()->value( MFDRUGS_SETTING_WATERMARKPRESENCE ).toInt()),
                        Qt::AlignmentFlag(diCore::settings()->value( MFDRUGS_SETTING_WATERMARKALIGNEMENT ).toInt()));
#else
    QString header = "Work in progress";
//    diCore::patient()->replaceTokens(header);
//    tkGlobal::replaceToken(header, TOKEN_DATE, QDate::currentDate().toString( QLocale().dateFormat() ) );
    QString footer = "Work in progress";
    footer.replace("</body>",QString("<br /><span style=\"align:left;font-size:6pt;color:black;\">%1</span></p></body>")
                   .arg(tr("Made with FreeMedForms.")));
#endif
    p.setHeader( header );
    p.setFooter( footer );
    p.printWithDuplicata(true);
    return p.print( mfDrugsIO::instance()->prescriptionToHtml() );
}
