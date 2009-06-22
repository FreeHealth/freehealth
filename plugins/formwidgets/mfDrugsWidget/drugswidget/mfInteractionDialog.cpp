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
#include "mfInteractionDialog.h"

#ifdef DRUGS_INTERACTIONS_STANDALONE
#include <diCore.h>
#endif

#include <drugsmodel/mfDrugsModel.h>
#include <mfDrugsConstants.h>

#include <tkTheme.h>
#include <tkSettings.h>
#include <tkPrinter.h>
#include <tkLog.h>

using namespace mfInteractionsConstants;
Q_TK_USING_CONSTANTS

mfInteractionDialog::mfInteractionDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    setObjectName("mfInteractionDialog");
    printButton->setIcon( tkTheme::icon(ICONPRINT) );
    setWindowTitle( tr("Synthetic interactions") + " - " + qApp->applicationName() );
    textBrowser->setHtml( mfDrugsModel::instance()->index(0, Interaction::FullSynthesis).data().toString() );
//    textBrowser->setHtml( mfDrugsModel::instance()->prescriptionToHtml() );
}

void mfInteractionDialog::on_printButton_clicked()
{
#ifdef DRUGS_INTERACTIONS_STANDALONE
    tkPrinter p(this);
    p.askForPrinter(this);
    p.printWithDuplicata(false);
    QString header = tkSettings::instance()->value( MFDRUGS_SETTING_USERHEADER ).toString();
    tkGlobal::replaceToken(header, TOKEN_PATIENTNAME, diCore::patientName() );
    tkGlobal::replaceToken(header, TOKEN_DATE, QDate::currentDate().toString( QLocale().dateFormat() ) );
    tkGlobal::replaceToken(header, TOKEN_WEIGHT, diCore::patientWeight() );
    tkGlobal::replaceToken(header, TOKEN_SIZE, diCore::patientSize() );
    tkGlobal::replaceToken(header, TOKEN_DATEOFBIRTH, diCore::patientDateOfBirth() );
    tkGlobal::replaceToken(header, TOKEN_CLCR, diCore::patientClCr() );
//    p.setHeader( header , tkPrinter::FirstPageOnly );
    p.setHeader( header );
    p.setFooter( diCore::settings()->value( MFDRUGS_SETTING_USERFOOTER ).toString() );
//    p.addHtmlWatermark( diCore::settings()->value( MFDRUGS_SETTING_WATERMARK_HTML ).toString(),
//                        tkPrinter::Presence(diCore::settings()->value( MFDRUGS_SETTING_WATERMARKPRESENCE ).toInt()),
//                        tkPrinter::EachPages,
//                       Qt::AlignmentFlag(diCore::settings()->value( MFDRUGS_SETTING_WATERMARKALIGNEMENT ).toInt()));
//                        Qt::AlignCenter );
//    p.setContent( mfDrugsModel::instance()->prescriptionToHtml() );
//    p.previewDialog(this);
//    p.printWithDuplicata(true);
//    qWarning() << mfDrugsModel::instance()->prescriptionToHtml();
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
