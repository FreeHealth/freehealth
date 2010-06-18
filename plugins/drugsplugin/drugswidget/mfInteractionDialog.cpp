/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
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

#include <coreplugin/ipatient.h>
#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>

#include <drugsbaseplugin/drugsmodel.h>
#include <drugsbaseplugin/constants.h>

#include <drugsplugin/constants.h>
#include <drugsplugin/drugswidgetmanager.h>

#include <coreplugin/constants_icons.h>
#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>
#include <coreplugin/dialogs/helpdialog.h>

#include <printerplugin/printer.h>

#include <utils/log.h>
#include <utils/global.h>

using namespace DrugsWidget;
using namespace DrugsWidget::Internal;

inline static DrugsDB::DrugsModel *drugModel() { return DrugsWidget::DrugsWidgetManager::instance()->currentDrugsModel(); }
inline static Core::ITheme *theme() {return Core::ICore::instance()->theme();}
inline static Core::ISettings *settings() {return Core::ICore::instance()->settings();}
inline static Core::IUser *user() {return Core::ICore::instance()->user();}

InteractionDialog::InteractionDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    setObjectName("InteractionDialog");
    printButton->setIcon( theme()->icon(Core::Constants::ICONPRINT) );
    zoomIn->setIcon( theme()->icon(Core::Constants::ICONFONTBIGGER) );
    zoomOut->setIcon( theme()->icon(Core::Constants::ICONFONTSMALLER) );
    setWindowTitle( tr("Synthetic interactions") + " - " + qApp->applicationName() );
    textBrowser->setHtml( drugModel()->index(0, DrugsDB::Constants::Interaction::FullSynthesis).data().toString() );
    m_Zoom = settings()->value(Constants::S_INTERACTIONVIEW_ZOOM,1).toInt();
    textBrowser->zoomIn(m_Zoom);
}

InteractionDialog::~InteractionDialog()
{
    settings()->setValue(Constants::S_INTERACTIONVIEW_ZOOM, m_Zoom);
    settings()->sync();
}

void InteractionDialog::on_helpButton_clicked()
{
    Core::HelpDialog::showPage("iamtesteur.html#synthetiseur_iam");
}


void InteractionDialog::on_printButton_clicked()
{
    /** \todo add functionnality to FMF */
    Core::ISettings *s = settings();
    Print::Printer p(this);
    p.askForPrinter(this);
    p.printWithDuplicata(false);

    QString paper = user()->value(Core::IUser::PrescriptionHeader).toString();
    user()->replaceTokens(paper);
    Core::ICore::instance()->patient()->replaceTokens(paper);
//    Utils::replaceToken(paper, Core::Constants::TOKEN_DATE, QDate::currentDate().toString(QLocale().dateFormat()));
    p.setHeader(paper);

    paper = user()->value(Core::IUser::PrescriptionFooter).toString();
    paper.replace("</body>",QString("<br /><span style=\"align:left;font-size:6pt;color:black;\">%1</span></p></body>")
                   .arg(tr("Made with %1.").arg(qApp->applicationName())));
    p.setFooter(paper);

    p.addTextWatermark(tr("Made with %1.").arg(qApp->applicationName()), Print::Printer::EachPages, Qt::AlignCenter, Qt::AlignCenter,QFont(), QColor(200,200,200));
    p.print(textBrowser->toHtml());
}

void InteractionDialog::on_zoomIn_clicked()
{
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
