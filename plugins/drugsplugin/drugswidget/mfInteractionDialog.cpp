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

/**
   \class InteractionDialog
   \brief Show a dialog that presents the synthesis of interactions.
   Result can be printed.
*/

#include "mfInteractionDialog.h"

#ifdef FREEDIAMS
#  include <fdcoreplugin/patient.h>
#endif

// include drugswidget headers
#include <drugsmodel/mfDrugsModel.h>
#include <mfDrugsConstants.h>
#include <mfDrugsManager.h>

// include toolkit headers
#include <coreplugin/constants.h>
#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>
#include <coreplugin/dialogs/helpdialog.h>

#include <printerplugin/printer.h>
#include <utils/log.h>
#include <utils/global.h>

using namespace mfInteractionsConstants;

using namespace Drugs::Internal;

InteractionDialog::InteractionDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    setObjectName("InteractionDialog");
    printButton->setIcon( Core::ICore::instance()->theme()->icon(Core::Constants::ICONPRINT) );
    zoomIn->setIcon( Core::ICore::instance()->theme()->icon(Core::Constants::ICONFONTBIGGER) );
    zoomOut->setIcon( Core::ICore::instance()->theme()->icon(Core::Constants::ICONFONTSMALLER) );
    setWindowTitle( tr("Synthetic interactions") + " - " + qApp->applicationName() );
    textBrowser->setHtml( DRUGMODEL->index(0, Interaction::FullSynthesis).data().toString() );
    m_Zoom = Core::ICore::instance()->settings()->value(mfDrugsConstants::MFDRUGS_SETTING_INTERACTIONVIEW_ZOOM,1).toInt();
    textBrowser->zoomIn(m_Zoom);
    qWarning() << Core::ICore::instance()->settings()->value(mfDrugsConstants::MFDRUGS_SETTING_INTERACTIONVIEW_ZOOM,1);
}

InteractionDialog::~InteractionDialog()
{
    Core::ISettings *s = Core::ICore::instance()->settings();
    s->setValue(mfDrugsConstants::MFDRUGS_SETTING_INTERACTIONVIEW_ZOOM, m_Zoom);
    s->sync();
}

void InteractionDialog::on_helpButton_clicked()
{
    Core::HelpDialog::showPage("iamtesteur.html#synthetiseur_iam");
}


void InteractionDialog::on_printButton_clicked()
{
    /** \todo add functionnality to FMF */
    Core::ISettings *s = Core::ICore::instance()->settings();
#ifdef FREEDIAMS
    Print::Printer p(this);
    p.askForPrinter(this);
    p.printWithDuplicata(false);
    QString header = s->value( MFDRUGS_SETTING_USERHEADER ).toString();
    Core::ICore::instance()->patient()->replaceTokens(header);
    Utils::replaceToken(header, Core::Constants::TOKEN_DATE, QDate::currentDate().toString( QLocale().dateFormat() ) );
    p.setHeader(header);
    header = s->value( MFDRUGS_SETTING_USERFOOTER ).toString();
    header.replace("</body>",QString("<br /><span style=\"align:left;font-size:6pt;color:black;\">%1</span></p></body>")
                   .arg(tr("Made with FreeDiams.")));
    p.setFooter(header);
    p.addTextWatermark(tr("Made with FreeDiams."), Print::Printer::EachPages, Qt::AlignCenter, Qt::AlignCenter,QFont(), QColor(200,200,200));
    p.print( textBrowser->toHtml() );
#endif
}

void InteractionDialog::on_zoomIn_clicked()
{
    /** \todo save zoom factor in settings */
    textBrowser->zoomIn(2);
    m_Zoom += 2;
}

void InteractionDialog::on_zoomOut_clicked()
{
    textBrowser->zoomOut(2);
    m_Zoom -= 2;
}

void InteractionDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}
