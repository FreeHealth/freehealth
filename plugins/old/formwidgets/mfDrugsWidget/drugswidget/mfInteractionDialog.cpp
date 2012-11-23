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

/**
   \class mfInteractionDialog
   \brief Show a dialog that presents the synthesis of interactions.
   Result can be printed.
*/

#include "mfInteractionDialog.h"

#ifdef DRUGS_INTERACTIONS_STANDALONE
#include <diCore.h>
#include "diPatient.h"
#endif

// include drugswidget headers
#include <drugsmodel/mfDrugsModel.h>
#include <mfDrugsConstants.h>
#include <mfDrugsManager.h>

// include toolkit headers
#include <tkTheme.h>
#include <tkSettings.h>
#include <tkPrinter.h>
#include <tkLog.h>
#include <tkHelpDialog.h>

using namespace mfInteractionsConstants;
Q_TK_USING_CONSTANTS

mfInteractionDialog::mfInteractionDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    setObjectName("mfInteractionDialog");
    printButton->setIcon( tkTheme::icon(ICONPRINT) );
    setWindowTitle( tr("Synthetic interactions") + " - " + qApp->applicationName() );
    textBrowser->setHtml( DRUGMODEL->index(0, Interaction::FullSynthesis).data().toString() );
//    textBrowser->setHtml( mfDrugsModel::instance()->prescriptionToHtml() );
}

void mfInteractionDialog::on_helpButton_clicked()
{
    tkHelpDialog::showPage("iamtesteur.html#synthetiseur_iam");
}


void mfInteractionDialog::on_printButton_clicked()
{
    // TODO: add functionnality to FMF */
#ifdef DRUGS_INTERACTIONS_STANDALONE
    tkPrinter p(this);
    p.askForPrinter(this);
    p.printWithDuplicata(false);
    QString header = tkSettings::instance()->value( MFDRUGS_SETTING_USERHEADER ).toString();
    diCore::patient()->replaceTokens(header);
    tkGlobal::replaceToken(header, TOKEN_DATE, QDate::currentDate().toString( QLocale().dateFormat() ) );
    p.setHeader( header );
    header = diCore::settings()->value( MFDRUGS_SETTING_USERFOOTER ).toString();
    header.replace("</body>",QString("<br /><span style=\"align:left;font-size:6pt;color:black;\">%1</span></p></body>")
                   .arg(tr("Made with FreeDiams.")));
    p.setFooter( header );
    p.addTextWatermark(tr("Made with FreeDiams."), tkPrinter::EachPages, Qt::AlignCenter, Qt::AlignCenter,QFont(), QColor(200,200,200));
    p.print( textBrowser->toHtml() );
#endif
}

void mfInteractionDialog::changeEvent(QEvent *e)
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
